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

    std::pair<unsigned int, const TemplateType*> compute(const TemplateType& probe, const Templates &candidates) const;
};
}

#endif // MATCHMANY_H
