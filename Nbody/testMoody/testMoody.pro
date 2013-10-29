#-------------------------------------------------
#
# Project created by QtCreator 2012-07-18T12:35:20
#
#-------------------------------------------------

QT       += core

QT       -= gui

#TARGET = ./../../../lab/testMoody/testMoody
TARGET = ~/bin/testMoody
CONFIG   += console warn_off
CONFIG   -= app_bundle
INCLUDEPATH+=/usr/local/cuda/include


TEMPLATE = app

LIBS += -lgomp
QMAKE_CXXFLAGS+=-fopenmp


SOURCES += main.cpp \
    ./../libs/moody/capsule/capsuleBase/tinyxml/ticpp.cpp \
    ./../libs/moody/capsule/capsuleBase/tinyxml/tinyxml.cpp \
    ./../libs/moody/capsule/capsuleBase/tinyxml/tinyxmlerror.cpp \
    ./../libs/moody/capsule/capsuleBase/tinyxml/tinyxmlparser.cpp \
    ./../libs/moody/capsule/capsuleBase/mopfile/MopState.cpp \
    ./../libs/moody/capsule/capsuleBase/particle/Particle.cpp

HEADERS  +=  ./../libs/moody/capsule/capsuleBase/tinyxml/ticpp.h \
    ./../libs/moody/capsule/capsuleBase/tinyxml/ticpprc.h \
    ./../libs/moody/capsule/capsuleBase/tinyxml/tinyxml.h \
    ./../libs/moody/capsule/capsuleBase/mopfile/MopFile.h \
    ./../libs/moody/capsule/capsuleBase/mopfile/MopItem.h \
    ./../libs/moody/capsule/capsuleBase/mopfile/MopState.h \
    ./../libs/moody/capsule/capsuleBase/etc/Advisor.h \
    ./../libs/moody/capsule/capsuleBase/particle/Particle.h \
    ./../libs/moody/capsule/capsuleBase/CapsuleBase.h \
    ../libs/moody/capsule/Capsule.h \
    ../libs/moody/capsule/capsuleBase/CapsuleTBB.h \
    ../libs/moody/capsule/capsuleBase/CapsuleSerial.h \
    ../libs/moody/capsule/capsuleBase/CapsuleOpenMP.h \
    ../libs/moody/capsule/capsuleBase/CapsuleCUDA.h \
    ../libs/moody/capsule/capsuleBase/CapsuleBase.h

