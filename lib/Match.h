#ifndef MATCH_H
#define MATCH_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Config.h"
#include "Template.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Instantiate on the heap.
// This class reserves a wide array to improve duplicate checking efficiency...
//
template <typename T> class Match
{
public:
    Match() { m_pairs.reserve(64); }

    void compute(const Template& probe, const Template& candidate) const;
    void compute(T& result, const Fingerprint& probe, const Fingerprint& candidate) const;

private:
    mutable Pair::Pairs m_pairs;
    mutable Triplet::Dupes m_dupes;
};

#endif // MATCH_H
