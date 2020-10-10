#ifndef PAIR_H
#define PAIR_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Config.h"
#include "MinutiaPoint.h"

#include <unordered_set>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Pair
{
public:
    using Pairs = std::vector<Pair>;
    using Set = std::unordered_set<const Pair*>;

    Pair(const float similarity, const MinutiaPoint* probe, const MinutiaPoint* candidate)
        : m_similarity(similarity)
        , m_probe(probe)
        , m_candidate(candidate)
    {
    }

    [[nodiscard]] float similarity() const { return m_similarity; }
    [[nodiscard]] const MinutiaPoint* probe() const { return m_probe; }
    [[nodiscard]] const MinutiaPoint* candidate() const { return m_candidate; }

    bool operator<(const Pair& other) const { return m_similarity > other.m_similarity; } // descending sort

private:
    float m_similarity {};
    const MinutiaPoint* m_probe {};
    const MinutiaPoint* m_candidate {};
};

#endif // PAIR_H
