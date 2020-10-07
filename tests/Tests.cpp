
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Log.h"
#include "Match.h"
#include "Render.h"
#include "StringUtil.h"
#include "TemplateISO19794_2_2005.h"

#include <cassert>
#include <chrono>
#include <filesystem>
#include <iostream>

#include <fstream>
#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
constexpr auto LineWidth = 100;


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Loading/enrolling efficiency is relevant too...
//
void testBulkload()
{
    logTest(std::string(LineWidth, '='));
    logTest(StringUtil::center("BULK LOAD TEMPLATE TEST", LineWidth));
    logTest(std::string(LineWidth, '='));

    std::vector<std::vector<uint8_t>> files;
    for (const auto& entry : std::filesystem::recursive_directory_iterator("/dev/project/os/openafis/data/valid")) {
        const auto strcasecmp
            = [](const std::string& a, const std::string& b) { return std::equal(a.begin(), a.end(), b.begin(), [](const char a, const char b) { return tolower(a) == tolower(b); }); };

        if (!entry.is_regular_file()) {
            continue;
        }
        const auto& path = entry.path();
        if (!strcasecmp(path.extension().string(), ".iso")) {
            continue;
        }
        std::basic_ifstream<uint8_t> f(path, std::ifstream::in | std::ifstream::binary);
        if (!f) {
            logError("unable to open " << path);
            return;
        }
        std::vector<uint8_t> data(TemplateISO19794_2_2005::MaximumLength);
        f.read(data.data(), data.size());
        if ((f.rdstate() & std::ifstream::eofbit) == 0) {
            logError("filesize > MaximumLength " << path);
            return;
        }
        data.resize(static_cast<size_t>(f.gcount()));
        files.push_back(data);
    }
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
void testMatchSingle()
{
    logTest(std::string(LineWidth, '='));
    logTest(StringUtil::center("1:1 SIMPLE 50K ITERATION MATCH TEST", LineWidth));
    logTest(std::string(LineWidth, '='));

    TemplateISO19794_2_2005 t1_1(101);
    if (!t1_1.load("/dev/project/os/openafis/data/valid/fvc2002/DB1_B/101_1.iso")) {
        return;
    }
    logTest("Template " << t1_1.id() << ": size " << t1_1.bytes() << " bytes, #fingerprints " << t1_1.fingerprints().size());

    TemplateISO19794_2_2005 t1_2(102);
    if (!t1_2.load("/dev/project/os/openafis/data/valid/fvc2002/DB1_B/101_2.iso")) {
        return;
    }
    logTest("Template " << t1_2.id() << ": size " << t1_2.bytes() << " bytes, #fingerprints " << t1_2.fingerprints().size());

    TemplateISO19794_2_2005 t2_1(201);
    if (!t2_1.load("/dev/project/os/openafis/data/valid/fvc2002/DB1_B/102_1.iso")) {
        return;
    }
    logTest("Template " << t2_1.id() << ": size " << t2_1.bytes() << " bytes, #fingerprints " << t2_1.fingerprints().size());

    if (t1_1.fingerprints().empty() || t1_2.fingerprints().empty() || t2_1.fingerprints().empty()) {
        return;
    }

    const auto test = [](TemplateISO19794_2_2005& a, TemplateISO19794_2_2005& b) {
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

    logTest("");
    test(t1_1, t1_2); // matching
    logTest("");
    test(t1_1, t2_1); // not matching
    logTest(std::string(LineWidth, '=') << std::endl << std::endl);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void testRender()
{
    logTest(std::string(LineWidth, '='));
    logTest(StringUtil::center("MINUTIAE PAIRING RENDER TEST", LineWidth));
    logTest(std::string(LineWidth, '='));

    TemplateISO19794_2_2005 t1_1(101);
    if (!t1_1.load("/dev/project/os/openafis/data/valid/fvc2002/DB1_B/101_1.iso")) {
        return;
    }
    logTest("Template " << t1_1.id() << ": size " << t1_1.bytes() << " bytes, #fingerprints " << t1_1.fingerprints().size());

    TemplateISO19794_2_2005 t1_2(102);
    if (!t1_2.load("/dev/project/os/openafis/data/valid/fvc2002/DB1_B/101_2.iso")) {
        return;
    }
    logTest("Template " << t1_2.id() << ": size " << t1_2.bytes() << " bytes, #fingerprints " << t1_2.fingerprints().size());

    if (t1_1.fingerprints().empty() || t1_2.fingerprints().empty()) {
        return;
    }

    const auto test = [](TemplateISO19794_2_2005& a) {
        std::string svg;
        if (!Render::minutiae(svg, a.fingerprints()[0])) {
            return;
        }
        const auto fn = StringUtil::format(R"(%s.svg)", std::to_string(a.id()));
        std::ofstream f(fn, std::ofstream::binary);
        assert(f);
        f.write(svg.data(), svg.size());
        logTest("Written " << fn);
    };

    logTest("");
    test(t1_1);
    test(t1_2);
    logTest(std::string(LineWidth, '=') << std::endl << std::endl);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int main(int, const char**)
{
    testBulkload();
    testMatchSingle();
    testRender();

    return 0;
}
