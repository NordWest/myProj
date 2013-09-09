#-------------------------------------------------
#
# Project created by QtCreator 2013-09-09T15:07:48
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = testCUDA
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

LIBS += -lcudart
INCLUDEPATH += /usr/local/cuda-5.5/include

