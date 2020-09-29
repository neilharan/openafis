#ifndef FASTMATH_H
#define FASTMATH_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Config.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class FastMath
{
public:
    static constexpr float PI = 3.14159265358979323846f; // until C++20 we have to provide this
    static constexpr float PI2 = PI * 2; // "

    static int sqrt(const int x);
    static float atan2(const float x, const float y);
    static float theta360ToRadians(const unsigned int theta);
};

#endif // FASTMATH_H
