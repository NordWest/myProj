TEMPLATE = app
HEADERS += \
    ./../libs/astro.h \
    ./../libs/fitsio.h \
    ./../libs/redStat.h \
    ../libs/comfunc.h \
    ../libs/mb.h \
    ../libs/sscat.h \
    ../libs/multidim.h \
    ../libs/fitsdata.h \
    ../libs/observatory.h \
    ../libs/identify.h \
    ../libs/cmnk.h \
    ../libs/fBuffer.h \
    ../libs/ccdmeasurements.h \
    ../libs/spline3.h \
    ../libs/spline2d.h \
    ../libs/ap.h \
    ../libs/vectGrid3D.h \
    ../libs/ialglib.h
SOURCES += ./../libs/astro.cpp \
    main.cpp \
    ./../libs/redStat.cpp \
    ../libs/comfunc.cpp \
    ../libs/sscat.cpp \
    ../libs/multidim.cpp \
    ../libs/fitsdata.cpp \
    ../libs/identify.cpp \
    ../libs/cmnk.cpp \
    ../libs/observatory.cpp \
    ../libs/fBuffer.cpp \
    ../libs/ccdmeasurements.cpp \
    ../libs/spline3.cpp \
    ../libs/spline2d.cpp \
    ../libs/ap.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/ialglib.cpp
//SOURCES += astro.cpp
TARGET = estimates
CONFIG += console \
    exceptions \
    warn_off

win32:LIBS += ./../libs/cfitsio.lib  \
    ../libs/ablas.dll
unix:LIBS += ./../libs/libcfitsio.a
win32:LIBS += ./../libs/win/libmb.a
unix:LIBS += ./../libs/unix/libmb.a
