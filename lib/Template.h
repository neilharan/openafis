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
   
    Template(const T &id):
        m_data(id) {}

    const T &id() const { return m_data.id; }
    const std::vector<std::vector<std::vector<LMTS>>> &lmts() const { return m_data.lmts; }
    void clear() { m_data.lmts.clear(); }
    size_t size() const;

protected:
    class Minutia
    {
    public:
        Minutia(const unsigned short abscissa, const unsigned short ordinate, const unsigned char orientation)
            : m_abscissa(abscissa)
            , m_ordinate(ordinate)
            , m_orientation(orientation) {}

        unsigned short abscissa() const { return m_abscissa; } // aka x
        unsigned short ordinate() const { return m_ordinate; } // aka y
        unsigned char orientation() const { return m_orientation; } // aka angle
    
    private:   
        const unsigned short m_abscissa;
        const unsigned short m_ordinate;
        const unsigned char m_orientation;
    };

    bool load(const std::pair<unsigned short, unsigned short> &resolution, const std::vector<std::vector<Minutia>> &fps);
    
private:
    PACK(struct Data
    {
        Data(const T &id):
            id(id) {}

        std::vector<std::vector<std::vector<LMTS>>> lmts; // [fingerprint][minutia][LMTS]
        const T id;
    }) m_data;
};

#endif // TEMPLATE_H
