// Copyright (c) 2020 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "zerocoin_verify.h"

#include "chainparams.h"
#include "consensus/consensus.h"
#include "guiinterface.h"        // for ui_interface
#include "init.h"                // for ShutdownRequested()
#include "main.h"
#include "txdb.h"

bool RecalculatePIVSupply(int nHeightStart, bool fSkipZpiv)
{
    const Consensus::Params& consensus = Params().GetConsensus();
    const int chainHeight = chainActive.Height();
    if (nHeightStart > chainHeight)
        return false;

    CBlockIndex* pindex = chainActive[nHeightStart];
    CAmount nSupplyPrev = pindex->pprev->nMoneySupply;
    if (nHeightStart == consensus.height_start_ZC)
        nSupplyPrev = CAmount(5449796547496199);

    uiInterface.ShowProgress(_("Recalculating PIV supply..."), 0);
    while (true) {
        if (pindex->nHeight % 1000 == 0) {
            LogPrintf("%s : block %d...\n", __func__, pindex->nHeight);
            int percent = std::max(1, std::min(99, (int)((double)((pindex->nHeight - nHeightStart) * 100) / (chainHeight - nHeightStart))));
            uiInterface.ShowProgress(_("Recalculating PIV supply..."), percent);
        }

        CBlock block;
        assert(ReadBlockFromDisk(block, pindex));

        CAmount nValueIn = 0;
        CAmount nValueOut = 0;
        for (const CTransaction& tx : block.vtx) {
            for (unsigned int i = 0; i < tx.vin.size(); i++) {
                if (tx.IsCoinBase())
                    break;

                if (tx.vin[i].IsZerocoinSpend()) {
                    nValueIn += tx.vin[i].nSequence * COIN;
                    continue;
                }

                COutPoint prevout = tx.vin[i].prevout;
                CTransaction txPrev;
                uint256 hashBlock;
                assert(GetTransaction(prevout.hash, txPrev, hashBlock, true));
                nValueIn += txPrev.vout[prevout.n].nValue;
            }

            for (unsigned int i = 0; i < tx.vout.size(); i++) {
                if (i == 0 && tx.IsCoinStake())
                    continue;

                nValueOut += tx.vout[i].nValue;
            }
        }

        // Rewrite money supply
        pindex->nMoneySupply = nSupplyPrev + nValueOut - nValueIn;
        nSupplyPrev = pindex->nMoneySupply;

        // Rewrite zMXT supply too
        if (!fSkipZpiv && pindex->nHeight >= consensus.height_start_ZC) {
            UpdateZPIVSupply(block, pindex);
        }

        assert(pblocktree->WriteBlockIndex(CDiskBlockIndex(pindex)));

        // Stop if shutdown was requested
        if (ShutdownRequested()) return false;

        if (pindex->nHeight < chainHeight)
            pindex = chainActive.Next(pindex);
        else
            break;
    }
    uiInterface.ShowProgress("", 100);
    return true;
}

bool UpdateZPIVSupply(const CBlock& block, CBlockIndex* pindex)
{
    const Consensus::Params& consensus = Params().GetConsensus();
    if (pindex->nHeight < consensus.height_start_ZC)
        return true;

    // Reset the supply to previous block
    pindex->mapZerocoinSupply = pindex->pprev->mapZerocoinSupply;

    // If we're past the final accumulator; skip this entirely
    if (pindex->nHeight > consensus.height_last_ZC_AccumCheckpoint)
        return true;

    // Add mints to zMXT supply
    std::list<CZerocoinMint> listMints;
    BlockToZerocoinMintList(block, listMints, true);
    for (const auto& m : listMints) {
        pindex->mapZerocoinSupply.at(m.GetDenomination())++;
    }

    // Remove spends from zMXT supply
    std::list<libzerocoin::CoinDenomination> listDenomsSpent = ZerocoinSpendListFromBlock(block, true);
    for (const auto& denom : listDenomsSpent) {
        pindex->mapZerocoinSupply.at(denom)--;
    }

    for (const auto& denom : libzerocoin::zerocoinDenomList)
        LogPrint("zero", "%s coins for denomination %d pubcoin %s\n", __func__, denom, pindex->mapZerocoinSupply.at(denom));

    return true;
}