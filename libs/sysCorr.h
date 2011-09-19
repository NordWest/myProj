#ifndef SYSCORR_H
#define SYSCORR_H

#include "astro.h"
#include "vectGrid3D.h"

//  syscorr ////////////////////////////

double magEqCorr(double mag, double x, QList <double*> mCoefX, int mDeg);
double comaCorr(double x, double mag, double c, double mag0);




struct magEqCorrParam
{
    QList <double*> mCoefX, mCoefY;
    int mCoefDeg;

    int init(QString xCoefFileName, QString yCoefFileName, QString dataSep);
    double corrX(double x, double magn);
    double corrY(double y, double magn);
};

struct comaCorrParam
{
    double cKsi, cEta, cMag0;
    double dKsi(double ksi, double mag);
    double dEta(double eta, double mag);
};

struct vfCorrParam
{
    vectGrid3D *vectFcorr;
    double rMax;
    int corrType;   //0-int2D, 1-intRad, 2-intIDW, 3-intRadM, 4-intIdwM
    int nMin, mDeg, isW;
    int init(QString vfFileName, double rmax, int nmin, int corrType, int ndeg = 2, int isW = 0);
    int detCorr(double *dx, double *dy, long *ni, double x, double y, double magn);
};

struct mdCorrParam
{
    QString mdCorrX, mdCorrY;
    QString colSep;
    int corrModel;
    QList <double*> mdCoefX, mdCoefY;
    int mdCoefXsz, mdCoefYsz, dCoefXsz, dCoefYsz;

    int init(QString mdcorrx, QString mdcorry, QString colsep, int corrmodel);
};

struct reductionParams
{
    double maxres;
    double maxresMAG;
    double sigma;
    int weights;
    int minRefStars;
    int redType;
    double sMax;
    double uweMax;
    int maxRefStars;
 };

class sysCorrParam
{
public:
    int isMagEqCorr;
    int isComaCorr;
    int isVfCorr0;
    int isVfCorr1;
    int isMdCorr;


    comaCorrParam *comaCorr;
    magEqCorrParam *magEqCorr;
    vfCorrParam *vfCorr0, *vfCorr1;
    mdCorrParam *mdCorr;

    sysCorrParam();
    int init(QString confName);

};

#endif // SYSCORR_H
