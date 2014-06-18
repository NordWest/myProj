#-------------------------------------------------
#
# Project created by QtCreator 2013-09-09T15:07:48
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = testCUDA
CONFIG   += console warn_off
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    cppIntegration_gold.cpp \
    cppIntegration.cu
SOURCES -= cppIntegration.cu
#LIBS += -lcudart
INCLUDEPATH += /usr/local/cuda-5.5/include
INCLUDEPATH += /usr/local/cuda-5.5/samples/common/inc
INCLUDEPATH += /usr/local/cuda-5.5/lib

CUDASOURCES = cppIntegration.cu
LIBS += -L/usr/local/cuda-5.5/lib -lcudart

cu.output = ${QMAKE_FILE_BASE}.o
cu.commands = /usr/local/cuda/bin/nvcc $$join(INCLUDEPATH,'" -I "','-I "','"') -c ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT}
cu.input = CUDASOURCES
cu.CONFIG += no_link
cu.variable_out = OBJECTS

QMAKE_EXTRA_COMPILERS += cu
