#ifndef MATCHMANY_H
#define MATCHMANY_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Match.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Instantiate on the heap.
// This class reserves a wide array to improve duplicate checking efficiency...
//
namespace OpenAFIS
{

template <class TemplateType> class MatchMany
{
public:
    using Templates = std::vector<TemplateType>;

    std::pair<unsigned int, const TemplateType*> compute(const TemplateType& probe, const Templates &candidates) const
    {
        static MatchSimilarity match;

        unsigned int maxSimilarity{};
        const TemplateType* maxCandidate{};

        for (const auto& candidate : candidates) {
            unsigned int similarity{};

            match.compute(similarity, probe.fingerprints()[0], candidate.fingerprints()[0]);
            if (similarity > maxSimilarity) {
                maxSimilarity = similarity;
                maxCandidate = &candidate;
            }
        }
        return std::make_pair(maxSimilarity, maxCandidate);
    }
};
}

#endif // MATCHMANY_H
