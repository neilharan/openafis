cmake_minimum_required (VERSION 3.8.0)

set(VERSION "0.0.1")

include_directories(${PROJECT_SOURCE_DIR}/lib)
add_executable(openafis-cli Main.cpp)
target_link_libraries(openafis-cli PRIVATE openafis Threads::Threads)
