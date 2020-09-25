#ifndef TRIPLET_H
#define TRIPLET_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Config.h"
#include "Field.h"

#ifdef OPENAFIS_TRIPLETS_PACK
#define TRIPLET_PACK PACK
#else
#define TRIPLET_PACK
#endif


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// When packing is enabled a triplet can consume as little as three bytes but read accesses will be slightly slower - depending on platform architecture.
// With packing disabled triplets will consume 3 x architectures word size regardless but performance may be improved.
//
class Triplet
{
public:
    Triplet() = default;

    Triplet(const Field::Triplet a, const Field::Triplet b, const Field::Triplet c)
        : m_data(a, b, c)
        {}

    Field::Triplet a() const { return m_data.a; }
    Field::Triplet b() const { return m_data.b; }
    Field::Triplet c() const { return m_data.c; }

private:
    //NJH-TODO TRIPLET_PACK(
        
    struct Data
    {
        Data() = default;

        Data(const Field::Triplet a, const Field::Triplet b, const Field::Triplet c)
            : a(a)
            , b(b)
            , c(c)
            {}

        Field::Triplet a{};
        Field::Triplet b{};
        Field::Triplet c{};
    } m_data;
};

#endif // TRIPLET_H
