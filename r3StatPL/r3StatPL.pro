# -------------------------------------------------
# Project created by QtCreator 2010-05-27T13:59:25
# -------------------------------------------------

unix:TARGET = ./../../lab/r3StatPL.unix/r3StatPL
win32:TARGET = ./../../../lab/r3StatPL.win32/r3StatPL
#unix:TARGET = ./../../lab/r3StatPL/r3StatPL
#win32:TARGET = ./../../../lab/r3StatPL/r3StatPL
CONFIG += qt
CONFIG += console warn_off
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ./../libs/redStat.cpp \
    ./../libs/astro.cpp \
    ./../libs/comfunc.cpp \
    ./../libs/multidim.cpp \
    ./../libs/observatory.cpp \
    ./../libs/fitsdata.cpp \
    ./../libs/identify.cpp \
    ./../libs/cmnk.cpp  \
    ./../libs/ccdmeasurements.cpp \
    ../libs/fitstools.cpp \
    ../libs/sscat.cpp \
    ../libs/fbStream.cpp \
    ../libs/fBuffer.cpp \
    ../libs/spline3.cpp \
    ../libs/spline2d.cpp \
    ../libs/ap.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/ialglib.cpp \
    ../libs/mpccat.cpp \
    ../libs/orbcat.cpp

HEADERS += ../libs/astro.h \
    ../libs/mb.h \
    ../libs/comfunc.h \
    ../libs/astro.h \
    ../libs/redStat.h \
    ../libs/multidim.h \
    ../libs/observatory.h \
    ../libs/fitsdata.h \
    ../libs/fitsio.h \
    ../libs/identify.h \
    ../libs/cmnk.h  \
    ../libs/longnam.h   \
    ./../libs/ccdmeasurements.h \
    ../libs/fitstools.h \
    ../libs/sscat.h \
    ../libs/fbStream.h \
    ../libs/fBuffer.h \
    ../libs/spline3.h \
    ../libs/spline2d.h \
    ../libs/ap.h \
    ../libs/vectGrid3D.h \
    ../libs/ialglib.h \
    ../libs/mpccat.h \
    ../libs/orbcat.h

win32:LIBS += ./../libs/cfitsio.lib \
    ../libs/ablas.dll
unix:LIBS += ./../libs/unix/libcfitsio.a
win32:LIBS += ./../libs/win32/libmb.a
unix:LIBS += ./../libs/unix/libmb.a
