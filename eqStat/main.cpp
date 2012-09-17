#include <QtCore>

#include "math.h"
#include "./../libs/astro.h"
#include "./../libs/mpcfile.h"
#include "./../libs/comfunc.h"
#include "./../libs/ringpix.h"
#include "./../libs/mpcStats.h"
#include "./../libs/redStat.h"
#include "./../libs/observ.h"


/*
struct objStat
{
    QString objName;
    QVector <double>
};

struct obsStat
{
    QList <>
};

*/

#define CENTER CENTER_BARY
#define SK SK_EKVATOR

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    setlocale(LC_NUMERIC, "C");

    int i, sz;
    QTextStream out_stream;

    QFile fout("./fout.dat");
    fout.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);

    out_stream.setDevice(&fout);

    QSettings *settings = new QSettings("./eqStat.ini",QSettings::IniFormat);

    int isObs = settings->value("general/isObs", 1).toInt();
    int isYear = settings->value("general/isYear", 1).toInt();
    int isCatFlag = settings->value("general/isCatFlag", 1).toInt();
    int isObj = settings->value("general/isObj", 1).toInt();
    int isSphere = settings->value("general/isSphere", 1).toInt();
    int sphereMod = settings->value("general/sphereMod", 0).toInt();
    QString obsFile = settings->value("general/obsFile", "Obs.txt").toString();
    QString jplFile = settings->value("general/jplFile", "./../../data/cats/binp1940_2020.405").toString();

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
     double *Eps = new double[3];
     Eps[0] = mas_to_rad(settings->value("rotation/w1", 0).toDouble());
     Eps[1] = mas_to_rad(settings->value("rotation/w2", 0).toDouble());
     Eps[2] = mas_to_rad(settings->value("rotation/w3", 0).toDouble());
     double *mMatr = new double[9];
     mMatr[0] = mas_to_rad(settings->value("rotation/M11", 0).toDouble());
     mMatr[1] = mas_to_rad(settings->value("rotation/M12", 0).toDouble());
     mMatr[2] = mas_to_rad(settings->value("rotation/M13", 0).toDouble());
     mMatr[3] = mas_to_rad(settings->value("rotation/M21", 0).toDouble());
     mMatr[4] = mas_to_rad(settings->value("rotation/M22", 0).toDouble());
     mMatr[5] = mas_to_rad(settings->value("rotation/M23", 0).toDouble());
     mMatr[6] = mas_to_rad(settings->value("rotation/M31", 0).toDouble());
     mMatr[7] = mas_to_rad(settings->value("rotation/M32", 0).toDouble());
     mMatr[8] = mas_to_rad(settings->value("rotation/M33", 0).toDouble());
     double disp = mas_to_rad(settings->value("rotation/disp", 0).toDouble());
     double kPar = mas_to_rad(settings->value("rotation/kPar", 0).toDouble());
     double bPar = mas_to_rad(settings->value("rotation/bPar", 0).toDouble());



     double s1 = sin(dMin);
     double s2 = sin(dMax);

        ocRecO ocR;
        QString tStr, obsCode, catName, objName;
        double mjd, disp1;

        DATEOBS date_obs;
        QString mpcFile(argv[1]);

        observ oPos;

        int oires = oPos.init(obsFile.toAscii().data(), jplFile.toAscii().data());
        if(oires)
        {
            qDebug() << QString("oPos init error\n");
            return 1;
        }


        QList <obsCodeCounter*> obsList;
        QList <yearCounter*> yrList;
        QList <catFlagCounter*> cfList;
        QList <objCounter*> objList;


        QFile inFile(mpcFile);
        if(!inFile.open(QIODevice::ReadOnly | QFile::Text))
        {
            qDebug() << QString("File %1 not open. stop\n").arg(mpcFile);
            return 1;
        }
        QTextStream inStm(&inFile);

        QFileInfo mpcI(mpcFile);
        QString wDirName = QString(mpcI.absolutePath());

        double dect, rat, lam, beta;
        double Az, hVal, zet, lns;
        double oc_ra, oc_de;
        double objR[2];
        double x, y, s, z0, z1;
        long ipix, ipixMax;

        //nsMax = 32;//8192;
        QVector <int> iNum;
        QVector <double> ocRA;
        QVector <double> ocDE;

        if(isSphere)
        {
            ipixMax = nsMax*nsMax*12;
            iNum.fill(0, ipixMax);
            ocRA.fill(0.0, ipixMax);
            ocDE.fill(0.0, ipixMax);
        }
        //int *iNum = new int[ipixMax];
        //for(i=0; i<ipixMax; i++) iNum[i] = 0;
        long oNum=0;
srand(time(NULL));

        while(!inFile.atEnd())
        {
            tStr = inStm.readLine();
            //if(tStr==0) break;

            ocR.s2rec(tStr);

            dect = grad2rad(ocR.de);
            rat = grad2rad(ocR.ra);

            objName = ocR.name;//.getMpNumber(mpNum);
            obsCode = ocR.obsCode;
            mjd = ocR.MJday;
            getDATEOBSfromMJD(&date_obs, mjd);
            catName = ocR.catName;

            if(sphereMod)
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


                objR[0] = -(sin(dect)*cos(rat))*Eps[0]+(-(sin(dect)*sin(rat)))*Eps[1]+(cos(dect))*Eps[2] - mMatr[2]*sin(dect)*sin(rat) + mMatr[5]*sin(dect)*cos(rat) + mMatr[1]*cos(dect)*cos(2.0*rat) - 0.5*mMatr[0]*cos(dect)*sin(2.0*rat) + 0.5*mMatr[4]*cos(dect)*sin(3.0*rat) + disp*z0;
                objR[1] = sin(rat)*Eps[0]+(-cos(rat))*Eps[1] - 0.5*mMatr[1]*sin(2.0*dect)*sin(2.0*rat) + mMatr[2]*cos(2.0*dect)*cos(rat) + mMatr[5]*cos(2.0*dect)*sin(rat) - 0.5*mMatr[0]*sin(2.0*dect)*pow(cos(rat), 2.0) - 0.5*mMatr[4]*sin(2.0*dect)*pow(sin(rat), 2.0) + 0.5*mMatr[8]*sin(2.0*dect) + disp*z1;

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
                if(oPos.set_obs_parpam(GEOCENTR_NUM, CENTER, SK, obsCode.toAscii().data())) qDebug() << QString("warn obsParam\n");
                oPos.det_observ(mjd2jd(mjd));
                detAhnumGC(&Az, &hVal, oPos.obs->stime, oPos.obs->record->Cos, oPos.obs->record->Sin, rat, dect);
                zet = PI/2.0 - hVal;
                disp1 = kPar*zet + bPar;
                qDebug() << QString("obsCode:%1\thVal: %2\tzet: %3\n").arg(obsCode).arg(rad2grad(hVal)).arg(rad2grad(zet));

//////////////////////

                objR[0] += disp1*z0;
                objR[1] += disp1*z1;

                oc_ra = objR[0] - rat;
                oc_de = objR[1] - dect;
            }
            else
            {
                oc_ra = ocR.ocRaCosDe;
                oc_de = ocR.ocDe;
            }

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


            if((dect<dMin)||(dect>dMax))
            {
                //qDebug() << QString("%1\t%2\t%3\n").arg(dMin).arg(dect).arg(dMax);
                continue;
            }

oNum++;
qDebug() << QString("oNum: %1\n").arg(oNum);
/*
            objName = ocR.name;//.getMpNumber(mpNum);
            obsCode = ocR.obsCode;
            mjd = ocR.MJday;
            getDATEOBSfromMJD(&date_obs, mjd);
            catName = ocR.catName;
//            oc_ra = ocR.ocRaCosDe;
//            oc_de = ocR.ocDe;
*/

            if(isObs) addObsCode(obsList, obsCode);
            if(isYear) addYear(yrList, date_obs.year);
            if(isCatFlag) addCatFlag(cfList, catName);
            if(isObj) addObjNum(objList, objName);

            if(isSphere)
            {



                if(isZonal) dect = asin((2.0*sin(dect)/(s2-s1))-(s2+s1)/(s2-s1));


                ang2pix_ring(nsMax, dect+M_PI/2.0, rat, &ipix);
                if(ipix>ipixMax||ipix<0) qDebug() << QString("WARN ipix: %1\n").arg(ipix);
            else
                {
                    iNum[ipix]++;


/////////////////////////
                    if(sphereMod)
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


                        objR[0] = -(sin(dect)*cos(rat))*Eps[0]+(-(sin(dect)*sin(rat)))*Eps[1]+(cos(dect))*Eps[2] - mMatr[2]*sin(dect)*sin(rat) + mMatr[5]*sin(dect)*cos(rat) + mMatr[1]*cos(dect)*cos(2.0*rat) - 0.5*mMatr[0]*cos(dect)*sin(2.0*rat) + 0.5*mMatr[4]*cos(dect)*sin(3.0*rat) + disp*z0;
                        objR[1] = sin(rat)*Eps[0]+(-cos(rat))*Eps[1] - 0.5*mMatr[1]*sin(2.0*dect)*sin(2.0*rat) + mMatr[2]*cos(2.0*dect)*cos(rat) + mMatr[5]*cos(2.0*dect)*sin(rat) - 0.5*mMatr[0]*sin(2.0*dect)*pow(cos(rat), 2.0) - 0.5*mMatr[4]*sin(2.0*dect)*pow(sin(rat), 2.0) + 0.5*mMatr[8]*sin(2.0*dect) + disp*z1;

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
                        oPos.set_obs_parpam(GEOCENTR_NUM, CENTER, SK, obsCode.toAscii().data());
                        oPos.det_observ(mjd2jd(mjd));
                        detAhnumGC(&Az, &hVal, oPos.obs->stime, oPos.obs->record->Cos, oPos.obs->record->Sin, rat, dect);
                        zet = PI/2.0 - hVal;
                        disp1 = kPar*zet + bPar;
                        qDebug() << QString("zet: %1\n").arg(rad2grad(zet));

    //////////////////////

                        objR[0] += disp1*z0;
                        objR[1] += disp1*z1;

                        oc_ra = objR[0] - rat;
                        oc_de = objR[1] - dect;
                    }



/////////////////////////


                    ocRA[ipix] += oc_ra;
                    ocDE[ipix] += oc_de;
                }
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
                    oc_ra = mas2rad(ocRA[i]/(1.0*iNum[i]));
                    oc_de = mas2rad(ocDE[i]/(1.0*iNum[i]));

                    if(sqrt(oc_ra*oc_ra + oc_de*oc_de)>mas2rad(1000)) continue;


                    pix2ang_ring( nsMax, i, &dect, &rat);
                    dect = dect-M_PI/2.0;
                    if(isZonal) dect = asin(0.5*sin(dect)*(s2-s1) + 0.5*(s2+s1));




                    resStm << QString("%1|%2|%3|%4|%5|%6|%7\n").arg(rat, 13, 'e', 8).arg(dect, 13, 'e', 8).arg(oc_ra*10000+rat, 13, 'e', 8).arg(oc_de*10000+dect, 13, 'e', 8).arg(oc_ra, 13, 'e', 8).arg(oc_de, 13, 'e', 8).arg(iNum[i]);
                }

                resFile.close();
            }
        }
    
    return 0;//a.exec();
}
