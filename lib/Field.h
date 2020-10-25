#ifndef FIELD_H
#define FIELD_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <cstdint>
#include <limits>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{

class Field
{
public:
    // Coordinates and distances are always scaled to 8-bits so uint8_t is sufficient, int is likely to be more efficient though.
    // When memory constrained specify a smaller type here, otehrwise leave default...
    using MinutiaCoordSize = uint8_t;
    using MinutiaCoordType = int16_t;

    using MinutiaDistanceType = int8_t;

    // Angles are expressed in radians, but are mapped over [0,255] when AngleType is an integer.
    // Options are int16_t or float. Double is not supported.
    // During benchmarks approximately 1.2x total match speed was observed withs ints over floats (will vary considerably between platforms)...
    using AngleSize = uint8_t;
    using AngleType = int16_t;

    // We only use the 16 LSB's in the key type. While MinutiaCoordType is uint8_t this could be uint16_t.
    // It is used only to identify duplicate triplets...
    using MinutiaKeyType = int16_t;
};
}

#endif // FIELD_H
