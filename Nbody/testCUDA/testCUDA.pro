#-------------------------------------------------
#
# Project created by QtCreator 2013-09-09T15:07:48
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ./../../../lab/testNbody/testCUDA
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app
QT           += xml


SOURCES += main.cpp \
    cppIntegration_gold.cpp \
    cppIntegration.cu \
    rada.cpp \
    force_ev.cpp \
    force_cuda.cu  \
    ./../../libs/orbit.cpp \
    ./../../libs/orbcat.cpp \
    ./../../libs/observatory.cpp \
    ./../../libs/observ.cpp \
    ./../../libs/mpccat.cpp \
    ./../../libs/comfunc.cpp \
    ./../../libs/fBuffer.cpp \
    ./../../libs/fbStream.cpp \
    ./../../libs/DynArr.cpp \
    ./../../libs/coord.cpp \
    ./../../libs/sscat.cpp \
    ./../../libs/sscatFB.cpp \
#    ../libs/astro.cpp \
    ./../../libs/rec.cpp \
    ./../../libs/commetorbcat.cpp \
#    ../libs/force_ev.cpp \
#    ./force_ev.cpp \
#    ../libs/rada.cpp \
#    ./rada.cpp \
#    ./../../libs/dele.cpp \
#    dele.cpp \
    ./../../libs/redStat.cpp \
    ./../../libs/mpcs.cpp \
    ./../../libs/moody/capsule/capsuleBase/tinyxml/ticpp.cpp \
    ./../../libs/moody/capsule/capsuleBase/tinyxml/tinyxml.cpp \
    ./../../libs/moody/capsule/capsuleBase/tinyxml/tinyxmlerror.cpp \
    ./../../libs/moody/capsule/capsuleBase/tinyxml/tinyxmlparser.cpp \
    ./../../libs/moody/capsule/capsuleBase/mopfile/MopState.cpp \
    ./../../libs/moody/capsule/capsuleBase/particle/Particle.cpp \
#    ./../../libs/ephem_util.cpp \
    ./../../libs/calc_epm.cpp \
    ./../../libs/time_a.cpp \
    ../../libs/filedynstr.cpp

HEADERS += ../../libs/orbit.h \
    ./../../libs/orbcat.h \
    ./../../libs/observatory.h \
    ./../../libs/observ.h \
    ./../../libs/mpccat.h \
    ./../../libs/comfunc.h \
    ./../../libs/fBuffer.h \
    ./../../libs/fbStream.h \
    ./../../libs/DynArr.h \
    ./../../libs/coord.h \
    ./../../libs/sscat.h \
    ./../../libs/sscatFB.h \
#    ../libs/astro.h \
    ./../../libs/rec.h \
    ./../../libs/commetorbcat.h \
#    ../libs/mb.h \
#    ./../../libs/ephem_types.h \
#    ./../../libs/dele.h \
#    dele.h \
#    ephem_types.h \
#    ../libs/rada.h \
#    ./rada.h \
    ./../../libs/redStat.h \
    ./../../libs/mpcs.h \
    ./../../libs/moody/capsule/capsuleBase/tinyxml/ticpp.h \
    ./../../libs/moody/capsule/capsuleBase/tinyxml/ticpprc.h \
    ./../../libs/moody/capsule/capsuleBase/tinyxml/tinyxml.h \
    ./../../libs/moody/capsule/capsuleBase/mopfile/MopFile.h \
    ./../../libs/moody/capsule/capsuleBase/mopfile/MopItem.h \
    ./../../libs/moody/capsule/capsuleBase/mopfile/MopState.h \
    ./../../libs/moody/capsule/capsuleBase/etc/Advisor.h \
    ./../../libs/moody/capsule/capsuleBase/particle/Particle.h \
    ./../../libs/moody/capsule/capsuleBase/CapsuleBase.h \
    ./../../libs/moody/moody.h \
    ./../../libs/moody/capsule/Capsule.h \
    ./../../libs/myDomMoody.h \
#    ./../../libs/ephem_util.h \
    ./../../libs/calc_epm.h \
    ./../../libs/time_a.h \
    ../../libs/filedynstr.h


LIBS += -lmb -lastro -ldele -lephem_read
unix:LIBS+=-lcalceph -lcspice

SOURCES -= cppIntegration.cu \
            force_cuda.cu
#LIBS += -lcudart
INCLUDEPATH += /usr/local/cuda-6.0/include
INCLUDEPATH += /usr/local/cuda-6.0/samples/common/inc
INCLUDEPATH += /usr/local/cuda-6.0/lib

CUDASOURCES = cppIntegration.cu force_cuda.cu
LIBS += -L/usr/local/cuda-6.0/lib -lcudart

cu.output = ${QMAKE_FILE_BASE}.o
cu.commands = /usr/local/cuda/bin/nvcc -arch=sm_21 $$join(INCLUDEPATH,'" -I "','-I "','"') -c ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT}
cu.input = CUDASOURCES
cu.CONFIG += no_link
cu.variable_out = OBJECTS

QMAKE_EXTRA_COMPILERS += cu

HEADERS += \
    rada.h
