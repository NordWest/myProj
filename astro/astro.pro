#-------------------------------------------------
#
# Project created by QtCreator 2011-03-30T13:35:41
#
#-------------------------------------------------

QT       -= gui

#TARGET = ./../../libs/win32/astro
TARGET = astro
TEMPLATE = lib
CONFIG += staticlib warn_off

SOURCES += astro.cpp
HEADERS += astro.h

header.path=/usr/local/include
header.files=astro.h
target.path +=/usr/local/lib
INSTALLS+=header
INSTALLS+=target
