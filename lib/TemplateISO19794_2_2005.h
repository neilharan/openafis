#ifndef TEMPLATEISO19794_2_2005_H
#define TEMPLATEISO19794_2_2005_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Template.h"

#include <string>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T> class TemplateISO19794_2_2005: public Template<T>
{
public:
    TemplateISO19794_2_2005(const T &id):
        Template<T>(id) {}
        
    bool load(const std::string &path);
    bool load(const uint8_t *data, size_t length);

private:
    static const unsigned char MagicVersion[8];
    static const size_t HeaderLength = 20;
    static const size_t MinutiaLength = 6;
    static const size_t MaximumLength = sizeof(MagicVersion) + HeaderLength + 256 * MinutiaLength; // minutia count is stored in 8-bits so we have an implied maximum size
};

#endif // TEMPLATEISO19794_2_2005_H
