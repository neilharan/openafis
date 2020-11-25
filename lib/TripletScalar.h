#ifndef TRIPLETSCALAR_H
#define TRIPLETSCALAR_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Field.h"
#include "MinutiaPoint.h"
#include "Param.h"
#include "Triplet.h"

#include <unordered_set>
#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{

class TripletScalar : public Triplet
{
public:
    explicit TripletScalar(const Minutiae& minutiae);

    [[nodiscard]] bool skipPair(const TripletScalar& probe) const;
    void emplacePair(Pair::Pairs& pairs, const TripletScalar& probe) const;
    [[nodiscard]] inline Field::MinutiaDistanceType maxDistance() const { return static_cast<Field::MinutiaDistanceType>(m_distances); }
    [[nodiscard]] size_t bytes() const { return sizeof(*this); };

    bool operator<(const TripletScalar& other) const;
    friend bool operator<(const TripletScalar& lhs, const Field::MinutiaDistanceType rhs)
    {
        return static_cast<Field::MinutiaDistanceType>(lhs.m_distances) < rhs;
    } // lower_bound/upper_bound (the binary search)
    friend bool operator<(const Field::MinutiaDistanceType lhs, const TripletScalar& rhs) { return lhs < static_cast<Field::MinutiaDistanceType>(rhs.m_distances); } // "

private:
    uint32_t m_distances {}; // (min << 16 | mid << 8 | max) side lengths respectively (sorted minutiae distances)
};
}

#endif // TRIPLETSCALAR_H
