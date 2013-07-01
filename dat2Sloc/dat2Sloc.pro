#-------------------------------------------------
#
# Project created by QtCreator 2013-07-01T13:53:52
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = dat2Sloc
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ./../libs/comfunc.cpp \
    ../libs/observatory.cpp

HEADERS += ./../libs/comfunc \
    ../libs/observatory.h

LIBS += -lastro -lmb
