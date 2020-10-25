#ifndef MATCHMANY_H
#define MATCHMANY_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Match.h"
#include "Param.h"


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
    using OneManyResult = std::pair<uint8_t, const TemplateType*>;

    MatchMany();

    OneManyResult oneMany(const TemplateType& probe, const Templates& candidates) const;
    void manyMany(std::vector<uint8_t>& scores, const Templates& templates) const;

    [[nodiscard]] unsigned int concurrency() const { return m_concurrency; }

private:
    const unsigned int m_concurrency;
};
}

#endif // MATCHMANY_H
