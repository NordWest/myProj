#include <QtCore/QCoreApplication>
#include "./../../libs/redStat.h"
#include "./../../mb/mb.h"
#include "./../../astro/astro.h"

double mjd2year(double mjd)
{
    DATEOBS date_obs;
    getDATEOBSfromMJD(&date_obs, mjd);
    return(date_obs.year+date_obs.month/12.0+(date_obs.day+date_obs.pday)/365.2425);
}


int main(int argc, char *argv[])
{

    double t0, t1, dt, tt0, tt1, tt, yrP;
    int i, sz, k;
    int tnum;
    double mjd2000 = 51544.5;
    QList <double> lListX, lListY, lListT;
    if(argc<2)
    {
        qDebug() << QString("there must be 1 argements: file.txt\n");
        return 1;
    }

    QSettings sett("orStat.ini", QSettings::IniFormat);
    int taskNum = sett.value("general/taskNum", 0).toInt();
    double timeSpan = sett.value("general/timeSpan", 1).toDouble();
    double mag0 = sett.value("general/mag0", 1).toDouble();
    double mag1 = sett.value("general/mag1", 1).toDouble();
    t0 = sett.value("general/t0", 32551.0).toDouble();
    t1 = sett.value("general/t1", 47000.0).toDouble();

    //t0 = jd2mjd(2432551.5);
    //t1 = 47000.0;
    dt = timeSpan*365.2425;
    tnum = 1+(t1-t0)/dt;
    switch(taskNum)
    {
        case 0:
        {
            residualFile rFile;
            residualFile* rFileT;
            residualsRec* tRec;
            rFile.init(argv[1]);
            QList <residualFile*> bResList;

            QFile resFile("./resRes.txt");
            resFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QTextStream resStm(&resFile);





            for(i=0;i<tnum;i++)
            {
                rFileT = new residualFile;
                bResList << rFileT;
            }
            sz = rFile.resList.size();
            for(i=0;i<sz;i++)
            {
                tRec = rFile.resList.at(i);
                if((tRec->mag<mag0)||(tRec->mag>mag1)) continue;
                k = (tRec->mJD-t0)/dt;
                if(k>=0&&k<tnum) bResList.at(k)->resList << tRec;
            }
            for(i=0;i<tnum;i++)
            {
                bResList.at(i)->remSigma(3.0);
                if(bResList.at(i)->resList.size()>20)
                {
                    bResList.at(i)->detStat();
                    tt = t0+dt*(i+0.5);

                    resStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9\n").arg(tt).arg(mjd2year(tt)).arg(bResList.at(i)->resList.size()).arg(bResList.at(i)->meanKsi).arg(bResList.at(i)->meanEta).arg(bResList.at(i)->rmsMeanKsi).arg(bResList.at(i)->rmsMeanEta).arg(bResList.at(i)->rmsOneKsi).arg(bResList.at(i)->rmsOneEta);
                    lListX << pow(bResList.at(i)->rmsOneKsi,2.0);
                    lListY << pow(bResList.at(i)->rmsOneEta,2.0);
                    lListT << pow(mjd2000-tt, 2.0);
                }
            }

            resFile.close();
        /*
            sz = lListX.size();

            double *Lx,*Ly, *Cx, *Cy, *Zx, *Zy;
            Lx = new double[sz];
            Ly = new double[sz];
            Zx = new double[sz];
            Zy = new double[sz];
            Cx = new double[sz*2];
            Cy = new double[sz*2];

            for(i=0; i<sz; i++)
            {
                Lx[i] = lListX[i];
                Ly[i] = lListY[i];
                Cx[i*2] = 1.0;
                Cx[i*2+1] = lListT[i];
                Cy[i*2] = 1.0;
                Cy[i*2+1] = lListT[i];
            }

            slsm(2, sz, Zx, Cx, Lx);
            qDebug() << QString("\nx: %1 mas %2 mas/yr\n").arg(sqrt(fabs(Zx[0]))).arg(sqrt(fabs(Zx[1]))*365.2425);

            slsm(2, sz, Zy, Cy, Ly);
            qDebug() << QString("y: %1 mas %2 mas/yr\n").arg(sqrt(fabs(Zy[0]))).arg(sqrt(fabs(Zy[1]))*365.2425);
        */
        }
        break;
        case 1:
        {
            //QCoreApplication a(argc, argv);
            objResidualFile iniResFile;
            objResidualFile *tempRF;
            objResRec *tempOR;
            QList <objResidualFile*> rfList;

            iniResFile.init(QString(argv[1]));

            iniResFile.do3sigma(0.0, 3.0);

            //sortORList(&iniResFile.ocList);
            for(i=0;i<tnum;i++)
            {
                tempRF = new objResidualFile;
                rfList << tempRF;
            }

            sz = iniResFile.ocList.size();
            //t0 = iniResFile.ocList.first()->mJD;
            //t1 = iniResFile.ocList.last()->mJD;
            //double mjd2000 = 51544.5;

//            dt = atof(argv[3])*365.2425;

            QFile resFile("./orRes.txt");
            resFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QTextStream resStm(&resFile);

            QList <colRec*> cListT;
            //QList <double> lListX, lListY, lListT;
        //    double *x;


            k=0;
            tt0 = t0+dt*k;
            tt1 = t0+dt*(k+1);
            for(i=0; i<sz; i++)
            {
                //if((iniResFile.ocList.at(i)->mag-iniResFile.ocList.at(i)->magOC>12))resStm << QString("%1|%2|%3|%4\n").arg(iniResFile.ocList.at(i)->mJD).arg(iniResFile.ocList.at(i)->mag-iniResFile.ocList.at(i)->magOC).arg(iniResFile.ocList.at(i)->ksiOC).arg(iniResFile.ocList.at(i)->etaOC);
                tempOR = iniResFile.ocList.at(i);
                if((tempOR->mag<mag0)||(tempOR->mag>mag1)) continue;
                k = (tempOR->mJD-t0)/dt;
                if(k>=0&&k<tnum) rfList.at(k)->ocList << tempOR;
            }

            for(i=0;i<tnum;i++)
            {
                tempRF = rfList.at(i);
                    tempRF->do3sigma(0.0, 3.0);
                    //resStm << QString("%1\n").arg(tempRF.ocList.size());
                    if(!tempRF->countCols(&cListT, "0,6,7")&&cListT.at(0)->num>20)
                    {
                        resStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9\n").arg(cListT.at(0)->mean).arg(mjd2year(cListT.at(0)->mean)).arg(cListT.at(0)->num).arg(cListT.at(1)->mean).arg(cListT.at(2)->mean).arg(cListT.at(1)->rmsMean).arg(cListT.at(2)->rmsMean).arg(cListT.at(1)->rmsOne).arg(cListT.at(2)->rmsOne);
                        lListX << pow(cListT.at(1)->rmsOne,2.0);
                        lListY << pow(cListT.at(2)->rmsOne,2.0);
                        lListT << pow(mjd2000-cListT.at(0)->mean, 2.0);
                    }
                    //tempRF.clear();
                    cListT.clear();
                    //k++;
                    //tt0 = t0+dt*k;
                    //tt1 = t0+dt*(k+1);
            }



            resFile.close();
        }
        break;
    case 2:
    {
        errBudgetFile errB;
        errB.init(QString(argv[1]));

    }
    break;
}

////////////////////

    sz = lListX.size();

    double *Lx,*Ly, *Cx, *Cy, *Zx, *Zy;
    double uweX, uweY;
    double *Dx, *Dy;
    int rnX, rnY;
    Lx = new double[sz];
    Ly = new double[sz];
    Zx = new double[sz];
    Zy = new double[sz];
    Cx = new double[sz*2];
    Cy = new double[sz*2];
    Dx = new double[sz*2];
    Dy = new double[sz*2];
    int *excX = new int[sz];
    int *excY = new int[sz];

    for(i=0; i<sz; i++)
    {
        Lx[i] = lListX[i];
        Ly[i] = lListY[i];
        Cx[i*2] = 1.0;
        Cx[i*2+1] = lListT[i];
        Cy[i*2] = 1.0;
        Cy[i*2+1] = lListT[i];
    }

    qDebug() << QString("\ntaskNum: %1").arg(taskNum);
    qDebug() << QString("t: %1 - %2\nyrSpan: %3").arg(t0).arg(t1).arg(timeSpan);

    qDebug() << QString("eqNum: %1\n").arg(sz);

    //slsm(2, sz, Zx, Cx, Lx);
    iLSM(2, sz, 300, excX, Zx, Cx, Lx, uweX, Dx, 3.0, rnX);
    qDebug() << QString("\nx: %1 mas %2 mas/yr\n").arg(sqrt(fabs(Zx[0]))).arg((Zx[1]/fabs(Zx[1]))*sqrt(fabs(Zx[1]))*365.2425);
    qDebug() << QString("rn: %1\tuwe:%2\n").arg(rnX).arg(uweX);

    slsm(2, sz, Zy, Cy, Ly);
    iLSM(2, sz, 300, excY, Zy, Cy, Ly, uweY, Dy, 3.0, rnY);
    qDebug() << QString("y: %1 mas %2 mas/yr\n").arg(sqrt(fabs(Zy[0]))).arg((Zy[1]/fabs(Zy[1]))*sqrt(fabs(Zy[1]))*365.2425);
    qDebug() << QString("rn: %1\tuwe:%2\n").arg(rnY).arg(uweY);




    return 0;//a.exec();
}
//
