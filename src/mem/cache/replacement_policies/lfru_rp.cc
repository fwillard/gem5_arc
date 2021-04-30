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

#include "mem/cache/replacement_policies/lfru_rp.hh"

#include <cassert>
#include <memory>

#include "params/LFRURP.hh"

LFRURP::LFRURP(const Params *p)
        : BaseReplacementPolicy(p)
{

}

void
LFRURP::invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
const
{
    // Reset all values
    std::shared_ptr<LFRUReplData> data =
    std::static_pointer_cast<LFRUReplData>(replacement_data);
    data->privileged = false;
    data->lastTouchTick = Tick(0);
    data->refCount = 0;
}

void
LFRURP::touch(const std::shared_ptr<ReplacementData>& replacement_data)
const
{
    //update data and if accessed more than two times add to priv
    std::shared_ptr<LFRUReplData> data =
    std::static_pointer_cast<LFRUReplData>(replacement_data);
    data->lastTouchTick = curTick();
    data->refCount++;
    if (data->refCount > 2){
        data->privileged = true;
    }

}

void
LFRURP::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{	//new entry into cache
    std::shared_ptr<LFRUReplData> data =
    std::static_pointer_cast<LFRUReplData>(replacement_data);
    data->lastTouchTick = curTick();
    data->refCount = 1;
    data->privileged = false;
}

ReplaceableEntry*
LFRURP::getVictim(const ReplacementCandidates& candidates) const
{
        // There must be at least one replacement candidate
        assert(candidates.size() > 0);

        // Visit all candidates to find victim
        ReplaceableEntry* victim = candidates[0];
    bool priv_full = true;
        //only evict un-privileged blocks. LFU for unprivlidged partition
        for (const auto& candidate : candidates) {
        // Update victim entry if necessary
        std::shared_ptr<LFRUReplData> victim_data =
        std::static_pointer_cast<LFRUReplData>(
            victim->replacementData);
        std::shared_ptr<LFRUReplData> data =
        std::static_pointer_cast<LFRUReplData>(
            candidate->replacementData);
            if (!data->privileged){
                priv_full = false;
                if (data->refCount < victim_data->refCount){
                    victim = candidate;
                }
            }
        }

    //if all entries are in the priviledged list
    //evict the least recently used
    if (priv_full){
        for (const auto& candidate : candidates) {
                // Update victim entry if necessary
            std::shared_ptr<LFRUReplData> victim_data =
            std::static_pointer_cast<LFRUReplData>(
                victim->replacementData);
            std::shared_ptr<LFRUReplData> data =
            std::static_pointer_cast<LFRUReplData>(
                candidate->replacementData);
            if (data->lastTouchTick < victim_data->lastTouchTick){
                victim = candidate;
            }
        }
    }

        return victim;
}

std::shared_ptr<ReplacementData>
LFRURP::instantiateEntry()
{
        return std::shared_ptr<ReplacementData>(new LFRUReplData());
}

LFRURP*
LFRURPParams::create()
{
        return new LFRURP(this);
}
