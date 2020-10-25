#ifndef TRIPLET_H
#define TRIPLET_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Field.h"
#include "MinutiaPoint.h"

#include <unordered_set>
#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{

class Triplet
{
public:
    class Pair
    {
    public:
        using Pairs = std::vector<Pair>;

        Pair(const int similarity, const Triplet* probe, const Triplet* candidate)
            : m_similarity(similarity)
            , m_probe(probe)
            , m_candidate(candidate)
        {
        }

        [[nodiscard]] inline int similarity() const { return m_similarity; }
        [[nodiscard]] inline const Triplet* probe() const { return m_probe; }
        [[nodiscard]] inline const Triplet* candidate() const { return m_candidate; }

        bool operator<(const Pair& other) const { return m_similarity > other.m_similarity; } // descending sort

        static const int SimilarityMultiplier = 1024;

    private:
        int m_similarity {}; // only required for sorting (getter is for Render class only)
        const Triplet* m_probe {};
        const Triplet* m_candidate {};
    };

    using Distances = std::vector<Field::MinutiaDistanceType>;
    using Dupes = std::unordered_set<Field::MinutiaKeyType>;

    explicit Triplet(const MinutiaPoint::Minutiae& minutiae);
    Triplet() = default;

    [[nodiscard]] bool skipPair(const Triplet& probe) const;
    void emplacePair(Pair::Pairs& pairs, const Triplet& probe) const;

    [[nodiscard]] inline const MinutiaPoint::Minutiae& minutiae() const { return m_minutiae; }
    [[nodiscard]] inline const Distances& distances() const { return m_distances; }
    [[nodiscard]] size_t bytes() const;

    bool operator<(const Triplet& other) const;
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
    static MinutiaPoint::Minutiae shiftClockwise(MinutiaPoint::Minutiae minutiae);
    void sortDistances();

    MinutiaPoint::Minutiae m_minutiae; // three points making up the triplet
    Distances m_distances {}; // max, mid, min side lengths respectively (sorted minutiae distances)
};
}

#endif // TRIPLET_H
