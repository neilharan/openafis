TARGET = openafis

QT -= core
QT -= gui

INCLUDEPATH += \
    ../3rdparty/OpenBLAS-x86/include \
    ../3rdparty/eigen-3.3.7 \
    ../lib

HEADERS = \
    ../lib/Config.h \
    ../lib/Fingerprint.h \
    ../lib/LMTS.h \
    ../lib/Log.h \
    ../lib/Minutia.h \
    ../lib/Render.h \
    ../lib/Score.h \
    ../lib/Template.h \
    ../lib/TemplateISO19794_2_2005.h

SOURCES = \
    ../lib/Render.cpp \
    ../lib/Score.cpp \
    ../lib/Template.cpp \
    ../lib/TemplateISO19794_2_2005.cpp

*-msvc*: {
    DEFINES += _HAS_EXCEPTIONS=0
    QMAKE_CXXFLAGS += /std:c++17
}
