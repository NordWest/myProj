#ifndef OBSERV_H
#define OBSERV_H

#include <stdio.h>
#include <QtCore>

#include "observatory.h"
#include "comfunc.h"
#include "mpcs.h"

#include "time_a.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define DELE_LIB
#define CALCEPH_LIB

#ifdef DELE_LIB
#include <dele.h>
#endif

#ifdef CALCEPH_LIB
#include <calceph.h>
#endif

int det_vect_radec(double *obj2ssb, double *obs2ssb, double *raRad, double *decRad, double *range=NULL);

class observ
{
public:

    double *mass;
    int ephType;
    QString obsName, obsyCode, centerName, refName;
	observatory *obs;

//position and velocity of observer
    double state[6];
    //double ox, oy, oz, ovx, ovy, ovz;           //R, Rt
    double ovxt, ovyt, ovzt;					//Rtt

//obstime
    time_a ctime;
    int nplanet, center, sk, centr_num;


    int init(char *fname_obs, char *fname_eph, int e_type = 0);
    int initObservatory(char *fname_obs);

#ifdef DELE_LIB
    dele *place;
    int initDELE(char *fname_dele_bin);
#endif

#ifdef CALCEPH_LIB

#endif

    int set_obs_parpam(char *pl_name, int center, int sk, char *nobsy);
	int set_obs_parpam(int nplanet, int center, int sk, char *nobsy);

    int det_observ();
    int det_observ_utc(double jdUTC);
    int det_observ_tt(double jdTT);
    int det_observ_tdb(double jdTDB);

	observ(char *dele_header);
	observ();
	~observ();

    int detStatePlanet(char *pl_name, int cent, int skoord, double *statePL);
    int detSunRADEC(double *raS, double *decS);
    int det_vect_radec(double *state2sun, double *raRad, double *decRad, double *range = NULL, int corr = 1);
    int det_vect_radec_ssb(double *state2ssb, double *raRad, double *decRad, double *range = NULL, int corr = 1);

    QString getObsCode();

    int radec500(double *raRad0, double *decRad0, double raRad, double decRad, double ro);

private:
    double et;
    char *nobsy;
};

#endif
