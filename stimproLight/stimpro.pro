TEMPLATE = app
INCLUDEPATH = ./../libs
HEADERS += mainwindow.h fitsdata.h imagelabel.h
SOURCES += main.cpp mainwindow.cpp fitsdata.cpp imagelabel.cpp
win32 {
 LIBS +=  ./../libs/win32/cfitsio.lib
}
unix {
 LIBS +=  ./../libs/unix/libcfitsio.a
}
TARGET = stimproLight
CONFIG += qt release warn_on

win32:LIBS += ./../libs/win32/libmb.a
unix:LIBS += ./../libs/unix/libmb.a
win32:LIBS += ./../libs/win32/libastro.a
unix:LIBS += ./../libs/unix/libastro.a
