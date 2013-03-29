#include "stdafx.h"
//#include "..\DAster\StdAfx.h"
//#include "..\DAster\DAster.h"

#include "observ.h"

//#include "comfunc.h"
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
//#define EKV 0.4093197552027299
/
#define EPO 1900
#define JDEPO 2415020.5
*/

////////////////////////////////////observ

////////////public

int observ::initObservatory(char *fname_obs)
{
	this->obs->init(fname_obs, OBS_SIZE);

	if(this->obs->nstr==0) return 1;

	return 0;
}


observ::observ(char *dele_header)
{
	this->obs = new observatory();
	this->place = new dele(dele_header);

//	printf("ini complete\n");
    useSpice = 0;

	mass = new double[20];
}

observ::observ()
{
//	printf("initialization\n");
	this->obs = new observatory();
	this->place = new dele();

//	printf("ini complete\n");

    useSpice = 0;
	mass = new double[20];
}

observ::~observ()
{
	delete(this->obs);
	delete(this->place);
}
/*
int observ::initDELE(char *fname_dele_h, char *fname_dele)
{
        //if(this->place->init_header(fname_dele_h)) return 1;
        //if(this->place->init_jpl_bin(fname_dele)) return 2;
        if(this->place->init(fname_dele)) return 2;
        //rec1040 rc;
	int i;
	for(i=8; i<19; i++)
	{
                //this->place->g1041->getElem(&rc, i);
                mass[i-8] = this->place->H2.data.constValue[i];//rc.value;
	}



	return 0;
}
*/
int observ::initDELE(char *fname_dele)
{
        //if(this->place->init_header(fname_dele_h)) return 1;
        if(this->place->init(fname_dele)) return 2;
        //rec1040 rc;
        int i;
        for(i=8; i<19; i++)
        {
                //this->place->g1041->getElem(&rc, i);
                mass[i-8] = this->place->H2.data.constValue[i];//rc.value;
        }



        return 0;
}

int observ::initSPICE(QString bspName, QString lskName)
{
    furnsh_c ( lskName.toAscii().data() );     //load LSK kernel
    furnsh_c ( bspName.toAscii().data()  );     //load SPK/BSP kernel with planets ephemerides
    useSpice = 1;
    return 0;
}


int observ::init(char *fname_obs, char *fname_dele_bin)
{
//	printf("init\n");
        if(this->initObservatory(fname_obs)) return 1;
        if(this->initDELE(fname_dele_bin)) return 2;

//	printf("init complete\n");
        return 0;
}

int observ::init(QString fname_obs, QString bspName, QString lskName)
{
//	printf("init\n");
        if(this->initObservatory(fname_obs.toAscii().data())) return 1;
        if(this->initSPICE(bspName, lskName)) return 2;

//	printf("init complete\n");
        return 0;
}
/*
int observ::init(char *fname_obs, char *fname_dele_h, char *fname_dele_bin)
{
//	printf("init\n");
	if(this->initObservatory(fname_obs)) return 1;
	if(this->initDELE(fname_dele_h, fname_dele_bin)) return 2;

//	printf("init complete\n");
	return 0;
}
*/

int observ::set_obs_parpam(int nplanet, int center, int sk, char *nobsy)
{
//	printf("set obs param\n");
	this->nplanet = nplanet;
	this->center = center;
	this->nobsy = nobsy;
	this->sk = sk;

//	printf("end set obs param\n");
    return(this->obs->getobsynumO(nobsy));

//	return 0;
}

int observ::set_spice_parpam(QString obs_name, QString obsy_code, QString center_name, QString ref_name)
{
//	printf("set obs param\n");
    obsName = obs_name;
    obsyCode =obsy_code;
    centerName = center_name;
    refName = ref_name;

    return(this->obs->getobsynumO(obsyCode.toAscii().data()));

//	return 0;
}

void observ::setUTC(double tUTC)
{
    ctime.setUTC(tUTC);
}

void observ::setTDB(double tTDB)
{
    ctime.setTDB(tTDB);
}


int observ::det_observ()
{
    this->obs->det_state(ctime.UTC());

    if(useSpice)
    {
        QString sJD;
        double et;
        SpiceDouble             state [6];
        SpiceDouble             lt;
        sJD = QString("%1 JD").arg(ctime.TDB(), 16, 'f',8);
        str2et_c(sJD.toAscii().data(), &et);
        spkezr_c (obsName.toAscii().data(), et, refName.toAscii().data(), "NONE", centerName.toAscii().data(), state, &lt );
        ox = state[0]/AUKM;
        oy = state[1]/AUKM;
        oz = state[2]/AUKM;
        ovx = state[3]/AUKM;
        ovy = state[4]/AUKM;
        ovz = state[5]/AUKM;

    }
    else
    {

        if(this->place->detState(&this->ox, &this->oy, &this->oz, &this->ovx, &this->ovy, &this->ovz, ctime.TDB(), this->nplanet, this->center, sk)) return 1;

        if(this->place->detRtt(&this->ovxt, &this->ovyt, &this->ovzt, ctime.TDB(), this->nplanet, this->center, sk)) return 3;
    }

    X[0] = ox+obs->dcx;
    X[1] = oy+obs->dcy;
    X[2] = oz+obs->dcz;

    V[0] = ovx+obs->vx;
    V[1] = ovy+obs->vy;
    V[2] = ovz+obs->vz;

/*
	this->ovxt = -(ka*ka*this->ox)/(fabs(pow(this->ox, 3.0)));
	this->ovyt = -(ka*ka*this->oy)/(fabs(pow(this->oy, 3.0)));
	this->ovzt = -(ka*ka*this->oz)/(pow(fabs(this->oz), 3.0));
*/
	return 0;
}

int observ::detSunRADEC(double *raS, double *decS)
{
    double Xs, Ys, Zs;
    if(this->place->detR(&Xs, &Ys, &Zs, ctime.TDB(), SUN_NUM, 0, this->center, sk)) return 1;
    rdsys(raS, decS, Xs-ox, Ys-oy, Zs-oz);
	return 0;
}


/*
int observ::detFkey(char *fname)
{
	int len;
	char *ext;
	ext = new char[3];
	len = strlen(fname);
	strcpy(ext, &fname[len-3]);
	if(streqv(ext, "mpc")||streqv(ext, "MPC")) this->Fkey = 0;
	else if(streqv(ext, "gmt")||streqv(ext, "GMT")) this->Fkey = 1;
	else if(streqv(ext, "ast")||streqv(ext, "AST")) this->Fkey = 2;
	else
	{
		printf("\nerror ext\n");
		return 1;
	}

	return 0;
}






int observ::initFile(char *fname, char *fnobs)
{
	if(this->detFkey(fname)) return 1;
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

	int year, mth;
	int sign;
	double numb;
	char *slo, *s;

	char *head, *tail;

	double eJD, r, d;

	slo = new char[20];
	s = new char[1];

	switch(this->Fkey)
	{
	case 0:
		head = new char[14];
		tail = new char[25];
		strcpy(nstr, str);
		fgets(str, STRLEN, FN);

		while(strcmp(str, nstr)!=0)
		{
			//
			slovoG(str, slo, 0, 15);
			strcpy(head, slo);
			strcpy(&head[15], "\0");

			slovoG(str, slo, 15, 19);
			year = atoi(slo);
			slovoG(str, slo, 20, 22);
			mth = atoi(slo);
			slovoG(str, slo, 23, 31);
			numb = atof(slo);
			dat2JD(&eJD, year, mth, numb);

			slovoG(str, s, 31, 32);
			slovoG(str, slo, 32, 34);
			year = atoi(slo);
			slovoG(str, slo, 35, 37);
			mth = atoi(slo);
			slovoG(str, slo, 38, 43);
			numb = atof(slo);

			sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
			hms2rad(&r, sign, year, mth, numb);		

			slovoG(str, s, 44, 45);
			slovoG(str, slo, 45, 47);
			year = atoi(slo);
			slovoG(str, slo, 48, 50);
			mth = atoi(slo);
			slovoG(str, slo, 51, 55);
			numb = atof(slo);

			sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
			gms2rad(&d, sign, year, mth, numb);

			slovoG(str, slo, 55, 80);
			strcpy(tail, slo);
			strcpy(&tail[26], "\0");

			strcpy(nstr, str);
			fgets(str, STRLEN, FN);
			//

			if(strcmp(str, nstr)==0) return 0;
			addmpc(head, eJD, r, d, tail);
//			if(streqv(str, "")) return 0;
//			if(strcmp(str, nstr)==0) return 0;

		}
		break;
	case 1:
		this->hat = new char[1000];
		strcpy(this->hat, "");
		for(i=0; i<6; i++)
		{
			strcpy(nstr, str);
			fgets(str, STRLEN, FN);
			strcat(this->hat, str);
		}
		strcpy(nstr, str);
		fgets(str, STRLEN, FN);

		head = new char[2];
		tail = new char[42];

		while(strcmp(str, nstr)!=0)
		{
			slovoG(str, slo, 0, 2);
			strcpy(head, slo);
			strcpy(&head[3], "\0");

			slovoG(str, slo, 2, 6);
			year = atoi(slo);
			slovoG(str, slo, 7, 9);
			mth = atoi(slo);
			slovoG(str, slo, 10, 20);
			numb = atof(slo);
			dat2JD(&eJD, year, mth, numb);

			slovoG(str, s, 21, 22);
			slovoG(str, slo, 22, 24);
			year = atoi(slo);
			slovoG(str, slo, 25, 27);
			mth = atoi(slo);
			slovoG(str, slo, 28, 35);
			numb = atof(slo);

			sign = (streqv(s, " "))||(streqv(s, "+"));
			hms2rad(&r, sign, year, mth, numb); 

			slovoG(str, s, 36, 37);
			slovoG(str, slo, 37, 39);
			year = atoi(slo);
			slovoG(str, slo, 40, 42);
			mth = atoi(slo);
			slovoG(str, slo, 43, 49);
			numb = atof(slo);

			sign = (streqv(s, " "))||(streqv(s, "+"));
			gms2rad(&d, sign, year, mth, numb);

			slovoG(str, slo, 49, 91);
			strcpy(tail, slo);
			strcpy(&tail[42], "\0");

			strcpy(nstr, str);
			fgets(str, STRLEN, FN);	

			if(strcmp(str, nstr)==0) return 0;
			addmpc(head, eJD, r, d, tail);
		}
		break;
	case 2:
		this->hat = new char[1000];
		strcpy(this->hat, "");
		for(i=0; i<6; i++)
		{
			strcpy(nstr, str);
			fgets(str, STRLEN, FN);
			strcat(this->hat, str);
		}
		strcpy(nstr, str);
		fgets(str, STRLEN, FN);

		head = new char[2];
		tail = new char[42];

		while(strcmp(str, nstr)!=0)
		{
			slovoG(str, slo, 0, 2);
			strcpy(head, slo);
			strcpy(&head[3], "\0");

			slovoG(str, slo, 2, 6);
			year = atoi(slo);
			slovoG(str, slo, 7, 9);
			mth = atoi(slo);
			slovoG(str, slo, 10, 20);
			numb = atof(slo);
			dat2JD(&eJD, year, mth, numb);

			slovoG(str, s, 21, 22);
			slovoG(str, slo, 22, 24);
			year = atoi(slo);
			slovoG(str, slo, 25, 27);
			mth = atoi(slo);
			slovoG(str, slo, 28, 35);
			numb = atof(slo);

			sign = (streqv(s, " "))||(streqv(s, "+"));
			hms2rad(&r, sign, year, mth, numb); 

			slovoG(str, s, 36, 37);
			slovoG(str, slo, 37, 39);
			year = atoi(slo);
			slovoG(str, slo, 40, 42);
			mth = atoi(slo);
			slovoG(str, slo, 43, 49);
			numb = atof(slo);

			sign = (streqv(s, " "))||(streqv(s, "+"));
			gms2rad(&d, sign, year, mth, numb);

			slovoG(str, slo, 49, 91);
			strcpy(tail, slo);
			strcpy(&tail[42], "\0");

			strcpy(nstr, str);
			fgets(str, STRLEN, FN);	

			if(strcmp(str, nstr)==0) return 0;
			addmpc(head, eJD, r, d, tail);
		}
		break;
	}

	if(fnobs!=NULL)
	{
		this->obs = new observatory();
		this->obs->initFile(fnobs);
	}

	delete(slo);
	delete(s);
//	delete(sign);
	delete(str);
	delete(nstr);
	fclose(FN);
	return 0;
}


int observ::initObs(observ *eobs)
{

	int i;
	mpc *temp;
	temp = new mpc(eobs->Fkey);

	for(i=0; i<eobs->nmpcs; i++)
	{
		eobs->getmpc(temp, i);
		this->addmpc(temp->head, temp->eJD, temp->r, temp->d, temp->tail);
//		temp = this->getlmpc(i);
//		temp->headcut();
	}


	return 0;
}


int observ::getmpc(mpc *n, int nofmpc)
{
	if(n==NULL) return 1;
	if(nofmpc>this->nmpcs-1) return 1;
	mpcs *bin;

	bin = this->lmpc;

	int i;

	i=0;

	while((i<nofmpc)&&(bin!=NULL))
	{
		bin = bin->next;
		i++;
	}

	if(bin==NULL) return 1;

	strcpy(n->head, bin->pos->head);
//	strcpy(&n->head[15], "\0");
	n->eJD = bin->pos->eJD;
	n->r = bin->pos->r;
	n->d = bin->pos->d;
	strcpy(n->tail, bin->pos->tail);
//	strcpy(&n->tail[25], "\0");

	return 0;
}

mpc* observ::getlmpc(int nofmpc)
{
//	if(n==NULL) return 1;
	if(nofmpc>this->nmpcs) return NULL;
	mpcs *bin;

	bin = this->lmpc;

	int i;

	i=0;

	while((i<nofmpc)&&(bin!=NULL))
	{
		bin = bin->next;
		i++;
	}

	return bin->pos;

/*	if(bin==NULL) return 1;

	strcpy(n->head, bin->pos->head);
	strcpy(&n->head[15], "\0");
	n->eJD = bin->pos->eJD;
	n->r = bin->pos->r;
	n->d = bin->pos->d;
	strcpy(n->tail, bin->pos->tail);
	strcpy(&n->tail[25], "\0");
/
//	return 0;
}

int observ::writeMpc(char *fname, int mode, int nofn)
{
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
	str = new char[1000];
	nstr = new char[1000];

	strcpy(str, "");
	strcpy(nstr, "");

	int i;

	int y, m;
	double d;
	mpc *npos;
	npos = new mpc(this->Fkey);

//	npos.head = new char[15];
//	npos.tail = new char[25];	

	switch(this->Fkey)
	{
	case 0:
		for(i=0; i<this->nmpcs; i++)
		{
			this->getmpc(npos, i);
			this->makeStrPos(npos, str);

			fprintf(this->FC, "%s", str);
		}
		break;
	case 1:
		fprintf(this->FC, "%s", this->hat);
		for(i=0; i<this->nmpcs; i++)
		{
			this->getmpc(npos, i);
			this->makeStrPosGMT(npos, str);

			fprintf(this->FC, "%s", str);
		}
		break;
	}
		fclose(this->FC);
//	delete(str);
//	delete(nstr);
//	delete(npos);

	return 0;
}



////////////////////////////////////private

int observ::makeStrPos(mpc *pos, char *str)
{
	if(str==NULL) return 1;

	strcpy(str, "");

	char *zero1, *zero2, *zero3, *zero4, *zero5, *s2;

	zero1 = new char[1];
	zero2 = new char[1];
	zero3 = new char[1];
	zero4 = new char[1];
	zero5 = new char[1];

	s2 = new char[1];

	strcpy(zero1, "");
	strcpy(zero2, "");
	strcpy(zero3, "");
	strcpy(zero4, "");
	strcpy(zero5, "");
	strcpy(s2, "+");

	

	int y, m;
	double d;

	int hour, min, grad, l;
	double sec, ll;
	int sign1, sign2;

	dat2YMD(pos->eJD, &y, &m, &d);
	rad2hms(pos->r, &sign1, &hour, &min, &sec);
	rad2gms(pos->d, &sign2, &grad, &l, &ll);

	if(m<10) strcpy(zero1, "0");
	if(hour<10) strcpy(zero2, "0");
	if(min<10) strcpy(zero3, "0");
	if(grad<10) strcpy(zero4, "0");
	if(l<10) strcpy(zero5, "0");

	if(!sign2) strcpy(s2, "-");


	sprintf(str, "%s%4d %s%d %8.5f %s%d %s%d %5.2f %s%s%d %s%d %4.1f%s\n", pos->head, y, zero1, m, d, zero2, hour, zero3, min, sec, s2, zero4, grad, zero5, l, ll, pos->tail);
/*
	delete(zero1);
	delete(zero2);
	delete(zero3);
	delete(zero4);
	delete(zero5);
	delete(s2);
/
	return 0;
}

int observ::makeStrPosGMT(mpc *pos, char *str)
{
	if(str==NULL) return 1;

	strcpy(str, "");

	char *zero1, *zero2, *zero3, *zero4, *zero5, *s2;

	zero1 = new char[1];
	zero2 = new char[1];
	zero3 = new char[1];
	zero4 = new char[1];
	zero5 = new char[1];

	s2 = new char[1];

	strcpy(zero1, "");
	strcpy(zero2, "");
	strcpy(zero3, "");
	strcpy(zero4, "");
	strcpy(zero5, "");
	strcpy(s2, "+");

	

	int y, m;
	double d;

	int hour, min, grad, l;
	double sec, ll;
	int sign1, sign2;

	dat2YMD(pos->eJD, &y, &m, &d);
	rad2hms(pos->r, &sign1, &hour, &min, &sec);
	rad2gms(pos->d, &sign2, &grad, &l, &ll);

	if(m<10) strcpy(zero1, "0");
	if(hour<10) strcpy(zero2, "0");
	if(min<10) strcpy(zero3, "0");
	if(grad<10) strcpy(zero4, "0");
	if(l<10) strcpy(zero5, "0");

	if(!sign2) strcpy(s2, "-");


	sprintf(str, "%s%4d %s%d %10.7f %s%d %s%d %7.4f %s%s%d %s%d %7.4f%s\n", pos->head, y, zero1, m, d, zero2, hour, zero3, min, sec, s2, zero4, grad, zero5, l, ll, pos->tail);
/*
	delete(zero1);
	delete(zero2);
	delete(zero3);
	delete(zero4);
	delete(zero5);
	delete(s2);
/
	return 0;
}

int observ::topo2geo(mpc *pos, double SX, double SY, double SZ, double po)
{
	double s;

//	s = mpc


	return 0;
}
*/
