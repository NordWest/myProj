#-------------------------------------------------
#
# Project created by QtCreator 2011-03-07T02:25:35
#
#-------------------------------------------------

QT       += core gui
INCLUDEPATH +=D:/svnProj/qextserialport-1.2win-alpha
QMAKE_LIBDIR +=D:/svnProj/qextserialport-1.2win-alpha/build
TARGET = napoint
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

CONFIG(debug, debug|release):LIBS  += -lqextserialportd
else:LIBS  += -lqextserialport

unix:DEFINES   = _TTY_POSIX_
win32:DEFINES  = _TTY_WIN_
