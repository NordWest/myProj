#include "hronobasefile.h"

int hronoBaseRec::fromString(QString iStr)
{
    QStringList sL = iStr.split("|");
    if(sL.size()!=4)
    {
        qDebug() << "hronoBaseRec::fromString error\n";
        return 1;
    }
    //qDebug() << sL.join("|") << "\n";

    date = QDate::fromString(sL[0], "dd.MM.yyyy");
    timeReal = QTime::fromString(sL[1], "hh mm ss.zzz");
    timeHrono = QTime::fromString(sL[2], "hh mm ss.zzz");
    comment = sL[3];
    return 0;
}

void hronoBaseRec::toString(QString *iStr)
{
    QStringList bStrL;
    bStrL << date.toString("dd.MM.yyyy");
    bStrL << timeReal.toString("hh mm ss.zzz");
    bStrL << timeHrono.toString("hh mm ss.zzz");

    bStrL << comment;

    QString bStr = bStrL.join("|");
    iStr->insert(0, bStr);
}

double hronoBaseRec::getMjdReal()
{
    double jdUtc, jDay, pday, gm1, s0, s1, dS, dS1;
    double Long = grad_to_rad(30.3274)/(2.0*PI);//day

    double sTime = timeReal.hour()/24.0 + timeReal.minute()/1440.0 + timeReal.second()/86400.0 + timeReal.msec()/86400000.0;

    gm1 = sTime - Long;
    //if(FD_LOG_LEVEL) qDebug() << QString("sTime= %1\tgm1= %2\tLong= %3\n").arg(sTime).arg(gm1).(arg(Long);
    //dat2JD(&jDay, date.year(), date.month(), date.day()+0.5);
    jDay = dat2JDN(date.year(), date.month(), date.day());

    s0 = gmst0jd(jDay);
    s1 = gmst0jd(jDay+1);
    dS = gm1 - s0;
    dS1 = gm1 - s1;
    //if(FD_LOG_LEVEL) qDebug() << QString("jDay= %1\ts0= %2\ts1= %3\tdS= %4\tdS1= %5\n").arg(jDay, 10, 'f', 4).arg(s0).arg(s1).arg(dS).arg(dS1);
    //if(dS<0.0) dS +=1.0;
    //if(dS<0.5) jDay+=1.0;

//    if(FD_LOG_LEVEL) qDebug() << QString("dS= %1\tjDay= %2\n").arg(dS).arg(jDay, 10, 'f', 4);

    GMST1_to_jdUT1(&jdUtc, gm1, jDay);
 //   if(FD_LOG_LEVEL) qDebug() << QString("jdUTC= %1\tdS= %2\tdS1= %3\n").arg(jdUtc, 10, 'f', 4).arg(dS).arg(dS1);
    //if(dS1<0.0)dS1+=1.0;
  //  if(FD_LOG_LEVEL) qDebug() << QString("dS= %1\tdS1= %2\n").arg(dS).arg(dS1);
    //if((dS1>0.5)&&(dS<0.5)) jdUtc-=(1.0-VIU);



    //dateObs0.hour = utc*24.0;
    //dateObs0.min = tList.at(1).toInt();
    //dateObs0.sec = tList.at(2).toDouble();
    return(jd2mjd(jdUtc));
}

double hronoBaseRec::getMjdReal1()
{
    double jdUtc, jDay, pday;
    double Long = grad_to_rad(30.3274)/(2.0*PI);//day

    double sTime = timeReal.hour()/24.0 + timeReal.minute()/1440.0 + timeReal.second()/86400.0 + timeReal.msec()/86400000.0;
    double gm1 = sTime - Long;

    dat2JD(&jDay, date.year(), date.month(), date.day());
    double jd0 = pdayUT1(jDay, &pday);
    GMST1_to_jdUT1(&jdUtc, gm1, jd0);

    qDebug() << QString("\nsTime= %1\ngm1= %2\njDay= %3\njd0= %4\njdUtc= %5\n").arg(sTime).arg(gm1).arg(jDay, 9, 'f', 2).arg(jd0, 9, 'f', 2).arg(jdUtc, 9, 'f', 2);
    return(jd2mjd(jdUtc));
}

double hronoBaseRec::getMjdHrono()
{
    double jdUtc, jDay;
    double Long = grad_to_rad(30.3274)/(2.0*PI);//day

    double sTime = timeHrono.hour()/24.0 + timeHrono.minute()/1440.0 + timeHrono.second()/86400.0 + timeHrono.msec()/86400000.0;
    double gm1 = sTime - Long;
    dat2JD(&jDay, date.year(), date.month(), date.day());
    GMST1_to_jdUT1(&jdUtc, gm1, jDay);
    return(jd2mjd(jdUtc));
}

double hronoBaseRec::getJDNreal()
{
    int jdn = dat2JDN(date.year(), date.month(), date.day());
    double sTime = timeReal.hour()/24.0 + timeReal.minute()/1440.0 + timeReal.second()/86400.0 + timeReal.msec()/86400000.0;

    return(jdn + sTime);
}

double hronoBaseRec::getJDNhrono()
{
    int jdn = dat2JDN(date.year(), date.month(), date.day());
    double sTime = timeHrono.hour()/24.0 + timeHrono.minute()/1440.0 + timeHrono.second()/86400.0 + timeHrono.msec()/86400000.0;

    return(jdn + sTime);
}

double hronoBaseRec::getU()
{
    double sTimeHrono = timeHrono.hour()/24.0 + timeHrono.minute()/1440.0 + timeHrono.second()/86400.0 + timeHrono.msec()/86400000.0;
    double sTimeReal = timeReal.hour()/24.0 + timeReal.minute()/1440.0 + timeReal.second()/86400.0 + timeReal.msec()/86400000.0;
    double u = (sTimeReal - sTimeHrono)*86400;
    if(fabs(u)>86000) u += -(u/fabs(u))*86400;
    return(u);
}

double hronoBaseRec::getJDT(double lam)
{
    double jdnT = getJDNhrono();
    //jdnT -= lam;
    int jN = (int) jdnT;
    double sT = jdnT - jN;
    //qDebug() << QString("jN= %1\tjDN= %2\tdJDN= %3\n").arg(jN).arg(jDN).arg(abs(jN-jDN));

    double T = detT(jN);
    double s0 = gmst0(T);
    //s0 = s0 - (int)s0;
    //if(s0<0) s0+=(int)s0;
    //if(s0>1.0) s0 -= (int)s0;
    //s0 -= lam;
    double s1 = gmst1(0.0, s0);
    double s1m = s1 + lam;
    //if(s1m<0) s1m+=1.0;
    //if(s1m>1.0) s1m -= 1.0;

    double dS = sT - s1m;
    //qDebug() << QString("sT= %1\ts0= %2\n").arg(sT).arg(s1m);
    if(dS<0) dS+=1.0;
    if(dS>1.0) dS -= 1.0;
    //if((dS<0.25)&&(dS>=0)) dS+=1.0;
    //if((fabs(dS)<0.25)) dS+=1.0;
    double jdt = jN + dS;

    return jdt;
}

/////////////////////////////////////////////////////////////////////////////////////////

hronoBaseFile::hronoBaseFile()
{

}

hronoBaseFile::hronoBaseFile(QString baseFileName)
{
    fileName = baseFileName;
}

void hronoBaseFile::initJournal()
{
    hronoBaseRec *bRec;

    QFile iFile(fileName);
    iFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream iStream(&iFile);

    QString dataStr;
    int i=0;

    while(!iStream.atEnd())
    {
        dataStr = iStream.readLine();
        bRec = new hronoBaseRec;
        if(bRec->fromString(dataStr)) qDebug() << QString("err line: %1\n").arg(i);
        hronoList << bRec;
        //qDebug() << QString("i:%1\tatEnd: %2").arg(i).arg((int)(iFile.atEnd()));
        i++;
    }
//    qDebug() << QString("linr num= %1\n").arg(i);

    iFile.close();
}

int triAngle(double *ang, QList <double*> dList, int p0, int p1, int p2)
{
    int sz = dList.size();
    if(p0<0||p1<0||p2<0||p0>sz||p1>sz||p2>sz) return 1;

    double *v0, *v1;
    v0 = new double[2];
    v1 = new double[2];

    qDebug() << QString("p0: %1\t%2\n").arg(dList[p0][0]).arg(dList[p0][1]);
    qDebug() << QString("p1: %1\t%2\n").arg(dList[p1][0]).arg(dList[p1][1]);
    qDebug() << QString("p2: %1\t%2\n").arg(dList[p2][0]).arg(dList[p2][1]);

    v0[0] = dList[p0][0] - dList[p1][0];
    v0[1] = dList[p0][1] - dList[p1][1];

    v1[0] = dList[p2][0] - dList[p1][0];
    v1[1] = dList[p2][1] - dList[p1][1];

    qDebug() << QString("v0: %1\t%2\n").arg(v0[0]).arg(v0[1]);
    qDebug() << QString("v1: %1\t%2\n").arg(v1[0]).arg(v1[1]);

    double x = (v0[0]*v1[0] + v0[1]*v1[1])/(sqrt(v0[0]*v0[0] + v0[1]*v0[1])*sqrt(v1[0]*v1[0] + v1[1]*v1[1]));
    qDebug() << QString("x= %1\n").arg(x);

    *ang = rad2grad(acos(x));


    delete [] v0;
    delete [] v1;

    return 0;
}


int hronoBaseFile::findU_mjd(double *uCorr, double mJD, double dDay)
{

    int i, j, szi, szj, k, posBase;
    szi = hronoList.size();
    double *dVect;// = new double[2];
    QList <double*> dList;
    QList <double*> resList;
    double mjdT;

    k=0;
    dList.clear();
    posBase = 0;
    for(i=0; i<szi; i++)
    {
        mjdT = hronoList.at(i)->getMjdReal();
        if(fabs(mJD-mjdT)<dDay)
        {
            dVect = new double[2];
            dVect[0] = mjdT-mJD;

            if(dVect[0]<0.0) posBase = k+1;
            k++;

            dVect[1] = hronoList.at(i)->getU();
            dList << dVect;
            //comList << hronoList.at(i)->comment;
        }
    }

    int szL = dList.size();

    if(szL==0)
    {
        *uCorr = 0;
        return 1;
    }
    if(szL==1)
    {
        *uCorr = dList.at(0)[1];
        return 0;
    }

    if(posBase==0)
    {
        *uCorr=0;
        return 1;
    }

    resList.clear();
    for(i=0; i<posBase; i++)
    {
        resList << dList.at(i);
    }

    int dnum;
    double *C, *L, *We, *D;
    int deg;
    int rn;
    double uwe, fl, maxdiv;
    int *exclind;
    double *z;

    deg = 2;
    maxdiv = 10;
    fl = -2.0;


    while(resList.size()>2)
    {
        dnum = resList.size();

        C = new double[dnum*deg];
        L = new double[dnum];
        D = new double[deg*deg];
        exclind = new int[dnum];
        z = new double[deg];

        for(i=0; i<dnum; i++)
        {
            for(j=0; j<deg; j++)
            {
                C[i*deg+j] = pow(resList[i][0], j);
            }
            L[i] = resList[i][1];
        }

        iLSM(deg, dnum, maxdiv, exclind, z, C, L, uwe, D, fl, rn);

        if(rn<dnum|(fabs(z[1])>15))
        {
            resList.removeAt(0);
            delete [] C;
            delete [] L;
            delete [] D;
            delete [] exclind;
            delete [] z;
        }
        else break;
    }

    int posBase1 = resList.size();

    for(i=posBase; i<szL; i++)
    {
        resList.append(dList.at(i));
    }


    while(resList.size()>2)
    {
        dnum = resList.size();

        C = new double[dnum*deg];
        L = new double[dnum];
        D = new double[deg*deg];
        exclind = new int[dnum];
        z = new double[deg];

        for(i=0; i<dnum; i++)
        {
            for(j=0; j<deg; j++)
            {
                C[i*deg+j] = pow(resList[i][0], j);
            }
            L[i] = resList[i][1];
        }

        iLSM(deg, dnum, maxdiv, exclind, z, C, L, uwe, D, fl, rn);

        if(rn<dnum)
        {
            resList.removeAt(dnum-1);
            delete [] C;
            delete [] L;
            delete [] D;
            delete [] exclind;
            delete [] z;
        }
        else break;
    }

    dnum = resList.size();
    if(dnum>5) deg = 3;
    else deg = 2;

    C = new double[dnum*deg];
    L = new double[dnum];
    D = new double[deg*deg];
    exclind = new int[dnum];
    z = new double[deg];
    fl = 3.0;

    for(i=0; i<dnum; i++)
    {
        for(j=0; j<deg; j++)
        {
            C[i*deg+j] = pow(resList[i][0], j);
        }
        L[i] = resList[i][1];
    }

    iLSM(deg, dnum, maxdiv, exclind, z, C, L, uwe, D, fl, rn);

    if(rn<2)
    {
        *uCorr=resList[posBase1][1];
        return 1;
    }

    *uCorr = z[0];

    delete [] C;
    delete [] L;
    delete [] D;
    delete [] exclind;
    delete [] z;

    return 0;

}


int hronoBaseFile::findU(double *uCorr, int jDN, double sTime, int dJDN)
{
   // qDebug() << "\n\nfindU\n\n";
  //  qDebug() << QString("jDN= %1\tsTime= %2\n").arg(jDN).arg(sTime);
    double lam = 30.3274/360.0;
    double T = detT(jDN-1);
    double s0 = gmst0(T);
    //s0 +=lam;
    /*s0 = s0 - (int)s0;
    if(s0<0) s0+=1.0;
    if(s0>1.0) s0 -= 1.0;*/
    double s1;
    s1 = gmst1(0.0, s0);
    double s1m;
    s1m = s1 + lam;
    if(s1m<0) s1m+=1.0;
    if(s1m>1.0) s1m -= 1.0;

  //  qDebug() << QString("sTime: %1\ts0: %2\ts1: %3\ts1m: %4\n").arg(sTime).arg(s0).arg(s1).arg(s1m);

    double dS = sTime - s1m;
 //   qDebug() << QString("dS= %1\n").arg(dS);
    if(dS>1.0) dS -= 1.0;
    if(dS<0.0) dS+=1.0;
    if((dS<0.5)&&(dS>=0)) dS+=1.0;
    //if(dS>1.0) dS -= 1.0;
    //s0 = dS;
    //dS = 0;

    //qDebug() << QString("dS= %1\n").arg(dS);



    double jd0 = jDN + dS;
    double jdt, sT;

    QStringList comList;
    int pos0, pos1;
    int p0, p1, p2, p, pb;
    double meanVol0, rmsVol0;
    double meanVol1, rmsVol1;
    double vol0, vol1;
 //   qDebug() << QString("jd0= %1\tjDN= %2\n").arg(jd0, 10, 'f', 4).arg(jDN);


    int forvNum, backNum, shiftNum;
    int i, j, szi, szj;
    szi = hronoList.size();
    double *dVect;// = new double[2];
    QList <double*> dList;

    int jN;
    double jdnT, sN;
    double dU;

    //jd0 -= lam;
    double v0, dv;

    for(i=0; i<szi; i++)
    {
        /*
        jdnT = hronoList.at(i)->getJDNhrono();
        //jdnT -= lam;
        jN = (int) jdnT;
        sT = jdnT - jN;
        //qDebug() << QString("jN= %1\tjDN= %2\tdJDN= %3\n").arg(jN).arg(jDN).arg(abs(jN-jDN));

        T = detT(jN-1);
        s0 = gmst0(T);
        s0 = s0 - (int)s0;
        if(s0<0) s0+=1.0;
        if(s0>1.0) s0 -= 1.0;
        //s0 -= lam;
        s1 = gmst1(0.0, s0);
        s1m = s1 + lam;
        if(s1m<0) s1m+=1.0;
        if(s1m>1.0) s1m -= 1.0;

        dS = sT - s1m;
        //qDebug() << QString("sT= %1\ts0= %2\n").arg(sT).arg(s1m);
        if(dS<0) dS+=1.0;
        if(dS>1.0) dS -= 1.0;
        //if((dS<0.25)&&(dS>=0)) dS+=1.0;
        //if((fabs(dS)<0.25)) dS+=1.0;
        jdt = jN + dS;
        */
        jdt = hronoList.at(i)->getJDT(lam);



        if((fabs(jdt-jd0)<=dJDN))
        {

            //qDebug() << QString("sT= %1\ts0= %2\n").arg(sT).arg(s1m);
            //qDebug() << QString("jdt= %1\tjd0= %2\tjN= %3\n").arg(jdt, 10, 'f', 2).arg(jd0, 10, 'f', 2).arg(jN);

            dVect = new double[2];
            dVect[0] = jdt - jd0;
            /*if(dList.size()==0)
            {
                v0 = 0.0;
                dv = 0.0;
            }
            else dv = dVect[0] - v0;
            if(dv<0.0) dVect[0] += 1.0*/;
            dVect[1] = hronoList.at(i)->getU();
            dList << dVect;
            comList << hronoList.at(i)->comment;
    //        qDebug() << QString("%1\t%2\t%3\t%4\t%5\n").arg(hronoList.at(i)->date.toString("yyyy MM dd")).arg(hronoList.at(i)->timeHrono.toString("hh mm ss.zzz")).arg(dVect[0], 10, 'f', 2).arg(jdt-jd0).arg(dVect[1]);
        }
        /*
        if(abs(jN-jDN)<=dJDN)
        {

            dVect = new double[2];
            dVect[0] = (jN-jDN) + jdnT-jN-s0;
            dVect[1] = hronoList.at(i)->getU();
            dList << dVect;
            qDebug() << QString("%1\t%2\t%3\t%4\n").arg(hronoList.at(i)->date.toString("yyyy MM dd")).arg(hronoList.at(i)->timeHrono.toString("hh mm ss.zzz")).arg(dVect[0]).arg(dVect[1]);
            qDebug() << QString("jN= %1\tjDN= %2\tdJDN= %3\n").arg(jN).arg(jDN).arg(jdnT-jDN);
        }*/
    }

    szi = dList.size();

   // qDebug() << QString("dList size= %1\n").arg(szi);

    if(szi<2)
    {
        *uCorr = 0.0;
   //     qDebug() << "\nThere is no 2 hronometer records\n";
        return 1;
    }
/*
    if(szi==1){*uCorr = dList[0][1]; return;}
    else if(szi==0){*uCorr = 0.0; return;}*/

    int pos0t, pos1t;

    pos0t = 0;
    pos1t = szi-1;
    int k=0;

    jd0 = 0.0;
    int b1;

    do
    {
        b1 = 0;

        forvNum = pos0 = pos0t;
        backNum = pos1 = pos1t;
 //       qDebug() << QString("pos0: %1\tpos1: %2\n").arg(pos0t).arg(pos1t);


        meanVol0 = meanVol1 = rmsVol0 = rmsVol1 = 0.0;

        if(jd0<dList[pos0][0]) pb = pos0;
        if(jd0>dList[pos1][0]) pb = pos1;
        j=pos1;
        for(i=pos0; i<pos1; i++)
        {

            //qDebug() << QString("i: %1\tj: %2\n").arg(i).arg(j);

            if(jd0>dList[i][0]&&jd0<dList[i+1][0]) pb = i;
            vol0 = (dList[i+1][1] - dList[i][1])/(dList[i+1][0] - dList[i][0]);
            vol1 = (dList[j][1] - dList[j-1][1])/(dList[j][0] - dList[j-1][0]);
            meanVol0 += vol0;
            meanVol1 += vol1;
     //       qDebug() << QString("%1f: %2\t%3\t\t%4\t%5\tvel[%1]: %6\n").arg(i+1).arg(dList[i][0], 12, 'f', 4).arg(dList[i][1]).arg(dList[i+1][0], 12, 'f', 4).arg(dList[i+1][1]).arg(vol0);
     //       qDebug() << QString("%1b: %2\t%3\t\t%4\t%5\t\tvel[%1]: %6\n").arg(j).arg(dList[j][0], 12, 'f', 4).arg(dList[j][1]).arg(dList[j-1][0], 12, 'f', 4).arg(dList[j-1][1]).arg(vol1);
            j--;
        }

        meanVol0 /= 1.0*(pos1-pos0);
        meanVol1 /= 1.0*(pos1-pos0);

      //  qDebug() << QString("mean0= %1\tmean1= %2\n").arg(meanVol0).arg(meanVol1);

        j=pos1;
        for(i=pos0; i<pos1; i++)
        {
            vol0 = (dList[i+1][1] - dList[i][1])/(dList[i+1][0] - dList[i][0]);
            vol1 = (dList[j][1] - dList[j-1][1])/(dList[j][0] - dList[j-1][0]);
            rmsVol0 += pow(vol0 - meanVol0, 2.0);
            rmsVol1 += pow(vol1 - meanVol1, 2.0);
            j--;
        }

        rmsVol0 = sqrt(rmsVol0/((pos1-pos0+1)*(pos1-pos0)));
        rmsVol1 = sqrt(rmsVol1/((pos1-pos0+1)*(pos1-pos0)));

      //  qDebug() << QString("rms0= %1\trms1= %2\n").arg(rmsVol0).arg(rmsVol1);

        j=pos1;
        for(i=pos0; i<pos1; i++)
        {
            vol0 = (dList[i+1][1] - dList[i][1])/(dList[i+1][0] - dList[i][0]);
            vol1 = (dList[j][1] - dList[j-1][1])/(dList[j][0] - dList[j-1][0]);
            if(fabs(vol0-meanVol0)>3.0*rmsVol0) forvNum = i+1;
            if(!b1&&(fabs(vol1-meanVol1)>3.0*rmsVol1))
            {
                backNum = j;
                b1 = 1;
            }
            j--;
        }

      //  qDebug() << QString("forvNum= %1\tbackNum= %2\n").arg(forvNum).arg(backNum);



     //   qDebug() << QString("pb= %1\t%2\t%3\n").arg(pb).arg(dList[pb][0]).arg(dList[pb][1]);

        if(forvNum==backNum)
        {
            shiftNum = forvNum;
            if(pb>=shiftNum)
            {
                pos0t = shiftNum;
                pos1t = pos1;
            }
            else
            {
                pos0t = pos0;
                pos1t = shiftNum-1;
            }
        }
        else
        {
            pos0t = pos0;
            pos1t = pos1;
        }

        k++;


    //    qDebug() << QString("pos0t: %1\tpos1t: %2\n").arg(pos0t).arg(pos1t);


    }while(!((pos0==pos0t)&&(pos1==pos1t))&&k<8);

    pos0 = pos0t;
    pos1 = pos1t;
/*
    QString pbS = comList.at(pb)[0];



    for(i=szi-1; i>=0; i--)
    {
        qDebug() << QString("%1: %2\n").arg(pbS).arg(comList.at(i)[0]);
        if(pbS!=comList.at(i)[0])

        {
            dList.removeAt(i);
            comList.removeAt(i);
        }
    }

    szi = dList.size();

    qDebug() << QString("dList size= %1\n").arg(szi);

    if(szi<2)
    {
        *uCorr = 0.0;
        qDebug() << "\nThere is no 2 hronometer records\n";
        return 1;
    }

    for(i=0; i<szi-1; i++)
    {
        if(jd0>dList[i][0]&&jd0<dList[i+1][0]) pb = i;
        qDebug() << QString("%1: %2\t%3\tvel[%1]: %4\n").arg(i).arg(dList[i][0]).arg(dList[i][1]).arg((dList[i+1][1] - dList[i][1])/(dList[i+1][0] - dList[i][0]));
    }

    qDebug() << QString("pb= %1\t%2\t%3\n").arg(pb).arg(dList[pb][0]).arg(dList[pb][1]);
*/

    QList <double*> dList1;

    for(i=pos0; i<=pos1; i++) dList1 << dList[i];


/*
    dList1 << dList[pb];
    double ang;
    int np;
    qDebug() << "\nback\n";


    np = 0;
    int ps = pb-2;
    if(pb<2) dList1 << dList[0];
    else
    {
        for(p=ps; p>=0; p--)
        {
            p0 = p;
            p1 = p+1;
            p2 = p+2;

            qDebug() << QString("p0= %1\tp1= %2\tp2= %3\n").arg(p0).arg(p1).arg(p2);

            if(triAngle(&ang, dList, p0, p1, p2)) continue;
            qDebug() << QString("ang= %1\n").arg(ang);
            if(ang<90) break;
            np = 1;
            dList1 << dList[p1];
        }
        if(np) dList1 << dList[p1];
    }

    qDebug() << "\nforv\n";

    np = 0;
    for(p=pb; p<szi-2; p++)
    {
        p0 = p;
        p1 = p+1;
        p2 = p+2;
        qDebug() << QString("p0= %1\tp1= %2\tp2= %3\n").arg(p0).arg(p1).arg(p2);
        if(triAngle(&ang, dList, p0, p1, p2)) continue;
        qDebug() << QString("ang= %1\n").arg(ang);
        if(ang<90) break;
        np = 1;
        dList1 << dList[p1];
    }
    qDebug() << QString("p= %1\tang= %2\tnp= %3\tp1= %4\n").arg(p).arg(ang).arg(np).arg(p1);
    if((p==szi-2)&&(ang>90))dList1 << dList[p+1];
    else if(np) dList1 << dList[p1];


    */



    int ed = 0;
    double *C, *L, *We, *D;
    int deg;
    int rn;
    double uwe, fl, maxdiv;
    int *exclind;
    double *z;
    QStringList resZ, resRn, excList;
    int dnum;
    fl = 2.0;
    maxdiv = 1;

    dnum = dList1.size();

  //  qDebug() << QString("dList1 size= %1\n").arg(dnum);
    if(dnum<2)
    {
        *uCorr = 0.0;
 //       qDebug() << "\nThere is no 2 close hronometer records\n";
        return 1;
    }
    if(dnum<3)
    {
        *uCorr = dList1[0][1];
        if(fabs(dList1[0][0])>fabs(dList1[1][0])) *uCorr = dList1[1][1];
 //       qDebug() << "\nOnly 2 close hronometer records\n";
        return 0;
    }
    //if(dnum==1){*uCorr = dList1[0][1]; return;}
    //else if(dnum==0){*uCorr = 0.0; return;}

    if(dnum>4) deg = 3;
    else deg = 2;

    C = new double[dnum*deg];
    L = new double[dnum];
    D = new double[deg*deg];
    exclind = new int[dnum];
    z = new double[deg];

    for(i=0; i<dnum; i++)
    {
//        qDebug() << QString("%1: %2\t%3\n").arg(i).arg(dList1[i][0], 12, 'f', 4).arg(dList1[i][1]);
        for(j=0; j<deg; j++)
        {
            C[i*deg+j] = pow(dList1[i][0], j);
        }
        L[i] = dList1[i][1];
    }

    iLSM(deg, dnum, maxdiv, exclind, z, C, L, uwe, D, fl, rn);

    resZ.clear();
    resRn.clear();

    for(i=0; i<dnum; i++)
    {
        resRn << QString("%1").arg(exclind[i]);
        if(exclind[i]==-1) excList << QString("%1").arg(i);
    }

 //   qDebug() << QString("UWE= %1\tRn= %2\n").arg(uwe).arg(rn);
 //   qDebug() << QString("exclind: %1\n").arg(resRn.join("|"));
 //   qDebug() << QString("Z: %1\n").arg(resZ.join("|"));

    dU = 0.0;
    //jd0 = 0.0;
    for(i=0; i<deg; i++)
    {
        resZ << QString("%1").arg(z[i]);

        dU += pow(jd0, i)*z[i];

    }
 //   qDebug() << QString("x= %1\tdU = %2\n").arg(jd0, 12, 'f', 4).arg(dU);
    *uCorr = dU;

    delete [] C;
    delete [] L;
    delete [] D;
    delete [] exclind;
    delete [] z;

    return 0;

    /*bPos = 0;
    ePos = szi;

    do
    {
        szi = ePos-bPos;
        qDebug() << QString("dList size= %1\n").arg(szi);
        if(szi==1){*uCorr = dList[bPos][1]; return;}
        else if(szi==0){*uCorr = 0.0; return;}


        //forv

        excList.clear();



        z = new double[2];

        z[1] = (dList[bPos+1][1] - dList[bPos][1])/(dList[bPos+1][0] - dList[bPos][0]);
        if(fabs(dList[bPos][0])>1e-5) z[0] = (dList[bPos][1] - z[1])/dList[bPos][0];
        else  z[0] = (dList[bPos+1][1] - z[1])/dList[bPos+1][0];

        qDebug() << QString("Zk0: %1\t%2\n").arg(z[0]).arg(z[1]);

        for(dnum=bPos+3; dnum<szi; dnum++)
        {
            qDebug() << QString("dnum: %1\n").arg(dnum);

            if(dnum>6) deg = 3;
            else deg = 2;


            dU = 0.0;
            for(i=0; i<deg; i++)
            {
                resZ << QString("%1").arg(z[i]);

                dU += pow(dList[dnum][0], i)*z[i];

            }
            qDebug() << QString("x= %1\tdU = %2\n").arg(dList[dnum][0]).arg(dU);
            if(dU>5)
            {
                bPos = dnum;
                break;
            }


            if(dnum==3) delete [] z;
            C = new double[dnum*deg];
            L = new double[dnum];
            D = new double[deg*deg];
            exclind = new int[dnum];
            z = new double[deg];

            for(i=0; i<dnum; i++)
            {
                qDebug() << QString("%1: %2\t%3\n").arg(i).arg(dList[i][0]).arg(dList[i][1]);
                for(j=0; j<deg; j++)
                {
                    C[i*deg+j] = pow(dList[i][0], j);
                }
                L[i] = dList[i][1];
            }

            iLSM(deg, dnum, maxdiv, exclind, z, C, L, uwe, D, fl, rn);

            resZ.clear();
            resRn.clear();



            for(i=0; i<dnum; i++)
            {
                resRn << QString("%1").arg(exclind[i]);
                if(exclind[i]==-1) excList << QString("%1").arg(i);
            }

            qDebug() << QString("UWE= %1\tRn= %2\n").arg(uwe).arg(rn);
            qDebug() << QString("exclind: %1\n").arg(resRn.join("|"));
            qDebug() << QString("Z: %1\n").arg(resZ.join("|"));


            delete [] C;
            delete [] L;
            delete [] D;
            delete [] exclind;
            delete [] z;
        }
        if((dnum==ePos-1)&&(dS<)) ed = 1;

    }while(ed);

    for(i=0; i<deg; i++)
    {
        resZ << QString("%1").arg(z[i]);

        dU += pow(dList[dnum][0], i)*z[i];

    }*/

}
