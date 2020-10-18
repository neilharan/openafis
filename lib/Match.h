#ifndef MATCH_H
#define MATCH_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Template.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Instantiate on the heap.
// This class reserves a wide array to improve duplicate checking efficiency...
//
namespace OpenAFIS
{

template <class ResultType, class FingerprintType, class PairType> class Match
{
public:
    Match()
    {
        m_tripletPairs.reserve(100);
        m_pairs.reserve(100);
    }

    void compute(ResultType& result, const FingerprintType& probe, const FingerprintType& candidate) const;

private:
    mutable Triplet::Pair::Pairs m_tripletPairs;
    mutable Triplet::Dupes m_probeDupes;
    mutable Triplet::Dupes m_candidateDupes;
    mutable std::vector<PairType> m_pairs;
};

using MatchSimilarity = Match<int, Fingerprint, MinutiaPoint::Pair>;
using MatchRenderable = Match<MinutiaPoint::PairRenderable::Set, FingerprintRenderable, MinutiaPoint::PairRenderable>;
}

#endif // MATCH_H
