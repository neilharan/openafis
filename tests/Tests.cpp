
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
#if 1
    TemplateISO19794_2_2005<unsigned short> t1_1(1);
    if (!t1_1.load("/dev/project/os/openafis/data/valid/fvc2002/DB1_B/101_1.iso")) {
        return 0;
    }
    log_info("template " << t1_1.id() << ": size " << t1_1.size() << " bytes, #fingerprints " << t1_1.fingerprints().size());

    TemplateISO19794_2_2005<unsigned short> t1_2(2);
    if (!t1_2.load("/dev/project/os/openafis/data/valid/fvc2002/DB1_B/101_2.iso")) {
        return 0;
    }
    log_info("template " << t1_2.id() << ": size " << t1_2.size() << " bytes, #fingerprints " << t1_2.fingerprints().size());

    TemplateISO19794_2_2005<unsigned short> t2_1(3);
    if (!t2_1.load("/dev/project/os/openafis/data/valid/fvc2002/DB1_B/102_1.iso")) {
        return 0;
    }
    log_info("template " << t2_1.id() << ": size " << t2_1.size() << " bytes, #fingerprints " << t2_1.fingerprints().size());

    assert(!t1_1.fingerprints().empty());
    assert(!t1_2.fingerprints().empty());
    assert(!t2_1.fingerprints().empty());

//    Score<unsigned short> score;
    {
    Score score;

    const auto start = std::chrono::high_resolution_clock::now();
    unsigned int s{};
    for(auto i = 1; i; --i) {
        s = score.compute(t1_1.fingerprints()[0], t1_2.fingerprints()[0]);
    }
    const auto finish = std::chrono::high_resolution_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

    log_info("score [alike]" << s << " in " << ms.count() << "ms");
    }

    {
    Score score;

    const auto start = std::chrono::high_resolution_clock::now();
    const auto s = score.compute(t1_1.fingerprints()[0], t2_1.fingerprints()[0]);
    const auto finish = std::chrono::high_resolution_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

    log_info("score [not alike]" << s << " in " << ms.count() << "ms");
    }
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
    auto count = 0, size = 0;
    const auto start = std::chrono::high_resolution_clock::now();
    TemplateISO19794_2_2005<unsigned short> t(1);

    for(const auto &f : files) {
        if (!t.load(f.data(), f.size())) {
            log_info("failed to load");
        }
        size += t.size();
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
    log_info("bulk load " << count << " templates in " << ms.count() << "ms, consuming " << size << " bytes");
#endif

    return 0;
}
