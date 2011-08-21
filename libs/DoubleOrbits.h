#ifndef ORB_H
#include "orbit.h"
#endif

#ifndef DOUBLE_ORB_H

class ProgFlags
{
public:
	int isVisual;
	int isStable;
	int isDetective;

	ProgFlags();
	~ProgFlags();
};


struct VisualVars
{
	double RA, DEC;
	double A, h;
//	double Amax, hmax, tmax;
};

struct DetectiveVars
{
	double Q, Qmax;
	double elongation;
	double m;
	double Edist, Sdist;
};

struct StableVars
{
	
	double R1, R2;
	double r, r1, r2;
	double r1min, r1max, dr1;
	double A, B, C, a, b, c;	//poluosi v km
	double M1, M2;
	double T;
	double norm;				//normalize const
	double mju1, mju2;
	double E, K, Ip, U;
};

class Variables
{
public:

//	VisualVars *v_vars;
	DetectiveVars *d_vars;
	StableVars *s_vars;

	note *Obj;

	char *Name;
	double num;
	int obs;

//	double isVisual;
	double isStable;
	double isDetective;

	Variables();
	Variables(char *fn_gud);
	~Variables();
	int copyTo(Variables *targ);

///////////////////////////////////////

	FILE *FGUD, *FOBS;
	char *fn_gud, *fn_obs;

	int get_gud_num(int pos);
	int get_gud_name(char *name);

	int get_name_stat(char *name);

	int add2gudlist(int status);
	int add2gudlist();

	int get_best_name();

	void clear();

	void set(char *name, int num);
	void set(Variables *st);

	void clear_gud();
	void clear_bad(int hdp);

	int count_all();
	int count_stat(int st);
	int count_bad();

	int init_obs_list(char *fn_obs_list);
	int inc_obs_list();
	int get_obs();

	int det_num(int isD, int isS);

	int status;
////////////////////////////////////////////

};

struct DoubleOrbitsParams
{
	double R2R1;

	double R, T;			//distance between bodies and both rotation perion  | normalized, hours
	double A, B, C, Tp;		//semimajor axies first body and rotation period | normalized, hours
	double a, b, c, To;		//semimajor axies second body and rotation period | normalized, hous

	double sigma;			//density
//	double M2M1;			//masses ratio

	double norm;			//normalize const

	double M1, M2;

//	double mju1, mju2;

//	double  E, K, Ip, U;

//detective
	double Qmin;	//razreshajushaya sposobnost' teleskopa [sec]
//	double Qmax;	//uglovoe rasstoyanie sfery dejstviya [sec]
	double dQ;
	double PhaseM;	//Diapazon Fazy
	double Mm;		//Predel`naya zvezdnaya velichina
	double EdistMax;
	double Radius;
	double c1;
	double Albedo;				//Al'bedo

	ProgFlags *flags;
};

class DoubleOrbits
{
public:
	

//	ComPParam *CP;
//	VisualParam *VP;
//	StableParam *SP;
//	DetectiveParam *DP;

	DoubleOrbitsParams *params;

	orbits *objorbs;

	Variables *vars;

	FILE *FCFG;

//	int init(char *fcat, char *fobs, char *fnres, char *fnvl, char *fnearth, int onum);

//	int init_CPV();
//	int init_CPV_Time(double tUTC);
//	int init_VP_lfh(double lam, double fi, double h);
//	int init_VP_onum(char *fname, int onum);
//	int init_VP_oname(char *fname, char *oname);
//	int init_VP_Time();
//	int init_SP();
//	int init_DP();

	int initCFG(char *fn_cfg);

	int init_Time(double tUTC);

//	char *fn_obs;
	char *fn_res;
	char *fn_varList;
//	char *fn_earth;
	char *fn_log;

	int nres;

//	void set_VP_hmin(double hmin);
//	void set_VP_frame(double RAmin, double RAmax, double DECmin, double DECmax);
/*
	int set_DP_Qmin(double Qmin);
	int set_DP_Phm(double PhaseM);
	int set_DP_Mm(double Mm);
	int set_DP_c1(double c1);
	int set_DP_EdM(double EdistMax);
	int set_DP_RtKM(double Rt);
*/
/*
	void set_SP_R2R1(double R2R1);
	int set_SP_T(double T);
	void set_SP_ABCTp(double A, double B, double C, double Tp);
	void set_SP_abcTo(double a, double b, double c, double To);
	void set_SP_sigma(double sigma);
	int set_SP_M2M1(double M2M1);

	int det_VP_h();
*/	
//	int det_CP_ro();
	int det_Obj();

	void det_vars_Qmax();

	int det_vars_r1_next();
	void det_vars_R1();
	void det_vars_R2();
	void det_vars_r1min();
	void det_vars_r1max();
	void det_vars_dr1();
	void det_vars_bodies();
	void det_vars_system();
	void det_vars_normalized();
	void det_vars_others();
	void det_vars_phase();
	void det_vars_m();
	void det_vars_Edist();
	void det_vars_Sdist();
	void det_vars_Q();
//	int det_vars_hmax_tmax();
	int det_vars();

//	int isVisual();
	int isDetective();
	int isStable();

	int save_vars();

	int print_vars();

	int count(double Jday);
	int count_range(double Jday0, double Jday1, double step);


//
	int init_obs(char *fn_gud_list);
//	int get_next_obj(char *pstr, double cTime);

	char *err_str;

	DoubleOrbits();
	~DoubleOrbits();
};


#define DOUBLE_ORB_H
#endif