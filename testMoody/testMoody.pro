#-------------------------------------------------
#
# Project created by QtCreator 2012-07-18T12:35:20
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ./../../lab/testMoody/testMoody
CONFIG   += console
CONFIG   -= app_bundle


TEMPLATE = app


SOURCES += main.cpp \
    moody/capsule/capsuleBase/tinyxml/ticpp.cpp \
    moody/capsule/capsuleBase/tinyxml/tinyxml.cpp \
    moody/capsule/capsuleBase/tinyxml/tinyxmlerror.cpp \
    moody/capsule/capsuleBase/tinyxml/tinyxmlparser.cpp \
    moody/capsule/capsuleBase/mopfile/MopState.cpp \
    moody/capsule/capsuleBase/particle/Particle.cpp

HEADERS  +=  moody/capsule/capsuleBase/tinyxml/ticpp.h \
    ./moody/capsule/capsuleBase/tinyxml/ticpprc.h \
    ./moody/capsule/capsuleBase/tinyxml/tinyxml.h \
    ./moody/capsule/capsuleBase/mopfile/MopFile.h \
    ./moody/capsule/capsuleBase/mopfile/MopItem.h \
    ./moody/capsule/capsuleBase/mopfile/MopState.h \
    ./moody/capsule/capsuleBase/etc/Advisor.h \
    moody/capsule/capsuleBase/particle/Particle.h \
    moody/capsule/capsuleBase/CapsuleBase.h

