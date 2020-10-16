
#include "Template.h"
#include "Log.h"

#include "delaunator.hpp"

#include <cassert>
#include <numeric>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class I, class F> bool Template<I, F>::load(const Dimensions& dimensions, const std::vector<Fingerprint::Minutiae>& fps)
{
    for (const auto& minutiae : fps) {
        if (minutiae.size() < MinimumMinutiae) {
            Log::error("minutiea count < MinimumMinutiae");
            return false;
        }
        if (minutiae.size() > MaximumMinutiae) {
            Log::error("minutiea count > MaximumMinutiae");
            return false;
        }
        thread_local static std::vector<double> coords(MaximumMinutiae * 2);
        coords.clear();
        for (const auto& m : minutiae) {
            coords.emplace_back(m.x());
            coords.emplace_back(m.y());
        }
        delaunator::Delaunator d(coords);
        assert(d.triangles.size() % 3 == 0);

        F* fp {};
        if constexpr (std::is_same_v<F, Fingerprint>) {
            fp = &m_data.fps.emplace_back(minutiae.size(), d.triangles.size() / 3);
        }
        if constexpr (std::is_same_v<F, FingerprintRenderable>) {
            fp = &m_data.fps.emplace_back(minutiae.size(), d.triangles.size() / 3, dimensions, minutiae);
        }
        auto& t = fp->triplets();

        // walk triangles backwards building triplet vector...
        for (auto i = d.triangles.size() - 3;; i -= 3) {
            const auto& a = d.triangles[i];
            if (a) {
                const auto& b = d.triangles[i + 1];
                const auto& c = d.triangles[i + 2];
                t.emplace_back(MinutiaPoint::Minutiae({ MinutiaPoint(dimensions, minutiae[a]), MinutiaPoint(dimensions, minutiae[b]), MinutiaPoint(dimensions, minutiae[c]) }));
            }
            if (!i) {
                break;
            }
        }
        t.shrink_to_fit();

        // Section 5 (sort required for binary search)...
        std::sort(t.begin(), t.end());
    }
    return true;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class I, class F> size_t Template<I, F>::bytes() const
{
    return sizeof(*this) + std::accumulate(m_data.fps.begin(), m_data.fps.end(), size_t {}, [](size_t sum, const auto& fp) { return sum + fp.bytes(); });
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Explicit instantiations...
//
template class Template<uint16_t, Fingerprint>;
template class Template<uint16_t, FingerprintRenderable>;
template class Template<uint32_t, Fingerprint>;
template class Template<uint32_t, FingerprintRenderable>;
template class Template<uint64_t, Fingerprint>;
template class Template<uint64_t, FingerprintRenderable>;
template class Template<std::string, Fingerprint>;
template class Template<std::string, FingerprintRenderable>;
}
