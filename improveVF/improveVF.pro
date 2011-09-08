#-------------------------------------------------
#
# Project created by QtCreator 2010-11-24T21:31:38
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ./../../lab/improveVF/improveVF
CONFIG   += console \
    warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../libs/spline3.cpp \
    ../libs/spline2d.cpp \
    ../libs/ap.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/ialglib.cpp\
    ./../libs/multidim.cpp \
    ../libs/comfunc.cpp
HEADERS += ../libs/mb.h \
    ../libs/spline3.h \
    ../libs/spline2d.h \
    ../libs/ap.h \
    ../libs/vectGrid3D.h \
    ../libs/ialglib.h\
    ../libs/multidim.h \
    ../libs/comfunc.h
win32:LIBS += ./../libs/win/libmb.a \
    ../libs/ablas.dll
unix:LIBS += ./../libs/unix/libmb.a
