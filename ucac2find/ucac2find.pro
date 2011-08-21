TEMPLATE = app
#INCLUDEPATH += C:\boost_1_33_1
SOURCES += main.cpp ./../libs/astro.cpp \
    ../libs/comfunc.cpp
HEADERS += ./../libs/fitsio.h ./../libs/astro.h \
    ../libs/comfunc.h
win32 {
 LIBS +=  ./../libs/win32/cfitsio.lib
}
unix {
 LIBS +=  ./../libs/unix/libcfitsio.a
}
TARGET = ucac2find
CONFIG += console exceptions warn_off release
