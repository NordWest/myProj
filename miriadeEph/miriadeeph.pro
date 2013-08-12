TEMPLATE = app
SOURCES += main.cpp \
    ../libs/comfunc.cpp \
        miriadeeph.cpp
HEADERS += miriadeeph.h \
    ../libs/comfunc.h \
    ../mb/mb.h
HEADERS += ./../libs/astro.h

#win32:TARGET = ./../../bin/miriadeEph
#unix:TARGET = ./../bin/miriadeEph
TARGET = miriadeEph
QT += network
CONFIG += console exceptions warn_off release
win32:LIBS += libmb libastro
win32:LIBS += ./../libs/win32/cfitsio.lib
unix:LIBS += ./../libs/unix/libcfitsio.a
unix:LIBS += ./../libs/unix/libmb.a \
            ./../libs/unix/libastro.a
win32:LIBS += ./../libs/win32/libmb.a \
            ./../libs/win32/libastro.a


