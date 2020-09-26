#ifndef FIELD_H
#define FIELD_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace Field
{
// Any identifier you like - maybe a std::string for research, or uint16_t for constrained targets...
    using TemplateIdType = uint16_t;
    
// Triplets are generated when loading templates and held in memory. Each template has tens of triplets so this type effects memory usage significantly.
// Generally uint8_t is sufficient but some sensors may generate a lot of minutiae, resulting in more triplets & possibly exceeding 8-bits...
    using TripletType = uint8_t;
};

#endif // FIELD_H
