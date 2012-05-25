# -------------------------------------------------
# Project created by QtCreator 2009-08-07T16:44:51
# -------------------------------------------------
TARGET = sphereMod
CONFIG += console \
    warn_off \
    qt
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ../libs/ringpix.cpp \
    ../libs/comfunc.cpp
#    ../libs/astro.cpp \
#    ../libs/mb.cpp
HEADERS += ../libs/astro.h \
    ../libs/mb.h \
    ../libs/ringpix.h \
    ../libs/comfunc.h
win32:LIBS += ./../libs/win32/cfitsio.lib
unix:LIBS += ./../libs/unix/libcfitsio.a

unix:LIBS += ./../libs/unix/libastro.a

unix:LIBS += ./../libs/unix/libmb.a
