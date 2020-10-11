#ifndef MINUTIAPOINT_H
#define MINUTIAPOINT_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Config.h"
#include "Dimensions.h"
#include "FastMath.h"
#include "Minutia.h"

#include <unordered_set>
#include <vector>

// NJH-TODO
#ifdef OPENAFIS_TRIPLETS_PACK
#define TRIPLET_PACK PACK
#else
#define TRIPLET_PACK
#endif


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// When packing is enabled a triplet can consume as little as three bytes but read accesses will be slightly slower - depending on platform architecture.
// With packing disabled triplets will consume 3 x architectures word size regardless but scoring performance may be improved.
//
class MinutiaPoint
{
public:
    class Pair
    {
    public:
        using Pairs = std::vector<Pair>;
        using Set = std::unordered_set<const Pair*>;

        Pair(const MinutiaPoint* probe, const MinutiaPoint* candidate
#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
            , const float similarity
#endif
        )
            : m_probe(probe)
            , m_candidate(candidate)
#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
            , m_similarity(similarity)
#endif
        {
        }

        [[nodiscard]] const MinutiaPoint* probe() const { return m_probe; }
        [[nodiscard]] const MinutiaPoint* candidate() const { return m_candidate; }
#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
        [[nodiscard]] float similarity() const { return m_similarity; }
#endif

    private:
        const MinutiaPoint* m_probe {};
        const MinutiaPoint* m_candidate {};
#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
        float m_similarity {};
#endif
    };

    using Minutiae = std::vector<MinutiaPoint>;

    // co-ordinates (and therefore distances) are always scales for 8-bits...
    MinutiaPoint(const Dimensions& dimensions, const Minutia& minutia)
        : m_x(static_cast<Field::MinutiaCoordType>(FastMath::round(static_cast<float>(minutia.x()) * (256.0f / dimensions.first))))
        , m_y(static_cast<Field::MinutiaCoordType>(FastMath::round(static_cast<float>(minutia.y()) * (256.0f / dimensions.second))))
        , m_angle(FastMath::degreesToRadians(minutia.angle()))
    {
    }

    [[nodiscard]] Field::MinutiaCoordType x() const { return m_x; }
    [[nodiscard]] Field::MinutiaCoordType y() const { return m_y; }
    [[nodiscard]] float angle() const { return m_angle; }
    [[nodiscard]] Field::MinutiaCoordType distance() const { return m_distance; }
    [[nodiscard]] Field::MinutiaKeyType key() const { return static_cast<Field::MinutiaKeyType>(m_x) << 8 | m_y; }
    [[nodiscard]] size_t bytes() const { return sizeof(*this); }

    // distance between two vectors: a^2 + b^2 = c^2
    void setDistanceFrom(const MinutiaPoint& other)
    {
        const auto a = static_cast<int>(m_x) - other.x();
        const auto b = static_cast<int>(m_y) - other.y();
        m_distance = static_cast<Field::MinutiaCoordType>(FastMath::isqrt(a * a + b * b));
    }

private:
    Field::MinutiaCoordType m_x; // scaled for dimensions
    Field::MinutiaCoordType m_y; // "
    float m_angle; // radians

    Field::MinutiaCoordType m_distance {}; // distance from adjacent side, also scaled
};

#endif // MINUTIAPOINT_H
