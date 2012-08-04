#-------------------------------------------------
#
# Project created by QtCreator 2011-02-17T13:32:48
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = deleCompile
CONFIG += qt \
    console \
    warn_off \
    debug

TEMPLATE = app


SOURCES += main.cpp \
    ../libs/dele.cpp \
    ../libs/DynArr.cpp \
    ../libs/coord.cpp \
    ../libs/comfunc.cpp \
    ../libs/ephem_util.cpp

HEADERS += \
    ../libs/dele.h \
    ../libs/ephem_types.h \
    ../libs/DynArr.h \
    ../libs/coord.h \
    ../libs/comfunc.h \
    ../libs/mb.h \
    ../libs/ephem_util.h
unix:LIBS+=./../libs/unix/libmb.a
win32:LIBS+=./../libs/win/libmb.a
