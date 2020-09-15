
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Log.h"
#include "Score.h"
#include "TemplateISO19794_2_2005.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <filesystem>

#include <fstream>
#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int main(int, const char**)
{
#if 0
    TemplateISO19794_2_2005<unsigned short> t1(1);
    assert(t1.load("/dev/project/os/openafis/data/fvc2004/DB1_B/101_1_corrupt.iso"));
    log_info("template " << t1.id() << ": size " << t1.size() << " bytes, #fingerprints " << t1.lmts().size());

    /*TemplateISO19794_2_2005<unsigned short> t2(2);
    assert(t2.load("data/njh1.iso"));
    log_info("template " << t2.id() << ": size " << t2.size() << " bytes, #fingerprints " << t2.lmts().size());

    Score<unsigned short> score;

    const auto start = std::chrono::high_resolution_clock::now();
    const auto s = score.compute(t1, t2);
    const auto finish = std::chrono::high_resolution_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

    log_info("score " << s << " in " << ms.count() << "ms");*/
#else

#if 1
    std::vector<std::vector<uint8_t>> files;
    for(const auto& entry: std::filesystem::recursive_directory_iterator("/dev/project/os/openafis/data/valid")) {
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
        std::basic_ifstream<uint8_t> f(path, std::ifstream::in | std::ifstream::binary);
        if (!f) {
            log_error("unable to open " << path);
            return false;
        }
        std::vector<uint8_t> data(TemplateISO19794_2_2005<unsigned short>::MaximumLength);
        f.read(data.data(), data.size());
        if ((f.rdstate() & std::ifstream::eofbit) == 0) {
            log_error("filesize > MaximumLength " << path);
            return false;
        }
        data.resize(static_cast<size_t>(f.gcount()));
        files.push_back(data);
    }
    auto count = 0;
    const auto start = std::chrono::high_resolution_clock::now();
    TemplateISO19794_2_2005<unsigned short> t(1);

    for(const auto &f : files) {
        if (!t.load(f.data(), f.size())) {
            log_info("failed to load");
        }
        t.clear();
        count++;
    }
#else
    auto count = 0;
    const auto start = std::chrono::high_resolution_clock::now();

    for(const auto& entry: std::filesystem::recursive_directory_iterator("/dev/project/os/openafis/data/valid")) {
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
        if (!t.load(path.string())) {
            log_info("failed to load " << path);
        }
        count++;
    }
#endif
    const auto finish = std::chrono::high_resolution_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    log_info("bulk load " << count << " templates in " << ms.count() << "ms");
#endif

    return 0;
}
