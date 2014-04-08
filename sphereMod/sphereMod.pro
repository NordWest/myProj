# -------------------------------------------------
# Project created by QtCreator 2009-08-07T16:44:51
# -------------------------------------------------
TARGET = ./../../lab/sphereMod/sphereMod
CONFIG += console \
    warn_off \
    qt
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ../libs/ringpix.cpp \
    ../libs/comfunc.cpp
HEADERS += ../libs/ringpix.h \
    ../libs/comfunc.h
win32:LIBS += ./../libs/win32/cfitsio.lib
unix:LIBS += -lastro -lmb
