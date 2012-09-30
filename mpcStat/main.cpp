#include <QCoreApplication>
//#include <QtGui>
//#include "OpenFileExDlg.h"
//#include "fitsio.h"
#include <iostream>
#include <QtCore>
#include <QDataStream>

//#include "conio.h"
#include "math.h"
#include "./../libs/astro.h"
#include "./../libs/mpcfile.h"
#include "./../libs/comfunc.h"
#include "./../libs/ringpix.h"
#include "./../libs/mpcStats.h"

#include "./../libs/vsfFunc.h"
#include "./../libs/observ.h"
//#include "./../libs/redStat.h"
//#include "./../libs/multidim.h"
//#include "./../libs/vectGrid3D.h"
//#include "gammaf.h"
/*
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
*/

#define CENTER CENTER_BARY
#define SK SK_EKVATOR


int main(int argc, char *argv[])
{  
	///////////////////////////
	//QTranslator translator;
       //translator.load("astrored2_ru");
       //app.installTranslator(&translator);
       /////////////////////////////////
/*         COpenFileExDlg *mainWin = new COpenFileExDlg;
       mainWin->show(); */
       
	
	QCoreApplication app(argc, argv);
    setlocale(LC_NUMERIC, "C");
	
	
   int i, j, sz, mnum;
    QTextStream out_stream;

	QFile fout("./fout.dat");
	fout.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
	
	out_stream.setDevice(&fout);

    QSettings *settings = new QSettings("./mpcStat.ini",QSettings::IniFormat);

//general
    int isObs = settings->value("general/isObs", 1).toInt();
    int isYear = settings->value("general/isYear", 1).toInt();
    int isCatFlag = settings->value("general/isCatFlag", 1).toInt();
    int isObj = settings->value("general/isObj", 1).toInt();
    int isSphere = settings->value("general/isSphere", 1).toInt();
    int isMagn = settings->value("general/isMagn", 1).toInt();

//model
    int isSphMod =  settings->value("model/isSphMod", 0).toInt();
    int isObsMod =  settings->value("model/isObsMod", 0).toInt();
    int isMagMod =  settings->value("model/isMagMod", 0).toInt();
    int isUBmod =  settings->value("model/isUBmod", 0).toInt();


//objCounter
     int objMax = settings->value("objCounter/objMax", 32).toLongLong();


//objSphere
     long nsMax = settings->value("objSphere/nsMax", 32).toLongLong();
     int isEcl = settings->value("objSphere/isEcl", 0).toInt();
     int pMin = settings->value("objSphere/pMin", 10).toInt();
     double dMin = grad2rad(settings->value("objSphere/dMin", -90).toDouble());
     double dMax = grad2rad(settings->value("objSphere/dMax", 90).toDouble());
     int isZonal = settings->value("objSphere/isZonal", 0).toInt();

//obj
     int isSortObj = settings->value("obj/isSortObj", 0).toInt();

//magCounter
     int magNum = settings->value("magCounter/magNum", 1).toInt();
     double mag0 = settings->value("magCounter/mag0", -30).toDouble();
     double mag1 = settings->value("magCounter/mag1", 30).toDouble();

//sphereMod
     QStringList sjList = settings->value("sphereMod/sj", "").toString().split("|");
     QStringList tjList = settings->value("sphereMod/tj", "").toString().split("|");

     int coefNum = sjList.size();
     double *sCoef, *tCoef;

     sCoef = new double[coefNum];
     tCoef = new double[coefNum];

     for(i=0; i<coefNum; i++)
     {
         sCoef[i] = sjList[i].toDouble();
         tCoef[i] = tjList[i].toDouble();
         qDebug() << QString("index: %1:\t%2\t%3\n").arg(indexesStr(i+1)).arg(sCoef[i]).arg(tCoef[i]);
     }

//obsMod
     int numLev = settings->value("obsMod/numLev", 100000).toInt();
     double kParObj = settings->value("obsMod/kParObj", 500).toDouble();
     double bParObj = settings->value("obsMod/bParObj", -100).toDouble();
     double dispObs = settings->value("obsMod/dispObs", 200).toDouble();

//magMod
     int magNorm = settings->value("magMod/magNorm", 17.5).toInt();
     double kParMag = settings->value("magMod/kParMag", 500).toDouble();
     double bParMag = settings->value("magMod/bParMag", 200).toDouble();
//     double dispObs = settings->value("magMod/dispObs", 200).toDouble();
//ubMod
    double kParUB = mas_to_rad(settings->value("ubMod/kPar", 0).toDouble());
     double bParUB = mas_to_rad(settings->value("ubMod/bPar", 0).toDouble());
     QString obsFile = settings->value("ubMod/obsFile", "Obs.txt").toString();
     QString jplFile = settings->value("ubMod/jplFile", "./../../data/cats/binp1940_2020.405").toString();


//	QString workingFolder = settings->value("general/workingFolder").toString();
//	QString outputFolder = settings->value("general/outputFolder").toString();
//	int taskNum = settings->value("general/taskNum").toInt();
/*
//	double velMin = settings->value("task14/velMin").toDouble();
	double velMax = settings->value("general/velMax").toDouble();
	
	int isSysCorr = settings->value("general/isSysCorr").toInt();
    QString sysCorrFile = settings->value("general/sysCorrFile").toString();
        QStringList excNames = settings->value("general/excNames").toString().split("|");
        int expMin = settings->value("general/expMin").toInt();
        int sigmaTest = settings->value("general/sigmaTest", 0).toInt();
*/


     double s1 = sin(dMin);
     double s2 = sin(dMax);
/*
     double rs1 = sin(grad2rad(250));
     double rs2 = sin(grad2rad(300));
*/
        mpcRec mpR;
        QString mpNum, obsCode, catFlag;
        double mjd, magn, Az, hVal, zet, disp1;
        DATEOBS date_obs;
        QString mpcFile(argv[1]);
        double dRa, dDe, rat1, dect1;

        observ oPos;
int oires;
        if(isUBmod)
        {
            oires = oPos.init(obsFile.toAscii().data(), jplFile.toAscii().data());
            if(oires)
            {
                qDebug() << QString("oPos init error\n");
                return 1;
            }
        }



        QList <obsCodeCounter*> obsList;
        QList <yearCounter*> yrList;
        QList <catFlagCounter*> cfList;
        QList <objCounter*> objList;

        int* magnCount = new int[magNum];
        for(i=0; i<magNum;i++) magnCount[i] = 0;


        QFile inFile(mpcFile);
        if(!inFile.open(QIODevice::ReadOnly))
        {
            qDebug() << QString("File %1 not open. stop\n").arg(mpcFile);
            return 1;
        }
        QTextStream inStm(&inFile);

        QFileInfo mpcI(mpcFile);
        QString wDirName = QString(mpcI.absolutePath());

        double dect, rat, lam, beta;
        double x, y, s, lns, z0, z1, disp;
        long ipix, ipixMax;
        int mpNumber;

        //nsMax = 32;//8192;
        QVector <int> iNum;
        QVector <double> vDRA;
        QVector <double> vDDE;



        if(isSphere)
        {
            ipixMax = nsMax*nsMax*12;
            qDebug() << QString("ipix: %1\n").arg(ipixMax);
            iNum.fill(0, ipixMax);
            vDRA.fill(0, ipixMax);
            vDDE.fill(0, ipixMax);
        }
        //int *iNum = new int[ipixMax];
        //for(i=0; i<ipixMax; i++) iNum[i] = 0;
        int oNum=0;

        objCounter *objc = new objCounter;;

srand(time(NULL));
        while(!inStm.atEnd())
        {
            mpR.fromStr(inStm.readLine());
dRa = dDe = 0.0;
            dect = grad2rad(mpR.dec());
            rat = grad2rad(mpR.ra());
            mpR.getMpNumber(mpNum);
            mpR.getObsCode(obsCode);
            mjd = mpR.mjd();
            getDATEOBSfromMJD(&date_obs, mjd);
            mpR.getCatFlag(catFlag);
            mpNumber = mpR.mpNumber();
            magn = mpR.magn();



            if(isObsMod)
            {

            /////////////   dispersion  ////////////////
                    do
                    {
                //           srand(time(NULL));
                        x = 2.0*rand()/(1.0*RAND_MAX) - 1.0;
                        y = 2.0*rand()/(1.0*RAND_MAX) - 1.0;
                        s = x*x + y*y;
                    }while((s==0)||(s>1));
                    lns = sqrt(-2.0*log(s)/s);
                    z0 = x*lns;
                    z1 = y*lns;

                    //zStm << QString("%1|%2|%3|%4|%5|%6\n").arg(x).arg(y).arg(s).arg(lns).arg(z0).arg(z1);

            ////////////////////////////////////////////


                disp = dispObs + kParObj*(mpNumber*1.0/numLev) + bParObj;

                //qDebug() << QString("mpNumber: %4\ndisp: %1\tz0: %2\tz1: %3\n").arg(disp).arg(z0).arg(z1).arg(mpNumber);

                dRa += disp*z0;
                dDe += disp*z1;
            }

            if(isMagMod)
            {

            /////////////   dispersion  ////////////////
                    do
                    {
                //           srand(time(NULL));
                        x = 2.0*rand()/(1.0*RAND_MAX) - 1.0;
                        y = 2.0*rand()/(1.0*RAND_MAX) - 1.0;
                        s = x*x + y*y;
                    }while((s==0)||(s>1));
                    lns = sqrt(-2.0*log(s)/s);
                    z0 = x*lns;
                    z1 = y*lns;

                    //zStm << QString("%1|%2|%3|%4|%5|%6\n").arg(x).arg(y).arg(s).arg(lns).arg(z0).arg(z1);

            ////////////////////////////////////////////


                disp = kParMag*(magn*1.0/magNorm) + bParMag;

                //qDebug() << QString("mpNumber: %4\ndisp: %1\tz0: %2\tz1: %3\n").arg(disp).arg(z0).arg(z1).arg(mpNumber);

                dRa += disp*z0;
                dDe += disp*z1;
            }

            if(isUBmod)
            {

                do
                {
            //           srand(time(NULL));
                    x = 2.0*rand()/(1.0*RAND_MAX) - 1.0;
                    y = 2.0*rand()/(1.0*RAND_MAX) - 1.0;
                    s = x*x + y*y;
                }while((s==0)||(s>1));
                lns = sqrt(-2.0*log(s)/s);
                z0 = x*lns;
                z1 = y*lns;

//////////////////////
                if(oPos.set_obs_parpam(GEOCENTR_NUM, CENTER, SK, obsCode.toAscii().data())) qDebug() << QString("warn obsParam, %1\n").arg(obsCode);
                oPos.det_observ(mjd2jd(mjd));
                detAhnumGC(&Az, &hVal, oPos.obs->stime, oPos.obs->record->Cos, oPos.obs->record->Sin, rat, dect);
                zet = PI/2.0 - hVal;
                disp1 = kParUB*sin(zet) + bParUB;
                if(hVal<0.0) qDebug() << QString("obsCode:%1\thVal: %2\tzet: %3\n").arg(obsCode).arg(rad2grad(hVal)).arg(rad2grad(zet));

//////////////////////

                //dRa += disp1*z0;
                dDe += disp1*z1;
            }




/*
            if(isEcl==1)
            {
                lam = atan2(cos(dect)*sin(rat)*cos(-EKV)-sin(dect)*sin(-EKV), cos(dect)*cos(rat));

                beta = asin(cos(dect)*sin(rat)*sin(-EKV)+sin(dect)*cos(-EKV));

                if(beta>PI/2.0) {lam += PI; beta = PI/2.0 - beta;}
                if(beta<-PI/2.0) {lam += PI; beta = PI/2.0 + beta;}

                if((lam>2.0*PI)) lam -=2.0*PI;
                if((lam<0.0)) lam +=2.0*PI;
                rat = lam;
                dect = beta;

            }
  */
            if(isEcl==1)
            {
                rat1 = rat + mas2rad(dRa)/cos(dect);
                dect1 = dect + mas2rad(dDe);

                lam = atan2(cos(dect)*sin(rat)*cos(-EKV)-sin(dect)*sin(-EKV), cos(dect)*cos(rat));

                beta = asin(cos(dect)*sin(rat)*sin(-EKV)+sin(dect)*cos(-EKV));

                if(beta>PI/2.0) {lam += PI; beta = PI/2.0 - beta;}
                if(beta<-PI/2.0) {lam += PI; beta = PI/2.0 + beta;}

                if((lam>2.0*PI)) lam -=2.0*PI;
                if((lam<0.0)) lam +=2.0*PI;
                rat = lam;
                dect = beta;

                lam = atan2(cos(dect1)*sin(rat1)*cos(-EKV)-sin(dect1)*sin(-EKV), cos(dect1)*cos(rat1));

                beta = asin(cos(dect1)*sin(rat1)*sin(-EKV)+sin(dect1)*cos(-EKV));

                if(beta>PI/2.0) {lam += PI; beta = PI/2.0 - beta;}
                if(beta<-PI/2.0) {lam += PI; beta = PI/2.0 + beta;}

                if((lam>2.0*PI)) lam -=2.0*PI;
                if((lam<0.0)) lam +=2.0*PI;
                rat1 = lam;
                dect1 = beta;

                dRa = rad2mas((rat1 - rat)*cos(dect));
                dDe = rad2mas(dect1 - dect);


            }




            if(dect<dMin||dect>dMax) continue;
            if((magn<mag0||magn>mag1)&&isMagn) continue;

            oNum++;





            if(isObs) addObsCode(obsList, obsCode);
            if(isYear) addYear(yrList, date_obs.year);
            if(isCatFlag) addCatFlag(cfList, catFlag);
            if(isObj)
            {
                if(QString().compare(objc->objNum, mpNum)!=0)
                {
                    objc = new objCounter;
                    objc->objNum = mpNum;
                    objc->count = 1;
                    objList << objc;
                }
                else objc->count++;

            }

            if(isSphMod)
            {

                for(j=0; j<coefNum; j++)
                {

                    dRa += sCoef[j]*SLJ(j+1, rat, dect) + tCoef[j]*TLJ(j+1, rat, dect);
                    dDe += sCoef[j]*SBJ(j+1, rat, dect) + tCoef[j]*TBJ(j+1, rat, dect);
                }
            }

            if(isSphere)
            {




                if(isZonal)
                {
                    dect = asin((2.0*sin(dect)/(s2-s1))-(s2+s1)/(s2-s1));
                    //rat = asin((2.0*sin(rat)/(rs2-rs1))-(rs2+rs1)/(rs2-rs1));
                }


                ang2pix_ring(nsMax, dect+M_PI/2.0, rat, &ipix);
                if(ipix>ipixMax||ipix<0)
                {
                    qDebug() << QString("WARN ipix: %1\n").arg(ipix);
                    continue;
                }
                iNum[ipix]++;

//////////////////////////////


                //qDebug() << QString("dRa: %1\tdDe: %2\n").arg(dRa).arg(dDe);

                vDRA[ipix] += dRa;
                vDDE[ipix] += dDe;

//////////////////////////////
            }

            if(isMagn)
            {
                mnum = ((magn-mag0)/(mag1-mag0))*magNum;
                magnCount[mnum]++;
            }
        }

        qDebug() << QString("mpcNum: %5\nobs: %1\nyears: %2\ncatFlags: %3\nobjects: %4\n").arg(obsList.count()).arg(yrList.count()).arg(cfList.count()).arg(objList.count()).arg(oNum);

QFile resFile;
QTextStream resStm;

        //save obsList
        if(isObs)
        {
            if(isSortObj) sortObsNum(obsList);

            resFile.setFileName(QString("%1/obsCounter.txt").arg(wDirName));

            if(resFile.open(QFile::WriteOnly | QFile::Truncate))
            {
                resStm.setDevice(&resFile);
                sz = obsList.count();
                for(i=0; i<sz; i++)
                {

                    resStm << QString("%1|%2\n").arg(obsList.at(i)->obsCode).arg(obsList.at(i)->count);
                }

                resFile.close();
            }
        }

    //save yrList
        if(isYear)
        {
            sortYear(yrList, -1);

            //resFile.setFileName("./yearCounter.txt");
            resFile.setFileName(QString("%1/yearCounter.txt").arg(wDirName));

            if(resFile.open(QFile::WriteOnly | QFile::Truncate))
            {
                resStm.setDevice(&resFile);
                sz = yrList.count();
                for(i=0; i<sz; i++)
                {

                    resStm << QString("%1|%2\n").arg(yrList.at(i)->year).arg(yrList.at(i)->count);
                }

                resFile.close();
            }
        }


    //save cfList
        if(isCatFlag)
        {

            //resFile.setFileName("./cfCounter.txt");
            resFile.setFileName(QString("%1/cfCounter.txt").arg(wDirName));

            if(resFile.open(QFile::WriteOnly | QFile::Truncate))
            {
                resStm.setDevice(&resFile);
                sz = cfList.count();
                for(i=0; i<sz; i++)
                {

                    resStm << QString("%1|%2\n").arg(cfList.at(i)->catFlag).arg(cfList.at(i)->count);
                }

                resFile.close();
            }
        }

    //save objList
        if(isObj)
        {
            sortObjNum(objList);

            //resFile.setFileName("./objCounter.txt");
            resFile.setFileName(QString("%1/objCounter.txt").arg(wDirName));

            if(resFile.open(QFile::WriteOnly | QFile::Truncate))
            {
                resStm.setDevice(&resFile);
                sz = objList.count();
                for(i=0; i<sz; i++)
                {

                    resStm << QString("%1|%2\n").arg(objList.at(i)->objNum).arg(objList.at(i)->count);
                }

                resFile.close();
            }
        }

    //save sphere

        if(isSphere)
        {

            //resFile.setFileName("./objSphere.txt");
            resFile.setFileName(QString("%1/objSphere.txt").arg(wDirName));

            if(resFile.open(QFile::WriteOnly | QFile::Truncate))
            {
                resStm.setDevice(&resFile);
                for(i=0; i<ipixMax; i++)
                {
                    if(iNum[i]<pMin) continue;// iNum[i]=0;



                    pix2ang_ring( nsMax, i, &dect, &rat);
                    dect = M_PI/2.0-dect;
                    if(isZonal)
                    {
                        dect = asin(0.5*sin(dect)*(s2-s1) + 0.5*(s2+s1));
                        //rat = asin(0.5*sin(rat)*(rs2-rs1) + 0.5*(rs2+rs1));
                    }

                    resStm << QString("%1|%2|%3|%4|%5|%6\n").arg(rat, 15, 'e', 10).arg(dect, 15, 'e', 10).arg(iNum[i], 6).arg(log10(iNum[i]), 8).arg(mas2rad(vDRA[i]/iNum[i]), 16, 'e', 10).arg(mas2rad(vDDE[i]/iNum[i]), 16, 'e', 10);
                }

                resFile.close();
            }
        }

        double mDiap;

        if(isMagn)
        {
            resFile.setFileName(QString("%1/magnCount.txt").arg(wDirName));

            if(resFile.open(QFile::WriteOnly | QFile::Truncate))
            {
                resStm.setDevice(&resFile);
                for(i=0; i<magNum; i++)
                {
                    //if(iNum[i]<pMin) continue;// iNum[i]=0;

                    mDiap = (i+0.5)*((mag1-mag0)/magNum)+mag0;

                    resStm << QString("%1|%2\n").arg(mDiap, 5, 'f', 2).arg(magnCount[i]);
                }

                resFile.close();
            }
        }


	
//	printf("end\n");
//	getch();
	return 0; 
}
	
