# -------------------------------------------------
# Project created by QtCreator 2010-05-27T13:59:25
# -------------------------------------------------
#QT -= gui
unix:TARGET = ./../../lab/r3Stat/r3Stat
win32:TARGET = ./../../../lab/r3Stat/r3Stat
CONFIG += qt
CONFIG += console warn_off debug
CONFIG -= app_bundle
TEMPLATE = app
#SOURCES += main.cpp \
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

HEADERS += ../libs/astro.h \
    ../libs/mb.h \
    ../libs/comfunc.h \
    ../libs/astro.h \
    ../libs/redStat.h \
    ../libs/multidim.h \
    ../libs/observatory.h \
  #      stdafx.h \
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
    ../libs/ialglib.h

win32:LIBS += ./../libs/cfitsio.lib \
    ../libs/ablas.dll
unix:LIBS += ./../libs/libcfitsio.a
win32:LIBS += ./../libs/libmb.a
unix:LIBS += ./libmb.a
#INCLUDEPATH += C:\Qt\GnuWin32\include
