#-------------------------------------------------
#
# Project created by QtCreator 2012-09-13T12:25:33
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ./../../lab/mpcTools/eqStat
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../libs/mpcfile.cpp \
    ../libs/comfunc.cpp \
    ../libs/ringpix.cpp \
    ../libs/redStat.cpp
HEADERS += ../libs/mpcfile.h \
            ../libs/astro.h \
            ../libs/mb.h \
    ../libs/comfunc.h \
    ../libs/ringpix.h \
    ../libs/mpcStats.h \
    ../libs/redStat.h

win32:LIBS += ./../libs/win32/libmb.a \
            ./../libs/win32/libastro.a
unix:LIBS += ./../libs/unix/libmb.a \
            ./../libs/unix/libastro.a


