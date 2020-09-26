#ifndef MINUTIAPOINT_H
#define MINUTIAPOINT_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Dimensions.h"
#include "Minutia.h"

#include <cmath>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MinutiaPoint
{
public:
    MinutiaPoint(const Dimensions& dimensions, const Minutia& minutia)
        : m_x(static_cast<uint8_t>(std::lround(minutia.x() * (256.0f / dimensions.first))))
        , m_y(static_cast<uint8_t>(std::lround(minutia.y() * (256.0f / dimensions.second)))) {}

    uint8_t x() const { return m_x; } // scaled for dimensions
    uint8_t y() const { return m_y; } // "
    
private:
    uint8_t m_x;
    uint8_t m_y;
};

#endif // MINUTIAPOINT_H
