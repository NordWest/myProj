#-------------------------------------------------
#
# Project created by QtCreator 2013-02-04T13:49:37
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ~/bin/recOC
CONFIG   += console warn_off
CONFIG   -= app_bundle
QT += xml

TEMPLATE = app


SOURCES += main.cpp \
    ../libs/orbit.cpp \
    ../libs/orbcat.cpp \
    ../libs/observatory.cpp \
    ../libs/observ.cpp \
    ../libs/mpccat.cpp \
    ../libs/comfunc.cpp \
    ../libs/fBuffer.cpp \
    ../libs/fbStream.cpp \
    ../libs/DynArr.cpp \
    ../libs/coord.cpp \
    ../libs/sscat.cpp \
    ../libs/sscatFB.cpp \
#    ../libs/astro.cpp \
    ../libs/rec.cpp \
    ../libs/commetorbcat.cpp \
    ../libs/dele.cpp \
#    dele.cpp \
    ../libs/redStat.cpp \
    ../libs/mpcs.cpp \
    ../libs/ephem_util.cpp \
    ../libs/calc_epm.cpp \
    ../libs/time_a.cpp

HEADERS += ../libs/orbit.h \
    ../libs/orbcat.h \
    ../libs/observatory.h \
    ../libs/observ.h \
    ../libs/mpccat.h \
    ../libs/comfunc.h \
    ../libs/fBuffer.h \
    ../libs/fbStream.h \
    ../libs/DynArr.h \
    ../libs/coord.h \
    ../libs/sscat.h \
    ../libs/sscatFB.h \
    ../libs/astro.h \
    ../libs/rec.h \
    ../libs/commetorbcat.h \
#    ../libs/mb.h \
    ../libs/ephem_types.h \
    ../libs/dele.h \
#    dele.h \
    ../libs/ephem_types.h \
    ../libs/redStat.h \
    ../libs/mpcs.h \
    ../libs/ephem_util.h \
    ../libs/calc_epm.h \
    ../libs/time_a.h

#unix:LIBS+=./../libs/unix/libmb.a
win32:LIBS+=./../libs/win32/libmb.a
#unix:LIBS+=./../libs/unix/libastro.a
win32:LIBS+=./../libs/win32/libastro.a
unix:LIBS+=./../libs/unix/cspice.a
unix:LIBS += -lmb -lastro
