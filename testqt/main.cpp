#include <QApplication>
#include <QtCore>
//#include "multidim.h"
#include "./../libs/astro.h"
#include "./../libs/comfunc.h"
//#include "./../libs/observatory.h"
//#include "./../libs/fitsdata.h"
//#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

int findIndex(int *tInd, int dLev, int expNum, QVector <int*> *eIndex);
int proofInd(int *tInd, int expNum, QVector <int*> *eIndex);

int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);
    QApplication app(argc, argv);

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
    clog = new QDataStream(logFile);
    ///////////////////////////////////////////////////////////////////////////////////////

    int i, j;
    QVector <int*> eIndex;
    int expNum = 3;
    int dLev = 0;
    findIndex(NULL, dLev, expNum, &eIndex);

    int iSize = eIndex.size();
    QString rStr;

    for(i=0; i<iSize; i++)
    {
        rStr.clear();
        rStr.append(QString("eIndex %1:\t").arg(i));
        for(j=0; j<expNum; j++)
        {
            rStr.append(QString("%1\t").arg(eIndex[i][j]));
        }
        qDebug() << rStr << "\n";
    }


    ///////////////////////////////////////////////////////////////////////////////////////
    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);

    return 0;
}

int findIndex(int *tInd, int dLev, int expNum, QVector <int*> *eIndex)
{
    qDebug() << QString("findIndex: dLev= %1\teIndex size: %2\n").arg(dLev).arg(eIndex->size());
    int i, j, nt, pi;
    if(dLev==0) tInd = new int[expNum];
    int iSize;// = eIndex->size();
    //if(iSize<expNum)
    //{
    iSize = 0;
    int *nInd;
    QString rStr;
    //int tInd;
    for(i=0; i<expNum; i++)
    {

        nt = 1;
        //if(findIndex(expNum, eIndex))
        for(j=0; j<dLev; j++)if(i==tInd[j])
        {
            nt = 0;
            break;
        }
        if(nt)
        {
            dLev;
            tInd[dLev] = i;
            if(dLev==expNum-1)
            {
                pi = proofInd(tInd, expNum, eIndex);
                if(!pi)
                {
                    nInd = new int[expNum];
                    rStr.clear();
                    rStr.append(QString("%1: ").arg(eIndex->size()));
                    for(j=0; j<expNum; j++)
                    {
                        nInd[j] = tInd[j];
                        rStr.append(QString("%1 ").arg(tInd[j]));
                    }

                    eIndex->append(nInd);

                    qDebug() << rStr << "\n";

                }
                //dLev--;
            }
            else findIndex(tInd, dLev+1, expNum, eIndex);
            //eIndex->append(i);
            //break;
        }
    }
    //}
    //else

    //iSize = eIndex->size();
    //if(iSize<expNum) findIndex(expNum, eIndex);

}

int proofInd(int *tInd, int expNum, QVector <int*> *eIndex)
{
    int i, j, res;
    int iSize = eIndex->size();
    for(i=0; i<iSize; i++)
    {
        res = 1;
        for(j=0; j<expNum; j++)
        {
            res *= (int)(tInd[j]==eIndex->at(i)[j]);
        }
        if(res) return 1;
    }
    return 0;
}

/*
int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);
    QApplication app(argc, argv);

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
    clog = new QDataStream(logFile);
    ///////////////////////////////////////////////////////////////////////////////////////
    //setlocale(LC_ALL, 0);
    QLocale::setDefault(QLocale::C);


    int y = atoi(argv[1]);
    int m = atoi(argv[2]);
    double d = atof(argv[3]);

    qDebug() << QString("date: %1 %2 %3\n").arg(y).arg(m).arg(d);

    int jdn = dat2JDN(y, m, d);
    double jDay;


    qDebug() << QString("jdn: %1\n").arg(jdn);
    dat2JD(&jDay, y, m, d);
    qDebug() << QString("dat2JD: %1\n").arg(jDay, 10, 'f', 2);
    dat2JD_time(&jDay, y, m, d, 0, 0, 0.0);
    qDebug() << QString("dat2JD_time: %1\n").arg(jDay, 10, 'f', 2);
    //dat2JDalt(&jDay, y, m, d);
    //qDebug() << QString("dat2JDalt: %1\n").arg(jDay, 10, 'f', 2);
    //dat2JDalt2(&jDay, y, m, d);
    //qDebug() << QString("dat2JDalt2: %1\n").arg(jDay, 10, 'f', 2);

    ///////////////////////////////////////////////////////////////////////////////////////
    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);

    return 0;
}




/*
int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);
    QApplication app(argc, argv);

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
    clog = new QDataStream(logFile);
    ///////////////////////////////////////////////////////////////////////////////////////
    refractionParam refParam;
    double ra0, de0, ra1, de1;

    observatory *oBs = new observatory;
    oBs->init("d:/data/cats/Obs.txt", OBS_SIZE);
    qDebug() << QString("init obs: %1\n").arg(oBs->nstr);
    if(oBs->getobsynumO("084")) qDebug() << "observatory is not found\n";




    refParam.Fi = oBs->record->getFi();
    refParam.Long = oBs->record->Long;

    refParam.press = 765;
    refParam.temp = 0.0;
    refParam.lam = 0.575;
    refParam.vi = 0.9;

    refParam.utc = 49573.0;
    double gm1;
    jdUT1_to_GMST1(&gm1, mjd2jd(refParam.utc));//pady
    double s = 2.0*PI*gm1 + refParam.Long;

    double t, P1, P2, P3;
    double Zed = grad2rad(30.0);
//    refParam.de0 = rad2grad(oBs->record->getFi())-Zed;
    refractionMaker *refMake = new refractionMaker(refParam);
    for(double Az=0; Az<=grad2rad(90); Az+=grad2rad(10))
    {
        //refParam.ra0 = rad2grad(s)+Az;
        //t = grad2rad(Az);
        P1 = cos(Zed)*cos(refParam.Fi) + sin(Zed)*sin(refParam.Fi)*cos(Az);
        P2 = sin(Zed)*sin(Az);
        P3 = cos(Zed)*sin(refParam.Fi) - sin(Zed)*cos(refParam.Fi)*cos(Az);


        t = atan2(P2, P1);
        refParam.de0 = rad2grad(cos(t)*atan2(P3, P1));
        refParam.ra0 = rad2grad(s - t);

        ra0 = refParam.ra0;
        de0 = refParam.de0;
        qDebug() << QString("ra0= %1\tde0= %2\n").arg(deg_to_hms(ra0, " ", 4)).arg(deg_to_damas(de0, " ", 4));
        refMake->forvRef(&ra1, &de1, ra0, de0);
        qDebug() << QString("ra1= %1\tde1= %2\n").arg(deg_to_hms(ra1, " ", 4)).arg(deg_to_damas(de1, " ", 4));
        refMake->backRef(&ra0, &de0, ra1, de1);
        qDebug() << QString("ra0= %1\tde0= %2\n").arg(deg_to_hms(ra0, " ", 4)).arg(deg_to_damas(de0, " ", 4));

    }









    ///////////////////////////////////////////////////////////////////////////////////////
    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);

    return 0;

}
*/
/*
int main(int argc, char *argv[])
{  
    qInstallMsgHandler(customMessageHandler);
    QApplication app(argc, argv);

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);

    double t, t0, t1, s, jd, utc, jdpart;
    observatory *obsPos = new observatory;
    qDebug() << QString("obsPos:") << obsPos << "\t" << obsPos->record << "\n";
    qDebug() << "init: " << obsPos->init("./Obs.txt", OBS_SIZE) << "\n";
    obsPos->getobsynumO("084");

    qDebug() << QString("long= %1\n").arg(obsPos->record->Long);

//    double ut1 = grad2rad((9 + 15*60.0 + 10.5*3600.0)*15.0);
    double jdUT1, gm1, pday;

    dat2JD_time(&jdUT1, 2000, 5, 10, 9, 15, 10.5);
    qDebug() << QString("jdUT1: %1\n").arg(jdUT1, 10, 'f');
    dat2JD(&jdUT1, 2000, 5, 10 + 9/24.0 + 15/1440.0 + 10.5/86400.0);
    qDebug() << QString("jdUT1: %1\n").arg(jdUT1, 10, 'f');
    jdUT1_to_GMST1(&gm1, jdUT1);

    qDebug() << QString("gm1: %1\n").arg(gm1);

    GMST1_to_jdUT1(&jdUT1, gm1, pdayUT1(jdUT1, &pday));
    qDebug() << QString("jdUT1: %1\n").arg(jdUT1, 10, 'f');
    //s2UTC(gm1, 0, &jdUT1);
    //qDebug() << QString("s2UTC jdUT1: %1\n").arg(jdUT1, 10, 'f');

    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);

    return 0;

}

/*
void masGetRaDeToTang1(long double *ksi, long double *eta, long double ra, long double de, long double ra_c, long double de_c)
{
//long double rd_vector[2];
ra_c = mas_to_rad(ra_c);
de_c = mas_to_rad(de_c);
ra = mas_to_rad(ra);
de = mas_to_rad(de);
*ksi = cos(de)*sin(ra-ra_c)/(sin(de)*sin(de_c)+cos(de)*cos(de_c)*cos(ra - ra_c));
*eta = (sin(de)*cos(de_c)-cos(de)*sin(de_c)*cos(ra - ra_c))/(sin(de)*sin(de_c)+cos(de)*cos(de_c)*cos(ra - ra_c));
*ksi = rad_to_mas(*ksi)/1000.0; //"
*eta = rad_to_mas(*eta)/1000.0;
//return rd_vector;
};

void gradGetRaDeToTang1(long double *ksi, long double *eta, long double ra, long double de, long double ra_c, long double de_c)
{
//long double rd_vector[2];
ra_c = grad_to_rad(ra_c);
de_c = grad_to_rad(de_c);
ra = grad_to_rad(ra);
de = grad_to_rad(de);
*ksi = cos(de)*sin(ra-ra_c)/(sin(de)*sin(de_c)+cos(de)*cos(de_c)*cos(ra - ra_c));
*eta = (sin(de)*cos(de_c)-cos(de)*sin(de_c)*cos(ra - ra_c))/(sin(de)*sin(de_c)+cos(de)*cos(de_c)*cos(ra - ra_c));
*ksi = rad_to_mas(*ksi)/1000.0;
*eta = rad_to_mas(*eta)/1000.0;
//return rd_vector;
};

void masGetTangToRaDe1(long double *rd0, long double *rd1, long double ksi, long double eta, long double ra_c, long double de_c)
{
//    long double rd_vector[2];
        ra_c = mas_to_rad(ra_c);
        de_c = mas_to_rad(de_c);
        ksi = mas_to_rad(ksi*1000);
        eta = mas_to_rad(eta*1000);
        long double secD = 1/cos(de_c);
        long double commP = 1-eta*tan(de_c);
        long double ra = atan((ksi*secD)/(commP))+ra_c;
        long double de = atan((eta+tan(de_c))*cos(ra - ra_c)/commP);
        *rd0 = rad2mas(ra);
        *rd1 = rad2mas(de);
//	return rd_vector;
};

void gradGetTangToRaDe1(long double *rd0, long double *rd1, long double ksi, long double eta, long double ra_c, long double de_c)
{
//    long double rd_vector[2];
        ra_c = grad_to_rad(ra_c);
        de_c = grad_to_rad(de_c);
        ksi = mas_to_rad(ksi*1000);
        eta = mas_to_rad(eta*1000);
        long double secD = 1/cos(de_c);
        long double commP = 1-eta*tan(de_c);
        long double ra = atan((ksi*secD)/(commP))+ra_c;
        long double de = atan((eta+tan(de_c))*cos(ra - ra_c)/commP);
        *rd0 = rad2grad(ra);
        *rd1 = rad2grad(de);
//	return rd_vector;
};

int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);
    QApplication app(argc, argv);

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);
//===========================================================================================

    double ra, de, aresT0, aresT1;
    long double raN, deN, eta, ksi;
    long double ra_c, de_c, dRa, dDe;
    long double ra_c1, de_c1;
    long double ra0, ra1, de0, de1;
    int i, totnum;

    totnum = 1000;


    //T0: mas -> rad -> tang -> rad -> mas




    ra0 = 0;
    ra1 = grad_to_mas(360);
    de0 = grad_to_mas(-90);
    de1 = grad_to_mas(90);

    srand(time(NULL));
    ra_c = (rand()*(ra1 - ra0)/RAND_MAX) + ra0;
    srand(time(NULL));
    de_c = (rand()*(de1 - de0)/RAND_MAX) + de0;

    ra_c1 = mas_to_grad(ra_c);
    de_c1 = mas_to_grad(de_c);

    dRa = 100000;
    dDe = 100000;
    ra0 = ra_c - dRa/2.0;
    ra1 = ra_c + dRa/2.0;
    de0 = de_c - dDe/2.0;
    de1 = de_c + dDe/2.0;

    srand(time(NULL));

    aresT0 = 0.0;
    aresT1 = 0.0;

    qDebug() << QString("sizeof T0 %1\n").arg(sizeof(aresT0));

    for(i=0; i<totnum; i++)
    {
        ra = (rand()*(ra1 - ra0)/RAND_MAX) + ra0;
        de = (rand()*(de1 - de0)/RAND_MAX) + de0;

        masGetRaDeToTang1(&ksi, &eta, ra, de, ra_c, de_c);
        masGetTangToRaDe1(&raN, &deN, ksi, eta, ra_c, de_c);

        //qDebug() << QString("ra: %1\tde: %2\tksi: %3\teta: %4\traN: %5\tdeN: %6\tdRa: %7\tdDe: %8\n").arg((double)ra).arg((double)de).arg((double)ksi).arg((double)eta).arg((double)raN).arg((double)deN).arg((double)fabs(ra - raN)).arg((double)fabs(de - deN));
        aresT0 += fabs(ra - raN) + fabs(de - deN);

        ra = mas_to_grad(ra);
        de = mas_to_grad(de);


        gradGetRaDeToTang1(&ksi, &eta, ra, de, ra_c1, de_c1);
        gradGetTangToRaDe1(&raN, &deN, ksi, eta, ra_c1, de_c1);

        //qDebug() << QString("ra: %1\tde: %2\tksi: %3\teta: %4\traN: %5\tdeN: %6\tdRa: %7\tdDe: %8\n").arg((double)ra).arg((double)de).arg((double)ksi).arg((double)eta).arg((double)raN).arg((double)deN).arg((double)fabs(ra - raN)).arg((double)fabs(de - deN));
        aresT1 += fabs(ra - raN) + fabs(de - deN);
    }

    //T1: grad -> rad -> tang -> rad -> grad

/*


    ra_c = mas_to_grad(ra_c);
    de_c = mas_to_grad(de_c);


    ra0 = mas_to_grad(ra0);
    ra1 = mas_to_grad(ra1);
    de0 = mas_to_grad(de0);
    de1 = mas_to_grad(de1);

    srand(time(NULL));

    aresT1 = 0.0;

    for(i=0; i<totnum; i++)
    {
        ra = (rand()*(ra1 - ra0)/RAND_MAX) + ra0;
        de = (rand()*(de1 - de0)/RAND_MAX) + de0;

        gradGetRaDeToTang1(&ksi, &eta, ra, de, ra_c, de_c);
        gradGetTangToRaDe1(&raN, &deN, ksi, eta, ra_c, de_c);

        qDebug() << QString("ra: %1\tde: %2\tksi: %3\teta: %4\traN: %5\tdeN: %6\tdRa: %7\tdDe: %8\n").arg(ra).arg(de).arg(ksi).arg(eta).arg(raN).arg(deN).arg(fabs(ra - raN)).arg(fabs(de - deN));
        aresT1 += fabs(ra - raN) + fabs(de - deN);
    }
/
    /////////////////////////

    qDebug() << QString("\naresT0 = %1\n").arg((double)aresT0, 12, 'e');
    qDebug() << QString("\naresT1 = %1\n").arg((double)grad_to_mas(aresT1), 12, 'e');




//===========================================================================================

    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);

    return 0;

}
*/
