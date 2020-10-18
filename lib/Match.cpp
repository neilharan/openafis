
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
    const auto& probeT = probe.triplets();
    const auto& candidateT = candidate.triplets();
    m_tripletPairs.clear();

    // Local matching 5.1.1-2...
    for (const auto& p : probeT) {
        auto it = std::lower_bound(candidateT.begin(), candidateT.end(), p.distances()[0] - Param::MaximumLocalDistance);
        const auto end = std::upper_bound(it, candidateT.end(), p.distances()[0] + Param::MaximumLocalDistance); // NJH-TODO profile these - possibly bake custom binary search

        for (; it < end; ++it) {
            it->emplacePair(m_tripletPairs, p);
        }
    }
    if (m_tripletPairs.size() < Param::MinimumMinutiae) {
        return;
    }

    // Local matching 5.1.3-5...
    std::sort(m_tripletPairs.begin(), m_tripletPairs.end());
    m_dupes.first.fill(false);
    m_dupes.second.fill(false);
    m_pairs.clear();

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
                // Similarity values are scaled for integers over [0,1000], for render % is fine...
                m_pairs.emplace_back(&p.probe()->minutiae()[i], &p.candidate()->minutiae()[i], std::lround(static_cast<float>(p.similarity()) / 10.0f));
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
                const auto x = p1.candidate()->x() + cosTheta * (p2.probe()->x() - p1.probe()->x()) - sinTheta * (p2.probe()->y() - p1.probe()->y());
                const auto y = p1.candidate()->y() + sinTheta * (p2.probe()->x() - p1.probe()->x()) + cosTheta * (p2.probe()->y() - p1.probe()->y());
                const auto a = x - p2.candidate()->x();
                const auto b = y - p2.candidate()->y();
                const auto c = a * a + b * b;
                return c <= Param::MaximumGlobalDistance * Param::MaximumGlobalDistance;
            }();
            if (!lengths) {
                continue;
            }

            const auto directions = [&]() {
                const auto a = [&]() {
                    // Wrap within range ...
                    if constexpr (std::is_same_v<Field::AngleType, float>) {
                        const auto s = p2.probe()->angle() + theta;

                        if (s > FastMath::TwoPI) {
                            return s - FastMath::TwoPI;
                        }
                        if (s < 0) {
                            return s + FastMath::TwoPI;
                        }
                        return s;
                    }
                    if constexpr (std::is_same_v<Field::AngleType, int>) {
                        return static_cast<uint8_t>(p2.probe()->angle() + theta);
                    }
                };
                return FastMath::minimumAngle(a(), p2.candidate()->angle()) <= Param::maximumDirectionDifference();
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
        if (maxMatched > Param::MinimumMinutiae) {
            result = (maxMatched * maxMatched * 100) / static_cast<R>(probe.minutiaeCount() * candidate.minutiaeCount());
        }
    }
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Explicit instantiations...
//
template class Match<unsigned int, Fingerprint, MinutiaPoint::Pair>;
template class Match<MinutiaPoint::PairRenderable::Set, FingerprintRenderable, MinutiaPoint::PairRenderable>;
}
