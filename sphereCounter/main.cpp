#include <QtCore/QCoreApplication>

#include <QFile>
#include <QSettings>
#include <QTextStream>
#include <QList>


#include "./../libs/astro.h"
#include "./../libs/comfunc.h"
#include "./../libs/mb.h"
#include "./../libs/ringpix.h"

#include "./vsfFunc.h"

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

int lsmCount(double *ra, double *de, double *dRa, double *dDe, int pointNum, double *Eps);
int vsfCount(double *ra, double *de, double *dRa, double *dDe, int pointNum, double *sCoef, double *tCoef, int coefNum, double &sigmaVal);


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
    int isZonal = sett->value("general/isZonal", 0).toInt();
    double dMin = grad2rad(sett->value("general/dMin", -90).toDouble());
    double dMax = grad2rad(sett->value("general/dMax", 90).toDouble());
    int coefNum = sett->value("general/coefNum", 9).toInt();

    double s1 = sin(dMin);
    double s2 = sin(dMax);

    QString fileName = QString(argv[1]);

    double *ra, *de, *dRa, *dDe, de1;
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
    int res;

    while(!inpStm.atEnd())
    {
        tStr = inpStm.readLine();
        data = new double[4];
        data[0] = grad2rad(tStr.section(" ", 2, 2).toDouble());
        data[1] = grad2rad(tStr.section(" ", 3, 3).toDouble());
        data[2] = grad2rad(tStr.section(" ", 4, 4).toDouble());
        data[3] = grad2rad(tStr.section(" ", 5, 5).toDouble());
        if(isZonal&&(data[1]<dMin||data[1]>dMax)) continue;

        dataVect << data;
    }

    dSize = dataVect.size();
    ra = new double[dSize];
    de = new double[dSize];
    dRa = new double[dSize];
    dDe = new double[dSize];

    int N, K, P;
    double *sCoef = new double[coefNum];
    double *tCoef = new double[coefNum];
    double sigmaVal;

    qDebug() << QString("point num: %1\n").arg(dSize);

    for(i=0; i<dSize; i++)
    {
        ra[i] = dataVect[i][0];

        de[i] = dataVect[i][1];
        dRa[i] = dataVect[i][2];
        dDe[i] = dataVect[i][3];
        if(isZonal)
        {
            de1 = de[i]+dDe[i];
            de[i] = asin((2.0*sin(de[i])/(s2-s1))-(s2+s1)/(s2-s1));
            de1 = asin((2.0*sin(de1)/(s2-s1))-(s2+s1)/(s2-s1));
            dDe[i] = de1-de[i];
        }
    }

    switch(solMode)
    {
    case 0:
        res = lsmCount(ra, de, dRa, dDe, dSize, &Eps[0]);
        break;
    case 1:
        res = vsfCount(ra, de, dRa, dDe, dSize, &sCoef[0], &tCoef[0], coefNum, sigmaVal);

        for(i=0; i<coefNum; i++)
        {
            indexes(i, N, K, P);
            qDebug() << QString("%1: %2 %3 %4:\t%5\t%6\n").arg(i).arg(N).arg(K).arg(P).arg(rad2mas(sCoef[i])).arg(rad2mas(tCoef[i]));
        }

        Eps[0] = tCoef[indexJ(1, 1, 1)-1]/2.89;
        Eps[1] = tCoef[indexJ(1, 1, 0)-1]/2.89;
        Eps[2] = tCoef[indexJ(1, 0, 1)-1]/2.89;

        sgEps[0] = sigmaVal/2.89;
        sgEps[1] = sigmaVal/2.89;
        sgEps[2] = sigmaVal/2.89;

        break;
    }

    qDebug() << QString("Eps: %1\t%2\t%3\n").arg(rad2mas(Eps[0])).arg(rad2mas(Eps[1])).arg(rad2mas(Eps[2]));
    qDebug() << QString("sgEps: %1\t%2\t%3\n").arg(rad2mas(sgEps[0])).arg(rad2mas(sgEps[1])).arg(rad2mas(sgEps[2]));


    
    return 0;//a.exec();
}


int lsmCount(double *ra, double *dec, double *dRa, double *dDe, int pointNum, double *Eps)
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

    Eps[0] = Zra[0];
    Eps[1] = Zra[1];
    Eps[2] = Zra[2];

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
            Eps[2] += (dRa[i]*TL(1, 0, 1, ra[i], dec[i])+dDe[i]*TB(1, 0, 1,ra[i], dec[i]));//*cos(dec[i]);*/
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

    sigmaVal = (rmsMeanRa + rmsMeanDe)/(2.0*coefNum);

    qDebug() << QString("ra: %1\t%2\t%3\n").arg(rad2mas(meanRa)).arg(rad2mas(rmsOneRa), 10, 'e').arg(rad2mas(rmsMeanRa), 10, 'e');
    qDebug() << QString("de: %1\t%2\t%3\n").arg(rad2mas(meanDe)).arg(rad2mas(rmsOneDe), 10, 'e').arg(rad2mas(rmsMeanDe), 10, 'e');
}
