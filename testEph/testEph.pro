#-------------------------------------------------
#
# Project created by QtCreator 2012-10-05T12:33:02
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ./../../lab/testNbody/testEph
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../libs/dele.cpp \
    ../libs/coord.cpp \
    ../libs/comfunc.cpp

HEADERS += ../libs/dele.h \
    ../libs/ephem_types.h \
    ../libs/astro.h \
    ../libs/coord.h \
    ../libs/mb.h    \
    ../libs/comfunc.h

unix:LIBS+=./../libs/unix/libmb.a
win32:LIBS+=./../libs/win32/libmb.a
unix:LIBS+=./../libs/unix/libastro.a
win32:LIBS+=./../libs/win32/libastro.a
