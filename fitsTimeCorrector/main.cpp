#include <QtCore>

#include "./../libs/fitsdata.h"
#include "./../astro/astro.h"


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

    QString logFileName = "./fitsRenamer.log";
    QFile* logFile = new QFile(logFileName);
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);

    QTextCodec *codec1 = QTextCodec::codecForName("Windows-1251");
    Q_ASSERT( codec1 );

    QSettings *sett = new QSettings("./fitsTimeCorrector.ini", QSettings::IniFormat);

    QString workDirName = sett->value("general/workDir", "./orig").toString();
    QString resPathName = sett->value("general/resPath", "./res/").toString();

    QStringList dirList;
    QStringList dataFiles, filters, wfList;
    QString tFile;
    QDir tDir, rDir;
    int i, j, k, szi, szj, wfSz;
    double t0, t1, dt;
    fitsdata fitsd;
    QString dateCode, dateCodeCur, dateCodeNew, nName, nDirName;
    QFileInfo fi;
    double mjd0, mjd1, dmjd0, dmjd1, mjdN;

    //rDir.setPath(resDirName);

    filters << "*.fit";

    QDirIterator it(workDirName, QDir::Dirs|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    dirList << workDirName;
     while (it.hasNext()) {
         //qDebug() << it.filePath();
         tFile = it.next();
         dirList << tFile;
         qDebug() << tFile << "\n\n";
     }
     int serieNum = 0;

     QFile residFile("./timeRes.txt");
     residFile.open(QFile::WriteOnly | QFile::Truncate);
     QTextStream residStm(&residFile);

     szi = dirList.size();
     for(i=0; i<szi; i++)
     {
         if(dirList.at(i).indexOf("dark")!=-1||dirList.at(i).indexOf("flat")!=-1) continue;
         tDir.setPath(dirList.at(i));
         dataFiles = tDir.entryList(filters, QDir::Files, QDir::Time | QDir::Reversed);
         qDebug() << dataFiles.join("\n") << "\n\n";
         szj = dataFiles.size();
         wfList.clear();
/*
         nDirName = dirList.at(i);
         nDirName.replace(workDirName, resDirName);
         qDebug() << QString("ndir: %1\n").arg(nDirName);
         QDir().mkpath(nDirName);
*/
         for(j=0; j<szj; j++)
         {
             mjd0 = mjd1;
             dmjd0 = dmjd1;


             fitsd.clear();
             tFile = QString("%1/%2").arg(dirList.at(i)).arg(dataFiles.at(j));
             wfList << tFile;

             qDebug() << QString("tFile: %1\n").arg(tFile);

             if(fitsd.openFile(tFile))
             {
                 qDebug() << QString("err open file: %1\n").arg(tFile);
                 continue;
             }

             mjd1 = fitsd.MJD;
             dmjd1 = fabs(mjd1-mjd0);


             mjdDateCode_file(&dateCode, fitsd.MJD);
             //qDebug() << QString("dcCur: %1\tdc: %2\n").arg(dateCodeCur).arg(dateCode);

             if((dmjd1>(fitsd.exptime*2.0/86400.0)||j==szj-1)&&wfList.size()>1)
             {
                 fitsd.clear();
                 fitsd.openFile(wfList.at(0));
                 t0 = fitsd.MJD;
                 mjdDateCode_file(&dateCodeCur, fitsd.MJD);
                 //nName = QString("%1%2.fit").arg(resPathName).arg(dateCodeNew);
                 //dateCode = dateCodeCur;
                 dt = fitsd.exptime/86400.0;
                 wfSz = wfList.size();
                 qDebug() << QString("wfSz: %1\n").arg(wfSz);
                 for(k=0; k<wfSz; k++)
                 {

                     fitsd.clear();
                     fitsd.openFile(wfList.at(k));
                     mjdDateCode_file(&dateCodeCur, fitsd.MJD);

                     mjdN = t0+dt*k;
                     if((QString().compare(dateCodeCur, dateCode)!=0))
                     {
                         if(k>0) residStm << QString("%1\n").arg((mjdN - fitsd.MJD)*86400);
                     }
                     else
                     {
                         fitsd.MJD = mjdN;
                         mjdDateCode_file(&dateCodeNew, fitsd.MJD);
                         nName = QString("%1%2.fit").arg(resPathName).arg(dateCodeNew);
                         qDebug() << QString("new file name: %1\n").arg(nName);
                         fitsd.saveFitsAs(nName);
                     }

                     dateCode = dateCodeCur;
                 }
                 serieNum++;
                 wfList.clear();
             }


         }
     }

     qDebug() << QString("series: %1\n").arg(serieNum);
     residFile.close();

    
    return 0;//a.exec();
}
