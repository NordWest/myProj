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
    if(k<0) return 0.0;
    double aExpCorr = aA*expTime + aB;
    double bExpCorr = bA*expTime + bB;
    qDebug() << QString("a: %1\tb:%2\n").arg(aExpCorr).arg(bExpCorr);
    double expCorr = (aExpCorr*k+bExpCorr)/86400.0;
    return(expCorr);
}

int detCorrL(double &expCorr, double expTime, int expNum, QStringList eAplyL, QStringList aAplyL, QStringList bAplyL)
{
    if(expNum<0)
    {
        expCorr=0.0;
        return 0;
    }
    for(int l=0;l<eAplyL.size();l++)
    {
        if(floor(eAplyL.at(l).toDouble())==floor(expTime))
        {
            expCorr = (aAplyL.at(l).toDouble()*expNum+bAplyL.at(l).toDouble())/86400.0;
            return(0);
            break;
        }
    }
    return 1;
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
    int i, j, k, l, lNum, szi, szj, wfSz, expNum, sTarg;
    double t0, t1, dt;
    fitsdata fitsd;
    QString dateCode0, dateCode1, dateCodeNew, nName, nDirName;
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


    QSettings *sett = new QSettings("./ftc.ini", QSettings::IniFormat);

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


     QFile residFile;
     residFile.setFileName("./timeRes.txt");
     residFile.open(QFile::WriteOnly | QFile::Truncate);
     QTextStream residStm(&residFile);

     szi = dirList.size();
     for(i=0; i<szi; i++)
     {
        //skip dark dir
         if(dirList.at(i).indexOf("dark")!=-1||dirList.at(i).indexOf("flat")!=-1) continue;
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
             mjd0 = mjd1;
             dmjd0 = dmjd1;
             objName0 = objName1;



             sTarg = 0;
             tFile = QString("%1/%2").arg(dirList.at(i)).arg(dataFiles.at(j));




             fitsd.clear();
             if(fitsd.openFile(tFile))
             {
                 qDebug() << QString("err open file: %1\n").arg(tFile);
                 brCounter++;
                 continue;
             }



             fitsd.headList.getKeyName("TARGET", &tstr);
             objName1 = tstr.section("\'", 1, 1);
             if(objName1.indexOf("target")!=-1)
             {
                 qDebug() << QString("skip target: %1\n").arg(objName1);
                 //target miss counter
                 brCounter++;
                 sTarg = 1;
                 //continue;
             }
             else
             {
                 mjd1 = fitsd.MJD;
                 dmjd1 = fabs(mjd1-mjd0);
             }


             //mjdDateCode_file(&dateCode, fitsd.MJD);
             //qDebug() << QString("dcCur: %1\tdc: %2\n").arg(dateCodeCur).arg(dateCode);

             //if((dmjd1>(fitsd.exptime*1.5/86400.0)||j==szj-1)&&wfList.size()>0)
             //if((dmjd1>(fitsd.exptime*2.5/86400.0))||(dmjd1>dmjd0*1.5||j==szj-1||QString().compare(objName1, objName0)!=0))
             //if(((dmjd1>(fitsd.exptime*1.5/86400.0))&&(dmjd1>dmjd0*1.5))||j==szj-1||QString().compare(objName1, objName0)!=0)
             if((dmjd1>(fitsd.exptime*1.5/86400.0))||(dmjd1>dmjd0*1.5)||j==szj-1||QString().compare(objName1, objName0)!=0)
             {
                 if(wfList.size()>0)
                 {
                     if(QString().compare(objName1, objName0)==0&&j==szj-1) wfList << tFile;



                     fitsd.clear();
                     if(fitsd.openFile(wfList.at(0)))
                     {
                         qDebug() << QString("break: err open file %1\n").arg(wfList.at(0));
                         brCounter++;
                         break;
                     }
                     t0 = fitsd.MJD;

                     fitsd.headList.getKeyName("DATE-OBS", &tstr);
                     mjdBeg = getMJDfromStrT(tstr.section("\'", 1, 1));
                     mjdDateCode_file(&dateCode0, mjdBeg);
                     //dateCode = dateCodeCur;
                     //nName = QString("%1%2.fit").arg(resPathName).arg(dateCodeNew);
                     //dateCode = dateCodeCur;
                     switch(aplyType)
                     {
                     case 1:
                         expCorr0 = detCorr0(fitsd.exptime, 0, aAcorr, aBcorr, bAcorr, bBcorr);
                         break;
                     case 2:

                         if(detCorrL(expCorr0, fitsd.exptime, 0, eAplyL, aAplyL, bAplyL)) expCorr0 = detCorr0(fitsd.exptime, 0, aAcorr, aBcorr, bAcorr, bBcorr);
                         break;
                     default:
                         expCorr0 = dTcorr/86400.0;
                     }
                     qDebug() << QString("expCorr0: %1\n").arg(expCorr0*86400.0);


                     dt = fitsd.exptime/86400.0;
                     wfSz = wfList.size();
                     qDebug() << QString("wfSz: %1\n").arg(wfSz);
                     if(wfSz>1)
                     {
                        fitsd.clear();
                        if(fitsd.openFile(wfList.at(wfSz-1)))
                        {
                            qDebug() << QString("break: err open file %1\n").arg(wfList.at(wfSz-1));
                            brCounter++;
                            break;
                        }
                        fitsd.headList.getKeyName("DATE-OBS", &tstr);
                        mjdBeg = getMJDfromStrT(tstr.section("\'", 1, 1));
                        mjdDateCode_file(&dateCode1, mjdBeg);

                        //if((QString().compare(dateCode0, dateCode1)!=0))


                         //qDebug() << QString("wfSz: %1\n").arg(wfSz);
                 for(k=0; k<wfSz; k++)
                 {

                     fitsd.clear();
                     fitsd.openFile(wfList.at(k));
                     //mjdDateCode_file(&dateCodeCur, fitsd.MJD);

                     //qDebug() << QString("fitsHeader:\n%1").arg(fitsd.fitsHeader);
                     fitsd.headList.getKeyName("DATE-OBS", &tstr);
                     mjdBeg = getMJDfromStrT(tstr.section("\'", 1, 1));
                     //mjdBeg = fitsd.MJD - fitsd.exptime/86400.0/2.0;


                     if(detCorr==2) expNum=k;
                     else
                     {
                         tstr = wfList.at(k).section("_", -1, -1).section(".", 0,0);
                         expNum = tstr.toInt()-1;
                     }
                     //qDebug() << QString("k: %1\texpNum: %2\ttstr: %3\n").arg(k).arg(expNum).arg(tstr);

                     mjdN = t0+dt*expNum;
                     //dobsN = t0 - fitsd.exptime/86400.0/2.0 + dt*expNum;
                     //dendN = t0 + fitsd.exptime/86400.0/2.0 + dt*expNum;

                     if(expNum)
                     {
                         switch(aplyType)
                         {
                         case 1:

                             //expCorr = detCorr0(fitsd.exptime, expNum-1, aAcorr, aBcorr, bAcorr, bBcorr);
                             //dobsN -= expCorr;
                             expCorr = detCorr0(fitsd.exptime, expNum, aAcorr, aBcorr, bAcorr, bBcorr);
                             mjdN -= expCorr;
                             //dendN -= expCorr0;
                             break;
                         case 2:
                             //if(detCorrL(expCorr, fitsd.exptime, expNum-1, eAplyL, aAplyL, bAplyL)) expCorr = detCorr0(fitsd.exptime, expNum-1, aAcorr, aBcorr, bAcorr, bBcorr);
                             //dobsN -= expCorr;
                             if(detCorrL(expCorr, fitsd.exptime, expNum, eAplyL, aAplyL, bAplyL)) expCorr = detCorr0(fitsd.exptime, expNum, aAcorr, aBcorr, bAcorr, bBcorr);
                             mjdN -= expCorr;
                             //dendN -= expCorr0;
                             break;
                         default:
                             expCorr = 0;
                         }
                         dobsN = mjdN-(dt+expCorr0)/2.0;
                         dendN = mjdN+(dt+expCorr0)/2.0;
                     }
                     else
                     {
                         dobsN = mjdN-dt/2.0;
                         dendN = mjdN+dt/2.0+expCorr0;
                     }
                     qDebug() << QString("expCorr: %1\n").arg(expCorr*86400.0);

                     //mjdN = (dobsN+dendN)/2.0;




                     fitsd.headList.getKeyName("DATE-END", &tstr);
                     mjdEnd = getMJDfromStrT(tstr.section("\'", 1, 1));

                     mjdNend = fitsd.MJD+fitsd.exptime/86400.0/2.0;



                 if((QString().compare(dateCode0, dateCode1)!=0))
                 {

                     if(fabs(mjdEnd-mjdNend)>(fitsd.exptime/86400.0))
                     {
                         mjdEnd = mjdNend+expCorr0;
                         realExp = (mjdEnd-mjdBeg)*86400.0;
                         fitsd.MJD = (mjdEnd+mjdBeg)/2.0;
                        dendCounter++;
                        fitsType = 1;
                     }
                     else
                     {

                         realExp = (mjdEnd-mjdBeg)*86400.0;
                         fitsd.MJD = (mjdEnd+mjdBeg)/2.0;
                         fitsType = 0;

                         if(k>0&&k==expNum)
                         {
                             if(detCorr)
                             {
                                 lNum = -1;
                                 for(l=0; l<expCorrList.size(); l++)
                                 {
                                     if(expCorrList.at(l)->expSec==(int)fitsd.exptime)
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




                             residStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10\n").arg(expNum, 3).arg(expNum*dt*86400.0, 6).arg((mjdN - fitsd.MJD)*86400, 12, 'f', 4).arg(fitsd.MJD, 12, 'f', 6).arg(mjdN, 12, 'f', 6).arg(t0, 12, 'f', 6).arg(fitsd.exptime).arg(realExp, 8, 'f', 3, QLatin1Char(' ')).arg(realExp-fitsd.exptime, 8, 'f', 3).arg(wfList.at(k));residStm.flush();
                         }
                     }
                 }
                 else
                 {
                     mjdBeg = dobsN;//mjdN - fitsd.exptime/86400.0/2.0;
                     mjdEnd = dendN;// fitsd.MJD - fitsd.exptime/86400.0/2.0;
                     fitsType = 2;
                 }

                 fitsCounter++;

////////////////////SAVE FITS ////////////////////////////////

                 fitsd.MJD = (mjdBeg+mjdEnd)/2.0;
                 mjdDateCode_file(&dateCodeNew, mjdBeg);
                 nName = QString("%1/%2.fit").arg(nDirName).arg(dateCodeNew);
                 qDebug() << QString("new file name: %1\tTIMECORR: %2\n").arg(nName).arg(fitsType);

                 if(saveFits)
                 {
                     QFile().remove(nName);

                     fits_open_file(&fptr, fitsd.fileName.toAscii().data(), READONLY, &status);
                     if(status)
                     {
                         qDebug() << QString("%1\topen_old %2\n").arg(fitsd.fileName).arg(status);
                         exit(1);
                     }
                     status = 0;

                     fits_create_file(&fptr_out, nName.toAscii().data(), &status);
                     if(status)
                     {
                         qDebug() << QString("create %1\n").arg(status);
                         exit(1);
                     }
                     status = 0;

                     fits_copy_hdu(fptr, fptr_out, 0, &status);
                     status = 0;

                     naxes[0] = fitsd.imgArr->naxes[0];
                     naxes[1] = fitsd.imgArr->naxes[1];
                     nelements = fitsd.imgArr->getNelements();

                     fits_write_img(fptr_out, TUSHORT, 1, naxes[0]*naxes[1]+1, (void*) fitsd.imgArr->ushD, &status);
                     if(status)
                     {
                         qDebug() << QString("write_img %1\n").arg(status);
                         exit(1);
                     }
                     status=0;

                     if(fitsType==2)
                     {
                         getStrTfromMJD(&tstr, mjdBeg);
                         fits_update_key(fptr_out, TSTRING, "DATE-OBS", tstr.toAscii().data(), "UTC of start(Corrected from app)", &status);
                         status = 0;
                     }

                     if(fitsType>0)
                     {
                        getStrTfromMJD(&tstr, mjdEnd);
                        fits_update_key(fptr_out, TSTRING, "DATE-END", tstr.toAscii().data(), "UTC of end(Corrected from app)", &status);
                        status = 0;
                     }

                     fits_write_key(fptr_out, TINT, "TIMECORR",(void*) &fitsType, "Time correction key", &status);


                     fits_close_file(fptr, &status);
                     status=0;
                     fits_close_file(fptr_out, &status);
                     status=0;
                }


///////////////////////////////////////////////////////////////

                             //dateCode = dateCodeCur;
                         }

                         if((QString().compare(dateCode0, dateCode1)==0)) serieCorr++;

                     }

                     serieTot++;
                 }

                 wfList.clear();
                 //if(j!=fitsCounter) qDebug() << QString("\n\nWARN:%1\n\n").arg(j-fitsCounter);
             }

             if(!sTarg)
             {
                 wfList << tFile;
                 qDebug() << QString("%1:tFile: %2\n").arg(j).arg(tFile);

             }
             else
             {
                 qDebug() << QString("skip target: %1\n").arg(objName1);
                 brCounter++;
             }

         }
     }

     qDebug() << QString("=============================\nseriesTot: %1\nseriesCorr: %2\nfitsCounter: %3\ndendCounter: %4\nbreaks: %5\n========================================\n").arg(serieTot).arg(serieCorr).arg(fitsCounter).arg(dendCounter).arg(brCounter);
     residFile.close();

     if(detCorr)
     {
         QStringList expResList, aResList, bResList;
         lNum = expCorrList.size();
         QVector <double> expVal, aVal, bVal;
         double aA, aB, bA, bB, uwe;
         double *L, *Z, *C, *D;
         int *excI;
         int rn;
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
                 delete [] D;
                 delete [] excI;
             }
             L = new double[szi];
             Z = new double[2];
             D = new double[4];
             C = new double[szi*2];
             excI = new int[szi];

             for(i=0; i<szi; i++)
             {
                 residStm << QString("%1|%2|%3|%4\n").arg(expCorrList.at(l)->kNum[i], 3).arg(expCorrList.at(l)->corrL[i], 12, 'f', 4).arg(expCorrList.at(l)->durat[i], 8, 'f', 4).arg(expCorrList.at(l)->dExp[i], 8, 'f', 4);
                 L[i] = expCorrList.at(l)->corrL[i];
                 C[i*2] = expCorrList.at(l)->kNum[i];
                 C[i*2+1] = 1;
             }

             //slsm(2, szi, Z, C, L);
             iLSM(2, szi, 10, excI, Z, C, L, uwe, D, 3.0, rn);

             qDebug() << QString("%1: %2\t%3\n").arg(expCorrList.at(l)->expSec).arg(Z[0]).arg(Z[1]);
             qDebug() << QString("uwe: %1\trn: %2\n").arg(uwe).arg(rn);
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
