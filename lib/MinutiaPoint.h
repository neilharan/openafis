#ifndef MINUTIAPOINT_H
#define MINUTIAPOINT_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Dimensions.h"
#include "FastMath.h"
#include "Minutia.h"

#include <unordered_set>
#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// When packing is enabled a triplet can consume as little as three bytes but read accesses will be slightly slower - depending on platform architecture.
// With packing disabled triplets will consume 3 x architectures word size regardless but scoring performance may be improved.
//
namespace OpenAFIS
{

class MinutiaPoint
{
public:
    class Pair
    {
    public:
        Pair(const MinutiaPoint* probe, const MinutiaPoint* candidate)
            : m_probe(probe)
            , m_candidate(candidate)
        {
        }

        [[nodiscard]] const MinutiaPoint* probe() const { return m_probe; }
        [[nodiscard]] const MinutiaPoint* candidate() const { return m_candidate; }

    private:
        const MinutiaPoint* m_probe {};
        const MinutiaPoint* m_candidate {};
    };

    class PairRenderable : public Pair
    {
    public:
        using Set = std::unordered_set<const PairRenderable*>;

        PairRenderable(const MinutiaPoint* probe, const MinutiaPoint* candidate, const int similarity)
            : Pair(probe, candidate)
            , m_similarity(similarity)
        {
        }

        [[nodiscard]] int similarity() const { return m_similarity; }

    private:
        int m_similarity {};
    };

    using Minutiae = std::vector<MinutiaPoint>;

    // co-ordinates (and therefore distances) are always scales for 8-bits...
    MinutiaPoint(const Dimensions& dimensions, const Minutia& minutia)
        : m_x(static_cast<Field::MinutiaCoordType>(std::lround(static_cast<float>(minutia.x()) * (256.0f / dimensions.first))))
        , m_y(static_cast<Field::MinutiaCoordType>(std::lround(static_cast<float>(minutia.y()) * (256.0f / dimensions.second))))
        , m_angle(FastMath::degreesToRadians(minutia.angle()))
    {
    }

    [[nodiscard]] Field::MinutiaCoordType x() const { return m_x; }
    [[nodiscard]] Field::MinutiaCoordType y() const { return m_y; }
    [[nodiscard]] Field::AngleType angle() const { return m_angle; }
    [[nodiscard]] Field::MinutiaDistanceType distance() const { return m_distance; }
    [[nodiscard]] Field::MinutiaKeyType key() const { return static_cast<Field::MinutiaKeyType>(m_x) << 8 | m_y; }
    [[nodiscard]] size_t bytes() const { return sizeof(*this); }

    // distance between two vectors: a^2 + b^2 = c^2
    void setDistanceFrom(const MinutiaPoint& other)
    {
        const auto a = static_cast<int>(m_x) - other.x();
        const auto b = static_cast<int>(m_y) - other.y();
        m_distance = static_cast<Field::MinutiaDistanceType>(FastMath::isqrt(a * a + b * b));
    }

private:
    Field::MinutiaCoordType m_x; // scaled for dimensions
    Field::MinutiaCoordType m_y; // "
    Field::AngleType m_angle; // radians [0,2PI] mapped over [0,255] (uint8_t)

    Field::MinutiaDistanceType m_distance {}; // distance from adjacent side, also scaled
};
}

#endif // MINUTIAPOINT_H
