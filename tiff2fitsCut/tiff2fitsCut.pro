#-------------------------------------------------
#
# Project created by QtCreator 2010-07-02T09:59:44
#
#-------------------------------------------------

QT       -= gui

TARGET = tiff2fitsCut
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

win32:LIBS += ./../libs/cfitsio.lib
unix:LIBS += ./../libs/libcfitsio.a
win32:LIBS += ./../libs/libmb.a
unix:LIBS += ./libmb.a
