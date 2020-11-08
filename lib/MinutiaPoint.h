#ifndef MINUTIAPOINT_H
#define MINUTIAPOINT_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Dimensions.h"
#include "FastMath.h"
#include "Minutia.h"

#include <unordered_set>


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

        [[nodiscard]] inline const MinutiaPoint* probe() const { return m_probe; }
        [[nodiscard]] inline const MinutiaPoint* candidate() const { return m_candidate; }

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

    // co-ordinates (and therefore distances) are always scaled for 8-bits...
    MinutiaPoint(const Dimensions& dimensions, const Minutia& minutia)
        : m_x(static_cast<Field::MinutiaCoordSize>(std::lround(minutia.x() * (256.0f / dimensions.first))))
        , m_y(static_cast<Field::MinutiaCoordSize>(std::lround(minutia.y() * (256.0f / dimensions.second))))
        , m_angle(FastMath::degreesToRadians(minutia.angle()))
    {
    }

    MinutiaPoint() = default;

    [[nodiscard]] inline Field::MinutiaCoordType x() const { return m_x; }
    [[nodiscard]] inline Field::MinutiaCoordType y() const { return m_y; }
    [[nodiscard]] inline Field::AngleType angle() const { return m_angle; }
    [[nodiscard]] inline Field::MinutiaDistanceType distance() const { return m_distance; }
    [[nodiscard]] inline Field::MinutiaKeyType key() const { return static_cast<Field::MinutiaKeyType>(m_x) << 8 | m_y; }
    [[nodiscard]] size_t bytes() const { return sizeof(*this); }

    // distance between two vectors: a^2 + b^2 = c^2
    void setDistanceFrom(const MinutiaPoint& other)
    {
        const auto a = m_x - other.x();
        const auto b = m_y - other.y();
        m_distance = FastMath::isqrt(a * a + b * b);
    }

private:
    Field::MinutiaCoordSize m_x {}; // scaled for dimensions
    Field::MinutiaCoordSize m_y {}; // "
    Field::AngleSize m_angle {}; // radians [0,2PI] mapped over [0,255] (uint8_t)

    Field::MinutiaDistanceType m_distance {}; // distance from adjacent side, also scaled
};
}

#endif // MINUTIAPOINT_H
