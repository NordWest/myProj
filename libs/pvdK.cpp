#include "stdafx.h"

#include "pvdK.h"

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

#pragma once

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

diffTable::diffTable(int fRsize, int fCsize, double **farr, int xsize, double *xarr)
{
	this->f = new mymatrix(fRsize, fCsize, farr);
	this->x = new myvector(xsize, xarr);
}

////////////public

pvdK::pvdK()
{
//	this->obs = NULL;
//	this->FC = NULL;
	this->dtd = NULL;
	this->dtr = NULL;

	this->fname = new char[255];

	this->coords = new List4;

	r = NULL;
	d = NULL;
	rt = NULL;
	dt = NULL;
	rtt = NULL;
	dtt = NULL;
	m = NULL;
	psi = NULL;
	mt = NULL;
	c = NULL;

	this->slog = NULL;
	this->klog = 0;
	this->tlen = 3;
}

pvdK::pvdK(int log)
{
//	this->FC = NULL;
	this->slog = new logstuff;

	this->dtd = NULL;
	this->dtr = NULL;

	this->fname = new char[255];

	this->coords = new List4;

	r = NULL;
	d = NULL;
	rt = NULL;
	dt = NULL;
	rtt = NULL;
	dtt = NULL;
	m = NULL;
	psi = NULL;
	mt = NULL;
	c = NULL;

	this->slog = NULL;
	this->klog = log;
	this->tlen = 3;
}

pvdK::~pvdK()
{
//	delete(this->obs);
//	delete(this->FC);
	delete(this->dtr);
	delete(this->dtd);
//	this->obs = NULL;
//	fclose(this->FC);
//	this->FC = NULL;

	delete this->coords;

	delete [] fname;

	delete(this->slog);
/*
	if(this->klog)
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

int pvdK::logging(char *basename)
{
	if(this->klog)
	{
		this->slog = new logstuff;

		this->slog->suff = new char[256];
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

int pvdK::initCoord(List4 *iniCoords, int iniTlen)
{
    if(iniCoords==NULL) return 1;
    logging("test.dat");

    int i, j;
    tlen = iniTlen;
    nt = iniCoords->GetNum();
    int dim = iniCoords->GetDim();

    double *xn = new double[dim];

    tVect = new double[nt];
    raVect = new double[nt];
    deVect = new double[nt];

    printf("\nnt = %d\n", nt);

    if(klog)fprintf(this->slog->FLOG, "\nnt = %d\n", nt);

    this->Dj = new mymatrix(tlen, this->nt);


    for(i=0; i<nt; i++)
    {
        iniCoords->Get(xn, i);
        tVect[i] = xn[0];
        raVect[i] = xn[1];
        deVect[i] = xn[2];
        if(klog)
        {
            fprintf(this->slog->FLOG, "xn: %f\t%f\t%f\n", tVect[i], raVect[i], deVect[i]);
        }

        this->Dj->set(0, i, cos(deVect[i])*cos(raVect[i]));
        this->Dj->set(1, i, cos(deVect[i])*sin(raVect[i]));
        this->Dj->set(2, i, sin(deVect[i]));
    }

    detCent();

    mnkSys = new double[nt*tlen];
//    deSys = new double[nt*tlen];

    for(i=0; i<this->nt; i++)
    {
 //       yV->set(k, this->dtr->f->get(i, 0));

        for(j=0; j<tlen; j++)
        {
            mnkSys[i*tlen+j] = pow(tVect[i] - this->t, j)/(fakt(j)*1.0);
 //           printf("%d! = %d\n", j, fakt(j));
        }
//        k++;
    }

 //   initDT();

    return 0;
}

/*
int pvdK::initMPC(char *fname)
{
	this->logging(fname);

	this->obs = new ast;
	
	if(this->obs->initFile(fname)) return 1;
//	this->nt = this->obs->nmpcs;
	

	this->fname = new char[10];
	strcpy(this->fname, fname);

	return 0;
}
/*
int pvdK::initObs(ast *eobs)
{
	this->obs = new ast;

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
/*
int pvdK::initLS(char *fn)
{
	if((fn==NULL)||(this->logging==0)) return 1;
	strcpy(this->fname, fn);

	strcat(this->fname, ".log");

	this->slog->FLOG = fopen(this->fname, "w+");

	return 0;
}
*/
////////////private

int pvdK::initDT()
{
	strcpy(this->fname, fname);

	int i, j;
	
	double r, d, eJD;

	this->nt = this->coords->GetNum();
	
	double *x, **fr, **fd;
	x = (double*)calloc(this->nt, sizeof(double));

	fr = (double**)calloc(this->nt, sizeof(double*));
	fd = (double**)calloc(this->nt, sizeof(double*));

	this->Dj = new mymatrix(3, this->nt);

	if(this->klog) fprintf(this->slog->FLOG, "\nINIT DT\n\nDj:\n");

	double *xt;
	xt = new double[this->coords->GetDim()];

	for(i=0; i<this->nt; i++)
	{
		fr[i] = (double*)calloc(this->nt, sizeof(double));
		fd[i] = (double*)calloc(this->nt, sizeof(double));

		this->coords->Get(xt, i);
			
		r = xt[1];
		d = xt[2];
		eJD = xt[0];
			
		fr[i][0] = r;
		fd[i][0] = d;

		x[i] = eJD;

		this->Dj->set(0, i, cos(d)*cos(r));
		this->Dj->set(1, i, cos(d)*sin(r));
		this->Dj->set(2, i, sin(d));
	}

	for(j=1; j<this->nt; j++)
	{
		for(i=0; i<this->nt-j; i++)
		{
			fr[i][j] = fr[i+1][j-1] - fr[i][j-1];
			fd[i][j] = fd[i+1][j-1] - fd[i][j-1];
		}
	}

	this->dtr = new diffTable(this->nt ,this->nt, fr, this->nt, x);
	this->dtd = new diffTable(this->nt ,this->nt, fd, this->nt, x);

//	free(x);
//	free(fr);
//	free(fd);
	delete [] xt;

	return 0;
}

int pvdK::detCent()
{

	double d;
	this->t = 0.0;
	for(int i=0; i<this->nt; i++)
	{
                this->t += tVect[i];
	}
	this->t /= this->nt;

	if(this->klog) fprintf(this->slog->FLOG, "\nTcentr: %f\n", this->t);


	return 0;
}

int pvdK::findPVD()
{
	char *strMV;
	strMV = new char[2000];

//	this->tlen = tlen;

	int i, j;

//	double r, d, rt, dt, rtt, dtt;

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

	this->initDT();
	this->detCent();

//	r = this->r = this->dtr->f->get(this->cent, 0);
//	d = this->d = this->dtd->f->get(this->cent, 0);

	myvector *yV, *xV;
	mymatrix *aM;

//	double **ar, *yr;//, *x;
//	yr = (double*)calloc(this->nt, sizeof(double));
//	x = (double*)calloc(2, sizeof(double));
//	ar = (double**)calloc(this->nt, sizeof(double*));
	
//right
	int k;
	k = 0;

//
	char *pv;
	pv = new char[STRLEN];
//	this->dtr->x->print(pv);
//	printf("\n%s\n", pv);
//

	yV = new myvector(this->nt);
	aM = new mymatrix(this->nt, tlen);
	xV = new myvector(tlen);
	this->sgr = new myvector(tlen);
	this->sgd = new myvector(tlen);

	for(i=0; i<this->nt; i++)
	{
		yV->set(k, this->dtr->f->get(i, 0));

		for(j=0; j<tlen; j++) aM->set(k, j, pow(this->dtr->x->get(i) - this->t, j)/fakt(j));
		k++;
	}

	MNK(aM, yV, xV, sgr);

	this->r->val = xV->get(0);
	this->r->acc = sgr->get(0);
	this->rt->val = xV->get(1);
	this->rt->acc = sgr->get(1);
	this->rtt->val = xV->get(2);
	this->rtt->acc = sgr->get(2);

	delete xV;
	delete yV;
	delete aM;
//	free(yr);
//	free(ar);

	yV = new myvector(this->nt);
	aM = new mymatrix(this->nt, tlen);
	xV = new myvector(tlen);

	k = 0;
	for(i=0; i<this->nt; i++)
	{
		yV->set(k, this->dtd->f->get(i, 0));
		for(j=0; j<tlen; j++) aM->set(k, j,  pow(( this->dtd->x->get(i) - this->t), j)/fakt(j));
		k++;
	}

	MNK(aM, yV, xV, sgd);

	this->d->val = xV->get(0);
	this->d->acc = sgd->get(0);
	this->dt->val = xV->get(1);
	this->dt->acc = sgd->get(1);
	this->dtt->val = xV->get(2);
	this->dtt->acc = sgd->get(2);

	delete xV;
	delete yV;
	delete aM;

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

	if(((fabs(this->Dt->get(0)*this->Dt->get(0) + this->Dt->get(1)*this->Dt->get(1) + this->Dt->get(2)*this->Dt->get(2))-1.0)>EPS)&&(this->tlen>1)) return 2;

	this->Dtt->set(0, -this->D->get(0)*(this->rt->val*this->rt->val + this->dt->val*this->dt->val) + 2.0*this->D->get(2)*this->rt->val*this->dt->val*sin(this->r->val) - this->D->get(1)*this->rtt->val - this->D->get(2)*this->dtt->val*cos(this->r->val));
	this->Dtt->set(1, -this->D->get(1)*(this->rt->val*this->rt->val + this->dt->val*this->dt->val) - 2.0*this->D->get(2)*this->rt->val*this->dt->val*cos(this->r->val) + this->D->get(0)*this->rtt->val - this->D->get(2)*this->dtt->val*sin(this->r->val));
	this->Dtt->set(2, -this->dt->val*this->dt->val*sin(this->d->val) + this->dtt->val*cos(this->d->val));


	int hour, min;
	double sec;
	int sn;
	char ssn;

	if(((fabs(this->Dtt->get(0)*this->Dtt->get(0) + this->Dtt->get(1)*this->Dtt->get(1) + this->Dtt->get(2)*this->Dtt->get(2))-1.0)>EPS)&&(tlen>2)) return 2;

//////////////////////	Spesial alg		///////////////////////////////////////////////////

	double m1;
//	this->m->val = sqrt(this->Dt->get(0)*this->Dt->get(0) + this->Dt->get(1)*this->Dt->get(1) + this->Dt->get(2)*this->Dt->get(2));
	this->m->val = sqrt(pow(this->rt->val*cos(this->d->val), 2.0) + this->dt->val*this->dt->val);
	this->m->acc = 0.0;
//	m1 = sqrt((rt*cos(d))*(rt*cos(d)) + dt*dt);
//	printf("\nm1 %f\n", m1);

//	this->psi->val = atan2(this->rt->val*cos(this->d->val), this->dt->val);
//	(11)
//	this->psi->val = atan2(-this->Dt->get(0)*sin(this->r->val) + this->Dt->get(1)*cos(this->r->val), -this->Dt->get(0)*cos(this->r->val)*sin(this->d->val) - this->Dt->get(1)*sin(this->r->val)*sin(this->d->val) + -this->Dt->get(2)*cos(this->d->val));
	this->psi->val = atan2(this->rt->val*cos(this->d->val), this->dt->val);
	this->psi->acc = 0.0;
	if(this->psi->val<0.0) this->psi->val += 2.0*PI;

	this->mt->val = (this->Dt->get(0)*this->Dtt->get(0) + this->Dt->get(1)*this->Dtt->get(1) + this->Dt->get(2)*this->Dtt->get(2))/this->m->val;
	this->mt->acc = 0.0;

/*
	myvector *DtmDtt;
	DtmDtt = new myvector(3);

	*DtmDtt = Vmul(this->Dt, this->Dtt);
	this->c->val = (sqrt(DtmDtt->get(0)*DtmDtt->get(0) + DtmDtt->get(1)*DtmDtt->get(1) + DtmDtt->get(2)*DtmDtt->get(2)))/pow(sqrt(this->Dt->get(0)*this->Dt->get(0) + this->Dt->get(1)*this->Dt->get(1) + this->Dt->get(2)*this->Dt->get(2)), 3.0);
	this->c->acc = 0.0;
*/
//
	this->T = new myvector(3);

	this->T->set(0, (-sin(this->psi->val)*sin(this->r->val) - cos(this->psi->val)*sin(this->d->val)*cos(this->r->val)));
	this->T->set(1, (sin(this->psi->val)*cos(this->r->val) - cos(this->psi->val)*sin(this->d->val)*sin(this->r->val)));
	this->T->set(2, (cos(this->psi->val)*cos(this->d->val)));

	if(this->klog) fprintf(this->slog->FLOG, "\n\nT: %15.11f\t%15.11f\t%15.11f", this->T->get(0), this->T->get(1), this->T->get(2));

	this->M = new myvector(3);

	this->M->set(0, (cos(this->psi->val)*sin(this->r->val) - sin(this->psi->val)*sin(this->d->val)*cos(this->r->val)));
	this->M->set(1, (-cos(this->psi->val)*cos(this->r->val) - sin(this->psi->val)*sin(this->d->val)*sin(this->r->val)));
	this->M->set(2, (sin(this->psi->val)*cos(this->d->val)));

	if(this->klog) fprintf(this->slog->FLOG, "\n\nM iz RA&DEC:\t\t%15.11f\t%15.11f\t%15.11f", this->M->get(0), this->M->get(1), this->M->get(2));

	*this->M = Vmul(this->D, this->T);

	double Gch, Gzn, tgbta;
	myvector *Di;
	Di = new myvector(3);

	Gch = Gzn = 0.0;


	for(i=0; i<this->nt; i++)
	{
 		*Di = this->Dj->column(i);

		Gch += (Smul(this->D, Di) - 1.0)*Smul(this->M, Di);
		Gzn += pow((Smul(this->D, Di) - 1.0), 2.0);
	}

	tgbta = Gch/Gzn;

	if(this->klog) fprintf(this->slog->FLOG, "\n\ntg(b) = %15.11f", tgbta);

	this->c->val = sqrt(1.0 + tgbta*tgbta);
	this->c->acc = 0.0;

	if(this->klog) fprintf(this->slog->FLOG, "\n\nc = %15.11f", this->c->val);

//
	double sinbta, cosbta, bta;
//	myvector *B;
	this->B = new myvector(3);

	bta = atan2(Gch, Gzn);

	if(this->klog) fprintf(this->slog->FLOG, "\n\nbeta = %15.11f", bta);

	cosbta = sign(Gzn)*fabs(cos(bta));
	sinbta = sign(Gch)*fabs(sin(bta));

	this->B->set(0, this->M->get(0)*cosbta - this->D->get(0)*sinbta);
	this->B->set(1, this->M->get(1)*cosbta - this->D->get(1)*sinbta);
	this->B->set(2, this->M->get(2)*cosbta - this->D->get(2)*sinbta);

/*
	this->Lj = new mymatrix(3, this->nt);

	myvector *Li;
	Li = new myvector(3);
	double norm;
	double Tetaj;
	myvector *TvLi, *BvDi, *TTj;
	TvLi = new myvector(3);
	BvDi = new myvector(3);
	TTj = new myvector(this->nt);

	for(i=0; i<this->nt; i++)
	{
		*Di = this->Dj->column(i);
		*Li = Vmul(this->B, Di);
		norm = Li->norm();

		Li->set(0, Li->get(0)/norm);
		Li->set(1, Li->get(1)/norm);
		Li->set(2, Li->get(2)/norm);

		*TvLi = Vmul(this->T, Li);
		*BvDi = Vmul(this->B, Di);

		Tetaj = atan2(Smul(this->T, Di), Smul(this->T, BvDi));

		TTj->set(i, sign(Smul(this->T, Di))*fabs(Tetaj)*cosbta);
	}
*/

	myvector *sig;
	sig = new myvector(this->nt);
	double TetaJ;

//	fprintf(this->slog->FLOG, "\n\nsigma j: ");

	for(j=0; j<this->nt; j++)
	{
		*Di = this->Dj->column(j);
		TetaJ = atan2(Smul(this->T, Di), Smul(this->T, &Vmul(this->B, Di)));
		sig->set(j, TetaJ*cosbta);

		if(this->klog) fprintf(this->slog->FLOG, "\n%15.11f", sig->get(j));
	}

	if(this->klog) fprintf(this->slog->FLOG, "\n\n");

	yV = new myvector(this->nt);
	aM = new mymatrix(this->nt, 4);
	xV = new myvector(4);
	this->sgd = new myvector(4);
	k = 0;
	for(i=0; i<this->nt; i++)
	{
		yV->set(k, sig->get(i));
		for(j=0; j<4; j++) aM->set(k, j, pow((this->dtd->x->get(i) - this->t), j));
		k++;
	}

	MNK(aM, yV, xV, sgd);

	if(fabs(xV->get(1))>EPS)
	{
		this->m->val = xV->get(1);
		this->m->acc = sgd->get(1);
	}

	if(fabs(xV->get(2))>EPS)
	{
		this->mt->val = 2.0*xV->get(2);
		this->mt->acc = sgd->get(2);
	}


//	this->detacc();

	return 0;
}

int pvdK::findPVDnew()
{
        char *strMV;
        strMV = new char[2000];

        printf("\nfindPVDnew\n");

//	this->tlen = tlen;

        int i, j;

//	double r, d, rt, dt, rtt, dtt;

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

//	this->initDT();
//	this->detCent();

//	r = this->r = this->dtr->f->get(this->cent, 0);
//	d = this->d = this->dtd->f->get(this->cent, 0);

        myvector *yV, *xV;
        mymatrix *aM;

//	double **ar, *yr;//, *x;
//	yr = (double*)calloc(this->nt, sizeof(double));
//	x = (double*)calloc(2, sizeof(double));
//	ar = (double**)calloc(this->nt, sizeof(double*));

//right
        int k;
        k = 0;

//
        char *pv;
        pv = new char[STRLEN];
//	this->dtr->x->print(pv);
//	printf("\n%s\n", pv);
//

        yV = new myvector(this->nt);
        aM = new mymatrix(this->nt, tlen);
        xV = new myvector(tlen);
        this->sgr = new myvector(tlen);
        this->sgd = new myvector(tlen);

        for(i=0; i<this->nt; i++)
        {
                yV->set(k, raVect[i]);

                for(j=0; j<tlen; j++) aM->set(k, j, pow(tVect[i] - this->t, j)/fakt(j));
                k++;
        }

        MNK(aM, yV, xV, sgr);

        double *raRes = new double[tlen];
        double *raD = new double[tlen*tlen];
        double *raW = new double[nt];
        double uwe;

        for(i=0; i<nt; i++)
        {
            raW[i] = 1.0;
        }

        slsm(tlen, nt, raRes, raVect, mnkSys, uwe, raD, raW);

        printf("\nlsm\n");
        if(klog)
        {

            for(i=0; i<nt; i++)
            {
                fprintf(this->slog->FLOG, "%f = ", raVect[i]);
                for(j=0; j<tlen; j++) fprintf(this->slog->FLOG, "%f ", mnkSys[i*tlen + j]);
                fprintf(this->slog->FLOG, "\n");
            }
            fprintf(this->slog->FLOG, "\n");

            fprintf(this->slog->FLOG, "x: %f %f %f\n", raRes[0], raRes[1], raRes[2]);
        }





        this->r->val = raRes[0];
        this->r->acc = sgr->get(0);
        this->rt->val = raRes[1];
        this->rt->acc = sgr->get(1);
        this->rtt->val = raRes[2];
        this->rtt->acc = sgr->get(2);

        delete xV;
        delete yV;
        delete aM;
//	free(yr);
//	free(ar);

        yV = new myvector(this->nt);
        aM = new mymatrix(this->nt, tlen);
        xV = new myvector(tlen);

        k = 0;
        for(i=0; i<this->nt; i++)
        {
                yV->set(k, deVect[i]);
                for(j=0; j<tlen; j++) aM->set(k, j,  pow(( tVect[i] - this->t), j)/fakt(j));
                k++;
        }

        MNK(aM, yV, xV, sgd);

        this->d->val = xV->get(0);
        this->d->acc = sgd->get(0);
        this->dt->val = xV->get(1);
        this->dt->acc = sgd->get(1);
        this->dtt->val = xV->get(2);
        this->dtt->acc = sgd->get(2);

        delete xV;
        delete yV;
        delete aM;

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

        if(((fabs(this->Dt->get(0)*this->Dt->get(0) + this->Dt->get(1)*this->Dt->get(1) + this->Dt->get(2)*this->Dt->get(2))-1.0)>EPS)&&(this->tlen>1)) return 2;

        this->Dtt->set(0, -this->D->get(0)*(this->rt->val*this->rt->val + this->dt->val*this->dt->val) + 2.0*this->D->get(2)*this->rt->val*this->dt->val*sin(this->r->val) - this->D->get(1)*this->rtt->val - this->D->get(2)*this->dtt->val*cos(this->r->val));
        this->Dtt->set(1, -this->D->get(1)*(this->rt->val*this->rt->val + this->dt->val*this->dt->val) - 2.0*this->D->get(2)*this->rt->val*this->dt->val*cos(this->r->val) + this->D->get(0)*this->rtt->val - this->D->get(2)*this->dtt->val*sin(this->r->val));
        this->Dtt->set(2, -this->dt->val*this->dt->val*sin(this->d->val) + this->dtt->val*cos(this->d->val));


        int hour, min;
        double sec;
        int sn;
        char ssn;

        if(((fabs(this->Dtt->get(0)*this->Dtt->get(0) + this->Dtt->get(1)*this->Dtt->get(1) + this->Dtt->get(2)*this->Dtt->get(2))-1.0)>EPS)&&(tlen>2)) return 2;

//////////////////////	Spesial alg		///////////////////////////////////////////////////

        double m1;
//	this->m->val = sqrt(this->Dt->get(0)*this->Dt->get(0) + this->Dt->get(1)*this->Dt->get(1) + this->Dt->get(2)*this->Dt->get(2));
        this->m->val = sqrt(pow(this->rt->val*cos(this->d->val), 2.0) + this->dt->val*this->dt->val);
        this->m->acc = 0.0;
//	m1 = sqrt((rt*cos(d))*(rt*cos(d)) + dt*dt);
//	printf("\nm1 %f\n", m1);

//	this->psi->val = atan2(this->rt->val*cos(this->d->val), this->dt->val);
//	(11)
//	this->psi->val = atan2(-this->Dt->get(0)*sin(this->r->val) + this->Dt->get(1)*cos(this->r->val), -this->Dt->get(0)*cos(this->r->val)*sin(this->d->val) - this->Dt->get(1)*sin(this->r->val)*sin(this->d->val) + -this->Dt->get(2)*cos(this->d->val));
        this->psi->val = atan2(this->rt->val*cos(this->d->val), this->dt->val);
        this->psi->acc = 0.0;
        if(this->psi->val<0.0) this->psi->val += 2.0*PI;

        this->mt->val = (this->Dt->get(0)*this->Dtt->get(0) + this->Dt->get(1)*this->Dtt->get(1) + this->Dt->get(2)*this->Dtt->get(2))/this->m->val;
        this->mt->acc = 0.0;

/*
        myvector *DtmDtt;
        DtmDtt = new myvector(3);

        *DtmDtt = Vmul(this->Dt, this->Dtt);
        this->c->val = (sqrt(DtmDtt->get(0)*DtmDtt->get(0) + DtmDtt->get(1)*DtmDtt->get(1) + DtmDtt->get(2)*DtmDtt->get(2)))/pow(sqrt(this->Dt->get(0)*this->Dt->get(0) + this->Dt->get(1)*this->Dt->get(1) + this->Dt->get(2)*this->Dt->get(2)), 3.0);
        this->c->acc = 0.0;
*/
//
        this->T = new myvector(3);

        this->T->set(0, (-sin(this->psi->val)*sin(this->r->val) - cos(this->psi->val)*sin(this->d->val)*cos(this->r->val)));
        this->T->set(1, (sin(this->psi->val)*cos(this->r->val) - cos(this->psi->val)*sin(this->d->val)*sin(this->r->val)));
        this->T->set(2, (cos(this->psi->val)*cos(this->d->val)));

        if(this->klog) fprintf(this->slog->FLOG, "\n\nT: %15.11f\t%15.11f\t%15.11f", this->T->get(0), this->T->get(1), this->T->get(2));

        this->M = new myvector(3);

        this->M->set(0, (cos(this->psi->val)*sin(this->r->val) - sin(this->psi->val)*sin(this->d->val)*cos(this->r->val)));
        this->M->set(1, (-cos(this->psi->val)*cos(this->r->val) - sin(this->psi->val)*sin(this->d->val)*sin(this->r->val)));
        this->M->set(2, (sin(this->psi->val)*cos(this->d->val)));

        if(this->klog) fprintf(this->slog->FLOG, "\n\nM iz RA&DEC:\t\t%15.11f\t%15.11f\t%15.11f", this->M->get(0), this->M->get(1), this->M->get(2));

        *this->M = Vmul(this->D, this->T);

        double Gch, Gzn, tgbta;
        myvector *Di;
        Di = new myvector(3);

        Gch = Gzn = 0.0;


        for(i=0; i<this->nt; i++)
        {
                *Di = this->Dj->column(i);

                Gch += (Smul(this->D, Di) - 1.0)*Smul(this->M, Di);
                Gzn += pow((Smul(this->D, Di) - 1.0), 2.0);
        }

        tgbta = Gch/Gzn;

        if(this->klog) fprintf(this->slog->FLOG, "\n\ntg(b) = %15.11f", tgbta);

        this->c->val = sqrt(1.0 + tgbta*tgbta);
        this->c->acc = 0.0;

        if(this->klog) fprintf(this->slog->FLOG, "\n\nc = %15.11f", this->c->val);

//
        double sinbta, cosbta, bta;
//	myvector *B;
        this->B = new myvector(3);

        bta = atan2(Gch, Gzn);

        if(this->klog) fprintf(this->slog->FLOG, "\n\nbeta = %15.11f", bta);

        cosbta = sign(Gzn)*fabs(cos(bta));
        sinbta = sign(Gch)*fabs(sin(bta));

        this->B->set(0, this->M->get(0)*cosbta - this->D->get(0)*sinbta);
        this->B->set(1, this->M->get(1)*cosbta - this->D->get(1)*sinbta);
        this->B->set(2, this->M->get(2)*cosbta - this->D->get(2)*sinbta);


        myvector *sig;
        sig = new myvector(this->nt);
        double TetaJ;

        for(j=0; j<this->nt; j++)
        {
                *Di = this->Dj->column(j);
                TetaJ = atan2(Smul(this->T, Di), Smul(this->T, &Vmul(this->B, Di)));
                sig->set(j, TetaJ*cosbta);

                if(this->klog) fprintf(this->slog->FLOG, "\n%15.11f", sig->get(j));
        }

        if(this->klog) fprintf(this->slog->FLOG, "\n\n");

        yV = new myvector(this->nt);
        aM = new mymatrix(this->nt, 4);
        xV = new myvector(4);
        this->sgd = new myvector(4);
        k = 0;
        for(i=0; i<this->nt; i++)
        {
                yV->set(k, sig->get(i));
                for(j=0; j<4; j++) aM->set(k, j, pow((tVect[i] - this->t), j));
                k++;
        }

        MNK(aM, yV, xV, sgd);

        if(fabs(xV->get(1))>EPS)
        {
                this->m->val = xV->get(1);
                this->m->acc = sgd->get(1);
        }

        if(fabs(xV->get(2))>EPS)
        {
                this->mt->val = 2.0*xV->get(2);
                this->mt->acc = sgd->get(2);
        }


//	this->detacc();

        return 0;
}


int pvdK::writePVD()
{
	int len;
	char *fn;
	fn = new char[256];
	len = strlen(this->fname);
	strcpy(fn, "");
	strncpy(fn, this->fname, len-4);
	strcpy(&fn[len-4], "\0");
	strcat(fn, ".pvdK");
//	strcpy(&fn[len+1], "\0");

	this->FC = fopen(fn, "w");
	if(FC==NULL) return 1;

	int y, m, sn;
	char *ssn;
	ssn = new char[32];
	double d;

	char *str;
	str = new char[STRLEN];
	this->dtr->x->print(str);
//	printf("\n%s\n", str);

	dat2YMD(this->t, &y, &m, &d);
	fprintf(this->FC, "t	%d %d %f", y, m, d);

	rad2hms(this->r->val, &sn, &y, &m, &d);
	if(sn==0) strcpy(ssn, "-");
	else strcpy(ssn, "+");
	fprintf(this->FC, "\nr	%s%d %d %9.6f", ssn, y ,m ,d);

	rad2hms(this->r->acc, &sn, &y, &m, &d);
	fprintf(this->FC, "	+-%f", this->r->acc*206265.0/15.0);

	rad2gms(this->d->val, &sn, &y, &m, &d);
	if(sn==0) strcpy(ssn, "-");
	else strcpy(ssn, "+");
	fprintf(this->FC, "\nd	%s%d %d %9.6f", ssn, y ,m ,d);

	rad2hms(this->d->acc, &sn, &y, &m, &d);
	fprintf(this->FC, "	+-%f", this->d->acc*206265.0);

	fprintf(this->FC, "\nm	%15.10f\t%e\npsi	%15.10f\nmt	%15.10f\t%e\nc	%15.10f\n", this->m->val*206265.0, this->m->acc*206265.0, this->psi->val*180.0/PI, this->mt->val*206265.0, this->mt->acc*206265.0, this->c->val);

	if(FC!=NULL) fclose(this->FC);
	delete [] str;
	delete [] fn;
	delete [] ssn;
	return 0;
}

int pvdK::aclose()
{
	if(this->slog!=NULL) fclose(this->slog->FLOG);
	return 0;
}

int pvdK::detacc()
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
