
#include "Match.h"
#include "FastMath.h"
#include "Log.h"
#include "Param.h"

#include <algorithm>
#include <set>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Match<T>::compute(const Template&, const Template&) const
{
    // NJH-TODO compare entire templates...
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// https://doi.org/10.3390/s120303418
//
template <typename T>
void Match<T>::compute(T& result, const Fingerprint& probe, const Fingerprint& candidate) const
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
        return;
    }
    std::sort(m_pairs.begin(), m_pairs.end());

    // Global matching 5.2...
    auto maxMatched = 0;

    for (const auto& p1 : m_pairs) {
        const auto theta = p1.candidate()->angle() - p1.probe()->angle();
        const auto cosTheta = std::cos(theta); // NJH-TODO use tables, domain can be determined
        const auto sinTheta = std::sin(theta);

        auto matched = 0;
        auto min = m_pairs.size();

        for (const auto& p2 : m_pairs) {
            if (p1.probe() == p2.probe() || p1.candidate() == p2.candidate()) {
                continue;
            }
            // 5.2.2...
            const auto x = p1.candidate()->x() + std::lround(cosTheta * static_cast<float>(p2.probe()->x() - p1.probe()->x()) - sinTheta * static_cast<float>(p2.probe()->y() - p1.probe()->y()));
            const auto y = p1.candidate()->y() + std::lround(sinTheta * static_cast<float>(p2.probe()->x() - p1.probe()->x()) + cosTheta * static_cast<float>(p2.probe()->y() - p1.probe()->y()));
            // NJH-TODO compare angles too

            const auto distance = [&]() {
                const auto a = x - p2.candidate()->x();
                const auto b = y - p2.candidate()->y();
                const auto c = a * a + b * b;
                return c > std::numeric_limits<Field::MinutiaCoordType>::max() ? Param::MaximumGlobalDistance + 1 : static_cast<Field::MinutiaCoordType>(FastMath::isqrt(c));
            };

            if (distance() <= Param::MaximumGlobalDistance) {
                if constexpr(std::is_same<T, MinutiaPoint::RenderPair>::value) {
                    if (matched == 0) {
                        result.first.push_back(p2.probe());
                        result.second.push_back(p2.candidate());
                        break;
                    }
                }
                matched++;
            }
            if (matched + --min < Param::MinimumMinutiae) {
                break;
            }
        }
        maxMatched = std::max(maxMatched, matched);
    }
    if constexpr(std::is_same<T, unsigned int>::value) {
        result = std::lround(static_cast<float>(maxMatched * maxMatched) / static_cast<float>(probe.minutiaeCount() * candidate.minutiaeCount()) * 100.0f);
    }
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template class Match<unsigned int>;
template class Match<MinutiaPoint::RenderPair>;
