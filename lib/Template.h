#ifndef TEMPLATE_H
#define TEMPLATE_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Config.h"
#include "Dimensions.h"
#include "Field.h"
#include "Fingerprint.h"
#include "Minutia.h"

#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Template
{
public:
    using Fingerprints = std::vector<Fingerprint>;

    explicit Template(const Field::TemplateIdType& id)
        : m_data(id)
    {
    }

    const Field::TemplateIdType& id() const { return m_data.id; }
    const Fingerprints& fingerprints() const { return m_data.fps; }
    void clear() { m_data.fps.clear(); }
    size_t bytes() const;

protected:
    static const size_t MaximumFingerprints = 8;
    static const size_t MinimumMinutiae = 2;
    static const size_t MaximumMinutiae = 128;

    bool load(const Dimensions& dimensions, const std::vector<std::vector<Minutia>>& fps);

private:
    PACK(struct Data {
        explicit Data(const Field::TemplateIdType& id)
            : id(id)
        {
        }

        Fingerprints fps;
        const Field::TemplateIdType id;
    })
    m_data;
};

#endif // TEMPLATE_H
