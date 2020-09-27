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

    Triplet() = default;

    Triplet(const Minutiae& minutiae);

    int score(const Triplet &other) const;

    const Minutiae &minutiae() const { return m_minutiae; }

private:
    static Minutiae sort(Minutiae minutiae);

    const Minutiae m_minutiae;
};

#endif // TRIPLET_H
