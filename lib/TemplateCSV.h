#ifndef TEMPLATECSV_H
#define TEMPLATECSV_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Template.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{

template <class IdType, class FingerprintType> class TemplateCSV : public Template<IdType, FingerprintType>
{
public:
    explicit TemplateCSV(const IdType& id)
        : Template<IdType, FingerprintType>(id)
    {
    }

    bool load(const std::string& path);
};
}

#endif // TEMPLATECSV_H
