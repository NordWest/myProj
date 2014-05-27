#-------------------------------------------------
#
# Project created by QtCreator 2014-05-22T11:37:34
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = ./../../../lab/mpcTools/mpccatDB
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../../libs/mpccat.cpp \
    ../../libs/orbit.cpp \
    ../../libs/orbcat.cpp \
    ../../libs/fbStream.cpp \
    ../../libs/fBuffer.cpp \
    ../../libs/comfunc.cpp

LIBS += -lmb -lastro

HEADERS += \
    ../../libs/mpccat.h \
    ../../libs/orbit.h \
    ../../libs/orbcat.h \
    ../../libs/fbStream.h \
    ../../libs/fBuffer.h \
    ../../libs/comfunc.h
