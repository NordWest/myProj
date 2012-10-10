//#pragma once
//#include "..\DAster\Eassistant\StdAfx.h"
//#include "..\DAster\DAster.h"

#include <QtCore>
#include "./../mb/mb.h"

//#include "..\DAster\MainFrm.h"
/*
//#ifndef DA_H
#include "DynArr.h"
//#endif
*/
#ifndef COMF_H
#include <stdio.h>
//#include <conio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
/*
#ifndef REC_H
#include "rec.h"
#endif*/
//#ifndef DELE_H
//#include "dele.h"
//#endif

#define BOWELL_ORIG 276
#define BOWELL_NEW 285
#define EPOS5_CAT 192

/*
#ifndef ORB_H
#include "orbit.h"
#endif
*/
#define AU_CONST 1
#define AUKM 1.49597870691000015e+8
#define AUSM 1.49597870691000015e+13
#define MS 1
#define SUN_MASS_GRAM 1.9891e+33
#define SUN_MASS_KG 1.9891e+30
#define VE 2979256.9529966
#define PI 3.141592653589
#define ka 0.017202098955
#define kaGRAD 0.98560767
#define kaGRAD1 0.071014884
#define kaSEC 3548.1876
#define Xi ka*sqrt((double)MS)
#define EKV 0.409092614174
//#define EKV 0.409092572
#define EPS 1.0e-8

#define SUN_MAGNITUDE -26.8

#define EPO 1900
#define JDEPO 2415020.5
#define SECINRAD 206264.806
#define SECINDAY 86400.0
#define TAIDUTC 32.0
#define T0J2000 2451545.0
#define CAUSEC 2.0039888134770902821e-3
#define CAU 173.144632685

#define STRLEN 1024
#define SIMPLE_STR 1024
#define FNLEN 255

#define MIU 0.002737909350795
#define VIU 0.0027304336

struct valacc
{
	double val;
	double acc;
};

//double det_average(myvector *x);

double str2JD(char *str);
double str2JD_time(char *str);

void JD2str(double JD, char *str);
void JD2str_time(double JD, char *str);

double partOfYear(int year, int mth, double day);


int dat2YMD(double Jday, int *year, int *mth, double *day);
void dat2YMD_time(double Jday, int *year, int *mth, int *day, int *hour, int *min, double *sec);
int dat2YMDalt(double Jday, int *juldat);
int YMD2JD(double *Jday, int ymd);

void day2HMS(double day, int *hour, int *min, double *sec);
void day2HMS(double day, int *iday, int *hour, int *min, double *sec);
void HMS2day(double *day, int hour, int min, double sec);



int dat2JD(double *Jday, int year, int mth, double day);
int dat2JD_time(double *Jday, int year, int mth, int day, int hour, int min, double sec);
//int dat2JDalt(double *Jday, int year, int mth, double day);
//int dat2JDalt2(double *Jday, int year, int mth, double day);

void dat2YMD_str(double JD, char *str, int acc=7);

int dat2day(double *day, int y, int m, double d, int epo);

void rad2hms_str(double rad, char *str);
void rad2gms_str(double rad, char *str);
void rad2gms_str1(double rad, char *str);

int installDirCut(char *installDir, char* str, char *strCutted);

double det_planet_H(int pl_num);

double det_L(double R, double ro, double r, double F);

void min2dhms(double min, int *d, int *h, int *m, int *s);

int slovoG(char *source, char *sres, int pos1, int pos2);
int slovo(char *str, char *slo, int posb, int *pose);
int slovo_stopsim(char *str, char *slo, char* stopsim, int posb, int *pose, int nmax);
int slovoDG(char *str, char *slo, int posb, int *pose);

void det_suff(char *fn, char *suff);

int dat2JDN(int year, int month, int day);

int isVes(int year);
double dinm(int mounth, int isves);

int sign(double x);
double round( double val, int signs );

int hms2rad(double *rad, int sign, int h, int m, double s);
void hms2rad(double *rad, char* str);
double hms2grad(int h, int m, double s);
double hms2rad(char* str);
int gms2rad(double *rad, int sign, int g, int m, double s);
double gms2grad(int plus, int g, int m, double s);
void gms2rad(double *rad, char *str);
double gms2rad(char *str);
int rad2hms(double rad, int *sign, int *h, int *m, double *s);
int rad2gms(double rad, int *sign, int *g, int *m, double *s);

double rad_to_mas(double angle);

int index(char *sInd, char *sIni, char *fil, int len);
int indexB(char *sInd, char *sIni, char *fil, int len);
char* del_end_space(char *str);

double fatan(double sinx, double cosx);

int rdsys(double *aV, double *dV, double P1, double P2, double P3);
int piNsys(double *p, double *inc, double *N, double P1, double P2, double P3);

int Ekv2Ecl(double *eclinc, double *eclNode, double *eclPer, double ekvinc, double ekvNode, double ekvPer);
int Ecl2Ekv(double *ekvinc, double *ekvNode, double *ekvPer, double eclinc, double eclNode, double eclPer);

int word2one(char *word, char *one);

int eKepler(double *E, double M, double ec);
int pKepler(double *s, double B);
int npKepler(double *s, double B, double ecc);
int gKepler(double *H, double M, double ec);

double npU(double ksi);
double npG(double g);
double npV(double ksi);

int sqEqv(double A, double B, double C, double *x1, double *x2);

int fakt(int n);

int str2int(char *str);
int int2str(char *str, int i);

void RotX(double *targ, double *sour, double angr);
void RotX(double *vect, double angr);

int detRiseSet(double *tr, double *ts, double de, double fi);

//Rotation against hour hand [XYZ]

//int RotZ(myvector *x, myvector *x0, double angr);
//int RotX(myvector *x, myvector *x0, double angr);

struct logstuff
{
//	int klog;
	FILE *FLOG;
	char *suff;

	logstuff();
	~logstuff();
};

int streqv(char *str1, char *str2);
int streqv1(char *str, char *strBase);

int chet(int val);

int UTC2TDT(double utc, double *tdt);

double dUT1();	//=UT1-UTC

double gmst0(double T);

int UTC2s(double utc, double lam, double *s);
int UTC2s_alt(double utc, double lam, double *s);

int s2UTC(double s, double lam, double *utc);
int sJD2UT1(double utcJDay, double s, double lam, double *utc);//[s]=day, [utc] = day

double UT0JD(double utJD, double *part_day = NULL);
//////////////////////////////////////////////////////////////
double gmst0(double T);
double pdayUT1(double jdUT1, double *pday);
double detT(double jd0);
double gmst0jd(double jd0);
double gmst1(double ut1, double g0);
void jdUT1_to_GMST1(double *gm1, double jdUT1);
void GMST1_to_jdUT1(double *jdUT1, double gm1, double jd0);
//////////////////////////////////////////////////////////////

double detLNode(double T);

double VEll(double A, double B, double C);

int redAber(double *x, double *y, double *z, double xt, double yt, double zt, double dt);

double det_r(double Qmax, double D);
double detPhase(double X, double Y, double Z, double x, double y, double z);
double detElong(double X, double Y, double Z, double x, double y, double z);
double det_m(double M, double r, double po, double c1, double F);
double detRfromM(double M, double A, double Sdist);

double VectAng(double *v1, double *v2);
double Smul3(double *V1, double *V2);
double norm(double *v);

void detABC(double *A, double *B, double *C, double R);

//double VectAng(myvector *V1, myvector *V2);

//int IntAlongCurv(myvector *C, myvector *A, myvector *B, double ca, double cb, int direct);
//int IntAlongCurv1(myvector *C, myvector *A, myvector *B, double ca, double cb);

double grad2rad(double grad);
double rad2grad(double rad);

double rad2mas(double rad);
double mas2rad(double mas);

int interp(double x1, double *x, double x2, double t1, double t, double t2);

int strnsep(char *str, int n, int nmax, char *sep, char *slo);

int findT(double *T, double M, double eJD, double q, double t);

double Tk(int k, double t);
double Uk(int k, double t);
double Rx(double t, int n, double *ak);
double Vx(double t, int n, double *ak);

int detRDnumGC(double *RA, double *DEC, double x, double y, double z, double xc, double yc, double zc, double xo, double yo, double zo);
int detAhnumGC(double *A, double *h, double s, double Cfi, double Sfi, double RA, double DEC);
//int detAhnumGC(double *A, double *h, double s, double RA, double DEC);

int packDigStr(char *pstr, char *fstr);
int upackDigStr(char *upstr, char *fstr);

int packString(QString upStr, QString *pStr, int targLen);
int unpackString(QString *upStr, QString pStr);


//int planet_num(char *pname);

//class bufStrRec;
//class bufStr;

//Fast Fourier Transform
void FFT(const double& dIn, const double *dOut, int dSz, int nn, int beginData);

void sortX(double *x, double *dx, int num);

struct procData
{
    QString name, folder;
    int waitTime;
};



class FileDynStr
{
public:
	char *fn, *fn_tmp;

//	bufStr *buffer;

	int nstr;
	int sizestr;

	char *str;

	FileDynStr();
	FileDynStr(int sizestr);
	FileDynStr(char *fn, int sizestr);
	~FileDynStr();
	int init(char* fn, int sizestr);

	int AddStr(char *nstr, int num);
	int DelStr(int num);
	int DelStr(char *pstr);
	int ReadStr(int num);
	int Exchange(int i, int j);
	int Update(char *nstr, int num);
	int Free();
};

class FileDynStrBin
{
public:
        QString fileName;

//	bufStr *buffer;

        //int nstr;
        //int sizestr;
        char *str;

        QStringList strList;

	FileDynStrBin();
	FileDynStrBin(int sizestr);
        FileDynStrBin(QString fn);
        FileDynStrBin(char *fn);
	~FileDynStrBin();
        int init(QString fn);
        int init(char *fn);

	int AddStr(char *nstr, int num);
	int DelStr(int num);
	int DelStr(char *pstr);
	int ReadStr(int num);
	int Exchange(int i, int j);
	int Update(char *nstr, int num);
	int Free();
        int nstr();

        int saveAs(QString fn);
        int save();
};


////////////////////////////////////////////////////
////////////////////////////////////////////////////

class Etop
{
public:
	int Ntask;
	char *name;
	double RA, DEC;
	double magn;
	double bonus;

	Etop();
	~Etop();
};

class MEtop
{
public:
	Etop *top;
	MEtop *next;

	MEtop();
	~MEtop();
};

class EtopList
{
public:
	MEtop *root;
	int num;

	EtopList();
	~EtopList();

	int addElem(Etop *npar);
	int remElem(int num);
	int clear();
};

///////////////////////////////////////////

struct Vparams
{
	double magn_min, magn_max;
	double RAL, RAR, DECU, DECB;
};

class MVpar
{
public:
	Vparams *param;
	MVpar *next;

	MVpar();
	~MVpar();
};

class VPList
{
public:
	MVpar *root;
	int num;

	VPList();
	~VPList();

	int addElem(Vparams *npar);
	int remElem(int num);
	int clear();
};
///////////////////////////////////////////////


int detCurf3(QString *resFunc, double *Zi, double *x, double *y, int num, int deg, double *We = NULL);
void summNum(double *x, double *dx, int num, int sNum);
double polyY(double *Zi, double x, int deg);
void mCoefFunc(QString coefFileName, QString mCoefFileName, QString dataSep, int coefDeg);


int initMdCoef(QString mdCorr, QList<double*> *mdCoef, int *mdCoefsz, QString colSep);
void detCoefM(QVector <double> *dCoef, QList <double*> mdCoef, int mdCoefsz, double mag);
int detDcorr(int corrModel, double x, double y, double *dx, double *dy, QVector <double> dCoefX, QVector <double> dCoefY);


/////////////////////////////////////////////////////////////////////
//orbit

////////////////////////////////////////////////////////////////////
#define COMF_H

#endif
