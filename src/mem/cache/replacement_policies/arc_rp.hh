/**
 * Copyright (c) 2018 Inria
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Carvalho
 */

 /**
  * @file
  * Declaration of a First In First Out replacement policy.
  * The victim is chosen using the timestamp. The oldest entry is always chosen
  * to be evicted, regardless of the amount of times it has been touched.
  */

#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_ARC_RP_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_ARC_RP_HH__

#include <list>

#include "base/types.hh"
#include "mem/cache/replacement_policies/base.hh"

struct ARCRPParams;

class ARCRP : public BaseReplacementPolicy
{
protected:
	/** ARC-specific implementation of replacement data. */
	struct ARCReplData : ReplacementData
	{
		/** Tick on which the entry was last touched. */
		Tick lastTouchTick;

		/** Number of references to this entry since it was reset. */
		unsigned refCount;

		/**
		 * Default constructor. Invalidate data.
		 */
		ARCReplData() : lastTouchTick(0), refCount(0) {}
        };
private:
        std::list<ARCReplData*> T1, T2, B1, B2;
        uint64_t size;
        int line_size;
        int assoc;
        int num_blocks;
        int T1_size;
        int T2_size;
public:
	/** Convenience typedef. */
	typedef ARCRPParams Params;

	/**
	 * Construct and initiliaze this replacement policy.
	 */
	ARCRP(const Params *p);

	/**
	 * Destructor.
	 */
	~ARCRP() {}

	/**
	 * Invalidate replacement data to set it as the next probable victim.
	 * Reset insertion tick to 0.
	 *
	 * @param replacement_data Replacement data to be invalidated.
	 */
	void invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
		const override;

	/**
	 * Touch an entry to update its replacement data.
	 * Does not modify the replacement data.
	 *
	 * @param replacement_data Replacement data to be touched.
	 */
	void touch(const std::shared_ptr<ReplacementData>& replacement_data) const
		override;

	/**
	 * Reset replacement data. Used when an entry is inserted.
	 * Sets its insertion tick.
	 *
	 * @param replacement_data Replacement data to be reset.
	 */
	void reset(const std::shared_ptr<ReplacementData>& replacement_data) const
		override;

	/**
	 * Find replacement victim using insertion timestamps.
	 *
	 * @param cands Replacement candidates, selected by indexing policy.
	 * @return Replacement entry to be replaced.
	 */
	ReplaceableEntry* getVictim(const ReplacementCandidates& candidates) const
		override;

	/**
	 * Instantiate a replacement data entry.
	 *
	 * @return A shared pointer to the new replacement data.
	 */
	std::shared_ptr<ReplacementData> instantiateEntry() override;
};

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_ARC_RP_HH__
