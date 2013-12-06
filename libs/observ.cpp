#include "stdafx.h"
//#include "..\DAster\StdAfx.h"
//#include "..\DAster\DAster.h"

#include "observ.h"


//#include "comfunc.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

/*
#define AU 1
#define MS 1
#define VE 2979256.9529966
#define PI 3.141592653589
#define ka 0.017202098955
#define EPS 1.0e-8
#define xi ka*sqrt(MS)
//#define EKV 0.4093197552027299
/
#define EPO 1900
#define JDEPO 2415020.5
*/

////////////////////////////////////observ

////////////public

int observ::initObservatory(char *fname_obs)
{
	this->obs->init(fname_obs, OBS_SIZE);

	if(this->obs->nstr==0) return 1;

	return 0;
}


observ::observ(char *dele_header)
{
	this->obs = new observatory();
	this->place = new dele(dele_header);

//	printf("ini complete\n");
    //ephType = 0;

	mass = new double[20];
}

observ::observ()
{
//	printf("initialization\n");
	this->obs = new observatory();
	this->place = new dele();

//	printf("ini complete\n");

    //ephType = 0;
	mass = new double[20];
}

observ::~observ()
{
	delete(this->obs);
	delete(this->place);
}

int observ::initDELE(char *fname_dele)
{
        //if(this->place->init_header(fname_dele_h)) return 1;
        if(this->place->init(fname_dele)) return 2;
        //rec1040 rc;
        int i;
        for(i=8; i<19; i++)
        {
                //this->place->g1041->getElem(&rc, i);
                //mass[i-8] = H2.data.constValue[i];//rc.value;
        }

        return 0;
}

int observ::init(char *fname_obs, char *fname_dele_bin)
{
//	printf("init\n");
        if(this->initObservatory(fname_obs)) return 1;
        if(this->initDELE(fname_dele_bin)) return 2;

//	printf("init complete\n");
        return 0;
}

int observ::set_obs_parpam(int nplanet, int center, int sk, char *nobsy)
{
//	printf("set obs param\n");
	this->nplanet = nplanet;
	this->center = center;
	this->nobsy = nobsy;
	this->sk = sk;

//	printf("end set obs param\n");
    return(this->obs->getobsynumO(nobsy));

//	return 0;
}

int observ::setUTC(double tUTC)
{
    ctime.setUTC(tUTC);
    return(det_observ());
}

int observ::setTDB(double tTDB)
{
    ctime.setTDB(tTDB);
    return(det_observ());
}

int observ::setTT(double tTT)
{
    ctime.setTDT(tTT);
    return(det_observ());
}


int observ::det_observ()
{
    qDebug() << "det_observ\n";
    qDebug() << "det_state\n";
    this->obs->det_state(ctime.UTC());
    int res = 0;


    qDebug() << "detState\n";
    if(this->place->detState(&this->ox, &this->oy, &this->oz, &this->ovx, &this->ovy, &this->ovz, ctime.TDB(), this->nplanet, this->center, sk)) return 1;
    qDebug() << "detRtt\n";
    if(this->place->detRtt(&this->ovxt, &this->ovyt, &this->ovzt, ctime.TDB(), this->nplanet, this->center, sk)) return 3;



    pos[0] = ox+obs->dcx;
    pos[1] = oy+obs->dcy;
    pos[2] = oz+obs->dcz;

    vel[0] = ovx+obs->vx;
    vel[1] = ovy+obs->vy;
    vel[2] = ovz+obs->vz;

/*
	this->ovxt = -(ka*ka*this->ox)/(fabs(pow(this->ox, 3.0)));
	this->ovyt = -(ka*ka*this->oy)/(fabs(pow(this->oy, 3.0)));
	this->ovzt = -(ka*ka*this->oz)/(pow(fabs(this->oz), 3.0));
*/
    return res;
}



int observ::det_vect_radec(double *stateRV, double *ra, double *dec, double *range)
{
    double *R, *X, *V, *X1, *V1, *XE0, *XEB0, *XS0, *VS0, *Q, *Qb, *P;
    R = new double[3];
    P = new double[3];
    X = new double[3];
    V = new double[3];
    X1 = new double[3];
    V1 = new double[3];
    XE0 = new double[3];
    XEB0 = new double[3];
    XS0 = new double[3];
    VS0 = new double[3];
    Q = new double[3];
    Qb = new double[3];

    double normR, normE, normP, normQ;
    double ct0, ct1, tau, muc2;

    muc2 = 9.8704e-9;

    X[0] = stateRV[0];
    X[1] = stateRV[1];
    X[2] = stateRV[2];

    V[0] = stateRV[3];
    V[1] = stateRV[4];
    V[2] = stateRV[5];

    XE0[0] = pos[0];
    XE0[1] = pos[1];
    XE0[2] = pos[2];


    if(this->place->detR(&XS0[0], &XS0[1], &XS0[2], ctime.TDB(), SUN_NUM, 0, 0, 0)) return 1;
    if(this->place->detR(&VS0[0], &VS0[1], &VS0[2], ctime.TDB(), SUN_NUM, 1, 0, 0)) return 1;


    X1[0] = X[0] + XS0[0];
    X1[1] = X[1] + XS0[1];
    X1[2] = X[2] + XS0[2];

    V1[0] = V[0] + VS0[0];
    V1[1] = V[1] + VS0[1];
    V1[2] = V[2] + VS0[2];

    XEB0[0] = XE0[0] + XS0[0];
    XEB0[1] = XE0[1] + XS0[1];
    XEB0[2] = XE0[2] + XS0[2];


    R[0] = X[0] - XE0[0];
    R[1] = X[1] - XE0[1];
    R[2] = X[2] - XE0[2];

    normR = norm(R);

    normE = norm(XE0);
    normP = normR;
    normQ = norm(X);

    ct1 = normP;


    do
    {

        ct0 = ct1;
        tau = ct1/CAU;

        Qb[0] = X1[0] - V1[0]*tau;
        Qb[1] = X1[1] - V1[1]*tau;
        Qb[2] = X1[2] - V1[2]*tau;

        P[0] = Qb[0] - XEB0[0];
        P[1] = Qb[1] - XEB0[1];
        P[2] = Qb[2] - XEB0[2];

        //qDebug() << QString("Qb: %1\t%2\t%3\n").arg(Qb[0], 17, 'f', 8).arg(Qb[1], 17, 'f', 8).arg(Qb[2], 17, 'f', 8);
        //qDebug() << QString("P: %1\t%2\t%3\n").arg(P[0], 17, 'f', 8).arg(P[1], 17, 'f', 8).arg(P[2], 17, 'f', 8);

        normP = norm(P);

        Q[0] = Qb[0] - XS0[0];
        Q[1] = Qb[1] - XS0[1];
        Q[2] = Qb[2] - XS0[2];

        normQ = norm(Q);
/*
        qDebug() << QString("normQ: %1\n").arg(normQ);
        qDebug() << QString("normP: %1\n").arg(normP);
        qDebug() << QString("normE: %1\n").arg(normE);
*/
        ct1 = normP+2.0*muc2*log((normE+normQ+normP)/(normE+normQ-normP));
        //qDebug() << QString("ct1: %1\n").arg(ct1, 10);

//        qDebug() << QString("dct1: %1\n").arg(fabs(ct1-ct0)/fabs(ct1), 10);
        //qDebug() << QString("tau: %1\n").arg(tau*86400.0);//qDebug() << QString("normQ: %1\n").arg(normQ);

    }while((fabs(ct1-ct0)/fabs(ct1))>1e-12);

//    tau = ct1/CAU;


    rdsys(ra, dec, P[0], P[1], P[2]);
    //rdsys(ra, dec, X[0], X[1], X[2]);
    //detRDnumGC(ra, dec, P[0], P[1], P[2], ox, oy, oz, obs_pos->obs->state[3], obs_pos->obs->state[4], obs_pos->obs->state[5]);

    if(range!=NULL)
    {
        range[0] = R[0];
        range[1] = R[1];
        range[2] = R[2];
    }

}

QString observ::getObsCode()
{
    return(QString(obs->record->num));
}

int observ::detSunRADEC(double *raS, double *decS)
{
    double Xs, Ys, Zs;
    if(this->place->detR(&Xs, &Ys, &Zs, ctime.TDB(), SUN_NUM, 0, this->center, sk)) return 1;
    rdsys(raS, decS, Xs-ox, Ys-oy, Zs-oz);
	return 0;
}

