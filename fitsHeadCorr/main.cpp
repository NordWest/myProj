#include <QCoreApplication>

#include "./../libs/fitsdata.h"
#include "./../libs/astro.h"
#include "./../libs/mb.h"


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

    //QList <expCorrRec*> expCorrListAply;
    //QList <expCorrRec*> expCorrList;
    //expCorrRec* ecRec;

    QStringList dirList;
    QStringList dataFiles, filters, wfList;
    QString tFile;
    QDir tDir, rDir;
    int i, j, k, l, lNum, szi, szj, wfSz, expNum, sTarg;
    double t0, t1, dt;
    fitsdata fitsd;
    QString dateCode0, dateCode1, dateCodeNew, nName, nDirName, fileName, obsName, obsNewName;
    QFileInfo fi;
    double mjd0, mjd1, dmjd0, dmjd1, mjdN, mjdNend, mjdEnd, mjdBeg, realExp, realMJD, dobsN, dendN, expCorr0;
    QString objName0, objName1, tstr;
    QString nTime;
    long nelements;
    long naxes[2];
    fitsfile *fptr;
    fitsfile *fptr_out;
    int status = 0;
    int isCorr;


    QSettings *sett = new QSettings("./fhc.ini", QSettings::IniFormat);

    QString workDirName = QDir(sett->value("general/workDir", "./orig").toString()).absolutePath();
    QString resDirName = QDir(sett->value("general/resDir", "./res").toString()).absolutePath();
    //QString goodPathName = QDir(sett->value("general/goodPath", "./resG/").toString()).absolutePath();
    int detCorr = sett->value("general/detCorr", 0).toInt();
    /*
     2 - номер файла в серии - просто порядковый номер выделенного фрагмента.
     number of file in serie - just a sequence number in selected fragment.

     >0 -
     */
    int aplyType = sett->value("general/aplyType", 0).toInt();
    int saveCorr = sett->value("general/saveCorr", 0).toInt();
    double dTcorr = sett->value("general/dTcorr", 0.0).toDouble();
    int saveFits = sett->value("general/saveFits", 0).toInt();

    double aAcorr, aBcorr, bAcorr, bBcorr;
    double aExpCorr, bExpCorr, expCorr;


    aAcorr = sett->value("corr0/aA", 0.0).toDouble();
    aBcorr = sett->value("corr0/aB", 0.0).toDouble();
    bAcorr = sett->value("corr0/bA", 0.0).toDouble();
    bBcorr = sett->value("corr0/bB", 0.0).toDouble();

    QStringList eAplyL = sett->value("corrL/expList", "").toString().split("|");
    QStringList aAplyL = sett->value("corrL/aList", "").toString().split("|");
    QStringList bAplyL = sett->value("corrL/bList", "").toString().split("|");



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
     int serieTot = 0;
     int serieCorr = 0;
     int dendCounter = 0;
     int fitsCounter = 0;
     int brCounter = 0;

     int fitsType;

     char strkey[81];


     QFile residFile;
     residFile.setFileName("./timeRes.txt");
     residFile.open(QFile::WriteOnly | QFile::Truncate);
     QTextStream residStm(&residFile);

     szi = dirList.size();
     for(i=0; i<szi; i++)
     {
        //skip dark dir
         if(dirList.at(i).indexOf("dark")!=-1||dirList.at(i).indexOf("flat")!=-1||dirList.at(i).indexOf("draft")!=-1) continue;
         qDebug() << dirList.at(i) << "\n\n";
         tDir.setPath(dirList.at(i));
         dataFiles = tDir.entryList(filters, QDir::Files, QDir::Name);// | QDir::Reversed);
         qDebug() << QString("filesNum: %1\n\n").arg(dataFiles.size());
         szj = dataFiles.size();
         wfList.clear();

         nDirName = dirList.at(i);
         nDirName.replace(workDirName, resDirName);
         qDebug() << QString("ndir: %1\n").arg(nDirName);
         if(saveFits) QDir().mkpath(nDirName);

         for(j=0; j<szj; j++)
         {
             if(dataFiles.at(j).indexOf("focus")==0)
             {
                 qDebug() << QString("SKIP: %1\n").arg(dataFiles.at(j));
                 continue;
             }
             fileName = QString("%1/%2").arg(dirList.at(i)).arg(dataFiles.at(j));


             fits_open_file(&fptr, fileName.toAscii().data(), READWRITE, &status);
             if(status)
             {
                 qDebug() << QString("%1: file not open\n").arg(fileName);
                 return 1;
             }
             status = 0;

             fits_read_key(fptr, TSTRING, "OBSERVER", strkey, NULL, &status);// try to read first WCS tag
             status = 0;
             obsName = QString(strkey);
             if((QString().compare(obsName, "Khovritchev")==0)||(QString().compare(obsName, "M.Yu.Khovritchev")==0)||(QString().compare(obsName, "M.Yu. Khovritch")==0)) obsNewName = "M.Yu. Khovritchev";

             fits_update_key(fptr, TSTRING, "OBSERVER", obsNewName.toAscii().data(), "", &status);
             status = 0;

             fits_close_file(fptr, &status);
             status = 0;
         }



     }

    
    return 0;//a.exec();
}
