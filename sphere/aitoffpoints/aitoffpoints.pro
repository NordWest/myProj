#-------------------------------------------------
#
# Project created by QtCreator 2011-06-28T01:22:46
#
#-------------------------------------------------

QT       += core

QT       -= gui

#TARGET = ./../../../lab/mpcTools/aitoffpoints
TARGET = ~/bin/aitoffpoints

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp
LIBS+= -lmgl -lastro -lmb
