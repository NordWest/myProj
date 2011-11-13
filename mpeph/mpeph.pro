TEMPLATE = app
SOURCES += main.cpp \
    ../libs/comfunc.cpp
SOURCES += ./../libs/mpeph.cpp
#SOURCES += ./../astro/astro.cpp
HEADERS += ./../libs/mpeph.h \
    ../libs/comfunc.h \
    ../libs/fitsio.h \
    ../libs/mb.h
HEADERS += ./../astro/astro.h
#win32 {
# INCLUDEPATH += C:\boost_1_33_1
#}
#unix {
#INCLUDEPATH += /mnt/winsys/boost_1_33_1
#INCLUDEPATH += /home/berezhnoy/myProjects/boost_1_33_1
#}
win32:TARGET = ./../../bin/mpeph
unix:TARGET = ./../../bin/mpeph
QT += network
CONFIG += console exceptions warn_off release
win32:LIBS += ./../libs/win32/cfitsio.lib
unix:LIBS += ./../libs/unix/libcfitsio.a
unix:LIBS += ./../libs/unix/libmb.a
win32:LIBS += ./../libs/win32/libmb.a
unix:LIBS += ./../libs/unix/libastro.a
win32:LIBS += ./../libs/win32/libastro.a
