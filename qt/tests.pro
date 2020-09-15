TARGET = tests

QT -= core
QT -= gui

INCLUDEPATH += ../lib
SOURCES = ../tests/Tests.cpp

*-msvc*: {
    DEFINES += _HAS_EXCEPTIONS=0
    QMAKE_CXXFLAGS += /std:c++17
}
contains(CONFIG, release) {
    BUILD_TYPE = "release"
} else {
    BUILD_TYPE = "debug"
}
LIBS = $$BUILD_TYPE/openafis.lib