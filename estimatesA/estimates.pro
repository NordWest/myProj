TEMPLATE = app
HEADERS += ../libs/comfunc.h \
    ../libs/redStat.h \
#    ../libs/multidim.h \
    ../libs/observatory.h \
#    ../libs/fitsdata.h \
#    ../libs/fitsio.h \
#    ../libs/identify.h \
#    ../libs/cmnk.h  \
#    ../libs/longnam.h   \
#    ./../libs/ccdmeasurements.h \
#    ../libs/fitstools.h \
    ../libs/sscat.h \
    ../libs/fbStream.h \
    ../libs/fBuffer.h \
#    ../libs/vectGrid3D.h \
#    ../libs/ialglib.h \
    ../libs/mpccat.h \
    ../libs/orbcat.h \
    ../libs/filedynstr.h
#    ../libs/sysCorr.h \
#    ../libs/uneven.h
SOURCES += main.cpp  \
    ./../libs/redStat.cpp \
#    ./../libs/astro.cpp \
    ./../libs/comfunc.cpp \
#    ./../libs/multidim.cpp \
    ./../libs/observatory.cpp \
#    ./../libs/fitsdata.cpp \
#    ./../libs/identify.cpp \
#    ./../libs/cmnk.cpp  \
#    ./../libs/ccdmeasurements.cpp \
#    ../libs/fitstools.cpp \
    ../libs/sscat.cpp \
    ../libs/fbStream.cpp \
    ../libs/fBuffer.cpp \
#    ../libs/vectGrid3D.cpp \
#    ../libs/ialglib.cpp \
    ../libs/mpccat.cpp \
    ../libs/orbcat.cpp \
    ../libs/filedynstr.cpp
#    ../libs/sysCorr.cpp \
#    ../libs/uneven.cpp

//SOURCES += astro.cpp
TARGET = ~/bin/estimates
CONFIG += console \
    exceptions \
    warn_off

#win32:LIBS += ./../libs/win32/cfitsio.lib
win32:LIBS += ./../libs/win32/libastro.a
#unix:LIBS += ./../libs/unix/libastro.a
#unix:LIBS += ./../libs/unix/libcfitsio.a
#unix:LIBS += ./../libs/unix/libmb.a
LIBS += -lmb -lastro
