#ifndef FINGERPRINT_H
#define FINGERPRINT_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "LMTS.h"

#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Fingerprint
{
public:
    using Data = std::vector<std::vector<LMTS>>;
    using Dimensions = std::pair<unsigned short, unsigned short>;

    Fingerprint(const size_t size
#ifdef OPENAFIS_FINGERPRINT_INCLUDE_DIMENSIONS
        , const Dimensions &dimensions
#endif
    )
        : m_data(size)
#ifdef OPENAFIS_FINGERPRINT_INCLUDE_DIMENSIONS
        , m_dimensions(dimensions)
#endif
        {}

    Data &data() { return m_data; };
    const Data &data() const { return m_data; };

#ifdef OPENAFIS_FINGERPRINT_INCLUDE_DIMENSIONS
    const Dimensions &dimensions() const { return m_dimensions; };
#endif

private:   
    Data m_data;

#ifdef OPENAFIS_FINGERPRINT_INCLUDE_DIMENSIONS
    const Dimensions m_dimensions;
#endif
};

#endif // FINGERPRINT_H
