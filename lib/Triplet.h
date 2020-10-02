#ifndef TRIPLET_H
#define TRIPLET_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Field.h"
#include "Log.h"
#include "MinutiaPoint.h"

#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Triplet
{
public:
    using Minutiae = std::vector<MinutiaPoint>;
    using Distances = std::vector<Field::TripletCoordType>;
    using Shift = std::vector<unsigned int>;

    class Pair
    {
    public:
        Pair(const float similarity, const Triplet* probe, const Triplet* candidate, const Shift* shift)
            : m_similarity(similarity)
            , m_probe(probe)
            , m_candidate(candidate)
            , m_shift(shift)
        {
        }

        [[nodiscard]] float similarity() const { return m_similarity; }
        [[nodiscard]] const Triplet* probe() const { return m_probe; }
        [[nodiscard]] const Triplet* candidate() const { return m_candidate; }
        [[nodiscard]] const Triplet::Shift* shift() const { return m_shift; }

        bool operator<(const Pair& other) const { return m_similarity > other.m_similarity; } // descending sort

    private:
        float m_similarity {};
        const Triplet* m_probe {};
        const Triplet* m_candidate {};
        const Triplet::Shift* m_shift {};
    };

    using Pairs = std::vector<Pair>;

    explicit Triplet(const Minutiae& minutiae);
    Triplet() = default;

    void emplacePair(Pairs& pairs, const Triplet& other) const;

    [[nodiscard]] const Minutiae& minutiae() const { return m_minutiae; }
    [[nodiscard]] const Distances& distances() const { return m_distances; }
    [[nodiscard]] size_t bytes() const;

    bool operator<(const Triplet& other) const { return m_distances[0] < other.m_distances[0]; }
    friend bool operator<(const Triplet& lhs, const Field::TripletCoordType rhs) { return lhs.m_distances[0] < rhs; }
    friend bool operator<(const Field::TripletCoordType lhs, const Triplet& rhs) { return lhs < rhs.m_distances[0]; }
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
