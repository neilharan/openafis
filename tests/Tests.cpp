
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Log.h"
#include "Match.h"
#include "Render.h"
#include "StringUtil.h"
#include "TemplateCSV.h"
#include "TemplateISO19794_2_2005.h"

#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
constexpr auto LineWidth = 100;


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Loading/enrolling efficiency is relevant too...
//
static void testBulkLoad(const std::string& path)
{
    Log::test(std::string(LineWidth, '='));
    Log::test(StringUtil::center("BULK LOAD TEMPLATE TEST", LineWidth));
    Log::test(std::string(LineWidth, '='));

    Log::test("Loading into memory...");

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
            Log::error("unable to open ", p);
            return;
        }
        std::vector<uint8_t> data(TemplateISO19794_2_2005<Fingerprint>::MaximumLength);
        f.read(data.data(), data.size());
        if ((f.rdstate() & std::ifstream::eofbit) == 0) {
            Log::error("filesize > MaximumLength ", p);
            return;
        }
        data.resize(static_cast<size_t>(f.gcount()));
        files.emplace_back(data);
    }
    Log::test("Parsing...");

    auto count = 0, size = 0;
    const auto start = std::chrono::high_resolution_clock::now();
    TemplateISO19794_2_2005 t(1);

    for (const auto& f : files) {
        if (!t.load(f.data(), f.size())) {
            Log::test("failed to load");
        }
        size += t.bytes();
        t.clear();
        count++;
    }
    const auto finish = std::chrono::high_resolution_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    Log::test("Loaded ", count, " templates in ", ms.count(), "ms, consuming ", size, " bytes");
    Log::test(std::string(LineWidth, '='), Log::LF, Log::LF);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Crude matching efficiency & efficacy test...
//
static void testMatchSingle(const std::string& path)
{
    Log::test(std::string(LineWidth, '='));
    Log::test(StringUtil::center("1:1 SIMPLE 50K ITERATION MATCH TEST", LineWidth));
    Log::test(std::string(LineWidth, '='));

    Log::test("Loading...");

    TemplateISO19794_2_2005 t1_1(101);
    if (!t1_1.load(StringUtil::format(R"(%s/fvc2002/DB1_B/101_1.iso)", path.c_str()))) {
        return;
    }
    Log::test("Template ", t1_1.id(), ": size ", t1_1.bytes(), " bytes, #fingerprints ", t1_1.fingerprints().size());

    TemplateISO19794_2_2005 t1_2(102);
    if (!t1_2.load(StringUtil::format(R"(%s/fvc2002/DB1_B/101_2.iso)", path.c_str()))) {
        return;
    }
    Log::test("Template ", t1_2.id(), ": size ", t1_2.bytes(), " bytes, #fingerprints ", t1_2.fingerprints().size());

    TemplateISO19794_2_2005 t2_1(201);
    if (!t2_1.load(StringUtil::format(R"(%s/fvc2002/DB1_B/102_1.iso)", path.c_str()))) {
        return;
    }
    Log::test("Template ", t2_1.id(), ": size ", t2_1.bytes(), " bytes, #fingerprints ", t2_1.fingerprints().size());

    if (t1_1.fingerprints().empty() || t1_2.fingerprints().empty() || t2_1.fingerprints().empty()) {
        return;
    }

    const auto test = [](const auto& a, const auto& b) {
        static const int Passes = 5;
        static const int Iterations = 50000;
        static MatchSimilarity match;

        for (auto i = 0; i < Passes; ++i) {
            unsigned int s {};

            const auto start = std::chrono::high_resolution_clock::now();
            for (auto i = 0; i < Iterations; ++i) {
                match.compute(s, a.fingerprints()[0], b.fingerprints()[0]);
            }
            const auto finish = std::chrono::high_resolution_clock::now();
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

            Log::test("Pass ", i + 1, ", similarity of ", a.id(), " and ", b.id(), " == ", s, "% [", Iterations, " iterations computed in ", ms.count(), "ms]");
        }
    };

    Log::test("Matching...");
    test(t1_1, t1_2); // matching
    Log::test("");
    test(t1_1, t2_1); // not matching
    Log::test(std::string(LineWidth, '='), Log::LF, Log::LF);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void testMatchMany(const std::string& path)
{
    Log::test(std::string(LineWidth, '='));
    Log::test(StringUtil::center("N:N SIMPLE MATCH EFFICACY TEST", LineWidth));
    Log::test(std::string(LineWidth, '='));

    // We're not concerned with loading efficiency (a lot of disk I/O and irrelevant tasks like regex)...
    Log::test("Loading...");

    std::regex re("\\_+"); // split underscore
    std::vector<TemplateISO19794_2_2005<Fingerprint>> templates;
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
                Log::error("unknown FVC dataset [file] ", p);
                return uint32_t {};
            }
            const auto s = StringUtil::lower(p.string());
            Field::TemplateIdType id {};

            // Yes, there are better ways to do this...
            if (StringUtil::contains(s, "fvc2002")) {
                id |= 2002, 20;
            } else if (StringUtil::contains(s, "fvc2004")) {
                id |= 2004, 20;
            } else if (StringUtil::contains(s, "fvc2006")) {
                id |= 2006, 20;
            } else {
                Log::error("unknown FVC dataset [year] ", p);
                return uint32_t {};
            }

            if (StringUtil::contains(s, "db1_b")) {
                id |= 1, 16;
            } else if (StringUtil::contains(s, "db2_b")) {
                id |= 2, 16;
            } else if (StringUtil::contains(s, "db3_b")) {
                id |= 3, 16;
            } else if (StringUtil::contains(s, "db4_b")) {
                id |= 4, 16;
            } else {
                Log::error("unknown FVC dataset [set] ", p);
                return uint32_t {};
            }
            id |= std::stoul(parts[0]), 8; // eg. 101
            id |= std::stoul(parts[1]);
            return id;
        };

        auto& t = templates.emplace_back(id());
        if (!t.load(p.string())) {
            Log::error("failed to load ", p);
            return;
        }
        if (t.fingerprints().empty()) {
            Log::error("template is empty ", p);
            return;
        }
    }
    std::vector<unsigned int> scores(templates.size() * templates.size());

    Log::test("Matching ", scores.capacity(), " permutations...");

    static MatchSimilarity match;
    size_t i {};

    const auto start = std::chrono::high_resolution_clock::now();
    for (const auto& t1 : templates) {
        for (const auto& t2 : templates) {
            match.compute(scores[i++], t1.fingerprints()[0], t2.fingerprints()[0]);
            if ((i % 50000) == 0) {
                Log::test("Matched ", i);
            }
        }
    }
    const auto finish = std::chrono::high_resolution_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    Log::test("Completed ", " in ", ms.count(), "ms");

    Log::test("Reporting...");

    const auto now = []() {
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);
        std::stringstream ss;
        ss << std::put_time(&tm, "%Y-%m-%d-%H-%M-%S");
        return ss.str();
    };

    const auto fn = StringUtil::format(R"(%s.csv)", now().c_str());
    std::ofstream f(fn, std::ofstream::binary);
    if (!f) {
        Log::test("Unable to write ", fn);
        return;
    }

    const auto expandId = [](const Field::TemplateIdType id) { return StringUtil::format(R"(FVC%d-DB%d_B-%d_%d)", id >> 20, (id >> 16) & 0xf, (id >> 8) & 0xff, id & 0xff); };

    i = 0;
    for (const auto& t1 : templates) {
        f << "," << expandId(t1.id());
    }
    f << std::endl;
    for (const auto& t1 : templates) {
        f << expandId(t1.id());

        for (const auto& t2 : templates) {
            std::ignore = t2;
            f << "," << scores[i++];
        }
        f << std::endl;
    }
    Log::test("Written ", fn);
    Log::test(std::string(LineWidth, '='), Log::LF, Log::LF);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void testRender(const std::string& path)
{
    Log::test(std::string(LineWidth, '='));
    Log::test(StringUtil::center("MINUTIAE PAIRING RENDER TEST", LineWidth));
    Log::test(std::string(LineWidth, '='));

    Log::test("Loading...");

    TemplateISO19794_2_2005<FingerprintRenderable> t1_1(101);
    if (!t1_1.load(StringUtil::format(R"(%s/fvc2002/DB1_B/101_1.iso)", path.c_str()))) {
        return;
    }
    Log::test("Template ", t1_1.id(), ": size ", t1_1.bytes(), " bytes, #fingerprints ", t1_1.fingerprints().size());

    TemplateISO19794_2_2005<FingerprintRenderable> t1_2(107);
    if (!t1_2.load(StringUtil::format(R"(%s/fvc2002/DB1_B/101_7.iso)", path.c_str()))) {
        return;
    }
    if (t1_1.fingerprints().empty() || t1_2.fingerprints().empty()) {
        return;
    }

    const auto test = [](const auto& a, const auto& b) {
        const auto write = [](const auto& t, const std::string& svg) {
            const auto fn = StringUtil::format(R"(%s.svg)", std::to_string(t.id()).c_str());
            std::ofstream f(fn, std::ofstream::binary);
            if (!f) {
                Log::test("Unable to write ", fn);
                return;
            }
            f.write(svg.data(), svg.size());
            Log::test("Written ", fn);
        };
        std::string svg1, svg2;
        if (!Render::all(svg1, svg2, a.fingerprints()[0], b.fingerprints()[0])) {
            return;
        }
        write(a, svg1);
        write(b, svg2);
    };

    Log::test("Rendering...");
    test(t1_1, t1_2);
    Log::test(std::string(LineWidth, '='), Log::LF, Log::LF);
}
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int main(int, const char**)
{
    const std::string path = "/dev/project/os/openafis/data/psy"; // NJH-TODO from command line

    //OpenAFIS::testBulkLoad(path);
    //OpenAFIS::testMatchSingle(path);
    OpenAFIS::testMatchMany(path);
    //OpenAFIS::testRender(path);
    return 0;
}
