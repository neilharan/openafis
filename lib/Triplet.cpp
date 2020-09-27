
#include "Param.h"
#include "Triplet.h"

#include <cassert>
#include <string>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Triplet::Triplet(const Minutiae& minutiae)
    : m_minutiae(sort(minutiae))
{
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Triplet::Minutiae Triplet::sort(Minutiae minutiae)
{
    assert(minutiae.size() == 3);

    const auto cx = ((minutiae[0].x() + minutiae[1].x()) / 2.0f + minutiae[2].x()) / 2.0f;
    const auto cy = ((minutiae[0].y() + minutiae[1].y()) / 2.0f + minutiae[2].y()) / 2.0f;

    auto a0 = std::atan2f(static_cast<float>(minutiae[0].y()) - cy, static_cast<float>(minutiae[0].x()) - cx);
    auto a1 = std::atan2f(static_cast<float>(minutiae[1].y()) - cy, static_cast<float>(minutiae[1].x()) - cx);
    auto a2 = std::atan2f(static_cast<float>(minutiae[2].y()) - cy, static_cast<float>(minutiae[2].x()) - cx);

    const auto swap = [](auto &x, auto &y) {
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
int Triplet::score(const Triplet &other) const
{
    const auto d0 = abs(m_minutiae[0].distance() - other.minutiae()[0].distance());
    if (d0 > Param::MaximumLocalDistance) {
    }
    const auto d1 = abs(m_minutiae[1].distance() - other.minutiae()[0].distance());
    if (d1 > Param::MaximumLocalDistance) {
    }
    const auto d2 = abs(m_minutiae[2].distance() - other.minutiae()[0].distance());
    if (d2 > Param::MaximumLocalDistance) {
    }
    return 0;
}
