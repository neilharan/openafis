
#include "Render.h"
#include "Log.h"
#include "Match.h"
#include "StringUtil.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Render::minutiae(std::string& svg, const Fingerprint& fp)
{
#ifdef OPENAFIS_RENDER_AVAILABLE
    open(svg, fp.dimensions());
    addMinutiae(svg, fp);
    close(svg);
    return true;
#else
    logError("rendering not possible, OPENAFIS_RENDER_AVAILABLE is not defined");

    svg = "Please #define OPENAFIS_RENDER_AVAILABLE to enable this feature";
    std::ignore = fp;
    return false;
#endif
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Render::pairs(std::string& svg1, std::string& svg2, const Fingerprint& fp1, const Fingerprint& fp2)
{
#ifdef OPENAFIS_RENDER_AVAILABLE
    open(svg1, fp1.dimensions());
    open(svg2, fp2.dimensions());
    addPairs(svg1, svg2, fp1, fp2);
    close(svg1);
    close(svg2);
    return true;
#else
    std::ignore = svg1;
    std::ignore = svg2;
    std::ignore = fp1;
    std::ignore = fp2;
    return false;
#endif
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Render::all(std::string& svg1, std::string& svg2, const Fingerprint& fp1, const Fingerprint& fp2)
{
#ifdef OPENAFIS_RENDER_AVAILABLE
    open(svg1, fp1.dimensions());
    open(svg2, fp2.dimensions());
    addMinutiae(svg1, fp1);
    addMinutiae(svg2, fp2);
    addPairs(svg1, svg2, fp1, fp2);
    close(svg1);
    close(svg2);
    return true;
#else
    std::ignore = svg1;
    std::ignore = svg2;
    std::ignore = fp1;
    std::ignore = fp2;
    return false;
#endif
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Render::addMinutiae(std::string& svg, const Fingerprint& fp)
{
#ifdef OPENAFIS_RENDER_AVAILABLE
    for (const auto& minutia : fp.minutiae()) {
        const auto r = FastMath::degreesToRadians(minutia.angle());
        const auto x1 = minutia.x();
        const auto y1 = minutia.y();
        const auto x2 = x1 + static_cast<int>(LineLength * std::cos(r));
        const auto y2 = y1 + static_cast<int>(LineLength * std::sin(r));
        svg += StringUtil::format(R"(<line x1="%d" y1="%d" x2="%d" y2="%d"/>)", x1, y1, x2, y2);

        if (minutia.type() == Minutia::Type::RidgeBifurcation) {
            svg += StringUtil::format(R"(<rect x="%d" y="%d" width="8" height="8"/>)", x1 - 2, y1 - 2);
        } else {
            svg += StringUtil::format(R"(<circle cx="%d" cy="%d" r="4"/>)", x1, y1);
        }
    }
#else
    std::ignore = svg;
    std::ignore = fp;
#endif
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Render::addPairs(std::string& svg1, std::string& svg2, const Fingerprint& fp1, const Fingerprint& fp2)
{
#ifdef OPENAFIS_RENDER_AVAILABLE
    if (fp1.dimensions() != fp2.dimensions()) {
        logError("mismatched dimensions not supported");
        return;
    }
    const auto scaleX = static_cast<float>(fp1.dimensions().first) / 256.0f;
    const auto scaleY = static_cast<float>(fp1.dimensions().second) / 256.0f;

    MinutiaPoint::Pair::Set pairs;
    const Match<decltype(pairs)> match;
    match.compute(pairs, fp1, fp2);

    for (const auto *p : pairs) {
        const auto x1 = std::lround(static_cast<float>(p->candidate()->x()) * scaleX);
        const auto y1 = std::lround(static_cast<float>(p->candidate()->y()) * scaleY);
        svg1 += StringUtil::format(R"(<circle cx="%d" cy="%d" r="8" stroke="#0000ff" stroke-width="2" fill="none"/>)", x1, y1);
        svg1 += StringUtil::format(R"(<text x="%d" y="%d" stroke="#0000ff" class="small">%.2f</text>)", x1 + 9, y1 + 2, p->similarity());

        const auto x2 = std::lround(static_cast<float>(p->probe()->x()) * scaleX);
        const auto y2 = std::lround(static_cast<float>(p->probe()->y()) * scaleY);
        svg2 += StringUtil::format(R"(<circle cx="%d" cy="%d" r="8" stroke="#0000ff" stroke-width="2" fill="none"/>)", x2, y2);
        svg2 += StringUtil::format(R"(<text x="%d" y="%d" stroke="#0000ff" class="small">%.2f</text>)", x2 + 9, y2 + 2, p->similarity());
    }
#else
    std::ignore = svg1;
    std::ignore = svg2;
    std::ignore = fp1;
    std::ignore = fp2;
#endif
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Render::open(std::string& svg, const Dimensions& dimensions)
{
    svg.reserve(1000);
    svg += StringUtil::format(R"(<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d" stroke="#545454" fill="#121212">)", dimensions.first, dimensions.second);
    svg += R"(
    <style>
        .small { font-size: 8px; font-family: monospace; }
    </style>
    )";
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Render::close(std::string& svg) { svg += "</svg>"; }
