/*
	Определение ПВД по специальному алгоритму
*/

#pragma once

#include "observ.h"
#include "DynArr.h"
#include "comfunc.h"
#include "coord.h"
#include "mb.h"

#ifndef PVDK_H

#include "ast.h"


//#define TLEN 2
/*
struct valacc
{
	double val;
	double acc;
};
*/
class diffTable
{
public:
	myvector *x;
	mymatrix *f;
	diffTable();
	~diffTable();
	diffTable(int fRsize, int fCsize, double **farr, int xsize, double *xarr);
};

class pvdK
{
public:
	double t;
	valacc *m, *psi, *mt, *c;
	valacc *r, *d, *rt, *dt, *rtt, *dtt;

	myvector *D;
	myvector *Dt;
	myvector *Dtt;
	myvector *sgr, *sgd, *sgmt;

	mymatrix *Dj, *Lj;

	myvector *M, *T, *B;

	pvdK();
	pvdK(int log);
	~pvdK();

	int tlen;

	int findPVD();
        int findPVDnew();

	int writePVD();

	int aclose();

	char *fname;

	List4 *coords;

	int initDT();
//	int initLS(char *fn);

	diffTable *dtr;
	diffTable *dtd;
	int nt;

	int logging(char *basename);

        int initCoord(List4 *iniCoords, int iniTlen=3);

        double *tVect, *raVect, *deVect;
        double *mnkSys;

private:
	int klog;
	logstuff *slog;

	int cent;

	int detCent();

	int detacc();

	FILE *FC;
	FILE *FLOG;
};

#define PVDK_H
#endif
