// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "blocksizecalculator.h"

using namespace BlockSizeCalculator;
using namespace std;

static std::vector<unsigned int> blocksizes;
static bool sorted = false;
static unsigned int last_height_seen = 0;
static unsigned int last_result_returned = 0;

unsigned int BlockSizeCalculator::ComputeBlockSize(CBlockIndex *pblockindex, unsigned int pastblocks) {

	unsigned int proposedMaxBlockSize = 0;
	unsigned int result = MIN_BLOCK_SIZE;

	LOCK(cs_main);

	proposedMaxBlockSize = ::GetMedianBlockSize(pblockindex, pastblocks);

	if (proposedMaxBlockSize > 0) {
		//Absolute max block size will be 2^32-1 bytes due to the fact that unsigned int's are 4 bytes
		result = proposedMaxBlockSize * MAX_BLOCK_SIZE_INCREASE_MULTIPLE;
		result = result < proposedMaxBlockSize ?
				std::numeric_limits<unsigned int>::max() :
				result;
        if (result < MIN_BLOCK_SIZE) {
            //LogPrintf("ABS: in ComputeBlockSize limiting result (%u) to historic block size limit (%u)\n", result, MIN_BLOCK_SIZE);
            result = MIN_BLOCK_SIZE;
		}
	}
	LogPrintf("ABS - Block size calculed %u \n", result);

	return result;

}

inline unsigned int BlockSizeCalculator::GetMedianBlockSize(
	CBlockIndex *pblockindex, unsigned int pastblocks) {

	blocksizes = ::GetBlockSizes(pblockindex, pastblocks);

	if(!sorted) {
		std::sort(blocksizes.begin(), blocksizes.end());
		sorted = true;
	}

	unsigned int vsize = blocksizes.size();
	LogPrintf("ABS: vsize = %u\n", vsize);
	if (vsize == pastblocks) {
		double median = 0;
		if ((vsize % 2) == 0) {
			median = (blocksizes[vsize / 2] + blocksizes[(vsize / 2) - 1]) / 2.0;
		} else {
			median = blocksizes[vsize / 2];
		}
		//LogPrintf("ABS: GetMedianBlockSize = %u\n", static_cast<unsigned int>(floor(median)));
		return static_cast<unsigned int>(floor(median));
	} else {
		return 0;
	}

}

void BlockSizeCalculator::ClearBlockSizes() {
    blocksizes.clear();
    sorted = false;
}

inline std::vector<unsigned int> BlockSizeCalculator::GetBlockSizes(
		CBlockIndex *pblockindex, unsigned int pastblocks) {

	if (pblockindex->nHeight < (int)pastblocks) {
		//LogPrintf("ABS: GetBlockSizes: nHeight (%u) < pastblocks, returning blocksizes unchanged\n", pblockindex->nHeight);
		return blocksizes;
	}

	int firstBlock = pblockindex->nHeight - pastblocks;
	//LogPrintf("ABS: GetBlockSizes: firstBlock = %u\n", firstBlock);

	if (blocksizes.size() > 0) {
		//LogPrintf("ABS: GetBlockSizes: blocksizes.size() > 0 (%u)\n", blocksizes.size());
		int latestBlockSize = ::GetBlockSize(pblockindex);

		if (latestBlockSize != -1) {

			CBlockIndex *firstBlockIndex = FindBlockByHeight(firstBlock);
			int oldestBlockSize = ::GetBlockSize(firstBlockIndex);
			if (oldestBlockSize != -1) {
				std::vector<unsigned int>::iterator it;
				it = std::find(blocksizes.begin(), blocksizes.end(),
						oldestBlockSize);
				if (it != blocksizes.end()) {
					blocksizes.erase(it);
					it = std::lower_bound(blocksizes.begin(), blocksizes.end(),
							latestBlockSize);
					blocksizes.insert(it, latestBlockSize);
					//LogPrintf("ABS: GetBlockSizes: inserting latest size %u\n", latestBlockSize);
				}
		                else {
		                    //LogPrintf("ABS: GetBlockSizes: find yielded end of vector, not inserting latest size %u\n", latestBlockSize);
		                }
			}

		}

	} else {
		//LogPrintf("ABS: GetBlockSizes: blocksizes.size() == 0, pushing back\n");
		while (pblockindex != NULL && pblockindex->nHeight > firstBlock) {

			int blocksize = ::GetBlockSize(pblockindex);
			if (blocksize != -1) {
				blocksizes.push_back(blocksize);
				//LogPrintf("ABS: pushback: %u\n", blocksize);
			}
		        else {
	                	//LogPrintf("ABS: GetBlockSizes: not pushing back block at height %u because GetBlockSize = -1 (strange)\n", pblockindex->nHeight);
		        }
			pblockindex = pblockindex->pprev;
		}

	}

	return blocksizes;

}

inline int BlockSizeCalculator::GetBlockSize(CBlockIndex *pblockindex) {

	if (pblockindex == NULL) {
		//LogPrintf("ABS: GetBlockSize: returning -1 because pblockindex is NULL\n");
		return -1;
	}

	const CDiskBlockPos& pos = pblockindex->GetBlockPos();

	CAutoFile filein(OpenBlockFile(pos, false), SER_DISK, CLIENT_VERSION);

	/*if (filein.IsNull()) {
	        LogPrintf("ABS: GetBlockSize: returning -1 because filein.IsNull\n");
		return -1;
	}*/

	uint32_t size = 0;
	FILE* blockFile = filein.release();
	long int filePos = ftell(blockFile);
	fseek(blockFile, filePos - sizeof(uint32_t), SEEK_SET);

	// ABS BSZ, CLN begin
	// added evaluation of fread() return value
	size_t items_read = 0;
	items_read = fread(&size, sizeof(uint32_t), 1, blockFile);
	fclose(blockFile);
	if (items_read != 1) {
	    //LogPrintf("ABS BSZ: GetBlockSize: returning -1 because items_read != 1\n");
	    return -1;
	}
	else {
	    return (unsigned int) size;
	}
}

// ABS BSZ added function to scale soft limit. Currently stubbed
// to return the configured soft limit without actually scaling.
uint32_t BlockSizeCalculator::ComputeScaledBlockMaxSize(uint32_t nBlockSizeMax, unsigned int maxBlockSize)
{
    uint32_t scaledSoftLimit = nBlockSizeMax;

    // ABS TODO: perform actual scaling calculation, not just return the unscaled soft value
    return scaledSoftLimit;
}
