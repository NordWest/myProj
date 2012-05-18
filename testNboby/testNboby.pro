# -------------------------------------------------
# Project created by QtCreator 2010-02-21T18:45:28
# -------------------------------------------------
QT -= gui
TARGET = ./../../lab/testNbody/testNboby
CONFIG += qt \
    console \
    warn_off \
    debug
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ../libs/orbit.cpp \
    ../libs/orbcat.cpp \
    ../libs/observatory.cpp \
    ../libs/observ.cpp \
    ../libs/mpccat.cpp \
    ../libs/comfunc.cpp \
    ../libs/skyarea.cpp \
    ../libs/fBuffer.cpp \
    ../libs/fbStream.cpp \
    ../libs/DynArr.cpp \
    ../libs/coord.cpp \
    ../libs/sscat.cpp \
    ../libs/sscatFB.cpp \
#    ../libs/astro.cpp \
    ../libs/rec.cpp \
    ../libs/commetorbcat.cpp \
    ../libs/force_ev.cpp \
    ../libs/dele.cpp \
    ../libs/rada.cpp \
    ../libs/redStat.cpp \
    ../libs/mpcs.cpp

HEADERS += ../libs/orbit.h \
    ../libs/orbcat.h \
    ../libs/observatory.h \
    ../libs/observ.h \
    ../libs/mpccat.h \
    ../libs/comfunc.h \
    ../libs/skyarea.h \
    ../libs/fBuffer.h \
    ../libs/fbStream.h \
    ../libs/DynArr.h \
    ../libs/coord.h \
    ../libs/sscat.h \
    ../libs/sscatFB.h \
    ../libs/astro.h \
    ../libs/rec.h \
    ../libs/commetorbcat.h \
    ../libs/mb.h \
    ../libs/ephem_types.h \
    ../libs/dele.h \
    ../libs/ephem_types.h \
    ../libs/rada.h \
    ../libs/redStat.h \
    ../libs/mpcs.h
unix:LIBS+=./../libs/unix/libmb.a
win32:LIBS+=./../libs/win32/libmb.a
unix:LIBS+=./../libs/unix/libastro.a
win32:LIBS+=./../libs/win32/libastro.a
