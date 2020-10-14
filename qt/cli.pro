TARGET = openafis-cli
OBJECTS_DIR = openafis-cli

QT -= core
QT -= gui

INCLUDEPATH += ../lib
SOURCES = ../cli/Main.cpp

*-msvc*: {
    DEFINES += _HAS_EXCEPTIONS=0
    QMAKE_CXXFLAGS += /std:c++17
}
contains(CONFIG, release) {
    BUILD_TYPE = "release"
} else {
    BUILD_TYPE = "debug"
}
#LIBS = $$BUILD_TYPE/openafis.lib
LIBS = x64/$$BUILD_TYPE/openafis.lib
