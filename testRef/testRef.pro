# -------------------------------------------------
# Project created by QtCreator 2010-06-23T20:49:13
# -------------------------------------------------
QT -= gui
TARGET = testRef
CONFIG += console \
    warn_off
CONFIG -= app_bundle
win32:LIBS += ./../libs/cfitsio.lib
unix:LIBS += ./../libs/libcfitsio.a
win32:LIBS += ./../libs/libmb.a
unix:LIBS += ./libmb.a
TEMPLATE = app
SOURCES += main.cpp \
    ../libs/redStat.cpp \
    ../libs/fitsdata.cpp \
    ../libs/comfunc.cpp \
    ../libs/astro.cpp \
    ../libs/multidim.cpp \
    ../libs/sscat.cpp \
    ../libs/identify.cpp \
    ../libs/cmnk.cpp \
    ../libs/ccdmeasurements.cpp \
    ../libs/fBuffer.cpp \
    ../libs/observatory.cpp \
    ../libs/spline3.cpp \
    ../libs/spline2d.cpp \
    ../libs/ap.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/ialglib.cpp
HEADERS += ../libs/redStat.h \
    ../libs/fitsdata.h \
    ../libs/comfunc.h \
    ../libs/astro.h \
    ../libs/mb.h \
    ../libs/multidim.h \
    ../libs/sscat.h \
    ../libs/identify.h \
    ../libs/cmnk.h \
    ../libs/ccdmeasurements.h \
    ../libs/fBuffer.h \
    ../libs/astro.hpp \
    ../libs/observatory.h \
    ../libs/spline3.h \
    ../libs/spline2d.h \
    ../libs/ap.h \
    ../libs/vectGrid3D.h \
    ../libs/ialglib.h
