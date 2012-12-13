#include <QtCore>

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


struct expCorrRec
{
    int expSec;
    QVector <double> corrL;
    QVector <double> durat;
    QVector <int> kNum;
};

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

    QList <expCorrRec*> expCorrListAply;
    QList <expCorrRec*> expCorrList;
    expCorrRec* ecRec;

    QStringList dirList;
    QStringList dataFiles, filters, wfList;
    QString tFile;
    QDir tDir, rDir;
    int i, j, k, l, lNum, szi, szj, wfSz;
    double t0, t1, dt;
    fitsdata fitsd;
    QString dateCode0, dateCode1, dateCodeNew, nName, nDirName;
    QFileInfo fi;
    double mjd0, mjd1, dmjd0, dmjd1, mjdN;


    QSettings *sett = new QSettings("./fitsTimeCorrector.ini", QSettings::IniFormat);

    QString workDirName = QDir(sett->value("general/workDir", "./orig").toString()).absolutePath();
    QString resPathName = QDir(sett->value("general/resPath", "./res/").toString()).absolutePath();
    QString goodPathName = QDir(sett->value("general/goodPath", "./resG/").toString()).absolutePath();
    int detCorr = sett->value("general/detCorr", 0).toInt();
    int aplyType = sett->value("general/aplyType", 0).toInt();
    int saveCorr = sett->value("general/saveCorr", 0).toInt();

    double aAcorr, aBcorr, bAcorr, bBcorr;
    double aExpCorr, bExpCorr, expCorr;


    aAcorr = sett->value("corr/aA", 0.0).toDouble();
    aBcorr = sett->value("corr/aB", 0.0).toDouble();
    bAcorr = sett->value("corr/bA", 0.0).toDouble();
    bBcorr = sett->value("corr/bB", 0.0).toDouble();

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
     int serieGood = 0;
     int serieCorr = 0;

     QFile residFile;
     residFile.setFileName("./timeRes.txt");
     residFile.open(QFile::WriteOnly | QFile::Truncate);
     QTextStream residStm(&residFile);

     szi = dirList.size();
     for(i=0; i<szi; i++)
     {
         if(dirList.at(i).indexOf("dark")!=-1||dirList.at(i).indexOf("flat")!=-1) continue;
         qDebug() << dirList.at(i) << "\n\n";
         tDir.setPath(dirList.at(i));
         dataFiles = tDir.entryList(filters, QDir::Files, QDir::Name);// | QDir::Reversed);
         //qDebug() << dataFiles.join("\n") << "\n\n";
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


             qDebug() << QString("tFile: %1\n").arg(tFile);

             if(fitsd.openFile(tFile))
             {
                 qDebug() << QString("err open file: %1\n").arg(tFile);
                 continue;
             }

             mjd1 = fitsd.MJD;
             dmjd1 = fabs(mjd1-mjd0);


             //mjdDateCode_file(&dateCode, fitsd.MJD);
             //qDebug() << QString("dcCur: %1\tdc: %2\n").arg(dateCodeCur).arg(dateCode);

             //if((dmjd1>(fitsd.exptime*1.5/86400.0)||j==szj-1)&&wfList.size()>0)
             if((dmjd1>dmjd0*1.5||j==szj-1)&&wfList.size()>0)
             {
                 //if(j==szj-1) wfList << tFile;
                 fitsd.clear();
                 fitsd.openFile(wfList.at(0));
                 t0 = fitsd.MJD;
                 mjdDateCode_file(&dateCode0, fitsd.MJD);
                 //dateCode = dateCodeCur;
                 //nName = QString("%1%2.fit").arg(resPathName).arg(dateCodeNew);
                 //dateCode = dateCodeCur;
                 dt = fitsd.exptime/86400.0;
                 wfSz = wfList.size();
                 if(wfSz>1)
                 {
                    fitsd.clear();
                    fitsd.openFile(wfList.at(wfSz-1));
                    mjdDateCode_file(&dateCode1, fitsd.MJD);

                    //if((QString().compare(dateCode0, dateCode1)!=0))


                 qDebug() << QString("wfSz: %1\n").arg(wfSz);
                 for(k=0; k<wfSz; k++)
                 {

                     fitsd.clear();
                     fitsd.openFile(wfList.at(k));
                     //mjdDateCode_file(&dateCodeCur, fitsd.MJD);
                     mjdN = t0+dt*k;
                     switch(aplyType)
                     {
                         case 1:
                         aExpCorr = aAcorr*fitsd.exptime + aBcorr;
                         bExpCorr = bAcorr*fitsd.exptime + bBcorr;
                         expCorr = (aExpCorr*k+bExpCorr)/86400.0;
                         mjdN -= expCorr;
                         break;
                         case 2:
                             for(l=0;l<eAplyL.size();l++)
                             {
                                 if(floor(eAplyL.at(l).toDouble())==floor(fitsd.exptime))
                                 {
                                     expCorr = (aAplyL.at(l).toDouble()*k+bAplyL.at(l).toDouble())/86400.0;
                                     break;
                                 }
                             }
                             mjdN -= expCorr;
                             break;
                     }
                     if((QString().compare(dateCode0, dateCode1)!=0))
                     {
                         if(k>0)
                         {
                             residStm << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(k, 3).arg(k*dt*86400.0, 6).arg((mjdN - fitsd.MJD)*86400, 12, 'f', 4).arg(fitsd.MJD, 12, 'f', 6).arg(mjdN, 12, 'f', 6).arg(t0, 12, 'f', 6).arg(dt*86400.0).arg(wfList.at(k));
                             residStm.flush();

                             if(detCorr)
                             {
                                 lNum = -1;
                                 for(l=0; l<expCorrList.size(); l++)
                                 {
                                     if(expCorrList.at(l)->expSec==fitsd.exptime)
                                     {
                                         lNum = l;
                                         expCorrList.at(l)->corrL << (mjdN - fitsd.MJD)*86400;
                                         expCorrList.at(l)->durat << k*dt*86400.0;
                                         expCorrList.at(l)->kNum << k;
                                         break;
                                     }
                                 }
                                 if(lNum==-1)
                                 {
                                     ecRec = new expCorrRec;
                                     ecRec->expSec = fitsd.exptime;
                                     ecRec->corrL << (mjdN - fitsd.MJD)*86400;
                                     ecRec->durat << k*dt*86400.0;
                                     ecRec->kNum << k;
                                     expCorrList << ecRec;
                                 }
                             }
                         }
                         /*mjdDateCode_file(&dateCodeNew, fitsd.MJD);
                         nName = QString("%1/%2.fit").arg(goodPathName).arg(dateCodeNew);
                         qDebug() << QString("new file name: %1\n").arg(nName);
                         fitsd.saveFitsAs(nName);*/
                     }
                     else
                     {
                         fitsd.MJD = mjdN;
                         mjdDateCode_file(&dateCodeNew, fitsd.MJD);
                         nName = QString("%1/%2.fit").arg(resPathName).arg(dateCodeNew);
                         qDebug() << QString("new file name: %1\n").arg(nName);
                         fitsd.saveFitsAs(nName);
                     }

                     //dateCode = dateCodeCur;
                 }

                 if((QString().compare(dateCode0, dateCode1)==0)) serieCorr++;

                 }
                 serieTot++;
                 wfList.clear();
             }

             wfList << tFile;
         }
     }

     qDebug() << QString("seriesTot: %1\nseriesCorr: %2\n").arg(serieTot).arg(serieCorr);
     residFile.close();

     if(detCorr)
     {
         QStringList expResList, aResList, bResList;
         lNum = expCorrList.size();
         QVector <double> expVal, aVal, bVal;
         double aA, aB, bA, bB;
         double *L, *Z, *C;
         L = NULL;
         Z = NULL;
         C = NULL;

         qDebug() << QString("exp num: %1\n").arg(lNum);

         for(l=0; l<lNum; l++)
         {
             residFile.setFileName(QString("./timeRes_%1.txt").arg(expCorrList.at(l)->expSec, 3, 10, QLatin1Char( '0' ) ));
             residFile.open(QFile::WriteOnly | QFile::Truncate);
             residStm.setDevice(&residFile);

             szi = expCorrList.at(l)->corrL.size();
             qDebug() << QString("corrL num: %1\n").arg(szi);

             if(L!=NULL)
             {
                 delete [] L;
                 delete [] Z;
                 delete [] C;
             }
             L = new double[szi];
             Z = new double[2];
             C = new double[szi*2];

             for(i=0; i<szi; i++)
             {
                 residStm << QString("%1|%2\n").arg(expCorrList.at(l)->kNum[i], 3).arg(expCorrList.at(l)->corrL[i], 12, 'f', 4);
                 L[i] = expCorrList.at(l)->corrL[i];
                 C[i*2] = expCorrList.at(l)->kNum[i];
                 C[i*2+1] = 1;
             }

             slsm(2, szi, Z, C, L);

             qDebug() << QString("%1: %2\t%3\n").arg(expCorrList.at(l)->expSec).arg(Z[0]).arg(Z[1]);
             expResList << QString("%1").arg(expCorrList.at(l)->expSec);
             aResList << QString("%1").arg(Z[0]);
             bResList << QString("%1").arg(Z[1]);

             expVal << expCorrList.at(l)->expSec;
             aVal << Z[0];
             bVal << Z[1];
             residFile.close();

         }

         if(!aplyType)
         {
             sett->setValue("corrL/expList", expResList.join("|"));
             sett->setValue("corrL/aList", aResList.join("|"));
             sett->setValue("corrL/bList", bResList.join("|"));
             sett->sync();
         }

         szi = expVal.size();
         /*if(L!=NULL)
         {
             delete [] L;
             delete [] Z;
             delete [] C;
         }*/
         double *aL, *aZ, *aC, *aW, *Da;
         double uweA, uweB;
         aL = new double[szi];
         aZ = new double[2];
         aC = new double[szi*2];
         aW = new double[szi];
         Da = new double[4];
         for(i=0; i<szi; i++)
         {
             aL[i] = aVal[i];
             aC[i*2] = expVal[i];
             aC[i*2+1] = 1.0;
             aW[i] = 1.0;
             qDebug() << QString("%1 = %2 + %3\n").arg(aL[i]).arg(aC[i*2]).arg(aC[i*2+1]);
         }
         //lsm(2, szi, aZ, aC, aL, uweA, Da, aW);
         slsm(2, szi, aZ, aC, aL, aW);
         aA = aZ[0];
         aB = aZ[1];

         qDebug() << QString("a: %1\t%2\n").arg(aA).arg(aB);
         qDebug() << QString("uweA: %1\n").arg(uweA);

         /*if(L!=NULL)
         {
             delete [] L;
             delete [] Z;
             delete [] C;
         }*/
         double *bL, *bZ, *bC, *bW, *Db;
         bL = new double[szi];
         bZ = new double[2];
         bC = new double[szi*2];
         bW = new double[szi];
         Db = new double[4];
         for(i=0; i<szi; i++)
         {
             bL[i] = bVal[i];
             bC[i*2] = expVal[i];
             bC[i*2+1] = 1.0;
             bW[i] = 1.0;
             qDebug() << QString("%1 = %2 + %3\n").arg(bL[i]).arg(bC[i*2]).arg(bC[i*2+1]);
         }
         //lsm(2, szi, bZ, bC, bL, uweB, Db, bW);
         slsm(2, szi, bZ, bC, bL, bW);
         bA = bZ[0];
         bB = bZ[1];
         qDebug() << QString("b: %1\t%2\n").arg(bA).arg(bB);
         qDebug() << QString("uweB: %1\n").arg(uweB);

         for(i=0; i<szi; i++)
         {
             qDebug() << QString("%1: %2\t%3\n").arg(expVal[i]).arg(aA*expVal[i]+aB).arg(bA*expVal[i]+bB);
         }

     }

    
    return 0;//a.exec();
}
