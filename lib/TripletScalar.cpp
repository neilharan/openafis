
#include "TripletScalar.h"
#include "FastMath.h"
#include "Param.h"

#include <algorithm>
#include <numeric>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TripletScalar::TripletScalar(const Minutiae& minutiae)
    : Triplet(minutiae)
{
    // distance between two vectors: a^2 + b^2 = c^2
    const auto distance = [](const MinutiaPoint& mpa, const MinutiaPoint& mpb) {
        const auto a = mpa.x() - mpb.x();
        const auto b = mpa.y() - mpb.y();
        return FastMath::isqrt(a * a + b * b);
    };

    std::array<Field::MinutiaDistanceType, 3> d { distance(m_minutiae[0], m_minutiae[1]), distance(m_minutiae[1], m_minutiae[2]), distance(m_minutiae[2], m_minutiae[0]) };
    std::sort(d.begin(), d.end(), [](const Field::MinutiaCoordType& d1, const Field::MinutiaCoordType& d2) { return d1 > d2; });
    m_distances = static_cast<uint32_t>(d[2]) << 16 | static_cast<uint32_t>(d[1]) << 8 | d[0];
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Theorems 1, 2 & 3...
//
bool TripletScalar::skipPair(const TripletScalar& probe) const
{
    constexpr auto SkipMethod = 2;

    if constexpr (SkipMethod == 1) {
        // SWAR (ideas from https://programming.sirrida.de/swar.html)...
        constexpr uint32_t SignMask = 0x80808080u;
        constexpr uint8_t Shift = (1 << 3) - 1;

        const uint32_t t0 = ~(m_distances ^ probe.m_distances) & SignMask;

        const uint32_t x1 = ((m_distances | SignMask) - probe.m_distances) & ~SignMask;
        uint32_t t1 = (probe.m_distances & ~m_distances) & SignMask;
        t1 = t1 | (t0 & ~x1);
        t1 = (t1 << 1) - (t1 >> Shift);

        const uint32_t y2 = ((probe.m_distances | SignMask) - m_distances) & ~SignMask;
        uint32_t t2 = (m_distances & ~probe.m_distances) & SignMask;
        t2 = t2 | (t0 & ~y2);
        t2 = (t2 << 1) - (t2 >> Shift);

        auto d = ((x1 ^ t0) & ~t1) + ((y2 ^ t0) & ~t2);

        if (static_cast<int8_t>(d) >= Param::MaximumLocalDistance) {
            return true;
        }
        if (static_cast<int8_t>(d >> 8) >= Param::MaximumLocalDistance) {
            return true;
        }
        if (static_cast<int8_t>(d >> 16) >= Param::MaximumLocalDistance) {
            return true;
        }
        return false;
    }

    if constexpr (SkipMethod == 2) {
        // SWAR (ideas from https://www.chessprogramming.org/SIMD_and_SWAR_Techniques)...
        constexpr uint32_t SignMask = 0x80808080u;
        auto d = ((m_distances | SignMask) - probe.m_distances) ^ ((m_distances ^ SignMask) & SignMask);

        if (static_cast<int8_t>(d) >= Param::MaximumLocalDistance || static_cast<int8_t>(d) <= -Param::MaximumLocalDistance) {
            return true;
        }
        d >>= 8;
        if (static_cast<int8_t>(d) >= Param::MaximumLocalDistance || static_cast<int8_t>(d) <= -Param::MaximumLocalDistance) {
            return true;
        }
        d >>= 8;
        if (static_cast<int8_t>(d) >= Param::MaximumLocalDistance || static_cast<int8_t>(d) <= -Param::MaximumLocalDistance) {
            return true;
        }
        return false;
    }

    if constexpr (SkipMethod == 3) {
        // SWAR (ideas from https://www.chessprogramming.org/SIMD_and_SWAR_Techniques)...
        constexpr uint32_t MSBON = 0x80808080u;
        auto d = ((m_distances | MSBON) - probe.m_distances) ^ ((m_distances ^ MSBON) & MSBON);

        if (std::abs(static_cast<Field::MinutiaDistanceType>(d)) >= Param::MaximumLocalDistance) {
            return true;
        }
        if (std::abs(static_cast<Field::MinutiaDistanceType>(d >> 8)) >= Param::MaximumLocalDistance) {
            return true;
        }
        if (std::abs(static_cast<Field::MinutiaDistanceType>(d >> 16)) >= Param::MaximumLocalDistance) {
            return true;
        }
        return false;
    }

    if constexpr (SkipMethod == 4) {
        // Default (abs(x-y) b0, b1 and b2)...
        auto cd = m_distances;
        auto pd = probe.m_distances;
        if (FastMath::diff(static_cast<Field::MinutiaDistanceType>(cd), static_cast<Field::MinutiaDistanceType>(pd)) >= Param::MaximumLocalDistance) {
            return true;
        }
        cd >>= 8;
        pd >>= 8;
        if (FastMath::diff(static_cast<Field::MinutiaDistanceType>(cd), static_cast<Field::MinutiaDistanceType>(pd)) >= Param::MaximumLocalDistance) {
            return true;
        }
        cd >>= 8;
        pd >>= 8;
        if (FastMath::diff(static_cast<Field::MinutiaDistanceType>(cd), static_cast<Field::MinutiaDistanceType>(pd)) >= Param::MaximumLocalDistance) {
            return true;
        }
        return false;
    }
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Compute similarity per 5.1.1-3
// Note: equation return values are inverted and scaled for integer math...
//
void TripletScalar::emplacePair(Pair::Pairs& pairs, const TripletScalar& probe) const
{
    using Shift = std::array<uint8_t, 3>;
    static constexpr std::array<Shift, 3> Shifting = { { { 0, 1, 2 }, { 1, 2, 0 }, { 2, 0, 1 } } }; // rotate triplets when comparing
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
                if (FastMath::minimumAngle(m_minutiae[i].angle(), probe.minutiae()[shift[i]].angle()) >= Param::maximumDirectionDifference()) {
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
            auto cd = m_distances;
            auto pd = probe.m_distances;

            for (decltype(shift.size()) i = 0; i < shift.size(); ++i) {
                // NJH-TODO const auto d = FastMath::diff(m_minutiae[i].distance(), probe.minutiae()[shift[i]].distance());
                const auto d = FastMath::diff(static_cast<Field::MinutiaDistanceType>(cd), static_cast<Field::MinutiaDistanceType>(pd >> (shift[i] * 8)));
                // NJH-TODO const auto d = FastMath::diff(static_cast<Field::MinutiaDistanceType>(cd), probe.minutiae()[shift[i]].distance());
                if (d >= Param::MaximumLocalDistance) {
                    return Pair::SimilarityMultiplier;
                }
                cd >>= 8;
                max = std::max(max, static_cast<int>(d));
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
                    const auto y = static_cast<Field::MinutiaCoordType>(m_minutiae[i].y() - m_minutiae[j].y());
                    const auto x = static_cast<Field::MinutiaCoordType>(m_minutiae[i].x() - m_minutiae[j].x());
                    const auto d = FastMath::rotateAngle(m_minutiae[i].angle(), FastMath::atan2(y, x));

                    const auto oy = static_cast<Field::MinutiaCoordType>(probe.minutiae()[shift[i]].y() - probe.minutiae()[shift[j]].y());
                    const auto ox = static_cast<Field::MinutiaCoordType>(probe.minutiae()[shift[i]].x() - probe.minutiae()[shift[j]].x());
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
bool TripletScalar::operator<(const TripletScalar& other) const
{
    auto cd = m_distances;
    auto pd = other.m_distances;

    // max...
    if (static_cast<Field::MinutiaDistanceType>(cd) < static_cast<Field::MinutiaDistanceType>(pd)) {
        return true;
    }
    if (static_cast<Field::MinutiaDistanceType>(pd) < static_cast<Field::MinutiaDistanceType>(cd)) {
        return false;
    }

    // mid...
    cd >>= 8;
    pd >>= 8;
    if (static_cast<Field::MinutiaDistanceType>(cd) < static_cast<Field::MinutiaDistanceType>(pd)) {
        return true;
    }
    if (static_cast<Field::MinutiaDistanceType>(pd) < static_cast<Field::MinutiaDistanceType>(cd)) {
        return false;
    }

    // min...
    cd >>= 8;
    pd >>= 8;
    if (static_cast<Field::MinutiaDistanceType>(cd) < static_cast<Field::MinutiaDistanceType>(pd)) {
        return true;
    }
    return false;
}

}
