#ifndef RENDER_H
#define RENDER_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Fingerprint.h"

#include <string>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{

class Render
{
public:
    static bool minutiae(std::string& svg, const FingerprintRenderable& fp);
    static bool pairs(std::string& svg1, std::string& svg2, const FingerprintRenderable& fp1, const FingerprintRenderable& fp2);
    static bool all(std::string& svg1, std::string& svg2, const FingerprintRenderable& fp1, const FingerprintRenderable& fp2);

private:
    static constexpr unsigned int LineLength = 10;

    static void addMinutiae(std::string& svg, const FingerprintRenderable& fp);
    static void addPairs(std::string& svg1, std::string& svg2, const FingerprintRenderable& fp1, const FingerprintRenderable& fp2);
    static void open(std::string& svg, const Dimensions& dimensions);
    static void close(std::string& svg);
};
}

#endif // RENDER_H
