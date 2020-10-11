
#include "Match.h"
#include "FastMath.h"
#include "Log.h"
#include "Param.h"

#include <algorithm>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T> void Match<T>::compute(const Template&, const Template&) const
{
    // NJH-TODO compare entire templates...
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// https://doi.org/10.3390/s120303418
//
#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
#define SIMILARITY , p.similarity()
#else
#define SIMILARITY
#endif

template <typename T> void Match<T>::compute(T& result, const Fingerprint& probe, const Fingerprint& candidate) const
{
    m_tripletPairs.clear();
    m_dupes.first.fill(false);
    m_dupes.second.fill(false);
    m_pairs.clear();

    const auto& candidateT = candidate.triplets();

    // Local matching 5.1.1-2...
    for (const auto& probeT : probe.triplets()) {
        auto it = std::lower_bound(candidateT.begin(), candidateT.end(), probeT.distances()[0] - Param::MaximumLocalDistance);
        const auto end = std::upper_bound(it, candidateT.end(), probeT.distances()[0] + Param::MaximumLocalDistance); // NJH-TODO profile these - possibly bake custom binary search

        for (; it < end; ++it) {
            probeT.emplacePair(m_tripletPairs, *it);
        }
    }
    if (m_tripletPairs.size() < Param::MinimumMinutiae) {
        return;
    }

    // Local matching 5.1.3-5...
    std::sort(m_tripletPairs.begin(), m_tripletPairs.end());

    for (const auto &p : m_tripletPairs) {
        for (decltype(p.probe()->minutiae().size()) i = 0; i < p.probe()->minutiae().size(); ++i) {
            bool z{};
            auto& dp = m_dupes.first[p.probe()->minutiae()[i].key()];
            if (!dp) {
                dp = true;
                z = true;
            }
            auto& dc = m_dupes.second[p.candidate()->minutiae()[i].key()];
            if (!dc) {
                dc = true;
            } else if (!z) {
                continue;
            }
            m_pairs.emplace_back(&p.probe()->minutiae()[i], &p.candidate()->minutiae()[i] SIMILARITY);
        }
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
            if (&p1 == &p2) {
                continue;
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
                return c <= Param::MaximumGlobalDistance * Param::MaximumGlobalDistance;
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
            if (matched + --min < Param::MinimumMinutiae) {
                break;
            }

            // When this class is specialized for rendering only - no overhead when computing similarities...
            if constexpr (std::is_same<T, MinutiaPoint::Pair::Set>::value) {
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
template class Match<MinutiaPoint::Pair::Set>;
