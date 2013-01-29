# -------------------------------------------------
# Project created by QtCreator 2010-05-27T13:59:25
# -------------------------------------------------

#TARGET = r3StatPL
unix:TARGET = ./../../lab/r3StatPL.unix/r3StatPL
win32:TARGET = r3StatPL
#unix:TARGET = ./../../lab/r3StatPL/r3StatPL
#win32:TARGET = ./../../../lab/r3StatPL/r3StatPL
CONFIG += qt exceptions
CONFIG += console warn_off
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ./../libs/redStat.cpp \
#    ./../libs/astro.cpp \
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
    ../libs/alglib/ap.cpp \
    ../libs/alglib/interpolation.cpp \
    ../libs/alglib/specialfunctions.cpp \
    ../libs/alglib/solvers.cpp \
    ../libs/alglib/optimization.cpp \
    ../libs/alglib/linalg.cpp \
    ../libs/alglib/integration.cpp \
    ../libs/alglib/alglibmisc.cpp \
    ../libs/alglib/alglibinternal.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/ialglib.cpp \
    ../libs/mpccat.cpp \
    ../libs/orbcat.cpp \
    ../libs/sysCorr.cpp \
    ../libs/uneven.cpp \
    ../libs/residualsgrid.cpp

HEADERS += ../astro/astro.h \
    ../mb/mb.h \
    ../libs/comfunc.h \
    ../libs/redStat.h \
    ../libs/multidim.h \
    ../libs/observatory.h \
    ../libs/fitsdata.h \
    ../libs/fitsio.h \
    ../libs/identify.h \
    ../libs/cmnk.h  \
    ../libs/longnam.h   \
    ./../libs/ccdmeasurements.h \
    ../libs/fitstools.h \
    ../libs/sscat.h \
    ../libs/fbStream.h \
    ../libs/fBuffer.h \
    ../libs/alglib/ap.h \
    ../libs/alglib/interpolation.h \
    ../libs/alglib/specialfunctions.h \
    ../libs/alglib/solvers.h \
    ../libs/alglib/optimization.h \
    ../libs/alglib/linalg.h \
    ../libs/alglib/integration.h \
    ../libs/alglib/alglibmisc.h \
    ../libs/alglib/alglibinternal.h \
    ../libs/vectGrid3D.h \
    ../libs/ialglib.h \
    ../libs/mpccat.h \
    ../libs/orbcat.h \
    ../libs/sysCorr.h \
    ../libs/uneven.h \
    ../libs/residualsgrid.h

win32:LIBS += ./../libs/win32/cfitsio.lib
unix:LIBS += ./../libs/unix/libcfitsio.a
win32:LIBS += ./../libs/win32/libmb.a
unix:LIBS += ./../libs/unix/libmb.a
win32:LIBS += ./../libs/win32/libastro.a
unix:LIBS += ./../libs/unix/libastro.a
