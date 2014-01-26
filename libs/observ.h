//#pragma once
#include <stdio.h>
#include <QtCore>

#ifndef OBSRY_H
#include "observatory.h"
#endif

#ifndef COMF_H
#include "comfunc.h"
#endif

#ifndef MPCS_H
#include "mpcs.h"
#endif

#ifndef DELE_H
#include <dele.h>
#endif

#ifndef TIMEA_H
#include "time_a.h"
#endif

//
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#ifndef OBSERV_H


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
    double pos[3], vel[3];
    double ox, oy, oz, ovx, ovy, ovz;           //R, Rt
    double ovxt, ovyt, ovzt;					//Rtt

//obstime
    time_a ctime;
    int nplanet, center, sk;

	observ(char *dele_header);
	observ();
	~observ();

    int detSunRADEC(double *raS, double *decS);
    int det_vect_radec(double *stateSSB, double *raRad, double *decRad, double *range = NULL, int corr = 1);

    int det_observ();

    QString getObsCode();

private:
    double et;
    char *nobsy;
};

#define OBSERV_H
#endif
