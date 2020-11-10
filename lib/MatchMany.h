#ifndef MATCHMANY_H
#define MATCHMANY_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "ThreadPool.h"

#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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

    [[nodiscard]] size_t concurrency() const { return m_pool.size(); }

private:
    mutable ThreadPool m_pool;
};
}

#endif // MATCHMANY_H
