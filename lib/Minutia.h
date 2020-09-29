#ifndef MINUTIA_H
#define MINUTIA_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <algorithm>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// This class is used when reading template binaries and is not held in memory during scoring.
// As the size is not sensitive the definition uses types that suit the target architecture.
//
class Minutia
{
public:
    enum class Type { Invalid, RidgeEnding, RidgeBifurcation };

    Minutia(const Type type, const unsigned int x, const unsigned int y, const unsigned int angle)
        : m_type(type)
        , m_x(x)
        , m_y(y)
        , m_angle(angle)
    {
    }

    Type type() const { return m_type; }
    unsigned int x() const { return m_x; } // cm
    unsigned int y() const { return m_y; } // "
    unsigned int angle() const { return m_angle; } // degrees
    size_t bytes() const { return sizeof(*this); }

private:
    Type m_type;
    unsigned int m_x;
    unsigned int m_y;
    unsigned int m_angle;
};

#endif // MINUTIA_H
