#include "stdafx.h"
//#include "..\DAster\StdAfx.h"
//#include "..\DAster\DAster.h"




//#include "..\DAster\StdAfx.h"

//#include "stdafx.h"
//#include "WinLofo.h"

//#include "MainFrm.h"

/////////////////////////////////////

#include "rec.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

/*
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
*/


int detEthGelioCentr(note *Rec, char *fname)
{
//WARN
//
	int i;
	rec *earth;
	earth = new rec;
	earth->initFile(fname);
	earth->getNoteTime(Rec, Rec->eJD);
/*
	note *pre, *post;
	pre = new note;
	post = new note;

	i=0
	do
	{
		rec->
	}; i<rec->nrec; i++)
//
	Rec->x =  0.9918482875774;
	Rec->y =  0.1253176812641;
	Rec->z =  0.0543276390029;
	Rec->xt = -0.0026254118612;
	Rec->yt = 0.0155845649111;
	Rec->zt = 0.0067570837283;
/*
	Initialize_Ephemeris("bin2000.405");
	stateType *Earth;
	Earth = new stateType;
	Interpolate_State(Rec->eJD , 4 , Earth);
	Rec->x = Earth->Position[0];
	Rec->y = Earth->Position[1];
	Rec->z = Earth->Position[2];

	Rec->xt = Earth->Velocity[0];
	Rec->yt = Earth->Velocity[1];
	Rec->zt = Earth->Velocity[2];
*/

	return 0;
}

int detObsEthCentr(note *Obs)
{
//WARN!!!
	Obs->x = 0.0;
	Obs->y = 0.0;
	Obs->z = 0.0;

	Obs->xt = 0.0;
	Obs->yt = 0.0;
	Obs->zt = 0.0;

	Obs->xtt = 0.0;
	Obs->ytt = 0.0;
	Obs->ztt = 0.0;

	return 0;
}



////////////public

rec::rec()
{
	this->lspot = NULL;
	this->nrec = 0;
	this->hat = NULL;
}

rec::~rec()
{
	delete(this->hat);
	delete(this->FC);
	delete(this->lspot);

	this->lspot = NULL;
	this->hat = NULL;
}

int rec::addNote(double eJD, double x, double y, double z, double xt, double yt, double zt, double xtt, double ytt, double ztt, double R, double V, double Rt)
{
	notes *temp, *last;

	temp = this->lspot;

	while(this->lspot!=NULL)
	{
		last = this->lspot;
		this->lspot = this->lspot->next;
	}

	this->lspot = new notes;
	this->lspot->spot = new note;
	this->lspot->next = NULL;

	this->lspot->spot->eJD = eJD;
	this->lspot->spot->x = x;
	this->lspot->spot->y = y;
	this->lspot->spot->z = z;
	this->lspot->spot->xt = xt;
	this->lspot->spot->yt = yt;
	this->lspot->spot->zt = zt;
	this->lspot->spot->xtt = xtt;
	this->lspot->spot->ytt = ytt;
	this->lspot->spot->ztt = ztt;

	this->lspot->spot->R = R;
	this->lspot->spot->V = V;
	this->lspot->spot->Rt = Rt;

	if(temp!=NULL)
	{
		last->next = this->lspot;
		this->lspot = temp;
	}

	this->nrec++;

	return 0;
}

int rec::delNote(int num)
{
	if(num>=this->nrec) return 2;

	notes *before, *after;

	before = NULL;
	after = this->lspot;

	int i;

	i=0;
	while(i<num)
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
	else if(num==0) this->lspot = this->lspot->next;
		else return 1;

	this->nrec--;

	return 0;
}

int rec::initFile(char *fname)
{
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
	this->hat = new char[1000];
	strcpy(this->hat, "");
//

	for(i=0; i<8; i++)
	{
		strcpy(nstr, str);
		fgets(str, STRLEN, FN);
		strcat(this->hat, nstr);	//hat
	}

//	printf("%s", this->hat);
	
	int year, mth;
	double numb;
	char *slo;

	slo = new char[20];

	double eJD,  x,  y,  z,  xt,  yt,  zt,  xtt,  ytt,  ztt,  R,  V,  Rt;

	while(strcmp(str, nstr)!=0)
	{
		slovoG(str, slo, 0, 5);
		year = atoi(slo);
		slovoG(str, slo, 6, 8);
		mth = atoi(slo);
		slovoG(str, slo, 9, 19);
		numb = atof(slo);
		dat2JD(&eJD, year, mth, numb);

		slovoG(str, slo, 21, 37);
		x = atof(slo);
		slovoG(str, slo, 40, 56);
		y = atof(slo);
		slovoG(str, slo, 59, 75);
		z = atof(slo);

		strcpy(nstr, str);
		fgets(str, STRLEN, FN);

		slovoG(str, slo, 21, 37);
		xt = atof(slo);
		slovoG(str, slo, 40, 56);
		yt = atof(slo);
		slovoG(str, slo, 59, 75);
		zt = atof(slo);

		strcpy(nstr, str);
		fgets(str, STRLEN, FN);

		slovoG(str, slo, 21, 37);
		xtt = atof(slo);
		slovoG(str, slo, 40, 56);
		ytt = atof(slo);
		slovoG(str, slo, 59, 75);
		ztt = atof(slo);

		strcpy(nstr, str);
		fgets(str, STRLEN, FN);

		slovoG(str, slo, 21, 37);
		R = atof(slo);
		slovoG(str, slo, 40, 56);
		V = atof(slo);
		slovoG(str, slo, 59, 75);
		Rt = atof(slo);

		if(strcmp(str, nstr)==0) return 0;
		this->addNote(eJD,  x,  y,  z,  xt,  yt,  zt,  xtt,  ytt,  ztt,  R,  V,  Rt);

		strcpy(nstr, str);
		fgets(str, STRLEN, FN);



	}


	return 0;
}

int rec::getNote(note *n, int nofn)
{
	if(n==NULL) return 1;
	notes *bin;

	bin = this->lspot;

	int i;

	i=0;

	while((i<nofn)&&(bin!=NULL))
	{
//		bin = after;
		bin = bin->next;
		i++;
	}

	if(bin==NULL) return 1;

	n->eJD = bin->spot->eJD;
	n->x = bin->spot->x;
	n->y = bin->spot->y;
	n->z = bin->spot->z;
	n->xt = bin->spot->xt;
	n->yt = bin->spot->yt;
	n->zt = bin->spot->zt;
	n->xtt = bin->spot->xtt;
	n->ytt = bin->spot->ytt;
	n->ztt = bin->spot->ztt;

	n->R = bin->spot->R;
	n->V = bin->spot->V;
	n->Rt = bin->spot->Rt;

	return 0;
}



int rec::writeNote(char *fname, int mode, int nofn)
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
	str = new char[100];
	nstr = new char[100];

	strcpy(str, "");
	strcpy(nstr, "");

	int i;

	switch(mode)
	{
	case 1:
		for(i=0; i<8; i++)
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

//	int y, m;
//	double d;
	note nspot;

	for(i=0; i<this->nrec; i++)
	{
		this->getNote(&nspot, i);
		this->makeStrSpot(&nspot, str);

		fprintf(this->FC, "%s", str);
	}
//		printf("%s", str);

//	fclose(this->FC);
//	delete(this->FC);
//	this->FC=NULL;

	return 0;
}



////////////////////////////////////private

int rec::makeStrSpot(note *spot, char *str)
{
	if(str==NULL) return 1;

	strcpy(str, "");

	char *str1, *str2, *str3, *str4, *zero, *s1, *s2, *s3;

	str1 = new char[100];
	str2 = new char[100];
	str3 = new char[100];
	str4 = new char[100];
	zero = new char[1];
	s1 = new char[1];
	s2 = new char[1];
	s3 = new char[1];

	strcpy(str1, "");
	strcpy(str2, "");
	strcpy(str3, "");
	strcpy(str4, "");
	strcpy(zero, "");
	strcpy(s1, " ");
	strcpy(s2, " ");
	strcpy(s3, " ");
	

	int y, m;
	double d;

	dat2YMD(spot->eJD, &y, &m, &d);

	if(m<10) strcpy(zero, "0");
	if(sign(spot->x)==-1.0) strcpy(s1, "-");
	if(sign(spot->y)==-1.0) strcpy(s2, "-");
	if(sign(spot->z)==-1.0) strcpy(s3, "-");


	sprintf(str1, " %4d %s%d %10.7f  %s%15.13f   %s%15.13f   %s%15.13f\0", y, zero, m, d, s1, fabs(spot->x), s2, fabs(spot->y), s3, fabs(spot->z));

	strcpy(s1, " ");
	strcpy(s2, " ");
	strcpy(s3, " ");
	if(sign(spot->xt)==-1.0) strcpy(s1, "-");
	if(sign(spot->yt)==-1.0) strcpy(s2, "-");
	if(sign(spot->zt)==-1.0) strcpy(s3, "-");


	sprintf(str2, "                     %s%15.13f   %s%15.13f   %s%15.13f\0", s1, fabs(spot->xt), s2, fabs(spot->yt), s3, fabs(spot->zt));

	strcpy(s1, " ");
	strcpy(s2, " ");
	strcpy(s3, " ");
	if(sign(spot->xtt)==-1.0) strcpy(s1, "-");
	if(sign(spot->ytt)==-1.0) strcpy(s2, "-");
	if(sign(spot->ztt)==-1.0) strcpy(s3, "-");


	sprintf(str3, "                     %s%15.13f   %s%15.13f   %s%15.13f\0", s1, fabs(spot->xtt), s2, fabs(spot->ytt), s3, fabs(spot->ztt));

	strcpy(s1, " ");
	strcpy(s2, " ");
	strcpy(s3, " ");
	if(sign(spot->R)==-1.0) strcpy(s1, "-");
	if(sign(spot->V)==-1.0) strcpy(s2, "-");
	if(sign(spot->Rt)==-1.0) strcpy(s3, "-");


	sprintf(str4, "                     %s%15.13f   %s%14.5f    %s%14.5f\0", s1, fabs(spot->R), s2, fabs(spot->V), s3, fabs(spot->Rt));

	sprintf(str, "%s\n%s\n%s\n%s\n", str1, str2, str3, str4);

	return 0;
}

int rec::getNoteTime(note *n, double time)
{
	int itop, ibuttom, imid;

	note *pre, *post, *mid;
	pre = new note;
	post = new note;
	mid = new note;

	ibuttom = 0;
	itop = this->nrec-1;

	this->getNote(pre, ibuttom);
	this->getNote(post, itop);

	if((time<pre->eJD)||(time>post->eJD)) return 1;

	do
	{
		imid = (itop-ibuttom)/2 + ibuttom;
		this->getNote(mid, imid);

		if(time-mid->eJD>EPS) ibuttom = imid;
		else if(time-mid->eJD<-EPS) itop = imid;
		else if(fabs(time-mid->eJD)<EPS)
		{
			if(mid->copyTo(n)) return 1;
			return 0;
		}

		this->getNote(pre, ibuttom);
		this->getNote(post, itop);

	}while((itop-ibuttom)>1);

	n->eJD = time;
	n->interpNote(pre, post);


	return 0;
}
/*
note::note()
{
	this->eJD = 0.0;
	this->x = 0.0;
	this->y = 
}
~note();
*/
int note::copyTo(note *target)
{
	if(target==NULL) return 1;

	target->eJD = this->eJD;
	target->R = this->R;
	target->Rt = this->Rt;
	target->V = this->V;
	target->x = this->x;
	target->xt = this->xt;
	target->xtt = this->xtt;
	target->y = this->y;
	target->yt = this->yt; 
	target->ytt = this->ytt;
	target->z = this->z;
	target->zt = this->zt; 
	target->ztt = this->ztt;
	
	return 0;
}

int note::interpNote(note *nbuttom, note *ntop)
{
	if((nbuttom==NULL)||(ntop==NULL)) return 1;

	interp(nbuttom->R, &this->R, ntop->R, nbuttom->eJD, this->eJD, ntop->eJD);
	interp(nbuttom->Rt, &this->Rt, ntop->Rt, nbuttom->eJD, this->eJD, ntop->eJD);
	interp(nbuttom->V, &this->V, ntop->V, nbuttom->eJD, this->eJD, ntop->eJD);
	interp(nbuttom->x, &this->x, ntop->x, nbuttom->eJD, this->eJD, ntop->eJD);
	interp(nbuttom->xt, &this->xt, ntop->xt, nbuttom->eJD, this->eJD, ntop->eJD);
	interp(nbuttom->xtt, &this->xtt, ntop->xtt, nbuttom->eJD, this->eJD, ntop->eJD);
	interp(nbuttom->y, &this->y, ntop->y, nbuttom->eJD, this->eJD, ntop->eJD);
	interp(nbuttom->yt, &this->yt, ntop->yt, nbuttom->eJD, this->eJD, ntop->eJD);
	interp(nbuttom->ytt, &this->ytt, ntop->ytt, nbuttom->eJD, this->eJD, ntop->eJD);
	interp(nbuttom->z, &this->z, ntop->z, nbuttom->eJD, this->eJD, ntop->eJD);
	interp(nbuttom->zt, &this->zt, ntop->zt, nbuttom->eJD, this->eJD, ntop->eJD);
	interp(nbuttom->ztt, &this->ztt, ntop->ztt, nbuttom->eJD, this->eJD, ntop->eJD);

	return 0;
}