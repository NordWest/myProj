# -------------------------------------------------
# Project created by QtCreator 2009-08-04T11:04:46
# -------------------------------------------------
# QT -= gui
TARGET = binFilter
CONFIG += console \
    warn_off \
    qt
CONFIG -= app_bundle
TEMPLATE = app
HEADERS += ./../libs/fitsio.h \
    ./../libs/longnam.h \
    ./../libs/fitsdata.h \
    ./../libs/ccdmeasurements.h \
    ./../libs/multidim.h \
    ./../libs/astro.h \
    ./../libs/mb.h \
    ../libs/imgprofile.h \
    ../libs/multidim.h
SOURCES += ./../libs/ccdmeasurements.cpp \
    main.cpp \
    ./../libs/fitsdata.cpp \
    ./../libs/multidim.cpp \
    ./../libs/astro.cpp \
    ./../libs/mb.cpp \
    ../libs/imgprofile.cpp \
    ../libs/multidim.cpp
win32:LIBS += ./../libs/cfitsio.lib
unix:LIBS += ./../libs/libcfitsio.a
