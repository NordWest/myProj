#ifndef ORB_H
#define ORB_H

struct orbElem;
class orbit;
struct OrbitsParams;


#include "comfunc.h"
/*
#ifndef DA_H
#include "DynArr.h"
#endif
*/
//#include <iostream.h>
#include "observatory.h"
#include "orbcat.h"
#include "mpccat.h"
#include "commetorbcat.h"
#include <dele.h>
#include "rec.h"
#include "orbit.h"




//#define rs 1.000000000000e+9	//scaling radius [cm]

#define VAR_SIZE 100
#define V_VARS_LEN 4
#define D_VARS_LEN 6
#define S_VARS_LEN 24
#define OBJ_VARS_LEN 13
#define NAME_VARS_LEN 32



int findOrb(orbElem *orb, double *R0, double *V0, double t0);
//int findOrbEkv(orbElem *orb, double *R0ekv, double *V0ekv, double t0);

/*
class orbCat
{
public:
	orbCat();
	~orbCat();

	int writeCat(char *fname, int mode);

//	int index_number(int nnum);

//private:
	char* number;		//nomer
	char* epoha;		//эпоха [год]
	char* eJD;		//Юлианские дни с эпохи
	char* M0;		//средняя аномалия
	char* w;		//долгота перицентра
	char* Node;		//долгота восх узла
	char* ec;		//эксцентриситет
	char* inc;		//наклон орбиты
	char* q;		//перигелийное растояние
	char* H;		//точность
	char *G;		//X
	char *perturb;	//от чего считалась пертурбация 1000000000000000
	char* opp;		//число оппозиций
	char* obs;		//число наблюдений
	char* obsY;	//год наблюдений
	char* regY;	//год внесения в каталог
	char* un;		//neznaiu
	char *name;		//имя объекта + Автор
//	char *author;	//Автор
	char* makeD;		//Дата составления YYMMDD
//	char* diam;			//диаметр [km]


	int copyTo(orbCat* dest);

};
*/
struct orbElem
{
//public:
	double eJD;
	double M0;
	double q;
	double w;
	double Node;
    double ecc;
	double inc;

//	int set_from_cat(OrbCatE5 *ocat);
/*	int setUTC(double t);
	int setTDT(double t);
	int setTDB(double t);
*/
/*
	int detPolarOrb(double *r, double *v, double t);
	int detRecOrb(double *ksi, double *eta, double t);
	int detRecEcl(double *x, double *y, double *z, double t);
	int detRecEkv(double *x, double *y, double *z, double t);
*/	
	int isElips();
	int isPorabola();
	int isNPorabola();
	int isGiperbola();

    void get(mpcrec *mrec);
    void get(eposrec *erec);
    void set(eposrec *erec);
    void get(come5_rec *crec);
    void set(come5_rec *crec);


//	int findT(double *T, double M, double t);
//	int FindOrb(myvector *R0, myvector *V0, double t0, int log);	//nahozgdenie orbity po polozgeniju i skorosti v opred. moment vremeni

};


class orbit
{
public:
    orbElem *elem;      //[rad]
//	orbCat *line;

	orbit();
	orbit(int log);
//	orbit(char *fname, int nrec);
//	orbit(char *fname, int nrec, int log);

	~orbit();

//	int setElem(double eJD, double M0, double w, double N, double inc, double ecc, double q);

    void get(OrbCat *ocat);
    void get(mpccat *ocat);
    void get(eposrec *erec);
    void set(eposrec *erec);
    void set(OrbCat *ocat);

    void get(CommetOrbCat *ocat);
    void set(CommetOrbCat *ocat);

//	int setCat(char *number, char *epoha, char *eJD, char *M0, char *w, char *N, char *inc, char *ec, char *q, char *H, char *G, char *perturb, char *opp, char *obs, char *obsY, char *regY, char *un, char *name, char *makeD);
//	int setLCat(char *number, char *epoha, char *H, char *G, char *perturb, char *opp, char *obs, char *obsY, char *regY, char *un, char *name, char *makeD);

//	int indexElem();
//	int indexCat();

	int initElem(double eJD, double M0, double w, double N, double inc, double ecc, double q); //init by element of orbit with default rest parameters
//	int initFile(char *fname, int nrec);	//init nrec record from catalog file
//	int initStr(char *Sline);	//init orbit from string in catalog format

//	int writeCat(char *fname, int mode);	//write catalog format in file fname
//	int writeCStr(char *Sline);	//make string in catalog format ready for write in catalog file
//	int dispCat(char *sd);		//make string suit for display all parameters

	int detPolarOrb(double *r, double *v, double t);
	int detRecOrb(double *ksi, double *eta, double t);
	int detRecEcl(double *x, double *y, double *z, double t);
	int detRecEkv(double *x, double *y, double *z, double t);

	int detPolarOrbVel(double *Vr, double *Vn, double t);
        int detRecEclVel(double *Vx, double *Vy, double *Vz, double t);
        int detRecEkvVel(double *Vx, double *Vy, double *Vz, double t);

	double get_a();

//	int FindOrb(myvector *R0, myvector *V0, double t0, int log);	//nahozgdenie orbity po polozgeniju i skorosti v opred. moment vremeni

	int copyTo(orbit *dest);

private:

	int klog;
	logstuff *slog;
	int logging(char *basename);

};

/*
class orbs
{
public:
	orbs();
	~orbs();

	orbit *orb;
	orbs *next;
};
*/
struct OrbitsParams
{
//visual
	double lam, fi, h;
	double cosfi, sinfi;
	double s;

	observatory *st;

//common
	double UTC;
	double TDT;
	double TDB;

	note *Earth;
	note *Obs;

};

/*
class orbits
{
public:
	orbits();
	~orbits();

	orbit *curent;

	OrbitsParams *params;

	char *err_str;

	FILE *FCAT;


	int init(char *fn_cat, char *fn_obs, char *fn_earth);

	int set_observatory(int num);
	int set_observatory(char *name);

	int initFileCat(char *fname);

	int retime(double tUTC);

	char *fn_cat;

	int addorbStr(char *str);
	int getorb(int num);	//get copy of orbits in orbs with number "num"
	int getorb_name(char *oname);	//get copy of orbits in orbs with number "num"

	int norbs;

	int dispOneCat(char* str, int num);

	int detRDnumGC(double *RA, double *DEC, double tUTC, double x, double y, double z);
	int detAhnumGC(double *A, double *h, double tUTC, double lam, double fi, double RA, double DEC);

	int det_h();
	int det_ro();

	rec *earth;
};
*/

#endif
