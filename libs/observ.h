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
#include "dele.h"
#endif

#ifndef TIMEA_H
#include "time_a.h"
#endif

#include "./cspice/SpiceUsr.h"

#ifndef OBSERV_H



class observ
{
public:

	observatory *obs;
	dele *place;

	double *mass;

    QString obsName, obsyCode, centerName, refName;
    int ephType;    //0-dele;   1-SPICE;    2-calceph

	int initObservatory(char *fname_obs);
        //int initDELE(char *fname_dele_h, char *fname_dele_bin);
        //int init(char *fname_obs, char *fname_dele_h, char *fname_dele_bin);

    int initDELE(char *fname_dele_bin);
    int init(char *fname_obs, char *fname_dele_bin);
    int initSPICE(QString bspName, QString lskName);
    int initSPK(QString spkName);
    int init(QString fname_obs, QString bspName, QString lskName);

	int set_obs_parpam(int nplanet, int center, int sk, char *nobsy);
    int set_spice_parpam(QString obs_name, QString obsy_code, QString center_name, QString ref_name);


    double pos[3], vel[3];

    double ox, oy, oz, ovx, ovy, ovz;
    //double jdTDB;
    time_a ctime;
    double et;
	double ovxt, ovyt, ovzt;					//Rtt
	int nplanet, center, sk;
	char *nobsy;

	observ(char *dele_header);
	observ();
	~observ();

    int detSunRADEC(double *raS, double *decS);

    int setUTC(double tUTC);
    int setTDB(double tTDB);
    int det_observ();
    int det_obj_radec(QString objName, double *ra, double *dec, double *range);
    int det_vect_radec(double *stateRV, double *ra, double *dec, double *range = NULL);

    QString getObsCode();

/*
	mpcs *lmpc;
	int nmpcs;
	int Fkey;		//0-mpc; 1-gmt; 2-ast
	FILE *FC;
	char *hat;

	observatory *obs;
	

	observ();
	observ(int type);
	~observ();


	int detFkey(char *fname);
	int initFile(char *fname);
	int initFile(char *fname, char *fnobs);
	int initObs(observ *eobs);

	int getmpc(mpc *n, int nofapp);
	mpc* getlmpc(int nofmpc);

	int writeMpc(char *fname, int mode, int nofn);	//mode=0 new file, mode=1 add old file	//WORK NOT CORRECT!!!!!//work only mode=0, nofn=*

	int topo2geo(mpc *pos, double SX, double SY, double SZ, double po);


private:

	int makeStrPos(mpc *pos, char *str);
	int makeStrPosGMT(mpc *pos, char *str);
*/
};

#define OBSERV_H
#endif
