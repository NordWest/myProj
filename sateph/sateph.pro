TEMPLATE = app
win32 {
 INCLUDEPATH += C:\boost_1_33_1
}
unix {
 INCLUDEPATH += /mnt/winsys/boost_1_33_1
}
SOURCES += main.cpp
SOURCES += astro.cpp
SOURCES += sateph.cpp
HEADERS += astro.h
HEADERS += sateph.h
TARGET = sateph
QT += network
CONFIG += console exceptions warn_off release
