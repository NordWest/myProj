#-------------------------------------------------
#
# Project created by QtCreator 2010-12-12T12:09:37
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = refTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../libs/astro.cpp \
    ../libs/fitsdata.cpp \
    ../libs/DynArr.cpp \
    ../libs/comfunc.cpp \
    ../libs/multidim.cpp \
    ../libs/ccdmeasurements.cpp \
    ../libs/redStat.cpp \
    ../libs/observatory.cpp \
    ../libs/identify.cpp \
    ../libs/cmnk.cpp \
    ../libs/sscat.cpp \
    ../libs/fBuffer.cpp \
    ../libs/spline3.cpp \
    ../libs/spline2d.cpp \
    ../libs/ap.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/ialglib.cpp

win32:LIBS += cfitsio.lib  \
    ../libs/ablas.dll
unix:LIBS += ./../libs/libcfitsio.a
win32:LIBS += ./../libs/libmb.a
unix:LIBS += ./libmb.a

CONFIG += console \
    exceptions \
    warn_off \
    debug

HEADERS += ../libs/astro.h \
    ../libs/mb.h \
    ../libs/fitsio.h \
    ../libs/fitsdata.h \
    ../libs/DynArr.h \
    ../libs/comfunc.h \
    ../libs/multidim.h \
    ../libs/ccdmeasurements.h \
    ../libs/redStat.h \
    ../libs/observatory.h \
    ../libs/identify.h \
    ../libs/cmnk.h \
    ../libs/sscat.h \
    ../libs/fBuffer.h \
    ../libs/spline3.h \
    ../libs/spline2d.h \
    ../libs/ap.h \
    ../libs/vectGrid3D.h \
    ../libs/ialglib.h
