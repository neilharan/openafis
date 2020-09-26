#ifndef FINGERPRINT_H
#define FINGERPRINT_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Minutia.h"
#include "Triplet.h"

#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Only triplets are required for the production use-case of this library (efficient 1:N matching).
// In order to further reduce the memory required to store thousands of templates, fields required only for research or development purposes can be removed.
//
class Fingerprint
{
public:

#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
    using Minutiae = std::vector<Minutia>;
#endif
    using Triplets = std::vector<Triplet>;

    Fingerprint(const size_t size
#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
        , const Dimensions &dimensions
        , const Minutiae &minutiae
#endif
    )
#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
        : m_dimensions(dimensions)
        , m_minutiae(minutiae)
#endif
    {
        m_triplets.reserve(size);
    }

    Triplets &triplets() { return m_triplets; };
    const Triplets &triplets() const { return m_triplets; };

#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
    const Dimensions &dimensions() const { return m_dimensions; };
    const Minutiae &minutiae() const { return m_minutiae; };
#endif

private:   
    Triplets m_triplets;

#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
    const Dimensions m_dimensions;
    const Minutiae m_minutiae;
#endif
};

#endif // FINGERPRINT_H
