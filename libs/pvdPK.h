#pragma once

#include "rec.h"
#include "pvdK.h"
#include "orbit.h"
//#include "DynArr.h"
#include "comfunc.h"

#define N 100
#define MAX_ITER 250
#define CKEY_RANGE 1e-3

struct chvar
{
	double d, dt, dtt, r[3], rnorm, rt[3], rtnorm, f[N], rtt[3];
	double ksi[3], ksit[3], ksitt[3];
	myvector *R;
	int iter;
	
};

struct ComVar
{
	myvector *g, *gt, *gtt;
	myvector *g0, *g1;
	double g2, gt2, gtt2;
	myvector *T;
	myvector *R;
	myvector *G;
	myvector *M;

	double t0, t, t1;
	double r0, d0, r1, d1, r, d;

	double gsD, gtsD, gtsDt, gtsT, gttsT, RsT, gsT, RsM, gttsM, gsM, GsM, signGM;

	chvar chv[N];
//	double d[N], dt[N], r[N][3], rt[N][3], f[N][N];
	int last;

	int Ckey;	//type of iter compute: 0 - (c-1.0)>>0.0;	1 - (c-1.0)<<0.0
	double inird;
};
/*
struct logstuff
{
//	int klog;
	FILE *FLOG;
	char *suff;

	logstuff();
	~logstuff();
};
*/

class pvdP
{
public:
	rec *sun;
	pvdK *vpos;

	orbit *res;

	ComVar *comvar;

	logstuff *slog;
	int klog;

	int fini;
	FILE *FINI;
//	char *suff;

	pvdP();
	pvdP(int log);
	pvdP(char *fnrec, char *fnpvd);
	pvdP(char *fnrec, char *fnpvd, char *fnini, int log);
	~pvdP();

	int compute();
//	int appFerst();
	int appFerst1();
	int appSecond();
	int computeOrb();
	int computeOrb1();

	int AccDelive();

	int findSqr();

	int initFiles(char *fnrec, char *fnpvd, char *fnini);
	int initVar();
	int initC();
//	int chain(chvar *ur);//double *f, double *d, double *dt, double *r1, double *r2, double *r3, double *rt1, double *rt2, double *rt3);
//	int chain1(chvar *ur);
//	int comComp(double p1, double p2);
	int provTDM();
	int provRGD();
	int proofR();
	int proofSM();
	int proofPVD();

//	int proofRV(myvector *r, myvector *v);

	int iterR(double r, int *niter);
	int iterD(double d, int *niter);
	int iterRc(double r, int *niter);
	int iterDc(double d, int *niter);
	int iterDcD(int *niter);

	int iterEnd(double r, double d, double dt, myvector *R, int niter, int nitype);

	int logging(char *basename);

	int aclose();
	int detCkey();
	int printOC(double aV, double dV, double a, double d, double t);
};
