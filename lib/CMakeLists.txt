cmake_minimum_required (VERSION 3.9)

set(VERSION "0.0.1")

file(GLOB
    LIB_FILES
    *.cpp
    *.h)

include_directories(${PROJECT_SOURCE_DIR}/3rdparty)
add_library(openafis STATIC ${LIB_FILES})
