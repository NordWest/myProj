#-------------------------------------------------
#
# Project created by QtCreator 2011-03-30T13:35:41
#
#-------------------------------------------------

QT       -= gui

win32:TARGET = ./../../libs/win32/astro
unix:TARGET = ./../libs/unix/astro
TEMPLATE = lib
CONFIG += staticlib warn_off

SOURCES += astro.cpp \
    ../libs/comfunc.cpp

HEADERS += astro.h \
    ../libs/comfunc.h \
    ../mb/mb.h

win32:LIBS+=./../libs/win32/libmb.a
unix:LIBS+=./../libs/unix/libmb.a
