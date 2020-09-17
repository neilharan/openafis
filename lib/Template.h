#ifndef TEMPLATE_H
#define TEMPLATE_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef __GNUC__
#define PACK(__decl__) __decl__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK(__decl__) __pragma(pack(push, 1)) __decl__ __pragma(pack(pop))
#endif


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// templated identifier might be a std::string for test purposes but a short for more constrained systems...
//
template <class T> class Template
{
public:
    class LMTS
    {
    public:
        LMTS(const unsigned char radial, const unsigned char angular, const unsigned char orientation)
            : m_radial(radial)
            , m_angular(angular)
            , m_orientation(orientation) {}

        unsigned char radial() const { return m_radial; }
        unsigned char angular() const { return m_angular; }
        unsigned char orientation() const { return m_orientation; }

    private:
        const unsigned char m_radial;
        const unsigned char m_angular;
        const unsigned char m_orientation;
    };

    using Fingerprint = std::vector<std::vector<LMTS>>;
   
    explicit Template(const T &id):
        m_data(id) {}

    const T &id() const { return m_data.id; }
    const std::vector<Fingerprint> &fingerprints() const { return m_data.fps; }
    void clear() { m_data.fps.clear(); }
    size_t size() const;

protected:
    static const size_t MaximumFingerprints = 8;
    static const size_t MaximumMinutiae = 128;

    class Minutia
    {
    public:
        Minutia(const unsigned short x, const unsigned short y, const unsigned char angle)
            : m_x(x)
            , m_y(y)
            , m_angle(angle) {}

        unsigned short x() const { return m_x; } // cm
        unsigned short y() const { return m_y; } // cm
        unsigned char angle() const { return m_angle; }
    
        // distance between two vectors: a^2 + b^2 = c^2
        friend double operator-(const Minutia& lhs, const Minutia& rhs)
        {
            static const double Scale = 1.0 / 197.0; // NJH-TODO compute when loading & scale for uint16 here

            const double distanceX = (lhs.x() - rhs.x()) * Scale;
            const double distanceY = (lhs.y() - rhs.y()) * Scale;
            return sqrt(distanceX * distanceX + distanceY * distanceY);
        }

    private:   
        const unsigned short m_x;
        const unsigned short m_y;
        const unsigned char m_angle;
    };

    bool load(const std::vector<std::vector<Minutia>> &fps);
    
private:
    PACK(struct Data
    {
        explicit Data(const T &id):
            id(id) {}

        std::vector<Fingerprint> fps; // [fingerprint][minutia][LMTS]
        const T id;
    }) m_data;
};

#endif // TEMPLATE_H
