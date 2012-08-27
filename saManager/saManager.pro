#-------------------------------------------------
#
# Project created by QtCreator 2012-08-07T11:20:35
#
#-------------------------------------------------

QT       += core gui

TARGET = ./../../lab/saTest/saManager
TEMPLATE = app

CONFIG += warn_off

SOURCES += main.cpp\
        mainwindow.cpp \
    ../libs/sscat.cpp \
    ../libs/orbit.cpp \
    ../libs/orbcat.cpp \
    ../libs/observatory.cpp \
    ../libs/observ.cpp \
    ../libs/mpccat.cpp \
    ../libs/dele.cpp \
    ../libs/coord.cpp \
    ../libs/fBuffer.cpp \
    ../libs/comfunc.cpp \
    ../libs/sscatFB.cpp \
    ../libs/fbStream.cpp \
    ../libs/redStat.cpp \
    ../libs/mpcfile.cpp \
    ../libs/listfile.cpp \
    ../libs/skyarealf.cpp \
    catsview.cpp \
    addcatdlg.cpp \
    addtaskdlg.cpp \
    addobjdlg.cpp \
    adddeledlg.cpp \
    addmpcdlg.cpp

HEADERS  += mainwindow.h \
    ../libs/sscat.h \
    ../libs/orbit.h \
    ../libs/orbcat.h \
    ../libs/observatory.h \
    ../libs/observ.h \
    ../libs/mpccat.h \
    ../libs/dele.h \
    ../libs/coord.h \
    ../libs/fBuffer.h \
    ../libs/comfunc.h \
    ../libs/sscatFB.h \
    ../libs/fbStream.h \
    ../libs/mb.h \
    ../libs/astro.h \
    ../libs/redStat.h \
    ../libs/mpcfile.h \
    ../libs/listfile.h \
    ../libs/skyarealf.h \
    catsview.h \
    addcatdlg.h \
    addtaskdlg.h \
    addobjdlg.h \
    adddeledlg.h \
    addmpcdlg.h

win32:LIBS += ./../libs/win32/libmb.a \
            ./../libs/win32/libastro.a
unix:LIBS += ./../libs/unix/libmb.a \
            ./../libs/unix/libastro.a


FORMS    += mainwindow.ui \
    catsview.ui \
    addcatdlg.ui \
    addtaskdlg.ui \
    addobjdlg.ui \
    adddeledlg.ui \
    addmpcdlg.ui
