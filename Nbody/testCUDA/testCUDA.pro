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
    cppIntegration.cu \
    rada.cpp \
    force_ev.cpp \
    force_cuda.cu

LIBS += -lephem_read

SOURCES -= cppIntegration.cu \
            force_cuda.cu
#LIBS += -lcudart
INCLUDEPATH += /usr/local/cuda-6.0/include
INCLUDEPATH += /usr/local/cuda-6.0/samples/common/inc
INCLUDEPATH += /usr/local/cuda-6.0/lib

CUDASOURCES = cppIntegration.cu force_cuda.cu
LIBS += -L/usr/local/cuda-6.0/lib -lcudart

cu.output = ${QMAKE_FILE_BASE}.o
cu.commands = /usr/local/cuda/bin/nvcc -arch=sm_21 $$join(INCLUDEPATH,'" -I "','-I "','"') -c ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT}
cu.input = CUDASOURCES
cu.CONFIG += no_link
cu.variable_out = OBJECTS

QMAKE_EXTRA_COMPILERS += cu

HEADERS += \
    rada.h
