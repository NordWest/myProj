TEMPLATE = app
INCLUDEPATH = ./../libs
HEADERS += mainwindow.h fitsdata.h imagelabel.h \
    ./../astro/astro.h \
    ./../mb/mb.h
SOURCES += main.cpp mainwindow.cpp fitsdata.cpp imagelabel.cpp
win32 {
 LIBS +=  ./../libs/win32/cfitsio.lib
}
unix {
 LIBS +=  ./../libs/unix/libcfitsio.a \
    ./../libs/unix/libastro.a \
    ./../libs/unix/libmb.a
}
TARGET = stimproLite
CONFIG += qt release warn_off
