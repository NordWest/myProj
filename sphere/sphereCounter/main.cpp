#include <QtCore/QCoreApplication>

#include <QFile>
#include <QSettings>
#include <QTextStream>
#include <QList>

#include <astro.h>
#include <mb.h>

#include "./../../libs/comfunc.h"
#include "./../../libs/ringpix.h"
#include "./../../libs/vsfFunc.h"
#include "./../../libs/sphere.h"

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

struct improveState
{
    double jdTDB;
    double raO, decO;
    double raC, decC;
    double state[6];
    QString obsCode;
};
/*
int lsmCount(double *ra, double *de, double *dRa, double *dDe, int pointNum, double *Eps, double *sgEps);
int lsmCountVel(double *dTime, double *ra, double *dec, double *dRa, double *dDe, int pointNum, double *Eps, double *sgEps);
int vsfCount(double *ra, double *de, double *dRa, double *dDe, int pointNum, double *sCoef, double *tCoef, int coefNum, double &sigmaVal);
int vsfCount_lsm(double *ra, double *dec, double *dRa, double *dDe, int pointNum, int coefNum, double *sCoef, double *tCoef, double *sCoefSg, double *tCoefSg);
*/

int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);

    if(argc<2) return 1;

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);

    QSettings *sett = new QSettings("sphC.ini", QSettings::IniFormat);

    int solMode = sett->value("general/solutionMode", 0).toInt();
    //0 - LSM;
    //1 - VSF;
    //2 - vsfCount_lsm;
    //3 - lsmCountVel;

    int isZonal = sett->value("general/isZonal", 0).toInt();
    double dMin = grad2rad(sett->value("general/dMin", -90).toDouble());
    double dMax = grad2rad(sett->value("general/dMax", 90).toDouble());
    int coefNum = sett->value("general/coefNum", 9).toInt();

//input
    QString colSep = sett->value("input/colSep", "|").toString();
    int ct = sett->value("input/ct", 0).toInt();
    int cx = sett->value("input/cx", 0).toInt();
    int cy = sett->value("input/cy", 1).toInt();
    int cxc = sett->value("input/cxc", 2).toInt();
    int cyc = sett->value("input/cyc", 3).toInt();
    int cn = sett->value("input/cn", 4).toInt();
    int isDegree = sett->value("input/isDegree", 0).toInt();

    double time0 = sett->value("general/time0", 2450000.5).toDouble();

    double coef1 = sett->value("vsf/coef1", 2.89).toDouble();
    double coef2 = sett->value("vsf/coef2", 2.89).toDouble();
    double coef3 = sett->value("vsf/coef3", 2.89).toDouble();

    double s1 = sin(dMin);
    double s2 = sin(dMax);

    QString fileName = QString(argv[1]);

    double *ra, *de, *dRa, *dDe, de1, *dTime;
    int i, dSize;

    QVector <double*> dataVect;
    //QVector <double> deVect;

    QFile inpFile(fileName);
    inpFile.open(QIODevice::ReadOnly);
    QTextStream inpStm(&inpFile);

    QString tStr;
    double *data;
    double Eps[3];
    double sgEps[3];
    double EpsVel[6], sgEpsVel[6];
    int res;

    int N, K, P;
    double *sCoef = new double[coefNum];
    double *tCoef = new double[coefNum];
    double *sCoefSg = new double[coefNum];
    double *tCoefSg = new double[coefNum];
    double sigmaVal;

//ini
    QFile resFile("resCoef.txt");
    resFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream resStm(&resFile);
    int dataLen = 5;
    double tMean = 0.0;

    QFile rDeb("./resT.txt");
    rDeb.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream rStm(&rDeb);

    while(!inpStm.atEnd())
    {
        tStr = inpStm.readLine();
        data = new double[dataLen];
        /*data[0] = grad2rad(tStr.section(" ", 2, 2).toDouble());
        data[1] = grad2rad(tStr.section(" ", 3, 3).toDouble());
        data[2] = grad2rad(tStr.section(" ", 4, 4).toDouble());
        data[3] = grad2rad(tStr.section(" ", 5, 5).toDouble());*/
        /*
        data[0] = tStr.section(colSep, cx, cx).toDouble();
        data[1] = tStr.section(colSep, cy, cy).toDouble();
        data[2] = tStr.section(colSep, cdx, cdx).toDouble();
        data[3] = tStr.section(colSep, cdy, cdy).toDouble();*/
        data[0] = tStr.section(colSep, ct, ct).toDouble();

        data[1] = tStr.section(colSep, cx, cx).toDouble();
        data[2] = tStr.section(colSep, cy, cy).toDouble();
        data[3] = tStr.section(colSep, cxc, cxc).toDouble();
        data[4] = tStr.section(colSep, cyc, cyc).toDouble();

        if(isDegree)
        {
            data[1] = grad2rad(data[1]);
            data[2] = grad2rad(data[2]);
            data[3] = grad2rad(data[3]);
            data[4] = grad2rad(data[4]);
        }
        //qDebug() << QString("data: %1\t%2\t%3\t%4\t%5\n").arg(data[0],12, 'f', 8).arg(data[1],12, 'f', 8).arg(data[2],12, 'f', 8).arg(data[3],12, 'f', 8).arg(data[4],12, 'f', 8);
        if(isZonal&&(data[2]<dMin||data[2]>dMax)) continue;

        tMean += data[0];

        dataVect << data;
    }

    dSize = dataVect.size();
    dTime = new double[dSize];
    ra = new double[dSize];
    de = new double[dSize];
    dRa = new double[dSize];//cosD
    dDe = new double[dSize];

    //time0 = floor(tMean/dSize);
    qDebug() << QString("time0: %1\n").arg(time0,12, 'f', 8);

    qDebug() << QString("point num: %1\n").arg(dSize);

    double raC, decC;

    for(i=0; i<dSize; i++)
    {
        dTime[i] = (dataVect[i][0]-time0)/(365.2425);
        ra[i] = dataVect[i][1];
        de[i] = dataVect[i][2];
        raC = dataVect[i][3];
        decC = dataVect[i][4];
        if(solMode==4||solMode==1||solMode==2)
        {
            de[i] = de[i] + M_PI/2.0;
            decC = decC + M_PI/2.0;
        }
        dRa[i] = (ra[i]-raC)*cos(de[i]);
        dDe[i] = de[i] - decC;
        //rStm << QString("%1|%2|%3|%4|%5|%6|%7\n").arg(dTime[i],12, 'f', 8).arg(rad2grad(ra[i]),12, 'e', 8).arg(rad2grad(de[i]),12, 'e', 8).arg(rad2grad(dataVect[i][3]),12, 'e', 8).arg(rad2grad(dataVect[i][4]),12, 'e', 8).arg(rad2mas(dRa[i]),12, 'f', 8).arg(rad2mas(dDe[i]),12, 'f', 8);
        if(isZonal&&(solMode==1))
        {
            de1 = de[i]+dDe[i];
            de[i] = asin((2.0*sin(de[i])/(s2-s1))-(s2+s1)/(s2-s1));
            de1 = asin((2.0*sin(de1)/(s2-s1))-(s2+s1)/(s2-s1));
            dDe[i] = de1-de[i];
        }


    }

    double xival;

    switch(solMode)
    {
    case 0:
        res = lsmCount(ra, de, dRa, dDe, dSize, &Eps[0], &sgEps[0]);
        break;
    case 1:
        res = vsfCount(ra, de, dRa, dDe, dSize, &sCoef[0], &tCoef[0], coefNum, sigmaVal);

        for(i=0; i<coefNum; i++)
        {
            indexes(i+1, N, K, P);
            qDebug() << QString("%1: %2 %3 %4:\t%5\t%6\n").arg(i).arg(N).arg(K).arg(P).arg(rad2mas(sCoef[i])).arg(rad2mas(tCoef[i]));
        }

/*        Eps[0] = tCoef[indexJ(1, 1, 1)-1]/2.89;
        Eps[1] = tCoef[indexJ(1, 1, 0)-1]/2.89;
        Eps[2] = tCoef[indexJ(1, 0, 1)-1]/2.89;
        sgEps[0] = sigmaVal/2.89;
        sgEps[1] = sigmaVal/2.89;
        sgEps[2] = sigmaVal/2.89;*/

        qDebug() << QString("Eps coef: %1\t%2\t%3\n").arg(indexJ(1, 1, 1)-1).arg(indexJ(1, 1, 0)-1).arg(indexJ(1, 0, 1)-1);


        Eps[0] = tCoef[indexJ(1, 1, 1)-1]/coef1;
        Eps[1] = tCoef[indexJ(1, 1, 0)-1]/coef2;
        Eps[2] = tCoef[indexJ(1, 0, 1)-1]/coef3;

        sgEps[0] = sigmaVal/coef1;
        sgEps[1] = sigmaVal/coef2;
        sgEps[2] = sigmaVal/coef3;


        break;

    case 2:
        res = vsfCount_lsm(ra, de, dRa, dDe, dSize, coefNum, &sCoef[0], &tCoef[0], sCoefSg, tCoefSg, xival);

        for(i=0; i<coefNum; i++)
        {
            indexes(i, N, K, P);
            qDebug() << QString("%1: %2 %3 %4:\t%5\t%6\n").arg(i).arg(N).arg(K).arg(P).arg(rad2mas(sCoef[i])).arg(rad2mas(tCoef[i]));
        }
        qDebug() << QString("Eps coef: %1\t%2\t%3\n").arg(indexJ(1, 1, 1)-1).arg(indexJ(1, 1, 0)-1).arg(indexJ(1, 0, 1)-1);

        Eps[0] = tCoef[indexJ(2, 1, 1)-1]/coef1;
        Eps[1] = tCoef[indexJ(2, 1, 0)-1]/coef2;
        Eps[2] = tCoef[indexJ(2, 0, 1)-1]/coef3;

        sgEps[0] = tCoefSg[indexJ(2, 1, 1)-1]/coef1;
        sgEps[1] = tCoefSg[indexJ(2, 1, 0)-1]/coef2;
        sgEps[2] = tCoefSg[indexJ(2, 0, 1)-1]/coef3;
        break;
    case 3:
        res = lsmCountVel(dTime, ra, de, dRa, dDe, dSize, &EpsVel[0], &sgEpsVel[0]);

        break;
    case 4:
        ormlsm(ra, de, dRa, dDe, dSize, tCoef, sCoef, tCoefSg, sCoefSg, coefNum, xival);
        for(i=0; i<coefNum; i++)
        {
            indexes(i, N, K, P);
            qDebug() << QString("%1: %2 %3 %4:\t%5\t%6\n").arg(i).arg(N).arg(K).arg(P).arg(rad2mas(sCoef[i])).arg(rad2mas(tCoef[i]));
        }
        qDebug() << QString("Eps coef: %1\t%2\t%3\n").arg(indexJ(1, 1, 1)-1).arg(indexJ(1, 1, 0)-1).arg(indexJ(1, 0, 1)-1);

        Eps[0] = tCoef[indexJ(2, 1, 1)-1]/coef1;
        Eps[1] = tCoef[indexJ(2, 1, 0)-1]/coef2;
        Eps[2] = tCoef[indexJ(2, 0, 1)-1]/coef3;

        sgEps[0] = tCoefSg[indexJ(2, 1, 1)-1]/coef1;
        sgEps[1] = tCoefSg[indexJ(2, 1, 0)-1]/coef2;
        sgEps[2] = tCoefSg[indexJ(2, 0, 1)-1]/coef3;
        break;
    }

    for(i=0; i<3; i++)
    {
        qDebug() << QString("Eps[%1]: %2\t+-\t%3\n").arg(i).arg(rad2mas(Eps[i])).arg(rad2mas(sgEps[i]));
        resStm << QString("%1\t%2\n").arg(rad2mas(Eps[i])).arg(rad2mas(sgEps[i]));
    }

    resFile.close();
    //qDebug() << QString("sgEps: %1\t%2\t%3\n").arg(rad2mas(sgEps[0])).arg(rad2mas(sgEps[1])).arg(rad2mas(sgEps[2]));


    
    return 0;//a.exec();
}

/*
int lsmCount(double *ra, double *dec, double *dRa, double *dDe, int pointNum, double *Eps, double *sgEps)
{
    int i;

    double *A = new double[3*pointNum*2];
    double *r = new double[pointNum*2];
    double *W = new double[pointNum*2];
    double *Z = new double[3];

    double *Ara = new double[3*pointNum];
    double *rRa = new double[pointNum];
    double *Wra = new double[pointNum];
    double *Zra = new double[3];

    double *Ade = new double[2*pointNum];
    double *rDe = new double[pointNum];
    double *Wde = new double[pointNum];
    double *Zde = new double[2];

    double Dx[3][3];
    double Dra[3][3];
    double Dde[2][2];
    double uwe, uweRa, uweDe;

    for(i=0; i<pointNum; i++)
    {

//        obj = new double[2];
//        objR = new double[2];
        rRa[i] = r[i] = dRa[i];
        rDe[i] = r[pointNum+i] = dDe[i];

        Ara[i*3] = A[i*3] = -(sin(dec[i])*cos(ra[i]));
        Ara[i*3+1] = A[i*3+1] = -(sin(dec[i])*sin(ra[i]));
        Ara[i*3+2] = A[i*3+2] = cos(dec[i]);

        Ade[i*2] = A[pointNum+i*3] = sin(ra[i]);
        Ade[i*2+1] = A[pointNum+i*3+1] = -cos(ra[i]);
        A[pointNum+i*3+2] = 0.0;

        Wra[i] = Wde[i] = W[i] = 1.0;
        W[pointNum+i] = 1.0;

        //qDebug() << QString("%1\t%2\t%3\t = %7\n").arg((A[i*3]),12, 'f', 8).arg((A[i*3+1]),12, 'f', 8).arg((A[i*3+2]),12, 'f', 8).arg(r[i],12, 'f', 8);
        //qDebug() << QString("%1\t%2\t%3\t = %7\n").arg((A[pointNum+i*3]),12, 'f', 8).arg((A[pointNum+i*3+1]),12, 'f', 8).arg((A[pointNum+i*3+2]),12, 'f', 8).arg(r[pointNum+i],12, 'f', 8);
    }


////////////////////////////////////////////////////////////////////
//    qDebug() << QString("Eps: %1\t%2\t%3\n").arg(rad2mas(Eps[0]),12, 'f', 8).arg(rad2mas(Eps[1]),12, 'f', 8).arg(rad2mas(Eps[2]),12, 'f', 8);

    lsm(3, pointNum*2, Z, A, r, uwe, &Dx[0][0], W);

//    slsm(3, pointNum, Z, A, r, W);
//    iLSM(3, pointNum, 0.6, &exclind[0], Z, A, r, uwe, &Dx[0][0], -1, rn, W);
    qDebug() << QString("Z: %1 +- %2\t%3 +- %4\t%5 +- %6\n").arg(rad2mas(Z[0]),12, 'f', 8).arg(rad2mas(sqrt(Dx[0][0])),12, 'f', 8).arg(rad2mas(Z[1]),12, 'f', 8).arg(rad2mas(sqrt(Dx[1][1])),12, 'f', 8).arg(rad2mas(Z[2]),12, 'f', 8).arg(rad2mas(sqrt(Dx[2][2])),12, 'f', 8);
    qDebug() << QString("uwe: %1\n").arg(rad2mas(sqrt(uwe)),12, 'f', 8);
//    slsm(2, pointNum, Zd, Ad, rd, Wd);
  //  qDebug() << "Zd: " << Zd[0] << "\t" << Zd[1] << "\n";
//    qDebug() << QString("Zd: %1\t%2\n").arg(Zd[0],12, 'f', 8).arg(Zd[1],12, 'f', 8);

    lsm(3, pointNum, Zra, Ara, rRa, uweRa, &Dra[0][0], Wra);
    qDebug() << QString("Zra: %1 +- %2\t%3 +- %4\t%5 +- %6\n").arg(rad2mas(Zra[0]),12, 'f', 8).arg(rad2mas(sqrt(Dra[0][0])),12, 'f', 8).arg(rad2mas(Zra[1]),12, 'f', 8).arg(rad2mas(sqrt(Dra[1][1])),12, 'f', 8).arg(rad2mas(Zra[2]),12, 'f', 8).arg(rad2mas(sqrt(Dra[2][2])),12, 'f', 8);
    qDebug() << QString("uweRa: %1\n").arg(rad2mas(sqrt(uweRa)),12, 'f', 8);

    lsm(2, pointNum, Zde, Ade, rDe, uweDe, &Dde[0][0], Wde);
    qDebug() << QString("Zde: %1 +- %2\t%3 +- %4\n").arg(rad2mas(Zde[0]),12, 'f', 8).arg(rad2mas(sqrt(Dde[0][0])),12, 'f', 8).arg(rad2mas(Zde[1]),12, 'f', 8).arg(rad2mas(sqrt(Dde[1][1])),12, 'f', 8);
    qDebug() << QString("uweDe: %1\n").arg(rad2mas(sqrt(uweDe)),12, 'f', 8);

    Eps[0] = Zde[0];
    Eps[1] = Zde[1];
    Eps[2] = Zra[2];

    sgEps[0] = sqrt(Dde[0][0]);
    sgEps[1] = sqrt(Dde[1][1]);
    sgEps[2] = sqrt(Dra[2][2]);

    return 0;
}

int lsmCountVel(double *dTime, double *ra, double *dec, double *dRa, double *dDe, int pointNum, double *Eps, double *sgEps)
{
    int i;

    double *A = new double[6*pointNum*2];
    double *r = new double[pointNum*2];
    double *W = new double[pointNum*2];
    double *Z = new double[6];

    double *Ara = new double[6*pointNum];
    double *rRa = new double[pointNum];
    double *Wra = new double[pointNum];
    double *Zra = new double[6];

    double *Ade = new double[4*pointNum];
    double *rDe = new double[pointNum];
    double *Wde = new double[pointNum];
    double *Zde = new double[4];

    double Dx[6][6];
    double Dra[6][6];
    double Dde[4][4];
    double uwe, uweRa, uweDe;

    for(i=0; i<pointNum; i++)
    {

//        obj = new double[2];
//        objR = new double[2];
        rRa[i] = r[i] = dRa[i];
        rDe[i] = r[pointNum+i] = dDe[i];

        Ara[i*6] = A[i*6] = -(sin(dec[i])*cos(ra[i]));
        Ara[i*6+1] = A[i*6+1] = -(sin(dec[i])*sin(ra[i]));
        Ara[i*6+2] = A[i*6+2] = cos(dec[i]);
        Ara[i*6+3] = A[i*6+3] = -(sin(dec[i])*cos(ra[i]))*dTime[i];
        Ara[i*6+4] = A[i*6+4] = -(sin(dec[i])*sin(ra[i]))*dTime[i];
        Ara[i*6+5] = A[i*6+5] = cos(dec[i])*dTime[i];

        Ade[i*4] = A[pointNum+i*6] = sin(ra[i]);
        Ade[i*4+1] = A[pointNum+i*6+1] = -cos(ra[i]);
        A[pointNum+i*6+2] = 0.0;
        Ade[i*4+2] = A[pointNum+i*6+3] = sin(ra[i])*dTime[i];
        Ade[i*4+3] = A[pointNum+i*6+4] = -cos(ra[i])*dTime[i];
        A[pointNum+i*3+5] = 0.0;

        Wra[i] = Wde[i] = W[i] = 1.0;
        W[pointNum+i] = 1.0;

        //qDebug() << QString("dTime = %1\n").arg(dTime[i],12, 'f', 8);

        //qDebug() << QString("%1\t%2\t%3\t %4\t%5\t%6 = %7\n").arg((A[i*6]),12, 'f', 8).arg((A[i*6+1]),12, 'f', 8).arg((A[i*6+2]),12, 'f', 8).arg((A[i*6+3]),12, 'f', 8).arg((A[i*6+4]),12, 'f', 8).arg((A[i*6+5]),12, 'f', 8).arg((r[i]),12, 'f', 8);
        //qDebug() << QString("%1\t%2\t%3\t %4\t%5\t%6 = %7\n").arg(rad2mas(A[pointNum+i*6]),12, 'f', 8).arg(rad2mas(A[pointNum+i*6+1]),12, 'f', 8).arg(rad2mas(A[pointNum+i*6+2]),12, 'f', 8).arg(rad2mas(A[pointNum+i*6+3]),12, 'f', 8).arg(rad2mas(A[pointNum+i*6+4]),12, 'f', 8).arg(rad2mas(A[pointNum+i*6+5]),12, 'f', 8).arg(rad2mas(r[pointNum+i]),12, 'f', 8);
    }


////////////////////////////////////////////////////////////////////
//    qDebug() << QString("Eps: %1\t%2\t%3\n").arg(rad2mas(Eps[0]),12, 'f', 8).arg(rad2mas(Eps[1]),12, 'f', 8).arg(rad2mas(Eps[2]),12, 'f', 8);

    lsm(6, pointNum*2, Z, A, r, uwe, &Dx[0][0], W);

//    slsm(3, pointNum, Z, A, r, W);
//    iLSM(3, pointNum, 0.6, &exclind[0], Z, A, r, uwe, &Dx[0][0], -1, rn, W);
    qDebug() << QString("Z: %1 +- %2\t%3 +- %4\t%5 +- %6\n").arg(rad2mas(Z[0]),12, 'f', 8).arg(rad2mas(sqrt(Dx[0][0])),12, 'f', 8).arg(rad2mas(Z[1]),12, 'f', 8).arg(rad2mas(sqrt(Dx[1][1])),12, 'f', 8).arg(rad2mas(Z[2]),12, 'f', 8).arg(rad2mas(sqrt(Dx[2][2])),12, 'f', 8);
    qDebug() << QString("Zvel: %1 +- %2\t%3 +- %4\t%5 +- %6\n").arg(rad2mas(Z[3]),12, 'f', 8).arg(rad2mas(sqrt(Dx[3][3])),12, 'f', 8).arg(rad2mas(Z[4]),12, 'f', 8).arg(rad2mas(sqrt(Dx[4][4])),12, 'f', 8).arg(rad2mas(Z[5]),12, 'f', 8).arg(rad2mas(sqrt(Dx[5][5])),12, 'f', 8);
    qDebug() << QString("uwe: %1\n").arg(rad2mas(sqrt(uwe)),12, 'f', 8);
//    slsm(2, pointNum, Zd, Ad, rd, Wd);
  //  qDebug() << "Zd: " << Zd[0] << "\t" << Zd[1] << "\n";
//    qDebug() << QString("Zd: %1\t%2\n").arg(Zd[0],12, 'f', 8).arg(Zd[1],12, 'f', 8);

    lsm(6, pointNum, Zra, Ara, rRa, uweRa, &Dra[0][0], Wra);
    qDebug() << QString("Zra: %1 +- %2\t%3 +- %4\t%5 +- %6\n").arg(rad2mas(Zra[0]),12, 'f', 8).arg(rad2mas(sqrt(Dra[0][0])),12, 'f', 8).arg(rad2mas(Zra[1]),12, 'f', 8).arg(rad2mas(sqrt(Dra[1][1])),12, 'f', 8).arg(rad2mas(Zra[2]),12, 'f', 8).arg(rad2mas(sqrt(Dra[2][2])),12, 'f', 8);
    qDebug() << QString("ZraVel: %1 +- %2\t%3 +- %4\t%5 +- %6\n").arg(rad2mas(Zra[3]),12, 'f', 8).arg(rad2mas(sqrt(Dra[3][3])),12, 'f', 8).arg(rad2mas(Zra[4]),12, 'f', 8).arg(rad2mas(sqrt(Dra[4][4])),12, 'f', 8).arg(rad2mas(Zra[5]),12, 'f', 8).arg(rad2mas(sqrt(Dra[5][5])),12, 'f', 8);
    qDebug() << QString("uweRa: %1\n").arg(rad2mas(sqrt(uweRa)),12, 'f', 8);

    lsm(4, pointNum, Zde, Ade, rDe, uweDe, &Dde[0][0], Wde);
    qDebug() << QString("Zde: %1 +- %2\t%3 +- %4\n").arg(rad2mas(Zde[0]),12, 'f', 8).arg(rad2mas(sqrt(Dde[0][0])),12, 'f', 8).arg(rad2mas(Zde[1]),12, 'f', 8).arg(rad2mas(sqrt(Dde[1][1])),12, 'f', 8);
    qDebug() << QString("Zde: %1 +- %2\t%3 +- %4\n").arg(rad2mas(Zde[2]),12, 'f', 8).arg(rad2mas(sqrt(Dde[2][2])),12, 'f', 8).arg(rad2mas(Zde[3]),12, 'f', 8).arg(rad2mas(sqrt(Dde[3][3])),12, 'f', 8);
    qDebug() << QString("uweDe: %1\n").arg(rad2mas(sqrt(uweDe)),12, 'f', 8);

/*
    Eps[0] = Zde[0];
    Eps[1] = Zde[1];
    Eps[2] = Zra[2];

    sgEps[0] = sqrt(Dde[0][0]);
    sgEps[1] = sqrt(Dde[1][1]);
    sgEps[2] = sqrt(Dra[2][2]);
/
    Eps[0] = Z[0];
    Eps[1] = Z[1];
    Eps[2] = Z[2];
    Eps[3] = Z[3];
    Eps[4] = Z[4];
    Eps[5] = Z[5];

    sgEps[0] = sqrt(Dde[0][0]);
    sgEps[1] = sqrt(Dde[1][1]);
    sgEps[2] = sqrt(Dra[2][2]);
    sgEps[3] = sqrt(Dra[3][3]);
    sgEps[4] = sqrt(Dra[4][4]);
    sgEps[5] = sqrt(Dra[5][5]);

    return 0;
}

int vsfCount_lsm(double *ra, double *dec, double *dRa, double *dDe, int pointNum, int coefNum, double *sCoef, double *tCoef, double *sCoefSg, double *tCoefSg)
{
    int i, j, k;


    double *Ara = new double[coefNum*pointNum*2];
    double *rRa = new double[pointNum];
    double *Wra = new double[pointNum];
    double *Zra = new double[coefNum*2];

    double *Ade = new double[2*pointNum*coefNum];
    double *rDe = new double[pointNum];
    double *Wde = new double[pointNum];
    double *Zde = new double[coefNum*2];

    double Dx[3][3];
    double Dra[coefNum*2][coefNum*2];
    double Dde[coefNum*2][coefNum*2];
    double uwe, uweRa, uweDe;

    for(i=0; i<pointNum; i++)
    {

//        obj = new double[2];
//        objR = new double[2];
        rRa[i] = dRa[i];
        rDe[i] = dDe[i];

        for(j=0; j<coefNum; j++)
        {
            k= (i*2*coefNum);
            Ara[k+j*2] =  SLJ(j+1, ra[i], dec[i]);//sCoef[j]*SLJ(j+1, ra[i], dec[i]) + tCoef[j]*TLJ(j+1, ra[i], dec[i]);
            Ara[k+j*2+1] = TLJ(j+1, ra[i], dec[i]);
            Ade[k+j*2] =   SBJ(j+1, ra[i], dec[i]);
            Ade[k+j*2+1] = TBJ(j+1, ra[i], dec[i]);
        }
        Wra[i] = Wde[i]  = 1.0;
    }


////////////////////////////////////////////////////////////////////
//    qDebug() << QString("Eps: %1\t%2\t%3\n").arg(rad2mas(Eps[0]),12, 'f', 8).arg(rad2mas(Eps[1]),12, 'f', 8).arg(rad2mas(Eps[2]),12, 'f', 8);


    lsm(coefNum*2, pointNum, Zra, Ara, rRa, uweRa, &Dra[0][0], Wra);
    //qDebug() << QString("Zra: %1 +- %2\t%3 +- %4\t%5 +- %6\n").arg(rad2mas(Zra[0]),12, 'f', 8).arg(rad2mas(sqrt(Dra[0][0])),12, 'f', 8).arg(rad2mas(Zra[1]),12, 'f', 8).arg(rad2mas(sqrt(Dra[1][1])),12, 'f', 8).arg(rad2mas(Zra[2]),12, 'f', 8).arg(rad2mas(sqrt(Dra[2][2])),12, 'f', 8);
    //qDebug() << QString("uweRa: %1\n").arg(rad2mas(sqrt(uweRa)),12, 'f', 8);

    lsm(coefNum*2, pointNum, Zde, Ade, rDe, uweDe, &Dde[0][0], Wde);
    //qDebug() << QString("Zde: %1 +- %2\t%3 +- %4\n").arg(rad2mas(Zde[0]),12, 'f', 8).arg(rad2mas(sqrt(Dde[0][0])),12, 'f', 8).arg(rad2mas(Zde[1]),12, 'f', 8).arg(rad2mas(sqrt(Dde[1][1])),12, 'f', 8);
    //qDebug() << QString("uweDe: %1\n").arg(rad2mas(sqrt(uweDe)),12, 'f', 8);

    for(i=0;i<coefNum;i++)
    {
        sCoef[i] = Zra[i*2];
        tCoef[i] = Zra[i*2+1];
        sCoefSg[i] = sqrt(Dde[i*2][i*2]);
        tCoefSg[i] = sqrt(Dde[i*2+1][i*2+1]);
        qDebug() << QString("coefRA[%1]: %2\t%3\n").arg(indexesStr(i+1)).arg(rad2mas(Zra[i*2])).arg(rad2mas(Zra[i*2+1]));
        qDebug() << QString("coefDE[%1]: %2\t%3\n").arg(indexesStr(i+1)).arg(rad2mas(Zde[i*2])).arg(rad2mas(Zde[i*2+1]));
    }

    return 0;
}

int vsfCount(double *ra, double *dec, double *dRa, double *dDe, int pointNum, double *sCoef, double *tCoef, int coefNum, double &sigmaVal)
{
    int i, j;

    for(j=0; j<coefNum; j++)
    {
    tCoef[j] = 0;
    sCoef[j] = 0;
    }

    for(i=0; i<pointNum; i++)
    {
        for(j=0; j<coefNum; j++)
        {
            //indexes(j, n, k, p);
            /*
            Eps[0] += (dRa[i]*TL(1, 1, 1, ra[i], dec[i])+dDe[i]*TB(1, 1, 1,ra[i], dec[i]));//*cos(dec[i]);
            Eps[1] += (dRa[i]*TL(1, 1, 0, ra[i], dec[i])+dDe[i]*TB(1, 1, 0,ra[i], dec[i]));//*cos(dec[i]);
            Eps[2] += (dRa[i]*TL(1, 0, 1, ra[i], dec[i])+dDe[i]*TB(1, 0, 1,ra[i], dec[i]));//*cos(dec[i]);/
            tCoef[j] += (dRa[i]*TLJ(j+1, ra[i], dec[i])+dDe[i]*TBJ(j+1,ra[i], dec[i]))*4*PI/pointNum;
            sCoef[j] += (dRa[i]*SLJ(j+1, ra[i], dec[i])+dDe[i]*SBJ(j+1,ra[i], dec[i]))*4*PI/pointNum;
        }
    }

    double *vsfRA, *vsfDE;
    vsfRA = new double[pointNum];
    vsfDE = new double[pointNum];
    double dra1, dde1;

    for(i=0; i<pointNum; i++)
    {
        dra1 = 0;
        dde1 = 0;
        for(j=0; j<coefNum; j++)
        {
            dra1 += sCoef[j]*SLJ(j+1, ra[i], dec[i]) + tCoef[j]*TLJ(j+1, ra[i], dec[i]);
            dde1 += sCoef[j]*SBJ(j+1, ra[i], dec[i]) + tCoef[j]*TBJ(j+1, ra[i], dec[i]);
        }
        vsfRA[i] = dRa[i] - dra1;
        vsfDE[i] = (dDe[i] - dde1);
    }

    double meanRa, meanDe, rmsOneRa, rmsOneDe, rmsMeanRa, rmsMeanDe;

    meanRa = meanDe = rmsOneRa = rmsOneDe = rmsMeanRa = rmsMeanDe = 0.0;

    for(i=0; i<pointNum; i++)
    {
        meanRa += vsfRA[i];
        meanDe += vsfDE[i];
    }

    meanRa /= pointNum;
    meanDe /= pointNum;

    for(i=0; i<pointNum; i++)
    {
        rmsOneRa += pow(vsfRA[i]-meanRa, 2.0);
        rmsOneDe += pow(vsfDE[i]-meanDe, 2.0);
    }
    rmsOneRa = sqrt(rmsOneRa/(pointNum-1));
    rmsOneDe = sqrt(rmsOneDe/(pointNum-1));

    rmsMeanRa = rmsOneRa/sqrt(pointNum);
    rmsMeanDe = rmsOneDe/sqrt(pointNum);

    double sigmaVal0 = rmsMeanRa/2.0*coefNum;
    double sigmaVal1 = rmsMeanDe/2.0*coefNum;


    sigmaVal = sqrt((sigmaVal0*sigmaVal0 + sigmaVal1*sigmaVal1))/2.0;

    qDebug() << QString("ra: %1\t%2\t%3\n").arg(rad2mas(meanRa)).arg(rad2mas(rmsOneRa), 10, 'e').arg(rad2mas(rmsMeanRa), 10, 'e');
    qDebug() << QString("de: %1\t%2\t%3\n").arg(rad2mas(meanDe)).arg(rad2mas(rmsOneDe), 10, 'e').arg(rad2mas(rmsMeanDe), 10, 'e');

    qDebug() << QString("sigma: %1\t%2\t:%3\n").arg(rad2mas(sigmaVal0)).arg(rad2mas(sigmaVal1)).arg(rad2mas(sigmaVal));
}
*/

