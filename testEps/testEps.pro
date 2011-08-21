#-------------------------------------------------
#
# Project created by QtCreator 2010-08-10T11:52:43
#
#-------------------------------------------------



TARGET = testEps
CONFIG   += console \
    warn_off
CONFIG   -= app_bundle

TEMPLATE = app


win32:LIBS += ./../libs/win/libmb.a
unix:LIBS += ./../libs/unix/libmb.a

SOURCES += main.cpp
    
