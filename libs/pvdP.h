#pragma once

#ifndef REC_H
#include "rec.h"
#endif

//#include "pvdK.h"
#ifndef ORB_H
#include "orbit.h"
#endif

#ifndef OBSRY_H
#include "observatory.h"
#endif
//#include "DynArr.h"

#ifndef COMF_H
#include "comfunc.h"
#endif

#ifndef COORD_H
#include "coord.h"
#endif

#ifndef PVDK_H
#include "pvdK.h"
#endif

#ifndef AST_H
#include "ast.h"
#endif

#ifndef PVDP_H

#define N 100
#define MAX_ITER 250
#define CKEY_RANGE 1e-3

struct chvar
{
	double d, dt, dtt, r[3], rnorm, rt[3], rtnorm, f[N], rtt[3];
	double ksi[3], ksit[3], ksitt[3];
	double D[3], Dt[3], Dtt[3];
	myvector *R;
	int iter;
	double oc;
	
};

double modVect(double *vect);

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

	double gsD, gtsD, gtsDt, gtsT, gttsT, RsT, gsT, RsM, gttsM, gsM, GsM, signGM, GsD;

	chvar chv[N];
//	double d[N], dt[N], r[N][3], rt[N][3], f[N][N];
	int last;

	int Ckey;	//type of iter compute: 0 - (c-1.0)>>0.0;	1 - (c-1.0)<<0.0
	double inird;

	double *testVal;

	int numOfPR;

	char *obj_name;
};

class pvdP
{
public:
	observ *obs;
	pvdK *vpos;
//	rec *earth;
//	List4 *coord;

	orbit *res;
	CommetOrbCat *resCat;

	CommetOrbCat *baseCat;

	ComVar *comvar;

	logstuff *slog;
	int klog;

	int fini;
	FILE *FINI;

	char *suff;
	char *iniPath;
//	char *mpcFile;

	pvdP();
//	pvdP(int log);
//	pvdP(char *fn_dele_head, char *fn_dele, char *fn_obs, char *fnpvd);
//	pvdP(char *fn_dele_head, char *fn_dele, char *fn_obs, char *fnpvd, char *fnini, int log);
	~pvdP();

	int compute();
//	int appFerst();
	int appFerst1();
	int appSecond();
	int computeOrb();
	int computeOrb1();
	int computeOrb2();


	int AccDelive();

	int findSqr();

	int is_init_files;
	int is_init_data;
	int is_init_params;

	int init();
	int initFiles(char *fn_dele_head, char *fn_dele, char *fn_obs);	//взять установочные файлы.
	int initFiles();												//взять установочные файлы.
	int initAddFiles();												//взять установочные файлы.
	int initData(char *fnpvd);										//взять наблюдения.
	int initDataF(char *fnpvd);	
	int initParams(char *fnini);									//взять параметры из файла.
	int initParams(int ckey, double inird, int tlen);				//задать параметры.
	int initVarConst();												//найти постоянные переменные
	int initVar();													//найти переменные
	int initC();			
	int initVarTest(char *testVarFN);								//заполнить масив отладочных параметров
//	int chain(chvar *ur);//double *f, double *d, double *dt, double *r1, double *r2, double *r3, double *rt1, double *rt2, double *rt3);
//	int chain1(chvar *ur);
//	int comComp(double p1, double p2);
	int provTDM();
	int provRGD();
	int proofR();
	int proofSM();
	int proofPVD();
	int writePVD();

	int detMode(char *fname);
	int idatamode;

//	int proofRV(myvector *r, myvector *v);

	int iterR(double r, int *niter);
	int iterD(double d, int *niter);
	int iterRc(double r, int *niter);
	int iterDc(double d, int *niter);
	int iterDcD(double r, int *niter);

	int iterEnd(double r, double d, double dt, myvector *R, int niter, int nitype);

	int logging(char *basename);

	int aclose();
	int detCkey();
	int ParallaxCorrection();

	int printOC(char *name, double aV, double dV, double a, double d, double t, double *oc);

	int writeCat();
private:
	void set_suff(char *fn);
};

#define PVDP_H
#endif