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
    ../libs/dele.cpp \
    ../libs/time_a.cpp

HEADERS += ../libs/orbit.h \
    ../libs/orbcat.h \
    ../libs/observatory.h \
    ../libs/observ.h \
    ../libs/mpccat.h \
    ../libs/fBuffer.h \
    ../libs/fbStream.h \
#    ../libs/astro.h \
#    ../libs/mb.h \
    ../libs/comfunc.h \
    ../libs/mpcs.h \
    ../libs/DynArr.h \
    ../libs/coord.h \
    ../libs/dele.h \
    ../libs/time_a.h

unix:LIBS+= ./../libs/unix/cspice.a
#            ./../libs/unix/libastro.a \
#            ./../libs/unix/libmb.a
unix:LIBS += -lmb -lastro
