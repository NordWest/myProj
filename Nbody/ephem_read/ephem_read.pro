#-------------------------------------------------
#
# Project created by QtCreator 2014-01-15T11:28:53
#
#-------------------------------------------------

QT       -= gui

TARGET = ephem_read
TEMPLATE = lib

VERSION = 1.0.0

DEFINES += EPHEM_READ_LIBRARY

SOURCES += ephem_read.cpp \
    ephem_util.cpp

HEADERS += ephem_read.h\
        ephem_read_global.h \
    ephem_util.h \
    ephem_types.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/local/lib
        header.path=/usr/local/include
    }
    header.files=ephem_read.h ephem_util.h ephem_types.h


    INSTALLS += target
    INSTALLS+=header
}
