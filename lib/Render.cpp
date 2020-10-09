
#include "Render.h"
#include "Log.h"
#include "Match.h"
#include "StringUtil.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Render::minutiae(std::string& svg, const Fingerprint& fp)
{
#ifdef OPENAFIS_RENDER_AVAILABLE
    svg.reserve(1000);
    svg += StringUtil::format(R"(<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d" stroke="#121212" fill="#121212">)", fp.dimensions().first, fp.dimensions().second);
    addMinutiae(svg, fp);
    svg += "</svg>";
    return true;
#else
    logError("rendering not possible, OPENAFIS_RENDER_AVAILABLE is not defined");

    svg = "Please #define OPENAFIS_RENDER_AVAILABLE to enable this feature";
    (void)fp;
    return false;
#endif
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Render::pairs(std::string& svg1, std::string& svg2, const Fingerprint& fp1, const Fingerprint& fp2)
{
#ifdef OPENAFIS_RENDER_AVAILABLE
    svg1.reserve(1000);
    svg1 += StringUtil::format(R"(<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d" stroke="#121212" fill="#121212">)", fp1.dimensions().first, fp1.dimensions().second);
    svg2.reserve(1000);
    svg2 += StringUtil::format(R"(<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d" stroke="#121212" fill="#121212">)", fp1.dimensions().first, fp1.dimensions().second);

    addPairs(svg1, svg2, fp1, fp2);

    svg1 += "</svg>";
    svg2 += "</svg>";
    return true;
#else
    return false;
#endif
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Render::all(std::string& svg1, std::string& svg2, const Fingerprint& fp1, const Fingerprint& fp2)
{
#ifdef OPENAFIS_RENDER_AVAILABLE
    svg1.reserve(1000);
    svg1 += StringUtil::format(R"(<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d" stroke="#545454" fill="#121212">)", fp1.dimensions().first, fp1.dimensions().second);
    svg2.reserve(1000);
    svg2 += StringUtil::format(R"(<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d" stroke="#545454" fill="#121212">)", fp1.dimensions().first, fp1.dimensions().second);

    addMinutiae(svg1, fp1);
    addMinutiae(svg2, fp2);
    addPairs(svg1, svg2, fp1, fp2);

    svg1 += "</svg>";
    svg2 += "</svg>";
    return true;
#else
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
            svg += StringUtil::format(R"(<rect x="%d" y="%d" width="4" height="4"/>)", x1 - 2, y1 - 2);
        } else {
            svg += StringUtil::format(R"(<circle cx="%d" cy="%d" r="2"/>)", x1, y1);
        }
    }
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

    MinutiaPoint::RenderPair pairs;
    Match<MinutiaPoint::RenderPair> match;
    match.compute(pairs, fp1, fp2);

    for (const auto& m : pairs.first) {
        svg1 += StringUtil::format(R"(<circle cx="%d" cy="%d" r="5" stroke="#ff0000" fill="none"/>)", std::lround(static_cast<float>(m->x()) * scaleX), std::lround(static_cast<float>(m->y()) * scaleY));
    }
    for (const auto& m : pairs.second) {
        svg2 += StringUtil::format(R"(<circle cx="%d" cy="%d" r="5" stroke="#ff0000" fill="none"/>)", std::lround(static_cast<float>(m->x()) * scaleX), std::lround(static_cast<float>(m->y()) * scaleY));
    }
#endif
}
