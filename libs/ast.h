#include <stdio.h>
#include "comfunc.h"

#ifndef COORD_H
#include "coord.h"
#endif

#ifndef AST_H

class cap
{
public:
	char *str;

	int num;		//nomer po catalogu
	char *name;		//imya -//-
	int typeTime;	//tip vremeni: DT<=>0, UT<=>1, UT1<=>2

	int typeCoord;	//tip koordinat: Astrometric<=>0, Appearence<=>1, Geometric<=>2
	int typeEqx;	//tip epohi: J<=>0, B<=>1
	float Eqx;		//epoha, naprimer 2000.0
	char *pert;		//perturbacii: perevoditsya v celoe chislo is svoego dvoichnogo predstavleniya
	double acc;		//1.d-9 schitaetsya tochnost'iu, eto budet 1.0e-9
	int center;		//Earth<=>3, Sun<=>0
	int obs;		//nomer stancii, Center<=>0;

	cap();
	cap(int mode);
	~cap();

	int det();

private:
	int word2int(char *slo, int *chislo);
	int int2word(char *slo, int chislo);

	int mode;		//mode=0 - ast;		//mode=1 - gmt
};

class app
{
public:
	double eJD;

	double r, d, gd, hd, pha, vmagn, elong;
	int ed;
	double rt, dt, gdt;
	double rtt, dtt, gdtt;
};


struct apps
{
	app *pos;
	apps *next;
};

class ast
{
public:
	apps *lapp;
	cap *hat;
	int napps;
	FILE *FC;

	ast();
	~ast();

	int idatamode;
	int iexp;

	int detMode(char *fname);

	int addApp(double eJD, double r, double d, double gd, double hd, double pha, double vmagn, double elong, int ed, double rt, double dt, double gdt, double rtt, double dtt, double gdtt);
	int delApp(int num);

	int initFile(char *fname);

	int getApp(app *n, int nofapp);

	int GetCoordList(List4 *coord);

	int writeApp(char *fname, int mode, int nofn);	//mode=0 new file, mode=1 add old file	//WORK NOT CORRECT!!!!!//work only mode=0, nofn=*

	int hating();

private:

	int makeStrPos(app *pos, char *str);
};

#define AST_H
#endif