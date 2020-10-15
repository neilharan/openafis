TARGET = examples

QT -= core
QT -= gui

INCLUDEPATH += ../lib
SOURCES = ../examples/OneMany.cpp

*-msvc*: {
    DEFINES += _HAS_EXCEPTIONS=0
    QMAKE_CXXFLAGS += /std:c++17
}
contains(CONFIG, release) {
    BUILD_TYPE = "release"
} else {
    BUILD_TYPE = "debug"
}
OBJECTS_DIR = $$BUILD_TYPE/examples
LIBS = $$BUILD_TYPE/openafis.lib
