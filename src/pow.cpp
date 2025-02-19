// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <primitives/block.h>
#include <uint256.h>

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

	if((pindexLast->nHeight+1) == params.n2023Height) return params.n2023Bits;
	if((pindexLast->nHeight+1) == params.n2023Height2) return params.n2023Bits2;
    if((pindexLast->nHeight+1) == params.nFlexhashHeight) return params.nFlexhashBits;

	if((pindexLast->nHeight+1) < params.n2023Height) {
		// Only change once per difficulty adjustment interval
		if ((pindexLast->nHeight+1) % params.DifficultyAdjustmentInterval() != 0)
		{
			if (params.fPowAllowMinDifficultyBlocks)
			{
				// Special difficulty rule for testnet:
				// If the new block's timestamp is more than 2* 10 minutes
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
	} else if((pindexLast->nHeight+1) < params.n2023Height2) {
		// Only change once per difficulty adjustment interval
		if ((pindexLast->nHeight+1) % params.n2023Window != 0)
		{
			if (params.fPowAllowMinDifficultyBlocks)
			{
				// Special difficulty rule for testnet:
				// If the new block's timestamp is more than 2* 10 minutes
				// then allow mining of a min-difficulty block.
				if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
					return nProofOfWorkLimit;
				else
				{
					// Return the last non-special-min-difficulty-rules-block
					const CBlockIndex* pindex = pindexLast;
					while (pindex->pprev && pindex->nHeight % params.n2023Window != 0 && pindex->nBits == nProofOfWorkLimit)
						pindex = pindex->pprev;
					return pindex->nBits;
				}
			}
			return pindexLast->nBits;
		}
	} else {
		if (params.fPowAllowMinDifficultyBlocks)
		{
			if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
				return nProofOfWorkLimit;
			else
			{
				const CBlockIndex* pindex = pindexLast;
				pindex = pindex->pprev;
				return pindex->nBits;
			}
		}
	}

    int nHeightFirst;
    if((pindexLast->nHeight+1) < params.n2023Height) {
        // Go back by what we want to be 14 days worth of blocks
        nHeightFirst = pindexLast->nHeight - (params.DifficultyAdjustmentInterval()-1);
    } else if((pindexLast->nHeight+1) < params.n2023Height2) {
        nHeightFirst = pindexLast->nHeight - (params.n2023Window-1);
    } else {
        nHeightFirst = pindexLast->nHeight - 1;
	}

    assert(nHeightFirst >= 0);
    const CBlockIndex* pindexFirst = pindexLast->GetAncestor(nHeightFirst);
    assert(pindexFirst);

    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
	if((pindexLast->nHeight+1) < params.n2023Height) {
		if (nActualTimespan < params.nPowTargetTimespan/4)
			nActualTimespan = params.nPowTargetTimespan/4;
		if (nActualTimespan > params.nPowTargetTimespan*4)
			nActualTimespan = params.nPowTargetTimespan*4;
	} else if((pindexLast->nHeight+1) < params.n2023Height2) {
		if (nActualTimespan < params.n2023Timespan/1.014)
			nActualTimespan = params.n2023Timespan/1.014;
		if (nActualTimespan > params.n2023Timespan*1.014)
			nActualTimespan = params.n2023Timespan*1.014;
	} else {
        if (nActualTimespan < 37)
            nActualTimespan = 37;
        if (nActualTimespan > 39)
            nActualTimespan = 39;
	}

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew;

    // Special difficulty rule for Testnet4
    if (params.enforce_BIP94) {
        // Here we use the first block of the difficulty period. This way
        // the real difficulty is always preserved in the first block as
        // it is not allowed to use the min-difficulty exception.
        int nHeightFirst = pindexLast->nHeight - (params.DifficultyAdjustmentInterval()-1);
        const CBlockIndex* pindexFirst = pindexLast->GetAncestor(nHeightFirst);
        bnNew.SetCompact(pindexFirst->nBits);
    } else {
        bnNew.SetCompact(pindexLast->nBits);
    }

    bnNew *= nActualTimespan;
    if((pindexLast->nHeight+1) < params.n2023Height) {
        bnNew /= params.nPowTargetTimespan;
    } else if((pindexLast->nHeight+1) < params.n2023Height2) {
        bnNew /= params.n2023Timespan;
    } else {
        bnNew /= 38;
	}

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

// Check that on difficulty adjustments, the new difficulty does not increase
// or decrease beyond the permitted limits.
bool PermittedDifficultyTransition(const Consensus::Params& params, int64_t height, uint32_t old_nbits, uint32_t new_nbits, uint32_t old_ntime, uint32_t new_ntime, int32_t old_nversion, int32_t new_nversion)
{
    if (params.fPowAllowMinDifficultyBlocks) return true;

	if(height == params.n2023Height) return params.n2023Bits == new_nbits;
	if(height == params.n2023Height2) return params.n2023Bits2 == new_nbits;
    if(height >= params.nFlexhashHeight && !(new_nversion & 0x8000)) return false;
    if(height == params.nFlexhashHeight) return params.nFlexhashBits == new_nbits;

	int64_t smallest_timespan;
	int64_t largest_timespan;
	int64_t timespan;
	bool fDiffChange = false;

	if(height < params.n2023Height) {
		if (height % params.DifficultyAdjustmentInterval() == 0) {
			fDiffChange = true;
			timespan = params.nPowTargetTimespan;
			smallest_timespan = timespan/4;
			largest_timespan = timespan*4;
		}
	} else if (height < params.n2023Height2) {
		if (height % params.n2023Window == 0) {
			fDiffChange = true;
			timespan = params.n2023Timespan;
			smallest_timespan = timespan/1.014;
			largest_timespan = timespan*1.014;
		}
	} else {
		fDiffChange = true;
		timespan = 38;
		smallest_timespan = 37;
		largest_timespan = 39;
	}

    if (fDiffChange) {

        const arith_uint256 pow_limit = UintToArith256(params.powLimit);
        arith_uint256 observed_new_target;
        observed_new_target.SetCompact(new_nbits);

        // Calculate the largest difficulty value possible:
        arith_uint256 largest_difficulty_target;
        largest_difficulty_target.SetCompact(old_nbits);
        largest_difficulty_target *= largest_timespan;
        largest_difficulty_target /= timespan;

        if (largest_difficulty_target > pow_limit) {
            largest_difficulty_target = pow_limit;
        }

        // Round and then compare this new calculated value to what is
        // observed.
        arith_uint256 maximum_new_target;
        maximum_new_target.SetCompact(largest_difficulty_target.GetCompact());
        if (maximum_new_target < observed_new_target) return false;

        // Calculate the smallest difficulty value possible:
        arith_uint256 smallest_difficulty_target;
        smallest_difficulty_target.SetCompact(old_nbits);
        smallest_difficulty_target *= smallest_timespan;
        smallest_difficulty_target /= timespan;

        if (smallest_difficulty_target > pow_limit) {
            smallest_difficulty_target = pow_limit;
        }

        // Round and then compare this new calculated value to what is
        // observed.
        arith_uint256 minimum_new_target;
        minimum_new_target.SetCompact(smallest_difficulty_target.GetCompact());
        if (minimum_new_target > observed_new_target) return false;
    } else if (old_nbits != new_nbits) {
        return false;
    }
    return true;
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
