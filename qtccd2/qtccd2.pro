TEMPLATE = app
#INCLUDEPATH = C:\boost_1_33_1
HEADERS += mainwindow.h \
    ccdframe.h \
    S2C.h \
    ./../libs/fitsio.h \
    ./../libs/longnam.h \
    AccuTime.h \
    ./../astro/astro.h \
    ./../libs/mb.h \
    ./../libs/comfunc.h \
    serialWidget.h \
    begpropdlg.h \
    autosetuptimerdlg.h \
    wakeupdlg.h
SOURCES += main.cpp \
    mainwindow.cpp \
    ccdframe.cpp \
    S2C.cpp \
    ./../astro/astro.cpp \
    ./../libs/comfunc.cpp \
    serialWidget.cpp \
    begpropdlg.cpp \
    autosetuptimerdlg.cpp \
    wakeupdlg.cpp

win32 {
 LIBS =  ./../libs/win32/cfitsio.lib  AccuTime.lib
}
win32:LIBS +=  ./../libs/win32/libmb.a
TARGET = qtccd2
TRANSLATIONS = qtccd_ru.ts
CONFIG += windows exceptions warn_off

#serialport

INCLUDEPATH +=./qextserialport-1.2win-alpha
QMAKE_LIBDIR +=./qextserialport-1.2win-alpha/build

CONFIG(debug, debug|release):LIBS  += -lqextserialportd
else:LIBS  += -lqextserialport

unix:DEFINES   = _TTY_POSIX_
win32:DEFINES  = _TTY_WIN_

FORMS += begpropdlg.ui \
    autosetuptimerdlg.ui \
    wakeupdlg.ui

OTHER_FILES += \
    qtccd_ru.ts
