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

#include "mem/cache/replacement_policies/arc_rp.hh"

#include <cassert>
#include <memory>

#include "params/ARCRP.hh"

ARCRP::ARCRP(const Params *p)
        : BaseReplacementPolicy(p),
        size(p->size),
        line_size(p->line_size),
        assoc(p->assoc)
{
        std::cout << "Cache size from ARCRP: " << size << std::endl;
        std::cout << "Cache line size from ARCRP: " << line_size << std::endl;
        std::cout << "Cache assoc from ARCRP: " << assoc << std::endl;

        num_blocks = size / line_size;
        std::cout << "Num blocks from ARCRP: " << num_blocks << std::endl;

        T1_size = num_blocks / 2;
        T2_size = num_blocks / 2;
}

void
ARCRP::invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
const
{
	// Reset last touch timestamp
	std::static_pointer_cast<ARCReplData>(
		replacement_data)->lastTouchTick = Tick(0);

	// Reset reference count
	std::static_pointer_cast<ARCReplData>(replacement_data)->refCount = 0;
}

void
ARCRP::touch(const std::shared_ptr<ReplacementData>& replacement_data) const
{
	// Update last touch timestamp
	std::static_pointer_cast<ARCReplData>(
		replacement_data)->lastTouchTick = curTick();

	// Update reference count
	std::static_pointer_cast<ARCReplData>(replacement_data)->refCount++;
}

void
ARCRP::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{	//new entry into cache
        //add to t1

}

ReplaceableEntry*
ARCRP::getVictim(const ReplacementCandidates& candidates) const
{
	// There must be at least one replacement candidate
	assert(candidates.size() > 0);

	// Visit all candidates to find victim
	ReplaceableEntry* victim = candidates[0];

	/* FIFO algorithm currently, this is where ARC needs to be implemented
	for (const auto& candidate : candidates) {
		// Update victim entry if necessary
		if (std::static_pointer_cast<ARCReplData>(
					candidate->replacementData)->tickInserted <
				std::static_pointer_cast<ARCReplData>(
					victim->replacementData)->tickInserted) {
			victim = candidate;
		}
	}
	*/
	return victim;
}

std::shared_ptr<ReplacementData>
ARCRP::instantiateEntry()
{
	return std::shared_ptr<ReplacementData>(new ARCReplData());
}

ARCRP*
ARCRPParams::create()
{
	return new ARCRP(this);
}
