#-------------------------------------------------
#
# Project created by QtCreator 2010-07-02T09:59:44
#
#-------------------------------------------------

QT       -= gui

TARGET = wcs2mks
CONFIG   += console \
    warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../libs/fitsdata.cpp \
    ../libs/comfunc.cpp \
    ../libs/astro.cpp \
    ../libs/multidim.cpp \
    ../libs/ccdmeasurements.cpp \
    ../libs/redStat.cpp \
    ../libs/identify.cpp \
    ../libs/cmnk.cpp \
    ../libs/observatory.cpp \
    ../libs/sscat.cpp \
    ../libs/fBuffer.cpp \
    ../libs/spline3.cpp \
    ../libs/spline2d.cpp \
    ../libs/ap.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/ialglib.cpp
HEADERS += ../libs/fitsdata.h \
    ../libs/comfunc.h \
    ../libs/astro.h \
    ../libs/multidim.h \
    ../libs/mb.h \
    ../libs/ccdmeasurements.h \
    ../libs/redStat.h \
    ../libs/identify.h \
    ../libs/cmnk.h \
    ../libs/observatory.h \
    ../libs/sscat.h \
    ../libs/fBuffer.h \
    ../libs/spline3.h \
    ../libs/spline2d.h \
    ../libs/ap.h \
    ../libs/vectGrid3D.h \
    ../libs/ialglib.h

win32:LIBS += ./../libs/cfitsio.lib
unix:LIBS += ./../libs/unix/libcfitsio.a
win32:LIBS += ./../libs/win32/libmb.a
unix:LIBS += ./../libs/unix/libmb.a
