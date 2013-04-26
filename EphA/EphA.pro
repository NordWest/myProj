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
    ../libs/cspice/SpiceUsr.h \
    ../libs/redStat.h


FORMS    += mainwindow.ui \
    settwindow.ui

win32:LIBS += libmb libastro
#            ./../libs/win32/libastro.a
unix:LIBS += ./../libs/unix/libmb.a \
            ./../libs/unix/libastro.a
unix:LIBS+=./../libs/unix/cspice.a



