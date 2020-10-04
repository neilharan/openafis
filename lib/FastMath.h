#ifndef FASTMATH_H
#define FASTMATH_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Field.h"

#include <array>
#include <cassert>
#include <cmath>
#include <limits>


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
    class SquareRoots
    {
    public:
        SquareRoots()
            : m_values([]() {
                static Values v;
                for (size_t i = 0; i < Max; ++i) {
                    v.at(i) = static_cast<Field::MinutiaCoordType>(std::lround(std::sqrt(i)));
                }
                return &v;
            }())
        {
        }

        [[nodiscard]] constexpr int get(const int x) const
        {
            assert(x >= 0 && x < Max);
            return (*m_values)[x];
        }

        static constexpr auto Max = std::numeric_limits<Field::MinutiaCoordType>::max() * std::numeric_limits<Field::MinutiaCoordType>::max();

    private:
        using Values = std::array<Field::MinutiaCoordType, Max>;
        const Values* m_values;
    };

    class ArcTangents
    {
    public:
        ArcTangents()
            : m_values([]() {
                static Values v;
                for (auto x = Min; x < Max; ++x) {
                    for (auto y = Min; y < Max; ++y) {
                        v.at(x - Min).at(y - Min) = std::atan2f(static_cast<float>(x), static_cast<float>(y));
                    }
                }
                return &v;
            }())
        {
        }

        [[nodiscard]] constexpr float get(const int x, const int y) const
        {
            assert(x > Min && x < Max);
            assert(y > Min && y < Max);
            return (*m_values)[x - Min][y - Min];
        }

        static constexpr auto Min = -std::numeric_limits<Field::MinutiaCoordType>::max();
        static constexpr auto Max = std::numeric_limits<Field::MinutiaCoordType>::max();

    private:
        using Values = std::array<std::array<float, Max - Min>, Max - Min>;
        const Values* m_values;
    };
};

#endif // FASTMATH_H
