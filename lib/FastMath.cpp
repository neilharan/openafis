
#include "FastMath.h"

#include <algorithm>
#include <cmath>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int FastMath::sqrt(const int x)
{
    // NJH-TODO determine range & use lookup for common cases...
    return std::lround(std::sqrt(x));
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
float FastMath::atan2(const float x, const float y)
{
    // NJH-TODO research cordic options https://www.coranac.com/documents/arctangent/
    return std::atan2f(x, y);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
float FastMath::theta360ToRadians(const unsigned int theta)
{
    static constexpr float Factor = PI / 180;

    // NJH-TODO return int scaled for acceptable accuracy...
    return theta * Factor;
}
