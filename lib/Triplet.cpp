
#include "Triplet.h"
#include "FastMath.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Triplet::Triplet(const Minutiae& minutiae)
    : m_minutiae(shiftClockwise(minutiae))
{
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Triplet::Minutiae Triplet::shiftClockwise(Minutiae minutiae) const
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
}
