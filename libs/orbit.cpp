#include "stdafx.h"
//#include "..\DAster\StdAfx.h"
//#include "..\DAster\DAster.h"

//#include "stdafx.h"
//#include "WinLofo.h"

//#include "MainFrm.h"

/////////////////////////////////////

#include "orbit.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

//#define Gc 6.67259e-8
//#define AU 1.496e+13
//#define MS 1.99e+33
/*
#define AU 1
#define MS 1
//efine VE 2979256.9529966
#define PI 3.141592653589
#define ka 0.017202098955
#define EPS 1.0e-8
#define xi ka*sqrt(MS)
#define EKV 0.409092614174
*/
#define H_AREA 10.0
#define MAGN_AREA 1.0

///////////////////////////////orbCat
/*
orbCat::orbCat()
{
	this->number = new char[9];		//nomer
	this->epoha = new char[4];		//эпоха [год]
	this->eJD = new char[9];		//Юлианские дни с эпохи
	this->M0 = new char[10];		//средняя аномалия
	this->w = new char[10];		//долгота перицентра
	this->Node = new char[10];		//долгота восх узла
	this->inc = new char[10];		//наклон орбиты
	this->ec = new char[10];		//эксцентриситет
	this->q = new char[12];		//перигелийное растояние
	this->H = new char[6];		//точность
	this->G = new char[5];		//X
	this->perturb = new char[16];	//от чего считалась пертурбация 1000000000000000
	this->opp = new char[3];		//число оппозиций
	this->obs = new char[5];		//число наблюдений
	this->obsY = new char[4];	//год наблюдений
	this->regY = new char[4];	//год внесения в каталог
	this->un = new char[11];		//neznaiu
	this->name = new char[32];		//имя объекта
	this->makeD = new char[13];
}

orbCat::~orbCat()
{
/*
	delete(this->number);
	delete(this->epoha);
	delete(this->eJD);
	delete(this->M0);
	delete(this->w);
	delete(this->Node);
	delete(this->inc);
	delete(this->ec);
	delete(this->q);
	delete(this->H);
	delete(this->G);
	delete(this->perturb);
	delete(this->opp);
	delete(this->obs);
	delete(this->obsY);
	delete(this->regY);
	delete(this->un);
	delete(this->name);
	delete(this->makeD);
/
	free(this->number);
	free(this->epoha);
	free(this->eJD);
	free(this->M0);
	free(this->w);
	free(this->Node);
	free(this->inc);
	free(this->ec);
	free(this->q);
	free(this->H);
	free(this->G);
	free(this->perturb);
	free(this->opp);
	free(this->obs);
	free(this->obsY);
	free(this->regY);
	free(this->un);
	free(this->name);
	free(this->makeD);
}

int orbCat::writeCat(char *fname, int mode)
{
	FILE *FC;

	switch(mode)
	{
	case 0:
		FC = fopen(fname, "w");
		break;
	case 1:
		FC = fopen(fname, "a+");
		break;
	}

	if(FC==NULL)
	{
		printf("\nError open file for write Cat\n");
		return 1;
	}

	char *str, *nstr;
	str = new char[100];
	nstr = new char[100];

	strcpy(str, "");
	strcpy(nstr, "");

	switch(mode)
	{
	case 1:
		do
		{
			strcpy(str, nstr);
			fgets(nstr, 1000, FC);
		}while(!strcmp(str, nstr)==0);
		break;
	}


	strcpy(str, "");
	strncat(str, this->number, 9);
	strncat(str, this->epoha, 4);
	strncat(str, this->eJD, 9);
	strncat(str, this->M0, 10);
	strncat(str, this->w, 10);
	strncat(str, this->Node, 10);
	strncat(str, this->inc, 10);
	strncat(str, this->ec, 10);
	strncat(str, this->q, 12);

	fprintf(FC, "%s\n", str);

	strcpy(str, "");
	strncat(str, this->H, 6);
	strncat(str, this->G, 5);
	strncat(str, this->perturb, 16);
	strncat(str, this->opp, 3);
	strncat(str, this->obs, 5);
	strncat(str, this->obsY, 4);
	strncat(str, this->regY, 4);
	strncat(str, this->un, 11);
	strncat(str, this->name, 32);

	fprintf(FC, "%s\n", str);

	strcpy(str, "");
	strncat(str, this->makeD, 13);

	fprintf(FC, "%s\n", str);

	fclose(FC);

	free(nstr);
	free(str);

	return 0;
}

int orbCat::copyTo(orbCat *dest)
{
	strcpy(dest->number, "");
	strncat(dest->number, this->number, 9);
	strcpy(dest->epoha, "");
	strncat(dest->epoha, this->epoha, 4);
	strcpy(dest->eJD, "");
	strncat(dest->eJD, this->eJD, 9);
	strcpy(dest->M0, "");
	strncat(dest->M0, this->M0, 10);
	strcpy(dest->w, "");
	strncat(dest->w, this->w, 10);
	strcpy(dest->Node, "");
	strncat(dest->Node, this->Node, 10);
	strcpy(dest->inc, "");
	strncat(dest->inc, this->inc, 10);
	strcpy(dest->ec, "");
	strncat(dest->ec, this->ec, 10);
	strcpy(dest->q, "");
	strncat(dest->q, this->q, 12);
	strcpy(dest->H, "");
	strncat(dest->H, this->H, 6);
	strcpy(dest->G, "");
	strncat(dest->G, this->G, 5);
	strcpy(dest->perturb, "");
	strncat(dest->perturb, this->perturb, 16);
	strcpy(dest->opp, "");
	strncat(dest->opp, this->opp, 3);
	strcpy(dest->obs, "");
	strncat(dest->obs, this->obs, 5);
	strcpy(dest->obsY, "");
	strncat(dest->obsY, this->obsY, 4);
	strcpy(dest->regY, "");
	strncat(dest->regY, this->regY, 4);
	strcpy(dest->un, "");
	strncat(dest->un, this->un, 11);
	strcpy(dest->name, "");
	strncat(dest->name, this->name, 32);
	strcpy(dest->makeD, "");
	strncat(dest->makeD, this->makeD, 13);

	return 0;
}
*/
///////////////////////////////orbit
////////////public

orbit::orbit()
{
	this->elem = new orbElem;
//	this->line = new orbCat;
	this->klog = 0;

//	this->tcat = new orbCat;

//	this->setCat("", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");
//	this->setCat("1", "2000", "2450400.0", "0.0", "0.0", "0.0", "0.0", "0.0", "0.0", "   .0 ", "  X  ", "0000000000000000", "0", "0", "2000", "2000", "  .00      ", "noname                       BAA", "1000000");
//	this->indexElem();
}

orbit::orbit(int log)
{
	this->elem = new orbElem;
//	this->line = new orbCat;
	this->klog = log;

//	this->setCat("", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");
//	this->setCat("1", "2000", "2450400.0", "0.0", "0.0", "0.0", "0.0", "0.0", "0.0", "   .0 ", "  X  ", "0000000000000000", "0", "0", "2000", "2000", "  .00      ", "noname                       BAA", "1000000");
//	this->indexElem();
}
/*
orbit::orbit(char *fname, int nrec)
{
	this->klog = 0;

	this->elem = new orbElem;
//	this->line = new orbCat;

//	this->setCat("", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");
//	this->setCat("1", "2000", "2450400.0", "0.0", "0.0", "0.0", "0.0", "0.0", "0.0", "   .0 ", "  X  ", "0000000000000000", "0", "0", "2000", "2000", "  .00      ", "noname                       BAA", "1000000");

//	this->indexElem();

//	this->initFile(fname, nrec);
}
*/
/*
orbit::orbit(char *fname, int nrec, int log)
{
	this->klog = log;
	this->logging(fname);

	this->elem = new orbElem;
	this->line = new orbCat;

	this->setCat("", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "");
//	this->setCat("1", "2000", "2450400.0", "0.0", "0.0", "0.0", "0.0", "0.0", "0.0", "   .0 ", "  X  ", "0000000000000000", "0", "0", "2000", "2000", "  .00      ", "noname                       BAA", "1000000");

	this->indexElem();

	this->initFile(fname, nrec);
}
*/
orbit::~orbit()
{
	delete(this->elem);
//	delete(this->line);
	this->elem = NULL;
//	this->line = NULL;
}

int orbit::logging(char *basename)
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

		strcat(this->slog->suff, "_orb.log");
		strcpy(&this->slog->suff[len+8], "\0");

		this->slog->FLOG = fopen(this->slog->suff, "w");
	}

	return 0;
}

int orbit::initElem(double eJD, double M0, double w, double N, double inc, double ec, double q)
{
	this->elem->eJD = eJD;
	this->elem->M0 = M0;
	this->elem->w = w;
	this->elem->Node = N;
	this->elem->inc = inc;
	this->elem->ec = ec;
	this->elem->q = q;

//	this->indexCat();

	return 0;
}

int orbit::get(OrbCat *ocat)
{
	this->elem->ec = ocat->record->ec;

	this->elem->eJD = ocat->record->eJD;
	this->elem->inc = ocat->record->inc*PI/180,0;
	this->elem->M0 = ocat->record->M0*PI/180,0;
	this->elem->Node = ocat->record->Node*PI/180,0;
	this->elem->q = ocat->record->q;
	this->elem->w = ocat->record->w*PI/180,0;
	

	return 0;
}

int orbit::get(mpccat *ocat)
{
	this->elem->ec = ocat->record->ecc;

	this->elem->eJD = ocat->record->getEpoch();
	this->elem->inc = grad2rad(ocat->record->inc);
	this->elem->M0 = grad2rad(ocat->record->meanA);
	this->elem->Node = grad2rad(ocat->record->Node);
	this->elem->q = ocat->record->a*(1.0-this->elem->ec);
	this->elem->w = grad2rad(ocat->record->w);
	

	return 0;
}

int orbit::get(eposrec *erec)
{
	this->elem->ec = erec->ec;

	this->elem->eJD = erec->eJD;
	this->elem->inc = erec->inc*PI/180,0;
	this->elem->M0 = erec->M0*PI/180,0;
	this->elem->Node = erec->Node*PI/180,0;
	this->elem->q = erec->q;
	this->elem->w = erec->w*PI/180,0;
	

	return 0;
}

int orbit::set(OrbCat *ocat)
{
	ocat->record->ec = this->elem->ec;
	ocat->record->eJD = this->elem->eJD;
	ocat->record->inc = this->elem->inc*180.0/PI;
	ocat->record->M0 = this->elem->M0*180.0/PI;
	ocat->record->Node = this->elem->Node*180.0/PI;
	ocat->record->q = this->elem->q;
	ocat->record->w = this->elem->w*180.0/PI;

	return 0;
}

int orbit::get(CommetOrbCat *ocat)
{
	this->elem->ec = ocat->record->ec;

	this->elem->eJD = ocat->record->eJD;
	this->elem->inc = ocat->record->inc*PI/180,0;
	this->elem->M0 = ocat->record->M0;
	this->elem->Node = ocat->record->Node*PI/180,0;
	this->elem->q = ocat->record->q;
	this->elem->w = ocat->record->w*PI/180,0;
	

	return 0;
}

int orbit::set(CommetOrbCat *ocat)
{
	ocat->record->ec = this->elem->ec;
	ocat->record->eJD = this->elem->eJD;
	ocat->record->inc = this->elem->inc*180.0/PI;
	ocat->record->M0 = this->elem->M0;
	ocat->record->Node = this->elem->Node*180.0/PI;
	ocat->record->q = this->elem->q;
	ocat->record->w = this->elem->w*180.0/PI;

	return 0;
}


///////////////////det

int orbit::detPolarOrb(double *r, double *v, double t)
{
	if(this->elem->isElips())
	{
        double T, n, M, E, p, a;

		p = this->elem->q*(1.0 + this->elem->ec);
        a = this->elem->q/(1.0-this->elem->ec);
        n = grad2rad(kaGRAD*pow(a, -1.5));

		T = this->elem->eJD - this->elem->M0/n;
		M = n*(t - this->elem->eJD) + this->elem->M0;
		eKepler(&E, M, this->elem->ec);

    //            printf("p= %f\tn= %f\tT= %f\tM= %f\tE=%f\n", p, n, T, M, E);

		double Pr1, Pr2;
        //Pr1 = this->elem->q*sqrt((1.0 + this->elem->ec)/(1.0 - this->elem->ec))*sin(E);
        //Pr2 = this->elem->q*(cos(E) - this->elem->ec)/(1.0 - this->elem->ec);

        Pr1 = sqrt(1.0+this->elem->ec)*tan(E/2.0);
        Pr2 = sqrt(1.0-this->elem->ec);

		double viu;

        viu = 2.0*atan2(Pr1, Pr2);

        //*r = p/(1.0 + this->elem->ec*cos(viu));
        *r = a*(1.0 - this->elem->ec*this->elem->ec)/(1.0 + this->elem->ec*cos(viu));
		*v = viu;

		return 1;
	}
	else if(this->elem->isPorabola())
	{
		double B, s, T, dT, n;
		int key = 0;
//
//		findT(&T, this->elem->M0, this->elem->eJD, this->elem->q, this->elem->eJD);
		T = this->elem->M0;

		dT = t - T;		

		B = pow(this->elem->q, -1.5)*dT;
		if(B<0.0)
		{
			key = 1;
			B = -B;
		}

		pKepler(&s, B);
//
//		s = -tan(this->elem->M0/2.0);
		*r = this->elem->q*(1.0 + s*s);
		*v = 2.0*atan(s);
		if(key) *v = 2.0*PI - fabs(*v);

		return 2;

	}
	else if(this->elem->isGiperbola())
	{
/*		double M, a, T, H, n;

		a = this->elem->q/(1.0 - this->elem->ec);
		a = fabs(a);

		n = xi*pow(a, -1.5);
//		T = this->elem->eJD - this->elem->M0/n;
		M = n*(t - this->elem->eJD) + this->elem->M0;

		this->gKepler(&H, M);

		*v = fatan(a*sqrt(this->elem->ec*this->elem->ec - 1.0)*sinh(H), a*(this->elem->ec - cosh(H)));
		*r = a*(this->elem->ec*cosh(H) - 1.0);
//		M = xi*fabs(a)*(t - T);
*/
		return 3;

	}
	else if(this->elem->isNPorabola())
	{
		double B, s, T;
		int key = 0;

		findT(&T, this->elem->M0, this->elem->eJD, this->elem->q, this->elem->eJD);

		B = pow(this->elem->q, -1.5)*(t - T);
		if(B<0.0)
		{
			key = 1;
			B = -B;
		}

		npKepler(&s, B, this->elem->ec);

		double eps, ksi;
		eps = (1.0-this->elem->ec)/2.0;
		ksi = eps*s*s;

		*r = this->elem->q*(1.0 + this->elem->ec*s*s);
		*v = 2.0*atan(s*sqrt((1.0-eps)/(1.0-ksi)));
		if(key) *v = 2.0*PI - fabs(*v);

		return 2;

	}
	else return 1;

	
	return 0;
}

int orbit::detRecOrb(double *ksi, double *eta, double t)
{
	if(this->elem->isPorabola())
	{
		double r, v, s;
		this->detPolarOrb(&r, &v, t);
		s = tan(v/2.0);
		*ksi = this->elem->q*(1.0 - s*s);
		*eta = 2.0*this->elem->q*s;

		return 0;
	}
	else if(this->elem->isElips())
	{
		double r, v;

		this->detPolarOrb(&r, &v, t);

		*ksi = r*cos(v);
		*eta = r*sin(v);

		return 0;
	}
	else if(this->elem->isGiperbola())
	{
/*		double r, v;
		this->detPolarOrb(&r, &v, t);
		*ksi = r*cos(v);
		*eta = r*sin(v);*/
		return 1;
	}
	else if(this->elem->isNPorabola())
	{
		double B, s, T;
		int key = 0;

		findT(&T, this->elem->M0, this->elem->eJD, this->elem->q, this->elem->eJD);

		B = pow(this->elem->q, -1.5)*(t - T);
		if(B<0.0)
		{
			key = 1;
			B = -B;
		}

		npKepler(&s, B, this->elem->ec);

		double eps, dz;
		eps = (1.0-this->elem->ec)/2.0;
		dz = eps*s*s;

		*ksi = this->elem->q*(1.0 - s*s);
		*eta = sqrt(2.0)*ka*this->elem->q*sqrt(1.0 - eps)*s*npU(dz);
		return 0;
	}
	return 1;
}


int orbit::detRecEcl(double *x, double *y, double *z, double t)
{
	double r, v;
	this->detPolarOrb(&r, &v, t);

	double u = this->elem->w + v;

	*x = r*(cos(u)*cos(this->elem->Node) - sin(u)*sin(this->elem->Node)*cos(this->elem->inc));
	*y = r*(cos(u)*sin(this->elem->Node) + sin(u)*cos(this->elem->Node)*cos(this->elem->inc));
	*z = r*sin(u)*sin(this->elem->inc);

	return 0;
}

int orbit::detRecEkv(double *x, double *y, double *z, double t)
{
/*	double ksi, eta;

	if(this->detRecOrb(&ksi, &eta, t)) return 1;

	double px, py, pz, qx, qy, qz, rx, ry, rz;

	px = cos(this->elem->w)*cos(this->elem->Node) - sin(this->elem->w)*sin(this->elem->Node)*cos(this->elem->inc);
	py = (cos(this->elem->w)*sin(this->elem->Node) + sin(this->elem->w)*cos(this->elem->Node)*cos(this->elem->inc))*cos(EKV) - sin(this->elem->w)*sin(this->elem->inc)*sin(EKV);
	pz = (cos(this->elem->w)*sin(this->elem->Node) + sin(this->elem->w)*cos(this->elem->Node)*cos(this->elem->inc))*sin(EKV) + sin(this->elem->w)*sin(this->elem->inc)*cos(EKV);

	qx = -sin(this->elem->w)*cos(this->elem->Node) - cos(this->elem->w)*sin(this->elem->Node)*cos(this->elem->inc);
	qy = (-sin(this->elem->w)*sin(this->elem->Node) + cos(this->elem->w)*cos(this->elem->Node)*cos(this->elem->inc))*cos(EKV) - cos(this->elem->w)*sin(this->elem->inc)*sin(EKV);
	qz = (-sin(this->elem->w)*sin(this->elem->Node) + cos(this->elem->w)*cos(this->elem->Node)*cos(this->elem->inc))*sin(EKV) + cos(this->elem->w)*sin(this->elem->inc)*cos(EKV);

	rx = sin(this->elem->inc)*sin(this->elem->Node);
	ry = -sin(this->elem->inc)*cos(this->elem->Node)*cos(EKV) - cos(this->elem->inc)*sin(EKV);
	rz= -sin(this->elem->inc)*cos(this->elem->Node)*sin(EKV) - cos(this->elem->inc)*cos(EKV);

	if((fabs(sqrt(px*px + py*py + pz*pz) - 1.0)>EPS)||(fabs(sqrt(qx*qx + qy*qy + qz*qz) - 1.0)>EPS)||((px*qx + py*qy + pz*qz)>EPS)) return 1;

	*x = px*ksi + qx*eta;
	*y = py*ksi + qy*eta;
	*z = pz*ksi + qz*eta;
*/
    double *vect;
    vect = new double[3];

    detRecEcl(&vect[0], &vect[1], &vect[2], t);

    RotX(vect, EKV);

    *x = vect[0];
    *y = vect[1];
    *z = vect[2];

	return 0;
}

int orbit::copyTo(orbit *dest)
{
//	this->line->copyTo(dest->line);

	dest->elem->eJD = this->elem->eJD;
	dest->elem->M0 = this->elem->M0;
	dest->elem->Node = this->elem->Node;
	dest->elem->w = this->elem->w;
	dest->elem->inc = this->elem->inc;
	dest->elem->ec = this->elem->ec;
	dest->elem->q = this->elem->q;

	return 0;
}

double orbit::get_a()
{
	return(this->elem->q/(1.0-this->elem->ec));
}

int orbit::detPolarOrbVel(double *Vr, double *Vn, double t)
{
	double r, v, p;

	if(this->elem->isElips())
	{
		this->detPolarOrb(&r, &v, t);

		p = this->elem->q*(1.0 + this->elem->ec);

		*Vr = ka*this->elem->ec*sin(v)/sqrt(p);
		*Vn = ka*(1.0 + this->elem->ec*cos(v))/sqrt(p);

		return 0;
	}


	return 1;
}

int orbit::detRecEclVel(double *Vx, double *Vy, double *Vz, double t)
{
	double Vr, Vn, vui, u;
        double x, y, z, r, r0;

	if(this->elem->isElips())
	{
		this->detPolarOrbVel(&Vr, &Vn, t);
                //printf("polar orb vel: %f\t%f\n", Vr, Vn);
		this->detRecEcl(&x, &y, &z, t);
		this->detPolarOrb(&r, &vui, t);

                r0 = sqrt(x*x + y*y + z*z);

                //printf("r0= %f\tr= %f\n", r0, r);

		u = this->elem->w + vui;

		*Vx = x*Vr/r + (-sin(u)*cos(this->elem->Node) - cos(u)*sin(elem->Node)*cos(elem->inc))*Vn;
		*Vy = y*Vr/r + (-sin(u)*sin(this->elem->Node) + cos(u)*cos(elem->Node)*cos(elem->inc))*Vn;
		*Vz = z*Vr/r + cos(u)*sin(elem->inc)*Vn;

		return 0;
	}

	return 1;
}

int orbit::detRecEkvVel(double *Vx, double *Vy, double *Vz, double t)
{
        double Vr, Vn, vui, u;
        double x, y, z, r, r0;
        double vel[3];

        if(this->elem->isElips())
        {
/*                this->detPolarOrbVel(&Vr, &Vn, t);
                //printf("polar orb vel: %f\t%f\n", Vr, Vn);
                this->detRecEkv(&x, &y, &z, t);
                this->detPolarOrb(&r, &vui, t);

                r0 = sqrt(x*x + y*y + z*z);

                //printf("r0= %f\tr= %f\n", r0, r);

                u = this->elem->w + vui;

                *Vx = x*Vr/r + (-sin(u)*cos(this->elem->Node) - cos(u)*sin(elem->Node)*cos(elem->inc))*Vn;
                *Vy = y*Vr/r + (-sin(u)*sin(this->elem->Node) + cos(u)*cos(elem->Node)*cos(elem->inc))*Vn;
                *Vz = z*Vr/r + cos(u)*sin(elem->inc)*Vn;
*/
            detRecEclVel(&vel[0], &vel[1], &vel[2], t);
            RotX(vel, EKV);

            *Vx = vel[0];
            *Vy = vel[1];
            *Vz = vel[2];

                return 0;
        }

        return 1;
}


////////////private


//Find

int findOrb(orbElem *orb, double *R0, double *V0, double t0)
{
	char *pv;
	pv = new char[100];


    double h, p, TT;
    double E0, ss, n;
    double a, B, sg0, r0rt0, v0, r0, vel0;
	double P1, P2, P3;
	double u0;
    double resin, recos, pa;
    double rsinu, rcosu;
    double Sign, dT;

    r0 = norm(R0);
    vel0 = norm(V0);
    r0rt0 = Smul3(R0, V0);

	h = vel0*vel0 - 2.0*ka*ka/r0;
    //printf("\nh	%f\n", h);

    P1 = R0[1]*V0[2] - R0[2]*V0[1];
    P2 = R0[0]*V0[2] - R0[2]*V0[0];
    P3 = R0[0]*V0[1] - R0[1]*V0[0];


    piNsys(&p, &orb->inc, &orb->Node, P1, P2, P3);

	if(fabs(h)<EPS)
	{
		printf("\nporabola\n");

        orb->q = 0.5*p;
        orb->ec = 1.0;

        sg0 = r0rt0/(ka*sqrt(2.0*orb->q));

        v0 = 2.0*atan2(r0rt0, ka*sqrt(2.0*orb->q));

		B = sqrt(2.0)*(sg0 + sg0*sg0*sg0/3.0)/ka;

        TT = t0 - pow(orb->q, 1.5)*B;

        P1 = R0[2]/sin(orb->inc);
        P2 = R0[0]*cos(orb->Node) + R0[1]*sin(orb->Node);

		u0 = atan2(P1, P2);

        orb->w = u0 - v0;
/*
		M0 = 0.0;
		eJD = (double)(int)TT;
		TT = TT-eJD;
		B = pow(q, -1.5)*TT;
		pKepler(&ss, B);
		M0 = 2.0*atan(ss);
*/

        orb->M0 = 0.0;
		Sign = 1.0;
//		eJD = (double)(int)this->appcnt->t;
        orb->eJD = (double)(int)TT;
        dT = orb->eJD - TT;
        B = pow(orb->q, -1.5)*dT;
		if(B<0.0) 
		{
			B = fabs(B);
			Sign = -1.0;
		}
		pKepler(&ss, B);
		ss = Sign*ss;
        orb->M0 = 2.0*atan(ss);


	}
	else if(h<0.0)
	{
		
        //printf("\nellips\n");

		resin = sqrt(p)*r0rt0/ka;
		recos = p - r0;
		v0 = atan2(resin, recos);
        orb->ec = sqrt((resin*resin + recos*recos)/(r0*r0));

		
        if(fabs(orb->inc)<EPS) orb->w = -v0;
		else
		{
            rsinu = R0[2]/sin(orb->inc);
            rcosu = R0[0]*cos(orb->Node) + R0[1]*sin(orb->Node);
			u0 = atan2(rsinu, rcosu);
            orb->w = u0 - v0;
        }

        pa = sqrt((1.0 - orb->ec)/(1.0 + orb->ec))*tan(v0/2.0);
        E0 = 2.0*atan(pa);

        orb->M0 = E0 - orb->ec*sin(E0);

        a = p/(1.0 - orb->ec*orb->ec);

		n = ka*pow(a, -1.5);

        orb->eJD = t0 - orb->M0/n;

        orb->q = a*(1.0 - orb->ec);

        if(orb->M0<0.0) orb->M0+=2.0*PI;

        dT = fmod(orb->eJD*2.0, 1.0)/2.0;
        orb->eJD -= dT;

        orb->M0 = n*dT + orb->M0;

		

//		return 1;
	}
	else if(h>0.0)
	{
		printf("\ngiperbola\n");

		return 2;
/*
		resin = sqrt(p)*r0rt0/ka;
		recos = p - r0;
		v0 = atan2(resin, recos);
		ecc = sqrt((resin*resin + recos*recos)/(r0*r0));

		if(fabs(inc)<EPS) w = -v0;
		else
		{
            rsinu = r[2)/sin(inc);
            rcosu = r[0)*cos(Node) + r[1)*sin(Node);
			u0 = atan2(rsinu, rcosu);
			w = u0 - v0;
		}

		pa = sqrt((1.0 - ecc)/(1.0 + ecc))*tan(v0/2.0);
//		E0 asin((();
*/
		
	}

    if(orb->w<0.0) orb->w+=2.0*PI;
    if(orb->inc<0.0) orb->inc+=2.0*PI;
    if(orb->Node<0.0) orb->Node+=2.0*PI;
    if(orb->M0<0.0) orb->M0+=2.0*PI;

	return 0;
}


int findOrbEkv(orbElem *orb, double *R0ekv, double *V0ekv, double t0) //Duboshin-Sprav_Ruk p.270
{
    char *pv;
    pv = new char[100];


    double h, p, p1, TT;
    double a1, inc, Node, w;
    double E0, ss, n;
    double a, B, sg0, r0rt0, v0, r0, vel0;
    double P1, P2, P3, P4, P5, d;
    double u0;
    double resin, recos, pa;
    double rsinu, rcosu;
    double Sign, dT;
//1
    r0 = norm(R0ekv);
    vel0 = norm(V0ekv);
    r0rt0 = Smul3(R0ekv, V0ekv);

    h = 2.0/r0 - vel0*vel0/ka*ka;
    printf("\nh	%f\n", h);

//3
    P1 = R0ekv[1]*V0ekv[2] - R0ekv[2]*V0ekv[1];
    P2 = R0ekv[0]*V0ekv[2] - R0ekv[2]*V0ekv[0];
    P3 = R0ekv[0]*V0ekv[1] - R0ekv[1]*V0ekv[0];


    piNsys(&p, &inc, &Node, P1, P2, P3);

//2
    if(h>EPS) //ellips
    {
        a1 = 1.0/fabs(h);
        P1 = r0rt0/ka/sqrt(a1);
        P2 = 1.0 - r0/a1;
        orb->ec = sqrt(P1*P1 + P2*P2);
        E0 = atan2(P1/orb->ec, P2/orb->ec);

        orb->M0 = E0 - P1;

        p1 = a1*(1.0 - orb->ec*orb->ec);
        printf("dp: %f", p1-p);
    }
    if(h<-EPS) //hiperbola
    {
        a1 = 1.0/fabs(h);
        P1 = r0rt0/ka/sqrt(a1);
        P2 = r0/a1 + 1;
        orb->ec = sqrt(-P1*P1 + P2*P2);
        E0 = atanh(P1/P2);//H0
        orb->M0 = t0 - (orb->ec*sinh(E0) - E0)/(ka*pow(a1, -1.5));
        p1 = a1*(orb->ec*orb->ec - 1.0);
        printf("dp: %f", p1-p);
    }
    if(fabs(h)<EPS) //parabola
    {

    }


//4
    v0 = atan2(sqrt(p)*r0rt0, ka*(p - r0));
    u0 = atan2(R0ekv[2]/sin(inc), R0ekv[0]*cos(Node)+R0ekv[1]*sin(Node));

    w = u0-v0;

//5
    P1 = sin(inc)*sin(Node);
    P2 = -cos(inc)*sin(EKV) + sin(inc)*cos(EKV)*cos(Node);
    P3 = cos(inc)*cos(EKV) + sin(inc)*sin(EKV)*cos(Node);
    P4 = sin(EKV)*sin(Node);
    P5 = sin(inc)*cos(EKV) - cos(inc)*sin(EKV)*cos(Node);

    orb->Node = atan2(P1, P2);
    orb->inc = atan2(P1, P3)/sin(orb->Node);
    d = atan2(P4, P5);
    orb->w = w - d;

    orb->q = p;
/*
    if(orb->w<0.0) orb->w+=2.0*PI;
    if(orb->inc<0.0) orb->inc+=2.0*PI;
    if(orb->Node<0.0) orb->Node+=2.0*PI;
    if(orb->M0<0.0) orb->M0+=2.0*PI;
*/
    return 0;
}

///////////////// class orbs //////////////////////////////
/*
orbs::orbs()
{
	this->orb = new orbit;
	this->next = NULL;
}

orbs::~orbs()
{
	delete(this->orb);
	delete(this->next);
}


///////////////// class orbits ////////////////////////////

orbits::orbits()
{
	this->curent = new orbit;
//	this->params->obsProg = NULL;
	this->params = NULL;
//	this->VP = NULL;
//	this->SP = NULL;
//	this->DP = NULL;
//	this->vars = new Variables;

	this->fn_cat = NULL;
//	this->fn_obs = NULL;
//	this->fn_res = NULL;
//	this->fn_varList = NULL;
//	this->fn_log = NULL;

//	this->Rlist = NULL;

	this->norbs = 0;
//	this->nres = 0;
//	this->nRl = 0;

	this->FCAT = NULL;
//	this->VCAT = NULL;
//	this->FCFG = NULL;

	this->earth = NULL;

	this->err_str = new char[124];
	strcpy(this->err_str, "Unnown error\n");
}

orbits::~orbits()
{

	delete(this->curent);
//	delete(this->params);
//	delete(this->VP);
//	delete(this->SP);
//	delete(this->DP);
//	delete(this->vars);

	delete(this->fn_cat);
//	delete(this->fn_obs);
//	delete(this->fn_res);
//	delete(this->fn_varList);

	this->norbs = 0;

	if(this->FCAT!=NULL) fclose(this->FCAT);
	this->FCAT=NULL;

//	if(this->FCFG!=NULL) fclose(this->FCFG);
//	this->FCFG=NULL;

}

int orbits::addorbStr(char *str)
{
	if(this->curent->initStr(str))return 1;

	return 0;
}

int orbits::getorb(int num)
{
	char *str, *nstr, *Sline;
//	int STRLEN;
//	STRLEN = 1000;
	str = new char[STRLEN];
	nstr = new char[STRLEN];
	Sline = new char[STRLEN];

	fseek(FCAT, 189*num, SEEK_SET);
	
	strcpy(str, "");
	strcpy(nstr, "");
	strcpy(Sline, "");

	strcpy(str, nstr);
	fgets(nstr, STRLEN, FCAT);
	strcpy(Sline, nstr);
	if(!feof(FCAT))fgets(nstr, STRLEN, FCAT);
	else return 0;
	strcat(Sline, nstr);
	if(!feof(FCAT))fgets(nstr, STRLEN, FCAT);
	else return 0;
	strcat(Sline, nstr);
	strcpy(str, nstr);
//	if(!feof(FCAT))fgets(nstr, STRLEN, FCAT);
//	else return 0;

	this->addorbStr(Sline);

	fseek(FCAT, 0, SEEK_SET);

	delete(str);
	delete(nstr);
	delete(Sline);

	return 0;
}

int orbits::getorb_name(char *oname)
{
	char *str, *nstr, *Sline;
//	int STRLEN;
//	STRLEN = 1000;
	str = new char[STRLEN];
	nstr = new char[STRLEN];
	Sline = new char[STRLEN];

	fseek(FCAT, 0, SEEK_SET);

	while(!feof(FCAT))
	{
		strcpy(str, "");
		strcpy(nstr, "");
		strcpy(Sline, "");

		strcpy(str, nstr);
		fgets(nstr, STRLEN, FCAT);
		strcpy(Sline, nstr);
		if(!feof(FCAT))fgets(nstr, STRLEN, FCAT);
		else return 1;
		strcat(Sline, nstr);
		if(!feof(FCAT))fgets(nstr, STRLEN, FCAT);
		else return 1;
		strcat(Sline, nstr);
		strcpy(str, nstr);

		this->addorbStr(Sline);
		if(streqv(this->curent->line->name, oname)) break;
	}

	fseek(FCAT, 0, SEEK_SET);

	free(str);
	free(nstr);
	free(Sline);

	return 0;
}

int orbits::initFileCat(char *fname)
{
	this->norbs = 0;
	if(this->FCAT!=NULL) fclose(FCAT);
	char *str, *nstr, *Sline;
//	int STRLEN;
//	STRLEN = 1000;
	str = new char[STRLEN];
	nstr = new char[STRLEN];
	Sline = new char[STRLEN];
	FCAT = fopen(fname, "r");
	if(FCAT==NULL)
	{
		printf("\n\norbits::initFile: Error: failed open initial File %s\n", fname);
		return 1;
	}

	strcpy(str, "");
	strcpy(nstr, "");
	strcpy(Sline, "");

	fgets(nstr, STRLEN, FCAT);

	while(!feof(FCAT))
	{

//		fseek(FCAT, 189*num, SEEK_SET);
		strcpy(str, nstr);
		strcpy(Sline, nstr);
//		fgets(nstr, STRLEN, FCAT);
//		strcpy(Sline, nstr);
		if(!feof(FCAT))fgets(nstr, STRLEN, FCAT);
		else return 0;
		strcat(Sline, nstr);
		if(!feof(FCAT))fgets(nstr, STRLEN, FCAT);
		else return 0;
		strcat(Sline, nstr);
		strcpy(str, nstr);
		if(!feof(FCAT))fgets(nstr, STRLEN, FCAT);
		else return 0;

		if(streqv(str, nstr))
		{
			this->addorbStr(Sline);
			this->norbs++;
			return 0;
		}

		this->addorbStr(Sline);

		this->norbs++;
	}


	fclose(FCAT);
	FCAT = NULL;

	return 0;
}

int orbits::dispOneCat(char *str, int num)
{

	strcpy(str, "");
	

	this->getorb(num);

	this->curent->dispCat(str);

	return 0;
}


int orbits::detRDnumGC(double *RA, double *DEC, double tUTC, double x, double y, double z)
{
	double Dtime;

	UTC2TDT(tUTC, &Dtime);

//	this->params->OrbT->detRecEkv(&this->vars->Obj->x, &this->vars->Obj->y, z, Dtime);

	double P1, P2, P3;

//	double dt;
//	dt = sqrt(P1*P1 + P2*P2 + P3*P3)/CAU;
///	redAber(&this->params->Earth->x, &this->params->Earth->y, &this->params->Earth->z, this->params->Earth->xt, this->params->Earth->yt, this->params->Earth->zt, dt);

	P1 = x - (this->params->Earth->x + this->params->Obs->x);
	P2 = y - (this->params->Earth->y + this->params->Obs->y);
	P3 = z - (this->params->Earth->z + this->params->Obs->z);
/*
	myvector *radec;
	radec = new myvector(3);

	radec->set(0, P1);
	radec->set(1, P2);
	radec->set(2, P3);

	myvector *radec_x, *ort_x;
	radec_x = new myvector(2);
	ort_x = new myvector(2);

	radec_x->set(0, radec->get(0));
	radec_x->set(1, radec->get(1));

	ort_x->set(0, 1.0);
	ort_x->set(1, 0.0);

	double t_ra, t_dec;

	t_ra = 
/

	rdsys(RA, DEC, P1, P2, P3);

	return 0;
}

int orbits::detAhnumGC(double *A, double *h, double tUTC, double lam, double fi, double RA, double DEC)
{
	double P1, P2, P3;

	double s;

	UTC2s(tUTC, lam, &s);

//	this->VP->s = 3.25;

	P1 = -cos(DEC)*sin(s - RA);
	P2 = -cos(DEC)*cos(s - RA)*sin(fi) + sin(DEC)*cos(fi);
	P3 = cos(DEC)*cos(s - RA)*cos(fi) + sin(DEC)*sin(fi);

	*A = atan2(P1, P2);
	if(*A<0.0) *A+=2.0*PI;
	if(*A>2.0*PI) *A-=2.0*PI;

//	this->vars->*h = atan2(P3*cos(this->vars->v_vars->A), P2);
//	if(fabs(*A)<PI/2.0)	*h = atan(P3*sin(*A)/P1);
//	else *h = atan(-P3*sin(*A)/P1);
	
	*h = atan2(P3*sin(*A), P1);

	if(*h>PI/2.0)
	{
//		*h = PI - *h;
		*h -=PI;
//		*A += PI;
	}
	if(*h<-PI/2.0)
	{
		*h += PI;// - *h;
//		*A += PI;
	}

	if(*A<0.0) *A+=2.0*PI;
	if(*A>2.0*PI) *A-=2.0*PI;

//	double t_A, t_h;

//	rdsys(A, h, P1, P2, P3);

	return 0;
}

int orbits::init(char *fn_cat, char *fn_obs, char *fn_earth)
{
	if(this->params==NULL)this->params = new(OrbitsParams);
	this->params->Earth = new note;
	this->params->Obs = new note;

	if(this->earth==NULL) this->earth = new rec;

	this->earth->initFile(fn_earth);
	this->initFileCat(fn_cat);
	this->params->st = new observatory;
	this->params->st->initFile(fn_obs);

	this->det_h();

	return 0;
}

int orbits::set_observatory(int num)
{
	obsy *stan;
	stan = new obsy;

	if(this->params->st->getobsynum(stan, num)) return 1;

	this->params->lam = stan->Long;
	this->params->fi = atan2(stan->Sin, stan->Cos);
	this->params->cosfi = stan->Cos;
	this->params->sinfi = stan->Sin;

	return 0;
}

int orbits::set_observatory(char *name)
{
	obsy *stan;
	stan = new obsy;

	if(this->params->st->getobsyname(stan, name)) return 1;

	this->params->lam = stan->Long;
	this->params->fi = atan2(stan->Sin, stan->Cos);
	this->params->cosfi = stan->Cos;
	this->params->sinfi = stan->Sin;

	return 0;
}

int orbits::retime(double tUTC)
{
	this->params->UTC = tUTC;
	UTC2TDT(tUTC, &this->params->TDT);
	this->params->TDB = 0.0;

	UTC2s(this->params->UTC, this->params->lam, &this->params->s);

	if(this->params->Earth==NULL)	this->params->Earth = new note;
	this->params->Earth->eJD = this->params->TDT;
	if(this->earth->getNoteTime(this->params->Earth, this->params->Earth->eJD))
	{
		strcpy(this->err_str, "Earth.rec is not support this time\n");
		return 1;
	}

	if(this->params->Obs==NULL) this->params->Obs = new note;
	this->params->Obs->eJD = this->params->TDT;
	this->det_ro();
//	detObsEthCentr(this->params->Obs);

	return 0;
}

int orbits::det_ro()
{

	this->params->Obs->x = this->params->h*this->params->cosfi*cos(this->params->s);
	this->params->Obs->y = this->params->h*this->params->cosfi*sin(this->params->s);
	this->params->Obs->z = this->params->h*this->params->sinfi;
	return 0;
}

int orbits::det_h()
{
//WARN
	this->params->h = 6378.140/AUKM;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

int orbElem::detPolarOrb(double *r, double *v, double t)
{
	if(this->isElips())
	{
		double T, n, M, E, p;

		p = q*(1.0 + ec);
		n = xi*pow((double)(q/(1.0-ec)), (double)-1.5);
		T = eJD - M0/n;
		M = n*(t - eJD) + M0;
		eKepler(&E, M, ec);

		double Pr1, Pr2;
		Pr1 = q*sqrt((1.0 + ec)/(1.0 - ec))*sin(E);
		Pr2 = q*(cos(E) - ec)/(1.0 - ec);

		double viu;

		viu = fatan(Pr1, Pr2);

		*r = p/(1.0 + ec*cos(viu));
		*v = viu;

		return 1;
	}
	else if(this->isPorabola())
	{
		double B, s, T, dT, n;
		int key = 0;
//
		this->findT(&T, M0, eJD);

		dT = t - T;
		

		B = pow(q, -1.5)*dT;
		if(B<0.0)
		{
			key = 1;
			B = -B;
		}

		pKepler(&s, B);
//
//		s = -tan(M0/2.0);
		*r = q*(1.0 + s*s);
		*v = 2.0*atan(s);
		if(key) *v = 2.0*PI - fabs(*v);

		return 2;

	}
	else if(this->isGiperbola())
	{
/*		double M, a, T, H, n;

		a = q/(1.0 - ec);
		a = fabs(a);

		n = xi*pow(a, -1.5);
//		T = eJD - M0/n;
		M = n*(t - eJD) + M0;

		this->gKepler(&H, M);

		*v = fatan(a*sqrt(ec*ec - 1.0)*sinh(H), a*(ec - cosh(H)));
		*r = a*(ec*cosh(H) - 1.0);
//		M = xi*fabs(a)*(t - T);
/
		return 3;

	}
	else if(this->isNPorabola())
	{
		double B, s, T;
		int key = 0;

		this->findT(&T, M0, eJD);

		B = pow(q, -1.5)*(t - T);
		if(B<0.0)
		{
			key = 1;
			B = -B;
		}

		npKepler(&s, B, ec);

		double eps, ksi;
		eps = (1.0-ec)/2.0;
		ksi = eps*s*s;

		*r = q*(1.0 + ec*s*s);
		*v = 2.0*atan(s*sqrt((1.0-eps)/(1.0-ksi)));
		if(key) *v = 2.0*PI - fabs(*v);

		return 2;

	}
	else return 1;

	
	return 0;
}

int orbElem::detRecOrb(double *ksi, double *eta, double t)
{
	if(this->isPorabola())
	{
		double r, v, s;
		this->detPolarOrb(&r, &v, t);
		s = tan(v/2.0);
		*ksi = q*(1.0 - s*s);
		*eta = 2.0*q*s;

		return 0;
	}
	else if(this->isElips())
	{
		double r, v;

		this->detPolarOrb(&r, &v, t);

		*ksi = r*cos(v);
		*eta = r*sin(v);

		return 0;
	}
	else if(this->isGiperbola())
	{
/*		double r, v;
		this->detPolarOrb(&r, &v, t);
		*ksi = r*cos(v);
		*eta = r*sin(v);/
		return 1;
	}
	else if(this->isNPorabola())
	{
		double B, s, T;
		int key = 0;

		this->findT(&T, M0, eJD);

		B = pow(q, -1.5)*(t - T);
		if(B<0.0)
		{
			key = 1;
			B = -B;
		}

		npKepler(&s, B, ec);

		double eps, dz;
		eps = (1.0-ec)/2.0;
		dz = eps*s*s;

		*ksi = q*(1.0 - s*s);
		*eta = sqrt(2.0)*ka*q*sqrt(1.0 - eps)*s*npU(dz);
		return 0;
	}
	return 1;
}


int orbElem::detRecEcl(double *x, double *y, double *z, double t)
{
//1-j sposob
/*
	double ksi, eta;
	if(this->isElips())
	{
		this->detRecOrb(&ksi, &eta, t);
//		return 1;
	}

	if(this->isPorabola())
	{
		this->detRecOrb(&ksi, &eta, t);

//		return 2;
	}

	double px, py, pz, qx, qy, qz, rx, ry, rz;
	
	px = cos(w)*cos(Node) - sin(w)*sin(Node)*cos(inc);
	py = cos(w)*sin(Node) + sin(w)*cos(Node)*cos(inc);
	pz = sin(w)*sin(inc);

	qx = -sin(w)*cos(Node) - cos(w)*sin(Node)*cos(inc);
	qy = -sin(w)*sin(Node) + cos(w)*cos(Node)*cos(inc);
	qz = cos(w)*sin(inc);

	rx = sin(inc)*sin(Node);
	ry = -sin(inc)*cos(Node);
	rz= cos(inc);

	if((fabs(sqrt(px*px + py*py + pz*pz) - 1.0)>EPS)||(fabs(sqrt(qx*qx + qy*qy + qz*qz) - 1.0)>EPS)||(sqrt(px*qx + py*qy + pz*qz)>EPS)) return 1;

	*x = px*ksi + qx*eta;
	*y = py*ksi + qy*eta;
	*z = pz*ksi + qz*eta;
/
//2-j sposob

	double r, v;
	this->detPolarOrb(&r, &v, t);

	double u = w + v;

	*x = r*(cos(u)*cos(Node) - sin(u)*sin(Node)*cos(inc));
	*y = r*(cos(u)*sin(Node) + sin(u)*cos(Node)*cos(inc));
	*z = r*sin(u)*sin(inc);

	return 0;
}

int orbElem::detRecEkv(double *x, double *y, double *z, double t)
{
	double ksi, eta;

	if(this->detRecOrb(&ksi, &eta, t)) return 1;

	double px, py, pz, qx, qy, qz, rx, ry, rz;

	px = cos(w)*cos(Node) - sin(w)*sin(Node)*cos(inc);
	py = (cos(w)*sin(Node) + sin(w)*cos(Node)*cos(inc))*cos(EKV) - sin(w)*sin(inc)*sin(EKV);
	pz = (cos(w)*sin(Node) + sin(w)*cos(Node)*cos(inc))*sin(EKV) + sin(w)*sin(inc)*cos(EKV);

	qx = -sin(w)*cos(Node) - cos(w)*sin(Node)*cos(inc);
	qy = (-sin(w)*sin(Node) + cos(w)*cos(Node)*cos(inc))*cos(EKV) - cos(w)*sin(inc)*sin(EKV);
	qz = (-sin(w)*sin(Node) + cos(w)*cos(Node)*cos(inc))*sin(EKV) + cos(w)*sin(inc)*cos(EKV);

	rx = sin(inc)*sin(Node);
	ry = -sin(inc)*cos(Node)*cos(EKV) - cos(inc)*sin(EKV);
	rz= -sin(inc)*cos(Node)*sin(EKV) - cos(inc)*cos(EKV);

	if((fabs(sqrt(px*px + py*py + pz*pz) - 1.0)>EPS)||(fabs(sqrt(qx*qx + qy*qy + qz*qz) - 1.0)>EPS)||((px*qx + py*qy + pz*qz)>EPS)) return 1;

	*x = px*ksi + qx*eta;
	*y = py*ksi + qy*eta;
	*z = pz*ksi + qz*eta;

	return 0;
}
*/
int orbElem::isElips()
{
	if(ec<=0.9) return 1;
	return 0;
}

int orbElem::isNPorabola()
{
	if((ec<1.15)&&(ec>0.90)&&(fabs(ec-1.0)>EPS)) return 1;
	return 0;
}

int orbElem::isPorabola()
{
	if(fabs(ec-1.0)<EPS) return 1;
	return 0;
}


int orbElem::isGiperbola()
{
	if(ec>=1.15) return 1;
	return 0;
}


//Find
/*
int orbElem::FindOrb(myvector *R0, myvector *V0, double t0, int log)
{
//	rt = new myvector(3);
//	int n;
//	n = this->comvaR0->last;
/*	R0->set(0, rpos[0]);
	R0->set(1, rpos[1]);
	R0->set(2, rpos[2]);
	V0->set(0, vel[0]);
	V0->set(1, vel[1]);
	V0->set(2, vel[2]);/

	char *pv;
	pv = new char[100];


	double h;
	double p, inc, Node, TT;
	double E0, M0, eJD, ss, n;
	double a, ecc, q, B, sg0, r0rt0, v0, r0, vel0;
	double P1, P2, P3;
	double u0;
	double w;
	double resin, recos, pa;
	double rsinu, rcosu;

	r0 = R0->norm();
	vel0 = V0->norm();
	r0rt0 = Smul(R0, V0);

	h = vel0*vel0 - 2.0*ka*ka/r0;
	printf("\nh	%f\n", h);

	P1 = R0->get(1)*V0->get(2) - R0->get(2)*V0->get(1);
	P2 = R0->get(0)*V0->get(2) - R0->get(2)*V0->get(0);
	P3 = R0->get(0)*V0->get(1) - R0->get(1)*V0->get(0);


	piNsys(&p, &inc, &Node, P1, P2, P3);
//	piNsys(&p, &inc, &Node, P1, P2, P3);

	if(fabs(h)<EPS)
	{
		printf("\nporabola\n");

		q = 0.5*p;
		ecc = 1.0;

		sg0 = r0rt0/(ka*sqrt(2.0*q));

		v0 = 2.0*atan2(r0rt0, ka*sqrt(2.0*q));

		B = sqrt(2.0)*(sg0 + sg0*sg0*sg0/3.0)/ka;

		TT = t0 - pow(q, 1.5)*B;

		P1 = R0->get(2)/sin(inc);
		P2 = R0->get(0)*cos(Node) + R0->get(1)*sin(Node);

		u0 = atan2(P1, P2);

		w = u0 - v0;
/*
		M0 = 0.0;
		eJD = (double)(int)TT;
		TT = TT-eJD;
		B = pow(q, -1.5)*TT;
		pKepler(&ss, B);
		M0 = 2.0*atan(ss);
/
		double Sign, dT;
		M0 = 0.0;
		Sign = 1.0;
//		eJD = (double)(int)this->appcnt->t;
		eJD = (double)(int)TT;
		dT = eJD - TT;
		B = pow(q, -1.5)*dT;
		if(B<0.0) 
		{
			B = fabs(B);
			Sign = -1.0;
		}
		pKepler(&ss, B);
		ss = Sign*ss;
		M0 = 2.0*atan(ss);
		if(M0<0.0) M0+=2.0*PI;


		if(w<0.0) w+=2.0*PI;
//		if(Node<0.0) Node+=2.0*PI;
//		if(inc<0.0) inc+=2.0*PI;

	}
	else if(h<0.0)
	{
		
		printf("\nellips\n");

		resin = sqrt(p)*r0rt0/ka;
		recos = p - r0;
		v0 = atan2(resin, recos);
		ecc = sqrt((resin*resin + recos*recos)/(r0*r0));

		
		if(fabs(inc)<EPS) w = -v0;
		else
		{
			rsinu = R0->get(2)/sin(inc);
			rcosu = R0->get(0)*cos(Node) + R0->get(1)*sin(Node);
			u0 = atan2(rsinu, rcosu);
			w = u0 - v0;
		}

		pa = sqrt((1.0 - ecc)/(1.0 + ecc))*tan(v0/2.0);
		E0 = 2.0*atan(pa);

		M0 = E0 - ecc*sin(E0);

		a = p/(1.0 - ecc*ecc);

		n = ka*pow(a, -1.5);

		eJD = t0 - M0/n;

		q = a*(1.0 - ecc);

		if(M0<0.0) M0 += 2.0*PI;

		double dT;

		dT = fmod(eJD*2.0, 1.0)/2.0;
		eJD -= dT;

		M0 = n*dT + M0;

		

//		return 1;
	}
	else if(h>0.0)
	{
		printf("\ngiperbola\n");

		return 2;
/*
		resin = sqrt(p)*r0rt0/ka;
		recos = p - r0;
		v0 = atan2(resin, recos);
		ecc = sqrt((resin*resin + recos*recos)/(r0*r0));

		if(fabs(inc)<EPS) w = -v0;
		else
		{
			rsinu = r->get(2)/sin(inc);
			rcosu = r->get(0)*cos(Node) + r->get(1)*sin(Node);
			u0 = atan2(rsinu, rcosu);
			w = u0 - v0;
		}

		pa = sqrt((1.0 - ecc)/(1.0 + ecc))*tan(v0/2.0);
//		E0 asin((();
/
		
	}

	if(w<0.0) w+=2.0*PI;
	if(inc<0.0) inc+=2.0*PI;
	if(Node<0.0) Node+=2.0*PI;

//	this->initElem(eJD, M0, w, Node, inc, ecc, q);

	return 0;
}
*/
