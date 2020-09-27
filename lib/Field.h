#ifndef FIELD_H
#define FIELD_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <cstdint>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace Field
{
// Any identifier you like - maybe a std::string for research, or uint16_t for constrained targets...
    using TemplateIdType = uint16_t;
    
// Triplets are generated when loading templates and held in memory. Each template has tens of triplets so this type effects memory usage significantly.
// Generally uint8_t is sufficient but some sensors may generate a lot of minutiae, resulting in more triplets & possibly exceeding 8-bits...
    using TripletIndexType = uint8_t;

// Coordinates and distances are always scaled to 8-bits so uint8_t is sufficient...
    using TripletCoordType = uint8_t;
};

#endif // FIELD_H
