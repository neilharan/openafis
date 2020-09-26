
#include "Log.h"
#include "Triplet.h"

#include <string>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Triplet::Triplet(const Points& points)
    : m_points(sort(points))
{
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Triplet::Points Triplet::sort(Points points)
{
    const auto cx = ((points[0].second.x() + points[1].second.x()) / 2.0f + points[2].second.x()) / 2.0f;
    const auto cy = ((points[0].second.y() + points[1].second.y()) / 2.0f + points[2].second.y()) / 2.0f;

    auto a0 = std::atan2f(static_cast<float>(points[0].second.y()) - cy, static_cast<float>(points[0].second.x()) - cx);
    auto a1 = std::atan2f(static_cast<float>(points[1].second.y()) - cy, static_cast<float>(points[1].second.x()) - cx);
    auto a2 = std::atan2f(static_cast<float>(points[2].second.y()) - cy, static_cast<float>(points[2].second.x()) - cx);

    const auto swap = [](auto &x, auto &y) {
        const auto t = x;
        x = y;
        y = t;
    };
    if (a0 > a2) {
        swap(a0, a2);
        swap(points[0], points[2]);
    }
    if (a0 > a1) {
        swap(a0, a1);
        swap(points[0], points[1]);
    }
    if (a1 > a2) {
        swap(a1, a2);
        swap(points[1], points[2]);
    }
    return points;
}
