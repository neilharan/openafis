
#include "Match.h"
#include "Param.h"

#include <algorithm>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
unsigned int Match::compute(const Template&, const Template&)
{
    // NJH-TODO compare entire templates...
    return 0;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// https://doi.org/10.3390/s120303418
//
unsigned int Match::compute(const Fingerprint& probe, const Fingerprint& candidate)
{
    // Local matching 5.1.1-3...
    thread_local static Triplet::Pairs pairs;
    pairs.reserve(50);
    pairs.clear();

    const auto& candidateT = candidate.triplets();

    for (const auto& probeT : probe.triplets()) {
        auto it = std::lower_bound(candidateT.begin(), candidateT.end(), probeT.distances()[0] - Param::MaximumLocalDistance);
        const auto end = std::upper_bound(it, candidateT.end(), probeT.distances()[0] + Param::MaximumLocalDistance); // NJH-TODO profile these - possibly bake custom binary search

        for (; it < end; ++it) {
            it->emplacePair(pairs, probeT);
        }
    }
    std::sort(pairs.begin(), pairs.end());

    // 5.1.4-5...
    // NJH-TODO

    // Global matching 5.2...
    // NJH-TODO
    return 0;
}
