# -------------------------------------------------
# Project created by QtCreator 2009-11-07T17:37:18
# -------------------------------------------------
#QT -= gui
TARGET = ./../../lab/testNbody/everBody
CONFIG += console \
    warn_off \
    debug
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
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
    ../libs/sscat.cpp \
    ../libs/sscatFB.cpp \
#    ../libs/astro.cpp \
    ../libs/rada.cpp \
    ../libs/force_ev.cpp
HEADERS += \
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
    ../libs/sscat.h \
    ../libs/sscatFB.h \
    ../libs/astro.h \
    ../libs/mb.h \
    ../libs/rada.h \
    ../libs/ephem_types.h
unix:LIBS+=./../libs/unix/libmb.a
win32:LIBS+=./../libs/win/libmb.a

unix:LIBS+=./../libs/unix/libastro.a
win32:LIBS+=./../libs/win/libastro.a
