#ifndef RENDER_H
#define RENDER_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Config.h"
#include "Fingerprint.h"

#include <string>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Render
{
public:
    static bool minutiae(std::string& svg, const Fingerprint& fp);
    static bool pairs(std::string& svg1, std::string& svg2, const Fingerprint& fp1, const Fingerprint& fp2);
    static bool all(std::string& svg1, std::string& svg2, const Fingerprint& fp1, const Fingerprint& fp2);

private:
    static const unsigned int LineLength = 10;

    static void addMinutiae(std::string& svg, const Fingerprint& fp);
    static void addPairs(std::string& svg1, std::string& svg2, const Fingerprint& fp1, const Fingerprint& fp2);
};

#endif // RENDER_H
