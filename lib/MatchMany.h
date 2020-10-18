#ifndef MATCHMANY_H
#define MATCHMANY_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Match.h"
#include "Param.h"

#include <future>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Instantiate on the heap.
// This class reserves a wide array to improve duplicate checking efficiency...
//
namespace OpenAFIS
{

template <class TemplateType> class MatchMany
{
public:
    using Templates = std::vector<TemplateType>;
    using OneManyResult = std::pair<int, const TemplateType*>;

    MatchMany()
        : m_concurrency(std::min(Param::MaximumConcurrency, std::thread::hardware_concurrency())) { }

    OneManyResult oneMany(const TemplateType& probe, const Templates& candidates) const
    {
        if (candidates.empty()) {
            return std::make_pair(0, nullptr);
        }
        if (m_concurrency == 1) {
            static MatchSimilarity match;

            int maxSimilarity {};
            const TemplateType* maxCandidate {};
            const auto &probeT = probe.fingerprints()[0];

            for (const auto& t : candidates) {
                int similarity {};
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

                int maxSimilarity {};
                const TemplateType* maxCandidate {};

                for (auto it = fromIt; it < endIt; ++it) {
                    int similarity {};
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

    void manyMany(std::vector<int>& scores, const Templates& templates) const
    {
        if (scores.size() != templates.size() * templates.size()) {
            return;
        }
        if (m_concurrency == 1) {
            static MatchSimilarity match;
            auto scoresPtr = scores.data();

            for (const auto& t1 : templates) {
                auto &t1t = t1.fingerprints()[0];
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
                auto scoresPtr = &scores[i];
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

    [[nodiscard]] unsigned int concurrency() const { return m_concurrency; }

private:
    const unsigned int m_concurrency;
};
}

#endif // MATCHMANY_H
