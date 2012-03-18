#-------------------------------------------------
#
# Project created by QtCreator 2012-03-17T12:34:12
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ./../../lab/saTest/saGrade
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../libs/sscat.cpp \
    ../libs/skyarea.cpp \
    ../libs/orbit.cpp \
    ../libs/orbcat.cpp \
    ../libs/observatory.cpp \
    ../libs/observ.cpp \
    ../libs/mpccat.cpp \
    ../libs/dele.cpp \
    ../libs/coord.cpp \
    ../libs/fBuffer.cpp \
    ../libs/comfunc.cpp \
    ../libs/sscatFB.cpp \
    ../libs/fbStream.cpp \
    ../libs/redStat.cpp \
    ../libs/mpcfile.cpp

HEADERS += \
    ../libs/sscat.h \
    ../libs/skyarea.h \
    ../libs/orbit.h \
    ../libs/orbcat.h \
    ../libs/observatory.h \
    ../libs/observ.h \
    ../libs/mpccat.h \
    ../libs/dele.h \
    ../libs/coord.h \
    ../libs/fBuffer.h \
    ../libs/comfunc.h \
    ../libs/sscatFB.h \
    ../libs/fbStream.h \
    ../libs/mb.h \
    ../libs/astro.h \
    ../libs/redStat.h \
    ../libs/mpcfile.h

win32:LIBS += ./../libs/win32/libmb.a \
            ./../libs/win32/libastro.a
unix:LIBS += ./../libs/unix/libmb.a \
            ./../libs/unix/libastro.a
