#-------------------------------------------------
#
# Project created by QtCreator 2010-09-02T14:59:34
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = testPolynom
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app

LIBS+=./libmb.a

SOURCES += main.cpp

HEADERS += \
    mb.h
