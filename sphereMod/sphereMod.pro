# -------------------------------------------------
# Project created by QtCreator 2009-08-07T16:44:51
# -------------------------------------------------
TARGET = sphereMod
CONFIG += console \
    warn_off \
    qt
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ../libs/astro.cpp \
    ../libs/mb.cpp
HEADERS += ../libs/astro.h \
    ../libs/mb.h
win32:LIBS += ./../libs/cfitsio.lib
unix:LIBS += ./../libs/libcfitsio.a
