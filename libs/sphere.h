#ifndef SPHERE_H
#define SPHERE_H

#include <math.h>
#include <QString>
#include <QDebug>
#include <mb.h>
#include <astro.h>

#include "ringpix.h"
#include "vsfFunc.h"

int lsmCount(double *ra, double *de, double *dRa, double *dDe, int pointNum, double *Eps, double *sgEps);
int lsmCountVel(double *dTime, double *ra, double *dec, double *dRa, double *dDe, int pointNum, double *Eps, double *sgEps);
int vsfCount(double *ra, double *de, double *dRa, double *dDe, int pointNum, double *sCoef, double *tCoef, int coefNum, double &sigmaVal);
int vsfCount_lsm(double *ra, double *dec, double *dRa, double *dDe, int pointNum, int coefNum, double *sCoef, double *tCoef, double *sCoefSg, double *tCoefSg);

#endif // SPHERE_H
