
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Log.h"
#include "Match.h"
#include "Render.h"
#include "StringUtil.h"
#include "TemplateISO19794_2_2005.h"

#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
constexpr auto LineWidth = 100;


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Loading/enrolling efficiency is relevant too...
//
static void testBulkLoad(const std::string &path)
{
    logTest(std::string(LineWidth, '='));
    logTest(StringUtil::center("BULK LOAD TEMPLATE TEST", LineWidth));
    logTest(std::string(LineWidth, '='));

    logTest("Loading into memory...");

    std::vector<std::vector<uint8_t>> files;
    files.reserve(1000);
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path.c_str())) {
        if (!entry.is_regular_file()) {
            continue;
        }
        const auto& p = entry.path();
        if (StringUtil::lower(p.extension().string()) != ".iso") {
            continue;
        }
        std::basic_ifstream<uint8_t> f(p, std::ifstream::in | std::ifstream::binary);
        if (!f) {
            logError("unable to open " << p);
            return;
        }
        std::vector<uint8_t> data(TemplateISO19794_2_2005::MaximumLength);
        f.read(data.data(), data.size());
        if ((f.rdstate() & std::ifstream::eofbit) == 0) {
            logError("filesize > MaximumLength " << p);
            return;
        }
        data.resize(static_cast<size_t>(f.gcount()));
        files.emplace_back(data);
    }
    logTest("Parsing...");

    auto count = 0, size = 0;
    const auto start = std::chrono::high_resolution_clock::now();
    TemplateISO19794_2_2005 t(1);

    for (const auto& f : files) {
        if (!t.load(f.data(), f.size())) {
            logTest("failed to load");
        }
        size += t.bytes();
        t.clear();
        count++;
    }
    const auto finish = std::chrono::high_resolution_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    logTest("Loaded " << count << " templates in " << ms.count() << "ms, consuming " << size << " bytes");
    logTest(std::string(LineWidth, '=') << std::endl << std::endl);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Crude matching efficiency & efficacy test...
//
static void testMatchSingle(const std::string &path)
{
    logTest(std::string(LineWidth, '='));
    logTest(StringUtil::center("1:1 SIMPLE 50K ITERATION MATCH TEST", LineWidth));
    logTest(std::string(LineWidth, '='));

    logTest("Loading...");

    TemplateISO19794_2_2005 t1_1(101);
    if (!t1_1.load(StringUtil::format(R"(%s/fvc2002/DB1_B/101_1.iso)", path.c_str()))) {
        return;
    }
    logTest("Template " << t1_1.id() << ": size " << t1_1.bytes() << " bytes, #fingerprints " << t1_1.fingerprints().size());

    TemplateISO19794_2_2005 t1_2(102);
    if (!t1_2.load(StringUtil::format(R"(%s/fvc2002/DB1_B/101_2.iso)", path.c_str()))) {
        return;
    }
    logTest("Template " << t1_2.id() << ": size " << t1_2.bytes() << " bytes, #fingerprints " << t1_2.fingerprints().size());

    TemplateISO19794_2_2005 t2_1(201);
    if (!t2_1.load(StringUtil::format(R"(%s/fvc2002/DB1_B/102_1.iso)", path.c_str()))) {
        return;
    }
    logTest("Template " << t2_1.id() << ": size " << t2_1.bytes() << " bytes, #fingerprints " << t2_1.fingerprints().size());

    if (t1_1.fingerprints().empty() || t1_2.fingerprints().empty() || t2_1.fingerprints().empty()) {
        return;
    }

    const auto test = [](const TemplateISO19794_2_2005& a, const TemplateISO19794_2_2005& b) {
        static const int Iterations = 50000;
        Match match;

        for (auto i = 0; i < 5; ++i) {
            unsigned int s {};

            const auto start = std::chrono::high_resolution_clock::now();
            for (auto i = 0; i < Iterations; ++i) {
                s = match.compute(a.fingerprints()[0], b.fingerprints()[0]);
            }
            const auto finish = std::chrono::high_resolution_clock::now();
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

            logTest("Pass " << i + 1 << ", similarity of " << a.id() << " and " << b.id() << " == " << s << "% [" << Iterations << " iterations computed in " << ms.count() << "ms]");
        }
    };

    logTest("Matching...");
    test(t1_1, t1_2); // matching
    logTest("");
    test(t1_1, t2_1); // not matching
    logTest(std::string(LineWidth, '=') << std::endl << std::endl);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void testMatchMany(const std::string &path)
{
    logTest(std::string(LineWidth, '='));
    logTest(StringUtil::center("1:N SIMPLE MATCH EFFICACY TEST", LineWidth));
    logTest(std::string(LineWidth, '='));

    // We're not concerned with loading efficiency (a lot of disk I/O and irrelevant tasks like regex)...
    logTest("Loading...");

    std::regex re("\\_+"); // split underscore
    std::vector<TemplateISO19794_2_2005> templates;
    templates.reserve(1000);
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path.c_str())) {
        if (!entry.is_regular_file()) {
            continue;
        }
        const auto& p = entry.path();
        if (StringUtil::lower(p.extension().string()) != ".iso") {
            continue;
        }
        const auto id = [&]() {
            // Split the '101_1'...
            const auto stem = p.stem().string();
            const std::vector<std::string> parts(std::sregex_token_iterator(stem.begin(), stem.end(), re, -1), {});
            if (parts.size() != 2) {
                logError("unknown FVC dataset [file] " << p);
                return uint32_t{};
            }
            const auto s = StringUtil::lower(p.string());
            uint32_t id{};

            // Yes, there are better ways to do this...
            if (StringUtil::contains(s, "fvc2002")) {
                id |= 2002 << 20;
            } else if (StringUtil::contains(s, "fvc2004")) {
                id |= 2004 << 20;
            } else if (StringUtil::contains(s, "fvc2006")) {
                id |= 2006 << 20;
            } else {
                logError("unknown FVC dataset [year] " << p);
                return uint32_t{};
            }

            if (StringUtil::contains(s, "db1_b")) {
                id |= 1 << 16;
            } else if (StringUtil::contains(s, "db2_b")) {
                id |= 2 << 16;
            } else if (StringUtil::contains(s, "db3_b")) {
                id |= 3 << 16;
            } else if (StringUtil::contains(s, "db4_b")) {
                id |= 4 << 16;
            } else {
                logError("unknown FVC dataset [set] " << p);
                return uint32_t{};
            }
            id |= std::stoul(parts[0]) << 8; // eg. 101
            id |= std::stoul(parts[1]);
            return id;
        };
        
        auto &t = templates.emplace_back(id());
        if (!t.load(p.string())) {
            logError("failed to load " << p);
            return;
        }
        if (t.fingerprints().empty()) {
            logError("template is empty " << p);
            return;
        }
    }
    std::vector<unsigned int> scores(templates.size() * (templates.size() - 1));

    logTest("Matching " << scores.capacity() << " permutations...");

    Match match;
    size_t i{};

    const auto start = std::chrono::high_resolution_clock::now();
    for (const auto &t1 : templates) {
        for (const auto &t2 : templates) {
            if (t1.id() == t2.id()) {
                continue;
            }
            scores[i++] = match.compute(t1.fingerprints()[0], t2.fingerprints()[0]);
            if ((i % 50000) == 0) {
                logTest("Matched " << i);
            }
        }
    }
    const auto finish = std::chrono::high_resolution_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

    logTest("Reporting...");

    i = 0;
    for (const auto &t1 : templates) {
        for (const auto &t2 : templates) {
            if (t1.id() == t2.id()) {
                continue;
            }
            logTest(t1.id() << " " << t2.id() << " similarity of " << scores[i++]);
        }
    }
    logTest("Completed " << " in " << ms.count() << "ms");
    logTest(std::string(LineWidth, '=') << std::endl << std::endl);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void testRender(const std::string &path)
{
    logTest(std::string(LineWidth, '='));
    logTest(StringUtil::center("MINUTIAE PAIRING RENDER TEST", LineWidth));
    logTest(std::string(LineWidth, '='));

    logTest("Loading...");

    TemplateISO19794_2_2005 t1_1(101);
    if (!t1_1.load(StringUtil::format(R"(%s/fvc2002/DB1_B/101_1.iso)", path.c_str()))) {
        return;
    }
    logTest("Template " << t1_1.id() << ": size " << t1_1.bytes() << " bytes, #fingerprints " << t1_1.fingerprints().size());

    TemplateISO19794_2_2005 t1_2(102);
    if (!t1_2.load(StringUtil::format(R"(%s/fvc2002/DB1_B/101_2.iso)", path.c_str()))) {
        return;
    }
    logTest("Template " << t1_2.id() << ": size " << t1_2.bytes() << " bytes, #fingerprints " << t1_2.fingerprints().size());

    if (t1_1.fingerprints().empty() || t1_2.fingerprints().empty()) {
        return;
    }

    const auto test = [](const TemplateISO19794_2_2005& a) {
        std::string svg;
        if (!Render::minutiae(svg, a.fingerprints()[0])) {
            return;
        }
        const auto fn = StringUtil::format(R"(%s.svg)", std::to_string(a.id()).c_str());
        std::ofstream f(fn, std::ofstream::binary);
        assert(f);
        f.write(svg.data(), svg.size());
        logTest("Written " << fn);
    };

    logTest("Rendering...");
    test(t1_1);
    test(t1_2);
    logTest(std::string(LineWidth, '=') << std::endl << std::endl);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int main(int, const char**)
{
    const std::string path = "/dev/project/os/openafis/data/valid"; // NJH-TODO from command line

//    testBulkLoad(path);
//    testMatchSingle(path);
    testMatchMany(path);
//    testRender(path);

    return 0;
}
