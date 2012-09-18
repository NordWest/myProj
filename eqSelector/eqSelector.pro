#-------------------------------------------------
#
# Project created by QtCreator 2012-09-18T14:10:49
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ./../../lab/mpcTools/eqSelector
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../libs/comfunc.cpp \
    ../libs/ringpix.cpp \
    ../libs/redStat.cpp \
    ../libs/observatory.cpp \
    ../libs/observ.cpp \
    ../libs/dele.cpp \
    ../libs/coord.cpp
HEADERS += ../libs/astro.h \
            ../libs/mb.h \
    ../libs/comfunc.h \
    ../libs/ringpix.h \
    ../libs/mpcStats.h \
    ../libs/redStat.h \
    ../libs/observatory.h \
    ../libs/observ.h \
    ../libs/dele.h \
    ../libs/coord.h

win32:LIBS += ./../libs/win32/libmb.a \
            ./../libs/win32/libastro.a
unix:LIBS += ./../libs/unix/libmb.a \
            ./../libs/unix/libastro.a

