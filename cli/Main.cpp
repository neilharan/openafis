
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if __cplusplus == 201703L

#include "OpenAFIS.h"
#include "Param.h"
#include "StringUtil.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <random>
#include <sstream>
#include <thread>
#include <vector>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
constexpr auto LineWidth = 100;

using namespace OpenAFIS;


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Helper loads and parses templated from the supplied path, optionally duplicating by a supplied factor...
//
template <class T> static bool helperLoadPath(T& templates, const std::string& path, const int loadFactor)
{
    templates.reserve(loadFactor * 640ll);

    for (const auto& entry : std::filesystem::recursive_directory_iterator(path.c_str())) {
        if (!entry.is_regular_file()) {
            continue;
        }
        const auto& p = entry.path();
        if (StringUtil::lower(p.extension().string()) != ".iso") {
            continue;
        }
        auto& t = [=, &templates]() -> typename T::value_type& {
            if constexpr (std::is_same<typename T::value_type::IdType, std::string>::value) {
                return templates.emplace_back(p.relative_path().make_preferred().string());
            }

            if constexpr (std::is_integral_v<typename T::value_type::IdType>) {
                static typename T::value_type::IdType id{};
                return templates.emplace_back(++id);
            }

            Log::error("unsupported T (this should not be possible!)");
            std::exit(0); // no exceptions
        }();

        if (!t.load(p.string())) {
            Log::error("failed to load ", p.string());
            return false;
        }
        if (t.fingerprints().empty()) {
            Log::error("template is empty ", p.string());
            return false;
        }
    }
    // Duplicate templates if requested (extend size of test dataset)...
    if (loadFactor > 1) {
        const T copy = templates;
        for (auto i = 0; i < loadFactor; ++i) {
            for (const auto& t : copy) {
                templates.emplace_back(t);
            }
        }
        // Shuffle to minimize any unfair advantages from caching/prefetching gained by duplicating templates (this is for test purposes only)...
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(templates.begin(), templates.end(), g);
    }
    templates.shrink_to_fit();
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
    Log::test("Loaded ", count, " templates in ", ms.count(), "ms (requiring ", size, " bytes)");
    Log::test(std::string(LineWidth, '='), Log::LF);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 1:1 matching test...
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
        MatchSimilarity match;
        uint8_t s {};

        const auto start = std::chrono::steady_clock::now();
        match.compute(s, a.fingerprints()[0], b.fingerprints()[0]);
        const auto finish = std::chrono::steady_clock::now();
        const auto us = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);

        Log::test("Similarity of ", a.id(), " and ", b.id(), ": ", static_cast<int>(s), "% [in ", us.count(), "us]");
    };

    Log::test(Log::LF, "Matching...");
    test(t1, t2);
    Log::test("");
    test(t1, t3);
    Log::test(std::string(LineWidth, '='), Log::LF);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 1:N matching test...
//
static void oneMany(const std::string& path, const std::string& f1, const int loadFactor)
{
    using TemplateType = TemplateISO19794_2_2005<std::string, Fingerprint>;
    MatchMany<TemplateType> match;

    Log::test(std::string(LineWidth, '='));
    Log::test("1:N match", Log::LF);
    Log::test("Path: ", path);
    Log::test("Template 1: ", f1);
    Log::test("Load factor: ", loadFactor);
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
    const auto size = std::accumulate(candidates.begin(), candidates.end(), size_t {}, [](size_t sum, const auto& t) { return sum + t.bytes(); });
    Log::test("Loaded ", candidates.size() + 1, " templates (requiring ", size, " bytes)");

    Log::test(Log::LF, "Matching 1:", candidates.size());

    for (auto i = 1; i <= 3; ++i) {
        Log::test(Log::LF, "Pass ", i, "...");
        std::this_thread::sleep_for(std::chrono::seconds(1));

        const auto start = std::chrono::steady_clock::now();
        const auto result = match.oneMany(probe, candidates);
        const auto finish = std::chrono::steady_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

        if (result.second) {
            Log::test(
                "    Matched", Log::LF, "    probe [", probe.id(), "]", Log::LF, "    and candidate [", result.second->id(), "]", Log::LF, "    with ", static_cast<int>(result.first), "% similarity");
        } else {
            Log::test("    No matches");
        }
        Log::test("    Completed in ", ms.count(), "ms (", ms.count() ? static_cast<float>(candidates.size()) / ms.count() * 1000 : 0, " fp/s)");
    }
    Log::test(std::string(LineWidth, '='), Log::LF);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// N:N matching test. Matches every template against every other template; hashing this output is a quick way to check optimisations haven't affected efficacy...
//
static void manyMany(const std::string& path, const int loadFactor)
{
    using TemplateType = TemplateISO19794_2_2005<std::string, Fingerprint>;
    MatchMany<TemplateType> match;

    Log::test(std::string(LineWidth, '='));
    Log::test("N:N match", Log::LF);
    Log::test("Path: ", path);
    Log::test("Load factor: ", loadFactor);
    Log::test("Concurrency: ", match.concurrency());
    Log::test(std::string(LineWidth, '='), Log::LF);

    Log::test("Loading...");

    std::vector<TemplateType> templates;
    if (!helperLoadPath(templates, path, loadFactor)) {
        return;
    }
    const auto size = std::accumulate(templates.begin(), templates.end(), size_t {}, [](size_t sum, const auto& t) { return sum + t.bytes(); });
    Log::test("Loaded ", templates.size(), " templates (requiring ", size, " bytes)");

    std::vector<uint8_t> scores(templates.size() * templates.size());
    Log::test(Log::LF, "Matching ", scores.capacity(), " permutations...");

    const auto start = std::chrono::steady_clock::now();
    match.manyMany(scores, templates);
    const auto finish = std::chrono::steady_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    Log::test("Completed in ", ms.count(), "ms (", ms.count() ? static_cast<float>(scores.capacity()) / ms.count() * 1000 : 0, " fp/s)");

    Log::test(Log::LF, "Reporting...");

    const auto now = []() {
        auto t = std::time(nullptr);
        auto tm = std::localtime(&t);
        std::stringstream ss;
        ss << std::put_time(tm, "%Y-%m-%d-%H-%M-%S");
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
            f << "," << static_cast<int>(scores[i++]);
        }
        f << std::endl;
    }
    Log::test("Written ", fn);
    Log::test(std::string(LineWidth, '='), Log::LF);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if 0

//NJH-TODO
static void bench(const std::string& path, const std::string& f1, const int loadFactor)
{
    using TemplateType = TemplateISO19794_2_2005<uint32_t, Fingerprint>;
    MatchMany<TemplateType> match;

    Log::test(std::string(LineWidth, '='));
    Log::test("Benchmark", Log::LF);
    Log::test("Path: ", path);
    Log::test("Template 1: ", f1);
    Log::test("Load factor: ", loadFactor);
    Log::test("Concurrency: ", match.concurrency());
    Log::test(std::string(LineWidth, '='), Log::LF);

    Log::test("Loading...");

    std::vector<TemplateType> candidates;
    if (!helperLoadPath(candidates, path, loadFactor)) {
        return;
    }
    const auto pathF1 = std::filesystem::path(StringUtil::format(R"(%s/%s)", path.c_str(), f1.c_str()));
    TemplateType probe(0);
    if (!probe.load(pathF1.string())) {
        return;
    }
    const auto size = std::accumulate(candidates.begin(), candidates.end(), size_t {}, [](size_t sum, const auto& t) { return sum + t.bytes(); });
    Log::test("Loaded ", candidates.size() + 1, " templates (requiring ", size, " bytes)");

    Log::test(Log::LF, "Matching 1:", candidates.size());

    const auto result = match.oneMany(probe, candidates);

    if (result.second) {
        Log::test(
            "    Matched", Log::LF, "    probe [", probe.id(), "]", Log::LF, "    and candidate [", result.second->id(), "]", Log::LF, "    with ", static_cast<int>(result.first), "% similarity");
    } else {
        Log::test("    No matches");
    }
    Log::test(std::string(LineWidth, '='), Log::LF);
}
#endif


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


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int main(const int argc, const char** argv)
{
    const auto param = [](const char** begin, const char** end, const std::string& option) {
        auto *it = std::find(begin, end, option);
        if (it != end && ++it != end) {
            return std::string(*it);
        }
        return std::string();
    };

    const auto hasOption = [](const char** begin, const char** end, const std::string& option) { return std::find(begin, end, option) != end; };

    Log::init();
    Log::test("OpenAFIS: an efficient 1:N fingerprint matching library (", Param::EnableSIMD ? InstructionSet : "SCALAR", ")");
    Log::test("Build options:");
    Log::test("    MaximumLocalDistance: ", static_cast<int>(Param::MaximumLocalDistance));
    Log::test("    MaximumGlobalDistance: ", static_cast<int>(Param::MaximumGlobalDistance));
    Log::test("    MinimumMinutiae: ", static_cast<int>(Param::MinimumMinutiae));
    Log::test("    MaximumConcurrency: ", static_cast<int>(Param::MaximumConcurrency));
    Log::test("    MaximumRotations: ", static_cast<int>(Param::MaximumRotations), Log::LF);

    const auto f1 = param(argv, argv + argc, "--f1");
    const auto f2 = param(argv, argv + argc, "--f2");
    const auto f3 = param(argv, argv + argc, "--f3");
    const auto path = param(argv, argv + argc, "--path");
    const auto loadFactor = std::atoi(param(argv, argv + argc, "--load-factor").c_str());

    bool command {};
    if (hasOption(argv, argv + argc, "bulk-load")) {
        bulkLoad(path);
        command |= true;
    }
    if (hasOption(argv, argv + argc, "one")) {
        one(path, f1, f2, f3);
        command |= true;
    }
    if (hasOption(argv, argv + argc, "one-many")) {
        oneMany(path, f1, std::max(1, loadFactor));
        command |= true;
    }
    if (hasOption(argv, argv + argc, "many-many")) {
        manyMany(path, std::max(1, loadFactor));
        command |= true;
    }
    if (hasOption(argv, argv + argc, "render")) {
        render(path, f1, f2);
        command |= true;
    }
    if (hasOption(argv, argv + argc, "--help") || !command) {
        Log::test("Usage: openafis-cli [COMMAND]... [OPTIONS]... [--f1 ISO_FILE] [--f2 ISO_FILE] [--f3 ISO_FILE] [--path PATH]", Log::LF);
        Log::test("Commands:");
        Log::test("    bulk-load : load and parse every *.iso below --path");
        Log::test("    one       : match --f1,--f2 and --f1,--f3");
        Log::test("    one-many  : match --f1 against every *.iso below --path");
        Log::test("    many-many : match every *.iso below --path");
        Log::test("    render    : generate two SVG's showing minutiae and matched pairs between --f1,--f2");
        Log::test("    help      : this screen", Log::LF);
        Log::test("Options:");
        Log::test("    --load-factor : load *.iso below --path multiple times (default 1)", Log::LF);
        Log::test("Examples:");
        Log::test("    openafis-cli many-many --path data/valid");
        Log::test("    openafis-cli one-many --f1 fvc2002/DB1_B/101_2.iso --load-factor 4000 --path data/valid");
        Log::test("    openafis-cli render --f1 DB1_B/101_1.iso --f2 DB1_B/101_7.iso --path data/valid/fvc2002");
    }
    return 0;
}

#else

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int main(const int, const char**)
{
    // C++17 required
    return 0;
}

#endif
