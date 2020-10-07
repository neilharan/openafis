#ifndef STRINGUTIL_H
#define STRINGUTIL_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <string>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class StringUtil
{
public:
    static std::string center(const std::string &s, const int width = 80) { return std::string((width - s.length()) / 2, ' ') + s; }

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

#endif // STRINGUTIL_H
