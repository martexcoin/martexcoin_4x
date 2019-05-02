// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Copyright (c) 2014-2019 The MarteX Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "miner.h"

#include "amount.h"
#include "base58.h"
#include "chain.h"
#include "chainparams.h"
#include "coins.h"
#include "consensus/consensus.h"
#include "consensus/merkle.h"
#include "consensus/validation.h"
#include "governance-classes.h"
#include "hash.h"
#include "kernel.h"
#include "net.h"
#include "policy/policy.h"
#include "pow.h"
#include "primitives/transaction.h"
#include "script/standard.h"
#include "timedata.h"
#include "txmempool.h"
#include "util.h"
#include "utilmoneystr.h"
#include "masternode-payments.h"
#include "masternode-sync.h"
#include "validation.h"
#include "validationinterface.h"
#include "wallet/wallet.h"

#include <algorithm>
#include <boost/thread.hpp>
#include <boost/tuple/tuple.hpp>
#include <queue>
#include <utility>

//////////////////////////////////////////////////////////////////////////////
//
// MarteXMiner
//

//
// Unconfirmed transactions in the memory pool often depend on other
// transactions in the memory pool. When we select transactions from the
// pool, we select by highest priority or fee rate, so we might consider
// transactions that depend on transactions that aren't yet in the block.

uint64_t nLastBlockTx = 0;
uint64_t nLastBlockSize = 0;
int64_t nLastCoinStakeSearchInterval = 0;

extern unsigned int nMinerSleep;

class ScoreCompare
{
public:
    ScoreCompare() {}

    bool operator()(const CTxMemPool::txiter a, const CTxMemPool::txiter b)
    {
        return CompareTxMemPoolEntryByScore()(*b,*a); // Convert to less than
    }
};

int64_t UpdateTime(CBlockHeader* pblock, const Consensus::Params& consensusParams, const CBlockIndex* pindexPrev)
{
    int64_t nOldTime = pblock->nTime;
    int64_t nNewTime = std::max(pindexPrev->GetMedianTimePast()+1, GetAdjustedTime());

    if (nOldTime < nNewTime)
        pblock->nTime = nNewTime;

    // Updating time can change work required on testnet:
    if (consensusParams.fPowAllowMinDifficultyBlocks)
        pblock->nBits = GetNextWorkRequired(pindexPrev, pblock, consensusParams);

    return nNewTime - nOldTime;
}

BlockAssembler::BlockAssembler(const CChainParams& _chainparams)
    : chainparams(_chainparams)
{
    // Largest block you're willing to create:
    nBlockMaxSize = GetArg("-blockmaxsize", DEFAULT_BLOCK_MAX_SIZE);

    if (IsArgSet("-blockmintxfee")) {
        CAmount n = 0;
        ParseMoney(GetArg("-blockmintxfee", ""), n);
        blockMinFeeRate = CFeeRate(n);
    } else {
        blockMinFeeRate = CFeeRate(DEFAULT_BLOCK_MIN_TX_FEE);
    }

    // Limit to between 1K and MAX_BLOCK_SIZE-1K for sanity:
    nBlockMaxSize = std::max((unsigned int)1000, std::min((unsigned int)(MaxBlockSize(fDIP0001ActiveAtTip)-1000), nBlockMaxSize));
}

void BlockAssembler::resetBlock()
{
    inBlock.clear();

    // Reserve space for coinbase tx
    nBlockSize = 1000;
    nBlockSigOps = 100;

    // These counters do not include coinbase tx
    nBlockTx = 0;
    nFees = 0;

    lastFewTxs = 0;
    blockFinished = false;
}

std::unique_ptr<CBlockTemplate> BlockAssembler::CreateNewBlock(const CScript& scriptPubKeyIn, bool fProofOfStake)
{
    int64_t nTimeStart = GetTimeMicros();

    resetBlock();

    pblocktemplate.reset(new CBlockTemplate());

    if(!pblocktemplate.get())
        return nullptr;
    pblock = &pblocktemplate->block; // pointer for convenience

    // Add dummy coinbase tx as first transaction
    pblock->vtx.emplace_back();
    pblocktemplate->vTxFees.push_back(-1); // updated at end
    pblocktemplate->vTxSigOps.push_back(-1); // updated at end

    LOCK2(cs_main, mempool.cs);
    CBlockIndex* pindexPrev = chainActive.Tip();
    nHeight = pindexPrev->nHeight + 1;

    //pblock->nVersion = ComputeBlockVersion(pindexPrev, chainparams.GetConsensus());
    // -regtest only: allow overriding block.nVersion with
    // -blockversion=N to test forking scenarios
    if (chainparams.MineBlocksOnDemand())
        pblock->nVersion = GetArg("-blockversion", pblock->nVersion);

    pblock->nTime = GetAdjustedTime();
    const int64_t nMedianTimePast = pindexPrev->GetMedianTimePast();

    nLockTimeCutoff = (STANDARD_LOCKTIME_VERIFY_FLAGS & LOCKTIME_MEDIAN_TIME_PAST)
                       ? nMedianTimePast
                       : pblock->GetBlockTime();

    // Create coinbase transaction.
    CMutableTransaction coinbaseTx;
    coinbaseTx.vin.resize(1);
    coinbaseTx.vin[0].prevout.SetNull();
    coinbaseTx.vout.resize(1);

    addPriorityTxs();

    int nPackagesSelected = 0;
    int nDescendantsUpdated = 0;
    addPackageTxs(nPackagesSelected, nDescendantsUpdated);

    int64_t nTime1 = GetTimeMicros();

    if (!fProofOfStake)
    {
      coinbaseTx.vout[0].scriptPubKey = scriptPubKeyIn;

      // NOTE: unlike in bitcoin, we need to pass PREVIOUS block height here
      CAmount blockReward = nFees + GetBlockSubsidy(pindexPrev->nBits, pindexPrev->nHeight, Params().GetConsensus());

      // Compute regular coinbase transaction.
      coinbaseTx.vout[0].nValue = blockReward;
      coinbaseTx.vin[0].scriptSig = CScript() << nHeight << OP_0;

      // Update coinbase transaction with additional info about masternode and governance payments,
      // get some info back to pass to getblocktemplate
      FillBlockPayments(coinbaseTx, nHeight, blockReward, pblock->txoutMasternode, pblock->voutSuperblock);
      // LogPrintf("CreateNewBlock -- nBlockHeight %d blockReward %lld txoutMasternode %s coinbaseTx %s",
      //             nHeight, blockReward, pblock->txoutMasternode.ToString(), coinbaseTx.ToString());
      // Foundation Payments
			// int paymentsF = 1;
			// // start foundation payments
			// bool bFoundationPayment = false;
      //
			// if ( Params().NetworkIDString() == CBaseChainParams::TESTNET ){
			// 	if (GetTime() > START_FOUNDATION_PAYMENTS_TESTNET ){
			// 		bFoundationPayment = true;
			// 	}
			// }else{
			// 	if (GetTime() > START_FOUNDATION_PAYMENTS){
			// 		bFoundationPayment = true;
			// 	}
			// }
      //
			// bool hasfoundationPay = true;
      // CScript payeeF;
			// if(bFoundationPayment) {
			// 	// define address
			// 	CBitcoinAddress foundationaddress;
			// 	foundationaddress = CBitcoinAddress(Params().NetworkIDString() == CBaseChainParams::TESTNET ? FOUNDATION_T: FOUNDATION_M);
			// 	payeeF = GetScriptForDestination(foundationaddress.Get());
			// }else{
			// 	hasfoundationPay = false;
			// }
      //
			// if(hasfoundationPay){
			// 	paymentsF = coinbaseTx.vout.size() + 1;
			// 	coinbaseTx.vout.resize(paymentsF);
      //
			// 	coinbaseTx.vout[paymentsF-1].scriptPubKey = payeeF;
			// 	coinbaseTx.vout[paymentsF-1].nValue = 0;
      //
			// 	CTxDestination address0;
			// 	ExtractDestination(payeeF, address0);
			// 	CBitcoinAddress address3(address0);
      //
			// 	LogPrintf("Foundation payment to %s\n", address3.ToString().c_str());
			// }

    }else{
      // Height first in coinbase required for block.version=2
      coinbaseTx.vin[0].scriptSig = (CScript() << nHeight) + COINBASE_FLAGS;
      assert(coinbaseTx.vin[0].scriptSig.size() <= 100);
      coinbaseTx.vout[0].SetEmpty();

    }

    nLastBlockTx = nBlockTx;
    nLastBlockSize = nBlockSize;
    //LogPrintf("CreateNewBlock(): total size %u txs: %u fees: %ld sigops %d\n", nBlockSize, nBlockTx, nFees, nBlockSigOps);

    pblock->vtx[0] = MakeTransactionRef(std::move(coinbaseTx));
    pblocktemplate->vTxFees[0] = -nFees;

    // Fill in header
    pblock->hashPrevBlock  = pindexPrev->GetBlockHash();
    UpdateTime(pblock, chainparams.GetConsensus(), pindexPrev);
    pblock->nBits          = GetNextTargetRequired(pindexPrev, fProofOfStake);
    pblock->nNonce         = 0;
    pblocktemplate->vTxSigOps[0] = GetLegacySigOpCount(*pblock->vtx[0]);

    CValidationState state;
    if (!fProofOfStake && !TestBlockValidity(state, chainparams, *pblock, pindexPrev, false, false)) {
        throw std::runtime_error(strprintf("%s: TestBlockValidity failed: %s", __func__, FormatStateMessage(state)));
    }
    int64_t nTime2 = GetTimeMicros();

    LogPrint("bench", "CreateNewBlock() packages: %.2fms (%d packages, %d updated descendants), validity: %.2fms (total %.2fms)\n", 0.001 * (nTime1 - nTimeStart), nPackagesSelected, nDescendantsUpdated, 0.001 * (nTime2 - nTime1), 0.001 * (nTime2 - nTimeStart));

    return std::move(pblocktemplate);
}

bool BlockAssembler::isStillDependent(CTxMemPool::txiter iter)
{
    BOOST_FOREACH(CTxMemPool::txiter parent, mempool.GetMemPoolParents(iter))
    {
        if (!inBlock.count(parent)) {
            return true;
        }
    }
    return false;
}

void BlockAssembler::onlyUnconfirmed(CTxMemPool::setEntries& testSet)
{
    for (CTxMemPool::setEntries::iterator iit = testSet.begin(); iit != testSet.end(); ) {
        // Only test txs not already in the block
        if (inBlock.count(*iit)) {
            testSet.erase(iit++);
        }
        else {
            iit++;
        }
    }
}

bool BlockAssembler::TestPackage(uint64_t packageSize, unsigned int packageSigOps)
{
    if (nBlockSize + packageSize >= nBlockMaxSize)
        return false;
    if (nBlockSigOps + packageSigOps >= MaxBlockSigOps(fDIP0001ActiveAtTip))
        return false;
    return true;
}

// Perform transaction-level checks before adding to block:
// - transaction finality (locktime)
bool BlockAssembler::TestPackageTransactions(const CTxMemPool::setEntries& package)
{
    BOOST_FOREACH (const CTxMemPool::txiter it, package) {
        if (!IsFinalTx(it->GetTx(), nHeight, nLockTimeCutoff))
            return false;
    }
    return true;
}

bool BlockAssembler::TestForBlock(CTxMemPool::txiter iter)
{
    if (nBlockSize + iter->GetTxSize() >= nBlockMaxSize) {
        // If the block is so close to full that no more txs will fit
        // or if we've tried more than 50 times to fill remaining space
        // then flag that the block is finished
        if (nBlockSize >  nBlockMaxSize - 100 || lastFewTxs > 50) {
             blockFinished = true;
             return false;
        }
        // Once we're within 1000 bytes of a full block, only look at 50 more txs
        // to try to fill the remaining space.
        if (nBlockSize > nBlockMaxSize - 1000) {
            lastFewTxs++;
        }
        return false;
    }

    unsigned int nMaxBlockSigOps = MaxBlockSigOps(fDIP0001ActiveAtTip);
    if (nBlockSigOps + iter->GetSigOpCount() >= nMaxBlockSigOps) {
        // If the block has room for no more sig ops then
        // flag that the block is finished
        if (nBlockSigOps > nMaxBlockSigOps - 2) {
            blockFinished = true;
            return false;
        }
        // Otherwise attempt to find another tx with fewer sigops
        // to put in the block.
        return false;
    }

    // Must check that lock times are still valid
    // This can be removed once MTP is always enforced
    // as long as reorgs keep the mempool consistent.
    if (!IsFinalTx(iter->GetTx(), nHeight, nLockTimeCutoff))
        return false;

    return true;
}

void BlockAssembler::AddToBlock(CTxMemPool::txiter iter)
{
    pblock->vtx.emplace_back(iter->GetSharedTx());
    pblocktemplate->vTxFees.push_back(iter->GetFee());
    pblocktemplate->vTxSigOps.push_back(iter->GetSigOpCount());
    nBlockSize += iter->GetTxSize();
    ++nBlockTx;
    nBlockSigOps += iter->GetSigOpCount();
    nFees += iter->GetFee();
    inBlock.insert(iter);

    bool fPrintPriority = GetBoolArg("-printpriority", DEFAULT_PRINTPRIORITY);
    if (fPrintPriority) {
        double dPriority = iter->GetPriority(nHeight);
        CAmount dummy;
        mempool.ApplyDeltas(iter->GetTx().GetHash(), dPriority, dummy);
        LogPrintf("priority %.1f fee %s txid %s\n",
                  dPriority,
                  CFeeRate(iter->GetModifiedFee(), iter->GetTxSize()).ToString(),
                  iter->GetTx().GetHash().ToString());
    }
}

int BlockAssembler::UpdatePackagesForAdded(const CTxMemPool::setEntries& alreadyAdded,
        indexed_modified_transaction_set &mapModifiedTx)
{
    int nDescendantsUpdated = 0;
    BOOST_FOREACH(const CTxMemPool::txiter it, alreadyAdded) {
        CTxMemPool::setEntries descendants;
        mempool.CalculateDescendants(it, descendants);
        // Insert all descendants (not yet in block) into the modified set
        BOOST_FOREACH(CTxMemPool::txiter desc, descendants) {
            if (alreadyAdded.count(desc))
                continue;
            ++nDescendantsUpdated;
            modtxiter mit = mapModifiedTx.find(desc);
            if (mit == mapModifiedTx.end()) {
                CTxMemPoolModifiedEntry modEntry(desc);
                modEntry.nSizeWithAncestors -= it->GetTxSize();
                modEntry.nModFeesWithAncestors -= it->GetModifiedFee();
                modEntry.nSigOpCountWithAncestors -= it->GetSigOpCount();
                mapModifiedTx.insert(modEntry);
            } else {
                mapModifiedTx.modify(mit, update_for_parent_inclusion(it));
            }
        }
    }
    return nDescendantsUpdated;
}

// Skip entries in mapTx that are already in a block or are present
// in mapModifiedTx (which implies that the mapTx ancestor state is
// stale due to ancestor inclusion in the block)
// Also skip transactions that we've already failed to add. This can happen if
// we consider a transaction in mapModifiedTx and it fails: we can then
// potentially consider it again while walking mapTx.  It's currently
// guaranteed to fail again, but as a belt-and-suspenders check we put it in
// failedTx and avoid re-evaluation, since the re-evaluation would be using
// cached size/sigops/fee values that are not actually correct.
bool BlockAssembler::SkipMapTxEntry(CTxMemPool::txiter it, indexed_modified_transaction_set &mapModifiedTx, CTxMemPool::setEntries &failedTx)
{
    assert (it != mempool.mapTx.end());
    if (mapModifiedTx.count(it) || inBlock.count(it) || failedTx.count(it))
        return true;
    return false;
}

void BlockAssembler::SortForBlock(const CTxMemPool::setEntries& package, CTxMemPool::txiter entry, std::vector<CTxMemPool::txiter>& sortedEntries)
{
    // Sort package by ancestor count
    // If a transaction A depends on transaction B, then A's ancestor count
    // must be greater than B's.  So this is sufficient to validly order the
    // transactions for block inclusion.
    sortedEntries.clear();
    sortedEntries.insert(sortedEntries.begin(), package.begin(), package.end());
    std::sort(sortedEntries.begin(), sortedEntries.end(), CompareTxIterByAncestorCount());
}

// This transaction selection algorithm orders the mempool based
// on feerate of a transaction including all unconfirmed ancestors.
// Since we don't remove transactions from the mempool as we select them
// for block inclusion, we need an alternate method of updating the feerate
// of a transaction with its not-yet-selected ancestors as we go.
// This is accomplished by walking the in-mempool descendants of selected
// transactions and storing a temporary modified state in mapModifiedTxs.
// Each time through the loop, we compare the best transaction in
// mapModifiedTxs with the next transaction in the mempool to decide what
// transaction package to work on next.
void BlockAssembler::addPackageTxs(int &nPackagesSelected, int &nDescendantsUpdated)
{
    // mapModifiedTx will store sorted packages after they are modified
    // because some of their txs are already in the block
    indexed_modified_transaction_set mapModifiedTx;
    // Keep track of entries that failed inclusion, to avoid duplicate work
    CTxMemPool::setEntries failedTx;

    // Start by adding all descendants of previously added txs to mapModifiedTx
    // and modifying them for their already included ancestors
    UpdatePackagesForAdded(inBlock, mapModifiedTx);

    CTxMemPool::indexed_transaction_set::index<ancestor_score>::type::iterator mi = mempool.mapTx.get<ancestor_score>().begin();
    CTxMemPool::txiter iter;

    // Limit the number of attempts to add transactions to the block when it is
    // close to full; this is just a simple heuristic to finish quickly if the
    // mempool has a lot of entries.
    const int64_t MAX_CONSECUTIVE_FAILURES = 1000;
    int64_t nConsecutiveFailed = 0;

    while (mi != mempool.mapTx.get<ancestor_score>().end() || !mapModifiedTx.empty())
    {
        // First try to find a new transaction in mapTx to evaluate.
        if (mi != mempool.mapTx.get<ancestor_score>().end() &&
                SkipMapTxEntry(mempool.mapTx.project<0>(mi), mapModifiedTx, failedTx)) {
            ++mi;
            continue;
        }

        // Now that mi is not stale, determine which transaction to evaluate:
        // the next entry from mapTx, or the best from mapModifiedTx?
        bool fUsingModified = false;

        modtxscoreiter modit = mapModifiedTx.get<ancestor_score>().begin();
        if (mi == mempool.mapTx.get<ancestor_score>().end()) {
            // We're out of entries in mapTx; use the entry from mapModifiedTx
            iter = modit->iter;
            fUsingModified = true;
        } else {
            // Try to compare the mapTx entry to the mapModifiedTx entry
            iter = mempool.mapTx.project<0>(mi);
            if (modit != mapModifiedTx.get<ancestor_score>().end() &&
                    CompareModifiedEntry()(*modit, CTxMemPoolModifiedEntry(iter))) {
                // The best entry in mapModifiedTx has higher score
                // than the one from mapTx.
                // Switch which transaction (package) to consider
                iter = modit->iter;
                fUsingModified = true;
            } else {
                // Either no entry in mapModifiedTx, or it's worse than mapTx.
                // Increment mi for the next loop iteration.
                ++mi;
            }
        }

        // We skip mapTx entries that are inBlock, and mapModifiedTx shouldn't
        // contain anything that is inBlock.
        assert(!inBlock.count(iter));

        uint64_t packageSize = iter->GetSizeWithAncestors();
        CAmount packageFees = iter->GetModFeesWithAncestors();
        unsigned int packageSigOps = iter->GetSigOpCountWithAncestors();
        if (fUsingModified) {
            packageSize = modit->nSizeWithAncestors;
            packageFees = modit->nModFeesWithAncestors;
            packageSigOps = modit->nSigOpCountWithAncestors;
        }

        if (packageFees < blockMinFeeRate.GetFee(packageSize)) {
            // Everything else we might consider has a lower fee rate
            return;
        }

        if (!TestPackage(packageSize, packageSigOps)) {
            if (fUsingModified) {
                // Since we always look at the best entry in mapModifiedTx,
                // we must erase failed entries so that we can consider the
                // next best entry on the next loop iteration
                mapModifiedTx.get<ancestor_score>().erase(modit);
                failedTx.insert(iter);
            }

            ++nConsecutiveFailed;

            if (nConsecutiveFailed > MAX_CONSECUTIVE_FAILURES && nBlockSize > nBlockMaxSize - 1000) {
                // Give up if we're close to full and haven't succeeded in a while
                break;
            }
            continue;
        }

        CTxMemPool::setEntries ancestors;
        uint64_t nNoLimit = std::numeric_limits<uint64_t>::max();
        std::string dummy;
        mempool.CalculateMemPoolAncestors(*iter, ancestors, nNoLimit, nNoLimit, nNoLimit, nNoLimit, dummy, false);

        onlyUnconfirmed(ancestors);
        ancestors.insert(iter);

        // Test if all tx's are Final
        if (!TestPackageTransactions(ancestors)) {
            if (fUsingModified) {
                mapModifiedTx.get<ancestor_score>().erase(modit);
                failedTx.insert(iter);
            }
            continue;
        }

        // This transaction will make it in; reset the failed counter.
        nConsecutiveFailed = 0;

        // Package can be added. Sort the entries in a valid order.
        std::vector<CTxMemPool::txiter> sortedEntries;
        SortForBlock(ancestors, iter, sortedEntries);

        for (size_t i=0; i<sortedEntries.size(); ++i) {
            AddToBlock(sortedEntries[i]);
            // Erase from the modified set, if present
            mapModifiedTx.erase(sortedEntries[i]);
        }

        ++nPackagesSelected;

        // Update transactions that depend on each of these
        nDescendantsUpdated += UpdatePackagesForAdded(ancestors, mapModifiedTx);
    }
}

void BlockAssembler::addPriorityTxs()
{
    // How much of the block should be dedicated to high-priority transactions,
    // included regardless of the fees they pay
    unsigned int nBlockPrioritySize = GetArg("-blockprioritysize", DEFAULT_BLOCK_PRIORITY_SIZE);
    nBlockPrioritySize = std::min(nBlockMaxSize, nBlockPrioritySize);

    if (nBlockPrioritySize == 0) {
        return;
    }

    // This vector will be sorted into a priority queue:
    std::vector<TxCoinAgePriority> vecPriority;
    TxCoinAgePriorityCompare pricomparer;
    std::map<CTxMemPool::txiter, double, CTxMemPool::CompareIteratorByHash> waitPriMap;
    typedef std::map<CTxMemPool::txiter, double, CTxMemPool::CompareIteratorByHash>::iterator waitPriIter;
    double actualPriority = -1;

    vecPriority.reserve(mempool.mapTx.size());
    for (CTxMemPool::indexed_transaction_set::iterator mi = mempool.mapTx.begin();
         mi != mempool.mapTx.end(); ++mi)
    {
        double dPriority = mi->GetPriority(nHeight);
        CAmount dummy;
        mempool.ApplyDeltas(mi->GetTx().GetHash(), dPriority, dummy);
        vecPriority.push_back(TxCoinAgePriority(dPriority, mi));
    }
    std::make_heap(vecPriority.begin(), vecPriority.end(), pricomparer);

    CTxMemPool::txiter iter;
    while (!vecPriority.empty() && !blockFinished) { // add a tx from priority queue to fill the blockprioritysize
        iter = vecPriority.front().second;
        actualPriority = vecPriority.front().first;
        std::pop_heap(vecPriority.begin(), vecPriority.end(), pricomparer);
        vecPriority.pop_back();

        // If tx already in block, skip
        if (inBlock.count(iter)) {
            assert(false); // shouldn't happen for priority txs
            continue;
        }

        // If tx is dependent on other mempool txs which haven't yet been included
        // then put it in the waitSet
        if (isStillDependent(iter)) {
            waitPriMap.insert(std::make_pair(iter, actualPriority));
            continue;
        }

        // If this tx fits in the block add it, otherwise keep looping
        if (TestForBlock(iter)) {
            AddToBlock(iter);

            // If now that this txs is added we've surpassed our desired priority size
            // or have dropped below the AllowFreeThreshold, then we're done adding priority txs
            if (nBlockSize >= nBlockPrioritySize || !AllowFree(actualPriority)) {
                break;
            }

            // This tx was successfully added, so
            // add transactions that depend on this one to the priority queue to try again
            BOOST_FOREACH(CTxMemPool::txiter child, mempool.GetMemPoolChildren(iter))
            {
                waitPriIter wpiter = waitPriMap.find(child);
                if (wpiter != waitPriMap.end()) {
                    vecPriority.push_back(TxCoinAgePriority(wpiter->second,child));
                    std::push_heap(vecPriority.begin(), vecPriority.end(), pricomparer);
                    waitPriMap.erase(wpiter);
                }
            }
        }
    }
}

void IncrementExtraNonce(CBlock* pblock, const CBlockIndex* pindexPrev, unsigned int& nExtraNonce)
{
    // Update nExtraNonce
    static uint256 hashPrevBlock;
    if (hashPrevBlock != pblock->hashPrevBlock)
    {
        nExtraNonce = 0;
        hashPrevBlock = pblock->hashPrevBlock;
    }
    ++nExtraNonce;
    unsigned int nHeight = pindexPrev->nHeight+1; // Height first in coinbase required for block.version=2
    CMutableTransaction txCoinbase(*pblock->vtx[0]);
    txCoinbase.vin[0].scriptSig = (CScript() << nHeight << CScriptNum(nExtraNonce)) + COINBASE_FLAGS;
    assert(txCoinbase.vin[0].scriptSig.size() <= 100);

    pblock->vtx[0] = MakeTransactionRef(std::move(txCoinbase));
    pblock->hashMerkleRoot = BlockMerkleRoot(*pblock);
}

bool CheckStake(CBlock* pblock, CWallet& wallet)
{
    uint256 proofHash;
    uint256 hashTarget;
    uint256 hashBlock = pblock->GetHash();

    if(!pblock->IsProofOfStake())
        return error("CheckStake() : %s is not a proof-of-stake block", hashBlock.GetHex());

    CValidationState state;
    // verify hash target and signature of coinstake tx
    if (!CheckProofOfStake(state, mapBlockIndex[pblock->hashPrevBlock], *pblock->vtx[1], pblock->nBits, proofHash, hashTarget))
        return error("CheckStake() : proof-of-stake checking failed");

    //// debug print
    LogPrintf("CheckStake() : new proof-of-stake block found  \n  hash: %s \nproofhash: %s  \ntarget: %s\n", hashBlock.GetHex(), proofHash.GetHex(), hashTarget.GetHex());
    LogPrintf("%s\n", pblock->ToString());
    LogPrintf("out %s\n", FormatMoney(pblock->vtx[1]->GetValueOut()));

    // Found a solution
    {
        LOCK(cs_main);
        if (pblock->hashPrevBlock != chainActive.Tip()->GetBlockHash())
            return error("CheckStake() : generated block is stale");

        // Track how many getdata requests this block gets
        {
            LOCK(wallet.cs_wallet);
            wallet.mapRequestCount[hashBlock] = 0;
        }

        std::shared_ptr<const CBlock> shared_pblock = std::make_shared<const CBlock>(*pblock);
        if (!ProcessNewBlock(Params(), shared_pblock, true, NULL))
            return error("CheckStake() : ProcessBlock, block not accepted");
        // else
        // {
        //     //ProcessBlock successful for PoS. now FixSpentCoins.
        //     int nMismatchSpent;
        //     CAmount nBalanceInQuestion;
        //     {
        //       LOCK(wallet.cs_wallet);
        //       wallet.FixSpentCoins(nMismatchSpent, nBalanceInQuestion);
        //       if (nMismatchSpent != 0)
        //       {
        //           LogPrintf("PoS mismatched spent coins = %d and balance affects = %d \n", nMismatchSpent, nBalanceInQuestion);
        //       }
        //     }
        // }
    }

    return true;
}

void ThreadStakeMiner(CWallet *pwallet)
{
    CBlockIndex* pindexBest = chainActive.Tip();
    // Make this thread recognisable as the mining thread
    RenameThread("MarteX-miner");

    int nBestHeight; // TODO: set from new block signal?
    int64_t nBestTime;
    bool fTryToSync = true;

    while (true)
    {
        while (pwallet->IsLocked())
        {
            nLastCoinStakeSearchInterval = 0;
            MilliSleep(1000);
        }

        int num_blocks_of_peers, num_nodes;
        {
            LOCK(cs_main);
            nBestHeight = chainActive.Height();
            nBestTime = chainActive.Tip()->nTime;
            num_nodes = g_connman->GetNodeCount(CConnman::CONNECTIONS_ALL);
        }

        while (num_nodes == 0 || !masternodeSync.IsSynced())
        {
            nLastCoinStakeSearchInterval = 0;
            fTryToSync = true;
            MilliSleep(1000);
            num_nodes = g_connman->GetNodeCount(CConnman::CONNECTIONS_ALL);
        }

        if (fTryToSync)
        {
            fTryToSync = false;
            if (pindexBest->GetBlockTime() < GetTime() - 10 * 60)
            {
                MilliSleep(10000);
                continue;
            }
        }


        if(!CSuperblock::IsValidBlockHeight(nBestHeight +1)){
          //
          // Create new block
          //
          int64_t nFees;
          boost::shared_ptr<CReserveScript> coinbaseScript;
          GetMainSignals().ScriptForMining(coinbaseScript);

          static std::unique_ptr<CBlockTemplate> pblocktemplate;
          pblocktemplate = BlockAssembler(Params()).CreateNewBlock(coinbaseScript->reserveScript, true);
          CBlock* pblock = &pblocktemplate->block; // pointer for convenience

          if (!pblocktemplate)
              return;

          if (pwallet->SignBlock(pblocktemplate.get(), nBestHeight))
          {
              CheckStake(pblock, *pwallet);
              MilliSleep(60000);
          }
          else
              MilliSleep(nMinerSleep);

        }
    }
}

/** PoS Reward */
static const int64_t COIN_YEAR_REWARD = 1.5 * COIN; // Corrected in v1.0.2.0+
/** PoS Reward Fixed */
static const int64_t COIN_YEAR_REWARD_FIXED = 50 * CENT; // 50%
/** PoS Superblock Reward */
static const int64_t COIN_SPRB_REWARD = 3 * COIN; // Corrected in v1.0.2.0+
/** PoS Superblock Reward Fixed */
static const int64_t COIN_SPRB_REWARD_FIXED = 55 * CENT; // 55%
/** PoS Reward Correct */
static const int64_t COIN_YEAR_REWARD_CORRECT = 0.05 * COIN;
/** PoS Superblock Reward Correct */
static const int64_t COIN_SPRB_REWARD_CORRECT = 0.06 * COIN;
/** PoS Interest **/
static const int64_t COIN_YEAR_INTEREST = 50 * CENT; // 50%
/** Reward Fix toggle */
static const int64_t RWRD_FIX_TOGGLE = 827;
/** MN Reward Fix toggle */
static const int64_t MN_FIX_TOGGLE = 5500;
/** MN Reward Fixed */
static const int64_t MN_REWARD_FIXED = 50 * CENT;

// miner's coin stake reward
int64_t GetProofOfStakeReward(const CBlockIndex* pindexPrev, int64_t nCoinAge, int64_t nFees)
{
    /** NEW block subsidy version 3.0.0 */
    static const int64_t nBlockReward = 2.5 * COIN;
    int64_t nSubsidy = nCoinAge * COIN_YEAR_REWARD * 33 / (365 * 33 + 8);

    if(randreward() <= 20500) // 20.5% Chance of superblock
        nSubsidy = nCoinAge * COIN_SPRB_REWARD * 33 / (365 * 33 + 8);
    if(chainActive.Tip()->nHeight + 1 > RWRD_FIX_TOGGLE) // Correct block reward payouts
    {
        nSubsidy = nCoinAge * COIN_YEAR_REWARD_FIXED * 33 / (365 * 33 + 8);
        if(randreward() <= 20500) // 20.5% Chance of superblock (Fixed)
            nSubsidy = nCoinAge * COIN_SPRB_REWARD_FIXED * 33 / (365 * 33 + 8);
        // Correct subsidy for proper MN allocation
        if(chainActive.Tip()->nHeight + 1 > MN_FIX_TOGGLE)
            nSubsidy = nCoinAge * MN_REWARD_FIXED * 33 / (365 * 33 + 8);
    }

    // New reward version 3.0.0
    if ((chainActive.Tip()->nHeight + 1 > START_REWARD_3_0_T && Params().NetworkIDString() == CBaseChainParams::TESTNET) || (chainActive.Tip()->nHeight + 1 > START_REWARD_3_0_M && Params().NetworkIDString() == CBaseChainParams::MAIN)) {
         nSubsidy = nBlockReward;
         // Hard fork to reduce the block reward by 10 extra percent (allowing budget/superblocks)
         CAmount nSuperblockPart = (chainActive.Tip()->nHeight + 1 > Params().GetConsensus().nBudgetPaymentsStartBlock) ? nSubsidy/10 : 0;
         nSubsidy = nSubsidy - nSuperblockPart;
    }
    else if(chainActive.Tip()->nMoneySupply > MAX_SINGLE_TX)
    {
        LogPrintf("GetProofOfStakeReward(): create=%s nFees=%d\n", FormatMoney(nFees), nFees);
        return nFees;
    }
    LogPrintf("GetProofOfStakeReward(): create=%s nCoinAge=%d\n", FormatMoney(nSubsidy), nCoinAge);
    return nSubsidy + nFees;
}
