# -------------------------------------------------
# Project created by QtCreator 2010-11-02T01:38:41
# -------------------------------------------------
QT -= gui
TARGET = ./../../lab/sphereMod/vsf
CONFIG += console warn_off
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp
HEADERS += ../libs/astro.h \
    ../libs/mb.h
unix:LIBS += ./../libs/unix/libastro.a
unix:LIBS += ./../libs/unix/libmb.a

