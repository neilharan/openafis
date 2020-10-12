#ifndef TEMPLATE_H
#define TEMPLATE_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Field.h"
#include "Fingerprint.h"

#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{

template <class FingerprintType> class Template
{
public:
    using Fingerprints = std::vector<FingerprintType>;

    explicit Template(const Field::TemplateIdType& id)
        : m_data(id)
    {
    }

    [[nodiscard]] const Field::TemplateIdType& id() const { return m_data.id; }
    [[nodiscard]] const Fingerprints& fingerprints() const { return m_data.fps; }
    [[nodiscard]] void clear() { m_data.fps.clear(); }
    [[nodiscard]] size_t bytes() const;

protected:
    static constexpr size_t MaximumFingerprints = 8;
    static constexpr size_t MinimumMinutiae = 2;
    static constexpr size_t MaximumMinutiae = 128;

    bool load(const Dimensions& dimensions, const std::vector<Fingerprint::Minutiae>& fps);

private:
    struct Data {
        explicit Data(const Field::TemplateIdType& id)
            : id(id)
        {
        }

        Fingerprints fps;
        const Field::TemplateIdType id;
    } m_data;
};
}

#endif // TEMPLATE_H
