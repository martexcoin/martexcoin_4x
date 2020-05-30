// Copyright (c) 2012-2013 The PPCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_KERNEL_H
#define BITCOIN_KERNEL_H
#include "uint256.h"
#include "streams.h"
#include "arith_uint256.h"

#include "chainparams.h"
#include "consensus/validation.h"
#include "uint256.h"
#include "primitives/transaction.h"
#include "pow.h"

class CBlock;
class CWallet;
class COutPoint;
class CBlockIndex;
class CTransaction;
// MODIFIER_INTERVAL: time to elapse before new modifier is computed
static const unsigned int MODIFIER_INTERVAL = 60;
static const unsigned int MODIFIER_INTERVAL_TESTNET = 20;
extern unsigned int nModifierInterval;
extern unsigned int getIntervalVersion(bool fTestNet);
// MODIFIER_INTERVAL_RATIO:

/** Coinbase transaction outputs can only be staked after this number of new blocks (network rule) */
static const int nStakeMinConfirmations = 15;
// ratio of group interval length between the last group and the first group
static const int MODIFIER_INTERVAL_RATIO = 3;
/** Protocol toggle */
inline bool IsProtocolV3(int64_t nTime) { return Params().NetworkIDString() == CBaseChainParams::TESTNET || nTime > 1493596800; } // Mon, 01 May 2017 00:00:00 GMT
// Compute the hash modifier for proof-of-stake
bool ComputeNextStakeModifier(const CBlockIndex* pindexPrev, uint64_t& nStakeModifier, bool& fGeneratedStakeModifier);
uint256 ComputeStakeModifierV2(const CBlockIndex* pindexPrev, const uint256& kernel);
// Check whether stake kernel meets hash target
// Sets hashProofOfStake on success return
bool CheckStakeKernelHash(unsigned int nBits, const CBlock& blockFrom, unsigned int nTxPrevOffset,
                          std::shared_ptr<const CTransaction> txPrev, const COutPoint& prevout, unsigned int nTimeTx,
                          uint256& hashProofOfStake, bool fPrintProofOfStake = false);
bool OldCheckStakeKernelHash(const CBlockIndex* pindexPrev, unsigned int nBits, unsigned int nTimeBlockFrom,
                          const CTransactionRef& txPrev, const COutPoint& prevout, unsigned int nTimeTx,
                          uint256& hashProofOfStake, uint256& targetProofOfStake, bool fPrintProofOfStake);
// Check kernel hash target and coinstake signature
// Sets hashProofOfStake on success return
bool CheckProofOfStake(const CBlock &block, uint256& hashProofOfStake);
bool OldCheckProofOfStake(CValidationState &state, const CBlockIndex* pindexPrev, const CTransaction& tx, unsigned int nBits, uint256& hashProofOfStake, uint256& targetProofOfStake);

// Check whether the coinstake timestamp meets protocol
bool CheckCoinStakeTimestamp(int64_t nTimeBlock, int64_t nTimeTx);
// Get stake modifier checksum
unsigned int GetStakeModifierChecksum(const CBlockIndex* pindex);
// Check stake modifier hard checkpoints
bool CheckStakeModifierCheckpoints(int nHeight, unsigned int nStakeModifierChecksum);

// Get time weight using supplied timestamps
int64_t GetWeight(int64_t nIntervalBeginning, int64_t nIntervalEnd);

// Wrapper around CheckStakeKernelHash()
// Also checks existence of kernel input and min age
// Convenient for searching a kernel
bool CheckKernel(CBlockIndex* pindexPrev, unsigned int nBits, int64_t nTime, const COutPoint& prevout, int64_t* pBlockTime = NULL);
bool IsConfirmedInNPrevBlocks(const uint256 &hashBlock, const CBlockIndex *pindexFrom, int nMaxDepth, int &nActualDepth);
bool CheckAge(const CBlockIndex *pindexTip, const uint256 &hashKernelBlock, int &nDepth);

#endif // BITCOIN_KERNEL_H
