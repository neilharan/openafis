#ifndef TRIPLET_H
#define TRIPLET_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "MinutiaPoint.h"

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

    using Minutiae = std::array<MinutiaPoint, 3>;

    explicit Triplet(const Minutiae& minutiae);
    Triplet() = default;

    [[nodiscard]] inline const Minutiae& minutiae() const { return m_minutiae; }

protected:
    Minutiae m_minutiae; // three points making up the triplet

private:
    static Minutiae shiftClockwise(Minutiae minutiae);
};
}

#endif // TRIPLET_H
