cmake_minimum_required (VERSION 3.9)

set(VERSION "0.0.1")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CXXFeatureCheck)
cxx_feature_check(STEADY_CLOCK)

include_directories(${PROJECT_SOURCE_DIR}/lib)
add_executable(openafis-cli Main.cpp)
target_link_libraries(openafis-cli PRIVATE openafis Threads::Threads)
