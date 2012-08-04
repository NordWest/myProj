#-------------------------------------------------
#
# Project created by QtCreator 2011-05-28T15:27:00
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = qovar
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp
HEADERS +=  ../libs/mb.h

win32:LIBS += ./../libs/win32/libmb.a
unix:LIBS += ./../libs/unix/libmb.a
