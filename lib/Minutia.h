#ifndef MINUTIA_H
#define MINUTIA_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// This class is used when reading template binaries and is not held in memory during scoring.
// As the size is not sensitive the definition uses types that suit the target architecture.
//
namespace OpenAFIS
{

class Minutia
{
public:
    enum class Type { Invalid, RidgeEnding, RidgeBifurcation };

    Minutia(const Type type, const uint16_t x, const uint16_t y, const uint16_t angle)
        : m_type(type)
        , m_x(x)
        , m_y(y)
        , m_angle(angle)
    {
    }

    [[nodiscard]] Type type() const { return m_type; }
    [[nodiscard]] uint16_t x() const { return m_x; } // cm
    [[nodiscard]] uint16_t y() const { return m_y; } // "
    [[nodiscard]] uint16_t angle() const { return m_angle; } // degrees
    [[nodiscard]] size_t bytes() const { return sizeof(*this); }

private:
    Type m_type;
    uint16_t m_x;
    uint16_t m_y;
    uint16_t m_angle;
};
}

#endif // MINUTIA_H
