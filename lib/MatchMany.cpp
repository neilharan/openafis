
#include "MatchMany.h"
#include "TemplateISO19794_2_2005.h" // NJH-TODO


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
std::pair<unsigned int, const T*> MatchMany<T>::compute(const T& probe, const Templates &candidates) const
{
    static MatchSimilarity match;

    unsigned int maxSimilarity{};
    const T* maxCandidate{};

    for (const auto& candidate : candidates) {
        unsigned int similarity{};

        match.compute(similarity, probe.fingerprints()[0], candidate.fingerprints()[0]);
        if (similarity > maxSimilarity) {
            maxSimilarity = similarity;
            maxCandidate = &candidate;
        }
    }
    return std::make_pair(maxSimilarity, maxCandidate);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template class MatchMany<TemplateISO19794_2_2005<std::filesystem::path, Fingerprint>>;
}
