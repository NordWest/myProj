TEMPLATE = app
HEADERS += ccdmeasurements.h ./../libs/fitsio.h ./../libs/longnam.h astro.h
SOURCES += main.cpp ccdmeasurements.cpp astro.cpp
win32 {
 LIBS +=  cfitsio.lib
 INCLUDEPATH +=  C:\boost_1_33_1
}
unix {
 LIBS +=  libcfitsio.a
 INCLUDEPATH +=  /mnt/winsys/boost_1_33_1
}
TARGET = ruler2
CONFIG += qt exceptions warn_off release
