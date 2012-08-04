#-------------------------------------------------
#
# Project created by QtCreator 2010-05-11T15:06:37
#
#-------------------------------------------------

QT       -= gui
INCLUDEPATH = ./../libs
TARGET = twdb
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

win32 {
 LIBS +=  ./../libs/cfitsio.lib ./../libs/win32/libmb.a ./../libs/win32/libastro.a
}
unix {
LIBS +=  ./../libs/unix/libcfitsio.a ./../libs/unix/libmb.a ./../libs/unix/libastro.a
}
