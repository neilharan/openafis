
#include "Fingerprint.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t Fingerprint::bytes() const
{
    size_t sz = sizeof(*this);
    for (const auto& t : m_triplets) {
        sz += t.bytes();
    }
#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
    for (const auto& m : m_minutiae) {
        sz += m.bytes();
    }
#endif
    return sz;
}
