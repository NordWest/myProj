#-------------------------------------------------
#
# Project created by QtCreator 2009-09-02T13:52:24
#
#-------------------------------------------------

QT       -= gui

TARGET = ./../../lab/cleanApp/cleanApp
CONFIG   += qt \
    warn_off \
    console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../libs/uneven.cpp \
    ../libs/alglib/ap.cpp

HEADERS +=  ../libs/mb.h \
    ../libs/uneven.h \
    ../libs/alglib/ap.h
win32:LIBS += ./../libs/win32/libmb.a
unix:LIBS += ./../libs/unix/libmb.a




