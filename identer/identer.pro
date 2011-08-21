TEMPLATE = app
HEADERS += fitsio.h \
 ccdmeasurements.h \
 ./../libs/astro.h \
 ./../libs/cmnk.h \
 ./../libs/identify.h \
 ./../libs/mb.h \
 multidim.h \
 fitsio.h \
 longnam.h \
 ./../libs/fitsdata.h
SOURCES += ../libs/cmnk.cpp \
 ./../libs/identify.cpp \
 ccdmeasurements.cpp \
 main.cpp \
 ./../libs/astro.cpp \
 ./../libs/mb.cpp \
 multidim.cpp \
 ./../libs/fitsdata.cpp
win32 {
 LIBS +=  cfitsio.lib
}
unix:LIBS+=libcfitsio.a
TARGET = identer
CONFIG += qt warn_off console debug
