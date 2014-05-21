#-------------------------------------------------
#
# Project created by QtCreator 2013-12-16T14:08:35
#
#-------------------------------------------------

QT       += core

QT       -= gui

#TARGET = ~/bin/detOC
TARGET = ./../../../lab/testNbody/detOC
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app
QT += xml

LIBS += -lgomp
QMAKE_CXXFLAGS+=-fopenmp

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
#    ../libs/astro.cpp \
    ../../libs/rec.cpp \
    ../../libs/commetorbcat.cpp \
    #../libs/force_ev.cpp \
#    ../../libs/dele.cpp \
    #../libs/rada.cpp \
    ../../libs/redStat.cpp \
    ../../libs/mpcs.cpp \
    ../../libs/moody/capsule/capsuleBase/tinyxml/ticpp.cpp \
    ../../libs/moody/capsule/capsuleBase/tinyxml/tinyxml.cpp \
    ./../../libs/moody/capsule/capsuleBase/tinyxml/tinyxmlerror.cpp \
    ./../../libs/moody/capsule/capsuleBase/tinyxml/tinyxmlparser.cpp \
    ./../../libs/moody/capsule/capsuleBase/mopfile/MopState.cpp \
    ./../../libs/moody/capsule/capsuleBase/particle/Particle.cpp \
#    ../../libs/ephem_util.cpp \
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
#    ../libs/astro.h \
    ../../libs/rec.h \
    ../../libs/commetorbcat.h \
#    ../libs/mb.h \
 #   ../../libs/ephem_types.h \
 #   ../../libs/dele.h \
 #   ../../libs/ephem_types.h \
    #../libs/rada.h \
    ../../libs/redStat.h \
    ../../libs/mpcs.h \
    ./../../libs/moody/capsule/capsuleBase/tinyxml/ticpp.h \
    ./../../libs/moody/capsule/capsuleBase/tinyxml/ticpprc.h \
    ./../../libs/moody/capsule/capsuleBase/tinyxml/tinyxml.h \
    ./../../libs/moody/capsule/capsuleBase/mopfile/MopFile.h \
    ./../../libs/moody/capsule/capsuleBase/mopfile/MopItem.h \
    ./../../libs/moody/capsule/capsuleBase/mopfile/MopState.h \
    ./../../libs/moody/capsule/capsuleBase/etc/Advisor.h \
    ./../../libs/moody/capsule/capsuleBase/particle/Particle.h \
    ./../../libs/moody/capsule/capsuleBase/CapsuleBase.h \
    ../../libs/moody/moody.h \
    ../../libs/moody/capsule/Capsule.h \
 #   ../../libs/ephem_util.h \
    ../../libs/myDomMoody.h \
    ../../libs/calc_epm.h \
    ../../libs/time_a.h \
    ../../libs/mpcfile.h \
    ../../libs/filedynstr.h

#unix:LIBS+=./../../libs/unix/cspice.a
LIBS += -lmb -lastro -lephem_read -ldele -lrada -lcspice
unix:LIBS+=-lcalceph
