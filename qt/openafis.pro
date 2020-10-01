TARGET = openafis

QT -= core
QT -= gui

INCLUDEPATH += \
    ../3rdparty/delaunator-cpp/include \
    ../lib

HEADERS = \
    $$PWD/../lib/Config.h \
    $$PWD/../lib/Dimensions.h \
    $$PWD/../lib/FastMath.h \
    $$PWD/../lib/Field.h \
    $$PWD/../lib/Fingerprint.h \
    $$PWD/../lib/Log.h \
    $$PWD/../lib/Minutia.h \
    $$PWD/../lib/MinutiaPoint.h \
    $$PWD/../lib/Param.h \
    $$PWD/../lib/Render.h \
    $$PWD/../lib/Score.h \
    $$PWD/../lib/Template.h \
    $$PWD/../lib/TemplateISO19794_2_2005.h \
    $$PWD/../lib/Triplet.h

SOURCES = \
    ../lib/FastMath.cpp \
    ../lib/Fingerprint.cpp \
    ../lib/Param.cpp \
    ../lib/Render.cpp \
    ../lib/Score.cpp \
    ../lib/Template.cpp \
    ../lib/TemplateISO19794_2_2005.cpp \
    ../lib/Triplet.cpp

*-msvc*: {
    # /arch:AVX
    DEFINES += _HAS_EXCEPTIONS=0
    QMAKE_CXXFLAGS += /GS- /GT /Oy /std:c++17
    QMAKE_CXXFLAGS_EXCEPTIONS_ON -= -EHsc
}
