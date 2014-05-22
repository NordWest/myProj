#-------------------------------------------------
#
# Project created by QtCreator 2014-05-22T11:37:34
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = mpccatDB
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../../libs/mpccat.cpp

LIBS += -lmb -lastro

HEADERS += \
    ../../libs/mpccat.h
