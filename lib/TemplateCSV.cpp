
#include "TemplateCSV.h"
#include "Log.h"

#include <fstream>
#include <iostream>
#include <sstream>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CSV reader used for debug only:
//     First line is width,height
//     Followed by minutiae in the form type,x,y,angle (radians)
//
bool TemplateCSV::load(const std::string& path)
{
    std::ifstream f(path, std::ifstream::in);
    if (!f) {
        logError("unable to open " << path);
        return false;
    }
    std::vector<std::vector<Minutia>> fps;
    fps.reserve(1);
    auto& minutiae = fps.emplace_back();

    std::string line;
    unsigned int width {}, height {};

    if (std::getline(f, line)) {
        std::stringstream ss(line);

        ss >> width;
        if (ss.peek() == ',') {
            ss.ignore();
        }
        ss >> height;
    }
    if (!width || !height) {
        logError("invalid width or height " << path);
        return false;
    }
    unsigned int type {}, x {}, y {};
    float angle {};

    while (std::getline(f, line)) {
        std::stringstream ss(line);

        ss >> type;
        if (ss.peek() == ',') {
            ss.ignore();
        }
        ss >> x;
        if (ss.peek() == ',') {
            ss.ignore();
        }
        ss >> y;
        if (ss.peek() == ',') {
            ss.ignore();
        }
        ss >> angle;

        if (!type) {
            logError("invalid minutia type " << path);
            return false;
        }
        minutiae.emplace_back(Minutia::Type(type), x, y, FastMath::radiansToDegrees(angle));
    }
    return Template::load(std::make_pair(width, height), fps);
}
