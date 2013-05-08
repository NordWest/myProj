#-------------------------------------------------
#
# Project created by QtCreator 2010-08-20T10:09:37
#
#-------------------------------------------------

QT       += core gui

TARGET = ./../../lab/saTest/EphA
TEMPLATE = app

CONFIG += console \
        warn_off \
        debug

QT           += network

SOURCES += main.cpp\
        mainwindow.cpp \
    ephthread.cpp \
    ../libs/sscatFB.cpp \
    ../libs/sscat.cpp \
    ../libs/skyarealf.cpp \
    ../libs/rec.cpp \
    ../libs/orbit.cpp \
    ../libs/orbcat.cpp \
    ../libs/observatory.cpp \
    ../libs/observ.cpp \
    ../libs/mpccat.cpp \
    ../libs/fBuffer.cpp \
    ../libs/fbStream.cpp \
    ../libs/DynArr.cpp \
    ../libs/dele.cpp \
    ../libs/coord.cpp \
    ../libs/commetorbcat.cpp \
    ../libs/comfunc.cpp \
#    ../libs/astro.cpp
    settwindow.cpp \
    ../libs/listfile.cpp \
    ../libs/mpcs.cpp \
    ../libs/time_a.cpp \
    ../libs/redStat.cpp


HEADERS  += mainwindow.h \
    ephthread.h \
    ../libs/sscatFB.h \
    ../libs/sscat.h \
    ../libs/skyarealf.h \
    ../libs/rec.h \
    ../libs/orbit.h \
    ../libs/orbcat.h \
    ../libs/observatory.h \
    ../libs/observ.h \
    ../libs/mpccat.h \
    ../libs/fBuffer.h \
    ../libs/fbStream.h \
    ../libs/DynArr.h \
    ../libs/dele.h \
    ../libs/coord.h \
    ../libs/commetorbcat.h \
    ../libs/comfunc.h \
#    ../libs/astro.h \
#    ../libs/mb.h \
    settwindow.h \
    ../libs/listfile.h \
    ../libs/mpcs.h \
    ../libs/time_a.h \
    ../libs/redStat.h \
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
    ../libs/cspice/f2c.h


FORMS    += mainwindow.ui \
    settwindow.ui

win32:LIBS += libmb libastro
#            ./../libs/win32/libastro.a
win32:LIBS += ./../libs/win32/cspice.lib
unix:LIBS += ./../libs/unix/libmb.a \
            ./../libs/unix/libastro.a
unix:LIBS+=./../libs/unix/cspice.a



