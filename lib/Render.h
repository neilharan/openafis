#ifndef RENDER_H
#define RENDER_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Fingerprint.h"

#include <iostream>
#include <string>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Render
{
public:
    static bool minutiae(std::string& svg, const Fingerprint& fp);
    static bool pairs(std::string& svg, const Fingerprint& fp1, const Fingerprint& fp2);

private:
    static const unsigned int LineLength = 10;

    template <typename... A> static std::string format(const std::string& format, A... args)
    {
        const auto size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
        if (size <= 0) {
            return "";
        }
        const std::unique_ptr<char[]> buf(new char[size]);
        snprintf(buf.get(), size, format.c_str(), args...);
        return std::string(buf.get(), buf.get() + size - 1);
    }
};

#endif // RENDER_H
