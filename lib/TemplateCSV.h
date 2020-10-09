#ifndef TEMPLATECSV_H
#define TEMPLATECSV_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Config.h"
#include "Template.h"

#include <string>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TemplateCSV : public Template
{
public:
    explicit TemplateCSV(const Field::TemplateIdType& id)
        : Template(id)
    {
    }

    bool load(const std::string& path);
};

#endif // TEMPLATECSV_H
