
#include "Log.h"
#include "Triplet.h"
#include "FastMath.h"
#include "Param.h"

#include "mipp.h"

#include <algorithm>
#include <numeric>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Triplet::Triplet(const MinutiaPoint::Minutiae& minutiae)
    : m_minutiae(shiftClockwise(minutiae))
{
    sortDistances();
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MinutiaPoint::Minutiae Triplet::shiftClockwise(MinutiaPoint::Minutiae minutiae)
{
    const auto cx = ((minutiae[0].x() + minutiae[1].x()) / 2 + minutiae[2].x()) / 2;
    const auto cy = ((minutiae[0].y() + minutiae[1].y()) / 2 + minutiae[2].y()) / 2;

    auto a0 = FastMath::atan2(minutiae[0].y() - cy, minutiae[0].x() - cx);
    auto a1 = FastMath::atan2(minutiae[1].y() - cy, minutiae[1].x() - cx);
    auto a2 = FastMath::atan2(minutiae[2].y() - cy, minutiae[2].x() - cx);

    const auto swap = [](auto& x, auto& y) {
        const auto copy = x;
        x = y;
        y = copy;
    };
    if (a0 > a2) {
        swap(a0, a2);
        swap(minutiae[0], minutiae[2]);
    }
    if (a0 > a1) {
        swap(a0, a1);
        swap(minutiae[0], minutiae[1]);
    }
    if (a1 > a2) {
        swap(a1, a2);
        swap(minutiae[1], minutiae[2]);
    }
    minutiae[0].setDistanceFrom(minutiae[1]);
    minutiae[1].setDistanceFrom(minutiae[2]);
    minutiae[2].setDistanceFrom(minutiae[0]);
    return minutiae;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Triplet::sortDistances()
{
    Distances d({ m_minutiae[0].distance(), m_minutiae[1].distance(), m_minutiae[2].distance() });
    std::sort(d.begin(), d.end(), [](const Field::MinutiaCoordType& d1, const Field::MinutiaCoordType& d2) { return d1 > d2; });

    m_distances.resize(32);
    m_distances[0] = d[0];
    m_distances[1] = d[1];
    m_distances[2] = d[2];
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Triplet::skipPair(const Triplet& probe) const
{
    // Theorems 1, 2 & 3...
    if constexpr (Param::EnableSIMD) {
        mipp::Reg<Field::MinutiaDistanceType> cd;
        cd.load(m_distances.data());

        mipp::Reg<Field::MinutiaDistanceType> pd;
        pd.load(probe.distances().data());

        const auto d = mipp::abs(cd - pd) >= Param::MaximumLocalDistance;
        return !mipp::testz(d);
    }
    if constexpr (!Param::EnableSIMD) {
        for (decltype(m_distances.size()) i = 0; i < m_distances.size(); ++i) {
            if (std::abs(m_distances[i] - probe.distances()[i]) >= Param::MaximumLocalDistance) {
                return true;
            }
        }
        return false;
    }
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Compute similarity per 5.1.1-3
// Note: equation return values are inverted and scaled for integer math...
//
void Triplet::emplacePair(Pair::Pairs& pairs, const Triplet& probe) const
{
    using Shift = std::vector<int>;
    static const std::vector<Shift> Shifting = { { 0, 1, 2 } , { 1, 2, 0 } , { 2, 0, 1 } }; // rotate triplets when comparing
    static constexpr auto BestS = Pair::SimilarityMultiplier * Pair::SimilarityMultiplier * Pair::SimilarityMultiplier;
    auto bestS = BestS;
    auto rotations = Param::MaximumRotations;

    for (const auto& shift : Shifting) {
        if constexpr (Param::MaximumRotations != 3) {
            if (!rotations--) {
                break;
            }
        }

        // Equation 7 (3 iterations)...
        const auto directions = [&]() {
            for (decltype(shift.size()) i = 0; i < shift.size(); ++i) {
                if (FastMath::minimumAngle(m_minutiae[i].angle(), probe.minutiae()[shift[i]].angle()) > Param::maximumDirectionDifference()) {
                    return false;
                }
            }
            return true;
        }();
        if (!directions) {
            continue;
        }

        // Equation 8 (3 iterations)...
        const auto lengths = [&]() {
            auto max = 0;

            for (decltype(shift.size()) i = 0; i < shift.size(); ++i) {
                const auto d = static_cast<int>(std::abs(m_minutiae[i].distance() - probe.minutiae()[shift[i]].distance()));
                if (d > Param::MaximumLocalDistance) {
                    return Pair::SimilarityMultiplier;
                }
                max = std::max(max, d);
            }
            return (max * Pair::SimilarityMultiplier) / Param::MaximumLocalDistance;
        }();
        if (lengths == Pair::SimilarityMultiplier) {
            continue;
        }

        // Equation 10 (3 iterations)...
        const auto anglesBeta = [&]() {
            Field::AngleType max {};
            const auto& s = Shifting[1];

            for (decltype(shift.size()) i = 0; i < shift.size(); ++i) {
                const auto& j = s[i];
                const auto c = FastMath::rotateAngle(m_minutiae[i].angle(), m_minutiae[j].angle());
                const auto p = FastMath::rotateAngle(probe.minutiae()[shift[i]].angle(), probe.minutiae()[shift[j]].angle());
                const auto d = FastMath::minimumAngle(c, p);
                if (d >= Param::maximumAngleDifference()) {
                    return Pair::SimilarityMultiplier;
                }
                max = std::max(max, d);
            }
            return (max * Pair::SimilarityMultiplier) / Param::maximumAngleDifference();
        }();
        if (anglesBeta == Pair::SimilarityMultiplier) {
            continue;
        }

        // Equation 9 (6 iterations)...
        const auto anglesAlpha = [&]() {
            Field::AngleType max {};

            for (decltype(shift.size()) i = 0; i < shift.size(); ++i) {
                for (decltype(shift.size()) j = 0; j < shift.size(); ++j) {
                    if (i == j) {
                        continue;
                    }
                    const auto y = m_minutiae[i].y() - m_minutiae[j].y();
                    const auto x = m_minutiae[i].x() - m_minutiae[j].x();
                    const auto d = FastMath::rotateAngle(m_minutiae[i].angle(), FastMath::atan2(y, x));

                    const auto oy = probe.minutiae()[shift[i]].y() - probe.minutiae()[shift[j]].y();
                    const auto ox = probe.minutiae()[shift[i]].x() - probe.minutiae()[shift[j]].x();
                    const auto od = FastMath::rotateAngle(probe.minutiae()[shift[i]].angle(), FastMath::atan2(oy, ox));

                    const auto ad = FastMath::minimumAngle(d, od);
                    if (ad >= Param::maximumAngleDifference()) {
                        return Pair::SimilarityMultiplier;
                    }
                    max = std::max(max, ad);
                }
            }
            return (max * Pair::SimilarityMultiplier) / Param::maximumAngleDifference();
        }();
        if (anglesAlpha == Pair::SimilarityMultiplier) {
            continue;
        }
        const auto s = lengths * anglesBeta * anglesAlpha;
        if (s < bestS) {
            bestS = s;
            if (bestS == 0) {
                // Short-cut, exact match...
                pairs.emplace_back(Pair::SimilarityMultiplier, &probe, this);
                return;
            }
        }
    }
    if (bestS != BestS) {
        pairs.emplace_back(Pair::SimilarityMultiplier - (bestS / (Pair::SimilarityMultiplier * Pair::SimilarityMultiplier)), &probe, this);
    }
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t Triplet::bytes() const
{
    return sizeof(*this) + std::accumulate(m_minutiae.begin(), m_minutiae.end(), size_t {}, [](size_t sum, const auto& m) { return sum + m.bytes(); })
        + m_distances.capacity() * sizeof(decltype(m_distances[0]));
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Triplet::operator<(const Triplet& other) const
{
    for (decltype(m_distances.size()) i = 0; i < m_distances.size(); ++i) {
        if (m_distances[i] < other.m_distances[i]) {
            return true;
        }
        if (other.m_distances[i] < m_distances[i]) {
            return false;
        }
    }
    return false;
}
}
