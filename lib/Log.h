#ifndef LOG_H
#define LOG_H


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <string_view>


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace OpenAFIS
{

class Log
{
public:
    static void init()
    {
        std::cout << std::fixed;
        std::cout.precision(1);
    }

    static constexpr std::string_view LF = "\n";

    template <typename... A> static void test(A&&... args) { (std::cout << ... << std::forward<A>(args)) << std::endl << std::flush; }

    template <typename... A> static void info(A&&... args)
    {
        std::cout << __FUNCTION__ << ": ";
        (std::cout << ... << std::forward<A>(args)) << std::endl;
    }

    template <typename... A> static void error(A&&... args)
    {
        std::cerr << __FUNCTION__ << ": ";
        (std::cerr << ... << std::forward<A>(args)) << std::endl;
    }
};
}

#endif // LOG_H
