
#include "Score.h"
#include "Param.h"

#include <algorithm>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
unsigned int Score::compute(const Template&, const Template&)
{
    // NJH-TODO compare entire templates...
    return 0;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// https://doi.org/10.3390/s120303418
//
unsigned int Score::compute(const Fingerprint& probe, const Fingerprint& candidate)
{
    // Local matching 5.1.1-3...
    thread_local static Triplet::Pairs pairs(50);
    pairs.clear();

    for (const auto& probeT : probe.triplets()) {
        findPairs(pairs, probeT, candidate);
    }
    std::sort(pairs.begin(), pairs.end(), [](const Triplet::Pair& p1, const Triplet::Pair& p2) { return p1.similarity() > p2.similarity(); });

    // 5.1.4-5...
    // NJH-TODO

    // Global matching 5.2...
    // NJH-TODO
    return 0;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Score::findPairs(Triplet::Pairs& pairs, const Triplet& probeT, const Fingerprint& candidate) const
{
    const auto& candidateT = candidate.triplets();
    auto it = std::lower_bound(candidateT.begin(), candidateT.end(), probeT.distances()[0] - Param::MaximumLocalDistance);
    const auto end = std::upper_bound(it, candidateT.end(), probeT.distances()[0] + Param::MaximumLocalDistance); // NJH-TODO profile these - possibly bake custom binary search

    for (; it < end; ++it) {
        auto pair = it->findPair(probeT);
        if (pair.similarity() > 0) {
            pairs.emplace_back(pair);
        }
    }
}
