#ifndef FINGERPRINT_H
#define FINGERPRINT_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Param.h"
#include "TripletSIMD.h"
#include "TripletScalar.h"

#include <numeric>
#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{

class Fingerprint
{
public:
    using TripletType = std::conditional<Param::EnableSIMD, TripletSIMD, TripletScalar>::type;
    using Triplets = std::vector<TripletType>;
    using Minutiae = std::vector<Minutia>;

    Fingerprint(const size_t minutiaeCount, const size_t tripletsCount)
        : m_minutiaeCount(minutiaeCount)
    {
        m_triplets.reserve(tripletsCount);
    }

    [[nodiscard]] size_t minutiaeCount() const { return m_minutiaeCount; }
    [[nodiscard]] Triplets& triplets() { return m_triplets; }
    [[nodiscard]] const Triplets& triplets() const { return m_triplets; }
    [[nodiscard]] size_t bytes() const
    {
        return sizeof(*this) + std::accumulate(m_triplets.begin(), m_triplets.end(), size_t {}, [](size_t sum, const auto& t) { return sum + t.bytes(); });
    }

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
    [[nodiscard]] size_t bytes() const
    {
        return Fingerprint::bytes() + std::accumulate(m_minutiae.begin(), m_minutiae.end(), size_t {}, [](size_t sum, const auto& m) { return sum + m.bytes(); });
    }

private:
    Dimensions m_dimensions;
    Minutiae m_minutiae;
};
}

#endif // FINGERPRINT_H
