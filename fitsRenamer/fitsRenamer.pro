#-------------------------------------------------
#
# Project created by QtCreator 2010-11-19T12:38:13
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = fitsRenamer
CONFIG   -= app_bundle

TEMPLATE = app
CONFIG += console \
    exceptions \
    warn_off

SOURCES += main.cpp \
    ../libs/astro.cpp \
    ../libs/comfunc.cpp \
    ../libs/sscat.cpp \
    ../libs/redStat.cpp \
    ../libs/observatory.cpp \
    ../libs/multidim.cpp \
    ../libs/fitsdata.cpp \
    ../libs/fBuffer.cpp \
    ../libs/ccdmeasurements.cpp \
    ../libs/identify.cpp \
    ../libs/cmnk.cpp \
    ../libs/spline3.cpp \
    ../libs/spline2d.cpp \
    ../libs/ap.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/ialglib.cpp
win32:LIBS += ./../libs/cfitsio.lib  \
    ../libs/ablas.dll
unix:LIBS += ./../libs/libcfitsio.a
win32:LIBS += ./../libs/win/libmb.a
unix:LIBS += ./../libs/unix/libmb.a

HEADERS += ../libs/mb.hpp \
    ../libs/astro.h \
    ../libs/comfunc.h \
    ../libs/sscat.h \
    ../libs/redStat.h \
    ../libs/observatory.h \
    ../libs/multidim.h \
    ../libs/fitsio.h \
    ../libs/fitsdata.h \
    ../libs/fBuffer.h \
    ../libs/ccdmeasurements.h \
    ../libs/mb.h \
    ../libs/identify.h \
    ../libs/cmnk.h \
    ../libs/spline3.h \
    ../libs/spline2d.h \
    ../libs/ap.h \
    ../libs/vectGrid3D.h \
    ../libs/ialglib.h

