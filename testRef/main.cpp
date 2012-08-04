#include <QtCore>

#include "./../libs/fitsdata.h"

static QDataStream* clog = 0;
void customMessageHandler(QtMsgType type, const char* msg)
{
    static const char* msgType[] =
    {
        "Debug    : ",
        "Warning  : ",
        "Critical : ",
        "Fatal    : "
    };

    static QTextStream cout(stdout);
    static QTextStream cerr(stderr);

    cerr << msgType[type] << msg << endl;
    if(clog && clog->device())
        *clog << type << msg;
    if(type == QtFatalMsg)
    {
        cerr << "aborting..." << endl;

#if defined(Q_CC_MSVC) && defined(QT_DEBUG) && defined(_CRT_ERROR) && defined(_DEBUG)
        int reportMode = _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
        _CrtSetReportMode(_CRT_ERROR, reportMode);
        int ret = _CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, QT_VERSION_STR, msg);
        if(ret == 0 && reportMode & _CRTDBG_MODE_WNDW)
            return;
        else if(ret == 1)
            _CrtDbgBreak();
#endif

#if defined(Q_OS_UNIX) && defined(QT_DEBUG)
        abort();
#else
        exit(1);
#endif
    }
}


int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication app(argc, argv);

    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));


    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
    clog = new QDataStream(logFile);
    
    double ra1, de1;

   // refractionParam *refParam = new refractionParam;
    observatory *ob = new observatory;
    ob->init("./../../../data/cats/Obs.txt", OBS_SIZE);
    qDebug() << QString("\ninit obs: %1\n").arg(ob->nstr);
      if(ob->getobsynumO("084")) qDebug() << "observatory is not found\n";
qDebug() << QString("fi= %1\n").arg(ob->record->Cos);
    fitsdata *fitsd = new fitsdata;
//    fitsd->is_empty = 0;

    fitsd->MJD = 34987.8345941;


//    refParam.utc = 49573.0;
    double gm1;
    jdUT1_to_GMST1(&gm1, mjd2jd(fitsd->MJD));//pady
    double s = 2.0*PI*gm1 + ob->record->Long;
//    refParam.ra0 = rad2grad(s)+30;
 //   refParam.de0 = 10.0;


    fitsd->WCSdata[2] = rad2grad(5.50247);
    fitsd->WCSdata[3] = rad2grad(-0.128384);

    fitsd->initRefractParam(ob->record);
    fitsd->refParam->temp = 13.0;
    fitsd->refParam->press = 761.6;
    fitsd->refMaker = new refractionMaker(*fitsd->refParam);

    double ra0, de0, de;
    ra0 = fitsd->WCSdata[2];
    de0 = fitsd->WCSdata[3];

    for(de = de0+2; de>=de0-2;de -=0.1)
    {

        fitsd->refMaker->forvRef(&ra1, &de1, ra0, de);
    }
    
    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);
    return 0;
}
