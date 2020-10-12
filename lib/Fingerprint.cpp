
#include "Fingerprint.h"

#include <numeric>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t Fingerprint::bytes() const
{
    return sizeof(*this) + std::accumulate(m_triplets.begin(), m_triplets.end(), 0, [](int sum, const auto& t) { return sum + t.bytes(); })
#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
        + std::accumulate(m_minutiae.begin(), m_minutiae.end(), 0, [](int sum, const auto& m) { return sum + m.bytes(); })
#endif
        ;
}
}
