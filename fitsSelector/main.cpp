#include <QCoreApplication>
#include <QtCore>
#include "./../libs/fitsdata.h"
#include "./../astro/astro.h"
#include "./../libs/comfunc.h"
#include "./../libs/observatory.h"

static QTextStream* clog = 0;
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


int main(int argc, char *argv[])//fitsSelector [options]
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication app(argc, argv);

    setlocale(LC_NUMERIC, "C");

    QString codecName;
    #if defined(Q_OS_LINUX)
    codecName = "UTF-8";
    #elif defined(Q_OS_WIN)
    codecName = "CP1251";
    #endif

    QTextCodec *codec1 = QTextCodec::codecForName(codecName.toAscii().constData());
    Q_ASSERT( codec1 );
    QTextCodec::setCodecForCStrings(codec1);

    //command line  ///////////////////////////////////////
        QString optName, optVal, optStr, pnStr, headerFileName;
        QString tFile;
        sysCorrParam *sysCorr = NULL;
        QString cfgFileName = "fitsSelector.ini";
        QString resFileName = "resFile.txt";
        QString workDir = "./";
        int sz, i, j, szj, oNum;
        int resRed = 0;
        QString descS, oName;
        QList <catFinder*> starCatList;
        QString obsCode;
        int resdirDef=0;
        QStringList dataFiles;
        QString kVal;

        int isRF = 0;
        int isWP = 0;

        for(i=1; i<argc; i++)
        {
            optStr = QString(argv[i]);
            optName = optStr.section("=", 0, 0);
            optVal = optStr.section("=", 1, 1);
            if(QString::compare(optName, "config", Qt::CaseSensitive)==0)
            {
                cfgFileName = optVal;
            }
            else if(QString::compare(optName, "resFile", Qt::CaseSensitive)==0)
            {
                isRF = 1;
                resFileName = optVal;
            }
            else if(QString::compare(optName, "workDir", Qt::CaseSensitive)==0)
            {
                isWP = 1;
                workDir = optVal;
            }
            else
            {
                qDebug() << "\nERR: Wrong command line param: config | resFile | workDir\n";
                return 1;
            }
        }


///////// 1. Reading settings ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //stream << "started....\n";

    //BEGIN settings
    QSettings *sett = new QSettings(cfgFileName, QSettings::IniFormat);

    QStringList targetList = sett->value("general/targetList", "").toString().split("|");
    if(!isWP) workDir = sett->value("general/workDir", "./").toString();
    QStringList filList = sett->value("general/filtersList", ".fit").toString().split("|");


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    QDirIterator it(workDir, QDirIterator::Subdirectories);

     while (it.hasNext()) {
         tFile = it.next();
         if((tFile.lastIndexOf("/..")==tFile.lastIndexOf("/"))||(tFile.lastIndexOf("/.")==tFile.lastIndexOf("/"))||(tFile.right(1)==QString("."))) continue;

         for(i=0;i<filList.size();i++)
         {
            if((tFile.lastIndexOf(filList.at(i))==tFile.size()-filList.at(i).size())&&(tFile.size()>filList.at(i).size()))
            {
                dataFiles << tFile;
            }
         }
     }

     szj = targetList.size();
     fitsdata fitsd;
     sz = dataFiles.size();

     QFile resFile(resFileName);
     resFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
     QTextStream resStm(&resFile);


     for(i=0; i<sz; i++)
     {
         tFile = dataFiles.at(i);
         fitsd.clear();
         fitsd.openFile(dataFiles.at(i));
         fitsd.headList.getKeyName("TARGET", &kVal);
         for(j=0; j<szj; j++)
         {
             if(kVal.indexOf(targetList.at(j))!=-1)
             {
                 resStm << tFile << "\n";
             }
         }
     }


    return 0;//a.exec();
}
