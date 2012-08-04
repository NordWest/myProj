# -------------------------------------------------
# Project created by QtCreator 2010-04-05T11:45:30
# -------------------------------------------------
QT -= gui
TARGET = damPack
CONFIG += console \
    qt \
    warn_off \
    debug
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ../libs/fitsdata.cpp \
    ../libs/redStat.cpp \
    ../libs/multidim.cpp \
    ../libs/identify.cpp \
    ../libs/ccdmeasurements.cpp \
    ../libs/comfunc.cpp \
    ../libs/cmnk.cpp \
    ../libs/sscat.cpp \
    ../libs/fBuffer.cpp \
    ../libs/observatory.cpp \
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
    ../libs/redStat.h \
    ../libs/multidim.h \
    ../libs/mb.h \
    ../libs/identify.h \
    ../libs/ccdmeasurements.h \
    ../libs/astro.h \
    ../libs/comfunc.h \
    ../libs/astro.h \
    ../libs/cmnk.h \
    ../libs/sscat.h \
    ../libs/fBuffer.h \
    ../libs/observatory.h \
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

win32:LIBS += ./../libs/win32/cfitsio.lib \
            ./../libs/win32/libmb.a \
            ./../libs/win32/libastro.a
unix:LIBS += ./../libs/unix/libcfitsio.a \
            ./../libs/unix/libmb.a \
            ./../libs/unix/libastro.a
