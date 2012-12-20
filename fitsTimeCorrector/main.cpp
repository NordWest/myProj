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
    QVector <double> dExp;
    QVector <int> kNum;
};

double detCorr0(double expTime, int k, double aA, double aB, double bA, double bB)
{
    double aExpCorr = aA*expTime + aB;
    double bExpCorr = bA*expTime + bB;
    double expCorr = (aExpCorr*k+bExpCorr)/86400.0;
    return(expCorr);
}

int detCorrL(double &expCorr, double expTime, int expNum, QStringList eAplyL, QStringList aAplyL, QStringList bAplyL)
{
    for(int l=0;l<eAplyL.size();l++)
    {
        if(floor(eAplyL.at(l).toDouble())==floor(expTime))
        {
            expCorr = (aAplyL.at(l).toDouble()*expNum+bAplyL.at(l).toDouble())/86400.0;
            return(1);
            break;
        }
    }
    return 0;
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

    QList <expCorrRec*> expCorrListAply;
    QList <expCorrRec*> expCorrList;
    expCorrRec* ecRec;

    QStringList dirList;
    QStringList dataFiles, filters, wfList;
    QString tFile;
    QDir tDir, rDir;
    int i, j, k, l, lNum, szi, szj, wfSz, expNum;
    double t0, t1, dt;
    fitsdata fitsd;
    QString dateCode0, dateCode1, dateCodeNew, nName, nDirName;
    QFileInfo fi;
    double mjd0, mjd1, dmjd0, dmjd1, mjdN, mjdNend, mjdEnd, mjdBeg, realExp, realMJD, dobsN, dendN, expCorr0;
    QString objName0, objName1, tstr;
    QString nTime;
    long nelements;
    fitsfile *fptr;
    fitsfile *fptr_out;
    int status = 0;
    int isCorr;


    QSettings *sett = new QSettings("./fitsTimeCorrector.ini", QSettings::IniFormat);

    QString workDirName = QDir(sett->value("general/workDir", "./orig").toString()).absolutePath();
    QString resPathName = QDir(sett->value("general/resPath", "./res/").toString()).absolutePath();
    QString goodPathName = QDir(sett->value("general/goodPath", "./resG/").toString()).absolutePath();
    int detCorr = sett->value("general/detCorr", 0).toInt();
    int aplyType = sett->value("general/aplyType", 0).toInt();
    int saveCorr = sett->value("general/saveCorr", 0).toInt();
    double dTcorr = sett->value("general/dTcorr", 0.0).toDouble();

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
             objName0 = objName1;


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

             fitsd.headList.getKeyName("TARGET", &tstr);
             objName1 = tstr.section("\'", 1, 1);
             if(objName1.indexOf("target")!=-1) continue;


             //mjdDateCode_file(&dateCode, fitsd.MJD);
             //qDebug() << QString("dcCur: %1\tdc: %2\n").arg(dateCodeCur).arg(dateCode);

             //if((dmjd1>(fitsd.exptime*1.5/86400.0)||j==szj-1)&&wfList.size()>0)
             if((dmjd1>(fitsd.exptime*2.0/86400.0))||(dmjd1>dmjd0*1.5||j==szj-1||QString().compare(objName1, objName0)!=0))
             {
                 if(wfList.size()>0)
                 {
                     //if(j==szj-1) wfList << tFile;



                     fitsd.clear();
                     if(fitsd.openFile(wfList.at(0))) break;
                     t0 = fitsd.MJD;
                     mjdDateCode_file(&dateCode0, fitsd.MJD);
                     //dateCode = dateCodeCur;
                     //nName = QString("%1%2.fit").arg(resPathName).arg(dateCodeNew);
                     //dateCode = dateCodeCur;
                     switch(aplyType)
                     {
                     case 1:
                         expCorr0 = detCorr0(fitsd.exptime, 0, aAcorr, aBcorr, bAcorr, bBcorr);
                         break;
                     case 2:
                         if(!detCorrL(expCorr0, fitsd.exptime, 0, eAplyL, aAplyL, bAplyL)) expCorr0 = detCorr0(fitsd.exptime, 0, aAcorr, aBcorr, bAcorr, bBcorr);
                         break;
                     }
                     qDebug() << QString("expCorr0: %1\n").arg(expCorr0);


                     dt = (fitsd.exptime-dTcorr)/86400.0;
                     wfSz = wfList.size();
                     if(wfSz>1)
                     {
                        fitsd.clear();
                        if(fitsd.openFile(wfList.at(wfSz-1))) break;
                        mjdDateCode_file(&dateCode1, fitsd.MJD);

                        //if((QString().compare(dateCode0, dateCode1)!=0))


                         qDebug() << QString("wfSz: %1\n").arg(wfSz);
                         for(k=0; k<wfSz; k++)
                         {

                             fitsd.clear();
                             fitsd.openFile(wfList.at(k));
                             //mjdDateCode_file(&dateCodeCur, fitsd.MJD);

                             mjdBeg = fitsd.MJD - fitsd.exptime/86400.0/2.0;

                             tstr = wfList.at(k).section("_", -1, -1).section(".", 0,0);
                             expNum = tstr.toInt()-1;
                             //qDebug() << QString("k: %1\texpNum: %2\ttstr: %3\n").arg(k).arg(expNum).arg(tstr);

                             //mjdN = t0+dt*expNum;
                             dobsN = t0 - fitsd.exptime/86400.0/2.0 + dt*expNum;
                             dendN = t0 + fitsd.exptime/86400.0/2.0 + dt*expNum;

                             switch(aplyType)
                             {
                             case 1:
                                 expCorr = detCorr0(fitsd.exptime, expNum, aAcorr, aBcorr, bAcorr, bBcorr);
                                 dobsN -= expCorr;
                                 dendN -= expCorr;
                                 break;
                             case 2:
                                 if(!detCorrL(expCorr, fitsd.exptime, expNum, eAplyL, aAplyL, bAplyL)) expCorr = detCorr0(fitsd.exptime, expNum, aAcorr, aBcorr, bAcorr, bBcorr);
                                 dobsN -= expCorr;
                                 dendN -= expCorr;
                                 break;
                             }
/*
                             switch(aplyType)
                             {
                                 case 1:
                                 expCorr = detCorr0(fitsd.exptime, expNum, aAcorr, aBcorr, bAcorr, bBcorr);

                                 //mjdN -= expCorr;
                                 dobsN -= expCorr;
                                 dendN -= expCorr;
                                 break;
                                 case 2:
                                 isCorr=0;
                                     for(l=0;l<eAplyL.size();l++)
                                     {
                                         if(floor(eAplyL.at(l).toDouble())==floor(fitsd.exptime))
                                         {
                                             expCorr = (aAplyL.at(l).toDouble()*expNum+bAplyL.at(l).toDouble())/86400.0;
                                             expCorr0 = (aAplyL.at(l).toDouble()*0+bAplyL.at(l).toDouble())/86400.0;
                                             isCorr=1;
                                             break;
                                         }
                                     }
                                     if(!isCorr)
                                     {
                                         expCorr = detCorr0(fitsd.exptime, expNum, aAcorr, aBcorr, bAcorr, bBcorr);
                                         expCorr = detCorr0(fitsd.exptime, 0, aAcorr, aBcorr, bAcorr, bBcorr);
                                     }
                                     //mjdN -= expCorr;
                                     dobsN -= expCorr;
                                     dendN -= expCorr;
                                     break;
                             }
*/
                             mjdN = (dobsN+dendN)/2.0;

                                 getStrTfromMJD(&tstr, mjdBeg);
                                 //qDebug() << QString("DATE-OBS: %1\n").arg(tstr);

                                 fitsd.headList.getKeyName("DATE-END", &tstr);
                                 //qDebug() << QString("DATE-END: %1\n").arg(tstr.section("\'", 1, 1));
                                 mjdEnd = getMJDfromStrT(tstr.section("\'", 1, 1));

                                 mjdNend = fitsd.MJD+fitsd.exptime/86400.0/2.0;


                                 qDebug() << QString("dmjdObs: %1\tdmjdEnd: %2\tdT: %3\n").arg(mjdBeg-dobsN).arg(mjdEnd-dendN).arg((dendN+dobsN)/2.0-mjdN);

                                     //qDebug() << QString("realExp: %1\n").arg(realExp);



                                     if((QString().compare(dateCode0, dateCode1)!=0))
                                     {

                                         if(fabs(mjdEnd-mjdNend)>(fitsd.exptime/86400.0/2.0))
                                         {
                                             mjdEnd = mjdNend+expCorr0;
                                             realExp = (mjdEnd-mjdBeg)*86400.0;
                                             fitsd.MJD = (mjdEnd+mjdBeg)/2.0;
                                            dendCounter++;
                                         }
                                         else
                                         {

                                             realExp = (mjdEnd-mjdBeg)*86400.0;
                                             fitsd.MJD = (mjdEnd+mjdBeg)/2.0;

                                             if(k>0&&k==expNum)
                                             {
                                                 if(detCorr)
                                                 {
                                                     lNum = -1;
                                                     for(l=0; l<expCorrList.size(); l++)
                                                     {
                                                         if(expCorrList.at(l)->expSec==fitsd.exptime)
                                                         {
                                                             lNum = l;
                                                             expCorrList.at(l)->corrL << (mjdN - fitsd.MJD)*86400;
                                                             expCorrList.at(l)->durat << expNum*dt*86400.0;
                                                             expCorrList.at(l)->kNum << expNum;
                                                             expCorrList.at(l)->dExp << realExp-fitsd.exptime;
                                                             break;
                                                         }
                                                     }
                                                     if(lNum==-1)
                                                     {
                                                         ecRec = new expCorrRec;
                                                         ecRec->expSec = fitsd.exptime;
                                                         ecRec->corrL << (mjdN - fitsd.MJD)*86400;
                                                         ecRec->durat << expNum*dt*86400.0;
                                                         ecRec->kNum << expNum;
                                                         ecRec->dExp << realExp-fitsd.exptime;
                                                         expCorrList << ecRec;
                                                     }
                                                 }


                                                 residStm.flush();
                                             }


                                        }

                                         residStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10\n").arg(expNum, 3).arg(expNum*dt*86400.0, 6).arg((mjdN - fitsd.MJD)*86400, 12, 'f', 4).arg(fitsd.MJD, 12, 'f', 6).arg(mjdN, 12, 'f', 6).arg(t0, 12, 'f', 6).arg(fitsd.exptime).arg(realExp, 8, 'f', 3, QLatin1Char(' ')).arg(realExp-fitsd.exptime, 8, 'f', 3).arg(wfList.at(k));

                                 /*mjdDateCode_file(&dateCodeNew, fitsd.MJD);
                                 nName = QString("%1/%2.fit").arg(goodPathName).arg(dateCodeNew);
                                 qDebug() << QString("new file name: %1\n").arg(nName);
                                 fitsd.saveFitsAs(nName);*/
                             }
                             else
                             {
                                 fitsd.MJD = mjdN;

                                 mjdEnd = fitsd.MJD + fitsd.exptime/86400.0/2.0;
                                 mjdDateCode_file(&dateCodeNew, fitsd.MJD);
                                 nName = QString("%1/%2.fit").arg(resPathName).arg(dateCodeNew);
                                 qDebug() << QString("new file name: %1\n").arg(nName);
                                 //fitsd.saveFitsAs(nName);
                             }



                             fitsCounter++;

////////////////////SAVE FITS ////////////////////////////////

/*
                             fitsd.saveFitsAs(nName);
*/

/*
                             QFile().remove(nName);

                             fits_open_file(&fptr, fitsd.fileName.toAscii().data(), READONLY, &status);
                             //if(FD_LOG_LEVEL) qDebug() << QString("%1\topen_old %2\n").arg(fileName).arg(status);
                             status = 0;

                             fits_create_file(&fptr_out, nName.toAscii().data(), &status);
                             //if(FD_LOG_LEVEL) qDebug() << QString("create %1\n").arg(status);
                             status = 0;

                             fits_copy_hdu(fptr, fptr_out, 0, &status);
                             status = 0;

                             getStrTfromMJD(&tstr, mjdBeg);
                             fits_update_key(fptr_out, TSTRING, "DATE-OBS", tstr.toAscii().data(), "UTC of start(Corrected from app)", &status);
                             status = 0;

                             getStrTfromMJD(&tstr, mjdEnd);
                             fits_update_key(fptr_out, TSTRING, "DATE-OBS", tstr.toAscii().data(), "UTC of start(Corrected from app)", &status);
                             status = 0;
*/

///////////////////////////////////////////////////////////////

                             //dateCode = dateCodeCur;
                         }

                         if((QString().compare(dateCode0, dateCode1)==0)) serieCorr++;

                     }
                 }
                 serieTot++;
                 wfList.clear();
             }

             wfList << tFile;
         }
     }

     qDebug() << QString("=============================\nseriesTot: %1\nseriesCorr: %2\nfitsCounter: %3\ndendCounter: %4\n========================================\n").arg(serieTot).arg(serieCorr).arg(fitsCounter).arg(dendCounter);
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
                 residStm << QString("%1|%2|%3|%4\n").arg(expCorrList.at(l)->kNum[i], 3).arg(expCorrList.at(l)->corrL[i], 12, 'f', 4).arg(expCorrList.at(l)->durat[i], 8, 'f', 4).arg(expCorrList.at(l)->dExp[i], 8, 'f', 4);
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

         if(saveCorr)
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

         if(saveCorr)
         {
             sett->setValue("corr0/aA", aA);
             sett->setValue("corr0/aB", aB);
             sett->setValue("corr0/bA", bA);
             sett->setValue("corr0/bB", bB);
             sett->sync();
         }

     }

    
    return 0;//a.exec();
}
