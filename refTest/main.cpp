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
    qInstallMsgHandler(customMessageHandler);

    QCoreApplication a(argc, argv);

    QString logFileName("messages.log");
    QFile* logFile = new QFile(logFileName);
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);

    QString cfgFile("refTest.ini");
    QSettings *sett = new QSettings(cfgFile, QSettings::IniFormat);

    refractionParam *refParam = new refractionParam;

    refParam->utc = MJD;

    refParam->ra0 = WCSdata[2];
    refParam->de0 = WCSdata[3];

    refParam->Fi = obsCurr->getFi();
    refParam->Long = obsCurr->Long;
    refParam->lam = 0.575;



    qInstallMsgHandler(0);

    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    return 0;
   // return a.exec();
}
