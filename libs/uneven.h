//
#include <complex>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QSettings>

//#include "./alglib/ap.h"

#include "./mb.h"

#define STRLEN 1024
#define PI atan(1)*4

//using namespace alglib;
using namespace std;
double mod(complex<double> val);

struct harmParam
{
    double A, P, Fi;
};
double detHarm(double t, QList <harmParam*> resList);



struct cleanParam
{
    double g, q, xi;
};



void sortTvect(double* tVect, double* fVect, int szi);

int makePoly(double *f_time, double *F, int N, double* resCoef, int polDeg, int isLog = 0);
double detPoly(double x, double* coefV, int polDeg);

int makeClean(double *f_time, double *F, int N, cleanParam cp, QList <harmParam*> &resList, int isLog = 0);


