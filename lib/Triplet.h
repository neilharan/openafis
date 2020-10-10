#ifndef TRIPLET_H
#define TRIPLET_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Config.h"
#include "Field.h"
#include "Pair.h"

#include <array>
#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Triplet
{
public:
    class DupesHash
    {
    public:
        size_t operator()(const Field::MinutiaKeyType k) const { return k; } // replace std::unordered_set hash
    };

    using Distances = std::vector<Field::MinutiaCoordType>;
    using Dupes = std::pair<std::array<float, Field::MinutiaCoordMatrixSize>, std::array<float, Field::MinutiaCoordMatrixSize>>;

    explicit Triplet(const MinutiaPoint::Minutiae& minutiae);
    Triplet() = default;

    void emplacePair(Pair::Pairs& pairs, Triplet::Dupes& dupes, unsigned int& oust, const Triplet& probe) const;

    [[nodiscard]] const MinutiaPoint::Minutiae& minutiae() const { return m_minutiae; }
    [[nodiscard]] const Distances& distances() const { return m_distances; }
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
    static Distances sortDistances(const MinutiaPoint::Minutiae& minutiae);

    MinutiaPoint::Minutiae m_minutiae; // three points making up the triplet
    Distances m_distances {}; // max, mid, min side lengths respectively (sorted minutiae distances)
};

#endif // TRIPLET_H
