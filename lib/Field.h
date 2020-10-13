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
    // Any identifier you like - maybe a std::string for research, or uint16_t for constrained targets...
    using TemplateIdType = uint32_t;

    // Triplets are generated when loading templates and held in memory. Each template has tens of triplets so this type effects memory usage significantly.
    // Generally uint8_t is sufficient but some sensors may generate a lot of minutiae, resulting in more triplets & possibly exceeding 8-bits...
    using TripletIndexType = uint8_t;

    // Coordinates and distances are always scaled to 8-bits so uint8_t is sufficient...
    using MinutiaCoordType = uint8_t;
    static constexpr size_t MinutiaCoordMax = std::numeric_limits<Field::MinutiaCoordType>::max();
    static constexpr size_t MinutiaCoordMatrixSize = (MinutiaCoordMax + 1) * (MinutiaCoordMax + 1);

    // Angles are expressed in radians, but are mapped over [0,255] when using ints here.
    // Options are int or float. Double is not supported.
    // During benchmarks approximately 1.2x total match speed was observed withs ints over floats (will vary considerably between platforms)...
    using AngleType = int;
    static constexpr size_t AngleMax = 255;

    // We only use the 16 LSB's in the key type. While MinutiaCoordType is uint8_t this could be uint16_t.
    // It is used only to identify duplicate triplets...
    using MinutiaKeyType = uint32_t;
};
}

#endif // FIELD_H
