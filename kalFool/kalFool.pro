#-------------------------------------------------
#
# Project created by QtCreator 2010-11-25T17:35:21
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = kalFool
CONFIG += console warn_off debug
CONFIG   -= app_bundle

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
    ../libs/sscatFB.h \
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
unix:LIBS += ./../libs/libcfitsio.a
win32:LIBS += ./../libs/libmb.a
unix:LIBS += ./libmb.a
