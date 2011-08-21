TEMPLATE = app
INCLUDEPATH = ./../libs
HEADERS += mainwindow.h fitsdata.h imagelabel.h
SOURCES += main.cpp mainwindow.cpp fitsdata.cpp imagelabel.cpp
win32 {
 LIBS +=  cfitsio.lib
}
unix {
 LIBS +=  ./../libs/libcfitsio.a
}
TARGET = /home/ymax/bin/stimpro
CONFIG += qt release warn_on
