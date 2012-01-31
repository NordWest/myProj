#-------------------------------------------------
#
# Project created by QtCreator 2012-01-30T23:40:52
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = mpcSelector
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../libs/mpcfile.cpp \
    ../libs/comfunc.cpp
HEADERS += ../libs/mpcfile.h \
            ../libs/astro.h \
            ../libs/mb.h \
    ../libs/comfunc.h

win32:LIBS += ./../libs/win32/libmb.a \
            ./../libs/win32/libastro.a
unix:LIBS += ./../libs/unix/libmb.a \
            ./../libs/unix/libastro.a


