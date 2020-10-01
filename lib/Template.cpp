
#include "Template.h"
#include "Log.h"

#include "delaunator.hpp"

#include <cassert>
#include <set>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef OPENAFIS_FINGERPRINT_RENDERABLE
#define DIMENSIONS , dimensions
#define MINUTIAE , minutiae
#else
#define DIMENSIONS
#define MINUTIAE
#endif

bool Template::load(const Dimensions& dimensions, const std::vector<std::vector<Minutia>>& fps)
{
    for (const auto& minutiae : fps) {
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
        for (const auto& m : minutiae) {
            coords.emplace_back(m.x());
            coords.emplace_back(m.y());
        }
        delaunator::Delaunator d(coords);
        assert(d.triangles.size() % 3 == 0);
        auto& fp = m_data.fps.emplace_back(d.triangles.size() / 3 DIMENSIONS MINUTIAE);
        auto& t = fp.triplets();

#ifdef _DEBUG
        std::set<std::tuple<Field::TripletIndexType, Field::TripletIndexType, Field::TripletIndexType>> dupes;
#endif
        // walk triangles backwards building triplet vector...
        for (auto i = d.triangles.size() - 3;; i -= 3) {
            const auto& a = d.triangles[i];
            if (a) {
                const auto& b = d.triangles[i + 1];
                const auto& c = d.triangles[i + 2];
#ifdef _DEBUG
                const auto k = dupes.insert(std::make_tuple(static_cast<Field::TripletIndexType>(a), static_cast<Field::TripletIndexType>(b), static_cast<Field::TripletIndexType>(c)));
                assert(k.second);
#endif
                t.emplace_back(Triplet::Minutiae({ MinutiaPoint(dimensions, minutiae[a]), MinutiaPoint(dimensions, minutiae[b]), MinutiaPoint(dimensions, minutiae[c]) }));
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
size_t Template::bytes() const
{
    size_t sz = sizeof(*this);
    for (const auto& fp : m_data.fps) {
        sz += fp.bytes();
    }
    return sz;
}
