#-------------------------------------------------
#
# Project created by QtCreator 2014-01-15T11:28:53
#
#-------------------------------------------------

QT       -= gui

TARGET = ephem_read
TEMPLATE = lib

DEFINES += EPHEM_READ_LIBRARY

SOURCES += ephem_read.cpp

HEADERS += ephem_read.h\
        ephem_read_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
