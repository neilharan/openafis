#ifndef TRIPLET_H
#define TRIPLET_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Config.h"
#include "Field.h"
#include "MinutiaPoint.h"

#include <vector>

#ifdef OPENAFIS_TRIPLETS_PACK
#define TRIPLET_PACK PACK
#else
#define TRIPLET_PACK
#endif


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// When packing is enabled a triplet can consume as little as three bytes but read accesses will be slightly slower - depending on platform architecture.
// With packing disabled triplets will consume 3 x architectures word size regardless but performance may be improved.
//
class Triplet
{
public:
    using Points = std::vector<std::pair<Field::TripletType, MinutiaPoint>>;

    Triplet() = default;

    Triplet(const Points& points);

private:
    Points sort(Points points);

    const Points m_points;
};

#endif // TRIPLET_H
