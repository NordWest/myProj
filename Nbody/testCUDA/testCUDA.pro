#-------------------------------------------------
#
# Project created by QtCreator 2013-09-09T15:07:48
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = testCUDA
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

LIBS += -libcudart
INCLUDEPATH += /usr/local/cuda-6.0/include
INCLUDEPATH += /usr/local/cuda-6.0/samples/common/inc
INCLUDEPATH += /usr/local/cuda-6.0/lib
