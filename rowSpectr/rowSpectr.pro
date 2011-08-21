# -------------------------------------------------
# Project created by QtCreator 2009-10-14T23:21:44
# -------------------------------------------------
TARGET = rowSpectr
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    ../libs/comfunc.cpp \
    ../libs/fft.cpp \
    ../libs/complex.cpp
HEADERS += mainwindow.h \
    ../libs/comfunc.h \
    ../libs/fft.h \
    ../libs/complex.h
FORMS += mainwindow.ui
CONFIG += console \
    exceptions \
    warn_off \
    release
