# -------------------------------------------------
# Project created by QtCreator 2010-03-29T11:20:21
# -------------------------------------------------
# QT -= gui
unix:TARGET = ./../bin/plateWCS
win32:TARGET = ./../../bin/plateWCS

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
    ../libs/sysCorr.cpp
HEADERS += ../libs/fitsdata.h \
    ../libs/comfunc.h \
    ../astro/astro.h \
    ../libs/multidim.h \
    ../mb/mb.h \
    ../libs/ccdmeasurements.h \
    ../libs/redStat.h \
    ../libs/identify.h \
    ../libs/cmnk.h \
    ../libs/observatory.h \
    ../libs/sscat.h \
    ../libs/fBuffer.h \
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
    ../libs/sysCorr.h

unix:LIBS += ./../libs/unix/libcfitsio.a \
            ./../libs/unix/libmb.a \
            ./../libs/unix/libastro.a
win32:LIBS += ./../libs/win32/cfitsio.lib \
              ./../libs/win32/libmb.a \
              ./../libs/win32/libastro.a

