TEMPLATE = app
HEADERS += ccdmeasurements.h \
    ./../libs/astro.h \
    ./../libs/mb.h \
    ../libs/multidim.h \
    ../libs/comfunc.h \
    ../libs/StdAfx.h \
    ../libs/fitsio.h
SOURCES += main.cpp \
    ccdmeasurements.cpp \
    ./../libs/astro.cpp \
    ../libs/multidim.cpp \
    ../libs/comfunc.cpp
win32:LIBS += cfitsio.lib
unix:LIBS += libcfitsio.a
win32:LIBS += ./../libs/libmb.a
unix:LIBS += ./libmb.a

TARGET = iPlot
CONFIG += qt \
    warn_off \
    debug \
    console
