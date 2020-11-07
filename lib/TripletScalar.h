#ifndef TRIPLETSCALAR_H
#define TRIPLETSCALAR_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Field.h"
#include "MinutiaPoint.h"
#include "Triplet.h"

#include <unordered_set>
#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{

class TripletScalar : public Triplet
{
public:
    using Distances = std::array<Field::MinutiaDistanceType, 3>;

    explicit TripletScalar(const Minutiae& minutiae);

    [[nodiscard]] bool skipPair(const TripletScalar& probe) const;
    void emplacePair(Pair::Pairs& pairs, const TripletScalar& probe) const;

    [[nodiscard]] inline Field::MinutiaDistanceType maxDistance() const { return static_cast<Field::MinutiaDistanceType>(m_distances[0]); }
    [[nodiscard]] size_t bytes() const { return sizeof(*this); };

    bool operator<(const TripletScalar& other) const;
    friend bool operator<(const TripletScalar& lhs, const Field::MinutiaCoordType rhs) { return lhs.m_distances[0] < rhs; } // lower_bound/upper_bound (the binary search)
    friend bool operator<(const Field::MinutiaCoordType lhs, const TripletScalar& rhs) { return lhs < rhs.m_distances[0]; } // "

private:
    void sortDistances();

    [[nodiscard]] inline const Distances& distances() const { return m_distances; }

    Distances m_distances { 0, 0, 0 }; // max, mid, min side lengths respectively (sorted minutiae distances)
};
}

#endif // TRIPLETSCALAR_H
