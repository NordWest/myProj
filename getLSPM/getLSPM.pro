# -------------------------------------------------
# Project created by QtCreator 2010-05-10T00:03:05
# -------------------------------------------------
QT -= gui
TARGET = getLSPM
CONFIG += console \
    debug \
    warn_off
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ../libs/sscat.cpp \
    ../libs/comfunc.cpp \
    ../libs/astro.cpp \
    ../libs/fBuffer.cpp \
    ../libs/sscatFB.cpp \
    ../libs/fbStream.cpp
HEADERS += ../libs/sscat.h \
    ../libs/comfunc.h \
    ../libs/astro.h \
    ../libs/fBuffer.h \
    ../libs/sscatFB.h \
    ../libs/fbStream.h
