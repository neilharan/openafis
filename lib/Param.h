#ifndef PARAM_H
#define PARAM_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Field.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Param
{
public:
    static const float PI; // until C++20 we have to provide this

    static const Field::TripletCoordType MaximumLocalDistance;
    static const Field::TripletCoordType MaximumGlobalDistance;
    static const float MaximumAngleDifference;
    static const unsigned int MinimumMinutiae;
};

#endif // PARAM_H
