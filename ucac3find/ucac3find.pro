# -------------------------------------------------
# Project created by QtCreator 2009-10-14T23:50:26
# -------------------------------------------------
QT -= gui
//INCLUDEPATH += ./../libs/
TARGET = ucac3find
CONFIG += console warn_off
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ../libs/astro.cpp \
    ../libs/comfunc.cpp
HEADERS += ../libs/astro.h \
    ../libs/comfunc.h \
    ../libs/fitsio.h \
    ../libs/mb.h
win32:LIBS += ./../libs/win32/cfitsio.lib
unix:LIBS += ./../libs/unix/libcfitsio.a
win32:LIBS += ./../libs/win32/libmb.a
unix:LIBS += ./../libs/unix/libmb.a
