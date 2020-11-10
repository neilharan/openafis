
#include "TripletSIMD.h"
#include "FastMath.h"
#include "Param.h"

#define MIPP_ALIGNED_LOADS
#include "mipp.h"

#include <algorithm>
#include <numeric>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Note: this is a work-in-progress and is currently SLOWER than the scalar implementation!
//
namespace OpenAFIS
{


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TripletSIMD::TripletSIMD(const Minutiae& minutiae)
    : Triplet(minutiae)
{
    sortDistances();
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TripletSIMD::sortDistances()
{
    m_distances.resize(32);

    using Shift = std::vector<int>;
    static const std::vector<int> Shifting = { 0, 1, 2, 1, 2, 0, 2, 0, 1 };

    auto i = 0;
    m_distances[i++] = 1;
    m_distances[i++] = 2;
    m_distances[i++] = 3;
    std::sort(m_distances.begin(), m_distances.begin() + 3, [](const int16_t d1, const int16_t d2) { return d1 > d2; });

    m_distances[i++] = m_distances[Shifting[i]]; // shifted
    m_distances[i++] = m_distances[Shifting[i]]; // "
    m_distances[i++] = m_distances[Shifting[i]]; // "
    m_distances[i++] = m_distances[Shifting[i]]; // "
    m_distances[i++] = m_distances[Shifting[i]]; // "
    m_distances[i++] = m_distances[Shifting[i]]; // "

    std::memcpy(m_distances.data() + 9, m_distances.data(), 16 - 9);
    std::memcpy(m_distances.data() + 16, m_distances.data(), 16 - 5);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Compute similarity per 5.1.1-3
// Note: equation return values are inverted and scaled for integer math...
//
void TripletSIMD::emplacePair(Pair::Pairs& pairs, const TripletSIMD& probe) const
{
    // Theorems 1, 2 & 3...
    const mipp::Reg<int8_t> cd = m_distances.data();
    mipp::Reg<int8_t> pd = probe.distances();

    const auto d0 = mipp::abs(cd - pd) > Param::MaximumLocalDistance;
    if (!mipp::testz(d0)) {
        return;
    }
    const mipp::Reg<int8_t> shuffle { 0, 1, 2, 3, 4, 5, 6, 7, 8, 3, 4, 5, 6, 7, 8, 0, 1, 2, 6, 7, 8, 0, 1, 2, 3, 4, 5, -1, -1, -1, -1, -1 };
    pd = pd.shuff2(shuffle);
    const auto v = mipp::abs(cd - pd);
    const auto d1 = v > Param::MaximumLocalDistance;

    mipp::Reg<int8_t> z;
    z.set0();
    const auto b = mipp::blend(z, v, d1);
    const int l = mipp::hmax(b);

    using Shift = std::vector<int>;
    static const std::vector<Shift> Shifting = { { 0, 1, 2 }, { 1, 2, 0 }, { 2, 0, 1 } }; // rotate triplets when comparing
    static constexpr auto BestS = Pair::SimilarityMultiplier * Pair::SimilarityMultiplier * Pair::SimilarityMultiplier;
    auto bestS = BestS;

    for (const auto& shift : Shifting) {
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

        // Equation 8...
        switch (shift[0]) {
            default:
            case 0: {
                break;
            }
            case 1: {
                mipp::Msk<32> mask { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
                if (!mipp::testz(d1 & mask)) {
                    continue;
                }
                break;
            }
            case 2: {
                mipp::Msk<32> mask { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
                if (!mipp::testz(d1 & mask)) {
                    continue;
                }
                break;
            }
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
        const auto s = ((l * Pair::SimilarityMultiplier) / Param::MaximumLocalDistance) * anglesBeta * anglesAlpha;
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
bool TripletSIMD::operator<(const TripletSIMD& other) const
{
    for (decltype(m_distances.size()) i = 0; i < 3; ++i) {
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
