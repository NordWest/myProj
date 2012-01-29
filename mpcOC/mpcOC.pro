#-------------------------------------------------
#
# Project created by QtCreator 2012-01-23T13:50:21
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = mpcOC

CONFIG += console \
    exceptions \
    warn_off

HEADERS += ../libs/astro.h \
    ../libs/mb.h \
    ../libs/fitsio.h \
    ../libs/fitsdata.h \
    ../libs/comfunc.h \
    ../libs/multidim.h \
    ../libs/ccdmeasurements.h \
    ../libs/redStat.h \
    ../libs/observatory.h \
    ../libs/identify.h \
    ../libs/cmnk.h \
    ../libs/sscat.h \
    ../libs/fBuffer.h \
    ../libs/vectGrid3D.h \
    ../libs/fitstools.h\
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
    ../libs/mpcfile.h \
    ../libs/orbcat.h


TEMPLATE = app


SOURCES += main.cpp \
    ../libs/fitsdata.cpp \
    ../libs/comfunc.cpp \
    ../libs/multidim.cpp \
    ../libs/ccdmeasurements.cpp \
    ../libs/redStat.cpp \
    ../libs/observatory.cpp \
    ../libs/identify.cpp \
    ../libs/cmnk.cpp \
    ../libs/sscat.cpp \
    ../libs/fBuffer.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/fitstools.cpp\
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
    ../libs/mpcfile.cpp \
    ../libs/orbcat.cpp


win32:LIBS += ./../libs/win32/cfitsio.lib \
            ./../libs/win32/libmb.a \
            ./../libs/win32/libastro.a
unix:LIBS += ./../libs/unix/libcfitsio.a \
            ./../libs/unix/libmb.a \
            ./../libs/unix/libastro.a














