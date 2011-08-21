# -------------------------------------------------
# Project created by QtCreator 2010-03-29T11:20:21
# -------------------------------------------------
# QT -= gui
TARGET = plateWCS
unix:TARGET=./../../lab/ruler3PL.unix/plateWCS
win32:TARGET=./../../../lab/ruler3PL.win32/plateWCS
CONFIG += console \
    qt \
    warn_off


# CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ../libs/fitsdata.cpp \
    ../libs/comfunc.cpp \
    #../libs/astro.cpp \
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
win32:LIBS += ./../libs/cfitsio.lib  \
    ../libs/ablas.dll
unix:LIBS += ./../libs/unix/libcfitsio.a \
            ./../libs/unix/libmb.a \
            ./../libs/unix/libastro.a
win32:LIBS += ./../libs/win32/libmb.a \
              ./../libs/win32/libastro.a

