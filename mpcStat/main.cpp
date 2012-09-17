#include <QApplication>
#include <QtGui>
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
//#include "./../libs/redStat.h"
//#include "./../libs/multidim.h"
//#include "./../libs/vectGrid3D.h"
//#include "gammaf.h"
/*
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
*/



int main(int argc, char *argv[])
{  
	///////////////////////////
	//QTranslator translator;
       //translator.load("astrored2_ru");
       //app.installTranslator(&translator);
       /////////////////////////////////
/*         COpenFileExDlg *mainWin = new COpenFileExDlg;
       mainWin->show(); */
       
	
	QApplication app(argc, argv);
    setlocale(LC_NUMERIC, "C");
	
	
    int i, sz;
    QTextStream out_stream;

	QFile fout("./fout.dat");
	fout.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
	
	out_stream.setDevice(&fout);

    QSettings *settings = new QSettings("./mpcStat.ini",QSettings::IniFormat);

    int isObs = settings->value("general/isObs", 1).toInt();
    int isYear = settings->value("general/isYear", 1).toInt();
    int isCatFlag = settings->value("general/isCatFlag", 1).toInt();
    int isObj = settings->value("general/isObj", 1).toInt();
    int isSphere = settings->value("general/isSphere", 1).toInt();

    //objSphere
     long nsMax = settings->value("objSphere/nsMax", 32).toLongLong();
     int isEcl = settings->value("objSphere/isEcl", 0).toInt();
     int pMin = settings->value("objSphere/pMin", 10).toInt();
     double dMin = grad2rad(settings->value("objSphere/dMin", -90).toDouble());
     double dMax = grad2rad(settings->value("objSphere/dMax", 90).toDouble());
     int isZonal = settings->value("objSphere/isZonal", 0).toInt();
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

        mpcRec mpR;
        QString mpNum, obsCode, catFlag;
        double mjd;
        DATEOBS date_obs;
        QString mpcFile(argv[1]);



        QList <obsCodeCounter*> obsList;
        QList <yearCounter*> yrList;
        QList <catFlagCounter*> cfList;
        QList <objCounter*> objList;


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
        long ipix, ipixMax;

        //nsMax = 32;//8192;
        QVector <int> iNum;
        //QVector <> deRA;

        if(isSphere)
        {
            ipixMax = nsMax*nsMax*12;
            iNum.fill(0, ipixMax);
        }
        //int *iNum = new int[ipixMax];
        //for(i=0; i<ipixMax; i++) iNum[i] = 0;
        int oNum=0;


        while(!inStm.atEnd())
        {
            mpR.fromStr(inStm.readLine());

            dect = grad2rad(mpR.dec());
            rat = grad2rad(mpR.ra());
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


            if(dect<dMin||dect>dMax) continue;

            oNum++;

            mpR.getMpNumber(mpNum);
            mpR.getObsCode(obsCode);
            mjd = mpR.mjd();
            getDATEOBSfromMJD(&date_obs, mjd);
            mpR.getCatFlag(catFlag);



            if(isObs) addObsCode(obsList, obsCode);
            if(isYear) addYear(yrList, date_obs.year);
            if(isCatFlag) addCatFlag(cfList, catFlag);
            if(isObj) addObjNum(objList, mpNum);

            if(isSphere)
            {



                if(isZonal) dect = asin((2.0*sin(dect)/(s2-s1))-(s2+s1)/(s2-s1));


                ang2pix_ring(nsMax, dect+M_PI/2.0, rat, &ipix);
                if(ipix>ipixMax||ipix<0) qDebug() << QString("WARN ipix: %1\n").arg(ipix);
            else iNum[ipix]++;
            }
        }

        qDebug() << QString("mpcNum: %5\nobs: %1\nyears: %2\ncatFlags: %3\nobjects: %4\n").arg(obsList.count()).arg(yrList.count()).arg(cfList.count()).arg(objList.count()).arg(oNum);

QFile resFile;
QTextStream resStm;

        //save obsList
        if(isObs)
        {
            sortObsNum(obsList);

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
                    dect = dect-M_PI/2.0;
                    if(isZonal) dect = asin(0.5*sin(dect)*(s2-s1) + 0.5*(s2+s1));

                    resStm << QString("%1|%2|%3\n").arg(rat, 13, 'e', 8).arg(dect, 13, 'e', 8).arg(iNum[i]);
                }

                resFile.close();
            }
        }


	
//	printf("end\n");
//	getch();
	return 0; 
}
	
