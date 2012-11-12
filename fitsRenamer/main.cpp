#include <QtCore>

#include "./../libs/fitsdata.h"
#include "./../astro/astro.h"

#include <QtSql>
//#include <QSqlDatabase>

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


int main(int argc, char *argv[])//fitsRenamer workDir resDir
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);

    QString logFileName = "./fitsRenamer.log";
    QFile* logFile = new QFile(logFileName);
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);

    QTextCodec *codec1 = QTextCodec::codecForName("Windows-1251");
    Q_ASSERT( codec1 );

    QSettings *sett = new QSettings("./fitsRenamer.ini", QSettings::IniFormat);

    //sett

    int expMin = sett->value("general/expMin", 0).toInt();//atoi(argv[1]);

    QString workDir;
    QString resPath;
    if(argc>1) workDir = codec1->toUnicode(argv[1]);
    else workDir = sett->value("general/workDir", "./").toString();

    if(argc>2)resPath = codec1->toUnicode(argv[2]);
    else resPath = sett->value("general/resPath", "./").toString();

    QString excNames = sett->value("general/excNames", "dark|flat").toString();
    QStringList excNamesList;
    excNamesList = excNames.split("|");
    int enSz = excNamesList.size();

    QStringList filList;
    QString tFile;
    QStringList dataFiles;
    int i, j, isSkip;

    QDir cDir(workDir);

    filList << ".fit";

    qDebug() << QString("workDir: %1\n").arg(workDir);
    qDebug() << QString("resPath: %1\n").arg(resPath);
    qDebug() << QString("filters: %1\n").arg(filList.join("|"));
    qDebug() << QString("excNames: %1\n").arg(excNames);

    QDirIterator it(workDir, QDirIterator::Subdirectories);

     while (it.hasNext()) {
         //qDebug() << it.filePath();
         tFile = it.next();
         //qDebug() << QString("tFile: %1\n").arg(tFile);
         if((tFile.lastIndexOf("/..")==tFile.lastIndexOf("/"))||(tFile.lastIndexOf("/.")==tFile.lastIndexOf("/"))||(tFile.right(1)==QString("."))) continue;

         for(i=0;i<filList.size();i++)
         {
            if((tFile.lastIndexOf(filList.at(i))==tFile.size()-filList.at(i).size())&&(tFile.size()>filList.at(i).size()))
            {
                isSkip = 0;
                for(j=0; j<enSz; j++)
                {
                    if(tFile.indexOf(excNamesList.at(j))!=-1)
                    {
                        isSkip = 1;
                        break;
                    }
                }
                if(!isSkip) dataFiles << tFile;
            }
         }


     }

     QFile rnmLog(workDir+"rename.log");
     rnmLog.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text | QIODevice::Unbuffered);
     QTextStream logStm(&rnmLog);

     QFile errLog(workDir+"error.log");
     errLog.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text | QIODevice::Unbuffered);
     QTextStream errStm(&errLog);

     fitsdata fitsd;

     int szi = dataFiles.size();

     qDebug() << QString("dataFiles size: %1\n").arg(szi);

     QString dateCode;
     //DATEOBS dObs;
     QString nFile;

     QFile fileTemp;

     qDebug() << QString("\ntotal fits: %1\n").arg(szi);

     int renNum = 0;

     for(i=0; i<szi; i++)
     {
         fitsd.clear();
         tFile = dataFiles.at(i);

         qDebug() << QString("tFile: %1\n").arg(tFile);

         if(fitsd.openFile(tFile))
         {
             errStm << QString("err open file: %1\n").arg(tFile);
             continue;
         }

         if(fitsd.exptime<expMin)
         {
             qDebug() << QString("exptime: %1\ntoo small - continue").arg(fitsd.exptime);
             continue;
         }

         mjdDateCode_file(&dateCode, fitsd.MJD);

/*
         dObs = getDATEOBSfromMJD(fitsd.MJD);
         dateCode = QString("%1").arg((int)dObs.year, 4, 10, QLatin1Char( '0' ));
         dateCode.append(QString("%1").arg((int)dObs.month, 2, 10, QLatin1Char( '0' )));
         dateCode.append(QString("%1").arg((int)dObs.day, 2, 10, QLatin1Char( '0' )));
         dateCode.append(QString("%1").arg((int)dObs.hour, 2, 10, QLatin1Char( '0' )));
         dateCode.append(QString("%1").arg((int)dObs.min, 2, 10, QLatin1Char( '0' )));
         dateCode.append(QString("%1").arg((int)dObs.sec, 2, 10, QLatin1Char( '0' )));
         dateCode.append(QString("%1").arg((int)((int)(dObs.sec*10) - ((int)dObs.sec)*10), 1, 10, QLatin1Char( '0' )));
*/

         //QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");


         nFile = QString("%1%2.fit").arg(resPath).arg(dateCode);
         //fileTemp.setFileName(tFile);
         //
         qDebug() << QString("mJD: %1\tdateCode: %2\n").arg(fitsd.MJD).arg(dateCode);

         //if(cDir.rename(tFile, nFile)) logStm << QString("%1|%2\n").arg(tFile).arg(nFile);
         if(QFile().copy(tFile, nFile))
         {
             logStm << QString("%1|%2\n").arg(tFile).arg(nFile);
             renNum++;
         }
         else qDebug() << QString("File %1 allready exists\n").arg(nFile);
         tFile.clear();
         nFile.clear();
     }

     rnmLog.close();
     errLog.close();

     qDebug() << QString("\ntotal renamed fits: %1\n").arg(renNum);

     qDebug() << QString("\nproper conclusion\n");

     qInstallMsgHandler(0);

     return 0;

    //return a.exec();

}
