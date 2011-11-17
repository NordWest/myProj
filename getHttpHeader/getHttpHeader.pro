# -------------------------------------------------
# Project created by QtCreator 2010-03-30T12:56:39
# -------------------------------------------------
QT -= gui
TARGET = getHttpHeader
#unix:TARGET = ./../bin/getHttpHeader
CONFIG += console \
    exceptions \
    warn_off
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ../libs/httpheader.cpp
HEADERS += ../libs/httpheader.h
QT += network
