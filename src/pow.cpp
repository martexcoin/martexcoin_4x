// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pow.h"

#include "arith_uint256.h"
#include "chain.h"
#include "chainparams.h"
#include "primitives/block.h"
#include "timedata.h"
#include "uint256.h"
#include "util.h"
#include "validation.h"

#include <math.h>

unsigned int static KimotoGravityWell(const CBlockIndex* pindexLast, const Consensus::Params& params) {
    const CBlockIndex *BlockLastSolved = pindexLast;
    const CBlockIndex *BlockReading = pindexLast;
    uint64_t PastBlocksMass = 0;
    int64_t PastRateActualSeconds = 0;
    int64_t PastRateTargetSeconds = 0;
    double PastRateAdjustmentRatio = double(1);
    arith_uint256 PastDifficultyAverage;
    arith_uint256 PastDifficultyAveragePrev;
    double EventHorizonDeviation;
    double EventHorizonDeviationFast;
    double EventHorizonDeviationSlow;

    uint64_t pastSecondsMin = params.nPowTargetTimespan * 0.025;
    uint64_t pastSecondsMax = params.nPowTargetTimespan * 7;
    uint64_t PastBlocksMin = pastSecondsMin / params.nPowTargetSpacing;
    uint64_t PastBlocksMax = pastSecondsMax / params.nPowTargetSpacing;

    if (BlockLastSolved == NULL || BlockLastSolved->nHeight == 0 || (uint64_t)BlockLastSolved->nHeight < PastBlocksMin) { return UintToArith256(params.powLimit).GetCompact(); }

    for (unsigned int i = 1; BlockReading && BlockReading->nHeight > 0; i++) {
        if (PastBlocksMax > 0 && i > PastBlocksMax) { break; }
        PastBlocksMass++;

        PastDifficultyAverage.SetCompact(BlockReading->nBits);
        if (i > 1) {
            // handle negative arith_uint256
            if(PastDifficultyAverage >= PastDifficultyAveragePrev)
                PastDifficultyAverage = ((PastDifficultyAverage - PastDifficultyAveragePrev) / i) + PastDifficultyAveragePrev;
            else
                PastDifficultyAverage = PastDifficultyAveragePrev - ((PastDifficultyAveragePrev - PastDifficultyAverage) / i);
        }
        PastDifficultyAveragePrev = PastDifficultyAverage;

        PastRateActualSeconds = BlockLastSolved->GetBlockTime() - BlockReading->GetBlockTime();
        PastRateTargetSeconds = params.nPowTargetSpacing * PastBlocksMass;
        PastRateAdjustmentRatio = double(1);
        if (PastRateActualSeconds < 0) { PastRateActualSeconds = 0; }
        if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0) {
            PastRateAdjustmentRatio = double(PastRateTargetSeconds) / double(PastRateActualSeconds);
        }
        EventHorizonDeviation = 1 + (0.7084 * pow((double(PastBlocksMass)/double(28.2)), -1.228));
        EventHorizonDeviationFast = EventHorizonDeviation;
        EventHorizonDeviationSlow = 1 / EventHorizonDeviation;

        if (PastBlocksMass >= PastBlocksMin) {
                if ((PastRateAdjustmentRatio <= EventHorizonDeviationSlow) || (PastRateAdjustmentRatio >= EventHorizonDeviationFast))
                { assert(BlockReading); break; }
        }
        if (BlockReading->pprev == NULL) { assert(BlockReading); break; }
        BlockReading = BlockReading->pprev;
    }

    arith_uint256 bnNew(PastDifficultyAverage);
    if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0) {
        bnNew *= PastRateActualSeconds;
        bnNew /= PastRateTargetSeconds;
    }

    if (bnNew > UintToArith256(params.powLimit)) {
        bnNew = UintToArith256(params.powLimit);
    }

    return bnNew.GetCompact();
}

unsigned int static PoSWorkRequired(const CBlockIndex* pindexLast, const Consensus::Params& params) {
    arith_uint256 bnTargetLimit = (~arith_uint256(0) >> 24);
    int64_t nTargetSpacing = Params().GetConsensus().nPosTargetSpacing;
    int64_t nTargetTimespan = Params().GetConsensus().nPosTargetTimespan;
    int64_t nActualSpacing = 0;
    if (pindexLast->nHeight != 0)
        nActualSpacing = pindexLast->GetBlockTime() - pindexLast->pprev->GetBlockTime();
    if (nActualSpacing < 0)
        nActualSpacing = 1;
    // ppcoin: target change every block
    // ppcoin: retarget with exponential moving toward target spacing
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    int64_t nInterval = nTargetTimespan / nTargetSpacing;
    bnNew *= ((nInterval - 1) * nTargetSpacing + nActualSpacing + nActualSpacing);
    bnNew /= ((nInterval + 1) * nTargetSpacing);
    if (bnNew <= 0 || bnNew > bnTargetLimit)
        bnNew = bnTargetLimit;
    return bnNew.GetCompact();
}

unsigned int static DarkGravityWave(const CBlockIndex* pindexLast, const Consensus::Params& params) {
    /* current difficulty formula, martex - DarkGravity v3 */
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    int64_t nPastBlocks = 24;

    // make sure we have at least (nPastBlocks + 1) blocks, otherwise just return powLimit
    if (!pindexLast || pindexLast->nHeight < nPastBlocks || (pindexLast->nHeight >= 57450 && pindexLast->nHeight < 57460)) {
        return bnPowLimit.GetCompact();
    }
     //    if (pindexLast->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2){
       //         return bnPowLimit.GetCompact();
//	}

    const CBlockIndex *pindex = pindexLast;
    arith_uint256 bnPastTargetAvg;

    for (unsigned int nCountBlocks = 1; nCountBlocks <= nPastBlocks; nCountBlocks++) {
        arith_uint256 bnTarget = arith_uint256().SetCompact(pindex->nBits);
        if (nCountBlocks == 1) {
            bnPastTargetAvg = bnTarget;
        } else {
            // NOTE: that's not an average really...
            bnPastTargetAvg = (bnPastTargetAvg * nCountBlocks + bnTarget) / (nCountBlocks + 1);
        }

        if(nCountBlocks != nPastBlocks) {
            assert(pindex->pprev); // should never fail
            pindex = pindex->pprev;
        }
    }

    arith_uint256 bnNew(bnPastTargetAvg);

    int64_t nActualTimespan = pindexLast->GetBlockTime() - pindex->GetBlockTime();
    // NOTE: is this accurate? nActualTimespan counts it for (nPastBlocks - 1) blocks only...
    int64_t nTargetTimespan = nPastBlocks * params.nPowTargetSpacing;

    if (nActualTimespan < nTargetTimespan/3)
        nActualTimespan = nTargetTimespan/3;
    if (nActualTimespan > nTargetTimespan*3)
        nActualTimespan = nTargetTimespan*3;

    // Retarget
    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

    if (bnNew > bnPowLimit) {
        bnNew = bnPowLimit;
    }

    return bnNew.GetCompact();
}

unsigned int GetNextWorkRequiredBTC(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;

    // Only change once per interval
    if ((pindexLast->nHeight+1) % params.DifficultyAdjustmentInterval() != 0)
    {
	if (params.fPowAllowMinDifficultyBlocks)
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 2* 2.5 minutes
            // then allow mining of a min-difficulty block.
            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
                return nProofOfWorkLimit;
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                const CBlockIndex* pindex = pindexLast;
                while (pindex->pprev && pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 && pindex->nBits == nProofOfWorkLimit)
                    pindex = pindex->pprev;
                return pindex->nBits;
            }
        }
        return pindexLast->nBits;
    }

    // Go back by what we want to be 1 day worth of blocks
    int nHeightFirst = pindexLast->nHeight - (params.DifficultyAdjustmentInterval()-1);
    assert(nHeightFirst >= 0);
    const CBlockIndex* pindexFirst = pindexLast->GetAncestor(nHeightFirst);
    assert(pindexFirst);

   return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    // Most recent algo first
//	if (pindexLast->nHeight  == params.nLastPoWBlock) {
//	return 0x1e0ffff0;
//	}
    // Most recent algo first
	if (pindexLast->nHeight  <= params.nLastPoWBlock && pindexLast->nHeight >= params.nLastPoWBlock - 5) {
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
        return bnPowLimit.GetCompact();

//	return 0x1e0ffff0;
	}
    if (pindexLast->nHeight + 1 >= params.nLastPoWBlock) {
        return PoSWorkRequired(pindexLast, params);
	}
    // Most recent algo first
    else if (pindexLast->nHeight + 1 >= params.nPowDGWHeight) {
        return DarkGravityWave(pindexLast, params);
    }
    else if (pindexLast->nHeight + 1 >= params.nPowKGWHeight) {
        return KimotoGravityWell(pindexLast, params);
    }
    else {
        return GetNextWorkRequiredBTC(pindexLast, pblock, params);
    }
}

// for DIFF_BTC only!
unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    if (nActualTimespan < params.nPowTargetTimespan/4)
        nActualTimespan = params.nPowTargetTimespan/4;
    if (nActualTimespan > params.nPowTargetTimespan*4)
        nActualTimespan = params.nPowTargetTimespan*4;

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    bnNew /= params.nPowTargetTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}

// ppcoin: find last block index up to pindex
const CBlockIndex* GetLastBlockIndex(const CBlockIndex* pindex, bool fProofOfStake)
{
    while (pindex && pindex->pprev && (pindex->IsProofOfStake() != fProofOfStake))
        pindex = pindex->pprev;
    return pindex;
}

unsigned int OldDarkGravityWave(const CBlockIndex* pindexLast, bool fProofOfStake)
{
        const arith_uint256 nProofOfWorkLimit = fProofOfStake ? UintToArith256(Params().GetConsensus().posLimit) : UintToArith256(Params().GetConsensus().powLimit);
        const CBlockIndex *BlockLastSolved = pindexLast;
        const CBlockIndex *BlockLastSolved_lgf = GetLastBlockIndex(pindexLast, fProofOfStake);
        const CBlockIndex *BlockReading = pindexLast;
        // Low Gravity fix (PoW support)
        if(pindexLast->nHeight > nlowGravity)
        {
            BlockReading = BlockLastSolved_lgf;
        }
        int64_t nActualTimespan = 0;
        int64_t LastBlockTime = 0;
        int64_t PastBlocksMin = 7;
        int64_t PastBlocksMax = 24;
        int64_t CountBlocks = 0;
        arith_uint256 PastDifficultyAverage;
        arith_uint256 PastDifficultyAveragePrev;

	// make sure we have at least (nPastBlocks + 1) blocks, otherwise just return powLimit
        if (BlockLastSolved == NULL || BlockLastSolved->nHeight == 0 || BlockLastSolved->nHeight < PastBlocksMax || BlockLastSolved->nHeight == Params().GetConsensus().nPowDGWHeight) {
            return nProofOfWorkLimit.GetCompact();
        }

        for (unsigned int i = 1; BlockReading && BlockReading->nHeight > 0; i++) {
            if (PastBlocksMax > 0 && i > PastBlocksMax) { break; }
            CountBlocks++;

            if(CountBlocks <= PastBlocksMin) {
                if (CountBlocks == 1) { PastDifficultyAverage.SetCompact(BlockReading->nBits); }
                else { PastDifficultyAverage = ((PastDifficultyAveragePrev * CountBlocks) + (arith_uint256().SetCompact(BlockReading->nBits))) / (CountBlocks + 1); }
                PastDifficultyAveragePrev = PastDifficultyAverage;
            }

            if(LastBlockTime > 0){
                int64_t Diff = (LastBlockTime - BlockReading->GetBlockTime());
                nActualTimespan += Diff;
            }
            LastBlockTime = BlockReading->GetBlockTime();
            // Low Gravity chain support (Pre-fix)
            if(chainActive.Tip()->nHeight <= nlowGravity)
            {
                if (BlockReading->pprev == NULL) { assert(BlockReading); break; }
                    BlockReading = BlockReading->pprev;
            }
            // Low Gravity fix (PoW support)
            else if(chainActive.Tip()->nHeight > nlowGravity)
            {
                BlockReading = GetLastBlockIndex(BlockReading->pprev, fProofOfStake);
            }
        }

        arith_uint256 bnNew(PastDifficultyAverage);

        int64_t _nTargetTimespan = CountBlocks * Params().GetConsensus().nPowTargetSpacing;

        if (nActualTimespan < _nTargetTimespan/3)
            nActualTimespan = _nTargetTimespan/3;
        if (nActualTimespan > _nTargetTimespan*3)
            nActualTimespan = _nTargetTimespan*3;

        // Retarget
        bnNew *= nActualTimespan;
        bnNew /= _nTargetTimespan;

        if (bnNew > nProofOfWorkLimit){
            bnNew = nProofOfWorkLimit;
        }

        return bnNew.GetCompact();
}

// Define difficulty retarget algorithms
enum DiffMode {
    DIFF_DEFAULT = 0, // Default to invalid 0
    DIFF_VRX     = 1, // Retarget using Terminal-Velocity-RateX
};

unsigned int Terminal_Velocity_RateX(const CBlockIndex* pindexLast, bool fProofOfStake)
{
       // Terminal-Velocity-RateX, v10-Beta-R4, written by Jonathan Dan Zaretsky - cryptocoderz@gmail.com
       const arith_uint256 bnTerminalVelocity = fProofOfStake ? UintToArith256(Params().GetConsensus().posLimit) : UintToArith256(Params().GetConsensus().powLimit);
       // Define values
       double VLF1 = 0;
       double VLF2 = 0;
       double VLF3 = 0;
       double VLF4 = 0;
       double VLF5 = 0;
       double VLFtmp = 0;
       double VRFsm1 = 1;
       double VRFdw1 = 0.75;
       double VRFdw2 = 0.5;
       double VRFup1 = 1.25;
       double VRFup2 = 1.5;
       double VRFup3 = 2;
       double TerminalAverage = 0;
       double TerminalFactor = 10000;
       int64_t VLrate1 = 0;
       int64_t VLrate2 = 0;
       int64_t VLrate3 = 0;
       int64_t VLrate4 = 0;
       int64_t VLrate5 = 0;
       int64_t VLRtemp = 0;
       int64_t DSrateNRM = BLOCK_SPACING;
       int64_t DSrateMAX = BLOCK_SPACING_MAX;
       int64_t FRrateDWN = DSrateNRM - 60;
       int64_t FRrateFLR = DSrateNRM - 80;
       int64_t FRrateCLNG = DSrateMAX * 3;
       int64_t difficultyfactor = 0;
       int64_t AverageDivisor = 5;
       int64_t scanheight = 6;
       int64_t scanblocks = 1;
       int64_t scantime_1 = 0;
       int64_t scantime_2 = pindexLast->GetBlockTime();
       int64_t prevPoW = 0; // hybrid value
       int64_t prevPoS = 0; // hybrid value
       // Check for blocks to index | Allowing for initial chain start
       if (pindexLast->nHeight < scanheight+114)
           return bnTerminalVelocity.GetCompact(); // can't index prevblock
       // Set prev blocks...
       const CBlockIndex* pindexPrev = pindexLast;
       // ...and deduce spacing
       while(scanblocks < scanheight)
       {
           scantime_1 = scantime_2;
           pindexPrev = pindexPrev->pprev;
           scantime_2 = pindexPrev->GetBlockTime();
           // Set standard values
           if(scanblocks > 0){
               if     (scanblocks < scanheight-4){ VLrate1 = (scantime_1 - scantime_2); VLRtemp = VLrate1; }
               else if(scanblocks < scanheight-3){ VLrate2 = (scantime_1 - scantime_2); VLRtemp = VLrate2; }
               else if(scanblocks < scanheight-2){ VLrate3 = (scantime_1 - scantime_2); VLRtemp = VLrate3; }
               else if(scanblocks < scanheight-1){ VLrate4 = (scantime_1 - scantime_2); VLRtemp = VLrate4; }
               else if(scanblocks < scanheight-0){ VLrate5 = (scantime_1 - scantime_2); VLRtemp = VLrate5; }
           }
           // Round factoring
           if(VLRtemp >= DSrateNRM){ VLFtmp = VRFsm1;
               if(VLRtemp > DSrateMAX){ VLFtmp = VRFdw1;
                   if(VLRtemp > FRrateCLNG){ VLFtmp = VRFdw2; }
               }
           }
           else if(VLRtemp < DSrateNRM){ VLFtmp = VRFup1;
               if(VLRtemp < FRrateDWN){ VLFtmp = VRFup2;
                   if(VLRtemp < FRrateFLR){ VLFtmp = VRFup3; }
               }
           }
           // Record factoring
           if      (scanblocks < scanheight-4) VLF1 = VLFtmp;
           else if (scanblocks < scanheight-3) VLF2 = VLFtmp;
           else if (scanblocks < scanheight-2) VLF3 = VLFtmp;
           else if (scanblocks < scanheight-1) VLF4 = VLFtmp;
           else if (scanblocks < scanheight-0) VLF5 = VLFtmp;
           // Log hybrid block type
           if     (fProofOfStake) prevPoS ++;
           else if(!fProofOfStake) prevPoW ++;
           // move up per scan round
           scanblocks ++;
       }
       // Final mathematics
       TerminalAverage = (VLF1 + VLF2 + VLF3 + VLF4 + VLF5) / AverageDivisor;
       // Differentiate PoW/PoS prev block
       const CBlockIndex* BlockVelocityType = GetLastBlockIndex(pindexLast, fProofOfStake);
       // Skew for less selected block type
       int64_t nNow = GetAdjustedTime();
       int64_t nThen = 1493596800; // Toggle skew system fork - Mon, 01 May 2017 00:00:00 GMT
       if(nNow > nThen){if(prevPoW < prevPoS && !fProofOfStake){if((prevPoS-prevPoW) > 3) TerminalAverage /= 3;}
       else if(prevPoW > prevPoS && fProofOfStake){if((prevPoW-prevPoS) > 3) TerminalAverage /= 3;}
       if(TerminalAverage < 0.5) TerminalAverage = 0.5;} // limit skew to halving
       // Retarget
       arith_uint256 bnOld;
       arith_uint256 bnNew;
       TerminalFactor *= TerminalAverage;
       difficultyfactor = TerminalFactor;
       bnOld.SetCompact(BlockVelocityType->nBits);
       bnNew = bnOld / difficultyfactor;
       bnNew *= 10000;
       // Limit
       if (bnNew > bnTerminalVelocity)
         bnNew = bnTerminalVelocity;
       // Print for debugging
       // LogPrintf("Terminal-Velocity 1st spacing: %u: \n",VLrate1);
       // LogPrintf("Terminal-Velocity 2nd spacing: %u: \n",VLrate2);
       // LogPrintf("Terminal-Velocity 3rd spacing: %u: \n",VLrate3);
       // LogPrintf("Terminal-Velocity 4th spacing: %u: \n",VLrate4);
       // LogPrintf("Terminal-Velocity 5th spacing: %u: \n",VLrate5);
       // LogPrintf("Desired normal spacing: %u: \n",DSrateNRM);
       // LogPrintf("Desired maximum spacing: %u: \n",DSrateMAX);
       // LogPrintf("Terminal-Velocity 1st multiplier set to: %f: \n",VLF1);
       // LogPrintf("Terminal-Velocity 2nd multiplier set to: %f: \n",VLF2);
       // LogPrintf("Terminal-Velocity 3rd multiplier set to: %f: \n",VLF3);
       // LogPrintf("Terminal-Velocity 4th multiplier set to: %f: \n",VLF4);
       // LogPrintf("Terminal-Velocity 5th multiplier set to: %f: \n",VLF5);
       // LogPrintf("Terminal-Velocity averaged a final multiplier of: %f: \n",TerminalAverage);
       // LogPrintf("Prior Terminal-Velocity: %08x  %s\n", BlockVelocityType->nBits, bnOld.ToString());
       // LogPrintf("New Terminal-Velocity:  %08x  %s\n", bnNew.GetCompact(), bnNew.ToString());
       return bnNew.GetCompact();
}

unsigned int GetNextTargetRequired_legacy(const CBlockIndex* pindexLast, bool fProofOfStake)
{
    // Default with VRX
    unsigned int retarget = DIFF_VRX;

    // Check selection
    if (retarget != DIFF_VRX)
        //LogPrintf("GetNextTargetRequired() : Invalid retarget selection, using default \n");
        return Terminal_Velocity_RateX(pindexLast, fProofOfStake);

    // Retarget using Terminal-Velocity
    // debug info for testing
    //LogPrintf("Terminal-Velocity retarget selected \n");
    //LogPrintf("MarteX retargetted using: Terminal-Velocity difficulty curve \n");
    return Terminal_Velocity_RateX(pindexLast, fProofOfStake);

}

static unsigned int GetNextTargetRequired_new(const CBlockIndex* pindexLast, bool fProofOfStake)
{
    arith_uint256 bnTargetLimit = fProofOfStake ? UintToArith256(Params().GetConsensus().posLimit) : UintToArith256(Params().GetConsensus().powLimit);

    if (pindexLast == NULL)
        return bnTargetLimit.GetCompact(); // genesis block

    const CBlockIndex* pindexPrev = GetLastBlockIndex(pindexLast, fProofOfStake);
    if (pindexPrev->pprev == NULL)
        return bnTargetLimit.GetCompact(); // first block
    const CBlockIndex* pindexPrevPrev = GetLastBlockIndex(pindexPrev->pprev, fProofOfStake);
    if (pindexPrevPrev->pprev == NULL)
        return bnTargetLimit.GetCompact(); // second block

    int64_t nActualSpacing = pindexPrev->GetBlockTime() - pindexPrevPrev->GetBlockTime();
    if (nActualSpacing < 0)
        nActualSpacing = Params().GetConsensus().nPowTargetSpacing;

    // target change every block
    // retarget with exponential moving toward target spacing
    // Includes MartexCoin fix for wrong retargeting difficulty by Mammix2

    arith_uint256 bnNew;
    bnNew.SetCompact(pindexPrev->nBits);
    int64_t nInterval = Params().GetConsensus().nPowTargetTimespan / Params().GetConsensus().nPowTargetSpacing;
    bnNew *= ((nInterval - 1) * Params().GetConsensus().nPowTargetSpacing + nActualSpacing + nActualSpacing);
    bnNew /= ((nInterval + 1) * Params().GetConsensus().nPowTargetSpacing);

    if (bnNew <= 0 || bnNew > bnTargetLimit)
        bnNew = bnTargetLimit;

    return bnNew.GetCompact();
}

unsigned int GetNextTargetRequired(const CBlockIndex* pindexLast, bool fProofOfStake)
{
    int change;
    if( Params().NetworkIDString() == CBaseChainParams::TESTNET )
        change = 20;
    else
        change = 22500;

    if(pindexLast->nHeight + 1 > change){
        if ((Params().NetworkIDString() == CBaseChainParams::TESTNET) && pindexLast->nHeight + 1 < 198500){
            return OldDarkGravityWave(pindexLast, fProofOfStake);
        }else if ((Params().NetworkIDString() == CBaseChainParams::TESTNET) && pindexLast->nHeight + 1 > 252580){ //VRX Teste
            return Terminal_Velocity_RateX(pindexLast, fProofOfStake);
        }else if ((Params().NetworkIDString() == CBaseChainParams::MAIN) && pindexLast->nHeight >= Params().GetConsensus().nPowDGWHeight){
            return OldDarkGravityWave(pindexLast, fProofOfStake);
        }else{
            return GetNextTargetRequired_new(pindexLast, fProofOfStake);
        }
    }else{
      return GetNextTargetRequired_legacy(pindexLast, fProofOfStake);
    }

}
