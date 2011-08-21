TEMPLATE = app
INCLUDEPATH = ./../libs
HEADERS += mainwindow.h fitsdata.h imagelabel.h
SOURCES += main.cpp mainwindow.cpp fitsdata.cpp imagelabel.cpp
win32 {
 LIBS +=  ./../libs/cfitsio.lib
}
unix {
 LIBS +=  ./../libs/libcfitsio.a
}
TARGET = stimproLite
CONFIG += qt release warn_off
