// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_POW_H
#define BITCOIN_POW_H

#include "consensus/params.h"

#include <stdint.h>

class CBlockHeader;
class CBlockIndex;
class uint256;

static const int64_t nlowGravity = 198500; // Correct low gravity issue with DGW implementation.
/** Block spacing preferred */
static const int64_t BLOCK_SPACING = 2 * 60;
/** Block spacing minimum */
static const int64_t BLOCK_SPACING_MIN = 1.5 * 60;
/** Block spacing maximum */
static const int64_t BLOCK_SPACING_MAX = 3.5 * 60;
/** Block spacing preferred CORRECT */
static const int64_t BLOCK_SPACING_CORRECT = 0.5 * 60;
/** Block spacing minimum CORRECT */
static const int64_t BLOCK_SPACING_MIN_CORRECT = 0.25 * 60;
/** Block spacing maximum CORRECT */
static const int64_t BLOCK_SPACING_MAX_CORRECT = 0.5 * 60;
/** Block spacing preferred CORRECT NEW */
static const int64_t BLOCK_SPACING_CORRECT_NEW = 1 * 60;
/** Block spacing minimum CORRECT  NEW */
static const int64_t BLOCK_SPACING_MIN_CORRECT_NEW = 0.5 * 60;
/** Block spacing maximum CORRECT  NEW */
static const int64_t BLOCK_SPACING_MAX_CORRECT_NEW = 1.25 * 60;

unsigned int GetNextTargetRequired(const CBlockIndex* pindexLast, bool fProofOfStake);
unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params&);
unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params&);
const CBlockIndex* GetLastBlockIndex(const CBlockIndex* pindex, bool fProofOfStake);

/** Check whether a block hash satisfies the proof-of-work requirement specified by nBits */
bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params&);

#endif // BITCOIN_POW_H
