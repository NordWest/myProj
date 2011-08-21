# -------------------------------------------------
# Project created by QtCreator 2009-11-07T19:20:44
# -------------------------------------------------
QT -= gui
TARGET = pvdFind
CONFIG += console \
    qt \
    warn_off \
    debug
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ../libs/pvdK.cpp \
    ../libs/mpcs.cpp \
    ../libs/DynArr.cpp \
    ../libs/coord.cpp \
    ../libs/comfunc.cpp \
    ../libs/ast.cpp \
    ../libs/observatory.cpp \
    ../libs/observ.cpp \
    ../libs/dele.cpp \
    ../libs/mb.cpp
HEADERS += ../libs/pvdK.h \
    ../libs/mpcs.h \
    ../libs/DynArr.h \
    ../libs/coord.h \
    ../libs/comfunc.h \
    ../libs/ast.h \
    ../libs/observatory.h \
    ../libs/observ.h \
    ../libs/dele.h \
    ../libs/mb.h
INCLUDE += "./../libs/mb.hpp"
