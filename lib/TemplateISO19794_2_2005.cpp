
#include "Log.h"
#include "TemplateISO19794_2_2005.h"

#include <cassert>
#include <fstream>
#include <iostream>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
const unsigned char TemplateISO19794_2_2005<T>::MagicVersion[] = { 'F', 'M', 'R', 0, ' ', '2', '0', 0 };


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
bool TemplateISO19794_2_2005<T>::load(const std::string &path)
{
    std::basic_ifstream<uint8_t> f(path, std::ifstream::in | std::ifstream::binary);
    if (!f) {
        log_error("unable to open " << path);
        return false;
    }
    static uint8_t data[MaximumLength];   
    f.read(data, sizeof(data));
    if ((f.rdstate() & std::ifstream::eofbit) == 0) {
        log_error("filesize > MaximumLength " << path);
        return false;
    }
    return load(data, static_cast<size_t>(f.gcount()));
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// https://www.iso.org/standard/38746.html
// https://templates.machinezoo.com/iso-19794-2-2005
// https://github.com/atefm/ISO_19794-2-2005-converter/blob/master/main.cpp
// https://www.nist.gov/services-resources/software/biomdi-software-tools-supporting-standard-biometric-data-interchange
//
template <class T>
bool TemplateISO19794_2_2005<T>::load(const uint8_t *data, size_t length)
{
    if (length < sizeof(MagicVersion) + HeaderLength + MinutiaLength) {
        log_error("length < minimum; " << length);
        return false;
    }
    if (length > MaximumLength) {
        log_error("length > MaximumLength; " << length);
        return false;
    }
    if (memcmp(data, MagicVersion, sizeof(MagicVersion))) {
        log_error("invalid magic; unsupported file-format");
        return false;
    }

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
        _Minutia minutiae; // first entry
    });

    // check alignment for platform...
    assert(sizeof(_Header) % sizeof(unsigned int) == 0);

    std::vector<std::vector<Minutia>> fps;
    std::vector<Minutia> minutiae;

    data += sizeof(MagicVersion);
    const _Header *h = reinterpret_cast<const _Header*>(data);
    data += sizeof(_Header);

    for(auto f = 0; f < h->fingerPrintCount; ++f) {
        const _FingerPrint *fp = reinterpret_cast<const _FingerPrint*>(data);
        data += sizeof(_FingerPrint) + sizeof(_Minutia) * (fp->minutiaCount - 1);

        auto mp = &fp->minutiae;
        for(auto m = 0; m < fp->minutiaCount; ++m) {
            minutiae.push_back(Minutia(SWAP16(mp->type_X) & 0x3f, SWAP16(mp->rfu_Y) & 0x3f, mp->angle));
            mp++;
        }
        fps.push_back(minutiae);
        minutiae.clear();

        // skip extension data at the end - no deref here as address may no longer be aligned...
        uint16_t extensionData;
        memcpy(&extensionData, data, sizeof(extensionData));
        data += sizeof(uint16_t);
        data += extensionData;
    }
    return Template::load(std::pair<unsigned short, unsigned short>(SWAP16(h->resolutionX), SWAP16(h->resolutionY)), fps);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template class TemplateISO19794_2_2005<unsigned short>;
template class TemplateISO19794_2_2005<unsigned int>;
template class TemplateISO19794_2_2005<std::string>;
