win32 {
 TARGET = mb
}
unix {
TARGET = ./../libs/unix/mb
}
TEMPLATE = lib
SOURCES += mb.cpp
HEADERS += mb.h
CONFIG += staticlib warn_off
win32:header.path=./../libs
header.files=mb.h
INSTALLS+=header
unix:header.path=/usr/local/include
unix:target.path +=/usr/local/lib
unix:INSTALLS+=target
