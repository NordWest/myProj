TEMPLATE = app
SOURCES += main.cpp
SOURCES += cdsfind.cpp
HEADERS += cdsfind.h
win32:TARGET = ./../../bin/cdsfind
unix:TARGET = ./../bin/cdsfind
QT += network
CONFIG += console warn_off release
