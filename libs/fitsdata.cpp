#include <QtCore>
//#include <QtGui>
#include "fitsdata.h"
#include <iostream>
//#include "..\libs\astro.h"
//#include "./../libs/mb.h"
using namespace std;
/*
double magEqCorr(double mag, double x, QList <double*> mCoefX, int mDeg)
{
    int i, j;
    int xDeg = mCoefX.size();
    double *Zx = new double[xDeg];
    double res;

    res = 0;
    for(i=0; i<xDeg; i++)
    {
        Zx[i] = 0;
        for(j=0; j<mDeg; j++)
        {
            Zx[i] += mCoefX[i][j]*pow(mag, j);
        }
        res += Zx[i]*pow(x, i);
    }
    qDebug() << QString("magEqCorr res: %1\n").arg(res, 12, 'g');
    return res;
}


double comaCorr(double x, double mag, double c, double mag0)
{
    return(c*x*(mag-mag0));
}
*/

void getAperture(unsigned short *fd, double *const ap, int cx, int cy, int rho, int width, int height)
{
        int ulX = cx-rho;
        int ulY = cy-rho;
        double r;
        for(int y = 0; y<2*rho;y++)
                for(int x = 0; x<2*rho;x++)
                {
                        r = sqrt(pow(x-rho,2)+pow(y-rho,2));
                        if((ulX+x>=0)&&(ulX+x<width)&&(ulY+y>=0)&&(ulY+y<height)&&(r<=rho))
                                ap[y*2*rho+x] = (double)fd[(ulY+y)*width+(ulX+x)];
                        else ap[y*2*rho+x] = -10E-9;
                }
}

void getRing(unsigned short *fd, double *const ap, int cx, int cy, int rho1, int rho2, int width, int height)
{
        int ulX = cx-rho2;
        int ulY = cy-rho2;
        double r;
        for(int y = 0; y<2*rho2;y++)
                for(int x = 0; x<2*rho2;x++)
                {
                        r = sqrt(pow(x-rho2,2)+pow(y-rho2,2));
                        if((ulX+x>=0)&&(ulX+x<width)&&(ulY+y>=0)&&(ulY+y<height)&&(r>=rho1)&&(r<=rho2))
                                ap[y*2*rho2+x] = (double)fd[(ulY+y)*width+(ulX+x)];
                        else
                                ap[y*2*rho2+x] = -10E-9;
                }
}

bool getStimp(double *P, unsigned short *fd, int width, int height, double fpx, int x, int y, measureParam params)
{
    return(getStimp(P, fd, width, height, fpx, x, y, params.aperture, params.ringradius, params.ringwidth, params.sg, params.lb, params.model, params.nofit, params.delta));
}


bool getStimp(double *P, unsigned short *fd, int width, int height, double fpx, int x, int y, int apr, int rr, int rw, int sg, int lb, int model, int& nofit, double delta)
/* this function return true and PSF parameters of the stellar object image or false if fitting is impossible
P - initialised vector of PSF parameters (size = 21)
fd - pointer to fitsfile data
width,heingt - size of ccd image
fpx - most frequent pixel of ccd image
x,y - pixel coordinates of the center of the stellar object image
apr - aperture radius (in pixels)
rr - ring internal radius (in pixels)
rw - ring width (in pixels)
sg = 1 if subtraction of gradient is requared. overwise sg=0
lb = 1 if fpix must be calculated within the ring around aperture as mean pixel value, overwise lb =0
model = {0,1,2} - model of PSF (see description of psffit function)
nofit - number of iteration (see description of psffit function)
delta - order of the model (see description of psffit function)
*/
{
    if((x>=0)&&(y>=0)&&(x<width)&&(y<height))
    {
        //BEGIN get data arrays
        double *ringdata = new double[4*(rr + rw)*(rr + rw)];
        getRing(fd, ringdata, x, y,rr , rr + rw, width, height);
        double *apdata = new double[4*apr*apr];
        getAperture(fd, apdata, x, y,apr,width,height);
        //END get data arrays
        if(sg==1) //BEGIN fit and subtract gradient
        {
            double *rP = new double[6];
            if(ringfit(rP, ringdata,  rr + rw))
                subtractSurface(rP, apdata, fpx, apr);
            else
                {delete[]ringdata;delete[]apdata;return false;}
        } //END fit and subtract gradient
        else if(lb) fpx = meanRingValue(ringdata, rr+rw);
        delete[]ringdata;
        //BEGIN psf psffit(p,fd,rho,model,25,0.05,0.005,5,n_it,1);
        P[2] = fpx;P[7]=delta;
        if(psffit(P, apdata, apr, model, 25, 0.05, 0.005, 5, nofit, 1))
            {delete[] apdata;P[0]=x-apr+P[0];P[1]=y-apr+P[1];return true;}
        else
            {delete[] apdata;return false;}
        //END psf
    }
    else return false;
}


void getCatName(QString *cname, int ctype)
{
	switch(ctype)
	{
                case UCAC3_CAT_NUM:
                cname->insert(0, "ucac3");
		break;
                case USNOB_CAT_NUM:
		cname->insert(0, "usnob");
                break;
                case LSPMFIND_CAT_NUM:
                cname->insert(0, "lspm");
		break;
	}
}

void marksDist(marksP *m1, marksP *m2, marksP *mRes)
{
    mRes->mEkv[0] = m1->mEkv[0] - m2->mEkv[0];
    mRes->mEkv[0] = m1->mEkv[1] - m2->mEkv[1];
    mRes->mEkv[0] = m1->mEkv[2] - m2->mEkv[2];
    mRes->mTan[0] = m1->mTan[0] - m2->mTan[0];
    mRes->mTan[1] = m1->mTan[1] - m2->mTan[1];
    mRes->mTanImg[0] = m1->mTanImg[0] - m2->mTanImg[0];
    mRes->mTanImg[1] = m1->mTanImg[1] - m2->mTanImg[1];
    mRes->mTanImg[2] = m1->mTanImg[2] - m2->mTanImg[2];
}

double marksImgDist(marksP *m1, marksP *m2)
{
    return(sqrt(pow(m1->mTanImg[0] - m2->mTanImg[0], 2.0) + pow(m1->mTanImg[1] - m2->mTanImg[1], 2.0)));
}


void detOCgrid(marksGrid *mG, double *ra_oc, double *de_oc)
{
    double s0, s1;
    int sz, i;

    sz = mG->marks.size();
    s0 = s1 = 0.0;
    for(i=0; i<sz; i++)
    {
        s0 += mG->marks.at(i)->mEkv[0];
        s1 += mG->marks.at(i)->mEkv[1];
    }
    *ra_oc = s0/sz;
    *de_oc = s1/sz;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

double detFi(double x)
{
    double u = 1.0 - 7.5/(fabs(x) + 3.75);
    double y = (((((((((((((((3.328130055126039e-10*u - 5.718639670776992e-10)*u - 4.066088879757269e-9)*u + 7.532536116142436e-9)*u + 3.026547320064576e-8)*u - 7.043998994397452e-8)*u - 1.822565715362025e-7)*u + 6.575825478226343e-7)*u + 7.478317101785790e-7)*u - 6.182369348098529e-6)*u + 3.584014089915968e-6)*u + 4.78983822669598e-5)*u - 1.524627476123466e-4)*u - 2.553523453642242e-5)*u + 1.802962431316418e-3)*u - 8.220621168415435e-3)*u + 2.414322397093253e-2;
    double vi = (((((y*u - 5.480232669380236e-2)*u + 1.026043120322792e-1)*u - 1.635718955239687e-1)*u + 2.260080669166197e-1)*u - 2.734219314954260e-1)*u + 1.455897212750385e-1;
    return(vi*sqrt(PI)/2.0);
}

double detRo(double Z, double t, double k)
{
    double R = 8314.41;
    double T = t + 273;
    double Md = 28.9645;
    double g0 = 9.80665;
    double Rz = 6371000;

    double beta = (R*T)/(g0*Md*Rz);
    double alfa = beta - k/2.0;
    double x = cos(Z)/sqrt(2.0*alfa);
    return(k*(1.0 - k/2.0)*sqrt(2.0/alfa)*sin(Z)*detFi(x));
}

double detRo1(double Z, double T, double k)
{
    double R = 8314.41;
//    double T = t + 273;
    double Md = 28.9645;
    double g0 = 9.80665;
    double Rz = 6371000;

    double beta = (R*T)/(g0*Md*Rz);
    double alfa = beta - k/2.0;
    double x = cos(Z)/sqrt(2.0*alfa);
    return(k*(1.0 - k/2.0)*sqrt(2.0/alfa)*detFi(x));
}

double detPsi(double t)
{
    return(-5.32917 + t*(0.0688825 + t*(-2.9815e-4 + 1.39e-6*t)));
}

double detKa(double lam, double vi, double t, double press)
{
    double R = 8314.41;
    double Mw = 18.0152;
    double Rw = R/Mw;
    double Pw = vi*Rw*exp(detPsi(t));
    double P = (press/750.06)*100.0;
    double Pd = (P - Pw);
    double T = t + 273.0;
    qDebug() << QString("Pd= %1\tPw= %2\tP= %3\n").arg(Pd).arg(Pw).arg(P);
    double Dd = (Pd/T)*(1.0 + Pd*(57.90e-8 - 9.3250e-4/T + 0.25844/(T*T)));
    double Dw = (Pw/T)*(1.0 + Pw*(1.0 + 3.7e-4*Pw)*(-2.37321e-3 + 2.23366/T - 710.792/(T*T) + 7.75141e-4/(T*T*T)));
    qDebug() << QString("Dd= %1\tDw= %2\n").arg(Dd).arg(Dw);
    double Ka = ((2371.34 + 683939.7/(130 - lam*lam) + 4547.3/(38.9 - lam*lam))*Dd + (6487.31 + 58.058/(lam*lam) - 0.71150/pow(lam, 4.0) + 0.08851/pow(lam, 6.0))*Dw)*1.0e-8;
    qDebug() << QString("Ka= %1\n").arg(Ka);
    return(Ka);
}

refractionMaker::refractionMaker(refractionParam refParam)
{
    qDebug() << "\nrefractionMaker init\n";

    int i, j;
    double a, b;

    Fi = refParam.Fi;
    Long = refParam.Long;

    A0 = grad2rad(refParam.ra0);
    D0 = grad2rad(refParam.de0);

  //  qDebug() << QString("A0: %1\tD0: %2\n").arg(A0).arg(D0);


    a = 2.871e-04;
    b = 0.00567;
    T = refParam.temp + 273;

    double k0 = a*(1.0 + (b/pow(refParam.lam, 2.0)));
    qDebug() << QString("k0= %1\n").arg(k0);
    n1 = k0*(refParam.press/760.0)*(273.0/T);
    qDebug() << QString("n1= %1\n").arg(n1);

    ka1 = detKa(refParam.lam, refParam.vi, refParam.temp, refParam.press)*10;

    //

    qDebug() << QString("utc: %1\n").arg(refParam.utc);

    double gm1;
    jdUT1_to_GMST1(&gm1, mjd2jd(refParam.utc));//pady
    //UTC2s(mjd2jd(refParam.utc), refParam.Long, &s);
    s = 2.0*PI*gm1 + Long;

    //s /= 2.0*PI;

    t = s - A0;

    qDebug() << QString("gm1= %1\tLong= %2\ts= %3\tt= %4\n").arg(gm1*24.0).arg(Long).arg(s).arg(t);

    //



    qDebug() << QString("A0= %1\tD0= %2\n").arg(A0).arg(D0);

    rT0 = new double[3];

    rT0[0] = cos(A0)*cos(D0);
    rT0[1] = sin(A0)*cos(D0);
    rT0[2] = sin(D0);

    qDebug() << QString("rT0: %1\t%2\t%3\n").arg(rT0[0]).arg(rT0[1]).arg(rT0[2]);

    rA = new double[3];

    qDebug() << QString("fi= %1\n").arg(refParam.Fi);

    rA[0] = cos(s)*cos(refParam.Fi);
    rA[1] = sin(s)*cos(refParam.Fi);
    rA[2] = sin(refParam.Fi);

    qDebug() << QString("rA: %1\t%2\t%3\n").arg(rA[0]).arg(rA[1]).arg(rA[2]);



    double Z = VectAng(rA, rT0);
    //n1 *= tan(Z);
    double dZ =detRo(Z, T, n1);
    qDebug() << QString("Z= %1\tdZ= %2\n").arg(rad2grad(Z)).arg(dZ);

    forvRef(&A1, &D1, refParam.ra0, refParam.de0);
    dA = grad_to_rad(A1) - A0;
    dD = grad_to_rad(D1) - D0;

    qDebug() << QString("A1: %1\tD1: %2\n").arg(grad_to_rad(A1)).arg(grad_to_rad(D1));
    qDebug() << QString("dA: %1\tdD: %2\n").arg(dA).arg(dD);
    qDebug() << QString("dA: %1\tdD: %2\n").arg(rad2mas(dA)).arg(rad2mas(dD));

    qDebug() << "\nrefractionMaker init end\n\n";

}

refractionMaker::~refractionMaker()
{
    delete [] aij;
    delete [] ksi0;
    delete [] eta0;
    delete [] zeta0;
    delete [] ksi1;
    delete [] eta1;
    delete [] zeta1;
    delete [] rT0;
    delete [] rT1;
    delete [] rA;
}

void refractionMaker::forvRef(double *ra1, double *dec1, double ra0, double dec0)
{
    QFile refFile("./refFile.txt");
    refFile.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);
    QTextStream refStream(&refFile);
    ra0 = grad_to_rad(ra0);
    dec0 = grad_to_rad(dec0);

    //qDebug() << QString("ra= %1\tdec = %2\n").arg(ra).arg*dec);
    //qDebug() << QString("ra0= %1\tdec0= %2\n").arg(ra0).arg(dec0);

    this->t = this->s - ra0;
    //if(t<0) t += 1.0;

    //qDebug() << QString("s= %1\tt= %2\n").arg(s).arg(t);

    double P1, P2, P3;
    double Az, Zed;
    double Az1, Zed1;

    P1 = sin(dec0)*sin(Fi) + cos(dec0)*cos(Fi)*cos(t);
    P2 = cos(dec0)*sin(t);
    P3 = -sin(dec0)*cos(Fi) + cos(dec0)*sin(Fi)*cos(t);


    Zed = acos(P1);
    Az = atan2(P2/sin(Zed), P3/sin(Zed));
    //qDebug() << QString("Az= %1\tZed = %2\n").arg(rad2grad(Az)).arg(rad2grad(Zed));


    double dRa, dDec;
    double dRa0, dDec0;
    double cosd = cos(dec0);
    double cosZ = P1;

    double k1 = detRo1(Zed, T, n1);

    //qDebug() << QString("k1= %1\n").arg(k1);
    dDec0 = k1*(cos(dec0)*sin(Fi) - sin(dec0)*cos(Fi)*cos(t));
    dRa0 = k1*cos(Fi)*sin(t)/cosd;
    dDec = n1*(cos(dec0)*sin(Fi) - sin(dec0)*cos(Fi)*cos(t))/cosZ;
    dRa = n1*(cos(Fi)*sin(t))/cosZ/cosd;
/*
    qDebug() << QString("dRa0= %1\tdDec0 = %2\n").arg(dRa0).arg(dDec0);
    qDebug() << QString("dRa= %1\tdDec = %2\n").arg(dRa).arg(dDec);
    qDebug() << QString("dRa0= %1\tdDec0 = %2\n").arg(rad2mas(dRa0)).arg(rad2mas(dDec0));
    qDebug() << QString("dRa= %1\tdDec = %2\n").arg(rad2mas(dRa)).arg(rad2mas(dDec));
    qDebug() << QString("ddRa= %1\tddDec = %2\n").arg(rad2mas(dRa-dRa0)).arg(rad2mas(dDec-dDec0));
*/
 //   refStream << QString("%1|%2|%3|%4\n").arg(rad2grad(Zed)).arg(rad2mas(sqrt(dRa*dRa*cosd*cosd + dDec*dDec))).arg(t).arg(rad2mas(dRa));

    *ra1 = rad2grad(ra0 - dRa);
    *dec1 = rad2grad(dec0 - dDec);

    double ra = ra0 - dRa;
    double dec = dec0 - dDec;
    double t1 = s - ra;

    P1 = sin(dec)*sin(Fi) + cos(dec)*cos(Fi)*cos(t1);
    P2 = cos(dec)*sin(t1);
    P3 = -sin(dec)*cos(Fi) + cos(dec)*sin(Fi)*cos(t1);

    Zed1 = acos(P1);
    Az1 = atan2(P2/sin(Zed), P3/sin(Zed));
  //  qDebug() << QString("Az1= %1\tZed1 = %2\n").arg(rad2grad(Az1)).arg(rad2grad(Zed1));

    refStream << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9\n").arg(rad2grad(Zed)).arg(rad2grad(Zed1)).arg(rad2mas(Zed1-Zed)).arg(rad2mas(sqrt(dRa0*dRa0*cosd*cosd + dDec0*dDec0))/1000.0).arg(rad2mas(sqrt(dRa*dRa*cosd*cosd + dDec*dDec))/1000.0).arg(t).arg(rad2mas(dRa0)).arg(rad2grad(Az)).arg(rad2grad(Az1));
//    refStream << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(rad2grad(Zed)).arg(rad2grad(Zed1)).arg(rad2mas(Zed1-Zed)).arg(rad2mas(sqrt(dRa0*dRa0*cosd*cosd + dDec0*dDec0))).arg(t).arg(rad2mas(dRa0)).arg(rad2grad(Az)).arg(rad2grad(Az1));
//    qDebug() << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(rad2grad(Zed)).arg(rad2grad(Zed1)).arg(rad2mas(Zed1-Zed)).arg(rad2mas(sqrt(dRa0*dRa0*cosd*cosd + dDec0*dDec0))).arg(t).arg(rad2mas(dRa0)).arg(rad2grad(Az)).arg(rad2grad(Az1));

    refFile.close();
}

void refractionMaker::forvRefRel(double *ra1, double *dec1, double ra0, double dec0)
{
    QFile refFile("./refFile.txt");
    refFile.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);
    QTextStream refStream(&refFile);
    qDebug() << QString("ra0= %1\tdec0= %2\n").arg(ra0).arg(dec0);
    ra0 = grad_to_rad(ra0);
    dec0 = grad_to_rad(dec0);

    //qDebug() << QString("ra= %1\tdec = %2\n").arg(ra).arg*dec);

    t = s - ra0;

    double P1, P2, P3;
    double Az, Zed;
    double Az1, Zed1;

    P1 = sin(dec0)*sin(Fi) + cos(dec0)*cos(Fi)*cos(t);
    P2 = cos(dec0)*sin(t);
    P3 = -sin(dec0)*cos(Fi) + cos(dec0)*sin(Fi)*cos(t);

    qDebug() << QString("s= %1\tt= %2\n").arg(s).arg(t);
    Zed = acos(P1);
    Az = atan2(P2/sin(Zed), P3/sin(Zed));
    qDebug() << QString("Az= %1\tZed = %2\n").arg(rad2grad(Az)).arg(rad2grad(Zed));


    double dRa, dDec;
    double dRa0, dDec0;
    double cosd = cos(dec0);
    double cosZ = P1;

    double k1 = detRo1(Zed, T, n1);

    qDebug() << QString("k1= %1\n").arg(k1);
    dDec0 = k1*(cos(dec0)*sin(Fi) - sin(dec0)*cos(Fi)*cos(t));
    dRa0 = k1*cos(Fi)*sin(t)/cosd;
    dDec = n1*(cos(dec0)*sin(Fi) - sin(dec0)*cos(Fi)*cos(t))/cosZ;
    dRa = n1*(cos(Fi)*sin(t))/cosZ/cosd;



    qDebug() << QString("dRa0= %1\tdDec0 = %2\n").arg(dRa0).arg(dDec0);
    qDebug() << QString("dRa= %1\tdDec = %2\n").arg(dRa).arg(dDec);
    qDebug() << QString("dRa0= %1\tdDec0 = %2\n").arg(rad2mas(dRa0)).arg(rad2mas(dDec0));
    qDebug() << QString("dRa= %1\tdDec = %2\n").arg(rad2mas(dRa)).arg(rad2mas(dDec));
    qDebug() << QString("ddRa= %1\tddDec = %2\n").arg(rad2mas(dRa-dRa0)).arg(rad2mas(dDec-dDec0));
    qDebug() << QString("dA= %1\tdD = %2\n").arg(dA).arg(dD);
    qDebug() << QString("dA= %1\tdD = %2\n").arg(rad2mas(dA)).arg(rad2mas(dD));

    dDec0 -= dD;
    dRa0 -= dA;
    dDec -= dD;
    dRa -= dA;

    qDebug() << QString("dRa0= %1\tdDec0 = %2\n").arg(dRa0).arg(dDec0);
    qDebug() << QString("dRa= %1\tdDec = %2\n").arg(dRa).arg(dDec);
    qDebug() << QString("dRa0= %1\tdDec0 = %2\n").arg(rad2mas(dRa0)).arg(rad2mas(dDec0));
    qDebug() << QString("dRa= %1\tdDec = %2\n").arg(rad2mas(dRa)).arg(rad2mas(dDec));
    qDebug() << QString("ddRa= %1\tddDec = %2\n").arg(rad2mas(dRa-dRa0)).arg(rad2mas(dDec-dDec0));

 //   refStream << QString("%1|%2|%3|%4\n").arg(rad2grad(Zed)).arg(rad2mas(sqrt(dRa*dRa*cosd*cosd + dDec*dDec))).arg(t).arg(rad2mas(dRa));

    *ra1 = rad2grad(ra0 - dRa0);
    *dec1 = rad2grad(dec0 - dDec0);

    qDebug() << QString("ra1= %1\tdec1= %2\n").arg(*ra1).arg(*dec1);

    double ra = ra0 - dRa0;
    double dec = dec0 - dDec0;
    double t1 = s - ra;

    P1 = sin(dec)*sin(Fi) + cos(dec)*cos(Fi)*cos(t1);
    P2 = cos(dec)*sin(t1);
    P3 = -sin(dec)*cos(Fi) + cos(dec)*sin(Fi)*cos(t1);

    Zed1 = acos(P1);
    Az1 = atan2(P2/sin(Zed), P3/sin(Zed));
    qDebug() << QString("Az1= %1\tZed1 = %2\n").arg(rad2grad(Az1)).arg(rad2grad(Zed1));

    refStream << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9\n").arg(rad2grad(Zed)).arg(rad2grad(Zed1)).arg(rad2mas(Zed1-Zed)).arg(rad2mas(sqrt(dRa0*dRa0*cosd*cosd + dDec0*dDec0))).arg(rad2mas(sqrt(dRa*dRa*cosd*cosd + dDec*dDec))).arg(t).arg(rad2mas(dRa0)).arg(rad2grad(Az)).arg(rad2grad(Az1));
//    refStream << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(rad2grad(Zed)).arg(rad2grad(Zed1)).arg(rad2mas(Zed1-Zed)).arg(rad2mas(sqrt(dRa0*dRa0*cosd*cosd + dDec0*dDec0))).arg(t).arg(rad2mas(dRa0)).arg(rad2grad(Az)).arg(rad2grad(Az1));
//    qDebug() << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(rad2grad(Zed)).arg(rad2grad(Zed1)).arg(rad2mas(Zed1-Zed)).arg(rad2mas(sqrt(dRa0*dRa0*cosd*cosd + dDec0*dDec0))).arg(t).arg(rad2mas(dRa0)).arg(rad2grad(Az)).arg(rad2grad(Az1));

    refFile.close();
}

void refractionMaker::backRef(double *ra0, double *dec0, double ra1, double dec1)
{
    ra1 = grad_to_rad(ra1);
    dec1 = grad_to_rad(dec1);

    //qDebug() << QString("ra= %1\tdec = %2\n").arg(ra).arg*dec);
    qDebug() << QString("ra0= %1\tdec0= %2\n").arg(ra1).arg(dec1);

    t = s - ra1;

    qDebug() << QString("s= %1\tt= %2\n").arg(s).arg(t);

    double dRa, dDec;
    double cosd = cos(dec1);

    dDec = n1*(cos(dec1)*sin(Fi) - sin(dec1)*cos(Fi)*cos(t))/(sin(dec1)*sin(Fi) + cos(dec1)*cos(Fi)*cos(t));
    dRa = n1*(cos(Fi)*sin(t))/(sin(dec1) + cos(dec1)*cos(Fi)*cos(t));

    qDebug() << QString("dRa= %1\tdDec = %2\n").arg(dRa).arg(dDec);
    qDebug() << QString("dRa= %1\tdDec = %2\n").arg(rad2mas(dRa)).arg(rad2mas(dDec));

    *ra0 = rad2grad(ra1 - dRa/cosd);
    *dec0 = rad2grad(dec1 - dDec);
}
/*
double refractionMaker::forvRef(double *ksi1, double *eta1, double ksi0, double eta0)
{
    double dKsi, dEta;
    ksi0 = grad2rad(ksi0);
    eta0 = grad2rad(eta0);
    //qDebug() << QString("n1= %1\tksi0= %2\teta0= %3\ttan(D0)= %4\n").arg(n1).arg(ksi0).arg(eta0).arg(tan(D0));
    dKsi = n1*(-(1.0 + ksiA0*ksiA0)*ksi0 - (etaA0 - tan(D0))*ksiA0*eta0 + ksiA0*((1.0 + ksiA0*ksiA0)*ksi0*ksi0 + 2.0*ksiA0*etaA0*ksi0*eta0 + (1.0 + etaA0*etaA0)*eta0*eta0));
    dEta = n1*(-(etaA0 + tan(D0))*ksiA0*ksi0 - (1.0 + etaA0*etaA0)*eta0 + etaA0*((1.0 + ksiA0*ksiA0)*ksi0*ksi0 + 2.0*ksiA0*etaA0*ksi0*eta0 + (1.0 + etaA0*etaA0)*eta0*eta0));

//    dKsi =

    qDebug() << QString("dKsi0= %1\tdEta0= %2\n").arg(rad_to_mas(dKsi)).arg(rad_to_mas(dEta));

    *ksi1 = rad2grad(ksi0 + dKsi);
    *eta1 = rad2grad(eta0 + dEta);
    /*
    double A, D;
    A = grad2rad(ra0);
    D = grad2rad(de0);

    double *r0 = new double[3];

    r0[0] = cos(A)*cos(D);
    r0[1] = sin(A)*cos(D);
    r0[2] = sin(D);

    double Z = VectAng(rA, r0);
    double dZ = n1*tan(Z);
    qDebug() << QString("Z= %1\tdZ= %2\n").arg(Z).arg(dZ);

    double ka0 = sin(dZ)/sin(Z-dZ);
    *ka1 = -sin(dZ)/sin(Z);
    //double m0 = sqrt(1.0 + kaT0*kaT0 + 2.0*kaT0*Smul3(r0, rA));
    double secP = sqrt(1.0 + ksi0*ksi0 + eta0*eta0);
    double zna = (aij[2]*ksi0 + aij[5]*eta0 + aij[8]) + ka0*secP*cosRoA*(aij[2]*ksiA + aij[5]*etaA + aij[8]);
    *ksi1 = (aij[0]*ksi0 + aij[1]*eta0 + aij[2]) + ka0*secP*cosRoA*(aij[0]*ksiA + aij[1]*etaA + aij[2]);
    *eta1 = (aij[3]*ksi0 + aij[4]*eta0 + aij[5]) + ka0*secP*cosRoA*(aij[3]*ksiA + aij[4]*etaA + aij[5]);

    *ksi1 /= zna;
    *eta1 /= zna;/
}

double refractionMaker::backRef(double *ksi0, double *eta0, double ksi1, double eta1)
{
    double dKsi, dEta;
    ksi1 = grad2rad(ksi1);
    eta1 = grad2rad(eta1);
    dKsi = n1*(-(1.0 + ksiA1*ksiA1)*ksi1 - (etaA1 - tan(D1))*ksiA1*eta1 + ksiA1*((1.0 + ksiA1*ksiA1)*ksi1*ksi1 + 2.0*ksiA1*etaA1*ksi1*eta1 + (1.0 + etaA1*etaA1)*eta1*eta1));
    dEta = n1*(-(etaA1 + tan(D1))*ksiA1*ksi1 - (1.0 + etaA1*etaA1)*eta1 + etaA1*((1.0 + ksiA1*ksiA1)*ksi1*ksi1 + 2.0*ksiA1*etaA1*ksi1*eta1 + (1.0 + etaA1*etaA1)*eta1*eta1));

    qDebug() << QString("dKsi1= %1\tdEta1= %2\n").arg(rad_to_mas(dKsi)).arg(rad_to_mas(dEta));

    *ksi0 = rad2grad(ksi1 - dKsi);
    *eta0 = rad2grad(eta1 - dEta);
    /*
    double cosRo1 = 1.0/sqrt(1.0 + ksi1*ksi1 + eta1*eta1);
    double secP1 = 1.0/cosRo1;
    double zna = (aij[6]*ksi1 + aij[7]*eta1 + aij[8]) + ka0*secP*cosRoA*(aij[6]*ksiA + aij[7]*etaA + aij[8]);
    *ksi0 = (aij[0]*ksi1 + aij[3]*eta1 + aij[6]) + ka1*secP*cosRoA*(aij[0]*ksiA + aij[3]*etaA + aij[6]);
    *eta0 = (aij[1]*ksi1 + aij[4]*eta1 + aij[7]) + ka1*secP*cosRoA*(aij[1]*ksiA + aij[4]*etaA + aij[7]);

    *ksi0 /= zna;
    *eta0 /= zna;/
}
*/
///////////////////////////////////////////////////////////

reductionMaker::reductionMaker()
{
    refMaker = NULL;
    useRefraction = 0;

    KSI = NULL;
    ETA = NULL;
    MAG = NULL;
    WEksi = NULL;
    WEeta = NULL;
    Cke = NULL;
    Cksi = NULL;
    Ceta = NULL;
    CMAG = NULL;
    EXCLINDKSI  = NULL;
    EXCLINDETA  = NULL;
    EXCLINDMAG  = NULL;
    ZKSI = NULL;
    DKSI = NULL;
    UWEKSI = 0.0;
    RNKSI = 0;
    ZETA = NULL;
    DETA = NULL;
    UWEETA = 0.0;
    RNETA = 0;
    ZMAG = NULL;
    DMAG = NULL;
    UWEMAG = 0.0;
    RNMAG = 0;
    reductionType = -1;


}

reductionMaker::reductionMaker(int redType)
{
    ::reductionMaker();
    reductionType =redType;
    /*switch(reductionType)
    {
    case 0:

        break;
    }*/
}

reductionMaker::~reductionMaker()
{
    //clear();
    /*
    delete [] KSI;
    delete [] ETA;
    delete [] MAG;
    delete [] WEksi;
    delete [] WEeta;
    delete [] C;
    delete [] Cksi;
    delete [] Ceta;
    delete [] CMAG;
    delete [] EXCLINDKSI;
    delete [] EXCLINDETA;
    delete [] EXCLINDMAG;
    delete [] ZKSI;
    delete [] DKSI;
    delete [] ZETA;
    delete [] DETA;
    delete [] ZMAG;
    delete [] DMAG;*/
}

int reductionMaker::makeReduction(QString &mTimeCode, marksGrid *refMarks, QVector<int> &rsindex, reductionParams params, refractionParam *refParam)
{
    int res;

    detTimeCode(mTimeCode);
  /*  if(refParam!=NULL)
    {
        useRefraction = 1;
        refMaker = new refractionMaker(*refParam);
    }
*/
    switch(reductionType)
        {
        case 0:
            res = make6const(refMarks, rsindex, params);
            break;
        case 1:
            res = make8const(refMarks, rsindex, params);
            break;
        default:
            res = 1;
            break;
        }

    return res;
}
/*
void initRefraction(refractionParam *refParam)
{
    double a, b;
    double s;

    a = 2.871e-04;
    b = 0.00567;

    double k0 = a*(1.0 + (b/pow(refParam.lam, 2.0)));
    qDebug() << QString("k0= %1\n").arg(k0);
    double n1 = k0*(refParam.press/760.0)*(273.0/(refParam.temp + 273.0));
    qDebug() << QString("n1= %1\n").arg(n1);
}
*/
int reductionMaker::make6const(marksGrid *refMarks, QVector<int> &rsindex, reductionParams params)
{
    qDebug() << "\n\nmake8const\n\n";
    double errksi,erreta;
    double minErrKsi, minErrEta;
    //double ksi, eta, ka1;
    int i, p;
    int recount = 1;
    double *data;
    marksP *mT;
    qDebug() << QString("params.minRefStars= %1").arg(params.minRefStars);
    int rsSize;

    ZKSI = NULL;
    DKSI = NULL;
    ZETA = NULL;
    DETA = NULL;
    ZMAG = NULL;
    DMAG = NULL;
    KSI = NULL;
    ETA = NULL;
    MAG = NULL;
    WEksi = NULL;
    WEeta = NULL;
    Cke = NULL;
    CMAG = NULL;
    EXCLINDKSI = NULL;
    EXCLINDETA = NULL;
    EXCLINDMAG = NULL;

    rsSize = rsindex.count();
    qDebug() << QString("rsindex.count: %1\n").arg(rsSize);
    do
    {
        qDebug() << "\nclear\n";

        if(ZKSI!=NULL) delete [] ZKSI;
        if(DKSI!=NULL) delete [] DKSI;
        if(ZETA!=NULL) delete [] ZETA;
        if(DETA!=NULL) delete [] DETA;
        if(ZMAG!=NULL) delete [] ZMAG;
        if(DMAG!=NULL) delete [] DMAG;
        if(KSI!=NULL) delete [] KSI;
        if(ETA!=NULL) delete [] ETA;
        if(MAG!=NULL) delete [] MAG;
        if(WEksi!=NULL) delete [] WEksi;
        if(WEeta!=NULL) delete [] WEeta;
        if(Cke!=NULL) delete [] Cke;
        if(CMAG!=NULL) delete [] CMAG;
        if(EXCLINDKSI!=NULL) delete [] EXCLINDKSI;
        if(EXCLINDETA!=NULL) delete [] EXCLINDETA;
        if(EXCLINDMAG!=NULL) delete [] EXCLINDMAG;

        //clear();
        recount = 0;
        minErrEta=minErrKsi=1000;
        KSI = new double [rsSize];
        ETA = new double [rsSize];
        MAG = new double [rsSize];
        WEksi  = new double [rsSize];
        WEeta  = new double [rsSize];
        Cke = new double [rsSize*3];
        CMAG = new double [rsSize*2];

    //////////////////////////////////////

        qDebug() << QString("ref star good num: %1\n").arg(rsSize);
        //qDebug() << "sdata: " << sdata << "\n";
        if(params.weights)
        {
            for (i=0;i<rsSize;i++)
            {
                mT = refMarks->marks.at(rsindex[i]);
                data = mT->data;

                errksi = data[9];
                erreta = data[10];


                if(errksi<minErrKsi) minErrKsi = errksi;
                if(erreta<minErrEta) minErrEta = erreta;
            }
            qDebug() << QString("minErrKsi: %1\tminErrEta: %2\n").arg(minErrKsi).arg(minErrEta);
        }
    //////////////////////////////////////
        p=0;
        for(i=0;i<rsSize;i++)
        {
            mT = refMarks->marks.at(rsindex[i]);
            data = mT->data;

                //ksi = data[3];
                //eta = data[4];

            KSI[i] = data[3];
            ETA[i] = data[4];
            MAG[i] = data[2];
            Cke[i*3+0] = data[5];
            Cke[i*3+1] = data[6];
            Cke[i*3+2] = 1;


            if(isfinite(data[7]))
            {
                CMAG[p*2+0] = data[7];
                CMAG[p*2+1] = 1;
                p++;
            }

            //qDebug() << QString("mag: %1 = Am*%2 + Bm*%3\n").arg(MAG[i]).arg(CMAG[i*2+0]).arg(CMAG[i*2+1]);
            errksi = data[9];
            erreta = data[10];

            if(params.weights)
            {
                WEksi[i]=pow(minErrKsi/errksi,2);
                WEeta[i]=pow(minErrEta/erreta,2);
            }
            else
            {
                WEksi[i]=1;
                WEeta[i]=1;
            }
        }
        EXCLINDKSI = new int[rsSize];
        ZKSI=new double[3];
        DKSI=new double[3*3];
        RNKSI=0;
        iLSM(3, rsSize, mas_to_grad(params.maxres), EXCLINDKSI, ZKSI, Cke, KSI, UWEKSI, DKSI, params.sigma, RNKSI, WEksi);
        qDebug() << QString("Zx: %1\t%2\t%3\n").arg(ZKSI[0]).arg(ZKSI[1]).arg(ZKSI[2]);
        ZETA=new double[3];
        DETA=new double[3*3];
        EXCLINDETA = new int [rsSize];
        RNETA=0;
        iLSM(3, rsSize, mas_to_grad(params.maxres), EXCLINDETA, ZETA, Cke, ETA, UWEETA, DETA, params.sigma, RNETA, WEeta);
        qDebug() << QString("Zy: %1\t%2\t%3\n").arg(ZETA[0]).arg(ZETA[1]).arg(ZETA[2]);
        ZMAG=new double[2];
        DMAG=new double[2*2];
        EXCLINDMAG = new int [rsSize];
        RNMAG=0;
        iLSM(2, p, params.maxresMAG, EXCLINDMAG, ZMAG, CMAG, MAG, UWEMAG, DMAG, params.sigma, RNMAG);
        qDebug() << QString("Zm: %1\t%2\n").arg(ZMAG[0]).arg(ZMAG[1]);

        detSmax6const(rsindex);
        qDebug() << QString("Sx= %1\tSy= %2\tsMax= %3\n").arg(grad_to_mas(Sx)).arg(grad_to_mas(Sy)).arg(params.sMax);
        qDebug() << QString("UWE: %1\t%2\t%3\n").arg(grad_to_mas(sqrt(UWEKSI))).arg(grad_to_mas(sqrt(UWEETA))).arg(sqrt(UWEMAG));
        qDebug() << QString("RN: %1\t%2\t%3\n").arg(RNKSI).arg(RNETA).arg(RNMAG);

        if((RNKSI<params.minRefStars)||(RNETA<params.minRefStars)) return 1;
        if((grad_to_mas(sqrt(UWEKSI))>params.uweMax)||(Sx>mas_to_grad(params.sMax))||(grad_to_mas(sqrt(UWEETA))>params.uweMax)||(Sy>mas_to_grad(params.sMax)))
        {
            recount = 1;
        }

        if(!recount&&(rsSize>=params.maxRefStars)&&(params.maxRefStars>0)) recount = 1;


        if(recount)
        {
            if(errKsiMax>errEtaMax) rsindex.remove(posKsiMax);
            else rsindex.remove(posEtaMax);
        }

        rsSize = rsindex.count();

    }while((rsSize>=params.minRefStars)&&recount);
    return 0;
}
/*
void reductionMaker::clear()
{
    qDebug() << "\nclear\n";
    if(ZKSI!=NULL) delete [] ZKSI;
    if(DKSI!=NULL) delete [] DKSI;
    if(ZETA!=NULL) delete [] ZETA;
    if(DETA!=NULL) delete [] DETA;
    if(ZMAG!=NULL) delete [] ZMAG;
    if(DMAG!=NULL) delete [] DMAG;
    if(KSI!=NULL) delete [] KSI;
    if(ETA!=NULL) delete [] ETA;
    if(MAG!=NULL) delete [] MAG;
    if(WEksi!=NULL) delete [] WEksi;
    if(WEeta!=NULL) delete [] WEeta;
    if(Cke!=NULL) delete [] Cke;
    if(Cksi!=NULL) delete [] Cksi;
    if(Ceta!=NULL) delete [] Ceta;
    if(CMAG!=NULL) delete [] CMAG;
    if(EXCLINDKSI!=NULL) delete [] EXCLINDKSI;
    if(EXCLINDETA!=NULL) delete [] EXCLINDETA;
    if(EXCLINDMAG!=NULL) delete [] EXCLINDMAG;

    ZKSI = NULL;
    DKSI = NULL;
    ZETA = NULL;
    DETA = NULL;
    ZMAG = NULL;
    DMAG = NULL;
    KSI = NULL;
    ETA = NULL;
    MAG = NULL;
    WEksi = NULL;
    WEeta = NULL;
    Cke = NULL;
    Cksi = NULL;
    Ceta = NULL;
    CMAG = NULL;
    EXCLINDKSI = NULL;
    EXCLINDETA = NULL;
    EXCLINDMAG = NULL;
}
*/
void reductionMaker::detSmax6const(QVector<int> &rsindex)
{
    int i, j;
    posKsiMax = -1;
    posEtaMax = -1;
    Sx = Sy = 0.0;
    double errKsi, errEta;

    errEtaMax = errKsiMax = 0.0;

    int pNum = rsindex.count();

    double pixKsi, pixEta;
    int rNumX = 0;
    int rNumY = 0;

    for(i=0; i<pNum; i++)
    {
        j = rsindex.at(i);
        pixKsi = ZKSI[0]*Cke[i*3+0]+ZKSI[1]*Cke[i*3+1]+ZKSI[2];
        pixEta = ZETA[0]*Cke[i*3+0]+ZETA[1]*Cke[i*3+1]+ZETA[2];

        //if(RNKSI[i])
        //qDebug() << QString("dKsi= %1\tdEta= %2\n").arg(KSI[i] - pixKsi).arg(ETA[i] - pixEta);
        Sx=Sx+pow(KSI[i] - pixKsi,2);
        Sy=Sy+pow(ETA[i] - pixEta,2);

        errKsi = fabs(KSI[i] - pixKsi);
        errEta = fabs(ETA[i] - pixEta);
       // qDebug() << "\n" << QString("pixKsi= %1\tpixEta= %2\terrKsi= %3\terrEta= %4\n").arg(pixKsi).arg(pixEta).arg(grad_to_mas(errKsi), 10, 'f', 7).arg(grad_to_mas(errEta), 10, 'f', 7);
  //      qDebug() << "\n" << QString("ksi= %1\teta= %2\terr= %3\n").arg(KSI[j]).arg(ETA[j]);
        if(errKsi>errKsiMax)
        {
            posKsiMax = i;
            errKsiMax = errKsi;
        }
        if(errEta>errEtaMax)
        {
            posEtaMax = i;
            errEtaMax = errEta;
        }
    }
    Sx = sqrt(Sx/(pNum-1));
    Sy = sqrt(Sy/(pNum-1));
}

void reductionMaker::getCenterPos6const(double *Xoc, double *Yoc)
{
    double *mCD = new double[4];
    double *ImCD = new double[4];
    double *rPix = new double[2];
    double *shC = new double[2];
    int singulAR;

    mCD[0] = ZKSI[0]; mCD[1] = ZKSI[1];
    mCD[2] = ZETA[0]; mCD[3] = ZETA[1];
    shC[0]=-ZKSI[2];shC[1]=-ZETA[2];

    singulAR *= invgj(ImCD, mCD, 2);
    qDebug() << "\ninvgj res = " <<  singulAR << "\n";
    singulAR *= mvprod(rPix, ImCD, shC, 2, 2);
    qDebug() << "\nmvprod res = " <<  singulAR << "\n";
/*
    qDebug() << "ZKSI:" << ZKSI << "\n";
    qDebug() << "ZETA:" << ZETA << "\n";
    qDebug() << "ZKSI[0]:" << ZKSI[0] << "\n";
    qDebug() << "ZKSI[1]:" << ZKSI[1] << "\n";
    if(fabs(ZKSI[0])>fabs(ZKSI[1]))
    {
        *Yoc = (ZETA[2] - (ZETA[0]/ZKSI[0])*ZKSI[2])/(ZETA[1] - (ZETA[0]/ZKSI[0])*ZKSI[1]);
        *Xoc = -(ZKSI[1]/ZKSI[0])*(*Yoc) - ZKSI[2]/ZKSI[0];
    }
    else
    {
        *Xoc = ((ZETA[1]/ZKSI[1])*ZKSI[2] - ZETA[2])/(ZETA[0] - (ZETA[1]/ZKSI[1])*ZKSI[0]);
        *Yoc = -(ZKSI[0]/ZKSI[1])*(*Yoc) - ZKSI[2]/ZKSI[1];
    }
*/
   *Xoc = rPix[0];
   *Yoc = rPix[1];
    delete [] mCD;
    delete [] ImCD;
    delete [] rPix;
    delete [] shC;
}

void reductionMaker::makeErrorBudget(errBudgetRec *errBud)
{
    errBud->redType = reductionType;

    getCenterPos(&errBud->Xoc, &errBud->Yoc);
    getParams(errBud);
}

void reductionMaker::getParams6const(errBudgetRec *errBud)
{
    errBud->xParams.set3params(RNKSI, grad_to_mas(sqrt(UWEKSI)), ZKSI[0], sqrt(DKSI[0]), ZKSI[1], sqrt(DKSI[4]), ZKSI[2], sqrt(DKSI[8]));
    errBud->yParams.set3params(RNETA, grad_to_mas(sqrt(UWEETA)), ZETA[0], sqrt(DETA[0]), ZETA[1], sqrt(DETA[4]), ZETA[2], sqrt(DETA[8]));
    errBud->mParams.set2params(RNMAG, sqrt(UWEMAG), ZMAG[0], sqrt(DMAG[0]), ZMAG[1], sqrt(DMAG[3]));
}

void reductionMaker::detTan(double *ksi, double *eta, double x, double y)
{
    switch(reductionType)
    {
    case 0:
        detTan6const(ksi, eta, x, y);
        break;
    case 1:
        detTan8const(ksi, eta, x, y);
        break;
    }
}

void reductionMaker::detTan6const(double *ksi, double *eta, double x, double y)
{
    *ksi = ZKSI[0]*x+ZKSI[1]*y+ZKSI[2];
    *eta = ZETA[0]*x+ZETA[1]*y+ZETA[2];

}

void reductionMaker::detMagn(double *mag, double pixmag)
{
    switch(reductionType)
    {
    case 0:
        detMagn6const(mag, pixmag);
        break;
    case 1:
        detMagn6const(mag, pixmag);
        break;
    }
}

void reductionMaker::detMagn6const(double *mag, double pixmag)
{
    *mag = ZMAG[0]*pixmag+ZMAG[1];
}

void reductionMaker::detXcYc(double *x, double *y, double ksi, double eta)
{
    switch(reductionType)
    {
    case 0:
        detXcYc6const(x, y, ksi, eta);
        break;
    case 1:
        detXcYc8const(x, y, ksi, eta);
        break;
    }
}

void reductionMaker::detXcYc6const(double *x, double *y, double ksi, double eta)
{
    *y = ((eta-ZETA[2])-ZETA[0]*(ksi-ZKSI[2])/ZKSI[0])/(ZETA[1]-ZETA[0]*ZKSI[1]/ZKSI[0]);
    *x = ((ksi-ZKSI[2])-ZKSI[1]*(*y))/ZKSI[0];
}

void reductionMaker::detPmag(double *pmag, double mag)
{
    switch(reductionType)
    {
    case 0:
        detPmag6const(pmag, mag);
        break;
    case 1:
        detPmag6const(pmag, mag);
        break;
    }
}

void reductionMaker::detPmag6const(double *pmag, double mag)
{
    *pmag = (mag-ZMAG[1])/ZMAG[0];
}

void reductionMaker::getCenterPos(double *Xoc, double *Yoc)
{
    qDebug() << "Xoc: " << Xoc << "\tYoc: " << Yoc << "\n";
    switch(reductionType)
    {
    case 0:
        getCenterPos6const(Xoc, Yoc);
        break;
    case 1:
        getCenterPos8const(Xoc, Yoc);
        break;
    }

    qDebug() << QString("Xoc= %1\tYoc= %2\n").arg(*Xoc).arg(*Yoc);
}

void reductionMaker::getParams(errBudgetRec *errBud)
{
    switch(reductionType)
    {
    case 0:
        getParams6const(errBud);
        break;
    case 1:
        getParams8const(errBud);
        break;
    }
}

//8 const

int reductionMaker::make8const(marksGrid *refMarks, QVector<int> &rsindex, reductionParams params)
{
    qDebug() << "\n\nmake8const\n\n";

    double errksi,erreta;
    double minErrKsi, minErrEta;
    //double ksi, eta, ka1;
    int i, p;
    int recount = 1;
    double *data;
    marksP *mT;
    qDebug() << QString("params.minRefStars= %1").arg(params.minRefStars);
    int rsSize;
    int redDeg = 5;

    ZKSI = NULL;
    DKSI = NULL;
    ZETA = NULL;
    DETA = NULL;
    ZMAG = NULL;
    DMAG = NULL;
    KSI = NULL;
    ETA = NULL;
    MAG = NULL;
    WEksi = NULL;
    WEeta = NULL;
    Cksi = NULL;
    Ceta = NULL;
    CMAG = NULL;
    EXCLINDKSI = NULL;
    EXCLINDETA = NULL;
    EXCLINDMAG = NULL;

    rsSize = rsindex.count();
    qDebug() << QString("rsindex.count: %1\n").arg(rsSize);
    do
    {
        if(ZKSI!=NULL) delete [] ZKSI;
        if(DKSI!=NULL) delete [] DKSI;
        if(ZETA!=NULL) delete [] ZETA;
        if(DETA!=NULL) delete [] DETA;
        if(ZMAG!=NULL) delete [] ZMAG;
        if(DMAG!=NULL) delete [] DMAG;
        if(KSI!=NULL) delete [] KSI;
        if(ETA!=NULL) delete [] ETA;
        if(MAG!=NULL) delete [] MAG;
        if(WEksi!=NULL) delete [] WEksi;
        if(WEeta!=NULL) delete [] WEeta;
        if(Cksi!=NULL) delete [] Cksi;
        if(Ceta!=NULL) delete [] Ceta;
        if(CMAG!=NULL) delete [] CMAG;
        if(EXCLINDKSI!=NULL) delete [] EXCLINDKSI;
        if(EXCLINDETA!=NULL) delete [] EXCLINDETA;
        if(EXCLINDMAG!=NULL) delete [] EXCLINDMAG;
        //clear();
        recount = 0;
        minErrEta=minErrKsi=1000;
        KSI = new double [rsSize];
        ETA = new double [rsSize];
        MAG = new double [rsSize];
        WEksi  = new double [rsSize];
        WEeta  = new double [rsSize];
        Cksi = new double [rsSize*redDeg];
        Ceta = new double [rsSize*redDeg];
        CMAG = new double [rsSize*2];

    //////////////////////////////////////

        qDebug() << QString("ref star good num: %1\n").arg(rsSize);
        //qDebug() << "sdata: " << sdata << "\n";
        if(params.weights)
        {
            for (i=0;i<rsSize;i++)
            {
                mT = refMarks->marks.at(rsindex[i]);
                data = mT->data;

                errksi = data[9];
                erreta = data[10];


                if(errksi<minErrKsi) minErrKsi = errksi;
                if(erreta<minErrEta) minErrEta = erreta;
            }
            qDebug() << QString("minErrKsi: %1\tminErrEta: %2\n").arg(minErrKsi).arg(minErrEta);
        }
    //////////////////////////////////////
        p=0;
        for(i=0;i<rsSize;i++)
        {
            mT = refMarks->marks.at(rsindex[i]);
            data = mT->data;

                //ksi = data[3];
                //eta = data[4];

            KSI[i] = data[3];
            ETA[i] = data[4];
            MAG[i] = data[2];
            Cksi[i*redDeg+0] = data[5];
            Cksi[i*redDeg+1] = data[6];
            Cksi[i*redDeg+2] = 1;
            Cksi[i*redDeg+3] = data[5]*data[5];
            Cksi[i*redDeg+4] = data[5]*data[6];

            Ceta[i*redDeg+0] = data[5];
            Ceta[i*redDeg+1] = data[6];
            Ceta[i*redDeg+2] = 1;
            Ceta[i*redDeg+3] = data[5]*data[6];
            Ceta[i*redDeg+4] = data[6]*data[6];


            if(isfinite(data[7]))
            {
                CMAG[p*2+0] = data[7];
                CMAG[p*2+1] = 1;
                p++;
            }

            //qDebug() << QString("mag: %1 = Am*%2 + Bm*%3\n").arg(MAG[i]).arg(CMAG[i*2+0]).arg(CMAG[i*2+1]);
            errksi = data[9];
            erreta = data[10];

            if(params.weights)
            {
                WEksi[i]=pow(minErrKsi/errksi,2);
                WEeta[i]=pow(minErrEta/erreta,2);
            }
            else
            {
                WEksi[i]=1;
                WEeta[i]=1;
            }
        }
        EXCLINDKSI = new int[rsSize];
        ZKSI=new double[redDeg];
        DKSI=new double[redDeg*redDeg];
        RNKSI=0;
        iLSM(redDeg, rsSize, mas_to_grad(params.maxres), EXCLINDKSI, ZKSI, Cksi, KSI, UWEKSI, DKSI, params.sigma, RNKSI, WEksi);
        qDebug() << QString("Zx: %1\t%2\t%3\t%4\t%5\n").arg(ZKSI[0]).arg(ZKSI[1]).arg(ZKSI[2]).arg(ZKSI[3]).arg(ZKSI[4]);
        ZETA=new double[redDeg];
        DETA=new double[redDeg*redDeg];
        EXCLINDETA = new int [rsSize];
        RNETA=0;
        iLSM(redDeg, rsSize, mas_to_grad(params.maxres), EXCLINDETA, ZETA, Ceta, ETA, UWEETA, DETA, params.sigma, RNETA, WEeta);
        qDebug() << QString("Zy: %1\t%2\t%3\t%4\t%5\n").arg(ZETA[0]).arg(ZETA[1]).arg(ZETA[2]).arg(ZETA[3]).arg(ZETA[4]);
        ZMAG=new double[2];
        DMAG=new double[2*2];
        EXCLINDMAG = new int [rsSize];
        RNMAG=0;
        iLSM(2, p, params.maxresMAG, EXCLINDMAG, ZMAG, CMAG, MAG, UWEMAG, DMAG, params.sigma, RNMAG);
        qDebug() << QString("Zm: %1\t%2\n").arg(ZMAG[0]).arg(ZMAG[1]);

        detSmax8const(rsindex);
        qDebug() << QString("Sx= %1\tSy= %2\tsMax= %3\n").arg(grad_to_mas(Sx)).arg(grad_to_mas(Sy)).arg(params.sMax);
        qDebug() << QString("UWE: %1\t%2\t%3\n").arg(grad_to_mas(sqrt(UWEKSI))).arg(grad_to_mas(sqrt(UWEETA))).arg(sqrt(UWEMAG));
        qDebug() << QString("RN: %1\t%2\t%3\n").arg(RNKSI).arg(RNETA).arg(RNMAG);

        if((RNKSI<params.minRefStars)||(RNETA<params.minRefStars)) return 1;
        if((grad_to_mas(sqrt(UWEKSI))>params.uweMax)||(Sx>mas_to_grad(params.sMax))||(grad_to_mas(sqrt(UWEETA))>params.uweMax)||(Sy>mas_to_grad(params.sMax)))
        {
            recount = 1;
        }

        if(!recount&&(rsSize>=params.maxRefStars)&&(params.maxRefStars>0)) recount = 1;


        if(recount)
        {
            if(errKsiMax>errEtaMax) rsindex.remove(posKsiMax);
            else rsindex.remove(posEtaMax);
        }

        rsSize = rsindex.count();

    }while((rsSize>=params.minRefStars)&&recount);
    return 0;
}

void reductionMaker::getCenterPos8const(double *Xoc, double *Yoc)
{
    double *mCD = new double[4];
    double *ImCD = new double[4];
    double *rPix = new double[2];
    double *shC = new double[2];
    int singulAR;

    mCD[0] = ZKSI[0]; mCD[1] = ZKSI[1];
    mCD[2] = ZETA[0]; mCD[3] = ZETA[1];
    shC[0]=-ZKSI[2];shC[1]=-ZETA[2];

    singulAR *= invgj(ImCD, mCD, 2);
    qDebug() << "\ninvgj res = " <<  singulAR << "\n";
    singulAR *= mvprod(rPix, ImCD, shC, 2, 2);
    qDebug() << "\nmvprod res = " <<  singulAR << "\n";
/*
    qDebug() << "ZKSI:" << ZKSI << "\n";
    qDebug() << "ZETA:" << ZETA << "\n";
    qDebug() << "ZKSI[0]:" << ZKSI[0] << "\n";
    qDebug() << "ZKSI[1]:" << ZKSI[1] << "\n";
    if(fabs(ZKSI[0])>fabs(ZKSI[1]))
    {
        *Yoc = (ZETA[2] - (ZETA[0]/ZKSI[0])*ZKSI[2])/(ZETA[1] - (ZETA[0]/ZKSI[0])*ZKSI[1]);
        *Xoc = -(ZKSI[1]/ZKSI[0])*(*Yoc) - ZKSI[2]/ZKSI[0];
    }
    else
    {
        *Xoc = ((ZETA[1]/ZKSI[1])*ZKSI[2] - ZETA[2])/(ZETA[0] - (ZETA[1]/ZKSI[1])*ZKSI[0]);
        *Yoc = -(ZKSI[0]/ZKSI[1])*(*Yoc) - ZKSI[2]/ZKSI[1];
    }
*/
   *Xoc = rPix[0];
   *Yoc = rPix[1];
    delete [] mCD;
    delete [] ImCD;
    delete [] rPix;
    delete [] shC;
}

void reductionMaker::getParams8const(errBudgetRec *errBud)
{
    int i;
    double *pVect = new double[10];
    int pNum = 5;
    for(i=0; i<pNum; i++)
    {
        pVect[i*2] = ZKSI[i];
        pVect[i*2+1] = sqrt(DKSI[i*5+i]);
    }
    errBud->xParams.setNparams(RNKSI, grad_to_mas(sqrt(UWEKSI)), pVect, pNum);
    for(i=0; i<pNum; i++)
    {
        pVect[i*2] = ZETA[i];
        pVect[i*2+1] = sqrt(DETA[i*5+i]);
    }
    errBud->yParams.setNparams(RNETA, grad_to_mas(sqrt(UWEETA)), pVect, pNum);
    errBud->mParams.set2params(RNMAG, sqrt(UWEMAG), ZMAG[0], sqrt(DMAG[0]), ZMAG[1], sqrt(DMAG[3]));
}

void reductionMaker::detTan8const(double *ksi, double *eta, double x, double y)
{
    *ksi = ZKSI[0]*x+ZKSI[1]*y+ZKSI[2]+ZKSI[3]*x*x+ZKSI[4]*x*y;
    *eta = ZETA[0]*x+ZETA[1]*y+ZETA[2]+ZETA[3]*x*y+ZETA[4]*y*y;
}

void reductionMaker::detXcYc8const(double *x, double *y, double ksi, double eta)
{
    double c1, c2, b1, b2, a1, a2;

    c1 = ZKSI[2];
    c2 = ZETA[2];
    a1 = ZKSI[0] + (ZKSI[3]/ZKSI[0])*ksi;
    b1 = ZKSI[1] + (ZKSI[4]/ZKSI[0])*ksi;

    a2 = ZETA[0] + (ZETA[3]/ZETA[1])*eta;
    b2 = ZETA[1] + (ZETA[4]/ZETA[1])*eta;


    *y = ((eta-c2)-a2*(ksi-c1)/a1)/(b2-a2*b1/a1);
    *x = ((ksi-c1)-b1*(*y))/a1;
}

void reductionMaker::detSmax8const(QVector<int> &rsindex)
{
    int i, j;
    posKsiMax = -1;
    posEtaMax = -1;
    Sx = Sy = 0.0;
    double errKsiMax, errEtaMax, errKsi, errEta;

    errEtaMax = errKsiMax = 0.0;

    int pNum = rsindex.count();

    double pixKsi, pixEta;
    int rNumX = 0;
    int rNumY = 0;

    for(i=0; i<pNum; i++)
    {
        j = rsindex.at(i);
        //qDebug() << QString("x: %1\ty: %2\n").arg(Cksi[i*3+0]).arg(Cksi[i*3+1]);
        detTan8const(&pixKsi, &pixEta, Cksi[i*5+0], Cksi[i*5+1]);
        //pixKsi = ZKSI[0]*C[i*3+0]+ZKSI[1]*C[i*3+1]+ZKSI[2];
        //pixEta = ZETA[0]*C[i*3+0]+ZETA[1]*C[i*3+1]+ZETA[2];

        //if(RNKSI[i])
        //qDebug() << QString("dKsi= %1\tdEta= %2\n").arg(KSI[i] - pixKsi).arg(ETA[i] - pixEta);
        Sx=Sx+pow(KSI[i] - pixKsi,2);
        Sy=Sy+pow(ETA[i] - pixEta,2);

        errKsi = fabs(KSI[i] - pixKsi);
        errEta = fabs(ETA[i] - pixEta);
        //qDebug() << QString("pixKsi= %1\tpixEta= %2\terrKsi= %3\terrEta= %4\n").arg(pixKsi).arg(pixEta).arg(grad_to_mas(errKsi), 10, 'f', 7).arg(grad_to_mas(errEta), 10, 'f', 7);
        //qDebug() << QString("ksi= %1\teta= %2\n\n").arg(KSI[j]).arg(ETA[j]);
        if(errKsi>errKsiMax)
        {
            posKsiMax = i;
            errKsiMax = errKsi;
        }
        if(errEta>errEtaMax)
        {
            posEtaMax = i;
            errEtaMax = errEta;
        }
    }
    Sx = sqrt(Sx/(pNum-1));
    Sy = sqrt(Sy/(pNum-1));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HeadList::HeadList()
{

}

HeadList::~HeadList()
{
    headList.clear();
}

int HeadList::getKeyName(QString kName, QString *kVal)
{
    int sz = headList.size();
    QString tName;
    kVal->clear();
    for(int i=0; i<sz; i++)
    {
   //     qDebug() << QString("i= %1\tsz= %2\t|%3|%4|\n").arg(i).arg(sz).arg(headList.at(i)->keyName.simplified()).arg(kName);
        if(headList.at(i)->keyName.simplified()==kName)
        {
     //       qDebug() << "insert\n";
            kVal->insert(0, headList.at(i)->keyValue);
       //     qDebug() << "good end\n";
            return 0;
        }
    }
    //qDebug() << "end\n";
    return 1;
}

int HeadList::getKeyNum(HeadRecord* resRec, int pos)
{
    if(pos>=headList.size()) return 1;

    resRec->keyName.clear();
    resRec->keyName.insert(0, headList.at(pos)->keyName);
    resRec->keyValue.clear();
    resRec->keyValue.insert(0, headList.at(pos)->keyValue);
}

void HeadList::clear()
{
    headList.clear();
}
/*
HeadList& HeadList::operator<<(const HeadRecord& source)
{
    headList.headList << source;
}
*/
int HeadList::size()
{
    return headList.size();
}

QString ExposureRec::timeStr()
{
    return getStrFromDATEOBS(getDATEOBSfromMJD(expTime), " ", 2, 4);
}

void ExposureList::getMean(ExposureRec *eRec)
{
    int sz = exps.size();
    double time = 0.0;
    double expTime = 0.0;

    for(int j=0; j<sz; j++)
    {
        qDebug() << QString("j:%1\t").arg(j);
        qDebug() << "exps.at(j): " << exps.at(j) << "\n";
        qDebug() << QString("exps.at(i)=%1\t").arg(exps.at(j)->expTime, 15, 'f', 7);
        time += exps.at(j)->expTime;
        qDebug() << QString("exps.at(i)= %1\n").arg(exps.at(j)->expVal);
        expTime += exps.at(j)->expVal;
    }
    time /= sz*1.0;


    qDebug() << QString("time= %1\n").arg(time, 15, 'f', 7);

//    expNumSel->insertItem(sz, QString("Mean"));
    eRec->expTime = time;
    eRec->expVal = expTime;
}

double ExposureList::getExpTime(int pos)
{
    if(pos<exps.size()) return exps.at(pos)->expTime;
    return 0;
}

double ExposureList::getExpVal(int pos)
{
    if(pos<exps.size()) return exps.at(pos)->expVal;
    return 0;
}

QString ExposureList::getExpTimeStr(int pos)
{
    if(pos<exps.size()) return exps.at(pos)->timeStr();
    return QString("");
}

void ExposureList::getExp(ExposureRec *eRec, int pos)
{
    if((pos<exps.size())&&!(pos<0))
    {
        eRec->expTime = getExpTime(pos);
        eRec->expVal = getExpVal(pos);
    }
    else getMean(eRec);
}

ExposureList::~ExposureList()
{
    exps.clear();
}

//////////////////////////////////////////////////////////////////////

fitsdata::fitsdata()
{
	int fitsErrors[20];//buffer to store errors codes. Codes will be added to the buffer after each call of cfitsio function
        fileName = "empty";

	fitsHeader="";
        imgArr = NULL;//new img2d(USHORT_IMG, 0, 0);

        MJD = 0.0;


        ////////////////////////////////////////////////////////
        exptime = 0.0;
        //naxes[0] = 0;
        //naxes[1] = 0;
        //nelements = naxes[0]*naxes[1];//calculation of the number of elements (pixels) of the image
        workFrame.setCoords(0,0,0,0);

        is_empty = 1;

        //histD = new histogramData;
        /*histD->maxv = 0;
        histD->minv = 0;
	meanv = 0;
        minp = maxp = 0.0;
	hsize = (long)(maxv-minv+1);//calculation of size of the image histogramm array
	hD = new long[hsize];//allocate buffer for histogramm data
        for(int k=0;k<hsize;k++) hD[k]=0;//set zero for each pixel value in histogramm*/

        for(int i=0;i<13;i++)WCSdata[i] = 0;


        catMarks = new marksGrid();
        ipixMarks = new marksGrid();
        refMarks = new marksGrid();
        objMarks = new marksGrid();

        //refParam = NULL;
        //refMaker = NULL;

        //instr = NULL;

        obsPos = NULL;

        tmu = NULL;

        qDebug() << "\ndetNaxes\n";


        //detNaxes();
        //ast_eph_prog = "./mpeph.exe";
        //ast_eph_prog_folder = "./";
}

fitsdata::fitsdata(QString fitsFileName)
{
    fitsdata();
    openFile(fitsFileName);
    /*
    //fitsdata();
//debug
	QTextStream out_stream;
	QFile fout("stimpro.log");
	fout.open(QIODevice::Append);
	out_stream.setDevice(&fout);
//
//	fname = new char[255];
	qDebug() << "\nOpen fits\n";
	is_empty = 0;
	
	this->is_checked = 0;
	int bpix;
        //long naxes[2];
	int fitsErrors[20];//buffer to store errors codes. Codes will be added to the buffer after each call of cfitsio function
//	fileName = fitsFileName;
//	const char *fname = fitsFileName.toAscii().constData();//converting of QString to char* for file name

//	fileName.clear();
//	fileName.append("shmem://");
//	fileName.append(fitsFileName);
	fileName = fitsFileName;
	QByteArray tmp = fitsFileName.toAscii();
	const char *fname = tmp.data();//converting of QString to char* for file name
	fitsfile *fptr;//pointer ot FITSfile
//	fitsfile *fptr = *fptr0;
	int status = 0;//status
	qDebug() << "\nfname: " << fname << "\n";
	fits_open_file(&fptr, fname, READONLY,&status);fitsErrors[0]=status;//open file
	qDebug() << "\nfits_open_file: " << status << "\n";status = 0;
	fits_get_img_equivtype(fptr, &bpix, &status);fitsErrors[1]=status;status = 0;//determination of type of data (bpix)
	qDebug() << "\nfits_get_img_equivtype\n";
	fits_get_img_size(fptr, 2, naxes, &status);fitsErrors[2]=status;status = 0;//determination of size of the image
	//BEGIN read header
	qDebug() << "\nBEGIN read header\n";
	int nkeys;//variable to store number of line of the header
	fits_get_hdrspace(fptr, &nkeys, NULL, &status);fitsErrors[3]=status;status = 0;//determination of number of line of the header
	
	char card[FLEN_CARD];//buffer to read keys from header
	fitsHeader="";//strings from header
	for (int k = 1; k <= nkeys; k++)//reading strings from header
	{
	   fits_read_record(fptr, k, card, &status);// read separate record
	   fitsHeader = fitsHeader + QString(card)+"\n";// add record to fitsHeader
	}
	fitsErrors[4]=status;
	status = 0;
	//END read header
	///////////////////
	//BEGIN read positional and time data
	char strkey[FLEN_CARD];// buffer to read keys "DATE-OBS" and "EXPTIME"
	fits_read_key(fptr, TSTRING, "DATE-OBS", strkey, NULL, &status);fitsErrors[5]=status;status = 0;//read "DATE-OBS"
	fits_read_key(fptr, TDOUBLE, "EXPTIME", &exptime, NULL, &status);fitsErrors[6]=status;status = 0;//read "EXPTIME"
	QString dateObsStr = QString(strkey);//convert "DATE-OBS" to QString
	MJD = getMJDfromStrFTN(dateObsStr, exptime);//determine MJD

	////////////////////////////////////////////////////////
	fits_read_key(fptr, TSTRING, "CTYPE1", strkey, NULL, &status);// try to read first WCS tag

	if(!status)//if WCS is present
	{
		double wcsV;
		//reading WCS tags and adding WCS values to WCSdata buffer
		fits_read_key(fptr, TDOUBLE, "CRPIX1", &wcsV, NULL, &status); WCSdata[0] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CRPIX2", &wcsV, NULL, &status); WCSdata[1] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CRVAL1", &wcsV, NULL, &status); WCSdata[2] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CRVAL2", &wcsV, NULL, &status); WCSdata[3] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CDELT1", &wcsV, NULL, &status); WCSdata[4] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CDELT2", &wcsV, NULL, &status); WCSdata[5] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CROTA1", &wcsV, NULL, &status); WCSdata[6] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CROTA2", &wcsV, NULL, &status); WCSdata[7] = wcsV;
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		fits_read_key(fptr, TDOUBLE, "CD1_1", &wcsV, NULL, &status); WCSdata[8] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CD1_2", &wcsV, NULL, &status); WCSdata[9] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CD2_1", &wcsV, NULL, &status); WCSdata[10] = wcsV;
		fits_read_key(fptr, TDOUBLE, "CD2_2", &wcsV, NULL, &status); WCSdata[11] = wcsV;
		WCSdata[12] = 1;//flag of WCS: if it is equal to 1 then WCS is present 
		fitsErrors[7]=status;status = 0;
	}
	else 
	{//if WCS is absent
		status=0;
		fits_read_key(fptr, TSTRING, "RA", strkey, NULL, &status);fitsErrors[8]=status;// read "RA" key 
                if(!status) WCSdata[2] = hms_to_deg(QString(strkey),":");//RA in deg is added to WCSdata buffer
		else WCSdata[2] = 0.0;
		status = 0;
		fits_read_key(fptr, TSTRING, "DEC", strkey, NULL, &status);fitsErrors[9]=status;// read "DEC" key
                if(!status) WCSdata[3] = damas_to_deg(QString(strkey),":");//DEC in deg is added to WCSdata buffer
		else WCSdata[3] = 0.0;
		status = 0;
		WCSdata[12] = 0;//flag of WCS: if it is equal to 0 then WCS is absent 
	}
        qDebug() << QString("WCSdata[0]= %1\n").arg(WCSdata[0]);
        qDebug() << QString("WCSdata[1]= %1\n").arg(WCSdata[1]);
        qDebug() << QString("WCSdata[2]= %1\n").arg(WCSdata[2]);
        qDebug() << QString("WCSdata[3]= %1\n").arg(WCSdata[3]);
	fits_read_key(fptr, TDOUBLE, "EXPTIME", &exptime, NULL, &status);fitsErrors[6]=status;status = 0;//read "EXPTIME"
	//BEGIN read positional and time data
	//////////////////////////////////////////////////////
	//BEGIN read image data
	qDebug() << "\nBEGIN read image data\n";
	LONGLONG nelements = naxes[0]*naxes[1];//calculation of the number of elements (pixels) of the image
	qDebug() << "\nimgArr = new img2d\t:" << bpix << "\n";
	wFrame.setCoords(0, 0, naxes[0], naxes[1]);
	imgArr = new img2d(bpix, naxes[0], naxes[1]);
	int anynul = 0;//service variable
	long felem = 1;//service variable
	///////////////////////////////////////////////
	/*
	Рїв•ЄРїв•ќ РївЊЎРїВІРїв‰¤ Рїв–’РїпїЅпїЅ Рїв••Рї Рїв•™РїпїЅпїЅ , Рїв•ћ Рїв•ђРїВІРїв–“Рїв‰ҐРїв•” РїпїЅпїЅ РїГ·Рїв€™РїВ°Рїв•”Рїв•™ РїВ·РїГ·РїВІРїв••РїГ·Рїв•ќРївЊЎРївЊЎРїв•”РїГ·РїВІРїв–’Рїв•ќРїВ°Рїв•”Рїв•™, Рїв•‘РїпїЅпїЅ РїВІРїв–‘РїВ°РїВІ РїпїЅпїЅ Рїв•ќРїв••РїГ·Рїв•‘РїпїЅпїЅ Рїв•”Рїв•ђРїв•њ Рїв–’Рїв•ћРїв€™ РїпїЅпїЅ Рїв•ќРїВ°РїВ°Рїв•—Рїв€™ (РїВІРїв•ђРїв•ћРїв–“Рїв€™Рїв•ђРїв•— РїВ°Рїв•ќ РїВ·Рїв•”Рїв‰ҐРїв•ћРїв€™Рї Рїв•™РїС‘) Рїв–’ РївЊЎРїв•ќРїв•ћРїв•ћРїв•”Рїв–’ double.
	Рїв•ЄРїВІ Рїв•