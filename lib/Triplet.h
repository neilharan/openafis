#ifndef TRIPLET_H
#define TRIPLET_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Field.h"
#include "Log.h"
#include "MinutiaPoint.h"

#include <unordered_set>
#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Triplet
{
public:
    class Hash
    {
    public:
        size_t operator()(const Field::MinutiaKeyType k) const { return k; } // replace std::unordered_set hash
    };

    using Minutiae = std::vector<MinutiaPoint>;
    using Distances = std::vector<Field::MinutiaCoordType>;
    using Dupes = std::unordered_set<Field::MinutiaKeyType, Hash>;
    using Shift = std::vector<unsigned int>;

    class Pair
    {
    public:
        Pair(const float similarity, const Triplet* probe, const Triplet* candidate)
            : m_similarity(similarity)
            , m_probe(probe)
            , m_candidate(candidate)
        {
        }

        [[nodiscard]] float similarity() const { return m_similarity; }
        [[nodiscard]] const Triplet* probe() const { return m_probe; }
        [[nodiscard]] const Triplet* candidate() const { return m_candidate; }

        bool operator<(const Pair& other) const { return m_similarity > other.m_similarity; } // descending sort

    private:
        float m_similarity {};
        const Triplet* m_probe {};
        const Triplet* m_candidate {};
    };

    using Pairs = std::vector<Pair>;

    explicit Triplet(const Minutiae& minutiae);
    Triplet() = default;

    void emplacePair(Pairs& pairs, Triplet::Dupes& dupes, const Triplet& probe) const;

    [[nodiscard]] const Minutiae& minutiae() const { return m_minutiae; }
    [[nodiscard]] const Distances& distances() const { return m_distances; }
    [[nodiscard]] size_t bytes() const;

    bool operator<(const Triplet& other) const { return m_distances[0] < other.m_distances[0]; }
    friend bool operator<(const Triplet& lhs, const Field::MinutiaCoordType rhs) { return lhs.m_distances[0] < rhs; }
    friend bool operator<(const Field::MinutiaCoordType lhs, const Triplet& rhs) { return lhs < rhs.m_distances[0]; }
    friend void swap(Triplet& lhs, Triplet& rhs) noexcept
    {
        thread_local static Triplet tmp; // minimize dtor (improve sort performance)
        tmp = std::move(lhs);
        lhs = std::move(rhs);
        rhs = std::move(tmp);
    }

private:
    static Minutiae shiftClockwise(Minutiae minutiae);
    static Distances sortDistances(const Minutiae& minutiae);

    Minutiae m_minutiae;
    Distances m_distances {}; // max, mid, min side lengths respectively (sorted minutiae distances)
};

#endif // TRIPLET_H
