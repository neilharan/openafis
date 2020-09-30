#ifndef FASTMATH_H
#define FASTMATH_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Field.h"

#include <cassert>
#include <cmath>
#include <limits>
#include <array>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class FastMath
{
public:
    static constexpr float PI = 3.14159265358979323846f; // until C++20 we have to provide this
    static constexpr float PI2 = PI * 2; // "

    static int isqrt(int x);
    static float iatan2(int x, int y);
    static float theta360ToRadians(unsigned int theta);

private:
    class SquareRoots {
    public:
        SquareRoots()
        {
            for (size_t i = 0; i < Max; ++i) {
                m_values[i] = static_cast<Field::TripletCoordType>(std::lround(std::sqrt(i)));
            }
        }

        int get(const int x) const
        {
            assert(x >= 0 && x < Max);
            return m_values[x];
        }

        static constexpr size_t Max = std::numeric_limits<Field::TripletCoordType>::max() * std::numeric_limits<Field::TripletCoordType>::max();

    private:
        std::array<Field::TripletCoordType, Max> m_values;
    };

    class ArcTangents {
    public:
        ArcTangents()
        {
            for (auto x = Min; x < Max; ++x) {
                for (auto y = Min; y < Max; ++y) {
                    m_values[x - Min][y - Min] = std::atan2f(static_cast<float>(x), static_cast<float>(y));
                }
            }
        }

        float get(const int x, const int y) const
        {
            assert(x > Min && x < Max);
            assert(y > Min && y < Max);
            return m_values[x - Min][y - Min];
        }

        static constexpr auto Min = -std::numeric_limits<Field::TripletCoordType>::max();
        static constexpr auto Max = std::numeric_limits<Field::TripletCoordType>::max();

    private:
        std::array<std::array<float, Max - Min>, Max - Min> m_values;
    };
};

#endif // FASTMATH_H
