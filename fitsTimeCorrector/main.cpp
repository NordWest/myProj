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

    QString workDirName("./orig");
    QString resDirName("./res");

    QStringList dirList;
    QStringList dataFiles, filters, wfList;
    QString tFile;
    QDir tDir, rDir;
    int i, j, k, szi, szj, wfSz;
    double t0, t1, dt;
    fitsdata fitsd;
    QString dateCode, dateCodeCur, nName, nDirName;
    QFileInfo fi;

    rDir.setPath(resDirName);

    filters << "*.fit";

    QDirIterator it(workDirName, QDir::Dirs|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

     while (it.hasNext()) {
         //qDebug() << it.filePath();
         tFile = it.next();
         dirList << tFile;
         qDebug() << tFile << "\n\n";
     }

     szi = dirList.size();
     for(i=0; i<szi; i++)
     {
         tDir.setPath(dirList.at(i));
         dataFiles = tDir.entryList(filters, QDir::Files, QDir::Time | QDir::Reversed);
         qDebug() << dataFiles.join("\n") << "\n\n";
         szj = dataFiles.size();
         wfList.clear();

         nDirName = dirList.at(i);
         nDirName.replace(workDirName, resDirName);
         qDebug() << QString("ndir: %1\n").arg(nDirName);
         QDir().mkpath(nDirName);

         for(j=0; j<szj; j++)
         {
             fitsd.clear();
             tFile = QString("%1/%2").arg(dirList.at(i)).arg(dataFiles.at(j));

             qDebug() << QString("tFile: %1\n").arg(tFile);

             if(fitsd.openFile(tFile))
             {
                 qDebug() << QString("err open file: %1\n").arg(tFile);
                 continue;
             }


             mjdDateCode_file(&dateCodeCur, fitsd.MJD);


             qDebug() << QString("dcCur: %1\tdc: %2\n").arg(dateCodeCur).arg(dateCode);

             if((QString().compare(dateCodeCur, dateCode)!=0))
             {
                 wfSz = wfList.size();
                 if(wfSz>0)
                 {
                     fitsd.clear();
                     fitsd.openFile(wfList.at(0));
                     t0 = fitsd.MJD;

                     nName = wfList.at(0);
                     nName.replace(workDirName, resDirName);
                     qDebug() << QString("new file name: %1\n").arg(nName);
                     fitsd.saveFitsAs(nName);

                     fitsd.clear();
                     fitsd.openFile(wfList.at(wfSz-1));
                     t1 = fitsd.MJD;

                     dt = fitsd.exptime;

                     for(k=1; k<wfSz; k++)
                     {
                         fitsd.clear();
                         fitsd.openFile(wfList.at(k));
                         fi.setFile(wfList.at(k));

                         fitsd.MJD = t0+dt*k;
                         nName = wfList.at(k);
                                 nName.replace(workDirName, resDirName);
                         qDebug() << QString("new file name: %1\n").arg(nName);
                         fitsd.saveFitsAs(nName);
                     }

                 }

                wfList.clear();


                dateCode = dateCodeCur;
             }

             wfList << tFile;
         }
     }


    
    return 0;//a.exec();
}
