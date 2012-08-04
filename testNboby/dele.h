//#pragma once
#ifndef COMF_H
#include "comfunc.h"
#endif

#ifndef COORD_H
#include "coord.h"
#endif

#ifndef DA_H
#include "DynArr.h"
#endif

#include "ephem_types.h"
#include "ephem_read.h"

#ifndef DELE_H

#define DE405_CAT 192

#define MERCURY_NUM 0
#define VENUS_NUM 1
#define EARTH_NUM 2
#define MARS_NUM 3
#define JUPITER_NUM 4
#define SATURN_NUM 5
#define URANUS_NUM 6
#define NEPTUNE_NUM 7
#define PLUTO_NUM 8
#define MOON_NUM 9
#define SUN_NUM 10
#define NUTATIONS_NUM 11
#define LIBRATIONS_NUM 12
#define GEOCENTR_NUM 13


#define CENTER_SUN 1
#define CENTER_BARY 0

#define SK_ECLIPTIC 1
#define SK_EKVATOR 0

#define SCALE_KMSEC 1.377534792908385547227E+6
#define SCALE_AUDAY 2.3737097724907275533844871479929e+9

int planet_num(char *pname);
double det_planet_H(int pl_num);


class group1010	//titul
{
public:
	char *eph_name;
	char *eph_begin;
	char *eph_end;

	group1010();
	~group1010();
};

class group1030
{
public:
        double eph_begin;
        double eph_end;
        double eph_step;
};

class rec1040
{
public:
	char *name;
	double value;
	

	rec1040();
	~rec1040();
};

class Mrec1040
{
public:
	rec1040 *top;
	Mrec1040 *next;

	Mrec1040();
	~Mrec1040();
};

class group1040
{
public:
	Mrec1040 *root;
	int num;

	group1040();
	~group1040();

	int addElem(rec1040 *npar);
	int remElem(int num);
	int getElem(rec1040 *nrec, int pos);
	int getElemByName(rec1040 *nrec, char *name);
	int clear();
};

class group1041
{
};

class group1050
{
public:
	mymatrix *coef;

	group1050();
	~group1050();
};

class group1070
{
public:
	group1070();
	group1070(char *fnbin);
	~group1070();
	int convert2bin(char *asc_name, char *bin_name);

	char *fname;

//	FileDynStrBin *fbin;

	double value;

//	int get(int nrecord, int nplan, int ngran);

};

class ch_coeff
{
};

struct part_head
{
	int npart;
	int ncoeff;
	double T0;
	double T1;
};

class dele
{
public:
        int KSize, Ncoeff;

	group1010 *g1010;
	group1030 *g1030;
	group1040 *g1040;
	group1040 *g1041;
	group1050 *g1050;
	group1070 *g1070;

	char *fname_h;
	char *fname_bin;

	dele();
        dele(const char *fname);
        dele(const char *fname_h, const char *fname_bin);
	~dele();

        int init_header(const char *fname_h);
        int init_jpl(const char *jpl_name);

        int init_jpl_bin(const char *jpl_bin_name);

	int detR(double *x, double *y, double *z, double Time, int nplanet, int proizv, int centr, int sk);
	int detR(double *x, double *y, double *z, double Time, char *planet, int proizv, int centr, int sk);
	int detRtt(double *x, double *y, double *z, double Time, int nplanet, int centr, int sk);

	/*
		proiz: 0 - положения, 1 - скорости
		centr: 0 - barycenter SS, 1 - geliocentr
		sk: 0 - ekvator, 1 - ectiptic
	*/

//	int detState(double *x, double *y, double *z, double *vx, double *vy, double *vz, double time, int nplanet);
//	int get_rec(int nplanet, part_head *ph, double *granul);

};


#define DELE_H
#endif
