#ifndef LMTS_H
#define LMTS_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Config.h"
#include "Minutia.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class LMTS
{
public:
    LMTS(
        const unsigned char radial, const unsigned char angular, const unsigned char orientation
#ifdef OPENAFIS_LMTS_INCLUDE_MINUTIA
        , const Minutia &minutia
#endif
    )
        : m_radial(radial)
        , m_angular(angular)
        , m_orientation(orientation)
#ifdef OPENAFIS_LMTS_INCLUDE_MINUTIA
        , m_minutia(minutia)
#endif
        {}

    unsigned char radial() const { return m_radial; }
    unsigned char angular() const { return m_angular; }
    unsigned char orientation() const { return m_orientation; }

#ifdef OPENAFIS_LMTS_INCLUDE_MINUTIA
    const Minutia &minutia() const { return m_minutia; };
#endif

private:
    const unsigned char m_radial;
    const unsigned char m_angular;
    const unsigned char m_orientation;

#ifdef OPENAFIS_LMTS_INCLUDE_MINUTIA
    const Minutia m_minutia;
#endif
};

#endif // LMTS_H
