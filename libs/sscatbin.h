//#pragma once
#ifndef SSCATBIN_H

#ifndef COMF_H
#include "comfunc.h"
#endif

#ifndef FBUFFER_H
#include "fBuffer.h"
#endif

#define SSCAT_REC 257

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class sscat_rec
{
public:
	sscat_rec();
	~sscat_rec();

	char *LPSM;		//[LPSM]
	char *LSPM;		//LSPM star name (1)
	char *LHS;		//? LHS catalogue (I/87) identification
	int NLTT;		//? NLTT catalogue (I/98) identification (2)
	int HIP;		//? Hipparcos catalog (I/239) identification
	int TYC1;		//? Tycho-2 catalog (I/259) identification 
	int TYC2;		//? Tycho-2 catalog (I/259) identification
	int TYC3;		//? Tycho-2 catalog (I/259) identification
	int ASCC25;		//? ASCC2.5 catalog (I/280) identification
	int UCAC2;		//? UCAC2 catalog (I/289) identification
	char *OName;	//Original name in astronomical literature (3)
	char *MASS2;	//2MASS catalog (II/246) identification
	char *USNOB1;	//USNO-B1.0 catalog (I/284) identification
	double RAdeg;	//Right Ascension in degrees (J2000, Ep=J2000)
	double DEdeg;	//Declination in degrees (J2000, Ep=J2000)
	double rPM;		//Total relative Proper Motion (4) 
	double rPMRA;	//Relative proper Motion in Right Ascension (4) 
	double rPMDE;	//Relative Proper Motion in Declination (4)
	double pm;		//Total Proper Motion (5)
	double pmRA;	//Proper Motion in Right Ascension (5)
	double pmDE;	//Proper Motion in Declination (5)
	char *Aflag;	//[ASTO ] Astrometric Flag (6)
	double Bmag;	//?=99.99 Optical B magnitude (7)
	double Vmag;	//?=99.99 Optical V magnitude (7)
	double BJmag;	//?=99.9 Photographic Blue (IIIaJ) magnitude (8)
	double RFmag;	//?=99.9 Photographic Red (IIIaF) magnitude (8)
	double INmag;	//?=99.9 Photographic Infrared (IVN) mag. (8)
	double Jmag;	//?=99.99 Infrared J magnitude (9)
	double Hmag;	//?=99.99 Infrared H magnitude (9)
	double Kmag;	//?=99.99 Infrared K_s_ magnitude (9)
	double Vemag;	//?=99.9 Estimated V magnitude (10)
	double VJ;		//?=99.9 Estimated V-J color (10) 

	void copyTo(sscat_rec *nrec);
};

class sscatbin
{
public:
	sscat_rec *record;

	sscatbin();
	~sscatbin();

	char *fname, *fname_tmp;
	int nstr;

	int init(char *fname);

//	void s2rec(char *str_in);
//	void rec2s(char *str_out);

	int GetRec(int pos);
	int GetRecNameLSPM(char *name);
	/*
	int GetRecName(char *name);
	int GetRecNum(int num);*/
	int AddRec(int pos);
	int AddRec();

};

#define SSCATBIN_H
#endif
