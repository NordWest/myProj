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
    ../libs/astro.cpp \
    ../libs/sscat.cpp \
    ../libs/comfunc.cpp \
    ../libs/fBuffer.cpp \
    ../libs/ccdmeasurements.cpp \
    ../libs/mpcs.cpp \
    ../libs/coord.cpp \
    ../libs/DynArr.cpp

HEADERS += \
    ../libs/redStat.h \
    ../libs/multidim.h \
    ../libs/mb.h \
    ../libs/astro.h \
    ../libs/sscat.h \
    ../libs/comfunc.h \
    ../libs/fBuffer.h \
    ../libs/ccdmeasurements.h \
    ../libs/mpcs.h \
    ../libs/coord.h \
    ../libs/DynArr.h
win32:LIBS += ./../libs/win/libmb.a
unix:LIBS += ./../libs/unix/libmb.a
