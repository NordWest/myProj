#-------------------------------------------------
#
# Project created by QtCreator 2010-08-17T11:50:29
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = damDistCorr
CONFIG   += console \
    warn_off \
    debug
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../libs/spline3.cpp \
    ../libs/spline2d.cpp \
    ../libs/ap.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/ialglib.cpp\
    ./../libs/multidim.cpp
HEADERS += ../libs/mb.h \
    ../libs/spline3.h \
    ../libs/spline2d.h \
    ../libs/ap.h \
    ../libs/vectGrid3D.h \
    ../libs/ialglib.h\
    ../libs/multidim.h
win32:LIBS += ./../libs/libmb.a \
    ../libs/ablas.dll
unix:LIBS += ./libmb.a
