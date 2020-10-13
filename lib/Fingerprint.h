#ifndef FINGERPRINT_H
#define FINGERPRINT_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Triplet.h"

#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{

class Fingerprint
{
public:
    using Minutiae = std::vector<Minutia>;
    using Triplets = std::vector<Triplet>;

    Fingerprint(const size_t minutiaeCount, const size_t tripletsCount)
        : m_minutiaeCount(minutiaeCount)
    {
        m_triplets.reserve(tripletsCount);
    }

    [[nodiscard]] size_t minutiaeCount() const { return m_minutiaeCount; }
    [[nodiscard]] Triplets& triplets() { return m_triplets; }
    [[nodiscard]] const Triplets& triplets() const { return m_triplets; }
    [[nodiscard]] size_t bytes() const;

private:
    size_t m_minutiaeCount;
    Triplets m_triplets;
};


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class FingerprintRenderable : public Fingerprint
{
public:
    FingerprintRenderable(const size_t minutiaeCount, const size_t tripletsCount, Dimensions dimensions, Minutiae minutiae)
        : Fingerprint(minutiaeCount, tripletsCount)
        , m_dimensions(std::move(dimensions))
        , m_minutiae(std::move(minutiae))
    {
    }

    [[nodiscard]] const Dimensions& dimensions() const { return m_dimensions; }
    [[nodiscard]] const Minutiae& minutiae() const { return m_minutiae; }
    [[nodiscard]] size_t bytes() const;

private:
    Dimensions m_dimensions;
    Minutiae m_minutiae;
};
}

#endif // FINGERPRINT_H
