TEMPLATE = app
INCLUDEPATH =  C:\boost_1_33_1
SOURCES +=main.cpp \
    ../libs/ccdmeasurements.cpp \
    ../libs/fitstools.cpp \
    ../libs/comfunc.cpp \
    ../libs/fitsdata.cpp \
    ../libs/sscat.cpp \
    ../libs/fbStream.cpp \
    ../libs/fBuffer.cpp \
    ../libs/spline3.cpp \
    ../libs/spline2d.cpp \
    ../libs/ap.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/ialglib.cpp \
    ../libs/observatory.cpp \
    ../libs/redStat.cpp \
    ./../libs/multidim.cpp \
    ../libs/cmnk.cpp \
    ../libs/identify.cpp \
    maindlg.cpp
SOURCES +=OpenFileExDlg.cpp
HEADERS +=OpenFileExDlg.h \
    ../libs/ccdmeasurements.h \
    ../libs/astro.h \
    ../libs/fitstools.h \
    ../libs/longnam.h \
    ../libs/fitsio.h \
    ../libs/comfunc.h \
    ../libs/mb.h \
    ../libs/fitsdata.h \
    ../libs/sscatFB.h \
    ../libs/sscat.h \
    ../libs/fbStream.h \
    ../libs/fBuffer.h \
    ../libs/spline3.h \
    ../libs/spline2d.h \
    ../libs/ap.h \
    ../libs/vectGrid3D.h \
    ../libs/ialglib.h \
    ../libs/observatory.h \
    ../libs/redStat.h \
    ./../libs/multidim.h \
    ./../libs/cmnk.h \
    ./../libs/identify.h \
    maindlg.h
HEADERS +=./../libs\astro.h
HEADERS +=dialog.h
SOURCES +=./../libs/astro.cpp
SOURCES +=dialog.cpp
win32:LIBS += ./../libs/cfitsio.lib
unix:LIBS += ./../libs/libcfitsio.a

TARGET = tiff2fits
CONFIG += qt warn_off
win32:LIBS += ../libs/win/libmb.a
unix:LIBS += ../libs/unix/libmb.a

FORMS += \
    maindlg.ui
