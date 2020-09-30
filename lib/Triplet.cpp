
#include "Triplet.h"
#include "FastMath.h"
#include "Log.h"
#include "Param.h"

#include <algorithm>
#include <cassert>
#include <string>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Triplet::Triplet(const Minutiae& minutiae)
    : m_minutiae(shiftClockwise(minutiae))
    , m_distances(sortDistances(m_minutiae))
{
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Triplet::Minutiae Triplet::shiftClockwise(Minutiae minutiae)
{
    assert(minutiae.size() == 3);

    const auto cx = ((minutiae[0].x() + minutiae[1].x()) / 2 + minutiae[2].x()) / 2;
    const auto cy = ((minutiae[0].y() + minutiae[1].y()) / 2 + minutiae[2].y()) / 2;

    auto a0 = FastMath::iatan2(minutiae[0].y() - cy, minutiae[0].x() - cx);
    auto a1 = FastMath::iatan2(minutiae[1].y() - cy, minutiae[1].x() - cx);
    auto a2 = FastMath::iatan2(minutiae[2].y() - cy, minutiae[2].x() - cx);

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
Triplet::Distances Triplet::sortDistances(const Minutiae& minutiae)
{
    Distances d({ minutiae[0].distance(), minutiae[1].distance(), minutiae[2].distance() });
    std::sort(d.begin(), d.end(), [](const Field::TripletCoordType& d1, const Field::TripletCoordType& d2) { return d1 > d2; });
    return d;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Compute similarity per 5.1.1-3
// Note: equation return values are inverted...
//
Triplet::Pair Triplet::findPair(const Triplet& other) const
{
    static const std::vector<Shift> Shifting = { { 0, 1, 2 }, { 1, 2, 0 }, { 2, 0, 1 } }; // rotate triplets when comparing
    float maxS {};
    Shift* maxShift {};

    // NJH-TODO implement theorems 1, 2 & 3...

    for (const auto& shift : Shifting) {
        // Equation 8 (3 iterations)...
        const auto lengths = [&]() {
            auto max = 0;

            for (decltype(shift.size()) i = 0; i < shift.size(); ++i) {
                const auto d = abs(m_minutiae[i].distance() - other.minutiae()[shift[i]].distance());
                if (d > Param::MaximumLocalDistance) {
                    return 1.0f;
                }
                max = std::max(max, d);
            }
            return static_cast<float>(max) / Param::MaximumLocalDistance;
        }();
        if (lengths == 1.0f) {
            continue;
        }

        // Equation 1...
        const auto minimumAngle = [](const float a, const float b) {
            const auto d = abs(a - b);
            return std::min(d, FastMath::PI2 - d);
        };

        // Equation 7 (3 iterations)...
        const auto directions = [&]() {
            for (decltype(shift.size()) i = 0; i < shift.size(); ++i) {
                if (minimumAngle(m_minutiae[i].angle(), other.minutiae()[shift[i]].angle()) > Param::MaximumDirectionDifference) {
                    return false;
                }
            }
            return true;
        }();
        if (!directions) {
            continue;
        }

        // Equation 2...
        const auto rotateAngle = [](const float a, const float b) {
            if (b > a) {
                return b - a;
            }
            return b - a + FastMath::PI2;
        };

        // Equation 10 (3 iterations)...
        const auto anglesBeta = [&]() {
            static const std::vector<unsigned int> Sequence = { 0, 1, 2, 0 };
            auto max = 0.0f;

            for (decltype(shift.size()) i = 0; i < shift.size(); ++i) {
                const auto j = Sequence[i + 1];
                const auto q = rotateAngle(m_minutiae[i].angle(), m_minutiae[j].angle());
                const auto t = rotateAngle(other.minutiae()[shift[i]].angle(), other.minutiae()[shift[j]].angle());
                const auto d = minimumAngle(q, t);
                if (d >= Param::MaximumAngleDifference) {
                    return 1.0f;
                }
                if (abs(d) <= Param::EqualAngleDifference) {
                    return 0.0f; // short-cut
                }
                max = std::max(max, d);
            }
            return max / Param::MaximumAngleDifference;
        }();
        if (anglesBeta == 1.0f) {
            continue;
        }

        // Equation 9 (6 iterations)...
        const auto anglesAlpha = [&]() {
            auto max = 0.0f;

            for (decltype(shift.size()) i = 0; i < shift.size(); ++i) {
                for (decltype(shift.size()) j = 0; j < shift.size(); ++j) {
                    if (i == j) {
                        continue;
                    }
                    const auto y = m_minutiae[i].y() - m_minutiae[j].y();
                    const auto x = m_minutiae[i].x() - m_minutiae[j].x();
                    const auto d = rotateAngle(m_minutiae[i].angle(), FastMath::iatan2(y, x));

                    const auto oy = other.minutiae()[shift[i]].y() - other.minutiae()[shift[j]].y();
                    const auto ox = other.minutiae()[shift[i]].x() - other.minutiae()[shift[j]].x();
                    const auto od = rotateAngle(other.minutiae()[shift[i]].angle(), FastMath::iatan2(oy, ox));

                    const auto ad = minimumAngle(d, od);
                    if (ad >= Param::MaximumAngleDifference) {
                        return 1.0f;
                    }
                    if (abs(ad) <= Param::EqualAngleDifference) {
                        return 0.0f; // short-cut
                    }
                    max = std::max(max, ad);
                }
            }
            return max / Param::MaximumAngleDifference;
        }();
        if (anglesAlpha == 1.0f) {
            continue;
        }
        const auto s = 1.0f - lengths * anglesBeta * anglesAlpha;
        if (s > maxS) {
            maxS = s;
            maxShift = const_cast<Shift*>(&shift);
        }
        if (s == 1) {
            break; // short-cut
        }
    }
    return Pair(maxS, const_cast<Triplet*>(&other), const_cast<Triplet*>(this), maxShift); // NJH-TODO fix const_casts
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t Triplet::bytes() const
{
    size_t sz = sizeof(*this);
    for (const auto& m : m_minutiae) {
        sz += m.bytes();
    }
    sz += m_distances.capacity() * sizeof(decltype(m_distances[0]));
    return sz;
}
