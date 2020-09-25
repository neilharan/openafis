TARGET = openafis

QT -= core
QT -= gui

INCLUDEPATH += \
    ../3rdparty/delaunator-cpp/include \
    ../lib

HEADERS = \
    $$PWD/../lib/Config.h \
    $$PWD/../lib/Field.h \
    $$PWD/../lib/Fingerprint.h \
    $$PWD/../lib/Log.h \
    $$PWD/../lib/Minutia.h \
    $$PWD/../lib/Render.h \
    $$PWD/../lib/Score.h \
    $$PWD/../lib/Template.h \
    $$PWD/../lib/TemplateISO19794_2_2005.h \
    $$PWD/../lib/Triplet.h

SOURCES = \
    ../lib/Render.cpp \
    ../lib/Score.cpp \
    ../lib/Template.cpp \
    ../lib/TemplateISO19794_2_2005.cpp

*-msvc*: {
    DEFINES += _HAS_EXCEPTIONS=0
    QMAKE_CXXFLAGS += /std:c++17
}
