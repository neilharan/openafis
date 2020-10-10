#ifndef CONFIG_H
#define CONFIG_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define OPENAFIS_FINGERPRINT_RENDERABLE
#define OPENAFIS_TRIPLET_RENDERABLE

#if defined(OPENAFIS_FINGERPRINT_RENDERABLE) && defined(OPENAFIS_TRIPLET_RENDERABLE)
#define OPENAFIS_RENDER_AVAILABLE
#endif

// #define OPENAFIS_TRIPLETS_PACK


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef __GNUC__
#define PACK(__decl__) __decl__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK(__decl__) __pragma(pack(push, 1)) __decl__ __pragma(pack(pop))
#endif

#endif // CONFIG_H
