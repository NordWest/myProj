TEMPLATE = app
HEADERS += ./../libs/fitsio.h \
    ./../libs/longnam.h \
    mainwindow.h \
    imagelabel.h \
    ./../libs/fitsdata.h \
    newNameDlg.h \
    ./../libs/ccdmeasurements.h \
    ./../libs/multidim.h \
    ./../libs/cmnk.h \
    ./../libs/identify.h \
    ./../libs/mb.h \
    ../libs/astro.h \
    ../libs/comfunc.h \
    gethttpdlg.h \
    getfromheaderdlg.h \
    ../libs/observatory.h \
    ../libs/redStat.h \
    findobjectdlg.h \
    gridstatw.h \
    ../libs/fitstools.h \
    ../libs/sscatFB.h \
    ../libs/sscat.h \
    ../libs/fbStream.h \
    ../libs/fBuffer.h \
    ../libs/spline3.h \
    ../libs/spline2d.h \
    ../libs/ap.h \
    ../libs/vectGrid3D.h \
    ../libs/ialglib.h \
    propwidget.h \
    setupmarkscolor.h
SOURCES += ../libs/cmnk.cpp \
    ../libs/identify.cpp \
    newNameDlg.cpp \
    ./../libs/ccdmeasurements.cpp \
    main.cpp \
    mainwindow.cpp \
    imagelabel.cpp \
    ./../libs/fitsdata.cpp \
    ./../libs/multidim.cpp \
    ../libs/astro.cpp \
    ../libs/comfunc.cpp \
    gethttpdlg.cpp \
    getfromheaderdlg.cpp \
    ../libs/observatory.cpp \
    ../libs/redStat.cpp \
    findobjectdlg.cpp \
    gridstatw.cpp \
    ../libs/fitstools.cpp \
    ../libs/sscat.cpp \
    ../libs/fbStream.cpp \
    ../libs/fBuffer.cpp \
    ../libs/spline3.cpp \
    ../libs/spline2d.cpp \
    ../libs/ap.cpp \
    ../libs/vectGrid3D.cpp \
    ../libs/ialglib.cpp \
    propwidget.cpp \
    setupmarkscolor.cpp
FORMS = newNameDlg.ui \
    getHttpDlg.ui \
    getfromheaderdlg.ui \
    findobjectdlg.ui \
    gridstatw.ui \
    propwidget.ui \
    setupmarkscolor.ui
win32:LIBS += ./../libs/cfitsio.lib  \
    ../libs/ablas.dll
unix:LIBS += ./../libs/libcfitsio.a
win32:LIBS += ./../libs/win32/libmb.a
unix:LIBS += ./libmb.a
TARGET = stimpro
QT += network
CONFIG += qt \
    warn_off \
    uitools \
    console \
    debug
