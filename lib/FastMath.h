#ifndef FASTMATH_H
#define FASTMATH_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Field.h"

#include <array>
#include <cassert>
#include <cmath>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{

class FastMath
{
public:
    static constexpr float PI = 3.14159265358979323846f; // until C++20 we have to provide this
    static constexpr float TwoPI = PI * 2; // "
    static constexpr float Radians8 = 255.0f / TwoPI; // " = ~40.58: map radians [0,2PI] over uint8_t [0,255]
    static constexpr int TwoPI8 = 255; // " our mapped radian limit

    static int isqrt(int x);
    static Field::AngleType atan2(int x, int y);
    static float cos(Field::AngleType theta);
    static float sin(Field::AngleType theta);
    static Field::AngleType degreesToRadians(unsigned int theta);
    static Field::AngleType minimumAngle(Field::AngleType a, Field::AngleType b);
    static Field::AngleType rotateAngle(Field::AngleType a, Field::AngleType b);

private:
    class SquareRoots
    {
    public:
        SquareRoots();

        [[nodiscard]] constexpr int get(const int x) const
        {
            assert(x >= 0 && x < Max);
            return (*m_values)[x];
        }

        static constexpr auto Max = static_cast<int>(Field::MinutiaCoordMatrixSize) * 2;

    private:
        using Values = std::array<Field::MinutiaCoordType, Max>;
        const Values* m_values;
    };

    class ArcTangents
    {
    public:
        ArcTangents();

        [[nodiscard]] constexpr Field::AngleType get(const int x, const int y) const
        {
            assert(x > Min && x < Max);
            assert(y > Min && y < Max);
            return (*m_values)[x - Min][y - Min];
        }

        static constexpr auto Min = -static_cast<int>(Field::MinutiaCoordMax);
        static constexpr auto Max = static_cast<int>(Field::MinutiaCoordMax);

    private:
        using Values = std::array<std::array<Field::AngleType, Max - Min>, Max - Min>;
        const Values* m_values;
    };

    class Cosines
    {
    public:
        Cosines();

        [[nodiscard]] constexpr float get(const Field::AngleType x) const
        {
            assert(x > Min && x < Max);
            return (*m_values)[x - Min];
        }

        static constexpr auto Min = -static_cast<int>(Field::AngleMax);
        static constexpr auto Max = static_cast<int>(Field::AngleMax);

    private:
        using Values = std::array<float, Max - Min>;
        const Values* m_values;
    };

    class Sines
    {
    public:
        Sines();

        [[nodiscard]] constexpr float get(const Field::AngleType x) const
        {
            assert(x > Min && x < Max);
            return (*m_values)[x - Min];
        }

        static constexpr auto Min = -static_cast<int>(Field::AngleMax);
        static constexpr auto Max = static_cast<int>(Field::AngleMax);

    private:
        using Values = std::array<float, Max - Min>;
        const Values* m_values;
    };
};
}

#endif // FASTMATH_H
