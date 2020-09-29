
#include "Render.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Render::minutiae(std::string& svg, const Fingerprint& fp)
{
#ifdef OPENAFIS_RENDER_AVAILABLE
    svg.reserve(1000);
    svg += format(R"(<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d" stroke="#121212" fill="#121212">)", fp.dimensions().first, fp.dimensions().second);

    for (const auto& minutia : fp.minutiae()) {
        const auto theta = toRadians(minutia.angle());
        const auto x1 = minutia.x();
        const auto y1 = minutia.y();
        const auto x2 = x1 + static_cast<int>(LineLength * cos(theta));
        const auto y2 = y1 + static_cast<int>(LineLength * sin(theta));
        svg += format(R"(<line x1="%d" y1="%d" x2="%d" y2="%d"/>)", x1, y1, x2, y2);
        svg += format(R"(<circle cx="%d" cy="%d" r="2" fill="%s"/>)", x1, y1, minutia.type() == Minutia::Type::RidgeBifurcation ? "#ff0000" : "#0000ff");
    }
    svg += "</svg>";
    return true;
#else
    return false;
#endif
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Render::pairs(std::string&, const Fingerprint&, const Fingerprint&)
{
#ifdef OPENAFIS_RENDER_AVAILABLE
    return false;
#else
    return false;
#endif
}
