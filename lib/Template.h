#ifndef TEMPLATE_H
#define TEMPLATE_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Config.h"
#include "Field.h"
#include "Fingerprint.h"
#include "Minutia.h"

#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Template
{
public:
    using Fingerprints = std::vector<Fingerprint>;

    explicit Template(const Field::TemplateId &id)
        : m_data(id) {}

    const Field::TemplateId &id() const { return m_data.id; }
    const Fingerprints &fingerprints() const { return m_data.fps; }
    void clear() { m_data.fps.clear(); }
    size_t size() const;

protected:
    static const size_t MaximumFingerprints = 8;
    static const size_t MinimumMinutiae = 2;
    static const size_t MaximumMinutiae = 128;

    bool load(const Fingerprint::Dimensions& dimensions, const std::vector<std::vector<Minutia>>& fps);
    
private:
    PACK(struct Data
    {
        explicit Data(const Field::TemplateId &id)
            : id(id) {}

        Fingerprints fps;
        const Field::TemplateId id;
    }) m_data;
};

#endif // TEMPLATE_H
