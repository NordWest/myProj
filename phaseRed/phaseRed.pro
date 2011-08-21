# -------------------------------------------------
# Project created by QtCreator 2009-08-25T14:38:23
# -------------------------------------------------
TARGET = phaseRed
CONFIG += qt \
    warn_off \
    console

# CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    ../libs/redStat.cpp \
    ../libs/dele.cpp \
    ../libs/DynArr.cpp \
    ../libs/coord.cpp \
    ../libs/comfunc.cpp \
    ../libs/astro.cpp \
    ../libs/sscat.cpp \
    ../libs/fBuffer.cpp \
    ../libs/multidim.cpp \
    ../libs/ccdmeasurements.cpp
HEADERS += ../libs/fitsio.h \
    ../libs/longnam.h \
    ../libs/redStat.h \
    ../libs/dele.h \
    ../libs/DynArr.h \
    ../libs/coord.h \
    ../libs/comfunc.h \
    ../libs/astro.h \
    ../libs/mb.h \
    ../libs/sscat.h \
    ../libs/fBuffer.h \
    ../libs/multidim.h \
    ../libs/ccdmeasurements.h
win32:LIBS += ./../libs/cfitsio.lib
unix:LIBS += ./../libs/libcfitsio.a
win32:LIBS += ./../libs/win/libmb.a
unix:LIBS += ./../libs/unix/libmb.a
