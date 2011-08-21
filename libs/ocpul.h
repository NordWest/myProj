#ifndef OCPUL_H

#ifndef COMF_H
#include "comfunc.h"
#endif

#define OCPUL_REC 160

#ifndef COORD_H
#include "coord.h"
#endif

class ocpul_head
{
public:
	ocpul_head();
	~ocpul_head();

	char *Snum;		//nomer postoyannij perevedennyj

	char *SnumTemp;	//vremennyj nomer
	int numTemp;
	int numType;	//0-postoyannyj, 1-vremennyj

	char *ss1;		//spec simvol 1, ex *
	char *ss2;		//spec simvol 2, ex F

	char *tipOfObs;	//tip nabliudenij, naprimer CCD = "C"

	int get_head(char *head);
	int cut_head(char *head);

	int set_head(ocpul_head *donor);

	int get_num();


};

class ocpul_tail
{
public:
	ocpul_tail();
	~ocpul_tail();

	char *Smagn;
	char *bukva;	//bukva
	char *SnumOfPubl;//nomer publikacii
	char *SnumOfObservatory;

	int get_tail(char *tail);
	int cut_tail(char *tail);

	int set_tail(ocpul_tail *donor);
};

class ocpul
{
public:
//	char *string;
//	ocpul_head *head;
	char *name;
	int num;

	double eJD;		//dolgno byt' vsemirnoe vremya nabl. UT1
	double r, d;	//Right       Declination
	double r_oc, d_oc;	//Right       Declination



	double epoch;		//Blesk

	char *catalog;
	char *coorg_type;
	char *instrument;
	char *rec_type;

	char *tnull;

	ocpul();
	~ocpul();

	int set_ocpul(ocpul *donor);
};

class ocpulL
{
public:
//	char *string;

	ocpul_head *head;
	int num;

	double eJD;		//dolgno byt' vsemirnoe vremya nabl. UT1
	double r, d;	//Right       Declination

	double magn;		//Blesk
	int numOfPubl;
	int numOfObservatory;

	ocpul_tail *tail;

	char *tnull;

	ocpulL();
	~ocpulL();

	int set_ocpul(ocpulL *donor);
};


class ocpuls : public FileDynStr
{
public:

	ocpul *record;

/*	int add(ocpul *nrec, int pos);
	int add(ocpul *nrec);
	int add();
	int add(int pos);
	int del(int pos);*/
	int get(int pos);

	void rec2s(char *arec);
	void s2rec(char *arec);

	int init(char *fname);
	int init(char *fname, int sizestr);

//	int average(double *vra, double *vdec);

	int GetCoordList(List4 *coord);
/*
	int selbynum(int num);
	int selbynumT(int numT);
	int selbytime(double time, double dtime);
	int Uselbynum(int num);
	int UselbynumT(int numT);
	int Uselbytime(double time, double dtime);
*/
	ocpuls();
	~ocpuls();
};

class ocpulsL : public FileDynStrBin
{
public:

	ocpulL *record;

	int add(ocpulL *nrec, int pos);
	int add(ocpulL *nrec);
	int add();
	int add(int pos);
	int del(int pos);
	int get(int pos);

	void rec2s(char *arec);
	void s2rec(char *arec);

	int init(char *fname);
	int init(char *fname, int sizestr);

	int average(double *vra, double *vdec);

	int GetCoordList(List4 *coord);
/*
	int selbynum(int num);
	int selbynumT(int numT);
	int selbytime(double time, double dtime);
	int Uselbynum(int num);
	int UselbynumT(int numT);
	int Uselbytime(double time, double dtime);
*/
	ocpulsL();
	~ocpulsL();
};
/*
class oc_stat
{
public:
//	char *string;
//	ocpul_head *head;
	char *name;
	int num;

	double eJD;		//dolgno byt' vsemirnoe vremya nabl. UT1
	double r, d;	//Right       Declination
	double magn;	//Magnitude



//	double epoch;		//Blesk

	char *catalog;
	char *coorg_type;
	char *instrument;
	char *rec_type;

	char *tnull;

	ocpul();
	~ocpul();

	int set_ocpul(ocpul *donor);
};
*/

#define SSCAT_H
#endif
