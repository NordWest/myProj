TEMPLATE = app
SOURCES += main.cpp \
    ../libs/comfunc.cpp \
        ephemcc.cpp
HEADERS += ephemcc.h \
    ../libs/comfunc.h \
    ../mb/mb.h
HEADERS += ./../astro/astro.h

#win32:TARGET = ./../../bin/ephemcc
#unix:TARGET = ./../bin/ephemcc
TARGET = ephemcc
QT += network
CONFIG += console exceptions warn_off release
win32:LIBS += ./../libs/win32/cfitsio.lib
unix:LIBS += ./../libs/unix/libcfitsio.a
unix:LIBS += ./../libs/unix/libmb.a \
            ./../libs/unix/libastro.a
win32:LIBS += ./../libs/win32/libmb.a \
            ./../libs/win32/libastro.a


