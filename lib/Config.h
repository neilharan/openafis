#ifndef CONFIG_H
#define CONFIG_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define OPENAFIS_LMTS_INCLUDE_MINUTIA
#define OPENAFIS_FINGERPRINT_INCLUDE_DIMENSIONS

#if defined(OPENAFIS_LMTS_INCLUDE_MINUTIA) && defined(OPENAFIS_FINGERPRINT_INCLUDE_DIMENSIONS)
#define OPENAFIS_RENDER_AVAILABLE
#endif

#endif // CONFIG_H
