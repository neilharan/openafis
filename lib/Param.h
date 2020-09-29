#ifndef PARAM_H
#define PARAM_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Field.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Param
{
public:
    static const Field::TripletCoordType MaximumLocalDistance;
    static const Field::TripletCoordType MaximumGlobalDistance;
    static const float EqualAngleDifference;
    static const float MaximumAngleDifference;
    static const float MaximumDirectionDifference;
    static const unsigned int MinimumMinutiae;
};

#endif // PARAM_H
