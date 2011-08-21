TEMPLATE = app
SOURCES += main.cpp \
 #   ../libs/astro.cpp \
    ../libs/fitsdata.cpp \
    ../libs/DynArr.cpp \
    ../libs/comfunc.cpp \
    ../libs/multidim.cpp \
    ../libs/ccdmeasurements.cpp \
    ../libs/redStat.cpp \
    ../libs/observatory.cpp \
    ../libs/identify.cpp \
    ../libs/cmnk.cpp \
    ../libs/sscat.cpp \
    ../libs/fBuffer.cpp \
    ../libs/spline3.cpp \
    ../libs/spline2d.cpp \
    ../libs/ap.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/ialglib.cpp \
    ../libs/fitstools.cpp

win32:LIBS += ./../libs/win32/cfitsio.lib \
            ./../libs/win32/libmb.a \
            ./../libs/win32/libastro.a \
            ../libs/ablas.dll
unix:LIBS += ./../libs/unix/libcfitsio.a \
            ./../libs/unix/libmb.a \
            ./../libs/unix/libastro.a

unix:TARGET = ./../../lab/ruler3PL.unix/ruler3PL
win32:TARGET = ./../../../lab/ruler3PL.win32/ruler3PL

CONFIG += console \
    exceptions \
    warn_off

HEADERS += ../libs/astro.h \
    ../libs/mb.h \
    ../libs/fitsio.h \
    ../libs/fitsdata.h \
    ../libs/DynArr.h \
    ../libs/comfunc.h \
    ../libs/multidim.h \
    ../libs/ccdmeasurements.h \
    ../libs/redStat.h \
    ../libs/observatory.h \
    ../libs/identify.h \
    ../libs/cmnk.h \
    ../libs/sscat.h \
    ../libs/fBuffer.h \
    ../libs/spline3.h \
    ../libs/spline2d.h \
    ../libs/ap.h \
    ../libs/vectGrid3D.h \
    ../libs/ialglib.h \
    ../libs/fitstools.h
