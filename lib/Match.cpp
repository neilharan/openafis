
#include "Match.h"
#include "FastMath.h"
#include "Log.h"
#include "Param.h"

#include <algorithm>
#include <set>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T> void Match<T>::compute(const Template&, const Template&) const
{
    // NJH-TODO compare entire templates...
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// https://doi.org/10.3390/s120303418
//
template <typename T> void Match<T>::compute(T& result, const Fingerprint& probe, const Fingerprint& candidate) const
{
    m_pairs.clear();
    m_dupes.first.fill(0);
    m_dupes.second.fill(0);

    const auto& candidateT = candidate.triplets();
    unsigned int oust{};

    // Local matching 5.1.1-5...
    for (const auto& probeT : probe.triplets()) {
        auto it = std::lower_bound(candidateT.begin(), candidateT.end(), probeT.distances()[0] - Param::MaximumLocalDistance);
        const auto end = std::upper_bound(it, candidateT.end(), probeT.distances()[0] + Param::MaximumLocalDistance); // NJH-TODO profile these - possibly bake custom binary search

        for (; it < end; ++it) {
            probeT.emplacePair(m_pairs, m_dupes, oust, *it);
        }
    }
    if (m_pairs.size() < Param::MinimumMinutiae) {
        return;
    }
    std::sort(m_pairs.begin(), m_pairs.end());
    for(; oust; --oust) {
        m_pairs.pop_back();
    }

    // Global matching 5.2...
    auto maxMatched = 0;

    for (const auto& p1 : m_pairs) {
        const auto theta = p1.candidate()->angle() - p1.probe()->angle();
        const auto cosTheta = std::cos(theta); // NJH-TODO use tables, domain can be determined
        const auto sinTheta = std::sin(theta);

        auto matched = 1;
        auto min = m_pairs.size() - 1;

        for (const auto& p2 : m_pairs) {
            if (p1.probe() == p2.probe() || p1.candidate() == p2.candidate()) {
                continue;
            }
            if (matched + --min < Param::MinimumMinutiae) {
                break;
            }

            // 5.2.2...
            const auto length = [&]() {
                const auto x
                    = p1.candidate()->x() + FastMath::round(cosTheta * static_cast<float>(p2.probe()->x() - p1.probe()->x()) - sinTheta * static_cast<float>(p2.probe()->y() - p1.probe()->y()));
                const auto y
                    = p1.candidate()->y() + FastMath::round(sinTheta * static_cast<float>(p2.probe()->x() - p1.probe()->x()) + cosTheta * static_cast<float>(p2.probe()->y() - p1.probe()->y()));

                const auto a = x - p2.candidate()->x();
                const auto b = y - p2.candidate()->y();
                const auto c = a * a + b * b;
                if (c > Param::MaximumGlobalDistance * Param::MaximumGlobalDistance) {
                    return false;
                }
                return FastMath::isqrt(c) <= Param::MaximumGlobalDistance;
            }();
            if (!length) {
                continue;
            }

            const auto directions = [&]() {
                auto a = p2.probe()->angle() + theta;
                if (a > FastMath::PI2) { // wrap within range
                    a -= FastMath::PI2;
                } else if (a < 0) {
                    a += FastMath::PI2;
                }
                return FastMath::minimumAngle(a, p2.candidate()->angle()) <= Param::MaximumDirectionDifference;
            }();
            if (!directions) {
                continue;
            }

            const auto anglesBeta = [&]() {
                const auto p = FastMath::rotateAngle(p1.probe()->angle(), p2.probe()->angle());
                const auto c = FastMath::rotateAngle(p1.candidate()->angle(), p2.candidate()->angle());
                return FastMath::minimumAngle(p, c) <= Param::MaximumAngleDifference;
            }();
            if (!anglesBeta) {
                continue;
            }
            matched++;

            // When this class is specialized for rendering only - no overhead when computing similarities...
            if constexpr (std::is_same<T, Pair::Set>::value) {
                result.insert(&p2);
            }
        }
        maxMatched = std::max(maxMatched, matched);
    }
    if constexpr (std::is_same<T, unsigned int>::value) {
        result = FastMath::round(static_cast<float>(maxMatched * maxMatched) / static_cast<float>(probe.minutiaeCount() * candidate.minutiaeCount()) * 100.0f);
    }
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template class Match<unsigned int>;
template class Match<Pair::Set>;
