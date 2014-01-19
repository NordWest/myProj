TEMPLATE = app
SOURCES += main.cpp \
    miriadeeph.cpp
HEADERS += miriadeeph.h

TARGET = ~/bin/miriadeEph
QT += network
CONFIG += console exceptions warn_off release

win32:LIBS += libmb libastro
unix:LIBS += -lmb -lastro
