#-------------------------------------------------
#
# Project created by QtCreator 2012-03-17T12:34:12
#
#-------------------------------------------------

QT       += core

QT       -= gui

#unix:TARGET = ~/bin/saGrade
unix:TARGET = ./../../lab/EphA/saGrade
win32:TARGET = ./../../../lab/EphA/saGrade
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ../libs/sscat.cpp \
    ../libs/skyarealf.cpp \
    ../libs/orbit.cpp \
    ../libs/orbcat.cpp \
    ../libs/observatory.cpp \
    ../libs/observ.cpp \
    ../libs/mpccat.cpp \
    ../libs/coord.cpp \
    ../libs/fBuffer.cpp \
    ../libs/comfunc.cpp \
    ../libs/sscatFB.cpp \
    ../libs/fbStream.cpp \
    ../libs/mpcfile.cpp \
    ../libs/listfile.cpp \
    ../libs/mpcs.cpp \
    ../libs/DynArr.cpp \
    ../libs/time_a.cpp

HEADERS += ../libs/sscat.h \
    ../libs/skyarealf.h \
    ../libs/orbit.h \
    ../libs/orbcat.h \
    ../libs/observatory.h \
    ../libs/observ.h \
    ../libs/mpccat.h \
    ../libs/coord.h \
    ../libs/fBuffer.h \
    ../libs/comfunc.h \
    ../libs/sscatFB.h \
    ../libs/fbStream.h \
    ../libs/mpcfile.h \
    ../libs/listfile.h \
    ../libs/mpcs.h \
    ../libs/DynArr.h \
    ../libs/time_a.h

win32:LIBS += libmb libastro
unix:LIBS += -lmb -lastro -lephem_read -ldele





