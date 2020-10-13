
#include "Match.h"
#include "FastMath.h"
#include "Param.h"

#include <algorithm>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// https://doi.org/10.3390/s120303418
//
template <class R, class F, class P> void Match<R, F, P>::compute(R& result, const F& probe, const F& candidate) const
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

    for (const auto& p : m_tripletPairs) {
        for (decltype(p.probe()->minutiae().size()) i = 0; i < p.probe()->minutiae().size(); ++i) {
            bool z {};
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
            if constexpr (std::is_same_v<R, MinutiaPoint::PairRenderable::Set>) {
                m_pairs.emplace_back(&p.probe()->minutiae()[i], &p.candidate()->minutiae()[i], p.similarity());
            } else {
                m_pairs.emplace_back(&p.probe()->minutiae()[i], &p.candidate()->minutiae()[i]);
            }
        }
    }

    // Global matching 5.2...
    auto maxMatched = 0;

    for (const auto& p1 : m_pairs) {
        const auto theta = p1.candidate()->angle() - p1.probe()->angle();
        const auto cosTheta = FastMath::cos(theta);
        const auto sinTheta = FastMath::sin(theta);

        auto matched = 1;
        auto min = m_pairs.size() - 1;

        for (const auto& p2 : m_pairs) {
            if (&p1 == &p2) {
                continue;
            }

            // 5.2.2...
            const auto lengths = [&]() {
                const auto x
                    = p1.candidate()->x() + cosTheta * static_cast<float>(p2.probe()->x() - p1.probe()->x()) - sinTheta * static_cast<float>(p2.probe()->y() - p1.probe()->y());
                const auto y
                    = p1.candidate()->y() + sinTheta * static_cast<float>(p2.probe()->x() - p1.probe()->x()) + cosTheta * static_cast<float>(p2.probe()->y() - p1.probe()->y());

                const auto a = x - p2.candidate()->x();
                const auto b = y - p2.candidate()->y();
                const auto c = a * a + b * b;
                return c <= Param::MaximumGlobalDistance * Param::MaximumGlobalDistance;
            }();
            if (!lengths) {
                continue;
            }

            const auto directions = [&]() {
                auto a = p2.probe()->angle() + theta;

                if constexpr (std::is_same_v<Field::AngleType, float>) {
                    if (a > FastMath::TwoPI) { // wrap within range
                        a -= FastMath::TwoPI;
                    } else if (a < 0) {
                        a += FastMath::TwoPI;
                    }
                }
                if constexpr (std::is_same_v<Field::AngleType, float>) {
                    if (a > FastMath::TwoPI8) { // wrap within range
                        a -= FastMath::TwoPI8;
                    } else if (a < 0) {
                        a += FastMath::TwoPI8;
                    }
                }
                return FastMath::minimumAngle(a, p2.candidate()->angle()) <= Param::maximumDirectionDifference();
            }();
            if (!directions) {
                continue;
            }

            const auto anglesBeta = [&]() {
                const auto p = FastMath::rotateAngle(p1.probe()->angle(), p2.probe()->angle());
                const auto c = FastMath::rotateAngle(p1.candidate()->angle(), p2.candidate()->angle());
                return FastMath::minimumAngle(p, c) <= Param::maximumAngleDifference();
            }();
            if (!anglesBeta) {
                continue;
            }
            matched++;
            if (matched + --min < Param::MinimumMinutiae) {
                break;
            }

            // When this class is specialized for rendering only - no overhead when computing similarities...
            if constexpr (std::is_same_v<R, MinutiaPoint::PairRenderable::Set>) {
                result.insert(&p2);
            }
        }
        maxMatched = std::max(maxMatched, matched);
    }
    if constexpr (std::is_same_v<R, unsigned int>) {
        result = static_cast<unsigned int>(static_cast<float>(maxMatched * maxMatched) / static_cast<float>(probe.minutiaeCount() * candidate.minutiaeCount()) * 100.0f);
    }
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template class Match<unsigned int, Fingerprint, MinutiaPoint::Pair>;
template class Match<MinutiaPoint::PairRenderable::Set, FingerprintRenderable, MinutiaPoint::PairRenderable>;
}
