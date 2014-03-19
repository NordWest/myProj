#-------------------------------------------------
#
# Project created by QtCreator 2014-03-12T14:03:51
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ./../../../lab/testNbody/eqsEstimate
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../../libs/orbit.cpp \
    ../../libs/orbcat.cpp \
    ../../libs/observatory.cpp \
    ../../libs/observ.cpp \
    ../../libs/mpccat.cpp \
    ../../libs/comfunc.cpp \
    ../../libs/fBuffer.cpp \
    ../../libs/fbStream.cpp \
    ../../libs/DynArr.cpp \
    ../../libs/coord.cpp \
    ../../libs/sscat.cpp \
    ../../libs/sscatFB.cpp \
    ../../libs/rec.cpp \
    ../../libs/commetorbcat.cpp \
    ../../libs/redStat.cpp \
    ../../libs/mpcs.cpp \
    ../../libs/calc_epm.cpp \
    ../../libs/time_a.cpp \
    ../../libs/mpcfile.cpp \
    ../../libs/filedynstr.cpp

HEADERS += ../../libs/orbit.h \
    ../../libs/orbcat.h \
    ../../libs/observatory.h \
    ../../libs/observ.h \
    ../../libs/mpccat.h \
    ../../libs/comfunc.h \
    ../../libs/fBuffer.h \
    ../../libs/fbStream.h \
    ../../libs/DynArr.h \
    ../../libs/coord.h \
    ../../libs/sscat.h \
    ../../libs/sscatFB.h \
    ../../libs/commetorbcat.h \
    ../../libs/redStat.h \
    ../../libs/mpcs.h \
    ../../libs/time_a.h \
    ../../libs/mpcfile.h \
    ../../libs/filedynstr.h

#unix:LIBS+=./../../libs/unix/cspice.a
LIBS += -lmb -lastro -ldele -lephem_read

