# -------------------------------------------------
# Project created by QtCreator 2010-05-10T00:03:05
# -------------------------------------------------
QT -= gui
unix:TARGET = ./../bin/lspmFind
win32:TARGET = ./../../bin/lspmFind
CONFIG += console \
    release \
    warn_off
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ../libs/sscat.cpp \
    ../libs/comfunc.cpp \
    ../libs/fBuffer.cpp \
    ../libs/sscatFB.cpp \
    ../libs/fbStream.cpp
HEADERS += ../libs/sscat.h \
    ../libs/comfunc.h \
    ../astro/astro.h \
    ../libs/fBuffer.h \
    ../libs/sscatFB.h \
    ../libs/fbStream.h \
    ../mb/mb.h
unix:LIBS += ./../libs/unix/libmb.a
win32:LIBS += ./../libs/win32/libmb.a
unix:LIBS += ./../libs/unix/libastro.a
win32:LIBS += ./../libs/win32/libastro.a
