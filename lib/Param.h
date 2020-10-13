#ifndef PARAM_H
#define PARAM_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "FastMath.h"
#include "Field.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{

class Param
{
public:
    static constexpr Field::MinutiaCoordType MaximumLocalDistance = 12;
    static constexpr Field::MinutiaCoordType MaximumGlobalDistance = 12;
    static constexpr unsigned int MinimumMinutiae = 4;

    static constexpr Field::AngleType maximumAngleDifference()
    {
        if constexpr (std::is_same_v<Field::AngleType, float>) {
            return MaximumAngleDifference;
        }
        if constexpr (std::is_same_v<Field::AngleType, int>) {
            return static_cast<Field::AngleType>(round(MaximumAngleDifference * FastMath::Radians8));
        }
    }

    static constexpr Field::AngleType maximumDirectionDifference()
    {
        if constexpr (std::is_same_v<Field::AngleType, float>) {
            return MaximumDirectionDifference;
        }
        if constexpr (std::is_same_v<Field::AngleType, int>) {
            return static_cast<Field::AngleType>(round(MaximumDirectionDifference * FastMath::Radians8));
        }
    }

private:
    static constexpr float MaximumAngleDifference = FastMath::PI / 6;
    static constexpr float MaximumDirectionDifference = FastMath::PI / 4;

    // A very limited, but constexpr, round function to help with params...
    static constexpr Field::AngleType round(const float x) { return (x >= 0.0f) ? static_cast<Field::AngleType>(x + 0.5f) : static_cast<Field::AngleType>(x - 0.5f); }
};
}

#endif // PARAM_H
