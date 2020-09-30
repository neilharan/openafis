
#include "FastMath.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Domain: 0 < x < max(Field::TripletCoordType)^2
// Maximum result is also a dimension (default uint8_t), so a table is viable and quite small (<64K)...
//
int FastMath::isqrt(const int x)
{
#if 1
    static const SquareRoots Table;

    return Table.get(x);
#else
    return std::lround(std::sqrt(x));
#endif
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// This function is on the hot path during scoring.
// Domain: -max(Field::TripletCoordType) < x < max(Field::TripletCoordType), y is the same so lookups possible (they weigh about ~1MB with floats).
// Profiling reveals ~2x speedup using lookups vs CRT (with SSE2).
// TODO: research cordic options https://www.coranac.com/documents/arctangent/ for memory constrained builds...
//
float FastMath::iatan2(const int x, const int y)
{
#if 1
    static const ArcTangents Table;

    return Table.get(x, y);
#else
    return std::atan2f(static_cast<float>(x), static_cast<float>(y));
#endif
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
float FastMath::theta360ToRadians(const unsigned int theta)
{
    static constexpr float Factor = PI / 180;

    // NJH-TODO return int scaled for acceptable accuracy...
    return theta * Factor;
}
