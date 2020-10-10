
#include "FastMath.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Domain: 0 < x < max(Field::MinutiaCoordType)^2
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
// Domain: -max(Field::MinutiaCoordType) < x < max(Field::MinutiaCoordType), y is the same so lookups possible (they weigh about ~1MB with floats).
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
float FastMath::degreesToRadians(const unsigned int theta)
{
    static constexpr float Factor = PI / 180.0f;

    return static_cast<float>(theta) * Factor;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
unsigned int FastMath::radiansToDegrees(const float theta)
{
    static constexpr float Factor = PI / 180.0f;

    return static_cast<unsigned int>(std::lround(theta / Factor));
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Equation 1...
//
float FastMath::minimumAngle(const float a, const float b)
{
    const auto d = abs(a - b);
    return std::min(d, FastMath::PI2 - d);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Equation 2...
//
float FastMath::rotateAngle(const float a, const float b)
{
    if (b > a) {
        return b - a;
    }
    return b - a + FastMath::PI2;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int FastMath::round(const float v)
{
#if 0
    return static_cast<int>(v);
#else
    return std::lround(v);
#endif
}
