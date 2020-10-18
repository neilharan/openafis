#ifndef MINUTIA_H
#define MINUTIA_H


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

    Minutia(const Type type, const int x, const int y, const int angle)
        : m_type(type)
        , m_x(x)
        , m_y(y)
        , m_angle(angle)
    {
    }

    [[nodiscard]] Type type() const { return m_type; }
    [[nodiscard]] int x() const { return m_x; } // cm
    [[nodiscard]] int y() const { return m_y; } // "
    [[nodiscard]] int angle() const { return m_angle; } // degrees
    [[nodiscard]] size_t bytes() const { return sizeof(*this); }

private:
    Type m_type;
    int m_x;
    int m_y;
    int m_angle;
};
}

#endif // MINUTIA_H
