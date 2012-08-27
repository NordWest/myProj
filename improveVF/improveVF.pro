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
    ../libs/vectGrid3D.cpp\
    ./../libs/multidim.cpp \
    ../libs/comfunc.cpp \
    ../libs/alglib/ap.cpp \
    ../libs/alglib/interpolation.cpp \
    ../libs/alglib/specialfunctions.cpp \
    ../libs/alglib/solvers.cpp \
    ../libs/alglib/optimization.cpp \
    ../libs/alglib/linalg.cpp \
    ../libs/alglib/integration.cpp \
    ../libs/alglib/alglibmisc.cpp \
    ../libs/alglib/alglibinternal.cpp \
    ../libs/sysCorr.cpp \
    ../libs/astro.cpp
HEADERS += ../libs/mb.h \
    ../libs/vectGrid3D.h\
    ../libs/multidim.h \
    ../libs/comfunc.h \
    ../libs/alglib/ap.h \
    ../libs/alglib/interpolation.h \
    ../libs/alglib/specialfunctions.h \
    ../libs/alglib/solvers.h \
    ../libs/alglib/optimization.h \
    ../libs/alglib/linalg.h \
    ../libs/alglib/integration.h \
    ../libs/alglib/alglibmisc.h \
    ../libs/alglib/alglibinternal.h \
    ../libs/sysCorr.h \
    ../libs/astro.h
win32:LIBS += ./../libs/win/libmb.a
#    ../libs/ablas.dll
unix:LIBS += ./../libs/unix/libmb.a
#INCLUDEPATH += ../libs/alglib









