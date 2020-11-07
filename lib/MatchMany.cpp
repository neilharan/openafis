
#include "MatchMany.h"
#include "Param.h"
#include "TemplateISO19794_2_2005.h"

#include <future>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T>
MatchMany<T>::MatchMany()
    : m_concurrency(std::min(Param::MaximumConcurrency, std::thread::hardware_concurrency()))
{
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T> typename MatchMany<T>::OneManyResult MatchMany<T>::oneMany(const T& probe, const Templates& candidates) const
{
    if (candidates.empty()) {
        return std::make_pair(0, nullptr);
    }
    if (m_concurrency == 1) {
        static MatchSimilarity match;

        uint8_t maxSimilarity {};
        const T* maxCandidate {};
        const auto& probeT = probe.fingerprints()[0];

        for (const auto& t : candidates) {
            uint8_t similarity {};
            match.compute(similarity, probeT, t.fingerprints()[0]);
            if (similarity > maxSimilarity) {
                maxSimilarity = similarity;
                maxCandidate = &t;
            }
        }
        return std::make_pair(maxSimilarity, maxCandidate);
    }
    std::vector<std::future<OneManyResult>> futures;
    futures.reserve(1 + m_concurrency);

    for (auto fromIt = candidates.begin();;) {
        auto endIt = fromIt + std::min(static_cast<size_t>(candidates.end() - fromIt), candidates.size() / m_concurrency);

        futures.emplace_back(std::async(std::launch::async, [=, &probeT = probe.fingerprints()[0]]() {
            thread_local static MatchSimilarity match;

            uint8_t maxSimilarity {};
            const T* maxCandidate {};

            for (auto it = fromIt; it < endIt; ++it) {
                uint8_t similarity {};
                match.compute(similarity, probeT, it->fingerprints()[0]);
                if (similarity > maxSimilarity) {
                    maxSimilarity = similarity;
                    maxCandidate = &(*it);
                }
            }
            return std::make_pair(maxSimilarity, maxCandidate);
        }));

        if (endIt == candidates.end()) {
            break;
        }
        fromIt = endIt;
    }
    OneManyResult bestR;
    for (auto& f : futures) {
        const auto& r = f.get();
        if (r.second && r.first > bestR.first) {
            bestR = r;
        }
    }
    return bestR;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T> void MatchMany<T>::manyMany(std::vector<uint8_t>& scores, const Templates& templates) const
{
    if (scores.size() != templates.size() * templates.size()) {
        return;
    }
    if (m_concurrency == 1) {
        static MatchSimilarity match;
        auto* scoresPtr = scores.data();

        for (const auto& t1 : templates) {
            auto& t1t = t1.fingerprints()[0];
            for (const auto& t2 : templates) {
                match.compute(*scoresPtr++, t1t, t2.fingerprints()[0]);
            }
        }
        return;
    }
    std::vector<std::future<void>> futures;
    futures.reserve(templates.size());
    size_t i {};
    for (const auto& t1 : templates) {
        futures.emplace_back(std::async(std::launch::async, [=, &t1t = t1.fingerprints()[0], &scores, &templates]() {
            thread_local static MatchSimilarity match;
            auto* scoresPtr = &scores[i];
            for (const auto& t2 : templates) {
                match.compute(*scoresPtr++, t1t, t2.fingerprints()[0]);
            }
        }));
        i += templates.size();
    }
    for (const auto& f : futures) {
        f.wait();
    }
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Explicit instantiations...
//
template class MatchMany<TemplateISO19794_2_2005<std::string, Fingerprint>>;
}
