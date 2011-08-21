#include "stdafx.h"
#include "pvd.h"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define AU 1
#define MS 1
#define VE 2979256.9529966
#define PI 3.141592653589
#define ka 0.017202098955
#define EPS 1.0e-8
#define xi ka*sqrt(MS)
#define EKV 0.4093197552027299

#define EPO 1900
#define JDEPO 2415020.5
//#define STRLOGLEN 1000


diffTable::diffTable()
{
	this->f = NULL;
	this->x = NULL;
}

diffTable::~diffTable()
{
	delete(this->f);
	delete(this->x);
	this->f = NULL;
	this->x = NULL;
}
/*
diffTable::diffTable(int fRsize, int fCsize, double **farr, int xsize, double *xarr)
{
	this->f = new mymatrix(fRsize, fCsize, farr);
	this->x = new myvector(xsize, xarr);
}
*/
diffTable::diffTable(int fRsize, int fCsize, int xsize)
{
	this->f = new mymatrix(fRsize, fCsize);
	this->x = new myvector(xsize);
}

////////////public

pvd::pvd()
{
	this->FC = NULL;
	this->dtd = new diffTable();
	this->dtr = new diffTable();

	this->obs = new mpcs;

	this->r = new valacc;
	this->d = new valacc;
	this->rt = new valacc;
	this->dt = new valacc;
	this->rtt = new valacc;
	this->dtt = new valacc;
	this->m = new valacc;
	this->psi = new valacc;
	this->mt = new valacc;
	this->c = new valacc;

	this->slog = NULL;
	this->klog = 0;
}

pvd::pvd(int log)
{
	this->obs = NULL;
	this->FC = NULL;
	this->dtd = NULL;
	this->dtr = NULL;

	this->obs = new mpcs;

	this->r = new valacc;
	this->d = new valacc;
	this->rt = new valacc;
	this->dt = new valacc;
	this->rtt = new valacc;
	this->dtt = new valacc;
	this->m = new valacc;
	this->psi = new valacc;
	this->mt = new valacc;
	this->c = new valacc;

	this->slog = NULL;
	this->klog = log;
/*	if(this->klog)
	{
		int len;
		char *flogn;

		flogn = new char[25];

		len = strlen(this->fname);

		strcpy(flogn, "");

		strncpy(flogn, this->fname, len-4);
		strcpy(&flogn[len-4], "\0");
		strcat(flogn, ".log");
		strcpy(&flogn[len+1], "\0");

		this->FLOG = fopen(flogn, "w");
	}*/
}

pvd::~pvd()
{
	delete(this->obs);
	delete(this->FC);
	delete(this->dtr);
	delete(this->dtd);
	fclose(this->FC);
	this->FC = NULL;

	delete(this->slog);
/*	if(this->klog)
	{
		fclose(this->FLOG);
		this->FLOG = NULL;
	}
*/
	delete(r);
	delete(d);
	delete(rt);
	delete(dt);
	delete(rtt);
	delete(dtt);
	delete(m);
	delete(psi);
	delete(mt);
	delete(c);
}

int pvd::logging(char *basename)
{
	if(this->klog)
	{
		this->slog = new logstuff;

		this->slog->suff = new char[25];
		int len = strlen(basename);
		strcpy(this->slog->suff, "");
		strncpy(this->slog->suff, basename, len-4);
		strcpy(&this->slog->suff[len-4], "\0");

		len = strlen(this->slog->suff);

		strcat(this->slog->suff, "_pvd.log");
		strcpy(&this->slog->suff[len+8], "\0");

		this->slog->FLOG = fopen(this->slog->suff, "w");
	}

	return 0;
}

int pvd::initMPC(char *fname)
{
	this->logging(fname);

//	this->obs = new mpcs;
	
	if(this->obs->init(fname)) return 1;
//	this->nt = this->obs->nmpcs;
	

	this->fname = new char[10];
	strcpy(this->fname, fname);

	return 0;
}
/*
int pvd::initObs(observ *eobs)
{
	this->obs = new observ;

	mpcs *temp;
	temp = eobs->lmpc;

	while(temp!=NULL)
	{
		this->obs->addmpc(temp->pos->head, temp->pos->eJD, temp->pos->r, temp->pos->d, temp->pos->tail);
		temp = temp->next;
	}

	if(this->klog)
	{
		this->slog->FLOG = fopen("noname.log", "w");
	}


	return 0;
}
*/

////////////private

int pvd::initDT()
{
//	mpc *plan;
//	plan = new mpc(this->obs->Fkey);

	int i, j;
	this->nt = this->obs->nstr;


//	myvector *x;
//	x = new myvector(this->nt);
	this->dtr->f = new mymatrix(this->nt ,this->nt);
	this->dtr->x = new myvector(this->nt);
	this->dtd->f = new mymatrix(this->nt ,this->nt);
	this->dtd->x = new myvector(this->nt);

//	fr = (double**)calloc(this->nt, sizeof(double*));
//	fd = (double**)calloc(this->nt, sizeof(double*));
	for(i=0; i<this->nt; i++)
	{
//		fr[i] = (double*)calloc(this->nt, sizeof(double));
//		fd[i] = (double*)calloc(this->nt, sizeof(double));

		this->obs->getmpc(i);

		dtr->f->set(i, 0, this->obs->record->r);
		dtd->f->set(i, 0, this->obs->record->d);

		dtr->x->set(i, this->obs->record->eJD);
		dtd->x->set(i, this->obs->record->eJD);
	}

	for(j=1; j<this->nt; j++)
	{
		for(i=0; i<this->nt-j; i++)
		{
			dtr->f->set(i, j, dtr->f->get(i+1, j-1) - dtr->f->get(i, j-1));
			dtd->f->set(i, j, dtd->f->get(i+1, j-1) - dtd->f->get(i, j-1));
		}
	}

	return 0;
}

int pvd::detCent()
{

/*
	double tmidle, dist, ddist;

	tmidle = (this->dtr->x->get(this->nt-1) - this->dtr->x->get(0))/2.0 + this->dtr->x->get(0);

	int i;
	i=0;
	do
	{
		dist = tmidle - this->dtr->x->get(i);
		i++;
		ddist = (fabs(dist)-fabs(tmidle - this->dtr->x->get(i)));
	}while(ddist>0.0);

	this->cent = i-1;
*/
	double d;
	this->t = 0.0;
	for(int i=0; i<this->obs->nstr; i++)
	{
		this->t += this->dtr->x->get(i);
	}
	this->t /= this->obs->nstr;

	if(this->klog) fprintf(this->slog->FLOG, "\nTcentr: %f\n", this->t);


	return 0;
}

int pvd::findPVD()
{
	char *strMV;
//	strMV = new char[1000];

	int i, j;

//	double r, d, rt, dt, rtt, dtt;


	this->initDT();
	this->detCent();

//	r = this->r = this->dtr->f->get(this->cent, 0);
//	d = this->d = this->dtd->f->get(this->cent, 0);

	myvector *yV, *xV;
	mymatrix *aM;

	double **ar, *yr;//, *x;
	yr = (double*)calloc(this->nt, sizeof(double));
//	x = (double*)calloc(2, sizeof(double));
	ar = (double**)calloc(this->nt, sizeof(double*));
	
//right
	int k;
	k = 0;

/*
	char *pv;
	pv = new char[1000];
	this->dtr->x->print(pv);
	printf("\n%s\n", pv);
*/

	for(i=0; i<this->nt; i++)
	{
//		if(i==this->cent) continue;
//		yr[k] = this->dtr->f->get(i, 0) - this->dtr->f->get(this->cent, 0);
		yr[k] = this->dtr->f->get(i, 0);
		ar[k] = (double*)calloc(TLEN, sizeof(double));
		for(j=0; j<TLEN; j++)
		{
			ar[k][j] = pow(this->dtr->x->get(i) - this->t, j)/fakt(j);
//			a[k][1] = (pow(this->dtr->x->get(i) - this->dtr->x->get(this->cent), 2.0))/2.0;
		}
		k++;
	}


	yV = new myvector(this->nt, yr);
	aM = new mymatrix(this->nt, TLEN, ar);
	xV = new myvector(TLEN);
	this->sgr = new myvector(TLEN);
	this->sgd = new myvector(TLEN);

/*	if(this->klog)
	{
		aM->print(strMV);
		fprintf(this->FLOG, "a\n%s\n", strMV);

		xV->print(strMV);
		fprintf(this->FLOG, "x\n%s\n", strMV);

		yV->print(strMV);
		fprintf(this->FLOG, "y\n%s\n", strMV);
	}*/



	MNK(aM, yV, xV, sgr);

/*	if(this->klog)
	{
		aM->print(strMV);
		fprintf(this->FLOG, "a\n%s\n", strMV);

		xV->print(strMV);
		fprintf(this->FLOG, "x\n%s\n", strMV);

		yV->print(strMV);
		fprintf(this->FLOG, "y\n%s\n", strMV);

		sgr->print(strMV);
		fprintf(this->FLOG, "sg\n%s\n", strMV);
	}*/


	this->r->val = xV->get(0);
	this->r->acc = sgr->get(0);
	this->rt->val = xV->get(1);
	this->rt->acc = sgr->get(1);
	this->rtt->val = xV->get(2);
	this->rtt->acc = sgr->get(2);

//	rt = xV->get(0);
//	rtt = xV->get(1);

	delete xV;
	delete yV;
	delete aM;
	free(yr);
	free(ar);

	double **ad, *yd;//, *x;
	yd = (double*)calloc(this->nt, sizeof(double));
//	x = (double*)calloc(2, sizeof(double));
	ad = (double**)calloc(this->nt, sizeof(double*));


	k = 0;
	for(i=0; i<this->nt; i++)
	{
//		if(i==this->cent) continue;
//		yd[k] = this->dtd->f->get(i, 0) - this->dtd->f->get(this->cent, 0);
		yd[k] = this->dtd->f->get(i, 0);
		ad[k] = (double*)calloc(TLEN, sizeof(double));
		for(j=0; j<TLEN; j++)
		{
			ad[k][j] = pow(( this->dtd->x->get(i) - this->t), j)/fakt(j);
//			a[k][1] = (pow(this->dtr->x->get(i) - this->dtr->x->get(this->cent), 2.0))/2.0;
		}
//		a[k][0] = this->dtd->x->get(i) - this->dtd->x->get(this->cent);
//		a[k][1] = (pow(this->dtd->x->get(i) - this->dtd->x->get(this->cent), 2.0))/2.0;
		k++;
	}

	yV = new myvector(this->nt, yd);
	aM = new mymatrix(this->nt, TLEN, ad);
	xV = new myvector(TLEN);

	MNK(aM, yV, xV, sgd);

/*	if(this->klog)
	{
		aM->print(strMV);
		fprintf(this->FLOG, "a\n%s\n", strMV);

		xV->print(strMV);
		fprintf(this->FLOG, "x\n%s\n", strMV);

		yV->print(strMV);
		fprintf(this->FLOG, "y\n%s\n", strMV);

		sgd->print(strMV);
		fprintf(this->FLOG, "sg\n%s\n", strMV);
	}*/

	this->d->val = xV->get(0);
	this->d->acc = sgd->get(0);
	this->dt->val = xV->get(1);
	this->dt->acc = sgd->get(1);
	this->dtt->val = xV->get(2);
	this->dtt->acc = sgd->get(2);

//	dt = xV->get(0);
//	dtt = xV->get(1);

	delete xV;
	delete yV;
	delete aM;
	free(yd);
	free(ad);

	myvector *D, *Dt, *Dtt;
	this->D = new myvector(3);
	this->Dt = new myvector(3);
	this->Dtt = new myvector(3);

	this->D->set(0, cos(this->d->val)*cos(this->r->val));
	this->D->set(1, cos(this->d->val)*sin(this->r->val));
	this->D->set(2, sin(this->d->val));

	if((fabs(this->D->get(0)*this->D->get(0) + this->D->get(1)*this->D->get(1) + this->D->get(2)*this->D->get(2))-1.0)>EPS) return 2;

	this->Dt->set(0, -(this->D->get(1)*this->rt->val + this->D->get(2)*this->dt->val*cos(this->r->val)));
	this->Dt->set(1, this->D->get(0)*this->rt->val - this->D->get(2)*this->dt->val*sin(this->r->val));
	this->Dt->set(2, this->dt->val*cos(this->d->val));

	if((fabs(this->Dt->get(0)*this->Dt->get(0) + this->Dt->get(1)*this->Dt->get(1) + this->Dt->get(2)*this->Dt->get(2))-1.0)>EPS) return 2;

	this->Dtt->set(0, -this->D->get(0)*(this->rt->val*this->rt->val + this->dt->val*this->dt->val) + 2.0*this->D->get(2)*this->rt->val*this->dt->val*sin(this->r->val) - this->D->get(1)*this->rtt->val - this->D->get(2)*this->dtt->val*cos(this->r->val));
	this->Dtt->set(1, -this->D->get(1)*(this->rt->val*this->rt->val + this->dt->val*this->dt->val) - 2.0*this->D->get(2)*this->rt->val*this->dt->val*cos(this->r->val) + this->D->get(0)*this->rtt->val - this->D->get(2)*this->dtt->val*sin(this->r->val));
	this->Dtt->set(2, -this->dt->val*this->dt->val*sin(this->d->val) + this->dtt->val*cos(this->d->val));

	if(this->klog)
	{
		fprintf(this->slog->FLOG, "\nD	%16.10f	%16.10f	%16.10f	%16.10f", this->D->get(0), this->D->get(1), this->D->get(2), sqrt(this->D->get(0)*this->D->get(0) + this->D->get(1)*this->D->get(1) + this->D->get(2)*this->D->get(2)));
		fprintf(this->slog->FLOG, "\nDt	%16.10f	%16.10f	%16.10f	%16.10f", this->Dt->get(0), this->Dt->get(1), this->Dt->get(2), sqrt(this->Dt->get(0)*this->Dt->get(0) + this->Dt->get(1)*this->Dt->get(2) + this->Dt->get(2)*this->Dt->get(2)));
		fprintf(this->slog->FLOG, "\nDtt	%16.10f	%16.10f	%16.10f	%16.10f\n", this->Dtt->get(0), this->Dtt->get(1), this->Dtt->get(2), sqrt(this->Dtt->get(0)*this->Dtt->get(0) + this->Dtt->get(1)*this->Dtt->get(2) + this->Dtt->get(2)*this->Dtt->get(2)));

		int hour, min;
		double sec;
		int sn;
		char ssn;

/*		rad2hms(this->r->val, &sn, &hour, &min, &sec);
		if(sn==0) strcpy(&ssn, "-");
		else strcpy(&ssn, "+");
		fprintf(this->FLOG, "\nr	%s%d %d %f", &ssn, hour, min, sec);
*/
		rad2hms(this->r->val, &sn, &hour, &min, &sec);
		if(sn==0) strcpy(&ssn, "-");
		else strcpy(&ssn, "+");
		fprintf(this->slog->FLOG, "\nr	%s%d %d %f", &ssn, hour, min, sec);
		rad2hms(this->r->acc, &sn, &hour, &min, &sec);
		if(sn==0) strcpy(&ssn, "-");
		else strcpy(&ssn, "+");
		fprintf(this->slog->FLOG, " +- %d %d %e", hour, min, sec);

		rad2hms(this->rt->val, &sn, &hour, &min, &sec);
		if(sn==0) strcpy(&ssn, "-");
		else strcpy(&ssn, "+");
		fprintf(this->slog->FLOG, "\nrt	%s%d %d %f", &ssn, hour, min, sec);
		rad2hms(this->rt->acc, &sn, &hour, &min, &sec);
		if(sn==0) strcpy(&ssn, "-");
		else strcpy(&ssn, "+");
		fprintf(this->slog->FLOG, " +- %d %d %e", hour, min, sec);

		rad2hms(this->rtt->val, &sn, &hour, &min, &sec);
		if(sn==0) strcpy(&ssn, "-");
		else strcpy(&ssn, "+");
		fprintf(this->slog->FLOG, "\nrtt	%s%d %d %f", &ssn, hour, min, sec);
		rad2hms(this->rtt->acc, &sn, &hour, &min, &sec);
		if(sn==0) strcpy(&ssn, "-");
		else strcpy(&ssn, "+");
		fprintf(this->slog->FLOG, " +- %d %d %e\n", hour, min, sec);

/*		rad2gms(d, &sn, &hour, &min, &sec);
		if(sn==0) strcpy(&ssn, "-");
		else strcpy(&ssn, "+");
		fprintf(this->FLOG, "\nd %s%d %d %f", &ssn, hour, min, sec);
*/		
		rad2gms(this->d->val, &sn, &hour, &min, &sec);
		if(sn==0) strcpy(&ssn, "-");
		else strcpy(&ssn, "+");
		fprintf(this->slog->FLOG, "\nd %s%d %d %f", &ssn, hour, min, sec);
		rad2gms(this->d->acc, &sn, &hour, &min, &sec);
		if(sn==0) strcpy(&ssn, "-");
		else strcpy(&ssn, "+");
		fprintf(this->slog->FLOG, " +- %d %d %e", hour, min, sec);

		rad2gms(this->dt->val, &sn, &hour, &min, &sec);
		if(sn==0) strcpy(&ssn, "-");
		else strcpy(&ssn, "+");
		fprintf(this->slog->FLOG, "\ndt %s%d %d %f", &ssn, hour, min, sec);
		rad2gms(this->dt->acc, &sn, &hour, &min, &sec);
		if(sn==0) strcpy(&ssn, "-");
		else strcpy(&ssn, "+");
		fprintf(this->slog->FLOG, " +- %d %d %e", hour, min, sec);

		rad2gms(this->dtt->val, &sn, &hour, &min, &sec);
		if(sn==0) strcpy(&ssn, "-");
		else strcpy(&ssn, "+");
		fprintf(this->slog->FLOG, "\ndtt %s%d %d %f", &ssn, hour, min, sec);
		rad2gms(this->dtt->acc, &sn, &hour, &min, &sec);
		if(sn==0) strcpy(&ssn, "-");
		else strcpy(&ssn, "+");
		fprintf(this->slog->FLOG, " +- %d %d %e\n", hour, min, sec);
	}

	if((fabs(this->Dtt->get(0)*this->Dtt->get(0) + this->Dtt->get(1)*this->Dtt->get(1) + this->Dtt->get(2)*this->Dtt->get(2))-1.0)>EPS) return 2;

	double m1;
	this->m->val = sqrt(this->Dt->get(0)*this->Dt->get(0) + this->Dt->get(1)*this->Dt->get(1) + this->Dt->get(2)*this->Dt->get(2));
	this->m->acc = 0.0;
//	m1 = sqrt((rt*cos(d))*(rt*cos(d)) + dt*dt);
//	printf("\nm1 %f\n", m1);

	this->psi->val = atan2(this->rt->val*cos(this->d->val), this->dt->val);
	this->psi->acc = 0.0;
	if(this->psi->val<0.0) this->psi->val += 2.0*PI;

	this->mt->val = (this->Dt->get(0)*this->Dtt->get(0) + this->Dt->get(1)*this->Dtt->get(1) + this->Dt->get(2)*this->Dtt->get(2))/this->m->val;
	this->mt->acc = 0.0;

	myvector *DtmDtt;
	DtmDtt = new myvector(3);

	*DtmDtt = Vmul(this->Dt, this->Dtt);
	this->c->val = (sqrt(DtmDtt->get(0)*DtmDtt->get(0) + DtmDtt->get(1)*DtmDtt->get(1) + DtmDtt->get(2)*DtmDtt->get(2)))/pow(sqrt(this->Dt->get(0)*this->Dt->get(0) + this->Dt->get(1)*this->Dt->get(1) + this->Dt->get(2)*this->Dt->get(2)), 3.0);
	this->c->acc = 0.0;

	this->detacc();

	return 0;
}

int pvd::writePVD()
{
	int len;
	char *fn;
	fn = new char[10];
	len = strlen(this->fname);
	strcpy(fn, "");
	strncpy(fn, this->fname, len-4);
	strcpy(&fn[len-4], "\0");
	strcat(fn, ".pvd");
//	strcpy(&fn[len+1], "\0");

	this->FC = fopen(fn, "w");

	int y, m, sn;
	char ssn;
	double d;

	char *str;
	str = new char[1000];
	this->dtr->x->print(str);
//	printf("\n%s\n", str);

	dat2YMD(this->t, &y, &m, &d);
	fprintf(this->FC, "t	%d %d %f", y, m, d);

	rad2hms(this->r->val, &sn, &y, &m, &d);
	if(sn==0) strcpy(&ssn, "-");
	else strcpy(&ssn, "+");
	fprintf(this->FC, "\nr	%s%d %d %9.6f", &ssn, y ,m ,d);

	rad2hms(this->r->acc, &sn, &y, &m, &d);
	fprintf(this->FC, "	+-%f", this->r->acc*206265.0/15.0);

	rad2gms(this->d->val, &sn, &y, &m, &d);
	if(sn==0) strcpy(&ssn, "-");
	else strcpy(&ssn, "+");
	fprintf(this->FC, "\nd	%s%d %d %9.6f", &ssn, y ,m ,d);

	rad2hms(this->d->acc, &sn, &y, &m, &d);
	fprintf(this->FC, "	+-%f", this->d->acc*206265.0);

	fprintf(this->FC, "\nm	%f\npsi	%f\nmt	%f\nc	%f\n", this->m->val*SECINRAD, this->psi->val*180.0/PI, this->mt->val*SECINRAD, this->c->val);

	fclose(this->FC);
	return 0;
}

int pvd::aclose()
{
	if(this->slog!=NULL) fclose(this->slog->FLOG);
	return 0;
}

int pvd::detacc()
{
	int sekey;
	double x1, x2;
	double A, B, C;

	A = 1.0;
	B = 2.0*this->m->val;
	C = 2.0*this->rt->val*this->rt->val*cos(this->d->val)*sin(this->d->val)*cos(this->d->acc)*sin(this->d->acc) + 4.0*this->rt->val*this->rt->acc*cos(this->d->val)*sin(this->d->val)*cos(this->d->acc)*sin(this->d->acc) - 2.0*this->rt->val*this->rt->acc*cos(this->d->val)*cos(this->d->val) - 2.0*this->dt->val*this->dt->acc;

	sekey = sqEqv(A, B, C, &x1, &x2);

	x1 = fabs(x1);
	x2 = fabs(x2);

	this->m->acc = x2;
	if(x1>x2) this->m->acc = x1;

	return 0;
}