#-------------------------------------------------
#
# Project created by QtCreator 2012-09-20T13:14:18
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = ./../../lab/mpcTools/mpcDebias
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../libs/mpcfile.cpp \
    ../libs/comfunc.cpp \
    ../libs/ringpix.cpp
HEADERS += ../libs/mpcfile.h \
            ../libs/astro.h \
            ../libs/mb.h \
    ../libs/comfunc.h \
    ../libs/ringpix.h \
    ../libs/mpcStats.h

win32:LIBS += ./../libs/win32/libmb.a \
            ./../libs/win32/libastro.a
unix:LIBS += ./../libs/unix/libmb.a \
            ./../libs/unix/libastro.a

