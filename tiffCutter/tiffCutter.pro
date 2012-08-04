TEMPLATE = app
SOURCES += main.cpp
TARGET = tifCutter
CONFIG += qt \
    warn_off \
    console \
    release

HEADERS += \
    ../libs/longnam.h \
    ../libs/fitsio.h
win32:LIBS += ./../libs/win32/cfitsio.lib
unix:LIBS += ./../libs/unix/libcfitsio.a
win32:LIBS += ./../libs/win32/libmb.a
unix:LIBS += ./../libs/unix/libmb.a
