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

    pos[0] = ox+obs->dcx;
    pos[1] = oy+obs->dcy;
    pos[2] = oz+obs->dcz;

    vel[0] = ovx+obs->vx;
    vel[1] = ovy+obs->vy;
    vel[2] = ovz+obs->vz;

    return res;
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

    X1[0] = state2sun[0];
    X1[1] = state2sun[1];
    X1[2] = state2sun[2];

    V1[0] = state2sun[3];
    V1[1] = state2sun[4];
    V1[2] = state2sun[5];

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

//    if(corr)
//    {
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

            normP = norm(P);

            Q[0] = Qb[0] - XS0[0];
            Q[1] = Qb[1] - XS0[1];
            Q[2] = Qb[2] - XS0[2];

            normQ = norm(Q);

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

