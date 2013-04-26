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

unix:header.path=/usr/local/include
unix:header.files=astro.h
unix:target.path +=/usr/local/lib
win32:header.path="c:\\Qt\\4.8.3\\include\\"
win32:header.files=astro.h
INSTALLS+=header
unix:INSTALLS+=target
