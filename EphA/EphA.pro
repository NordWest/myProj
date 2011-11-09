#-------------------------------------------------
#
# Project created by QtCreator 2010-08-20T10:09:37
#
#-------------------------------------------------

QT       += core gui

TARGET = EphA
TEMPLATE = app

CONFIG += console \
        warn_off \
        debug


SOURCES += main.cpp\
        mainwindow.cpp \
    ephthread.cpp \
    ../libs/sscatFB.cpp \
    ../libs/sscat.cpp \
    ../libs/skyarea.cpp \
    ../libs/rec.cpp \
    ../libs/orbit.cpp \
    ../libs/orbcat.cpp \
    ../libs/observatory.cpp \
    ../libs/observ.cpp \
    ../libs/mpccat.cpp \
    ../libs/fBuffer.cpp \
    ../libs/fbStream.cpp \
    ../libs/DynArr.cpp \
    ../libs/dele.cpp \
    ../libs/coord.cpp \
    ../libs/commetorbcat.cpp \
    ../libs/comfunc.cpp \
#    ../libs/astro.cpp
    settwindow.cpp

HEADERS  += mainwindow.h \
    ephthread.h \
    ../libs/sscatFB.h \
    ../libs/sscat.h \
    ../libs/skyarea.h \
    ../libs/rec.h \
    ../libs/orbit.h \
    ../libs/orbcat.h \
    ../libs/observatory.h \
    ../libs/observ.h \
    ../libs/mpccat.h \
    ../libs/fBuffer.h \
    ../libs/fbStream.h \
    ../libs/DynArr.h \
    ../libs/dele.h \
    ../libs/coord.h \
    ../libs/commetorbcat.h \
    ../libs/comfunc.h \
    ../libs/astro.h \
    ../libs/mb.h \
    settwindow.h

FORMS    += mainwindow.ui \
    settwindow.ui

win32:LIBS += ./../libs/win32/libmb.a \
            ./../libs/win32/libastro.a
unix:LIBS += ./../libs/unix/libmb.a \
            ./../libs/unix/libastro.a



