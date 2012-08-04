# -------------------------------------------------
# Project created by QtCreator 2009-08-13T15:35:46
# -------------------------------------------------
# QT -= gui
TARGET = hipPhase

# CONFIG += console
CONFIG += qt \
    warn_off \
    console \
    release

# CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ../libs/astro.cpp \
    ../libs/redStat.cpp \
    ../libs/comfunc.cpp
HEADERS += ../libs/astro.h \
    ../libs/fitsio.h \
    ../libs/longnam.h \
    ../libs/redStat.h \
    ../libs/comfunc.h
win32:LIBS += ./../libs/cfitsio.lib
unix:LIBS += ./../libs/libcfitsio.a
