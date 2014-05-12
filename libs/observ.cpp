#include "observ.h"

int det_vect_radec(double *obj2ssb, double *obs2ssb, double *raRad, double *decRad, double *range)
{
    double *R, *X, *V, *X1, *V1, *xE0, *xEB0, *XS0, *VS0, *Q, *Qb, *Qb0, *P;
    R = new double[3];
    P = new double[3];
    X = new double[3];
    V = new double[3];
    X1 = new double[3];
    V1 = new double[3];
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
    Qb[0] = obj2ssb[0];
    Qb[1] = obj2ssb[1];
    Qb[2] = obj2ssb[2];

    Qb[3] = obj2ssb[3];
    Qb[4] = obj2ssb[4];
    Qb[5] = obj2ssb[5];

//X1 - ssb

    xEB0[0] = obs2ssb[0];
    xEB0[1] = obs2ssb[1];
    xEB0[2] = obs2ssb[2];

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
        range[0] = Qb[0] - xEB0[0];
        range[1] = Qb[1] - xEB0[1];
        range[2] = Qb[2] - xEB0[2];
    }
}
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
#ifdef DELE_LIB
	this->place = new dele();
#endif
	mass = new double[20];
}

observ::~observ()
{
	delete(this->obs);
#ifdef DELE_LIB
	delete(this->place);
#endif
}
#ifdef DELE_LIB
int observ::initDELE(char *fname_dele)
{
        if(this->place->init(fname_dele)) return 2;
        int i;
        for(i=8; i<19; i++)
        {
                //this->place->g1041->getElem(&rc, i);
                //mass[i-8] = H2.data.constValue[i];//rc.value;
        }

        ephType=0;

        return 0;
}
#endif
int observ::init(char *fname_obs, char *fname_eph, int e_type)
{
    if(this->initObservatory(fname_obs)) return 1;
    ephType = e_type;
    switch(ephType)
    {
        case 0:
#ifdef DELE_LIB
        {
            if(this->initDELE(fname_eph)) return 2;
        }
#endif
        break;
        case 1:
#ifdef CALCEPH_LIB
        {
            int status = !calceph_sopen(fname_eph);
            centr_num = 11+!center;
            if(status)
            {
                qDebug() << QString("init ephemeride error\n");
                return 2;
            }
        }
#endif
        break;
    }
    return 0;
}

int observ::set_obs_parpam(char *pl_name, int center, int sk, char *nobsy)
{
    switch(ephType)
    {
        case 0:
#ifdef DELE_LIB
        {
            nplanet = planet_num(pl_name);
            center = center;
        }
#endif
        break;
        case 1:
#ifdef CALCEPH_LIB
        {
            nplanet = epm_planet_num(pl_name);
            centr_num = 11+!center;
        }
#endif
        break;
    }
    sk = sk;
    nobsy = nobsy;
    return(this->obs->getobsynumO(nobsy));
}

int observ::set_obs_parpam(int nplanet, int center, int sk, char *nobsy)
{
    nplanet = nplanet;
    center = center;
    nobsy = nobsy;
    sk = sk;
    centr_num = 11+!center;

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
    double timei = ctime.TDB();


    switch(ephType)
    {
    case 0:
        {
#ifdef DELE_LIB
        if(this->place->detState(&state[0], &state[1], &state[2], &state[3], &state[4], &state[5], ctime.TDB(), this->nplanet, this->center, sk)) return 1;
        if(this->place->detRtt(&this->ovxt, &this->ovyt, &this->ovzt, ctime.TDB(), this->nplanet, this->center, sk)) return 3;


#endif
        }
        break;
     case 1:
#ifdef CALCEPH_LIB
        {
            //calceph_scompute((int)timei, timei-(int)timei, 16, 0, state);
            //qDebug() << QString("TT-TDB = %1\n").arg(state[0]);
            //timei = time0 + state[0];
            calceph_scompute((int)timei, timei-(int)timei, nplanet, centr_num, state);

        }
#endif
            break;
    }
    state[0] += obs->x;
    state[1] += obs->y;
    state[2] += obs->z;
    state[3] += obs->vx;
    state[4] += obs->vy;
    state[5] += obs->vz;

    return res;
}

int observ::det_vect_radec(double *state2sun, double *raRad, double *decRad, double *range, int corr)
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

    double *stateSun = new double[6];
    detStatePlanet("Sun", 0, 0, stateSun);
    //if(this->place->detR(&XS0[0], &XS0[1], &XS0[2], ctime.TDB(), SUN_NUM, 0, 0, 0)) return 1;
    //if(this->place->detR(&VS0[0], &VS0[1], &VS0[2], ctime.TDB(), SUN_NUM, 1, 0, 0)) return 1;
    XS0[0] = stateSun[0];
    XS0[1] = stateSun[1];
    XS0[2] = stateSun[2];
    VS0[0] = stateSun[3];
    VS0[1] = stateSun[4];
    VS0[2] = stateSun[5];

//X1 - ssb

    Qb[0] = Q[0] + XS0[0];
    Qb[1] = Q[1] + XS0[1];
    Qb[2] = Q[2] + XS0[2];

    Qb[3] = Q[3] + VS0[0];
    Qb[4] = Q[4] + VS0[1];
    Qb[5] = Q[5] + VS0[2];

    if(center)
    {
        xE0[0] = state[0];
        xE0[1] = state[1];
        xE0[2] = state[2];

        xEB0[0] = xE0[0] + XS0[0];
        xEB0[1] = xE0[1] + XS0[1];
        xEB0[2] = xE0[2] + XS0[2];
    }
    else
    {
        xEB0[0] = state[0];
        xEB0[1] = state[1];
        xEB0[2] = state[2];
    }


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

int observ::det_vect_radec_ssb(double *state2ssb, double *raRad, double *decRad, double *range, int corr)
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

int observ::detStatePlanet(char *pl_name, int cent, int skoord, double *statePL)
{
    int pl_num;
    double timei = ctime.TDB();
    switch(ephType)
    {
    case 0:
        {
#ifdef DELE_LIB
        pl_num = planet_num(pl_name);
        if(this->place->detState(&statePL[0], &statePL[1], &statePL[2], &statePL[3], &statePL[4], &statePL[5], timei, pl_num, cent, skoord)) return 1;
#endif
        }

        break;
     case 1:
#ifdef CALCEPH_LIB
        {
            pl_num = epm_planet_num(pl_name);
            //calceph_scompute((int)timei, timei-(int)timei, 16, 0, state);
            //qDebug() << QString("TT-TDB = %1\n").arg(state[0]);
            //timei = time0 + state[0];
            calceph_scompute((int)timei, timei-(int)timei, pl_num, cent, statePL);

        }
#endif
            break;
    }
}

int observ::detSunRADEC(double *raS, double *decS)
{
    double *stateSun = new double[6];
    detStatePlanet("sun", 0, sk, stateSun);
    //if(this->place->detR(&Xs, &Ys, &Zs, ctime.TDB(), SUN_NUM, 0, this->center, sk)) return 1;
    rdsys(raS, decS, stateSun[0]-state[0], stateSun[1]-state[1], stateSun[2]-state[2]);
	return 0;
}

