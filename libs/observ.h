#ifndef OBSERV_H
#define OBSERV_H

#include <stdio.h>
#include <QtCore>

#include "observatory.h"
#include "comfunc.h"
#include "mpcs.h"
#include <dele.h>
#include "time_a.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

class observ
{
public:

	observatory *obs;
	dele *place;

	double *mass;

    QString obsName, obsyCode, centerName, refName;

	int initObservatory(char *fname_obs);
    int initDELE(char *fname_dele_bin);
    int init(char *fname_obs, char *fname_dele_bin);

	int set_obs_parpam(int nplanet, int center, int sk, char *nobsy);

    int det_observ_utc(double jdUTC);
    int det_observ_tt(double jdTT);
    int det_observ_tdb(double jdTDB);

//position and velocity of observer
    double state[6];
    double ox, oy, oz, ovx, ovy, ovz;           //R, Rt
    double ovxt, ovyt, ovzt;					//Rtt

//obstime
    time_a ctime;
    int nplanet, center, sk;

	observ(char *dele_header);
	observ();
	~observ();

    int detSunRADEC(double *raS, double *decS);
    int det_vect_radec(double *state2sun, double *raRad, double *decRad, double *range = NULL, int corr = 1);
    int det_vect_radec_ssb(double *state2ssb, double *raRad, double *decRad, double *range = NULL, int corr = 1);

    int det_observ();

    QString getObsCode();

    int radec500(double *raRad0, double *decRad0, double raRad, double decRad, double ro);


private:
    double et;
    char *nobsy;
};

#endif
