TEMPLATE = app
SOURCES += main.cpp \
    ../../libs/mpcfile.cpp \
    ../../libs/comfunc.cpp \
    ../../libs/ringpix.cpp \
    ../../libs/vsfFunc.cpp \
    ../../libs/redStat.cpp \
    ../../libs/observatory.cpp \
    ../../libs/observ.cpp \
#    ../libs/dele.cpp \
    ../../libs/coord.cpp \
    ../../libs/filedynstr.cpp \
    ../../libs/time_a.cpp
HEADERS += ../../libs/mpcfile.h \
#            ../libs/astro.h \
#            ../libs/mb.h \
    ../../libs/comfunc.h \
    ../../libs/ringpix.h \
    ../../libs/mpcStats.h \
    ../../libs/vsfFunc.h \
    ../../libs/redStat.h \
    ../../libs/observatory.h \
    ../../libs/observ.h \
#    ../libs/dele.h \
    ../../libs/coord.h \
    ../../libs/filedynstr.h \
    ../../libs/time_a.h

win32:LIBS += ./../../libs/win32/libmb.a \
            ./../../libs/win32/libastro.a
unix:LIBS += -lmb -lastro -ldele -lephem_read
unix:LIBS+=-lcalceph

TARGET = ./../../../lab/mpcStat/mpcStat
CONFIG += console \
    qt \
    warn_off
CONFIG -= gui

