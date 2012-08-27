#-------------------------------------------------
#
# Project created by QtCreator 2011-02-16T10:55:24
#
#-------------------------------------------------


TARGET = pics2fits
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += ../libs/fitstools.h \
    ../libs/longnam.h \
    ../libs/fitsio.h


SOURCES += main.cpp
win32:LIBS += ./../libs/cfitsio.lib
unix:LIBS += ./../libs/libcfitsio.a
