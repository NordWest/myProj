TEMPLATE = app
HEADERS += basewidget.h rulerprocess.h ccdmeasurements.h astro.h
SOURCES += main.cpp \
 basewidget.cpp \
 rulerprocess.cpp \
 ccdmeasurements.cpp \
 astro.cpp
win32 {
 INCLUDEPATH += C:\boost_1_33_1
}
unix {
 INCLUDEPATH += /mnt/winsys/boost_1_33_1
}
TARGET = rulermanager
CONFIG += qt warn_off release
