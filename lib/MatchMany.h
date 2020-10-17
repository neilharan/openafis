#ifndef MATCHMANY_H
#define MATCHMANY_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Match.h"

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
    using OneManyResult = std::pair<unsigned int, const TemplateType*>;

    OneManyResult oneMany(const TemplateType& probe, const Templates& candidates) const
    {
        static const auto Concurrency = std::thread::hardware_concurrency();

        if (candidates.empty()) {
            return std::make_pair(0, nullptr);
        }
        std::vector<std::future<OneManyResult>> futures;
        futures.reserve(1 + Concurrency);

        for (auto fromIt = candidates.begin();;) {
            auto endIt = fromIt + std::min(static_cast<size_t>(candidates.end() - fromIt), candidates.size() / Concurrency);

            futures.emplace_back(std::async(std::launch::async, [=, &probe]() {
                thread_local static MatchSimilarity match;

                unsigned int maxSimilarity {};
                const TemplateType* maxCandidate {};

                for (auto it = fromIt; it < endIt; ++it) {
                    unsigned int similarity {};

                    match.compute(similarity, probe.fingerprints()[0], it->fingerprints()[0]);
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

    void manyMany(std::vector<unsigned int>& scores, const Templates& templates) const
    {
        if (scores.size() != templates.size() * templates.size()) {
            return;
        }
        std::vector<std::future<void>> futures;
        futures.reserve(templates.size());
        size_t i {};
        for (const auto& t1 : templates) {
            futures.emplace_back(std::async(std::launch::async, [=, &scores, &templates]() {
                thread_local static MatchSimilarity match;
                auto scoresPtr = &scores[i];
                for (const auto& t2 : templates) {
                    match.compute(*scoresPtr++, t1.fingerprints()[0], t2.fingerprints()[0]);
                }
            }));
            i += templates.size();
        }
        for (const auto& f : futures) {
            f.wait();
        }
    }
};
}

#endif // MATCHMANY_H
