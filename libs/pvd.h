#ifndef MPCS_H
#include "mpcs.h"
#endif

#ifndef DA_H
#include "DynArr.h"
#endif

#ifndef COMF_H
#include "comfunc.h"
#endif


#ifndef PVD_H

#define TLEN 3


class pvd_rec
{
	double JD;
	valacc *m, *psi, *mt, *c;
	valacc *r, *d, *rt, *dt, *rtt, *dtt;

	pvd_rec();
	~pvd_rec();
};

class pvd_eph : public FileDynStrBin
{
	pvd_rec *record;

	pvd_eph();
	~pvd_eph();

	void rec2s();
	void s2rec();

	int AddRec(pvd_rec *prec, int pos);
	int PushRec(pvd_rec *prec);

	int GetRec(int pos);
	int DelRec(int pos);
};


class diffTable
{
public:
	myvector *x;
	mymatrix *f;
	diffTable();
	~diffTable();
//	diffTable(int fRsize, int fCsize, double **farr, int xsize, double *xarr);
	diffTable(int fRsize, int fCsize, int xsize);
//	diffTable();
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
//	int initObs(observ *eobs);

//	diffTable *dtr;
//	diffTable *dtd;

	int findPVD();

	int writePVD();

	int aclose();

	mpcs *obs;


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

#define PVD_H
#endif