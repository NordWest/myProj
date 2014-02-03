#ifndef COMF_H
#define COMF_H

#include <QtCore>
#include <mb.h>

#ifndef ASTRO_H
#include <astro.h>
#endif

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define BOWELL_ORIG 276
#define BOWELL_NEW 285
#define EPOS5_CAT 192

#define AU_CONST 1
#define AUKM 1.49597870691000015e+8
#define AUSM 1.49597870691000015e+13
#define MS 1
#define SUN_MASS_GRAM 1.9891e+33
#define SUN_MASS_KG 1.9891e+30
#define SUN_MASS 1.0
#define VE 2979256.9529966
#define PI 3.141592653589793238462
#define ka 0.017202098955
#define kaAU 0.017202098955
#define kaGRAD 0.98560767
#define kaGRAD1 0.071014884
#define kaSEC 3548.1876
#define Xi ka*sqrt((double)MS)
#define EKV 0.409092804
#define EPS 1.0e-8

#define SUN_MAGNITUDE -26.8

#define EPO 1900
#define JDEPO 2415020.5
#define SECINRAD 206264.806
#define SECINDAY 86400.0
#define TAIDUTC 32.0
#define T0J2000 2451545.0
#define CAUSEC 2.0039888134770902821e-3
#define CAU 173.144632685                   //[AU/sec]

#define STRLEN 1024
#define SIMPLE_STR 1024
#define FNLEN 255

#define MIU 0.002737909350795
#define VIU 0.0027304336

#define XI1RAD 0.000001559
#define XI1GRAD 0.000088889

#define GKONST 6.67428E-11

struct valacc
{
	double val;
	double acc;
};

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

int rdsys(double *aVrad, double *dVrad, double P1, double P2, double P3);
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

int detRiseSet(double *tr, double *ts, double de, double fi);

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
/*  astro.h
double UTC2TDT(double jdUTC);
double TDB2TDT(double jdTDB);
double UTC2TDB(double jdUTC);
double TAImUTC(double mjd);
double TDT2UTC(double jdTDT);
double TDB2UTC(double jdTDB);
double TDB2TT(double jdTDB);
double TDT2TDB(double jdTDT);

double dUT1();	//=UT1-UTC
*/
//////////////////////////////////////////////////////////////

int UTC2s(double utc, double lam, double *s);
//int UTC2s_alt(double utc, double lam, double *s);

int s2UTC(double s, double lam, double *utc);
int sJD2UT1(double utcJDay, double s, double lam, double *utc);//[s]=day, [utc] = day

double UT0JD(double utJD, double *part_day = NULL);

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
void RotX(double *targ, double *sour, double angr);
void RotX(double *vect, double angr);
void Vmul3(double *vectRes, double *vect0, double *vect1);


void detABC(double *A, double *B, double *C, double R);
/*
double grad2rad(double grad);
double rad2grad(double rad);

double rad2mas(double rad);
double mas2rad(double mas);
*/
double grad2mas(double grad);
double mas2grad(double mas);


int interp(double x1, double *x, double x2, double t1, double t, double t2);

int strnsep(char *str, int n, int nmax, char *sep, char *slo);

int findT(double *T, double M, double eJD, double q, double t);

double Tk(int k, double t);
double Uk(int k, double t);
double Rx(double t, int n, double *ak);
double Vx(double t, int n, double *ak);

int detRDnumGC(double *RA, double *DEC, double x, double y, double z, double xc, double yc, double zc, double xo, double yo, double zo);
int detRDnumGC_vel(double *muRAcosD, double *muDEC, double x, double y, double z, double xc, double yc, double zc, double xo, double yo, double zo, double *range);

int detAhnumGC(double *A, double *h, double s, double Cfi, double Sfi, double RA, double DEC);
//int detAhnumGC(double *A, double *h, double s, double RA, double DEC);

int packDigStr(char *pstr, char *fstr);
int upackDigStr(char *upstr, char *fstr);

int packString(QString upStr, QString *pStr, int targLen);
int unpackString(QString *upStr, QString pStr);


int epm_planet_num(QString name);

//Fast Fourier Transform
void FFT(const double& dIn, const double *dOut, int dSz, int nn, int beginData);

void sortX(double *x, double *dx, int num);

void desc2NumName(QString desc, int *num, QString *name);

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





struct refractionParam
{
    double utc;                     //MJD
    double ra0, de0;
    double Fi;                      //rad
    double Long;                    //rad

    double temp;                   //temperature
    double press;                   //pressure
    double lam;
    double vi;


//    impRedec(double *ra, double *dec);
};

void redRefraction(double *ra, double *dec, refractionParam refParam);


/////////////////////////////////////////////////////////////////////
//orbit

////////////////////////////////////////////////////////////////////
//#define COMF_H
struct spkRecord
{
    double time, X[3], V[3];
    QString toString()
    {
        return(QString("%1, %2, %3, %4, %5, %6, %7").arg(time, 15, 'f', 8).arg(X[0], 18, 'g', 12).arg(X[1], 18, 'g', 12).arg(X[2], 18, 'g', 12).arg(V[0], 18, 'g', 12).arg(V[1], 18, 'g', 12).arg(V[2], 18, 'g', 12));
    };
    int fromString(QString inStr)
    {
        QStringList opers;
        opers = inStr.split(", ");
        if(opers.size()<7) return 1;
        time = opers[0].toDouble();
        X[0] = opers[1].toDouble();
        X[1] = opers[2].toDouble();
        X[2] = opers[3].toDouble();
        V[0] = opers[4].toDouble();
        V[1] = opers[5].toDouble();
        V[2] = opers[6].toDouble();
        return 0;
    };
};


int body_num(QString pname);


#endif
