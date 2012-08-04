#-------------------------------------------------
#
# Project created by QtCreator 2010-08-10T11:52:43
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = epsReports
CONFIG   += console \
    warn_off
CONFIG   -= app_bundle

TEMPLATE = app

LIBS+=-lmgl

win32:LIBS += ./../libs/libmb.a
unix:LIBS += ./libmb.a

SOURCES += main.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/spline3.cpp \
    ../libs/spline2d.cpp \
    ../libs/ialglib.cpp \
    ../libs/ap.cpp \
    ../libs/multidim.cpp
HEADERS += ../libs/vectGrid3D.h \
    ../libs/spline3.h \
    ../libs/spline2d.h \
    ../libs/ialglib.h \
    ../libs/ap.h \
    ../libs/apvt.h \
    ../libs/multidim.h
INCLUDEPATH += C:\Qt\GnuWin32\include

win32:LIBS += ../libs/ablas.dll
