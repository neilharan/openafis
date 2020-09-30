#ifndef MINUTIAPOINT_H
#define MINUTIAPOINT_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Dimensions.h"
#include "FastMath.h"
#include "Minutia.h"

#include <cmath>

#ifdef OPENAFIS_TRIPLETS_PACK
#define TRIPLET_PACK PACK
#else
#define TRIPLET_PACK
#endif


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// When packing is enabled a triplet can consume as little as three bytes but read accesses will be slightly slower - depending on platform architecture.
// With packing disabled triplets will consume 3 x architectures word size regardless but scoring performance may be improved.
//
class MinutiaPoint
{
public:
    // co-ordinates (and therefore distances) are always scales for 8-bits...
    MinutiaPoint(const Dimensions& dimensions, const Minutia& minutia)
        : m_x(static_cast<Field::TripletCoordType>(std::lround(static_cast<float>(minutia.x()) * (256.0f / dimensions.first))))
        , m_y(static_cast<Field::TripletCoordType>(std::lround(static_cast<float>(minutia.y()) * (256.0f / dimensions.second))))
        , m_angle(FastMath::theta360ToRadians(minutia.angle()))
    {
    }

    Field::TripletCoordType x() const { return m_x; }
    Field::TripletCoordType y() const { return m_y; }
    float angle() const { return m_angle; }
    Field::TripletCoordType distance() const { return m_distance; }
    size_t bytes() const { return sizeof(*this); }

    // distance between two vectors: a^2 + b^2 = c^2
    void setDistanceFrom(const MinutiaPoint& other)
    {
        const auto a = static_cast<int>(m_x) - other.x();
        const auto b = static_cast<int>(m_y) - other.y();
        m_distance = static_cast<Field::TripletCoordType>(FastMath::isqrt(a * a + b * b));
    }

private:
    Field::TripletCoordType m_x; // scaled for dimensions
    Field::TripletCoordType m_y; // "
    float m_angle; // radians

    Field::TripletCoordType m_distance {}; // distance from adjacent side, also scaled
};

#endif // MINUTIAPOINT_H
