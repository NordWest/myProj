# -------------------------------------------------
# Project created by QtCreator 2010-04-05T11:45:30
# -------------------------------------------------
QT -= gui
TARGET = mPack
CONFIG += console \
    qt \
    warn_off
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ../libs/fitsdata.cpp \
    ../libs/redStat.cpp \
    ../libs/multidim.cpp \
    ../libs/identify.cpp \
    ../libs/ccdmeasurements.cpp \
    ../libs/astro.cpp \
    ../libs/comfunc.cpp \
    ../libs/cmnk.cpp \
    ../libs/sscat.cpp \
    ../libs/fBuffer.cpp \
    ../libs/observatory.cpp \
    ../libs/spline3.cpp \
    ../libs/spline2d.cpp \
    ../libs/ap.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/ialglib.cpp
HEADERS += ../libs/fitsdata.h \
    ../libs/redStat.h \
    ../libs/multidim.h \
    ../libs/mb.h \
    ../libs/identify.h \
    ../libs/ccdmeasurements.h \
    ../libs/astro.h \
    ../libs/comfunc.h \
    ../libs/astro.hpp \
    ../libs/cmnk.h \
    ../libs/sscat.h \
    ../libs/fBuffer.h \
    ../libs/observatory.h \
    ../libs/spline3.h \
    ../libs/spline2d.h \
    ../libs/ap.h \
    ../libs/vectGrid3D.h \
    ../libs/ialglib.h
win32:LIBS += ./../libs/cfitsio.lib  \
    ../libs/ablas.dll
unix:LIBS += ./../libs/unix/libcfitsio.a
unix:LIBS += ../libs/unix/libmb.a
