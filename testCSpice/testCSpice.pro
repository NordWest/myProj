#-------------------------------------------------
#
# Project created by QtCreator 2012-08-23T12:21:51
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ./../../lab/testCSpice/testCSpice
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../libs/orbit.cpp \
    ../libs/orbcat.cpp \
    ../libs/observatory.cpp \
    ../libs/observ.cpp \
    ../libs/mpccat.cpp \
    ../libs/comfunc.cpp \
    ../libs/mpcs.cpp \
    ../libs/DynArr.cpp \
    ../libs/coord.cpp \
    ../libs/fBuffer.cpp \
    ../libs/fbStream.cpp \
    ../libs/dele.cpp

HEADERS += ../libs/orbit.h \
    ../libs/orbcat.h \
    ../libs/observatory.h \
    ../libs/observ.h \
    ../libs/mpccat.h \
    ../libs/fBuffer.h \
    ../libs/fbStream.h \
    ../libs/cspice/zzerror.h \
    ../libs/cspice/zzalloc.h \
    ../libs/cspice/SpiceZst.h \
    ../libs/cspice/SpiceZpr.h \
    ../libs/cspice/SpiceZpl.h \
    ../libs/cspice/SpiceZmc.h \
    ../libs/cspice/SpiceZim.h \
    ../libs/cspice/SpiceZfc.h \
    ../libs/cspice/SpiceZdf.h \
    ../libs/cspice/SpiceZad.h \
    ../libs/cspice/SpiceUsr.h \
    ../libs/cspice/SpiceSPK.h \
    ../libs/cspice/SpicePln.h \
    ../libs/cspice/SpiceGF.h \
    ../libs/cspice/SpiceEll.h \
    ../libs/cspice/SpiceEK.h \
    ../libs/cspice/SpiceCK.h \
    ../libs/cspice/SpiceCel.h \
    ../libs/cspice/signal1.h \
    ../libs/cspice/rawio.h \
    ../libs/cspice/lio.h \
    ../libs/cspice/fp.h \
    ../libs/cspice/fmt.h \
    ../libs/cspice/fio.h \
    ../libs/cspice/f2cMang.h \
    ../libs/cspice/f2c.h \
    ../libs/astro.h \
    ../libs/mb.h \
    ../libs/comfunc.h \
    ../libs/mpcs.h \
    ../libs/DynArr.h \
    ../libs/coord.h \
    ../libs/dele.h

unix:LIBS+= ./../libs/unix/cspice.a \
            ./../libs/unix/libastro.a \
            ./../libs/unix/libmb.a
