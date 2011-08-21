#-------------------------------------------------
#
# Project created by QtCreator 2011-04-06T12:14:41
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = redStatSelect
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ./../libs/redStat.cpp \
    ./../libs/astro.cpp \
    ./../libs/comfunc.cpp \
    ../libs/sscat.cpp \
    ../libs/fBuffer.cpp \
    ../libs/multidim.cpp \
    ./../libs/observatory.cpp \
    ./../libs/fitsdata.cpp \
    ./../libs/identify.cpp \
    ./../libs/cmnk.cpp  \
    ./../libs/ccdmeasurements.cpp \
    ../libs/fitstools.cpp \
    ../libs/spline3.cpp \
    ../libs/spline2d.cpp \
    ../libs/ap.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/ialglib.cpp

HEADERS += ../libs/astro.h \
    ../libs/mb.h \
    ../libs/comfunc.h \
    ../libs/astro.h \
    ../libs/redStat.h \
    ../libs/sscat.h \
    ../libs/fBuffer.h \
    ../libs/multidim.h \
    ../libs/observatory.h \
    ../libs/fitsdata.h \
    ../libs/fitsio.h \
    ../libs/identify.h \
    ../libs/cmnk.h  \
    ../libs/longnam.h   \
    ./../libs/ccdmeasurements.h \
    ../libs/fitstools.h \

win32:LIBS += ./../libs/win/libmb.a
unix:LIBS += ./../libs/unix/libmb.a
win32:LIBS += ./../libs/cfitsio.lib \
    ../libs/ablas.dll
unix:LIBS += ./../libs/libcfitsio.a
