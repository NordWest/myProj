#ifndef MPCS_H
#define MPCS_H

#include <stdio.h>
#include "comfunc.h"
#include "coord.h"
#include "DynArr.h"
#include "filedynstr.h"

#define MPC_SIZE 81

class mpc_head
{
public:
	mpc_head();
	~mpc_head();

	char *Snum;		//nomer postoyannij perevedennyj

	char *SnumTemp;	//vremennyj nomer
	int numTemp;
	int numType;	//0-postoyannyj, 1-vremennyj

	char *ss1;		//spec simvol 1, ex *
	char *ss2;		//spec simvol 2, ex F

	char *tipOfObs;	//tip nabliudenij, naprimer CCD = "C"

	int get_head(char *head);
	int cut_head(char *head);

	int set_head(mpc_head *donor);

	int get_num();

        void set_typeOfObs(char *otype);
        void set_Snum(int num);
        void set_SnumTemp(int num);

};

class mpc_tail
{
public:
	mpc_tail();
	~mpc_tail();

	char *Smagn;
	char *bukva;	//bukva
	char *SnumOfPubl;//nomer publikacii
	char *SnumOfObservatory;

	int get_tail(char *tail);
	int cut_tail(char *tail);

	int set_tail(mpc_tail *donor);

        void set_numOfObs(char *noobs);
        void set_magn(double magn);
};

class mpc
{
public:
//	char *string;

	mpc_head *head;
	int num;

	double eJD;			//dolgno byt' vsemirnoe vremya nabl. UT1
        double r, d;		//Right       Declination   [rad]
	double r_oc, d_oc;	//O-C for RA & DEC

	double magn;		//Blesk
	int numOfPubl;
	int numOfObservatory;

	mpc_tail *tail;

	char *tnull;

	mpc();
	~mpc();

	int set_mpc(mpc *donor);
    void toString(char *arec);
};

class mpcL
{
public:
//	char *string;

	mpc_head *head;
	int num;

	double eJD;		//dolgno byt' vsemirnoe vremya nabl. UT1
	double r, d;	//Right       Declination

	double magn;		//Blesk
	int numOfPubl;
	int numOfObservatory;

	mpc_tail *tail;

	char *tnull;

	mpcL();
	~mpcL();

	int set_mpc(mpcL *donor);
};


class mpcs : public FileDynStrBin
{
public:

	mpc *record;

	int addmpc(mpc *nrec, int pos);
	int addmpc(mpc *nrec);
	int addmpc();
	int addmpc(int pos);
	int delmpc(int pos);
	int getmpc(int pos);

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
	mpcs();
	~mpcs();
};

class mpcsL : public FileDynStrBin
{
public:

	mpcL *record;

	int addmpc(mpcL *nrec, int pos);
	int addmpc(mpcL *nrec);
	int addmpc();
	int addmpc(int pos);
	int delmpc(int pos);
	int getmpc(int pos);

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
	mpcsL();
	~mpcsL();
};

#endif
