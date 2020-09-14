
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Log.h"
#include "Score.h"
#include "TemplateISO19794_2_2005.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <filesystem>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int main(int, const char**)
{
#if 0
    TemplateISO19794_2_2005<unsigned short> t1(1);
    assert(t1.load("data/njh0.iso"));
    log_info("template " << t1.id() << ": size " << t1.size() << " bytes, #fingerprints " << t1.lmts().size());

    TemplateISO19794_2_2005<unsigned short> t2(2);
    assert(t2.load("data/njh1.iso"));
    log_info("template " << t2.id() << ": size " << t2.size() << " bytes, #fingerprints " << t2.lmts().size());

    Score<unsigned short> score;

    const auto start = std::chrono::high_resolution_clock::now();
    const auto s = score.compute(t1, t2);
    const auto finish = std::chrono::high_resolution_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

    log_info("score " << s << " in " << ms.count() << "ms");
#else
    const auto start = std::chrono::high_resolution_clock::now();

    for(const auto& entry: std::filesystem::recursive_directory_iterator("/dev/project/os/openafis/data")) {
        const auto strcasecmp = [](const std::string& a, const std::string& b) {
            return std::equal(a.begin(), a.end(), b.begin(), [](const char a, const char b) { return tolower(a) == tolower(b); });
        };

        if (!entry.is_regular_file()) {
            continue;
        }
        const auto &path = entry.path();
        if (!strcasecmp(path.extension().string(), ".iso")) {
            continue;
        }
        TemplateISO19794_2_2005<unsigned short> t(1);
        assert(t.load(path.string()));
    }
    const auto finish = std::chrono::high_resolution_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    log_info("bulk load in " << ms.count() << "ms");
#endif

    return 0;
}
