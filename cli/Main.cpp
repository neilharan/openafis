
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Log.h"
#include "Match.h"
#include "MatchMany.h"
#include "Render.h"
#include "StringUtil.h"
#include "TemplateCSV.h"
#include "TemplateISO19794_2_2005.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
constexpr auto LineWidth = 100;


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class T> static bool helperLoadPath(T& templates, const std::string& path, const unsigned int loadFactor)
{
    templates.reserve(loadFactor * 1000);

    for (const auto& entry : std::filesystem::recursive_directory_iterator(path.c_str())) {
        if (!entry.is_regular_file()) {
            continue;
        }
        const auto& p = entry.path();
        if (StringUtil::lower(p.extension().string()) != ".iso") {
            continue;
        }
        auto& t = templates.emplace_back(p.relative_path().make_preferred().string());
        if (!t.load(p.string())) {
            Log::error("failed to load ", p.string());
            return false;
        }
        if (t.fingerprints().empty()) {
            Log::error("template is empty ", p.string());
            return false;
        }
    }
    // Duplicate templates if requested...
    if (loadFactor > 1) {
        const T copy = templates;
        for (auto i = 0u; i < loadFactor; ++i) {
            for (const auto &t : copy) {
                templates.emplace_back(t);
            }
        }
    }
    return true;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Loading/enrolling efficiency is relevant too...
//
static void bulkLoad(const std::string& path)
{
    Log::test(std::string(LineWidth, '='));
    Log::test("Bulk loading templates", Log::LF);
    Log::test("Path: ", path);
    Log::test(std::string(LineWidth, '='), Log::LF);

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
        std::vector<uint8_t> data(TemplateISO19794_2_2005<uint32_t, Fingerprint>::MaximumLength);
        f.read(data.data(), data.size());
        if ((f.rdstate() & std::ifstream::eofbit) == 0) {
            Log::error("filesize > MaximumLength ", p);
            return;
        }
        data.resize(static_cast<size_t>(f.gcount()));
        files.emplace_back(data);
    }
    Log::test(Log::LF, "Parsing...");

    auto count = 0;
    size_t size {};
    const auto start = std::chrono::steady_clock::now();
    TemplateISO19794_2_2005<uint32_t, Fingerprint> t(1);

    for (const auto& f : files) {
        if (!t.load(f.data(), f.size())) {
            Log::test("failed to load");
        }
        size += t.bytes();
        t.clear();
        count++;
    }
    const auto finish = std::chrono::steady_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    Log::test("Loaded ", count, " templates in ", ms.count(), "ms, consuming ", size, " bytes");
    Log::test(std::string(LineWidth, '='), Log::LF);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Crude matching efficiency & efficacy test...
//
static void one(const std::string& path, const std::string& f1, const std::string& f2, const std::string& f3)
{
    Log::test(std::string(LineWidth, '='));
    Log::test("50K iteration 1:1 match", Log::LF);
    Log::test("Path: ", path);
    Log::test("Template 1: ", f1);
    Log::test("Template 2: ", f2);
    Log::test("Template 3: ", f3);
    Log::test(std::string(LineWidth, '='), Log::LF);

    Log::test("Loading...");

    TemplateISO19794_2_2005<uint32_t, Fingerprint> t1(1);
    if (!t1.load(StringUtil::format(R"(%s/%s)", path.c_str(), f1.c_str()))) {
        return;
    }
    Log::test("Template ", t1.id(), ": size ", t1.bytes(), " bytes, #fingerprints ", t1.fingerprints().size());

    TemplateISO19794_2_2005<uint32_t, Fingerprint> t2(2);
    if (!t2.load(StringUtil::format(R"(%s/%s)", path.c_str(), f2.c_str()))) {
        return;
    }
    Log::test("Template ", t2.id(), ": size ", t2.bytes(), " bytes, #fingerprints ", t2.fingerprints().size());

    TemplateISO19794_2_2005<uint32_t, Fingerprint> t3(3);
    if (!t3.load(StringUtil::format(R"(%s/%s)", path.c_str(), f3.c_str()))) {
        return;
    }
    Log::test("Template ", t3.id(), ": size ", t3.bytes(), " bytes, #fingerprints ", t3.fingerprints().size());

    if (t1.fingerprints().empty() || t2.fingerprints().empty() || t3.fingerprints().empty()) {
        return;
    }

    const auto test = [](const auto& a, const auto& b) {
        static const int Passes = 5;
        static const int Iterations = 50000;
        static MatchSimilarity match;

        for (auto i = 0; i < Passes; ++i) {
            unsigned int s {};

            const auto start = std::chrono::steady_clock::now();
            for (auto i = 0; i < Iterations; ++i) {
                match.compute(s, a.fingerprints()[0], b.fingerprints()[0]);
            }
            const auto finish = std::chrono::steady_clock::now();
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

            Log::test("Pass ", i + 1, ", similarity of ", a.id(), " and ", b.id(), ": ", s, "% [", Iterations, " iterations in ", ms.count(), "ms]");
        }
    };

    Log::test(Log::LF, "Matching...");
    test(t1, t2);
    Log::test("");
    test(t1, t3);
    Log::test(std::string(LineWidth, '='), Log::LF);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void oneMany(const std::string& path, const std::string& f1, const unsigned int loadFactor)
{
    using TemplateType = TemplateISO19794_2_2005<std::string, Fingerprint>;
    MatchMany<TemplateType> match;

    Log::test(std::string(LineWidth, '='));
    Log::test("1:N match", Log::LF);
    Log::test("Path: ", path);
    Log::test("Template 1: ", f1);
    Log::test("Concurrency: ", match.concurrency());
    Log::test(std::string(LineWidth, '='), Log::LF);

    Log::test("Loading...");

    std::vector<TemplateType> candidates;
    if (!helperLoadPath(candidates, path, loadFactor)) {
        return;
    }
    const auto pathF1 = std::filesystem::path(StringUtil::format(R"(%s/%s)", path.c_str(), f1.c_str()));
    TemplateType probe(pathF1.relative_path().make_preferred().string());
    if (!probe.load(pathF1.string())) {
        return;
    }
    Log::test("Loaded ", candidates.size() + 1, " templates");

    Log::test(Log::LF, "Matching 1:", candidates.size(), "...");

    const auto start = std::chrono::steady_clock::now();
    const auto result = match.oneMany(probe, candidates);
    const auto finish = std::chrono::steady_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

    if (result.second) {
        Log::test("Matched probe [", probe.id(), "] and candidate [", result.second->id(), "] with ", result.first, "% similarity");
    } else {
        Log::test("No matches");
    }
    Log::test("Completed in ", ms.count(), "ms (", ms.count() ? std::round(static_cast<float>(candidates.size()) / ms.count() * 1000) : 0, " fp/s)");
    Log::test(std::string(LineWidth, '='), Log::LF);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void manyMany(const std::string& path, const unsigned int loadFactor)
{
    using TemplateType = TemplateISO19794_2_2005<std::string, Fingerprint>;
    MatchMany<TemplateType> match;

    Log::test(std::string(LineWidth, '='));
    Log::test("Exponential N:N match", Log::LF);
    Log::test("Path: ", path);
    Log::test("Concurrency: ", match.concurrency());
    Log::test(std::string(LineWidth, '='), Log::LF);

    Log::test("Loading...");

    std::vector<TemplateType> templates;
    if (!helperLoadPath(templates, path, loadFactor)) {
        return;
    }
    Log::test("Loaded ", templates.size(), " templates");

    std::vector<unsigned int> scores(templates.size() * templates.size());
    Log::test(Log::LF, "Matching ", scores.capacity(), " permutations...");

    const auto start = std::chrono::steady_clock::now();
    match.manyMany(scores, templates);
    const auto finish = std::chrono::steady_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    Log::test("Completed in ", ms.count(), "ms (", ms.count() ? std::round(static_cast<float>(scores.capacity()) / ms.count() * 1000) : 0, " fp/s)");

    Log::test(Log::LF, "Reporting...");

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

    size_t i {};
    for (const auto& t1 : templates) {
        f << "," << t1.id();
    }
    f << std::endl;
    for (const auto& t1 : templates) {
        f << t1.id();

        for (const auto& t2 : templates) {
            std::ignore = t2;
            f << "," << scores[i++];
        }
        f << std::endl;
    }
    Log::test("Written ", fn);
    Log::test(std::string(LineWidth, '='), Log::LF);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void render(const std::string& path, const std::string& f1, const std::string& f2)
{
    Log::test(std::string(LineWidth, '='));
    Log::test("Minutiae + pairing rendering", Log::LF);
    Log::test("Path: ", path);
    Log::test("Template 1: ", f1);
    Log::test("Template 2: ", f2);
    Log::test(std::string(LineWidth, '='), Log::LF);

    Log::test("Loading...");

    TemplateISO19794_2_2005<uint32_t, FingerprintRenderable> t1(1);
    if (!t1.load(StringUtil::format(R"(%s/%s)", path.c_str(), f1.c_str()))) {
        return;
    }
    Log::test("Template ", t1.id(), ": size ", t1.bytes(), " bytes, #fingerprints ", t1.fingerprints().size());

    TemplateISO19794_2_2005<uint32_t, FingerprintRenderable> t2(2);
    if (!t2.load(StringUtil::format(R"(%s/%s)", path.c_str(), f2.c_str()))) {
        return;
    }
    Log::test("Template ", t2.id(), ": size ", t2.bytes(), " bytes, #fingerprints ", t2.fingerprints().size());

    if (t1.fingerprints().empty() || t2.fingerprints().empty()) {
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

    Log::test(Log::LF, "Rendering...");
    test(t1, t2);
    Log::test(std::string(LineWidth, '='), Log::LF);
}
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int main(const int argc, const char** argv)
{
    const auto param = [](const char** begin, const char** end, const std::string& option) {
        auto it = std::find(begin, end, option);
        if (it != end && ++it != end) {
            return std::string(*it);
        }
        return std::string();
    };

    const auto option = [](const char** begin, const char** end, const std::string& option) { return std::find(begin, end, option) != end; };

    OpenAFIS::Log::test("OpenAFIS: an efficient 1:N fingerprint matching library");
    OpenAFIS::Log::test("Build options:");
    OpenAFIS::Log::test("  MaximumLocalDistance: ", static_cast<int>(OpenAFIS::Param::MaximumLocalDistance));
    OpenAFIS::Log::test("  MaximumGlobalDistance: ", static_cast<int>(OpenAFIS::Param::MaximumGlobalDistance));
    OpenAFIS::Log::test("  MinimumMinutiae: ", static_cast<int>(OpenAFIS::Param::MinimumMinutiae));
    OpenAFIS::Log::test("  MaximumConcurrency: ", static_cast<int>(OpenAFIS::Param::MaximumConcurrency));
    OpenAFIS::Log::test("  MaximumRotations: ", static_cast<int>(OpenAFIS::Param::MaximumRotations), OpenAFIS::Log::LF);

    const auto f1 = param(argv, argv + argc, "--f1");
    const auto f2 = param(argv, argv + argc, "--f2");
    const auto f3 = param(argv, argv + argc, "--f3");
    const auto path = param(argv, argv + argc, "--path");
    const auto loadFactor = atoi(param(argv, argv + argc, "--load-factor").c_str());

    bool command {};
    if (option(argv, argv + argc, "bulk-load")) {
        OpenAFIS::bulkLoad(path);
        command |= true;
    }
    if (option(argv, argv + argc, "one")) {
        OpenAFIS::one(path, f1, f2, f3);
        command |= true;
    }
    if (option(argv, argv + argc, "one-many")) {
        OpenAFIS::oneMany(path, f1, std::max(1, loadFactor));
        command |= true;
    }
    if (option(argv, argv + argc, "many-many")) {
        OpenAFIS::manyMany(path, std::max(1, loadFactor));
        command |= true;
    }
    if (option(argv, argv + argc, "render")) {
        OpenAFIS::render(path, f1, f2);
        command |= true;
    }
    if (option(argv, argv + argc, "--help") || !command) {
        OpenAFIS::Log::test("Usage: openafis-cli [COMMAND]... [OPTIONS]... [--f1 ISO_FILE] [--f2 ISO_FILE] [--f3 ISO_FILE] [--path PATH]", OpenAFIS::Log::LF);
        OpenAFIS::Log::test("Commands:");
        OpenAFIS::Log::test("  bulk-load : load and parse every *.iso below --path");
        OpenAFIS::Log::test("  one       : match --f1,--f2 and --f1,--f3");
        OpenAFIS::Log::test("  one-many  : match --f1 against every *.iso below --path");
        OpenAFIS::Log::test("  many-many : match every *.iso below --path");
        OpenAFIS::Log::test("  render    : generate two SVG's showing minutiae and matched pairs between --f1,--f2");
        OpenAFIS::Log::test("  help      : this screen", OpenAFIS::Log::LF);
        OpenAFIS::Log::test("Options:");
        OpenAFIS::Log::test("  --load-factor : load *.iso below --path multiple times (default 1)", OpenAFIS::Log::LF);
        OpenAFIS::Log::test("Examples:");
        OpenAFIS::Log::test("  openafis-cli many-many --path ~/openafis/data/fvc");
        OpenAFIS::Log::test("  openafis-cli one --f1 db1_b/101_1.iso --f2 db1_b/101_2.iso --f3 db1_b/102_1.iso --path ~/openafis/data/fvc2002");
        OpenAFIS::Log::test("  openafis-cli render --f1 db1_b/101_1.iso --f2 db1_b/101_7.iso --path ~/openafis/data/fvc2002");
    }
    return 0;
}
