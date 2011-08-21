TEMPLATE = app
HEADERS +=./../libs/fitsio.h
HEADERS +=./../libs/longnam.h
SOURCES +=main.cpp
win32:LIBS += cfitsio.lib
unix:LIBS += libcfitsio.a
TARGET = medianplus
CONFIG += console exceptions warn_off release  
