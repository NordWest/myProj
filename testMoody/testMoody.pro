#-------------------------------------------------
#
# Project created by QtCreator 2012-07-18T12:35:20
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ./../../lab/testMoody/testMoody
CONFIG   += console warn_off
CONFIG   -= app_bundle


TEMPLATE = app


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
    ./../libs/moody/moody/capsule/capsuleBase/particle/Particle.h \
    ./../libs/moody/moody/capsule/capsuleBase/CapsuleBase.h

