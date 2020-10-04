
#include "Match.h"
#include "Param.h"

#include <algorithm>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
unsigned int Match::compute(const Template&, const Template&) const
{
    // NJH-TODO compare entire templates...
    return 0;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// https://doi.org/10.3390/s120303418
//
unsigned int Match::compute(const Fingerprint& probe, const Fingerprint& candidate) const
{
    m_pairs.clear();
    m_dupes.clear();

    const auto& candidateT = candidate.triplets();

    // Local matching 5.1.1-5...
    for (const auto& probeT : probe.triplets()) {
        auto it = std::lower_bound(candidateT.begin(), candidateT.end(), probeT.distances()[0] - Param::MaximumLocalDistance);
        const auto end = std::upper_bound(it, candidateT.end(), probeT.distances()[0] + Param::MaximumLocalDistance); // NJH-TODO profile these - possibly bake custom binary search

        for (; it < end; ++it) {
            it->emplacePair(m_pairs, m_dupes, probeT);
        }
    }
    if (m_pairs.size() < Param::MinimumMinutiae) {
        return 0;
    }
    std::sort(m_pairs.begin(), m_pairs.end());

    // Global matching 5.2...
    // NJH-TODO
    return 0;
}
