#-------------------------------------------------
#
# Project created by QtCreator 2012-06-05T14:31:00
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ./../../lab/sphereMod/sphereCounter
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../libs/ringpix.cpp \
    ../libs/comfunc.cpp \
    vsfFunc.cpp

HEADERS += ../libs/astro.h \
    ../libs/mb.h \
    ../libs/ringpix.h \
    ../libs/comfunc.h \
    vsfFunc.h

win32:LIBS += ./../libs/win32/cfitsio.lib
unix:LIBS += ./../libs/unix/libcfitsio.a
unix:LIBS += ./../libs/unix/libastro.a
unix:LIBS += ./../libs/unix/libmb.a
