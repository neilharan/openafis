#ifndef TEMPLATEISO19794_2_2005_H
#define TEMPLATEISO19794_2_2005_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Template.h"

#include <string>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T> class TemplateISO19794_2_2005: public Template<T>
{
public:
    explicit TemplateISO19794_2_2005(const T &id):
        Template<T>(id) {}
        
    bool load(const std::string &path);
    bool load(const uint8_t *data, size_t length);

private:
    // fields are big-endian...
    PACK(struct _Header {
        uint32_t totalLength;
        uint16_t rfu1;
        uint16_t width; // pixels
        uint16_t height; // pixels
        uint16_t resolutionX; // ppcm
        uint16_t resolutionY; // ppcm
        uint8_t fingerPrintCount;
        uint8_t rfu2;
    });

    PACK(struct _Minutia {
        uint16_t type_X; // 2-bits type | 14-bits x-position pixels
        uint16_t rfu_Y; // 2-bits rfu | 14-bits y-position pixels
        uint8_t angle;
        uint8_t quality;
    });

    PACK(struct _FingerPrint {
        uint8_t position;
        uint8_t rfu;
        uint8_t quality;
        uint8_t minutiaCount;
    });

    static const unsigned char MagicVersion[8];

    static constexpr uint16_t swap16(const uint16_t x) { return (x << 8) | (x >> 8 ); }
    static constexpr uint32_t swap32(const uint32_t x) { const auto _x = ((x << 8) & 0xff00ff00) | ((x >> 8) & 0xff00ff); return (_x << 16) | (_x >> 16); }
    static const size_t LargestStruct = std::max(sizeof(_Header), std::max(sizeof(_Minutia), sizeof(_FingerPrint)));

public:
    static const size_t MinimumLength = sizeof(MagicVersion) + sizeof(_Header);
    static const size_t MaximumLength = MinimumLength + MaximumFingerprints * (sizeof(_FingerPrint) + MaximumMinutiae * sizeof(_Minutia));
};

#endif // TEMPLATEISO19794_2_2005_H
