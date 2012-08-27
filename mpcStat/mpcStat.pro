TEMPLATE = app
SOURCES += main.cpp \
    ../libs/mpcfile.cpp \
    ../libs/comfunc.cpp
HEADERS += ../libs/mpcfile.h \
            ../libs/astro.h \
            ../libs/mb.h \
    ../libs/comfunc.h

win32:LIBS += ./../libs/win32/libmb.a \
            ./../libs/win32/libastro.a
unix:LIBS += ./../libs/unix/libmb.a \
            ./../libs/unix/libastro.a


TARGET = ./../../lab/mpcTools/mpcStat
CONFIG += console \
    qt \
    warn_off
