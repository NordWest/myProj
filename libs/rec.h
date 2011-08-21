#include <stdio.h>

#ifndef COMF_H
#include "comfunc.h"
#endif

#ifndef REC_H

class note
{
public:
	double eJD;

	double x, y, z;
	double xt, yt, zt;
	double xtt, ytt, ztt;
	double R, V, Rt;

	note(){};
	~note(){};

	int copyTo(note *target);
	int interpNote(note *nbuttom, note *ntop);
};


struct notes
{
	note *spot;
	notes *next;
};

class rec
{
public:
	notes *lspot;
	char *hat;
	int nrec;
	int ndot;
	FILE *FC;

	rec();
	~rec();

	int addNote(double eJD, double x, double y, double z, double xt, double yt, double zt, double xtt, double ytt, double ztt, double R, double V, double Rt);
//	int addNote(double eJD, double x, double y, double z, double xt, double yt, double zt, double xtt, double ytt, double ztt, double R, double V, double Rt);
//	int addNote(double eJD, double x, double y, double z, double xt, double yt, double zt, double xtt, double ytt, double ztt, double R, double V, double Rt);
	int delNote(int num);

	int initFile(char *fname);

	int getNote(note *n, int nofn);
	int getNoteTime(note *n, double time);

	int writeNote(char *fname, int mode, int nofn);	//mode=0 new file, mode=1 add old file

private:

	int makeStrSpot(note *spot, char *str);
};

int detEthGelioCentr(note *Rec, char *fname);
int detObsEthCentr(note *Obs);

#define REC_H
#endif

