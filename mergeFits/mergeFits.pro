#-------------------------------------------------
#
# Project created by QtCreator 2011-11-17T21:00:38
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ./../../lab/ruler3/mergeFits
#TARGET = mergeFits
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
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
    ../libs/vectGrid3D.cpp \
    ../libs/alglib/ap.cpp \
    ../libs/alglib/interpolation.cpp \
    ../libs/alglib/specialfunctions.cpp \
    ../libs/alglib/solvers.cpp \
    ../libs/alglib/optimization.cpp \
    ../libs/alglib/linalg.cpp \
    ../libs/alglib/integration.cpp \
    ../libs/alglib/alglibmisc.cpp \
    ../libs/alglib/alglibinternal.cpp \
    ../libs/sysCorr.cpp \
    ../libs/fitstools.cpp

win32:LIBS += ./../lib/win32/cfitsio.lib
unix:LIBS += ./../libs/unix/libcfitsio.a

win32:LIBS += ./../libs/win32/libmb.a
unix:LIBS += ./../libs/unix/libmb.a
win32:LIBS += ./../libs/win32/libastro.a
unix:LIBS += ./../libs/unix/libastro.a
CONFIG += console \
    exceptions \
    warn_off

HEADERS += ../astro/astro.h \
    ../libs/comfunc.h \
    ../libs/sscat.h \
    ../libs/redStat.h \
    ../libs/observatory.h \
    ../libs/multidim.h \
    ../libs/fitsio.h \
    ../libs/fitsdata.h \
    ../libs/fBuffer.h \
    ../libs/ccdmeasurements.h \
    ../mb/mb.h \
    ../libs/identify.h \
    ../libs/cmnk.h \
    ../libs/vectGrid3D.h \
    ../libs/alglib/ap.h \
    ../libs/alglib/interpolation.h \
    ../libs/alglib/specialfunctions.h \
    ../libs/alglib/solvers.h \
    ../libs/alglib/optimization.h \
    ../libs/alglib/linalg.h \
    ../libs/alglib/integration.h \
    ../libs/alglib/alglibmisc.h \
    ../libs/alglib/alglibinternal.h \
    ../libs/sysCorr.h \
    ../libs/fitstools.h
