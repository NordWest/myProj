#include <QtCore/QCoreApplication>
#include "./../../libs/redStat.h"
#include "./../../mb/mb.h"

/*
int main(int argc, char *argv[])
{
    residualFile rFile;
    residualFile* rFileT;
    residualsRec* tRec;
    rFile.init(argv[1]);
    QList <residualFile*> bResList;

    double t0, t1, dt, tt;
    int i, sz, k, tnum;

    QFile resFile("./resRes.txt");
    resFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream resStm(&resFile);


    t0 = jd2mjd(2432551.5);
    t1 = 47000.0;
    dt = 365.2425*2;
    double mjd2000 = 51544.5;

    tnum = 1+(t1-t0)/dt;
    double *tBoxKsi = new double[tnum];
    double *tBoxEta = new double[tnum];
    int *tBoxNum = new int[tnum];
    QList <double> lListX, lListY, lListT;

    for(i=0;i<tnum;i++)
    {
        rFileT = new residualFile;
        bResList << rFileT;
    }
    sz = rFile.resList.size();
    for(i=0;i<sz;i++)
    {
        tRec = rFile.resList.at(i);
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
            resStm << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(tt).arg(bResList.at(i)->resList.size()).arg(bResList.at(i)->meanKsi).arg(bResList.at(i)->meanEta).arg(bResList.at(i)->rmsMeanKsi).arg(bResList.at(i)->rmsMeanEta).arg(bResList.at(i)->rmsOneKsi).arg(bResList.at(i)->rmsOneEta);
            lListX << pow(bResList.at(i)->rmsOneKsi,2.0);
            lListY << pow(bResList.at(i)->rmsOneEta,2.0);
            lListT << pow(mjd2000-tt, 2.0);
        }
    }

    resFile.close();

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


    return 0;
}
*/
//
int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    objResidualFile iniResFile;
    objResidualFile tempRF;

    iniResFile.init(QString(argv[1]));

    iniResFile.do3sigma(0.0, 3.0);

    sortORList(&iniResFile.ocList);
    double t0, t1, dt, tt0, tt1, tt;
    int i, sz, k;

    sz = iniResFile.ocList.size();
    t0 = iniResFile.ocList.first()->mJD;
    t1 = iniResFile.ocList.last()->mJD;
    double mjd2000 = 51544.5;

    dt = atof(argv[2])*365.2425;

    QFile resFile("./orRes.txt");
    resFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream resStm(&resFile);

    QList <colRec*> cListT;
    QList <double> lListX, lListY, lListT;
//    double *x;

    k=0;
    tt0 = t0+dt*k;
    tt1 = t0+dt*(k+1);
    for(i=0; i<sz; i++)
    {
        //if((iniResFile.ocList.at(i)->mag-iniResFile.ocList.at(i)->magOC>12))resStm << QString("%1|%2|%3|%4\n").arg(iniResFile.ocList.at(i)->mJD).arg(iniResFile.ocList.at(i)->mag-iniResFile.ocList.at(i)->magOC).arg(iniResFile.ocList.at(i)->ksiOC).arg(iniResFile.ocList.at(i)->etaOC);
        if(iniResFile.ocList.at(i)->mJD<tt1&&iniResFile.ocList.at(i)->mJD>=tt0)
        {
            if(iniResFile.ocList.at(i)->mag-iniResFile.ocList.at(i)->magOC>12) tempRF.ocList << iniResFile.ocList.at(i);
            //tempRF.ocList << iniResFile.ocList.at(i);

        }
        else
        {
            tempRF.do3sigma(0.0, 3.0);
            //resStm << QString("%1\n").arg(tempRF.ocList.size());
            if(!tempRF.countCols(&cListT, "0,6,7")&&cListT.at(0)->num>20)
            {
                resStm << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(cListT.at(0)->mean).arg(cListT.at(0)->num).arg(cListT.at(1)->mean).arg(cListT.at(2)->mean).arg(cListT.at(1)->rmsMean).arg(cListT.at(2)->rmsMean).arg(cListT.at(1)->rmsOne).arg(cListT.at(2)->rmsOne);
                lListX << pow(cListT.at(1)->rmsOne,2.0);
                lListY << pow(cListT.at(2)->rmsOne,2.0);
                lListT << pow(mjd2000-cListT.at(0)->mean, 2.0);
            }
            tempRF.clear();
            cListT.clear();
            k++;
            tt0 = t0+dt*k;
            tt1 = t0+dt*(k+1);
        }

    }


////////////////////

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


        resFile.close();

    return 0;//a.exec();
}
//
