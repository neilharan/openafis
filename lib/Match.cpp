
#include "Log.h"
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
        auto it = std::lower_bound(candidateT.begin(), candidateT.end(), p.maxDistance() - Param::MaximumLocalDistance);
        if (it == candidateT.end()) {
            continue;
        }
        const auto end = std::upper_bound(it, candidateT.end(), p.maxDistance() + Param::MaximumLocalDistance);

        for (; it < end; ++it) {
            if (!it->skipPair(p)) {
                it->emplacePair(m_tripletPairs, p);
            }
        }
    }
    if (m_tripletPairs.size() < Param::MinimumMinutiae) {
        return;
    }

    // Local matching 5.1.3-5...
    std::sort(m_tripletPairs.begin(), m_tripletPairs.end());
    m_probeDupes.clear();
    m_candidateDupes.clear();
    m_pairs.clear();

    for (const auto& p : m_tripletPairs) {
        for (decltype(p.probe()->minutiae().size()) i = 0; i < p.probe()->minutiae().size(); ++i) {
            const auto dp = m_probeDupes.emplace(p.probe()->minutiae()[i].key());
            const auto dc = m_candidateDupes.emplace(p.candidate()->minutiae()[i].key());
            if (!dp.second && !dc.second) {
                continue;
            }
            if constexpr (std::is_same_v<R, MinutiaPoint::PairRenderable::Set>) {
                // Similarity values are scaled for integers over [0,1000], for render % is fine...
                m_pairs.emplace_back(&p.probe()->minutiae()[i], &p.candidate()->minutiae()[i], std::lround(static_cast<float>(p.similarity()) / 10.0f));
            }
            if constexpr (std::is_same_v<R, uint8_t>) {
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
                    if constexpr (std::is_same_v<Field::AngleType, float>) {
                        // Wrap within range ...
                        const auto s = p2.probe()->angle() + theta;

                        if (s > FastMath::TwoPI) {
                            return s - FastMath::TwoPI;
                        }
                        if (s < 0) {
                            return s + FastMath::TwoPI;
                        }
                        return s;
                    }
                    if constexpr (std::is_same_v<Field::AngleType, int16_t>) {
                        // Wrap within size of uint8_t...
                        return static_cast<Field::AngleSize>(p2.probe()->angle() + theta);
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
    if constexpr (std::is_same_v<R, uint8_t>) {
        if (maxMatched > Param::MinimumMinutiae) {
            result = static_cast<uint8_t>((maxMatched * maxMatched * 100) / (probe.minutiaeCount() * candidate.minutiaeCount()));
        }
    }
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Explicit instantiations...
//
template class Match<uint8_t, Fingerprint, MinutiaPoint::Pair>;
template class Match<MinutiaPoint::PairRenderable::Set, FingerprintRenderable, MinutiaPoint::PairRenderable>;
}
