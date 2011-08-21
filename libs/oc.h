#ifndef ORB_H
#include "orbit.h"
#endif

#ifndef DELE_H
#include "dele.h"
#endif

#ifndef COMF_H
#include "comfunc.h"
#endif

#ifndef OBSERV_H
#include "observ.h"
#endif

#ifndef COORD_H
#include "coord.h"
#endif

#ifndef OC_H

#define OC_FILE_REC_SIZE 135

class oc_file_rec
{
public:
	
	void set(double JD, double r0, double  d0, double r1, double d1);
	void copyTo(oc_file_rec *rec);
//private:
	double JD;
	double r0, d0, r1, d1, r_dif, d_dif, tot_dif;
};

class oc_file : public FileDynStrBin
{
public:
	oc_file_rec *record;

	oc_file();
	~oc_file();

	int GetRec(int pos);
	int AddRec(double JD, double r0, double d0, double r1, double d1);
	int AddRec();

	void rec2s();
	void s2rec(char *arec);
};

class OC
{
public:
//	orbit *orb0, *orb1;
//	List4 *coords;

	oc_file *f_out;
	observ *obs;

	double sg;

	OC();
	~OC();

//	int CountOC(double t0, double t1, double step);
	int CountOC(orbit *orb, List4 *odata);
	int DetOC(double t);

	
};

#define OC_H
#endif

