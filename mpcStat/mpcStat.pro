TEMPLATE = app
SOURCES += main.cpp \
    ./../libs/astro.cpp \
    ./../libs/redStat.cpp \
    ../libs/comfunc.cpp \
    ../libs/spline3.cpp \
    ../libs/spline2d.cpp \
    ../libs/multidim.cpp \
    ../libs/ap.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/ialglib.cpp \
    ../libs/ccdmeasurements.cpp
HEADERS += ./../libs/fitsio.h \
    ./../libs/astro.h \
    ./../libs/redStat.h \
    ../libs/comfunc.h \
    ../libs/mb.h \
    ../libs/spline3.h \
    ../libs/spline2d.h \
    ../libs/multidim.h \
    ../libs/ap.h \
    ../libs/vectGrid3D.h \
    ../libs/ialglib.h \
    ../libs/ccdmeasurements.h
win32:LIBS += cfitsio.lib \
    ../libs/ablas.dll
win32:LIBS += ./../libs/libmb.a
unix:LIBS += ./libmb.a

unix:LIBS += libcfitsio.a
TARGET = mpcStat
CONFIG += console \
    qt \
    warn_off \
    debug
