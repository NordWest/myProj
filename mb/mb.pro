win32 {
 TARGET = ./../../libs/win32/mb
}
unix {
TARGET = ./../libs/unix/mb
}
TEMPLATE = lib
SOURCES += mb.cpp
HEADERS += mb.h
CONFIG += staticlib warn_off
header.path=./../libs
header.files=mb.h
INSTALLS+=header
