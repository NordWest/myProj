/*
	Определение ПВД по Лапласу
*/


#pragma once

#include "observ.h"
#include "DynArr.h"
#include "comfunc.h"

#define TLEN 3

struct valacc
{
	double val;
	double acc;
};

class diffTable
{
public:
	myvector *x;
	mymatrix *f;
	diffTable();
	~diffTable();
	diffTable(int fRsize, int fCsize, double **farr, int xsize, double *xarr);
};

class pvd
{
public:
	double t;
	valacc *m, *psi, *mt, *c;
	valacc *r, *d, *rt, *dt, *rtt, *dtt;

	myvector *D;
	myvector *Dt;
	myvector *Dtt;
	myvector *sgr, *sgd;

//	FILE *FLOG;
//	char *fname;

	pvd();
	pvd(int log);
	~pvd();

	int initMPC(char *fname);
	int initObs(observ *eobs);

//	diffTable *dtr;
//	diffTable *dtd;

	int findPVD();

	int writePVD();

	int aclose();

	observ *obs;
private:

	diffTable *dtr;
	diffTable *dtd;

//	observ *obs;

/*	double t;
	valacc *m, *psi, *mt, *c;
	valacc *r, *d, *rt, *dt, *rtt, *dtt;
/
	myvector *D;
	myvector *Dt;
	myvector *Dtt;
	myvector *sgr, *sgd;
*/

	int klog;
	logstuff *slog;
//	FILE *FLOG;
	char *fname;

	int cent;

	int initDT();
	int detCent();
	int logging(char *basename);

	int detacc();

	int nt;

	FILE *FC;
};