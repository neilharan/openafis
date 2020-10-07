#ifndef PARAM_H
#define PARAM_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Config.h"
#include "Field.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Param
{
public:
    static const Field::MinutiaCoordType MaximumLocalDistance;
    static const Field::MinutiaCoordType MaximumGlobalDistance;
    static const float EqualAngleDifference;
    static const float MaximumAngleDifference;
    static const float MaximumDirectionDifference;
    static const unsigned int MinimumMinutiae;
};

#endif // PARAM_H
