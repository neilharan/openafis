#ifndef TRIPLET_H
#define TRIPLET_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Config.h"
#include "Field.h"
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
        Pair() = default;

        Pair(const float similarity, Triplet* probe, Triplet* candidate, Shift* shift)
            : m_similarity(similarity)
            , m_probe(probe)
            , m_candidate(candidate)
            , m_shift(shift)
        {
        }

        float similarity() const { return m_similarity; }
        const Triplet* probe() const { return m_probe; }
        const Triplet* candidate() const { return m_candidate; }
        const Triplet::Shift* shift() const { return m_shift; }

    private:
        float m_similarity {};
        Triplet* m_probe {};
        Triplet* m_candidate {};
        Triplet::Shift* m_shift {};
    };

    using Pairs = std::vector<Pair>;

    Triplet() = default;

    explicit Triplet(const Minutiae& minutiae);
    // NJH-TODO ~Triplet() {}

    // NJH-TODO Triplet(Triplet const &) = delete;
    // NJH-TODO void operator=(Triplet const &) = delete;

    Pair findPair(const Triplet& other) const;

    const Minutiae& minutiae() const { return m_minutiae; }
    const Distances& distances() const { return m_distances; }
    size_t bytes() const;

    bool operator<(const Triplet& other) { return m_distances[0] < other.m_distances[0]; }
    friend bool operator<(const Triplet& lhs, const Field::TripletCoordType rhs) { return lhs.m_distances[0] < rhs; }
    friend bool operator<(const Field::TripletCoordType lhs, const Triplet& rhs) { return lhs < rhs.m_distances[0]; }

private:
    static Minutiae shiftClockwise(Minutiae minutiae);
    static Distances sortDistances(const Minutiae& minutiae);

    Minutiae m_minutiae;
    Distances m_distances {}; // max, mid, min side lengths respectively (sorted minutiae distances)
};

#endif // TRIPLET_H
