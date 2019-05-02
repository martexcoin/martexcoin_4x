// Copyright (c) 2012-2013 The PPCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef PPCOIN_KERNEL_H
#define PPCOIN_KERNEL_H

#include "chainparams.h"
#include "consensus/validation.h"
#include "uint256.h"
#include "primitives/transaction.h"
#include "pow.h"

// To decrease granularity of timestamp
// Supposed to be 2^n-1
static const int STAKE_TIMESTAMP_MASK = 15;

// MODIFIER_INTERVAL_RATIO:
// ratio of group interval length between the last group and the first group
static const int MODIFIER_INTERVAL_RATIO = 3;

/** Coinbase transaction outputs can only be staked after this number of new blocks (network rule) */
static const int nStakeMinConfirmations = 15;
/** Minimum nCoinAge required to stake PoS */
static const unsigned int nStakeMinAge = 10 * 60; // 2 horas
/** Time to elapse before new modifier is computed */
static const unsigned int nModifierInterval = 2 * 60;
/** Desired block times/spacing */
static const int64_t GetTargetSpacing = BLOCK_SPACING;
/** Protocol toggle */
inline bool IsProtocolV3(int64_t nTime) { return Params().NetworkIDString() == CBaseChainParams::TESTNET || nTime > 1493596800; } // Mon, 01 May 2017 00:00:00 GMT

class CBlockIndex;

// Compute the hash modifier for proof-of-stake
bool ComputeNextStakeModifier(const CBlockIndex* pindexPrev, uint64_t& nStakeModifier, bool& fGeneratedStakeModifier);
uint256 ComputeStakeModifierV2(const CBlockIndex* pindexPrev, const uint256& kernel);

// Check whether stake kernel meets hash target
// Sets hashProofOfStake on success return
bool CheckStakeKernelHash(const CBlockIndex* pindexPrev, unsigned int nBits, unsigned int nTimeBlockFrom, const CTransactionRef& txPrev, const COutPoint& prevout, unsigned int nTimeTx, uint256& hashProofOfStake, uint256& targetProofOfStake, bool fPrintProofOfStake=false);

// Check kernel hash target and coinstake signature
// Sets hashProofOfStake on success return
bool CheckProofOfStake(CValidationState &state, const CBlockIndex* pindexPrev, const CTransaction& tx, unsigned int nBits, uint256& hashProofOfStake, uint256& targetProofOfStake);

// Check whether the coinstake timestamp meets protocol
bool CheckCoinStakeTimestamp(int nHeight, int64_t nTimeBlock, int64_t nTimeTx);

// Get time weight using supplied timestamps
int64_t GetWeight(int64_t nIntervalBeginning, int64_t nIntervalEnd);

// Wrapper around CheckStakeKernelHash()
// Also checks existence of kernel input and min age
// Convenient for searching a kernel
bool CheckKernel(CBlockIndex* pindexPrev, unsigned int nBits, int64_t nTime, const COutPoint& prevout, int64_t* pBlockTime = NULL);
bool IsConfirmedInNPrevBlocks(const uint256 &hashBlock, const CBlockIndex *pindexFrom, int nMaxDepth, int &nActualDepth);

#endif // PPCOIN_KERNEL_H
