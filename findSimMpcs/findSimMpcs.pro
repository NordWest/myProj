#-------------------------------------------------
#
# Project created by QtCreator 2011-04-19T18:48:46
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = findSimMpcs
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../libs/mpcfile.cpp \
    ../libs/comfunc.cpp

HEADERS += ../mb/mb.h \
    ../libs/mpcfile.h \
    ../libs/comfunc.h \
    ../libs/astro.h

win32:LIBS += ./../libs/win32/libmb.a \
            ./../libs/win32/libastro.a
unix:LIBS += ./../libs/unix/libmb.a \
                ./../libs/unix/libastro.a
