#include "observ.h"

////////////////////////////////////observ

////////////public

int observ::initObservatory(char *fname_obs)
{
	this->obs->init(fname_obs, OBS_SIZE);

	if(this->obs->nstr==0) return 1;

	return 0;
}


observ::observ()
{
	this->obs = new observatory();
	this->place = new dele();

	mass = new double[20];
}

observ::~observ()
{
	delete(this->obs);
	delete(this->place);
}

int observ::initDELE(char *fname_dele)
{
        if(this->place->init(fname_dele)) return 2;
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
        if(this->initObservatory(fname_obs)) return 1;
        if(this->initDELE(fname_dele_bin)) return 2;

        return 0;
}

int observ::set_obs_parpam(int nplanet, int center, int sk, char *nobsy)
{
	this->nplanet = nplanet;
	this->center = center;
	this->nobsy = nobsy;
	this->sk = sk;

    return(this->obs->getobsynumO(nobsy));
}

int observ::det_observ_utc(double tUTC)
{
    ctime.setUTC(tUTC);
    return(det_observ());
}

int observ::det_observ_tdb(double tTDB)
{
    ctime.setTDB(tTDB);
    return(det_observ());
}

int observ::det_observ_tt(double tTT)
{
    ctime.setTDT(tTT);
    return(det_observ());
}

int observ::det_observ()
{
    this->obs->det_state(ctime.UTC());
    int res = 0;

    if(this->place->detState(&this->ox, &this->oy, &this->oz, &this->ovx, &this->ovy, &this->ovz, ctime.TDB(), this->nplanet, this->center, sk)) return 1;
    if(this->place->detRtt(&this->ovxt, &this->ovyt, &this->ovzt, ctime.TDB(), this->nplanet, this->center, sk)) return 3;

    state[0] = ox+obs->x;
    state[1] = oy+obs->y;
    state[2] = oz+obs->z;
    state[3] = ovx+obs->vx;
    state[4] = ovy+obs->vy;
    state[5] = ovz+obs->vz;

    return res;
}


int observ::det_vect_radec1(double *state2sun, double *raRad, double *decRad, double *range, int corr)
{
    double *R, *X, *V, *X1, *V1, *xE0, *xEB0, *XS0, *VS0, *Q, *Qb, *Qb0, *P;
    R = new double[3];
    P = new double[3];
    X = new double[3];
    V = new double[3];
    X1 = new double[3];
    V1 = new double[3];
    xE0 = new double[3];
    xEB0 = new double[3];
    XS0 = new double[3];
    VS0 = new double[3];
    Q = new double[6];
    Qb = new double[6];
    Qb0 = new double[3];

    double normE, normP, normQ;
    double ct0, ct1, tau, muc2;

    muc2 = 9.8704e-9;

//X - sun
    Q[0] = state2sun[0];
    Q[1] = state2sun[1];
    Q[2] = state2sun[2];

    Q[3] = state2sun[3];
    Q[4] = state2sun[4];
    Q[5] = state2sun[5];

//ssb
    xE0[0] = state[0];
    xE0[1] = state[1];
    xE0[2] = state[2];


    if(this->place->detR(&XS0[0], &XS0[1], &XS0[2], ctime.TDB(), SUN_NUM, 0, 0, 0)) return 1;
    if(this->place->detR(&VS0[0], &VS0[1], &VS0[2], ctime.TDB(), SUN_NUM, 1, 0, 0)) return 1;

//X1 - ssb

    Qb[0] = Q[0] + XS0[0];
    Qb[1] = Q[1] + XS0[1];
    Qb[2] = Q[2] + XS0[2];

    Qb[3] = Q[3] + VS0[0];
    Qb[4] = Q[4] + VS0[1];
    Qb[5] = Q[5] + VS0[2];

    xEB0[0] = xE0[0] + XS0[0];
    xEB0[1] = xE0[1] + XS0[1];
    xEB0[2] = xE0[2] + XS0[2];


    P[0] = Qb[0] - xEB0[0];
    P[1] = Qb[1] - xEB0[1];
    P[2] = Qb[2] - xEB0[2];

    normP = norm3(P);
    normE = norm3(xEB0);
    normQ = norm3(Qb);

    ct1 = normP;

    do
    {
        ct0 = ct1;
        tau = ct1/CAU;

        Qb0[0] = Qb[0] - Qb[3]*tau;
        Qb0[1] = Qb[1] - Qb[4]*tau;
        Qb0[2] = Qb[2] - Qb[5]*tau;

        P[0] = Qb0[0] - xEB0[0];
        P[1] = Qb0[1] - xEB0[1];
        P[2] = Qb0[2] - xEB0[2];

        normP = norm3(P);

        normQ = norm3(Qb0);

        ct1 = normP+2.0*muc2*log((normE+normQ+normP)/(normE+normQ-normP));

    }while((fabs(ct1-ct0)/fabs(ct1))>1e-12);

    rdsys(raRad, decRad, P[0], P[1], P[2]);

    if(range!=NULL)
    {
        range[0] = R[0];
        range[1] = R[1];
        range[2] = R[2];
    }
}

int observ::det_vect_radec1_ssb(double *state2ssb, double *raRad, double *decRad, double *range, int corr)
{
    double *R, *X, *V, *X1, *V1, *xE0, *xEB0, *XS0, *VS0, *Q, *Qb, *Qb0, *P;
    R = new double[3];
    P = new double[3];
    X = new double[3];
    V = new double[3];
    X1 = new double[3];
    V1 = new double[3];
    xE0 = new double[3];
    xEB0 = new double[3];
    XS0 = new double[3];
    VS0 = new double[3];
    Q = new double[6];
    Qb = new double[6];
    Qb0 = new double[3];

    double normE, normP, normQ;
    double ct0, ct1, tau, muc2;

    muc2 = 9.8704e-9;

//X - sun
    Qb[0] = state2ssb[0];
    Qb[1] = state2ssb[1];
    Qb[2] = state2ssb[2];

    Qb[3] = state2ssb[3];
    Qb[4] = state2ssb[4];
    Qb[5] = state2ssb[5];

//ssb
    xE0[0] = state[0];
    xE0[1] = state[1];
    xE0[2] = state[2];


    if(this->place->detR(&XS0[0], &XS0[1], &XS0[2], ctime.TDB(), SUN_NUM, 0, 0, 0)) return 1;
    if(this->place->detR(&VS0[0], &VS0[1], &VS0[2], ctime.TDB(), SUN_NUM, 1, 0, 0)) return 1;

//X1 - ssb
/*
    Qb[0] = Q[0] + XS0[0];
    Qb[1] = Q[1] + XS0[1];
    Qb[2] = Q[2] + XS0[2];

    Qb[3] = Q[3] + VS0[0];
    Qb[4] = Q[4] + VS0[1];
    Qb[5] = Q[5] + VS0[2];
*/
    xEB0[0] = xE0[0] + XS0[0];
    xEB0[1] = xE0[1] + XS0[1];
    xEB0[2] = xE0[2] + XS0[2];


    P[0] = Qb[0] - xEB0[0];
    P[1] = Qb[1] - xEB0[1];
    P[2] = Qb[2] - xEB0[2];

    normP = norm3(P);
    normE = norm3(xEB0);
    normQ = norm3(Qb);

    ct1 = normP;

    do
    {
        ct0 = ct1;
        tau = ct1/CAU;

        Qb0[0] = Qb[0] - Qb[3]*tau;
        Qb0[1] = Qb[1] - Qb[4]*tau;
        Qb0[2] = Qb[2] - Qb[5]*tau;

        P[0] = Qb0[0] - xEB0[0];
        P[1] = Qb0[1] - xEB0[1];
        P[2] = Qb0[2] - xEB0[2];

        normP = norm3(P);

        normQ = norm3(Qb0);

        ct1 = normP+2.0*muc2*log((normE+normQ+normP)/(normE+normQ-normP));

    }while((fabs(ct1-ct0)/fabs(ct1))>1e-12);

    rdsys(raRad, decRad, P[0], P[1], P[2]);

    if(range!=NULL)
    {
        range[0] = R[0];
        range[1] = R[1];
        range[2] = R[2];
    }
}

int observ::det_vect_radec(double *state2sun, double *raRad, double *decRad, double *range, int corr)
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


//X - sun
    X[0] = state2sun[0];
    X[1] = state2sun[1];
    X[2] = state2sun[2];

    V[0] = state2sun[3];
    V[1] = state2sun[4];
    V[2] = state2sun[5];

//ssb
    XE0[0] = state[0];
    XE0[1] = state[1];
    XE0[2] = state[2];


    if(this->place->detR(&XS0[0], &XS0[1], &XS0[2], ctime.TDB(), SUN_NUM, 0, 0, 0)) return 1;
    if(this->place->detR(&VS0[0], &VS0[1], &VS0[2], ctime.TDB(), SUN_NUM, 1, 0, 0)) return 1;

//X1 - ssb

    X1[0] = X[0] + XS0[0];
    X1[1] = X[1] + XS0[1];
    X1[2] = X[2] + XS0[2];

    V1[0] = V[0] + VS0[0];
    V1[1] = V[1] + VS0[1];
    V1[2] = V[2] + VS0[2];
/*
    XEB0[0] = XE0[0] + XS0[0];
    XEB0[1] = XE0[1] + XS0[1];
    XEB0[2] = XE0[2] + XS0[2];
*/

    R[0] = X1[0] - XE0[0];
    R[1] = X1[1] - XE0[1];
    R[2] = X1[2] - XE0[2];

    normR = norm3(R);

//    if(corr)
//    {
        normE = norm3(XE0);
        normP = normR;
        normQ = norm3(X1);

        ct1 = normP;

        do
        {
            ct0 = ct1;
            tau = ct1/CAU;

            Q[0] = X1[0] - V1[0]*tau;
            Q[1] = X1[1] - V1[1]*tau;
            Q[2] = X1[2] - V1[2]*tau;

            P[0] = Q[0] - XE0[0];
            P[1] = Q[1] - XE0[1];
            P[2] = Q[2] - XE0[2];

            normP = norm3(P);
/*
            Q[0] = Qb[0] - XS0[0];
            Q[1] = Qb[1] - XS0[1];
            Q[2] = Qb[2] - XS0[2];
*/
            normQ = norm3(Q);

            ct1 = normP+2.0*muc2*log((normE+normQ+normP)/(normE+normQ-normP));

        }while((fabs(ct1-ct0)/fabs(ct1))>1e-12);
//    }

    rdsys(raRad, decRad, P[0], P[1], P[2]);

    if(range!=NULL)
    {
        range[0] = R[0];
        range[1] = R[1];
        range[2] = R[2];
    }
}

int observ::det_vect_radec_ssb(double *state2ssb, double *raRad, double *decRad, double *range, int corr)
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


//X1 - ssb
    X1[0] = state2ssb[0];
    X1[1] = state2ssb[1];
    X1[2] = state2ssb[2];

    V1[0] = state2ssb[3];
    V1[1] = state2ssb[4];
    V1[2] = state2ssb[5];

//ssb
    XE0[0] = state[0];
    XE0[1] = state[1];
    XE0[2] = state[2];

/*
    if(this->place->detR(&XS0[0], &XS0[1], &XS0[2], ctime.TDB(), SUN_NUM, 0, 0, 0)) return 1;
    if(this->place->detR(&VS0[0], &VS0[1], &VS0[2], ctime.TDB(), SUN_NUM, 1, 0, 0)) return 1;

//X1 - ssb
/*
    X1[0] = X[0] + XS0[0];
    X1[1] = X[1] + XS0[1];
    X1[2] = X[2] + XS0[2];

    V1[0] = V[0] + VS0[0];
    V1[1] = V[1] + VS0[1];
    V1[2] = V[2] + VS0[2];
/*
    XEB0[0] = XE0[0] + XS0[0];
    XEB0[1] = XE0[1] + XS0[1];
    XEB0[2] = XE0[2] + XS0[2];
*/

    R[0] = X1[0] - XE0[0];
    R[1] = X1[1] - XE0[1];
    R[2] = X1[2] - XE0[2];

    normR = norm3(R);

//    if(corr)
//    {
        normE = norm3(XE0);
        normP = normR;
        normQ = norm3(X1);

        ct1 = normP;

        do
        {
            ct0 = ct1;
            tau = ct1/CAU;

            Q[0] = X1[0] - V1[0]*tau;
            Q[1] = X1[1] - V1[1]*tau;
            Q[2] = X1[2] - V1[2]*tau;

            P[0] = Q[0] - XE0[0];
            P[1] = Q[1] - XE0[1];
            P[2] = Q[2] - XE0[2];

            normP = norm3(P);
/*
            Q[0] = Qb[0] - XS0[0];
            Q[1] = Qb[1] - XS0[1];
            Q[2] = Qb[2] - XS0[2];
*/
            normQ = norm3(Q);

            ct1 = normP+2.0*muc2*log((normE+normQ+normP)/(normE+normQ-normP));

        }while((fabs(ct1-ct0)/fabs(ct1))>1e-12);
//    }

    rdsys(raRad, decRad, P[0], P[1], P[2]);

    if(range!=NULL)
    {
        range[0] = R[0];
        range[1] = R[1];
        range[2] = R[2];
    }
}

int observ::radec500(double *raRad0, double *decRad0, double raRad, double decRad, double ro)
{
    double X[3];

    X[0] = ro*cos(decRad)*cos(raRad);
    X[1] = ro*cos(decRad)*sin(raRad);
    X[2] = ro*sin(decRad);

    X[0] -= this->obs->state[0];
    X[1] -= this->obs->state[1];
    X[2] -= this->obs->state[2];

    rdsys(raRad0, decRad0, X[0], X[1], X[2]);

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

