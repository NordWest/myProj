TEMPLATE = app
#INCLUDEPATH = C:\boost_1_33_1
SOURCES += main.cpp \
    ./../libs/astro.cpp \
    ../libs/comfunc.cpp \
    ../libs/platesStat.cpp \
    ../libs/fitsdata.cpp \
    ../libs/ccdmeasurements.cpp \
    ../libs/identify.cpp \
    ../libs/cmnk.cpp \
    ../libs/mb.cpp \
    ../libs/redStat.cpp \
    ../libs/multidim.cpp \
    ../libs/observatory.cpp \
    ../libs/sscat.cpp \
    ../libs/fBuffer.cpp \
    ../libs/spline3.cpp \
    ../libs/spline2d.cpp \
    ../libs/ap.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/ialglib.cpp \
    ../libs/fitstools.cpp
HEADERS += fitsio.h \
    ./../libs/astro.h \
    ../libs/comfunc.h \
    ../libs/platesStat.h \
    ../libs/fitsdata.h \
    ../libs/ccdmeasurements.h \
    ../libs/identify.h \
    ../libs/cmnk.h \
    ../libs/mb.h \
    ../libs/redStat.h \
    ../libs/multidim.h \
    ../libs/observatory.h \
    ../libs/sscat.h \
    ../libs/fBuffer.h \
    ../libs/spline3.h \
    ../libs/spline2d.h \
    ../libs/ap.h \
    ../libs/vectGrid3D.h \
    ../libs/ialglib.h \
    ../libs/fitstools.h

# win32:LIBS += cfitsio.lib
# unix:LIBS += libcfitsio.a
TARGET = plStat
CONFIG += console \
    qt \
    warn_off \
    debug
win32:LIBS += ./../libs/win32/cfitsio.lib  \
    ../libs/ablas.dll
unix:LIBS += ./../libs/unix/libcfitsio.a
