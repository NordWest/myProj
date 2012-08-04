#-------------------------------------------------
#
# Project created by QtCreator 2010-05-04T14:00:46
#
#-------------------------------------------------

QT       -= gui
INCLUDEPATH = ./../libs
TARGET = matchstars
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp
win32 {
 LIBS +=  ./../libs/cfitsio.lib ./../libs/win/libmb.a ./../libs/libastro.a
}
unix {
LIBS +=  ./../libs/libcfitsio.a ./../libs/libmb.a ./../libs/libastro.a
}
