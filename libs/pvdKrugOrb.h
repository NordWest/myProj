#ifndef COMFUNC_H
#include "comfunc.h"
#endif

#ifndef REC_H
#include "rec.h"
#endif

#ifndef PVDK_H
#include "pvdK.h"
#endif

#ifndef COORD_H
#include "coord.h"
#endif

#ifndef AST_H
#include "ast.h"
#endif

#ifndef MPCS_H
#include "mpcs.h"
#endif

#ifndef ORBCAT_H
#include "orbcat.h"
#endif

#ifndef ORB_H
#include "orbit.h"
#endif

#ifndef OC_H
#include "oc.h"
#endif

#ifndef PVD_KRUG_ORB_H

#define N 100

struct chvar
{
	double d, dt, dtt, r[3], rnorm, rt[3], rtnorm, f[N], rtt[3];
	double ksi[3], ksit[3], ksitt[3];
//	myvector *R;
	int iter;
	
};

class PvdVars
{
public:
//	myvector *g, *gt, *gtt;
//	myvector *g0, *g1;
	double g2, gt2, gtt2;
//	myvector *T;

	myvector *r, *g, *d;
	myvector *rt, *gt, *dt;
	myvector *rtt, *gtt, *dtt;
	myvector *R, *G, *D;
	myvector *Dt, *Dtt;
	myvector *I, *J, *K;
	myvector *Q, *V;
	myvector *M, *T;
	double t;

	PvdVars();
	~PvdVars();

	int iter;
//	myvector *G;

//	myvector *M;

//	double t0, t, t1;
//	double r0, d0, r1, d1, r, d;

//	double gsD, gtsD, gtsDt, gtsT, gttsT, RsT, gsT, RsM, gttsM, gsM, GsM, signGM, GsD;

//	chvar chv[N];
//	double d[N], dt[N], r[N][3], rt[N][3], f[N][N];
	int last;

	int Ckey;	//type of iter compute: 0 - (c-1.0)>>0.0;	1 - (c-1.0)<<0.0
	double inird;
};

class pvdKrugOrb
{
public:
	pvdK *vpos;
	observ *obs;
	PvdVars *vars;

	char *fname;
	char *flog;
	char *foc;
	char *fhat;
	char *fres;

	char *fheader;
	char *fbin;
	char *fobs;
	char *inipref;
	int initFnames();

	int klog;
	FILE *FLOG;

	double sg;
	double rmin, rmax, rstep;
//	int niter;
	double progr;

	pvdKrugOrb();
	~pvdKrugOrb();

	int initEnv(char *fn_dele_h, char *fn_dele_bin, char *fn_obs);
	int initPrefix(char *prefix);

	int initObs(char *fn_pvd, int file_mod);	//0 - ast; 1 - mpc;
	int initVar();

	int compute();
	int computeIter();
	int compOnce(double r);
	int compOnceIter(double r);
	int appFirst(double r0);
	int appFirstIter(double *r0, int *iterR);

	int computeOrb1();
	int computeOrb();

	int computeOC();

//	OrbCat *res;
	orbit *orb;

private:
	double det_k(double r);
	double det_k1(double r);
	double det_m(double r);
	void det_R(double k, double m);
	double det_VM(double r);
	double det_VT(double r);
};

#define PVD_KRUG_ORB_H
#endif