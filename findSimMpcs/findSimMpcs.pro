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
    ../libs/mpcs.cpp \
    ../libs/coord.cpp \
    ../libs/comfunc.cpp \
    ../libs/DynArr.cpp \
    ../libs/astro.cpp

HEADERS += ../libs/mb.h \
    ../libs/mpcs.h \
    ../libs/coord.h \
    ../libs/comfunc.h \
    ../libs/DynArr.h \
    ../libs/astro.h

win32:LIBS += ./../libs/win/libmb.a
unix:LIBS += ./../libs/unix/libmb.a
