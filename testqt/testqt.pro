TEMPLATE = app
//INCLUDEPATH += D:/berezhnoy/develop/libs
//INCLUDEPATH += ./../libs
HEADERS += ../libs/observatory.h \
    ../libs/comfunc.h \
    ../libs/fitsio.h \
    ../libs/fitsdata.h \
    ../libs/redStat.h \
    ../libs/mb.h \
    ../libs/sscat.h \
    ../libs/fBuffer.h \
    ../libs/multidim.h \
    ../libs/identify.h \
    ../libs/cmnk.h \
    ../libs/spline3.h \
    ../libs/spline2d.h \
    ../libs/ap.h \
    ../libs/vectGrid3D.h \
    ../libs/ialglib.h

HEADERS += ../libs/ccdmeasurements.h
SOURCES += ../libs/observatory.cpp \
    ../libs/comfunc.cpp \
    ../libs/fitsdata.cpp \
    ../libs/redStat.cpp \
    ../libs/sscat.cpp \
    ../libs/fBuffer.cpp \
    ../libs/multidim.cpp \
    ../libs/identify.cpp \
    ../libs/cmnk.cpp \
    ../libs/spline3.cpp \
    ../libs/spline2d.cpp \
    ../libs/ap.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/ialglib.cpp

HEADERS += ./../libs/astro.h
SOURCES += main.cpp
SOURCES += ../libs/ccdmeasurements.cpp

SOURCES += ./../libs/astro.cpp
win32:LIBS += ./../libs/cfitsio.lib  \
    ../libs/ablas.dll
unix:LIBS += ./../libs/libcfitsio.a
win32:LIBS += ./../libs/libmb.a
unix:LIBS += ./libmb.a
TARGET = testqt
CONFIG += qt \
    warn_off \
    debug \
    console
//CONFIG += CONFIG \
    += \
    uitools
