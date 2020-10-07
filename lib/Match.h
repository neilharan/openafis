#ifndef MATCH_H
#define MATCH_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Config.h"
#include "Template.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Match
{
public:
    Match()
    {
        m_pairs.reserve(64);
        m_dupes.reserve(64);
    }

    unsigned int compute(const Template& probe, const Template& candidate) const;
    unsigned int compute(const Fingerprint& probe, const Fingerprint& candidate) const;

private:
    mutable Triplet::Pairs m_pairs;
    mutable Triplet::Dupes m_dupes;
};

#endif // MATCH_H
