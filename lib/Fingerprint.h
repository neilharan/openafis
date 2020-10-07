#ifndef FINGERPRINT_H
#define FINGERPRINT_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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

    Fingerprint(const size_t minutiaeCount, const size_t tripletsCount
#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
        ,
        const Dimensions& dimensions, const Minutiae& minutiae
#endif
        )
        : m_minutiaeCount(minutiaeCount)
#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
        , m_dimensions(dimensions)
        , m_minutiae(minutiae)
#endif
    {
        m_triplets.reserve(tripletsCount);
    }

    [[nodiscard]] size_t minutiaeCount() const { return m_minutiaeCount; }
    [[nodiscard]] Triplets& triplets() { return m_triplets; }
    [[nodiscard]] const Triplets& triplets() const { return m_triplets; }

#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
    [[nodiscard]] const Dimensions& dimensions() const { return m_dimensions; }
    [[nodiscard]] const Minutiae& minutiae() const { return m_minutiae; }
#endif

    [[nodiscard]] size_t bytes() const;

private:
    size_t m_minutiaeCount;
    Triplets m_triplets;

#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
    Dimensions m_dimensions;
    Minutiae m_minutiae;
#endif
};

#endif // FINGERPRINT_H
