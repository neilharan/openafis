
#include "Fingerprint.h"

#include <numeric>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t Fingerprint::bytes() const
{
    return sizeof(*this) + std::accumulate(m_triplets.begin(), m_triplets.end(), size_t {}, [](size_t sum, const auto& t) { return sum + t.bytes(); });
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t FingerprintRenderable::bytes() const
{
    return Fingerprint::bytes() + std::accumulate(m_minutiae.begin(), m_minutiae.end(), size_t {}, [](size_t sum, const auto& m) { return sum + m.bytes(); });
}
}
