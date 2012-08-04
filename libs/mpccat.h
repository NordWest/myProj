//#pragma once

#ifndef COMF_H
#include "comfunc.h"
#endif

#ifndef ORBCAT_H
#include "orbcat.h"
#endif

#ifndef FBSTREAM_H
#include "fbStream.h"
#endif

#ifndef MPCCAT_H

#define MPCCAT_LEN 203
#define MPCCAT_NAME_KPOS 175


class mpcrec
{
public:
	char *provnum;		//Number or provisional designation (in packed form), [a7]
	float H;			//Absolute magnitude, H, [f5.2]
	float G;			//Slope parameter, G, [f5.2]
	char *epoch;		//Epoch (in packed form, .0 TT), [a5]
	double meanA;		//Mean anomaly at the epoch, in degrees, f[9.5]
	double w;			//Argument of perihelion, J2000.0 (degrees), [f9.5]
	double Node;			//Longitude of the ascending node, J2000.0 (degrees), [f9.5]
	double inc;			//Inclination to the ecliptic, J2000.0 (degrees), [f9.5]
	double ecc;			//Orbital eccentricity, [f9.7]
	double n;			//Mean daily motion (degrees per day), [f11.8]
	double a;			//Semimajor axis (AU), [f11.7]
	int U;				//Uncertainty parameter, U, [i1]
/*
If this column contains `E' it indicates
that the orbital eccentricity was assumed.
For one-opposition orbits this column can
also contain `D' if a double (or multiple)
designation is involved or `F' if an e-assumed
double (or multiple) designation is involved.
*/
	char *ref;			//Reference, [a10]
	int obsnum;			//Number of observations, [i5]
	int oppnum;			//Number of oppositions, [i3]

//For multiple-opposition orbits:
	int obs_fst;		//Year of first observation, [i4]
//     132        a1     '-'
	char *flag_multi;	//'-' = multi
	int obs_lst;		//Year of last observation, [i4]

//For single-opposition orbits:
	int arclen;			//Arc length (days), [i4]
	char *days;			//'days', [a4]

//
	double rms;			//r.m.s residual ("), [f4.2]
	char *indCoarse;	//Coarse indicator of perturbers (blank if unperturbed one-opposition object), [a3]
	char *indPrecide;	//Precise indicator of perturbers (blank if unperturbed one-opposition object), [a3]
	char *compName;		//Computer name, [a10]
	char *flags;		//4-hexdigit flags
/*
					The bottom 6 bits are used to encode a
                    value representing the orbit type (other
                    values are undefined):

                      Value
                        2  Aten
                        3  Apollo
                        4  Amor
                        5  Object with q < 1.381 AU
                        6  Object with q < 1.523 AU
                        7  Object with q < 1.665 AU
                        8  Hilda
                        9  Jupiter Trojan
                       10  Centaur
                       14  Plutino
                       15  Other resonant TNO
                       16  Cubewano
                       17  Scattered disk

                    Additional information is conveyed by
                    adding in the following bit values:

                       64  Unused
                      128  Unused
                      256  Unused
                      512  Unused
                     1024  Unused
                     2048  Unused
                     4096  Unused
                     8192  1-opposition object seen at
                           earlier opposition
                    16384  Critical list numbered object
                    32768  Object is PHA
*/
//167 - 194  a      Readable designation
	char *number;	//Number
	char *name;		//Name

	char *lastObs;

	mpcrec();
	~mpcrec();

	void copyTo(mpcrec *nrec);
	void copyTo(eposrec *nrec);

	int getProvnum();
	int getNum();
	double getEpoch();
        void getNumStr(char *rstr);
        void getProvnumStr(char *rstr);

/*
	void set_number(int numb);
	void set_epoha(int epo);
	void set_eJD(double eJD);
	void set_M0(double m0);
	void set_w(double w);
	void set_Node(double node);
	void set_ec(double ec);
	void set_inc(double inc);
	void set_q(double q);
	void set_H(float H);
	void set_G(float G);
	void set_pr(char *pr);
	void set_opp(int opp);
	void set_obs(int obs);
	void set_obsY(int obsY);
	void set_regY(int regY);
	void set_un(char *un);
	void set_name(char *name);
	void set_author(char *author);
	void set_makeD(char *makeD);
	void set_diam(float diam);
*/
	void set_default();

};

class mpccat : public fbStreamStr
{
public:
	mpcrec *record;

	void s2rec(char *str_in);
//	void rec2s(char *str_out);

//	int GetRec(int pos);
	int GetRecName(char *name);
//	int GetRecNum(int num);
//	int AddRec(int pos);
//	int AddRec();

	mpccat();
	mpccat(char *fn);
	~mpccat();

	int init(char *fname);
	int init(char *fname, int catKpos);
};

#define MPCCAT_H
#endif


////////////////////////////////////////////////////////////
