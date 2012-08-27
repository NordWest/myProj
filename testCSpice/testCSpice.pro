#-------------------------------------------------
#
# Project created by QtCreator 2012-08-23T12:21:51
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = testCSpice
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

HEADERS += \
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

unix:LIBS+=./../libs/unix/cspice.a
