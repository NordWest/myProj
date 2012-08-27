# -------------------------------------------------
# Project created by QtCreator 2010-05-21T22:48:30
# -------------------------------------------------
QT -= gui
TARGET = plResStat
CONFIG += console \
warn_off \
debug
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ../libs/redStat.cpp \
    ../libs/astro.cpp \
    ../libs/comfunc.cpp
HEADERS += ../libs/redStat.h \
    ../libs/mb.h \
    ../libs/astro.h \
    ../libs/comfunc.h
win32:LIBS += ./../libs/libmb.a
unix:LIBS += ./libmb.a
