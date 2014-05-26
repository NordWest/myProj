#-------------------------------------------------
#
# Project created by QtCreator 2012-09-29T12:13:39
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ./../../lab/sphereMod/sphInter
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
#    ../libs/ringpix.cpp \
    ../../libs/comfunc.cpp

HEADERS += ../libs/comfunc.h \
    ../libs/healpix/chealpix.h

#win32:LIBS += ./../libs/win32/cfitsio.lib
#unix:LIBS += ./../libs/unix/libcfitsio.a

unix:LIBS += ./../libs/unix/libchealpix.a
unix:LIBS += -lmb -lastro
