#include "stdafx.h"
#include "ast.h"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>



////////////////////////cap
////////////public
cap::cap()
{
	this->str = new char[1000];

	this->name = new char[32];
	this->pert = new char[16];

}

cap::cap(int mode)
{
	this->str = new char[1000];

	this->name = new char[32];
	this->pert = new char[16];

	this->mode = mode;

}


cap::~cap()
{
	delete [] this->str;// = new char[1000];
	delete [] this->name;
	delete [] this->pert;
}

int cap::det()
{
	if(this->mode==0)
	{
		char *str1;
		str1 = new char[100];

		char *slo;
		slo = new char[100];

		strcpy(str1, "");
		slovoG(this->str, str1, 0, 95);

		slovoG(str1, slo, 0, 9);
		this->num = atoi(slo);
		slovoG(str1, slo, 10, 42);
		strcpy(this->name, slo);
		slovoG(str1, slo, 84, 86);
		if(strstr(slo, "DT")) this->typeTime = 0;
		if(strstr(slo, "UT")) this->typeTime = 1;
		if(strstr(slo, "UT1")) this->typeTime = 2;

		strcpy(str1, "");
		slovoG(this->str, str1, 96, 175);

		slovoG(str1, slo, 0, 25);
		if(strstr(slo, "Astrometric")) this->typeCoord = 0;
		if(strstr(slo, "Appearence")) this->typeCoord = 1;
		if(strstr(slo, "Geometric")) this->typeCoord = 2;

		slovoG(str1, slo, 36, 43);
		if(strstr(slo, "J")) this->typeEqx = 0;
		if(strstr(slo, "B")) this->typeEqx = 1;

		this->Eqx = atof(&slo[1]);

		slovoG(str1, slo, 54, 70);
		word2one(slo, this->pert);
//	strcpy(this->pert, slo);

		int step;
		step = atoi(&slo[6]);
		this->acc = pow(10.0, -step);

		strcpy(str1, "");
		slovoG(this->str, str1, 176, 231);

		slovoG(str1, slo, 36, 44);
		if(strstr(slo, "Sun")) this->center = 0;
		else if(strstr(slo, "Mercury")) this->center = 1;
		else if(strstr(slo, "Venus")) this->center = 2;
		else if(strstr(slo, "Earth")) this->center = 3;
		else if(strstr(slo, "Mars")) this->center = 4;
		else if(strstr(slo, "Jove")) this->center = 5;
		else if(strstr(slo, "Saturn")) this->center = 6;
		else this->center = 7;

		slovoG(str1, slo, 45, 58);
		this->obs = atoi(slo);

		delete [] str1;
		delete [] slo;
	}
	else if(this->mode==1)
	{
		char *str1;
		str1 = new char[100];

		char *slo;
		slo = new char[100];

		strcpy(str1, "");
		slovoG(this->str, str1, 0, 80);

		slovoG(str1, slo, 0, 9);
		this->num = atoi(slo);
		slovoG(str1, slo, 10, 42);
		strcpy(this->name, slo);
		slovoG(str1, slo, 68, 71);
		if(streqv(slo, "DT ")) this->typeTime = 0;
		if(streqv(slo, "UT ")) this->typeTime = 1;
		if(streqv(slo, "UT1")) this->typeTime = 2;

		strcpy(str1, "");
		slovoG(this->str, str1, 80, 159);

		slovoG(str1, slo, 1, 12);
		if(strstr(slo, "Astrometric")) this->typeCoord = 0;
		if(strstr(slo, "Appearence")) this->typeCoord = 2;
		if(strstr(slo, "Geometric")) this->typeCoord = 1;

		slovoG(str1, slo, 36, 43);
		if(strstr(slo, "J")) this->typeEqx = 0;
		if(strstr(slo, "B")) this->typeEqx = 1;

		this->Eqx = atof(&slo[1]);

		slovoG(str1, slo, 54, 70);
		word2one(slo, this->pert);
//	strcpy(this->pert, slo);

		slovoG(str1, slo, 71, 79);
		int step;
		step = atoi(&slo[6]);
		this->acc = pow(10.0, -step);

		strcpy(str1, "");
		slovoG(this->str, str1, 160, 250);
/*
		slovoG(str1, slo, 36, 44);
		if(strstr(slo, "Sun")) this->center = 0;
		else if(strstr(slo, "Mercury")) this->center = 1;
		else if(strstr(slo, "Venus")) this->center = 2;
		else if(strstr(slo, "Earth")) this->center = 3;
		else if(strstr(slo, "Mars")) this->center = 4;
		else if(strstr(slo, "Jove")) this->center = 5;
		else if(strstr(slo, "Saturn")) this->center = 6;
		else this->center = 7;

		slovoG(str1, slo, 45, 58);
		this->obs = atoi(slo);
*/
		delete [] str1;
		delete [] slo;
	}
	else return 1;

	return 0;
}

int ast::GetCoordList(List4 *coord)
{
	if(coord==NULL) return 1;

	double *x;
	x = new double[3];

	coord->Free();

	app *tapp;
	tapp = new app;

	char *nobs;
	nobs = new char[16];

	for(int i=0; i<this->napps; i++)
	{
		if(this->getApp(tapp, i)) return 2;

		x[0] = tapp->eJD;
		x[1] = tapp->r;
		x[2] = tapp->d;
		
		if(this->hat->center==3) sprintf(nobs, "%03d", this->hat->obs);
		else strcpy(nobs, "");

//		coord->Push(x);

		coord->Push(x, nobs, -1, -1, 0);
	}

	delete [] x;
	delete tapp;
	delete [] nobs;

	return 0;
}

int cap::word2int(char *slo, int *chislo)
{
	int i, k;

	char *slo1;
	int len = strlen(slo);
	slo1 = new char[len];
	strcpy(slo1, "");

	for(i=0; i<len; i++)
	{
		if(strncmp(&slo[i], "0", 1)==0) strcat(&slo1[i], "0");
		else strcat(&slo1[i], "1");

	}

	int num;
	num = 0;
	for(i=0; i<len; i++)
	{
		k = (strncmp(&slo1[len-i-1], "1", 1)==0);

		num += k*pow(2.0, i);
	}

	*chislo = num;

	delete [] slo1;

	return 0;
}

int cap::int2word(char *slo, int chislo)
{

/*
	int i, k;

	char *slo1;
	int len = strlen(slo);
	slo1 = new char[len];
	strcpy(slo1, "");

	for(i=0; i<len; i++)
	{
		if(strncmp(&slo[i], "0", 1)==0) strcat(&slo1[i], "0");
		else strcat(&slo1[i], "1");

	}

	int num;
	num = 0;
	for(i=0; i<len; i++)
	{
		k = (strncmp(&slo1[len-i-1], "1", 1)==0);

		num += k*pow(2.0, i);
	}
*/

	return 0;
}

////////////////////////ast
////////////public

ast::ast()
{
	this->lapp = NULL;
	this->napps = 0;
	this->hat = NULL;
	iexp = 0;
}

ast::~ast()
{
	delete(this->lapp);
//	delete(this->FC);
	delete(this->hat);
	
	this->lapp = NULL;

	this->hat = NULL;
}

int ast::addApp(double eJD, double r, double d, double gd, double hd, double pha, double vmagn, double elong, int ed, double rt, double dt, double gdt, double rtt, double dtt, double gdtt)
{
	apps *temp, *last;

	temp = this->lapp;
	
	while(this->lapp!=NULL)
	{
		last = this->lapp;
		this->lapp = this->lapp->next;
	}

	this->lapp = NULL;
	this->lapp = new apps;
	this->lapp->pos = new app;
	this->lapp->next = NULL;

	this->lapp->pos->eJD = eJD;
	this->lapp->pos->r = r;
	this->lapp->pos->d = d;
	this->lapp->pos->gd = gd;
	this->lapp->pos->hd = hd;
	this->lapp->pos->pha = pha;
	this->lapp->pos->vmagn = vmagn;
	this->lapp->pos->elong = elong;
	this->lapp->pos->ed = ed;

	this->lapp->pos->rt = rt;
	this->lapp->pos->dt = dt;

	this->lapp->pos->gdt = gdt;
	this->lapp->pos->rtt = rtt;
	this->lapp->pos->dtt = dtt;
	this->lapp->pos->gdtt = gdtt;

	if(temp!=NULL)
	{
		last->next = this->lapp;
		this->lapp = temp;
	}

	this->napps++;

	return 0;
}

int ast::delApp(int num)
{
	if(num>=this->napps) return 2;
	apps *before, *after;

	before = NULL;
	after = this->lapp;

	int i;

	i=0;

	while((i<num))
	{
		before = after;
		if(before==NULL) return 1;

		after = after->next;

		i++;
	}

	if(before!=NULL)
	{
		before->next = after->next;
		delete(after);
	}
	else if(num==0) this->lapp = this->lapp->next;
		else return 1;

	this->napps--;

	return 0;
}

int ast::detMode(char *fname)
{
	char *fn, *ext;
	fn = new char[256];
	strcpy(fn, "");
	ext = new char[256];
	strcpy(ext, "");
	int len = strlen(fname);

	strncpy(fn, fname, len-4);
	strcpy(&fn[len-4], "\0");
	strcpy(ext, &fname[len-3]);
	strcpy(&fn[3], "\0");

	if(streqv(ext, "ast")) this->idatamode = 0;
	if(streqv(ext, "gmt")) this->idatamode = 1;

	delete [] fn;
	delete [] ext;

	return 0;
}

int ast::initFile(char *fname)
{
	this->detMode(fname);
	FILE *FN;
	char *str, *nstr;

//	int STRLEN;
//	STRLEN = 1000;

	str = new char[STRLEN];
	nstr = new char[STRLEN];

	FN = fopen(fname, "r");
	if(FN==NULL) return 1;
	
	int i;
	i=0;

	strcpy(str, "");
	strcpy(nstr, "");

//hat
	this->hat = new cap(this->idatamode);
	strcpy(this->hat->str, "");
//
	

	for(i=0; i<6; i++)
	{
		strcpy(nstr, str);
		fgets(str, STRLEN, FN);
		strcat(this->hat->str, nstr);	//hat
	}
	strcat(this->hat->str, "\0");

	this->hat->det();

//	printf("%s", this->hat);
	
	int year, mth;
	int sign;
	double numb;
	char *slo, *s;

	

	slo = new char[36];
	s = new char[256];
	
//	sproba = new char[32];

	

	double eJD,  r, d, gd, hd, pha, vmagn, elong, rt, dt, gdt, rtt, dtt, gdtt;
	int ed;

	while(strcmp(str, nstr)!=0)
	{
		slovoG(str, slo, 1, 5);
		year = atoi(slo);
		slovoG(str, slo, 6, 8);
		mth = atoi(slo);
		slovoG(str, slo, 9, 20);
		numb = atof(slo);
		dat2JDalt2(&eJD, year, mth, numb);

		slovoG(str, s, 21, 22);
		slovoG(str, slo, 22, 24);
		year = atoi(slo);
		slovoG(str, slo, 25, 27);
		mth = atoi(slo);
		slovoG(str, slo, 28, 35);
		numb = atof(slo);
		
		sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
		hms2rad(&r, sign, year, mth, numb); 

		slovoG(str, s, 35, 36);
		slovoG(str, slo, 36, 38);
		year = atoi(slo);
		slovoG(str, slo, 39, 41);
		mth = atoi(slo);
		slovoG(str, slo, 42, 47);
		numb = atof(slo);

		sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
		gms2rad(&d, sign, year, mth, numb);
/*
		slovoG(str, slo, 50, 62);
		gd = atof(slo);

		slovoG(str, slo, 63, 70);
		hd = atof(slo);

		slovoG(str, slo, 71, 76);
		pha = atof(slo);

		slovoG(str, slo, 77, 83);
		vmagn = atof(slo);

		slovoG(str, slo, 84, 90);
		elong = atof(slo);

		slovoG(str, slo, 90, 91);
		ed = !strcmp(slo, "W");*/
/////////////////////////////////////////
		strcpy(nstr, str);
		fgets(str, STRLEN, FN);
/*		if(strcmp(str, nstr)==0)
		{
			delete [] s;
			delete [] slo;
			return 0;
		}
*/
		slovoG(str, slo, 0, 3);
		if(!streqv(slo, "   "))
		{
//			addApp(eJD, r, d, gd, hd, pha, vmagn, elong, ed, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			addApp(eJD, r, d, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			if(strcmp(str, nstr)==0)
			{
				delete [] s;
				delete [] slo;
				return 0;
			}
			continue;
		}

		this->iexp = 1;

		slovoG(str, s, 21, 22);
		slovoG(str, slo, 22, 24);
		year = atoi(slo);
		slovoG(str, slo, 25, 27);
		mth = atoi(slo);
		slovoG(str, slo, 28, 35);
		numb = atof(slo);

		sign = !(strcmp(s, " "))||(!strcmp(s, "+"));
		hms2rad(&rt, sign, year, mth, numb); 

		slovoG(str, s, 36, 37);
		slovoG(str, slo, 37, 39);
		year = atoi(slo);
		slovoG(str, slo, 40, 42);
		mth = atoi(slo);
		slovoG(str, slo, 43, 49);
		numb = atof(slo);

		sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
		gms2rad(&dt, sign, year, mth, numb);

		slovoG(str, slo, 50, 62);
		gdt = atof(slo);
////////////////////////////

		strcpy(nstr, str);
		fgets(str, STRLEN, FN);
/*
		if(strcmp(str, nstr)==0)
		{
			delete [] s;
			delete [] slo;
			return 0;
		}
*/
		slovoG(str, slo, 0, 3);
		if(!streqv(slo, "   "))
		{
			addApp(eJD, r, d, gd, hd, pha, vmagn, elong, ed, rt, dt, gdt, 0.0, 0.0, 0.0);
			if(strcmp(str, nstr)==0)
			{
				delete [] s;
				delete [] slo;
				return 0;
			}
			continue;
		}

		

		this->iexp = 2;

		slovoG(str, s, 21, 22);
		slovoG(str, slo, 22, 24);
		year = atoi(slo);
		slovoG(str, slo, 25, 27);
		mth = atoi(slo);
		slovoG(str, slo, 28, 35);
		numb = atof(slo);

		sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
		hms2rad(&rtt, sign, year, mth, numb); 

		slovoG(str, s, 36, 37);
		slovoG(str, slo, 37, 39);
		year = atoi(slo);
		slovoG(str, slo, 40, 42);
		mth = atoi(slo);
		slovoG(str, slo, 43, 49);
		numb = atof(slo);

		sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
		gms2rad(&dtt, sign, year, mth, numb);

		slovoG(str, slo, 50, 62);
		gdtt = atof(slo);
/*
		if(strcmp(str, nstr)==0)
		{
			delete [] s;
			delete [] slo;
			return 0;
		}
*/
		addApp(eJD, r, d, gd, hd, pha, vmagn, elong, ed, rt, dt, gdt, rtt, dtt, gdtt);

		if(strcmp(str, nstr)==0)
		{
			delete [] s;
			delete [] slo;
			return 0;
		}

		strcpy(nstr, str);
		fgets(str, STRLEN, FN);
	}

	delete [] s;
	delete [] slo;

	return 0;
}

int ast::getApp(app *n, int nofapp)
{
	if(n==NULL) return 1;
	apps *bin;

	bin = this->lapp;

	int i;

	i=0;

	while((i<nofapp)&&(bin!=NULL))
	{
//		bin = after;
		bin = bin->next;
		i++;
	}

	if(bin==NULL) return 1;

  	n->eJD = bin->pos->eJD;
	n->r = bin->pos->r;
	n->d = bin->pos->d;
	n->gd = bin->pos->gd;
	n->hd = bin->pos->hd;
	n->pha = bin->pos->pha;
	n->vmagn = bin->pos->vmagn;
	n->elong = bin->pos->elong;
	n->ed = bin->pos->ed;
	n->rt = bin->pos->rt;
	n->dt = bin->pos->dt;
	n->gdt = bin->pos->gdt;
	n->rtt = bin->pos->rtt;
	n->dtt = bin->pos->dtt;
	n->gdtt = bin->pos->gdtt;

	return 0;
}



int ast::writeApp(char *fname, int mode, int nofn)
{
//	FILE *FC;

	switch(mode)
	{
	case 0:
		this->FC = fopen(fname, "w");
		break;
	case 1:
		this->FC = fopen(fname, "r+");
		break;
	}

	if(this->FC==NULL)
	{
		printf("\nError open file for write REC\n");
		return 1;
	}

	char *str, *nstr;
	str = new char[STRLEN];
	nstr = new char[STRLEN];

	strcpy(str, "");
	strcpy(nstr, "");

	int i;

	switch(mode)
	{
	case 1:
		for(i=0; i<7; i++)
		{
			strcpy(nstr, str);
			fgets(str, 1000, this->FC);
//			strcat(this->hat, nstr);	//hat
		}
		do
		{
			strcpy(str, nstr);
			fgets(nstr, 1000, this->FC);
		}while(!strcmp(str, nstr)==0);
		break;
	case 0:
		fprintf(this->FC, "%s", this->hat);
		break;
	}

	int y, m;
	double d;
	app npos;

	for(i=0; i<this->napps; i++)
	{
		this->getApp(&npos, i);
		this->makeStrPos(&npos, str);

		fprintf(this->FC, "%s", str);
	}
//		printf("%s", str);

//	fclose(this->FC);
//	delete(this->FC);
//	this->FC=NULL;

	delete [] str;
	delete [] nstr;

	return 0;
}

int ast::hating()
{
	return 0;
}



////////////////////////////////////private

int ast::makeStrPos(app *pos, char *str)
{
	if(str==NULL) return 1;

	strcpy(str, "");

	char *str1, *str2, *str3, *zero, *s1, *s2, *s3;

	str1 = new char[STRLEN];
	str2 = new char[STRLEN];
	str3 = new char[STRLEN];
	zero = new char[16];
	s1 = new char[16];
	s2 = new char[16];
	s3 = new char[16];

	strcpy(str1, "");
	strcpy(str2, "");
	strcpy(str3, "");
	strcpy(zero, "");
	strcpy(s1, " ");
	strcpy(s2, " ");
	strcpy(s3, "E");
	

	int y, m;
	double d;

	int hour, min, grad, l;
	double sec, ll;
	int sign1, sign2;

	dat2YMD(pos->eJD, &y, &m, &d);
	rad2hms(pos->r, &sign1, &hour, &min, &sec);
	rad2gms(pos->d, &sign2, &grad, &l, &ll);

	if(m<10) strcpy(zero, "0");
	if(!sign1) strcpy(s1, "-");
	if(!sign2) strcpy(s2, "-");
	if(pos->ed) strcpy(s3, "W");

	sprintf(str1, "  %4d %s%d %10.7f %s%2d %2d %7.4f %s%2d %2d %6.3f %12.8f %7.3f %5.1f %6.1f %6.1f%s\0", y, zero, m, d, s1, hour, min, sec, s2, grad, l, ll, pos->gd, pos->hd, pos->pha, pos->vmagn, pos->elong, s3);

	strcpy(s1, " ");
	strcpy(s2, " ");

	rad2hms(pos->rt, &sign1, &hour, &min, &sec);
	rad2gms(pos->dt, &sign2, &grad, &l, &ll);

	if(!sign1) strcpy(s1, "-");
	if(!sign2) strcpy(s2, "-");


	sprintf(str2, "                     %s%2d %2d %7.4f %s%2d %2d %6.3f %12.8f\0", s1, hour, min, sec, s2, grad, l, ll, pos->gdt);

	strcpy(s1, " ");
	strcpy(s2, " ");

	rad2hms(pos->rtt, &sign1, &hour, &min, &sec);
	rad2gms(pos->dtt, &sign2, &grad, &l, &ll);

	if(!sign1) strcpy(s1, "-");
	if(!sign2) strcpy(s2, "-");


	sprintf(str3, "                     %s%2d %2d %7.4f %s%2d %2d %6.3f %12.8f\0", s1, hour, min, sec, s2, grad, l, ll, pos->gdtt);
	
	sprintf(str, "%s\n%s\n%s\n", str1, str2, str3);

	delete [] str1;
	delete [] str2;
	delete [] str3;
	delete [] zero;
	delete [] s1;
	delete [] s2;
	delete [] s3;

	return 0;
}
