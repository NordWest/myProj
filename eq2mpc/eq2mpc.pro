#-------------------------------------------------
#
# Project created by QtCreator 2011-05-14T14:32:48
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = eq2mpc
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../libs/redStat.cpp \
    ../libs/multidim.cpp \
    ../libs/sscat.cpp \
    ../libs/comfunc.cpp \
    ../libs/mpccat.cpp \
    ../libs/fBuffer.cpp \
    ../libs/fbStream.cpp \
    ../libs/orbcat.cpp

HEADERS += \
    ../libs/redStat.h \
    ../libs/multidim.h \
    ../libs/mb.h \
    ../libs/astro.h \
    ../libs/sscat.h \
    ../libs/comfunc.h \
    ../libs/mpccat.h \
    ../libs/fBuffer.h \
    ../libs/fbStream.h \
    ../libs/orbcat.h

win32:LIBS += ./../libs/win32/libmb.a \
            ./../libs/win32/libastro.a
unix:LIBS += ./../libs/unix/libmb.a \
            ./../libs/unix/libastro.a






