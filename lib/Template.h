#ifndef TEMPLATE_H
#define TEMPLATE_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Fingerprint.h"
#include "Minutia.h"


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
    using Resolution = std::pair<unsigned short, unsigned short>;

    explicit Template(const T &id):
        m_data(id) {}

    const T &id() const { return m_data.id; }
    const std::vector<Fingerprint> &fingerprints() const { return m_data.fps; }
    void clear() { m_data.fps.clear(); }
    size_t size() const;

protected:
    static const size_t MaximumFingerprints = 8;
    static const size_t MaximumMinutiae = 128;

    bool load(const Fingerprint::Dimensions& dimensions, const Resolution& resolution, const std::vector<std::vector<Minutia>>& fps);
    
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
