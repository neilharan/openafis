
#include "Param.h"
#include "FastMath.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
constexpr Field::MinutiaCoordType Param::MaximumLocalDistance = 12;
constexpr Field::MinutiaCoordType Param::MaximumGlobalDistance = 12;
constexpr float Param::EqualAngleDifference = 0.01f;
constexpr float Param::MaximumAngleDifference = FastMath::PI / 6;
constexpr float Param::MaximumDirectionDifference = FastMath::PI / 4;
constexpr unsigned int Param::MinimumMinutiae = 4;
