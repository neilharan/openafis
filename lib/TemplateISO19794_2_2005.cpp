
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
    std::vector<uint8_t> data(MaximumLength);
    f.read(data.data(), data.size());
    if ((f.rdstate() & std::ifstream::eofbit) == 0) {
        log_error("filesize > MaximumLength " << path);
        return false;
    }
    return load(data.data(), static_cast<size_t>(f.gcount()));
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// https://www.iso.org/standard/38746.html
// https://www.nist.gov/services-resources/software/biomdi-software-tools-supporting-standard-biometric-data-interchange
// https://templates.machinezoo.com/iso-19794-2-2005
//
template <class T>
bool TemplateISO19794_2_2005<T>::load(const uint8_t *data, const size_t length)
{
    if (length > MaximumLength) {
        log_error("length > MaximumLength; " << length);
        return false;
    }
    // return a pointer to struct at data provided all reads from that struct would not exceed bounds
    // also increment the source pointer to the next element...
    const auto safeRead = [data, length](auto **readFrom) {
        using T = decltype(*readFrom);
        constexpr auto sz = sizeof(**readFrom);
        if (reinterpret_cast<const uint8_t *>(*readFrom) - data + sz > length) {
            log_error("data invalid; attempted invalid read @" << readFrom);
            void *np{nullptr};
            return reinterpret_cast<T>(np);
        }
        const auto p = *readFrom;
        *reinterpret_cast<uint8_t **>(readFrom) += sz;

        // check alignment - platforms that support unaligned access (like x86) _could_ just return p
        // realigning here does improve performance though & is a requirement for some platforms (like arm) where unaligned access is UB...
        if (reinterpret_cast<uint32_t>(p) % sizeof(void *) == 0) {
            return p;
        }
        static std::vector<uint8_t> buff(LargestStruct);
        if (sz > buff.size()) {
            log_error("struct exceeded buffer while aligning @" << readFrom);
            *readFrom = nullptr;
            return *readFrom;
        }
        memcpy(buff.data(), p, sz);
        void *bp{buff.data()};
        return reinterpret_cast<T>(bp);
    };

    auto p = const_cast<uint8_t *>(data);
    if (memcmp(p, MagicVersion, sizeof(MagicVersion))) {
        log_error("invalid magic; unsupported format");
        return false;
    }
    p += sizeof(MagicVersion);

    const auto *h = safeRead(reinterpret_cast<_Header**>(&p));
    if (!h) {
        return false;
    }
    assert(swap32(h->totalLength) == length);

    std::vector<std::vector<Minutia>> fps;
    std::vector<Minutia> minutiae;

    for(auto f = 0; f < h->fingerPrintCount; ++f) {
        const auto *fp = safeRead(reinterpret_cast<_FingerPrint**>(&p));
        if (!fp) {
            return false;
        }
        for(auto m = 0; m < fp->minutiaCount; ++m) {
            const auto *mp = safeRead(reinterpret_cast<_Minutia**>(&p));
            if (!mp) {
                return false;
            }
            minutiae.push_back(Minutia(swap16(mp->type_X) & 0x3f, swap16(mp->rfu_Y) & 0x3f, mp->angle));
        }
        fps.push_back(minutiae);
        minutiae.clear();

        // skip extension data at the end - no deref here as address may no longer be aligned...
        const auto ex = safeRead(reinterpret_cast<uint16_t**>(&p));
        if (!ex) {
            return false;
        }
        uint16_t extensionData;
        memcpy(&extensionData, ex, sizeof(extensionData));
        p += swap16(extensionData);
    }
    return Template::load(std::pair<unsigned short, unsigned short>(swap16(h->resolutionX), swap16(h->resolutionY)), fps);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template class TemplateISO19794_2_2005<unsigned short>;
template class TemplateISO19794_2_2005<unsigned int>;
template class TemplateISO19794_2_2005<std::string>;
