# -------------------------------------------------
# Project created by QtCreator 2010-06-20T17:26:58
# -------------------------------------------------
QT -= gui
TARGET = uTimeCorr
CONFIG += console \
    warn_off \
    release
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ../libs/hronobasefile.cpp \
    ../libs/astro.cpp \
    ../libs/comfunc.cpp
HEADERS += ../libs/hronobasefile.h \
    ../libs/astro.h \
    ../libs/comfunc.h

win32:LIBS += ./../libs/cfitsio.lib  \
    ../libs/ablas.dll
unix:LIBS += ./../libs/unix/libcfitsio.a
win32:LIBS += ./../libs/win32/libmb.a
unix:LIBS += ./../libs/unix/libmb.a
