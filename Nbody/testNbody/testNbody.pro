# -------------------------------------------------
# Project created by QtCreator 2010-02-21T18:45:28
# -------------------------------------------------
QT -= gui
#TARGET = ./../../../lab/testNbody/testNbody
TARGET = ~/bin/testNbody
CONFIG += qt \
    console \
    warn_off \
    debug
CONFIG -= app_bundle
QT           += xml
TEMPLATE = app

LIBS += -lgomp
QMAKE_CXXFLAGS+=-fopenmp

SOURCES += main.cpp \
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

#unix:LIBS+=./../../libs/unix/libmb.a
win32:LIBS+=./../../libs/win32/libmb.a
#unix:LIBS+=./../../libs/unix/libastro.a
win32:LIBS+=./../../libs/win32/libastro.a

unix:LIBS += -lmb -lastro -ldele -lephem_read -lrada
unix:LIBS+=-lcalceph
unix:LIBS+= ./../../libs/unix/cspice.a
#INCLUDEPATH += /usr/local/cuda/include
