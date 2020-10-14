#ifndef TEMPLATE_H
#define TEMPLATE_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Fingerprint.h"

#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{

// Any identifier you like - maybe a std::string for research, or uint16_t when memory is a constraint...
template <class IdType, class FingerprintType> class Template
{
public:
    using Fingerprints = std::vector<FingerprintType>;

    explicit Template(const IdType& id)
        : m_data(id)
    {
    }

    [[nodiscard]] const IdType& id() const { return m_data.id; }
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
        explicit Data(const IdType& id)
            : id(id)
        {
        }

        Fingerprints fps;
        const IdType id;
    } m_data;
};
}

#endif // TEMPLATE_H
