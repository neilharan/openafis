#ifndef MATCH_H
#define MATCH_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Template.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Match
{
public:
    static unsigned int compute(const Template& probe, const Template& candidate);
    static unsigned int compute(const Fingerprint& probe, const Fingerprint& candidate);
};

#endif // MATCH_H
