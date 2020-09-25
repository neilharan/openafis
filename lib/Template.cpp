
#include "Log.h"
#include "Template.h"

#include "delaunator.hpp"

#include <cassert>
#include <string>
#include <set>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
# define DIMENSIONS , dimensions
# define MINUTIAE , minutiae
#endif

bool Template::load(const Fingerprint::Dimensions& dimensions, const std::vector<std::vector<Minutia>>& fps)
{
    for(const auto &minutiae : fps) {
        if (minutiae.size() < MinimumMinutiae) {
            log_error("minutiea count < MinimumMinutiae");
            return false;
        }
        if (minutiae.size() > MaximumMinutiae) {
            log_error("minutiea count > MaximumMinutiae");
            return false;
        }
        thread_local static std::vector<double> coords(MaximumMinutiae * 2);
        coords.clear();
        for(const auto &m : minutiae) {
            coords.emplace_back(m.x());
            coords.emplace_back(m.y());
        }
        delaunator::Delaunator d(coords);
        assert(d.triangles.size() % 3 == 0);
        auto& fp = m_data.fps.emplace_back(d.triangles.size() / 3 DIMENSIONS MINUTIAE);
        auto& t = fp.triplets();

#ifdef _DEBUG
        std::set<std::tuple<Field::Triplet, Field::Triplet, Field::Triplet>> dupes;
#endif
        // walk triangles backwards building triplet vector...
        for(auto i = d.triangles.size() - 3;; i -= 3) {
            const auto& a = d.triangles[i];
            if (a) {
                const auto& b = d.triangles[i + 1];
                const auto& c = d.triangles[i + 1];
#ifdef _DEBUG
                const auto k = dupes.insert(std::make_tuple(static_cast<Field::Triplet>(a), static_cast<Field::Triplet>(b), static_cast<Field::Triplet>(c)));
                assert(k.second);
#endif
                t.emplace_back(static_cast<Field::Triplet>(a), static_cast<Field::Triplet>(b), static_cast<Field::Triplet>(c));
            }
            if (!i) {
                break;
            }
        }
        t.shrink_to_fit();
    }
    return true;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t Template::size() const
{
    size_t sz{};
    for(const auto &fp : m_data.fps) {
        sz += fp.triplets().capacity() * sizeof(Triplet);
#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
        sz += sizeof(fp.dimensions());
        sz += fp.minutiae().capacity() * sizeof(Minutia);
#endif
    }
    return sz + sizeof(*this);
}
