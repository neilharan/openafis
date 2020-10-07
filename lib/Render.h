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
    static bool pairs(std::string& svg, const Fingerprint& fp1, const Fingerprint& fp2);

private:
    static const unsigned int LineLength = 10;
};

#endif // RENDER_H
