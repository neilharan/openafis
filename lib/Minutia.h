#ifndef MINUTIA_H
#define MINUTIA_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Minutia
{
public:
    enum class Type {
        Invalid,
        RidgeEnding,
        RidgeBifurcation
    };

    Minutia(const Type type, const unsigned short x, const unsigned short y, const unsigned short angle)
        : m_type(type)
        , m_x(x)
        , m_y(y)
        , m_angle(angle) {}

    Type type() const { return m_type; }
    unsigned short x() const { return m_x; } // cm
    unsigned short y() const { return m_y; } // cm
    unsigned short angle() const { return m_angle; } // degrees
    
    // distance between two vectors: a^2 + b^2 = c^2
    double distanceTo(const std::pair<unsigned short, unsigned short>& resolution, const Minutia& other) const
    {
        const double distanceX = (m_x - other.x()) * (1.0f / static_cast<float>(resolution.first));
        const double distanceY = (m_y - other.y()) * (1.0f / static_cast<float>(resolution.second));
        return sqrt(distanceX * distanceX + distanceY * distanceY);
    }

private:
    const Type m_type;
    const unsigned short m_x;
    const unsigned short m_y;
    const unsigned short m_angle;
};

#endif // MINUTIA_H
