TARGET = openafis

QT -= core
QT -= gui

INCLUDEPATH += \
    ../3rdparty \
    ../lib \
    ../OpenBLAS-x86/include

HEADERS = \
    ../lib/Config.h \
    ../lib/Log.h \
    ../lib/Score.h \
    ../lib/Template.h \
    ../lib/TemplateISO19794_2_2005.h

SOURCES = \
    ../lib/Score.cpp \
    ../lib/Template.cpp \
    ../lib/TemplateISO19794_2_2005.cpp

*-msvc*: {
    DEFINES += _HAS_EXCEPTIONS=0
    QMAKE_CXXFLAGS += /std:c++17
}
