#ifndef TRIPLETSIMD_H
#define TRIPLETSIMD_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Field.h"
#include "MinutiaPoint.h"
#include "Triplet.h"

#include <unordered_set>
#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{

class TripletSIMD : public Triplet
{
public:
    using Distances = std::vector<int8_t>;

    explicit TripletSIMD(const Minutiae& minutiae);

    [[nodiscard]] inline bool skipPair(const TripletSIMD&) const { return false; }
    void emplacePair(Pair::Pairs& pairs, const TripletSIMD& probe) const;

    [[nodiscard]] inline Field::MinutiaDistanceType maxDistance() const { return static_cast<Field::MinutiaDistanceType>(m_distances[0]); }
    [[nodiscard]] size_t bytes() const { return sizeof(*this) + m_distances.capacity() * sizeof(decltype(m_distances[0])); }

    bool operator<(const TripletSIMD& other) const;
    friend bool operator<(const TripletSIMD& lhs, const Field::MinutiaCoordType rhs) { return lhs.m_distances[0] < rhs; } // lower_bound/upper_bound (the binary search)
    friend bool operator<(const Field::MinutiaCoordType lhs, const TripletSIMD& rhs) { return lhs < rhs.m_distances[0]; } // "

private:
    void sortDistances();

    [[nodiscard]] inline const int8_t* distances() const { return m_distances.data(); }

    Distances m_distances; // max, mid, min side lengths respectively (sorted minutiae distances)
};
}

#endif // TRIPLETSIMD_H
