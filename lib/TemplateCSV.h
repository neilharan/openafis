#ifndef TEMPLATECSV_H
#define TEMPLATECSV_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Template.h"

#include <string>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{

template <class FingerprintType> class TemplateCSV : public Template<FingerprintType>
{
public:
    explicit TemplateCSV(const Field::TemplateIdType& id)
        : Template(id)
    {
    }

    bool load(const std::string& path);
};
}

#endif // TEMPLATECSV_H
