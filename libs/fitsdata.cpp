#include <QtCore>
//#include <QtGui>
#include "fitsdata.h"
#include <iostream>
//#include "..\libs\astro.h"
//#include "./../libs/mb.h"
using namespace std;

int initPlateRefParam(refractionParam *refParam, fitsdata *fitsd, obsy *obsPos)
{
    qDebug() << "initRefractParam\n";
    if(refParam==NULL) return 1;
    //refParam = new refractionParam;
    refParam->utc = fitsd->MJD;

    refParam->ra0 = fitsd->WCSdata[2];
    refParam->de0 = fitsd->WCSdata[3];

    if(obsPos!=NULL) refParam->Fi = obsPos->getFi();
    else refParam->Fi = 0.0;
    if(obsPos!=NULL) refParam->Long = obsPos->Long;
    else refParam->Long = 0.0;
    refParam->lam = 0.575;

    QString kVal;
    int p0, p1;
    double temp;

    if(!fitsd->headList.getKeyName("PRESSURE", &kVal))
    {
        qDebug() << QString("\nPRESSURE |%1|\n").arg(kVal);
        refParam->press = kVal.toDouble();
    }
    else refParam->press = 760.0;
    if(refParam->press<700.0)refParam->press = 760.0;
    if(!fitsd->headList.getKeyName("TEMPERAT", &kVal))
    {
        qDebug() << QString("\nTEMPERAT |%1|\n").arg(kVal);
        refParam->temp = kVal.toDouble();
    }
    else refParam->temp = 0.0;
}

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
    return(getStimp(P, fd, width, height, fpx, x, y, params.apRadius, params.ringradius, params.ringwidth, params.sg, params.lb, params.model, params.nofit, params.delta));
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

int detHeadType(HeadList hList)
{
    QString origName;
    if(!hList.getKeyName("ORIGIN", &origName))
    {
        if(QString().compare(origName, "Pulkovo Normal astrograph")==0) return 0;
        if(QString().compare(origName, " 'SDSS    '")==0) return 1;
    }
    return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    qDebug() << "\n\nmake6const\n\n";
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
    int i, j, p;
    int recount = 1;
    double *data;
    marksP *mT;
    qDebug() << QString("params.minRefStars= %1").arg(params.minRefStars);
    int rsSize;
    int redDeg = 8;
    int i0, j0, i1, j1;

    BVect = NULL;
    CVect = NULL;
    ZVect = NULL;
    EXCLIND = NULL;
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
    WE = NULL;
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
        if(BVect!=NULL) delete [] BVect;
        if(CVect!=NULL) delete [] CVect;
        if(ZVect!=NULL) delete [] ZVect;
        if(ZMAG!=NULL) delete [] ZMAG;
        if(DMAG!=NULL) delete [] DMAG;
        if(KSI!=NULL) delete [] KSI;
        if(ETA!=NULL) delete [] ETA;
        if(MAG!=NULL) delete [] MAG;
        if(WE!=NULL) delete [] WE;

        //clear();
        recount = 0;
        minErrEta=minErrKsi=1000;
        BVect = new double [rsSize*2];
        CVect = new double [rsSize*2*redDeg];
        //ZVect = new double [redDeg];
        KSI = new double [rsSize];
        ETA = new double [rsSize];
        MAG = new double [rsSize];
        WE  = new double [rsSize*2];
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

            BVect[i] = data[3];
            BVect[i+rsSize] = data[4];

            CVect[i*redDeg+0] = data[5];
            CVect[i*redDeg+1] = data[6];
            CVect[i*redDeg+2] = 1;
            CVect[i*redDeg+3] = 0.0;
            CVect[i*redDeg+4] = 0.0;
            CVect[i*redDeg+5] = 0.0;
            CVect[i*redDeg+6] = data[5]*data[5];
            CVect[i*redDeg+7] = data[5]*data[6];

            CVect[(i+rsSize)*redDeg+0] = 0.0;
            CVect[(i+rsSize)*redDeg+1] = 0.0;
            CVect[(i+rsSize)*redDeg+2] = 0.0;
            CVect[(i+rsSize)*redDeg+3] = data[5];
            CVect[(i+rsSize)*redDeg+4] = data[6];
            CVect[(i+rsSize)*redDeg+5] = 1;
            CVect[(i+rsSize)*redDeg+6] = data[5]*data[6];
            CVect[(i+rsSize)*redDeg+7] = data[6]*data[6];

            KSI[i] = data[3];
            ETA[i] = data[4];
            MAG[i] = data[2];
      /*
            CVect[i+0] =

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
        */

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
                WE[i]=pow(minErrKsi/errksi,2);
                WE[i+rsSize]=pow(minErrEta/erreta,2);
            }
            else
            {
                WE[i]=1;
                WE[i+rsSize]=1;
            }
        }
        EXCLIND = new int[rsSize*2];
        ZVect=new double[redDeg];
        DVect=new double[redDeg*redDeg];
        RN=0;
        iLSM(redDeg, rsSize*2, mas_to_grad(params.maxres), EXCLIND, ZVect, CVect, BVect, UWE, DVect, params.sigma, RN, WE);
        qDebug() << QString("ZVect: %1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\n").arg(ZVect[0]).arg(ZVect[1]).arg(ZVect[2]).arg(ZVect[3]).arg(ZVect[4]).arg(ZVect[5]).arg(ZVect[6]).arg(ZVect[7]);
        qDebug() << QString("RN: %1\n").arg(RN);

        /*
        ZETA=new double[redDeg];
        DETA=new double[redDeg*redDeg];
        EXCLINDETA = new int [rsSize];
        RNETA=0;
        iLSM(redDeg, rsSize, mas_to_grad(params.maxres), EXCLINDETA, ZETA, Ceta, ETA, UWEETA, DETA, params.sigma, RNETA, WEeta);
        qDebug() << QString("Zy: %1\t%2\t%3\t%4\t%5\n").arg(ZETA[0]).arg(ZETA[1]).arg(ZETA[2]).arg(ZETA[3]).arg(ZETA[4]);*/
        ZMAG=new double[2];
        DMAG=new double[2*2];
        EXCLINDMAG = new int [rsSize];
        RNMAG=0;
        iLSM(2, p, params.maxresMAG, EXCLINDMAG, ZMAG, CMAG, MAG, UWEMAG, DMAG, params.sigma, RNMAG);
        qDebug() << QString("Zm: %1\t%2\n").arg(ZMAG[0]).arg(ZMAG[1]);


        ZKSI=new double[5];
        DKSI=new double[5*5];
        EXCLINDKSI = new int [rsSize];
        RNKSI=0;
        ZETA=new double[5];
        DETA=new double[5*5];
        EXCLINDETA = new int [rsSize];
        RNETA=0;

        ZKSI[0] = ZVect[0];
        ZKSI[1] = ZVect[1];
        ZKSI[2] = ZVect[2];
        ZKSI[3] = ZVect[6];
        ZKSI[4] = ZVect[7];

        ZETA[0] = ZVect[3];
        ZETA[1] = ZVect[4];
        ZETA[2] = ZVect[5];
        ZETA[3] = ZVect[6];
        ZETA[4] = ZVect[7];


        for(i=0; i<5; i++)
        {
            for(j=0; j<5; j++)
            {
                if(i<3&&j<3)
                {
                    i0 = i;
                    j0 = j;
                }
                if(j>=3) j0 = j+3;
                if(i>=3) i0 = i+3;

                i1 = i+3;
                j1 = j+3;

                    DKSI[j+i*5] = DVect[j0+i0*redDeg];
                    DETA[j+i*5] = DVect[j1+i1*redDeg];
            }
        }


        RNKSI = 0; RNETA = 0;

        for(i=0;i<rsSize;i++)
        {
            EXCLINDKSI[i] = EXCLIND[i];
            EXCLINDETA[i] = EXCLIND[i+rsSize];
            if(EXCLINDKSI[i]>=0) RNKSI+=1;
            if(EXCLINDETA[i]>=0) RNETA+=1;
        }

        UWEKSI = UWEETA = UWE;



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
        detTan8const(&pixKsi, &pixEta, CVect[i*8+0], CVect[i*8+1]);
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

void ExposureList::clear()
{
    exps.clear();
}

int ExposureList::expNum()
{
    return exps.size();
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
	п╪п╝ п⌡п²п≤ п▒п�� п╕п п╙п�� , п╞ п═п²п▓п≥п╔ п�� п÷п∙п°п╔п╙ п·п÷п²п╕п÷п╝п⌡п⌡п╔п÷п²п▒п╝п°п╔п╙, п║п�� п²п░п°п² п�� п╝п╕п÷п║п�� п╔п═п╜ п▒п╞п∙ п�� п╝п°п°п╗п∙ (п²п═п╞п▓п∙п═п╗ п°п╝ п·п╔п≥п╞п∙п п╙пё) п▒ п⌡п╝п╞п╞п╔п▒ double.
	п╪п² п╘п═п² п·п÷п╔п▒п²п�� п╔п═ п≥ п°п∙п²п·п÷п╝п▒п�� п╝п°п°п²п⌡п║ п÷п╝п╞пёп²п�� п║ п²п·п∙п÷п╝п═п╔п▒п°п²п≤ п·п╝п⌡п╙п═п╔ (п°п╝п·п÷п╔п⌡п∙п÷, п▒п⌡п∙п╞п═п² п�� п▒п║пё п░п╝п≤п═ п°п╝ п·п╔п≥п╞п∙п , п⌡п╗ п▒п╞п∙п╕п�� п╝ п╔п⌡п∙п∙п⌡ 8) п╔ п║п⌡п∙п°п╜п╛п∙п°п╔п╖ п╞п≥п²п÷п²п╞п═п╔ п÷п╝п░п²п═п╗.
	я┌п╝п≤п  п⌡п∙п�� п п∙п°п°п² п²п═п≥п÷п╗п▒п╝п∙п═п╞п╙, п═п╝п≥ п≥п╝п≥ п°п∙п²п░пёп²п�� п╔п⌡ п√п╔п≥п  п·п² п▒п╞п∙п⌡ п·п╔п≥п╞п∙п п╙п⌡ п�� п п╙ п·п÷п∙п²п░п÷п╝п�� п²п▒п╝п°п╔п╙ п▒ double. 
	п╩п∙п�� п п∙п°п°п² п·п∙п÷п∙п╞п▓п╔п═п╗п▒п╝п╖п═п╞п╙ п�� п°п╝п▓п∙п°п╔п╙ п°п╝ п·п╔п≥п╞п∙п п╝пё п·п÷п╔ п·п÷п²п÷п╔п╞п²п▒п≥п∙ п╔п�� п²п░п÷п╝п╓п∙п°п╔п≤.
	п╬п² п╘п═п²п≤ п·п÷п╔п▓п╔п°п∙ пёп÷п╝п°п∙п°п╔п∙ п╔ п�� п²п╞п═п║п· п≥ п�� п╝п°п°п╗п⌡ п╔п�� п²п░п÷п╝п╓п∙п°п╔п╙ п²п╞п║п╚п∙п╞п═п▒п п╙п∙п═п╞п╙ п▒ п�� п╝п▒п╔п╞п╔п⌡п²п╞п═п╔ п²п═ п═п╔п·п╝ п�� п╝п°п°п╗пё USHORT_IMG, SHORT_IMG, LONG_IMG, FLOAT_IMG п╔п п╔ DOUBLE_IMG
        /
//scaled image prop
	scFactor = 1;
	qDebug() << "\nfits_read_key\n";
	fits_read_key(fptr, TSTRING, "SCFACTOR", &strkey, NULL, &status);
	if(!status) scFactor = atoi(strkey);
	status = 0;
	
	fits_read_key(fptr, TSTRING, "FSOURCE", &strkey, NULL, &status);
	if(!status) fSource.insert(0, strkey);
	else fSource.insert(0, fitsFileName);
	status = 0;
/////////////////	
	
	switch(bpix)
	{
		case USHORT_IMG:
		{
//			imgArr = new img2d(0, naxes[0], naxes[1]);
//			ushD = new unsigned short[nelements];//allocation of appropriate tamporary buffer
			fits_read_img(fptr, TUSHORT, felem, nelements, NULL, imgArr->ushD ,&anynul, &status);fitsErrors[10]=status;status = 0;//read data image from file
			break;
		}
		case SHORT_IMG:
		{
//			imgArr = new img2d(1, naxes[0], naxes[1]);
//			shD = new short[nelements];
			fits_read_img(fptr, TSHORT, felem, nelements, NULL, imgArr->shD ,&anynul, &status);fitsErrors[10]=status;status = 0;//read data image from file
			break;
		}
		case LONG_IMG:
		{
//			imgArr = new img2d(2, naxes[0], naxes[1]);
//			lD = new long[nelements];
			fits_read_img(fptr, TLONG, felem, nelements, NULL, imgArr->lD ,&anynul, &status);fitsErrors[10]=status;status = 0;//read data image from file
			break;
		}
		case FLOAT_IMG:
		{
//			imgArr = new img2d(3, naxes[0], naxes[1]);
//			fD = new float[nelements];
			fits_read_img(fptr, TFLOAT, felem, nelements, NULL, imgArr->fD ,&anynul, &status);fitsErrors[10]=status;status = 0;//read data image from file
			break;
		}
		case DOUBLE_IMG:
		{
//			imgArr = new img2d(4, naxes[0], naxes[1]);
//			dD = new double[nelements];
			fits_read_img(fptr, TDOUBLE, felem, nelements, NULL, imgArr->dD ,&anynul, &status);fitsErrors[10]=status;status = 0;//read data image from file
			break;
		}
	}
//		QMessageBox::information(0,"poinT", QString("%1").arg(status),QMessageBox::Ok,0,0);

	///////////////////////////////////////
	//END read image data
	//////////////////////
	//BEGIN histogramm
        qDebug() << "\nBEGIN histogramm\n";
	maxv = imgArr->getPos(0);
	minv = imgArr->getPos(0);
	//QMessageBox::information(0,"stimpro","ok",QMessageBox::Ok,0,0);
	meanv = 0;
	double pixv;
	for(LONGLONG i=0;i<nelements;i++)//determination of maximum, minimum and mean pixels values
	{
		pixv = imgArr->getPos(i);
		if(pixv>maxv)maxv=pixv;
		if(pixv<minv)minv=pixv;
		meanv = meanv + pixv;
	}
	meanv = meanv/nelements;
	hsize = (long)(maxv-minv+1);//calculation of size of the image histogramm array
	hD = new long[hsize];//allocate buffer for histogramm data
	for(int k=0;k<hsize;k++) hD[k]=0;//set zero for each pixel value in histogramm
	for(LONGLONG i=0;i<nelements;i++) hD[(LONGLONG)(imgArr->getPos(i)-minv)]++;//construction of the histogramm
	long fpn=0;//variable to store value of the most frequent pixel value
	
	for(int k=0;k<hsize;k++)
	{
                if(hD[k]>hD[fpn]) fpn=k;// determination of the most frequent pixel value as maximum of histogramm
	}
	fpix = fpn + minv;// calculation of the most frequent pixel value according to minimum pixel value
	maxH = hD[(LONGLONG)(fpix-minv)];
	/*
	п�� п п╙ п░п²п п╜п╛п╔п°п╞п═п▒п╝ п╝п╞п═п÷п²п°п²п⌡п╔п▓п∙п╞п≥п╔пё п╔п�� п²п░п÷п╝п╓п∙п°п╔п≤ пёп╝п÷п╝п≥п═п∙п÷п°п╝ п╕п╔п╞п═п²п╕п÷п╝п⌡п⌡п╝ п╞ "п²п╞п═п÷п╗п⌡" п·п╔п≥п²п⌡. п╬п²п╞п≥п²п п╜п≥п║ п·п²п▓п═п╔ п▒п╞п∙ п�� п°п╝п▓п╔п⌡п╗п∙ п�� п п╙ п·п÷п²п÷п╔п╞п²п▒п≥п╔ п·п╔п≥п╞п∙п п╔ п п∙п╓п╝п═ п▒ п·п÷п∙п�� п∙п п╝пё 
	п·п╔п≥п╝, п÷п╝п�� п║п⌡п°п² п╞п�� п∙п п╝п═п╜ п═п╝п≥, п▓п═п²п░п╗ п²п╞п═п╝п п╜п°п╗п∙ п·п╔п≥п╞п∙п п╔ п·п÷п╔ п·п÷п²п÷п╔п╞п²п▒п≥п∙ п°п∙ п║п▓п╔п═п╗п▒п╝п п╔п╞п╜. 
	п╬п╝п÷п╝п⌡п∙п═п÷ hw - п▒п╗п▓п╔п╞п п╙п∙п═п╞п╙ п≥п╝п≥ п╞п÷п∙п�� п°п∙п▒п�� п▒п∙п╛п∙п°п°п²п∙ п�� п°п╝п▓п∙п°п╔п╙ п⌡п²п�� п║п п╙ п║п≥п п²п°п∙п°п╔п╙ п²п═ п⌡п╝п≥п╞п╔п⌡п║п⌡п╝ п╕п╔п╞п═п²п╕п÷п╝п⌡п⌡п╗. я┴п═п² п�� п╝п∙п═ "п≥п²п⌡п╒п²п÷п═п°п╗п∙" п�� п п╙ п·п÷п²п╞п⌡п²п═п÷п╝ п≥п╝п÷п═п╔п°п≥п╔ п·п╝п÷п╝п⌡п∙п═п÷п╗ "п╚п∙п п╔ п·п÷п²п÷п╔п╞п²п▒п≥п╔"
        /
	hw=0;
	double summWeights = 0;
	for(long k=0;k<hsize;k++){hw=hw+abs(k-fpn)*hD[k]/hD[fpn];summWeights=summWeights+hD[k]/hD[fpn];}
	hw = hw/summWeights;
	maxp = fpix+hw;
	if(maxp>maxv)maxp=maxv;
	minp = fpix-hw;
	if(minp<minv)minp=minv;
	st = 2*hw/25;
	//END histogramm
	//Close FITS file
	fits_close_file(fptr, &status); status = 0;

        catMarks = new marksGrid();
        ipixMarks = new marksGrid();
        refMarks = new marksGrid();
        objMarks = new marksGrid();
        expList = new ExposureList;
        //ast_eph_prog = "./mpeph.exe";
        //ast_eph_prog_folder = "./";
        mpeWaitTime = 1000000;
        sbWaitTime = 10000000;
        refParam = NULL;
        refMaker = NULL;

        instr = NULL;

        isMagEqCorr = 0;
        isComaCorr = 0;
        isVFcorr = 0;

        qDebug() << "\ndetNaxes\n";
        detNaxes();
        */

};

fitsdata::~fitsdata()
{
	delete imgArr;

//	delete[] hD;
        delete catMarks;
        delete ipixMarks;
        delete objMarks;
        delete refMarks;
        //delete expList;

        delete obsPos;
};

int fitsdata::openFile(QString fitsFileName, int headType)
{

    qDebug() << "\nOpen fits\n";
    is_empty = 0;



    //this->is_checked = 0;
    LONGLONG nelements;
    int bpix;
    long naxes[2];
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
    qDebug() << "\nfits_open_file: " << status << "\n";
    if(status) return 1;
    status = 0;
    fits_get_img_equivtype(fptr, &bpix, &status);fitsErrors[1]=status;status = 0;//determination of type of data (bpix)
    qDebug() << "\nfits_get_img_equivtype: " << status << "\n";status = 0;
    fits_get_img_size(fptr, 2, naxes, &status);fitsErrors[2]=status;status = 0;//determination of size of the image


    //BEGIN read header
    qDebug() << "\nBEGIN read header\n";
    int nkeys;//variable to store number of line of the header
    fits_get_hdrspace(fptr, &nkeys, NULL, &status);fitsErrors[3]=status;status = 0;//determination of number of line of the header
    qDebug() << QString("header nkeys: %1\n").arg(nkeys);
    char card[FLEN_CARD];//buffer to read keys from header
    fitsHeader="";//strings from header
    for (int k = 1; k <= nkeys; k++)//reading strings from header
    {
       fits_read_record(fptr, k, card, &status);// read separate record
       qDebug() << QString("read header key: %1\n").arg(QString(card));
       fitsHeader = fitsHeader + QString(card)+"\n";// add record to fitsHeader
    }
    initHeadList();
    fitsErrors[4]=status;
    status = 0;
    qDebug() << "END read header\n";
    ///////////////////
    //BEGIN read positional and time data
    char strkey[FLEN_CARD];// buffer to read keys "DATE-OBS" and "EXPTIME"




    fits_read_key(fptr, TSTRING, "DATE-OBS", strkey, NULL, &status);fitsErrors[5]=status;status = 0;//read "DATE-OBS"
    fits_read_key(fptr, TDOUBLE, "EXPTIME", &exptime, NULL, &status);fitsErrors[6]=status;status = 0;//read "EXPTIME"
    QString dateObsStr = QString(strkey);//convert "DATE-OBS" to QString
    QString taiStr;
    double dtai;
    qDebug() << QString("headType: %1\n").arg(headType);
    if(headType==-1)
    {
        headType = detHeadType(headList);
        qDebug() << QString("detHeadType: %1\n").arg(headType);
    }
    switch(headType)
    {
    case 0:
        //MJD = getMJDfromStrFTN(dateObsStr, exptime);//determine MJD
        if(getMJDfromStrFTN(&MJD, dateObsStr, exptime))
        {
            qDebug() << "\nDATE-OBS error\n";
            fits_close_file(fptr, &status);
            return 1;
        }//determine MJD
        break;
        case 1:
        {
            fits_read_key(fptr, TSTRING, "TAIHMS", strkey, NULL, &status);status = 0;
            taiStr = QString(strkey);
            qDebug() << QString("TAIHMS: %1\n").arg(taiStr);
            dateObsStr += "T"+taiStr;
            if(getMJDfromStrFTN(&MJD, dateObsStr, exptime))
            {
                qDebug() << "\nDATE-OBS error\n";
                fits_close_file(fptr, &status);
                return 1;
            }//determine MJD
            qDebug() << QString("MJD: %1\n").arg(MJD);
            if(tmu!=NULL) dtai = tmu->getTmu(mjd2jd(MJD));
            else dtai = 32.0;
            qDebug() << QString("dtai= %1\n").arg(dtai);
            MJD -= dtai/86400.0;
        }
        break;
    default:
        qDebug() << QString("\nWARN: unknown header type\n");
        break;
    }

    ////////////////////////////////////////////////////////
    fits_read_key(fptr, TSTRING, "CTYPE1", strkey, NULL, &status);// try to read first WCS tag
    double wcsV;
    if(!status)//if WCS is present
    {
        qDebug() << "\nWCS is present\n";

            //reading WCS tags and adding WCS values to WCSdata buffer
            status = 0;
            fits_read_key(fptr, TSTRING, "CRPIX1", &strkey, NULL, &status); WCSdata[0] = atof(strkey); qDebug() << "WCSdata[0]:" << WCSdata[0] <<" stat: " << status << "\n"; status = 0;
            fits_read_key(fptr, TSTRING, "CRPIX2", &strkey, NULL, &status); WCSdata[1] = atof(strkey);qDebug() << "WCSdata[1]:" << WCSdata[1] <<" stat: " << status << "\n";status = 0;
            fits_read_key(fptr, TSTRING, "CRVAL1", &strkey, NULL, &status); WCSdata[2] = atof(strkey);qDebug() << "WCSdata[2]:" << WCSdata[2] <<" stat: " << status << "\n";status = 0;
            fits_read_key(fptr, TSTRING, "CRVAL2", &strkey, NULL, &status); WCSdata[3] = atof(strkey);qDebug() << "WCSdata[3]:" << WCSdata[3] <<" stat: " << status << "\n";status = 0;
            fits_read_key(fptr, TSTRING, "CDELT1", &strkey, NULL, &status); WCSdata[4] = atof(strkey);qDebug() << "WCSdata[4]:" << WCSdata[4] <<" stat: " << status << "\n";status = 0;
            //fits_read_key(fptr, TDOUBLE, "CDELT1", &WCSdata[4], NULL, &status);qDebug() << "WCSdata[4]:" << WCSdata[4] <<" stat: " << status << "\n";status = 0;
            fits_read_key(fptr, TSTRING, "CDELT2", &strkey, NULL, &status); WCSdata[5] = atof(strkey);qDebug() << "WCSdata[5]:" << WCSdata[5] <<" stat: " << status << "\n";status = 0;
            fits_read_key(fptr, TSTRING, "CROTA1", &strkey, NULL, &status); WCSdata[6] = atof(strkey);qDebug() << "WCSdata[6]:" << WCSdata[6] <<" stat: " << status << "\n";status = 0;
            fits_read_key(fptr, TSTRING, "CROTA2", &strkey, NULL, &status); WCSdata[7] = atof(strkey);qDebug() << "WCSdata[7]:" << WCSdata[7] <<" stat: " << status << "\n";status = 0;
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            fits_read_key(fptr, TSTRING, "CD1_1", &strkey, NULL, &status); WCSdata[8] = atof(strkey);qDebug() << "WCSdata[8]:" << WCSdata[8] <<" stat: " << status << "\n";status = 0;
            fits_read_key(fptr, TSTRING, "CD1_2", &strkey, NULL, &status); WCSdata[9] = atof(strkey);qDebug() << "WCSdata[9]:" << WCSdata[9] <<" stat: " << status << "\n";status = 0;
            fits_read_key(fptr, TSTRING, "CD2_1", &strkey, NULL, &status); WCSdata[10] = atof(strkey);qDebug() << "WCSdata[10]:" << WCSdata[10] <<" stat: " << status << "\n";status = 0;
            fits_read_key(fptr, TSTRING, "CD2_2", &strkey, NULL, &status); WCSdata[11] = atof(strkey);qDebug() << "WCSdata[11]:" << WCSdata[11] <<" stat: " << status << "\n";status = 0;

            WCSdata[12] = 1;//flag of WCS: if it is equal to 1 then WCS is present
            fitsErrors[7]=status;status = 0;

    }
    else
    {//if WCS is absent
        qDebug() << "\nWCS is absent\n";
            status=0;
            fits_read_key(fptr, TSTRING, "RA", strkey, NULL, &status);fitsErrors[8]=status;// read "RA" key
            if(!status)
            {
                if(!hms_to_deg(&wcsV, QString(strkey),":"))WCSdata[2] = wcsV;
                else
                {
                    qDebug() << "\nRA error\n";
                    fits_close_file(fptr, &status);
                    return 1;
                }
                //WCSdata[2] = hms_to_deg(QString(strkey),":");//RA in deg is added to WCSdata buffer
            }
            else WCSdata[2] = 0.0;
            status = 0;
            fits_read_key(fptr, TSTRING, "DEC", strkey, NULL, &status);fitsErrors[9]=status;// read "DEC" key
            if(!status)
            {
                if(!damas_to_deg(&wcsV, QString(strkey),":"))WCSdata[3] = wcsV;
                else
                {
                    qDebug() << "\nDEC error\n";
                    fits_close_file(fptr, &status);
                    return 1;
                }
                //WCSdata[3] = damas_to_deg(QString(strkey),":");//DEC in deg is added to WCSdata buffer
            }
            else WCSdata[3] = 0.0;
            status = 0;
            WCSdata[0] = naxes[0]/2.0;
            WCSdata[1] = naxes[1]/2.0;
            //WCSdata[12] = 0;//flag of WCS: if it is equal to 0 then WCS is absent
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
    nelements = naxes[0]*naxes[1];//calculation of the number of elements (pixels) of the image

    qDebug() << "\nimgArr = new img2d\t:" << bpix << "\n";
    workFrame.setCoords(0, 0, naxes[0], naxes[1]);
    imgArr = new img2d(bpix, naxes[0], naxes[1]);
    int anynul = 0;//service variable
    long felem = 1;//service variable
    ///////////////////////////////////////////////


    status = 0;
/////////////////

    switch(bpix)
    {
            case USHORT_IMG:
            {
//			imgArr = new img2d(0, naxes[0], naxes[1]);
//			ushD = new unsigned short[nelements];//allocation of appropriate tamporary buffer
                    fits_read_img(fptr, TUSHORT, felem, nelements, NULL, imgArr->ushD ,&anynul, &status);fitsErrors[10]=status;status = 0;//read data image from file
                    break;
            }
            case SHORT_IMG:
            {
//			imgArr = new img2d(1, naxes[0], naxes[1]);
//			shD = new short[nelements];
                    fits_read_img(fptr, TSHORT, felem, nelements, NULL, imgArr->shD ,&anynul, &status);fitsErrors[10]=status;status = 0;//read data image from file
                    break;
            }
            case LONG_IMG:
            {
//			imgArr = new img2d(2, naxes[0], naxes[1]);
//			lD = new long[nelements];
                    fits_read_img(fptr, TLONG, felem, nelements, NULL, imgArr->lD ,&anynul, &status);fitsErrors[10]=status;status = 0;//read data image from file
                    break;
            }
            case FLOAT_IMG:
            {
//			imgArr = new img2d(3, naxes[0], naxes[1]);
//			fD = new float[nelements];
                    fits_read_img(fptr, TFLOAT, felem, nelements, NULL, imgArr->fD ,&anynul, &status);fitsErrors[10]=status;status = 0;//read data image from file
                    break;
            }
            case DOUBLE_IMG:
            {
//			imgArr = new img2d(4, naxes[0], naxes[1]);
//			dD = new double[nelements];
                    fits_read_img(fptr, TDOUBLE, felem, nelements, NULL, imgArr->dD ,&anynul, &status);fitsErrors[10]=status;status = 0;//read data image from file
                    break;
            }
    }
//		QMessageBox::information(0,"poinT", QString("%1").arg(status),QMessageBox::Ok,0,0);

    ///////////////////////////////////////
    //END read image data
    //////////////////////



    //Close FITS file
    fits_close_file(fptr, &status); status = 0;

    updateHist();
/*
    catMarks = new marksGrid();
    ipixMarks = new marksGrid();
    refMarks = new marksGrid();
    objMarks = new marksGrid();
    expList = new ExposureList;
    //ast_eph_prog = "./mpeph.exe";
    //ast_eph_prog_folder = "./";
    //mpeWaitTime = 1000000;
    //sbWaitTime = 10000000;
    refParam = NULL;
    refMaker = NULL;

    instr = NULL;

    qDebug() << "\ndetNaxes\n";*/

    workFrame.setCoords(0, 0, imgArr->naxes[0], imgArr->naxes[1]);
    setRpix();
    /*WCSdata[0] = workFrame.center().x();
    WCSdata[1] = workFrame.center().y();*/

    //detNaxes();

    qDebug() << "\nEND Open fits\n";
    return 0;
}

void fitsdata::initTmu(QString tmuFile)
{
    tmu = new TAImUTC;
    if(tmu->initTmsFile(tmuFile))
    {
        delete tmu;
        tmu = NULL;
    }
}
/*
int fitsdata::openEmptyFile()
{

    qDebug() << "\nOpen empty\n";
    is_empty = 1;

    fileName = "empty";

    fitsHeader="";//strings from header

    //END read header
    ///////////////////

    MJD = 0.0;

    ////////////////////////////////////////////////////////
    exptime = 0.0;
    //naxes[0] = 0;
    //naxes[1] = 0;
    //nelements = naxes[0]*naxes[1];//calculation of the number of elements (pixels) of the image
    nelements = 0;

    imgArr = new img2d(USHORT_IMG, naxes[0], naxes[1]);
    int anynul = 0;//service variable
    long felem = 1;//service variable
    ///////////////////////////////////////////////


    catMarks = new marksGrid();

    ipixMarks = new marksGrid();
    refMarks = new marksGrid();
    objMarks = new marksGrid();
    expList = new ExposureList;

    refParam = NULL;
    refMaker = NULL;

    instr = NULL;

    qDebug() << "\ndetNaxes\n";
    detNaxes();

    return 0;
}
*/

int fitsdata::updateHist()
{
    if(imgArr==NULL) return 1;
    return (imgArr->updateHist());
}

void fitsdata::clear()
{
    qDebug() << QString("\nclear\n");
    fileName.clear();
    fitsHeader.clear();
    for(int i=0; i<12; i++) WCSdata[i] = 0.0;
    /*if(catMarks!=NULL) delete catMarks;// = new marksGrid();
    //catMarks = NULL;
    if(ipixMarks!=NULL) delete ipixMarks;// = new marksGrid();
    //catMarks = NULL;
    if(refMarks!=NULL) delete refMarks;// = new marksGrid();
    //catMarks = NULL;
    if(objMarks!=NULL) delete objMarks;// = new marksGrid();*/
    //catMarks = NULL;
    //if(expList!=NULL) delete expList;// = new ExposureList;
    //catMarks = NULL;

    //if(refParam!=NULL) delete refParam;
   //catMarks = NULL;
    //if(refMaker!=NULL) delete refMaker;
   //catMarks = NULL;

    if(obsPos!=NULL) delete obsPos;
    obsPos = NULL;

    //delete instr;
    if(imgArr!=NULL) delete imgArr;
    //fitsdata();
    if(tmu!=NULL) delete tmu;
    tmu = NULL;

    catMarks->clearMarks();
    ipixMarks->clearMarks();
    refMarks->clearMarks();
    objMarks->clearMarks();
    //expList = NULL;
    //refParam = NULL;
    //refMaker = NULL;
    //instr = NULL;


    imgArr = NULL;
}

void fitsdata::setInstrSettings(insSettings instr)
{
    WCSdata[4] = instr.scx;
    WCSdata[5] = instr.scy;
    WCSdata[6] = instr.rang;
    WCSdata[7] = 180.0;
}

/*
void fitsdata::initInst(QString instrIni, int iNum)
{
    qDebug() << QString("initInst: %1\n").arg(instrIni);
    instr = new insSettings(instrIni.toAscii().data());
    qDebug() << "instr: " << instr << "\n";
    qDebug() << "iNum: " << iNum << "\n";
    if(iNum==-1) instr->getNumIns(instr->curInst);
    else instr->getNumIns(iNum);


/*
    if(catMarks!=NULL) catMarks->setInstrProp(instr->scx, instr->scy, instr->rang);
    if(ipixMarks!=NULL) ipixMarks->setInstrProp(instr->scx, instr->scy, instr->rang);
    if(objMarks!=NULL) objMarks->setInstrProp(instr->scx, instr->scy, instr->rang);
    if(refMarks!=NULL) refMarks->setInstrProp(instr->scx, instr->scy, instr->rang);/
    //detNaxes();

}

void fitsdata::setInstNum(int iNum)
{
    if(iNum==-1) instr->getNumIns(instr->curInst);
    else instr->getNumIns(iNum);

    //setInstSett(instr);
}

void fitsdata::setInstParam(double scx, double scy, double fovp, double rang)
{
    instr->scx = scx;
    instr->scy = scy;
    instr->rang = rang;
    instr->fov = fovp;

//    setInstSett(instr);
}
/*
void fitsdata::setInstSett(insSettings *iSett)
{
    catMarks->setInstrProp(iSett->scx, iSett->scy, iSett->rang);
    ipixMarks->setInstrProp(iSett->scx, iSett->scy, iSett->rang);
    objMarks->setInstrProp(iSett->scx, iSett->scy, iSett->rang);
    refMarks->setInstrProp(iSett->scx, iSett->scy, iSett->rang);
}
*/
/*
double fitsdata::getPixelValue(LONGLONG k)
{
	double outRes;
	switch(bpix)
	{
		case USHORT_IMG:{outRes = (double)ushD[k];break;}
		case SHORT_IMG: {outRes = (double)shD[k];break;}
		case LONG_IMG:  {outRes = (double)lD[k];break;}
		case FLOAT_IMG: {outRes = (double)fD[k];break;}
		case DOUBLE_IMG:{outRes = dD[k];break;}
	}
	return outRes;
};
*/
uchar fitsdata::getImgPixValue(int x, int y)
{
    return(imgArr->getImgPixValue(x, y));
}

uchar fitsdata::getPixValue(int x, int y)
{
    return(imgArr->getPixValue(x, y));
}

void fitsdata::detTan()
{
        catMarks->detTan(&WCSdata[0]);
}

void fitsdata::detPpix(marksGrid *mGr, int apeDiam)
{
    //qDebug() << "fitsdata::detPpix\n";
    if(!is_empty)
    {
	int i, num;
	marksP *mPos;
//	marksGrid *mGr;
//	if(grid) mGr = ipixMarks;
//	else mGr = catMarks;
        //imgAperture* poinT;
	num = mGr->marks.size();
        //segment *sg0 = new segment(apeSize, apeSize);
        //qDebug() << QString("marks size: %1\n").arg(num);
        //qDebug() << QString("apeSize: %1\n").arg(apeSize);
	
	for(i=num-1; i>=0; i--)
	{
		mPos = mGr->marks.at(i);
                if(mPos->mPpix!=NULL) delete mPos->mPpix;
                mPos->mPpix = new imgAperture(apeDiam*apeDiam);
                if(imgArr->getAperture(mPos->mPpix, mPos->mTanImg[0], mPos->mTanImg[1], apeDiam/2+1))
		{
//			delete &mPos;
                        mGr->remMark(i);
                        continue;
		}
		mPos->mTanImg[2] = mPos->mPpix->seADC;

	}
        /*for(i=0; i<mGr->marks.size(); i++)
	{
		qDebug() << i << " " << mPos->mTanImg[0] << " " << mPos->mTanImg[1] << endl;
		for(int j=0; j<mPos->mPpix->Size; j++)
		{
			qDebug() << "\t" << mPos->mPpix->buffer[j].X << "\t" << mPos->mPpix->buffer[j].Y << "\t" << mPos->mPpix->buffer[j].I << endl;
		}
        }*/
        //delete sg0;
    }
    //qDebug() << "end fitsdata::detPpix\n";
}

int detMarkPpix(img2d *imgArr, marksP *mP, int apeSize)
{
    mP->mPpix = new imgAperture(apeSize*2);
    if(imgArr->getAperture(mP->mPpix, mP->mTanImg[0], mP->mTanImg[1], apeSize/2.0)) return 0;

    mP->mTanImg[2] = mP->mPpix->seADC;
    return 1;
}

void fitsdata::moveCenter(marksGrid *mGr)
{
	int i, num;
        marksP* mkP;
        num = mGr->marks.size();
	for(i=0; i<num; i++)
	{
                mkP = mGr->marks.at(i);
		mkP->mTanImg[0] = mkP->mPpix->Xmax;
		mkP->mTanImg[1] = mkP->mPpix->Ymax;
	}
}

void fitsdata::moveMassCenter(marksGrid *mk, int apeDiam)
{
    qDebug() << "\nfitsdata::moveMassCenter\n\n";
	int i, num;
	marksP* mkP;
        //num = mk->marks.size();
        STIMG poinT;
        double d0, summ0, summ1;
        summ1 = 0;
        int k = 0;
        do
        {
            summ0 = summ1;

            detPpix(mk, apeDiam);

            summ1 = 0;
            num = mk->marks.size();
            //qDebug() << QString("num= %1\n").arg(num);
            for(i=0; i<num; i++)
            {
                mkP = mk->marks.at(i);

                poinT = CenterOfMas(mkP->mPpix);
                d0 = sqrt(pow(mkP->mTanImg[0]-poinT.X, 2.0)+ pow(mkP->mTanImg[1]-poinT.Y, 2.0));
                //qDebug() << QString("d0= %1\n").arg(d0);
                /*while(d0>2)
                {
                    mkP->mTanImg[0] = poinT.X;
                    mkP->mTanImg[1] = poinT.Y;

                    poinT = CenterOfMas(mkP->mPpix);

                    d0 = sqrt(pow(mkP->mTanImg[0]-poinT.X, 2.0)+ pow(mkP->mTanImg[1]-poinT.Y, 2.0));
                }*/
                summ1 += d0;
                mkP->mTanImg[0] = poinT.X;
                mkP->mTanImg[1] = poinT.Y;

            }
            qDebug() << QString("summ0= %1\tsumm1= %2").arg(summ0).arg(summ1);
            qDebug() << QString("%1 > %2\n").arg(fabs(summ0-summ1)).arg(0.05*fabs(summ0-summ1));
            k++;
        }while(((fabs(summ0-summ1))>(0.05*fabs(summ0-summ1)))&&(k<100));
	
}

int markMassCenter(img2d *imgArr, marksP *mP, int aperture)
{
    qDebug() << QString("\nmarkMassCenter\n");
    STIMG poinT;
    double d0, summ0, summ1;
    summ1 = 0;
    do
    {
        summ0 = summ1;

        if(!detMarkPpix(imgArr, mP, aperture))
        {
            qDebug() << QString("\ndetMarkPpix error\n");
            return 1;
        }

        summ1 = 0;
        //num = mk->marks.size();
        //qDebug() << QString("num= %1\n").arg(num);


            poinT = CenterOfMas(mP->mPpix);
            d0 = sqrt(pow(mP->mTanImg[0]-poinT.X, 2.0)+ pow(mP->mTanImg[1]-poinT.Y, 2.0));

            summ1 = d0;
            mP->mTanImg[0] = poinT.X;
            mP->mTanImg[1] = poinT.Y;


    }while((fabs(summ0-summ1)/summ1)<0.05*summ1);
    return 0;
}

double fitsdata::detFov()
{
        double maxdim;

//	int naxes0;
        double sc;
        sc = 1.0;
/*
        if(instr!=NULL)
        {
            scx = instr->scx;
            scy = instr->scy;
        }
*/
        if(workFrame.width()>workFrame.height()) maxdim = workFrame.width();
        else maxdim = workFrame.height();

        qDebug() << QString("maxdim: %1\n").arg(maxdim);

        sc = ((WCSdata[4]+WCSdata[5])/2.0);

        qDebug() << QString("sc= %1\n").arg(sc);

        maxdim *= sc;


        /*
	marksGrid *mGr;
	if(!is_empty)
	{
                naxes0 =  detNaxes();
                mGr = catMarks;
	}
	else
	{
		max[0] = max[1] = 0;
                mGr = ipixMarks;
                if(mGr->marks.size()>5) naxes0 = mGr->detNaxes();
                else naxes0 = detNaxes();
/*		for(int i=0; i<ipixMarks->marks.size(); i++)
		{
                        if(max[0]<ipixMarks->marks[i]->mTanImg[0]) max[0] = ipixMarks->marks[i]->mTanImg[0];
                        if(max[1]<ipixMarks->marks[i]->mTanImg[1]) max[1] = ipixMarks->marks[i]->mTanImg[1];
		}
		qDebug() << "\nnaxes\n";
		qDebug() << max[0] << "\t" << max[1] << "\n";
//			fcurr->imgArr->naxes[0] = max[0];
//			fcurr->imgArr->naxes[1] = max[1];
		qDebug()  << "\nnaxes\t" << imgArr->naxes[0] << "\t" << imgArr->naxes[1] << "\n";
		if(max[0]>max[1]) naxes0 = max[0];
                else naxes0 = max[1];/
	}
//	qDebug() << "\nnaxes0\t" << naxes0;
//	qDebug() << "\nscales\t" << mGr->scales[0];
//	qDebug() << "\nscFactor\t" << scFactor;
*/
        return(maxdim*60.0);
}

int fitsdata::detWCS1(reductionParams params)
{
    qDebug() << "\ndetWCS1\n";

    QString mTimeCode;

    WCSdata[12] = 0;
    /*
    if(WCSdata[12]!=0)
    {

    }*/
        int i;
        double ksi_oc, eta_oc, xT, yT;
        QVector<int> rsindex;
        double OCp0, OCp1;

        bool singulAR = true;
        marksGrid *mk = refMarks;
        int szRef = mk->marks.size();
        //params.uweReduceMinStars = params.minRefStars;
        if(szRef<params.minRefStars)
        {
            qDebug() << QString("ref stars num = %1 and less than wcsN = %2\n").arg(szRef).arg(params.minRefStars);
            return 0;
        }
 //       int szRef = refMarks->marks.size();
        //detNaxes(&naxes[0], &naxes[1]);
        detTan();

        //qDebug() << QString("\nnaxes: %1 : %2\n").arg(naxes[0]).arg(naxes[1]);

        //prepRefStars();
        prerDataVect(refMarks, WCSdata[2], WCSdata[3], NULL, NULL);

        reductionMaker *redMake = new reductionMaker(params.redType);

        rsindex.clear();

        for(i=0; i<szRef; i++)
        {
            rsindex << i;
        }
/*
        reductionParams redParam;
        redParam.weights = params.weights;
        redParam.sigma = params.sigmaN;
        redParam.maxres = params.maxres;
        redParam.maxresMAG = 2;
        redParam.minRefStars = params.wcsN;
*/
        errBudgetRec *errB = new errBudgetRec;

        singulAR = !redMake->makeReduction(mTimeCode, refMarks, rsindex, params);

        redMake->getParams(errB);
        qDebug() << QString("UWEx= %1\tUWEy= %2\n").arg(errB->xParams.UWE).arg(errB->yParams.UWE);
        qDebug() << QString("Sx= %1\tSy= %2\n").arg(redMake->Sx).arg(redMake->Sy);

        xT = WCSdata[0];//naxes[0]/2.0;
        yT = WCSdata[1];//naxes[1]/2.0;
        qDebug() << QString("xT = %1\tyT = %2\n").arg(xT).arg(yT);
        redMake->detTan(&ksi_oc, &eta_oc, xT, yT);

        getTangToDeg(&OCp0, &OCp1, ksi_oc, eta_oc, WCSdata[2], WCSdata[3]);
        WCSdata[2] = OCp0;
        WCSdata[3] = OCp1;

        //prepRefStars();
        prerDataVect(refMarks, WCSdata[2], WCSdata[3], NULL, NULL);
        //redMake->clear();
        rsindex.clear();
        for(i=0; i<szRef; i++) rsindex << i;
        singulAR = !redMake->makeReduction(mTimeCode, refMarks, rsindex, params);

        redMake->makeErrorBudget(errB);

        Sx = redMake->Sx;
        Sy = redMake->Sy;

        qDebug() << QString("UWEx= %1\tUWEy= %2\n").arg(errB->xParams.UWE).arg(errB->yParams.UWE);
        qDebug() << QString("Sx= %1\tSy= %2\tSmax= %3\n").arg(grad_to_mas(redMake->Sx)).arg(grad_to_mas(redMake->Sy)).arg(params.sMax);

        //singulAR = (grad_to_mas(redMake->Sx)<params.sMax)&&(grad_to_mas(redMake->Sy)<params.sMax)&&(redMake->RNKSI>=params.minRefStars)&&(redMake->RNETA>=params.minRefStars);
        qDebug() << QString("singulAR= %1\n").arg(singulAR);

        WCSdata[12]=singulAR;//п·п²п≥п╝п�� п╗п▒п╝п∙п═, п▓п═п² WCS п░п║п�� п∙п═ п▒п·п╔п╞п╝п°п╝
        WCSdata[0]=errB->Xoc;//+curF->imgArr->naxes[0]/2;//п²п·п²п÷п°п╗п≤ п·п╔п≥п╞п∙п п╜ п·п÷п╔ п╞п⌡п∙п╚п∙п°п╔п╔ п°п╝п▓п╝п п╝ п≥п²п²п÷п�� п╔п°п╝п═ п▒ п п∙п▒п╗п≤ п▒п∙п÷пёп°п╔п≤ п║п╕п²п  п≥п╝п�� п÷п╝ (п·п÷п╝п▒п╔п п² п═п╝п≥п²п∙ п∙п╞п═п╜ п·п÷п╔ п▒п·п╔п╞п╗п▒п╝п°п╔п╔ WCS)
        WCSdata[1]=errB->Yoc;//+curF->imgArr->naxes[1]/2;
        //WCSdata[2]=OCp0;// RA, DEC п²п·п²п÷п°п²п╕п² п·п╔п≥п╞п∙п п╝ п▒ п╕п÷п╝п�� п║п╞п╝пё
        //WCSdata[3]=OCp1;
        WCSdata[4]=sqrt(pow(redMake->ZKSI[0],2)+pow(redMake->ZETA[0],2));// п÷п╝п╞п▓п∙п═ п⌡п╝п╞п╛п═п╝п░п²п▒ п▒п�� п²п п╜ п²п╞п∙п≤ (п╕п÷п╝п�� п║п╞п╗ п°п╝ п·п╔п≥п╞п∙п п╜ - п°п∙п║п�� п²п░п°п², п╝ п▓п═п² п�� п∙п п╝п═п╜ - п╞п═п╝п°п�� п╝п÷п═ п°п╝п�� п² п║п▒п╝п╓п╝п═п╜)
        WCSdata[5]=sqrt(pow(redMake->ZKSI[1],2)+pow(redMake->ZETA[1],2));
        double rho1 =180*atan2(-redMake->ZETA[0]/WCSdata[4],redMake->ZKSI[0]/WCSdata[4])/(atan(1)*4);//п÷п╝п╞п▓п∙п═ п║п╕п п²п▒ п⌡п∙п╓п�� п║ п²п╞п╙п⌡п╔ п═п╝п°п╕п∙п°п√п╔п╝п п╜п°п╗пё п╔ п·п╔п≥п╞п∙п п╜п°п╗пё п≥п²п²п÷п�� п╔п°п╝п═
        double rho2 = 180*atan2(redMake->ZKSI[1]/WCSdata[5],redMake->ZETA[1]/WCSdata[5])/(atan(1)*4);// п║п╕п п²п▒ п�� п▒п╝, п═п╝п≥ п≥п╝п≥ п▒ п²п░п╚п∙п⌡ п╞п п║п▓п╝п∙ п▒п²п�� п⌡п²п╓п°п╝ п≥п²п╞п²п║п╕п²п п╜п°п²п╞п═п╜
        if(rho1<0) rho1 = 360+rho1;
        if(rho2<0) rho2 = 360+rho2;
        WCSdata[6]=rho1;//п�� п╝п·п╔п╞п╗п▒п╝п∙п⌡ п║п╕п п╗
        WCSdata[7]=rho2;
        WCSdata[8]=redMake->ZKSI[0];//п�� п╝п·п╔п╞п╗п▒п╝п∙п⌡ п·п²п╞п═п²п╙п°п°п╗п∙ п≥п╝п�� п÷п╝ (п⌡п╝п╞п╛п═п╝п░+п·п²п▒п²п÷п²п═)
        WCSdata[9]=redMake->ZKSI[1];
        WCSdata[10]=redMake->ZETA[0];
        WCSdata[11]=redMake->ZETA[1];


	qDebug() << "\nWCSdata[12]\t" << WCSdata[12] << "\n";
	qDebug() << "\nWCSdata[0]\t" << WCSdata[0] << "\n";
	qDebug() << "\nWCSdata[1]\t" << WCSdata[1] << "\n";
	qDebug() << "\nWCSdata[2]\t" << WCSdata[2] << "\n";
	qDebug() << "\nWCSdata[3]\t" << WCSdata[3] << "\n";
	qDebug() << "\nWCSdata[4]\t" << WCSdata[4] << "\n";
	qDebug() << "\nWCSdata[5]\t" << WCSdata[5] << "\n";
	qDebug() << "\nWCSdata[6]\t" << WCSdata[6] << "\n";
	qDebug() << "\nWCSdata[7]\t" << WCSdata[7] << "\n";
	qDebug() << "\nWCSdata[8]\t" << WCSdata[8] << "\n";
	qDebug() << "\nWCSdata[9]\t" << WCSdata[9] << "\n";
	qDebug() << "\nWCSdata[10]\t" << WCSdata[10] << "\n";
	qDebug() << "\nWCSdata[11]\t" << WCSdata[11] << "\n";
	
        return singulAR;
	
}

int fitsdata::detWCS(int wcsN, double sMax)
{
        int szP, pNum, i, j;
        double mag, wcsMag1 = 6, wcsMag2 = 10;
        bool singulAR = true;
        marksGrid *mk = refMarks;
        pNum = mk->marks.size();
        if(pNum<wcsN)
        {
            qDebug() << QString("ref stars num = %1 and less than wcsN = %2\n").arg(szP).arg(wcsN);
            return 0;
        }
        //mk->sortMagn();



//        pNum = szP;

        //qDebug() << "\nszP\t" << szP << "\n";

        STDATA stdata;
        double x, y;
        double OCp0, OCp1;
        double *pX, *pY, *wcsKsi, *wcsEta, *psfR, *wcsRA, *wcsDE;
        /*pX = new double[szP];
        pY = new double[szP];
        wcsKsi = new double[szP];
        wcsEta = new double[szP];*/


//	double *ksi, *eta;

        double* const Zx = new double[3];
        double* const Zy = new double[3];
        double *mCD = new double[4];
        double *ImCD = new double[4];
        double *rPix = new double[2];
        double *shC = new double[2];

        double xT, yT, ksi_oc, eta_oc;
//	double Sx, Sy;
//	long naxes[2];

        //detNaxes(&naxes[0], &naxes[1]);
        //qDebug() << QString("naxes: %1\t%2\n").arg(naxes[0]).arg(naxes[1]);
      /*
        for (i=0;i<szP;i++)
        {
            mag = mk->marks[i]->mEkv[2];
            x = mk->marks[i]->mTanImg[0];
            y = mk->marks[i]->mTanImg[1];
            if ((x>0)&&(y>0)&&(x<naxes[0])&&(y<naxes[1]))
            {
                    pX[i] = x;
                    pY[i] = y;
                    wcsKsi[i] = mk->marks[i]->mTan[0];
                    wcsEta[i] = mk->marks[i]->mTan[1];
                    wcsRA[i] = mk->marks[i]->mEkv[0];
                    wcsDE[i] = mk->marks[i]->mEkv[1];
                    //pNum+=1;
            }
        }
*/
        for (i=pNum-1;i>=0;i--)
        {
            x = mk->marks[i]->mTanImg[0];
            y = mk->marks[i]->mTanImg[1];
            qDebug() << QString("x= %1\ty= %2\n").arg(x).arg(y);
            //if((x<0)||(y<0)||(x>naxes[0])||(y>naxes[1]))mk->remMark(i);
            if(!workFrame.contains(x,y))mk->remMark(i);
        }

        pNum = mk->marks.size();
        qDebug() << "\npNum\t" << pNum << "\n";

        qDebug() << "\nra\tde\tmagn\tksi\teta\tx\ty\t\n";
        for(i = 0; i< pNum; i++)
        {
                qDebug() << "\n" << QString("%1\t%2\t").arg(mk->marks[i]->mEkv[0], 12, 'f', 2).arg(mk->marks[i]->mEkv[1], 12, 'f', 2) << mk->marks[i]->mEkv[2] << mk->marks[i]->mTan[0] << mk->marks[i]->mTan[1] << mk->marks[i]->mTanImg[0] << mk->marks[i]->mTanImg[1] << "\n";
        }


        //if(pNum<wcsN) return 1;
/*
        if((WCSdata[2]==0)&&(WCSdata[3]==0))//п∙п╞п п╔ RA, DEC п²п·п═п╔п▓п∙п╞п≥п²п╕п² п√п∙п°п═п÷п╝ п°п∙ п░п╗п п╔ п�� п╝п�� п╝п°п╗ п▒ п╛п╝п·п≥п∙ fits-п╒п╝п≤п п╝, п▒п╗п▓п╔п╞п п╙п∙п⌡ п╔пё п≥п╝п≥ п╞п÷п∙п�� п°п╔п∙ п╔п��  п≥п²п²п÷п�� п╔п°п╝п═ п²п·п²п÷п°п╗пё п�� п▒п∙п�� п�� 
        {
                stdata = StDataFromVector(wcsRA, pNum);
                WCSdata[2] = mas_to_grad(stdata.mean);
                stdata = StDataFromVector(wcsDE, pNum);
                WCSdata[3] = mas_to_grad(stdata.mean);
        }*/
        OCp0 = WCSdata[2];
        OCp1 = WCSdata[3];

        qDebug() << "\n" << QString("OC: %1 %2").arg(OCp0, 12, 'f', 6).arg(OCp1, 12, 'f', 6) << "\n";

        double* pixP;
        double* We;
        double* ksi;
        double* eta;
        double* pixKsi;
        double* pixEta;
        int *exclindX;
        int *exclindY;
        double Dx[3][3];
        double Dy[3][3];
        double uweX, uweY;
        int rnX, rnY;
        double errPos, err;
        int errMax, pos;

        do
        {

            //pNum = 0;
            pNum = mk->marks.size();
            qDebug() << QString("pNum %1\n").arg(pNum);
            if(pNum<wcsN) return 0;
       /*     for (i=0;i<pNum;i++)
            {
                mag = mk->marks[i]->mEkv[2];
                x = mk->marks[i]->mTanImg[0];
                y = mk->marks[i]->mTanImg[1];

                pX[i] = x;
                pY[i] = y;
                wcsKsi[i] = mk->marks[i]->mTan[0];
                wcsEta[i] = mk->marks[i]->mTan[1];
                wcsRA[i] = mk->marks[i]->mEkv[0];
                wcsDE[i] = mk->marks[i]->mEkv[1];
//                pNum+=1;

            }
*/
            pixP = new double[3*pNum];
            We = new double[pNum];
            ksi = new double[pNum];
            eta = new double[pNum];
            pixKsi = new double[pNum];
            pixEta = new double[pNum];

            exclindX = new int[pNum];
            exclindY = new int[pNum];
            wcsRA = new double[pNum];
            wcsDE = new double[pNum];
            pX = new double[pNum];
            pY = new double[pNum];


            for (i=0;i<pNum;i++)
            {
                    pixP[i*3] = mk->marks[i]->mTanImg[0];
                    pixP[i*3+1] = mk->marks[i]->mTanImg[1];
                    pixP[i*3+2] = 1;
                    pX[i] = mk->marks[i]->mTanImg[0];
                    pY[i] = mk->marks[i]->mTanImg[1];
                    ksi[i] = mk->marks[i]->mTan[0];
                    eta[i] = mk->marks[i]->mTan[1];
                    We[i]=1;
                    wcsRA[i] = mk->marks[i]->mEkv[0];
                    wcsDE[i] = mk->marks[i]->mEkv[1];

                    //qDebug() << QString("%1 + %2 + %3 = %4 | %5\n").arg(pixP[i*3]).arg(pixP[i*3+1]).arg(pixP[i*3+2]).arg(ksi[i]).arg(eta[i]);

            }

            for (i=0;i<pNum;i++)
            {
                    qDebug() << "a1*" << pixP[i*3] << " + b1*" << pixP[i*3+1] << " + c1*" << pixP[i*3+2] << " = " << ksi[i] << " = " << eta[i] << " = " << We[i]  << "\n";
            }

//	slsm(3, pNum, Zx, pixP, wcsKsi, We);
            qDebug() << QString("\nFirst stage\n");
        iLSM(3, pNum, mas_to_grad(sMax), exclindX, Zx, pixP, ksi, uweX, &Dx[0][0], 3.0, rnX, We);
        qDebug() << "\n" << QString("Zx: %1\t%2\t%3").arg(Zx[0], 12, 'g').arg(Zx[1], 12, 'g').arg(Zx[2], 12, 'g') << "\n";
        qDebug() << "\nrnX: " << rnX << "\n";
        qDebug() << "\nuweX: " << grad_to_mas(uweX) << "\n";
 //

 //       QMessageBox::information(0, "Zx", QString("%1\t%2\t%3").arg(Zx[0]).arg(Zx[1]).arg(Zx[2]));
//	slsm(3, pNum, Zy, pixP, wcsEta, We);
        iLSM(3, pNum, mas_to_grad(sMax), exclindY, Zy, pixP, eta, uweY, &Dy[0][0], 3.0, rnY, We);
        qDebug() << "\n" << QString("Zy: %1\t%2\t%3").arg(Zy[0], 12, 'g').arg(Zy[1], 12, 'g').arg(Zy[2], 12, 'g') << "\n";
        qDebug() << "\nrnY: " << rnY << "\n";
        qDebug() << "\nuweY: " << grad_to_mas(uweY) << "\n";

        if(rnX<wcsN||rnY<wcsN)
        {
            qDebug() << QString("too small rn on first stage: rnX= %1\trnY= %2\n").arg(rnX).arg(rnY);

            errPos = -1;
            errMax = 0.0;
            for(i=0; i<pNum; i++)
            {


                pixKsi[i] = Zx[0]*pX[i]+Zx[1]*pY[i]+Zx[2];
                pixEta[i] = Zy[0]*pX[i]+Zy[1]*pY[i]+Zy[2];

                err = sqrt(pow(ksi[i] - pixKsi[i],2) + pow(eta[i] - pixEta[i],2));
                qDebug() << "\n" << QString("pixKsi= %1\tpixEta= %2\terr= %3\n").arg(pixKsi[i]).arg(pixEta[i]).arg(err, 10, 'e', 7);
                qDebug() << "\n" << QString("ksi= %1\teta= %2\terr= %3\n").arg(ksi[i]).arg(eta[i]);
                if(err>errMax)
                {
                    errPos = i;
                    errMax = err;
                }
            }
            qDebug() << QString("errPos= %1\n").arg(errPos);

            if(errPos!=-1)mk->remMark(errPos);

            delete [] pixP;
            delete [] We;
            delete [] ksi;
            delete [] eta;
            delete [] pixKsi;
            delete [] pixEta;

            delete [] exclindX;
            delete [] exclindY;
            //pNum--;
            continue;
        }

//	QMessageBox::information(0, "Zy", QString("%1\t%2\t%3").arg(Zy[0]).arg(Zy[1]).arg(Zy[2]));
//        xT = 0.0;
//        yT = 0.0;
        xT = WCSdata[0];//naxes[0]/2.0;
        yT = WCSdata[1];//naxes[1]/2.0;
        qDebug() << QString("xT = %1\tyT = %2\n").arg(xT).arg(yT);
//        xT = (Zx[1]*Zy[2] - Zy[1]*Zx[2])/(Zx[0]*Zy[1] + Zy[0]*Zx[1]);
//        yT = (Zy[0]*Zx[2] - Zx[0]*Zy[2])/(Zx[0]*Zy[1] + Zy[0]*Zx[1]);
        ksi_oc = Zx[0]*xT + Zx[1]*yT + Zx[2];
        eta_oc = Zy[0]*xT + Zy[1]*yT + Zy[2];
        getTangToDeg(&OCp0, &OCp1, ksi_oc, eta_oc, OCp0, OCp1);

        qDebug() << "\nksi_oc " << ksi_oc << "\teta_oc" << eta_oc << "\n";
        qDebug() << "\n" << QString("OC: %1 %2").arg(OCp0, 12, 'f', 6).arg(OCp1, 12, 'f', 6) << "\n";
/*
        Sx = Sy = 0;
        for(i=0;i<pNum;i++)//п╞п▓п╔п═п╝п∙п⌡ п²п╛п╔п░п≥п╔ п∙п�� п╔п°п╔п√п╗ п▒п∙п╞п╝.
        {
                pixKsi[i] = Zx[0]*pX[i]+Zx[1]*pY[i]+Zx[2];
                pixEta[i] = Zy[0]*pX[i]+Zy[1]*pY[i]+Zy[2];
                qDebug() << QString("ksi: %1 ?= %2\t\teta: %3 ?= %4\n").arg(ksi[i], 8, 'f', 3).arg(pixKsi[i], 8, 'f', 3).arg(eta[i], 8, 'f', 3).arg(pixEta[i], 8, 'f', 3);
                Sx=Sx+pow(ksi[i] - pixKsi[i],2);
                Sy=Sy+pow(eta[i] - pixEta[i],2);

        }
        Sx = sqrt(Sx/(pNum-1));
        Sy = sqrt(Sy/(pNum-1));
        qDebug() << "\nSx = " << Sx*1000.0 << "\nSy = " << Sy*1000.0 << "\n";
*/
        double tp0, tp1;
        for (i=0;i<pNum;i++)//п·п∙п÷п∙п╞п▓п╔п═п╗п▒п╝п∙п⌡ п═п╝п°п╕п∙п°п√п╔п╝п п╜п°п╗п∙ п≥п²п²п÷п�� п╔п°п╝п═п╗ п²п·п²п÷п°п╗пё п�� п▒п∙п�� п��  п╞ п°п²п▒п╗п⌡ п²п·п╔п═п▓п∙п╞п≥п╔п⌡ п√п∙п°п═п÷п²п⌡
        {
                getDegToTang(&tp0, &tp1, wcsRA[i], wcsDE[i], OCp0, OCp1);
                ksi[i]=tp0;
                eta[i]=tp1;
                qDebug() << "\nKsi\t" << ksi[i] << "\tEta\t" << eta[i] << "\n";
        }
//	Zx[0] = Zx[1] = Zx[2] = 0.0;
//	Zy[0] = Zy[1] = Zy[2] = 0.0;
        qDebug() << QString("\nSECOND STAGE\n");
//	slsm(3, pNum, Zx, pixP, wcsKsi, We);
        iLSM(3, pNum, mas_to_grad(sMax), exclindX, Zx, pixP, ksi, uweX, &Dx[0][0], 3.0, rnX, We);
        qDebug() << "\n" << QString("Zx: %1\t%2\t%3").arg(Zx[0], 12, 'g').arg(Zx[1], 12, 'g').arg(Zx[2], 12, 'g') << "\n";
        qDebug() << "\nrnX: " << rnX << "\n";
        qDebug() << "\nuweX: " << grad_to_mas(uweX) << "\n";
//	QMessageBox::information(0, "Zx", QString("%1\t%2\t%3").arg(Zx[0]).arg(Zx[1]).arg(Zx[2]));
//	printf("\n");
//	slsm(3, pNum, Zy, pixP, wcsEta, We);
        iLSM(3, pNum, mas_to_grad(sMax), exclindY, Zy, pixP, eta, uweY, &Dy[0][0], 3.0, rnY, We);
        qDebug() << "\n" << QString("Zy: %1\t%2\t%3").arg(Zy[0], 12, 'g').arg(Zy[1], 12, 'g').arg(Zy[2], 12, 'g') << "\n";
        qDebug() << "\nrnY: " << rnY << "\n";
        qDebug() << "\nuweY: " << grad_to_mas(uweY) << "\n";
//	QMessageBox::information(0, "Zy", QString("%1\t%2\t%3").arg(Zy[0]).arg(Zy[1]).arg(Zy[2]));

        Sx = Sy = 0;
        for(i=0;i<pNum;i++)//п╞п▓п╔п═п╝п∙п⌡ п²п╛п╔п░п≥п╔ п∙п�� п╔п°п╔п√п╗ п▒п∙п╞п╝.
        {
                pixKsi[i] = Zx[0]*pX[i]+Zx[1]*pY[i]+Zx[2];
                pixEta[i] = Zy[0]*pX[i]+Zy[1]*pY[i]+Zy[2];
                //qDebug() << QString("ksi: %1 ?= %2\t\teta: %3 ?= %4\n").arg(ksi[i], 8, 'f', 3).arg(pixKsi[i], 8, 'f', 3).arg(eta[i], 8, 'f', 3).arg(pixEta[i], 8, 'f', 3);
                Sx=Sx+pow(ksi[i] - pixKsi[i],2);
                Sy=Sy+pow(eta[i] - pixEta[i],2);

        }
        Sx = sqrt(Sx/(pNum-1));
        Sy = sqrt(Sy/(pNum-1));
        qDebug() << "\nSx = " << grad_to_mas(Sx) << "\nSy = " << grad_to_mas(Sy) << "\t" << "\nsMax = " << sMax << "\n";

        if(rnX<wcsN||rnY<wcsN||Sx>mas_to_grad(sMax)||Sy>mas_to_grad(sMax))
        {
            qDebug() << QString("too small rn or S on second stage: rnX= %1\trnY= %2\n").arg(rnX).arg(rnY);
            errPos = -1;
            errMax = 0.0;

            for(i=0; i<pNum; i++)
            {


                pixKsi[i] = Zx[0]*pX[i]+Zx[1]*pY[i]+Zx[2];
                pixEta[i] = Zy[0]*pX[i]+Zy[1]*pY[i]+Zy[2];


                err = sqrt(pow(ksi[i] - pixKsi[i],2) + pow(eta[i] - pixEta[i],2));

                qDebug() << "\n" << QString("pixKsi= %1\tpixEta= %2\terr= %3\n").arg(pixKsi[i]).arg(pixEta[i]).arg(err);
                qDebug() << "\n" << QString("ksi= %1\teta= %2\terr= %3\n").arg(ksi[i]).arg(eta[i]).arg(err, 10, 'e', 7);
                if(err>errMax)
                {
                    errPos = i;
                    errMax = err;
                }
            }

            if(errPos!=-1)mk->remMark(errPos);

            delete [] pixP;
            delete [] We;
            delete [] ksi;
            delete [] eta;
            delete [] pixKsi;
            delete [] pixEta;

            delete [] exclindX;
            delete [] exclindY;
            //pNum--;
            continue;
        }
        else break;
        }while(pNum>=wcsN);

/////////////////
        xT = WCSdata[0];//naxes[0]/2.0;
        yT = WCSdata[1];//naxes[1]/2.0;
//	xT = (Zx[1]*Zy[2] - Zy[1]*Zx[2])/(Zx[0]*Zy[1] + Zy[0]*Zx[1]);
//	yT = (Zy[0]*Zx[2] - Zx[0]*Zy[2])/(Zx[0]*Zy[1] + Zy[0]*Zx[1]);
        ksi_oc = Zx[0]*xT + Zx[1]*yT + Zx[2];
        eta_oc = Zy[0]*xT + Zy[1]*yT + Zy[2];
        qDebug() << QString("xT = %1\tyT = %2\n").arg(xT).arg(yT);
        qDebug() << "\nksi_oc " << ksi_oc << "\teta_oc" << eta_oc << "\n";
        qDebug() << "\n" << QString("OC: %1 %2").arg(OCp0, 12, 'f', 6).arg(OCp1, 12, 'f', 6) << "\n";
/////////////////

/*
        Sx = Sy = 0;
        for(i=0;i<pNum;i++)//п╞п▓п╔п═п╝п∙п⌡ п²п╛п╔п░п≥п╔ п∙п�� п╔п°п╔п√п╗ п▒п∙п╞п╝.
        {
                pixKsi[i] = Zx[0]*pixP[i*3]+Zx[1]*pixP[i*3+1]+Zx[2];
                pixEta[i] = Zy[0]*pixP[i*3]+Zy[1]*pixP[i*3+1]+Zy[2];
             //   qDebug() << QString("ksi: %1 ?= %2\t\teta: %3 ?= %4\n").arg(ksi[i], 8, 'f', 3).arg(pixKsi[i], 8, 'f', 3).arg(eta[i], 8, 'f', 3).arg(pixEta[i], 8, 'f', 3);
                Sx=Sx+pow(ksi[i] - pixKsi[i],2);
                Sy=Sy+pow(eta[i] - pixEta[i],2);

        }
        Sx = sqrt(Sx/(pNum-1));
        Sy = sqrt(Sy/(pNum-1));
        qDebug() << "\nSx = " << Sx*1000.0 << "\nSy = " << Sy*1000.0 << "\t" << "\nSy = " << sMax << "\n";*/
        if(Sx>mas_to_grad(sMax)||Sy>mas_to_grad(sMax)) singulAR *= false;
        qDebug() << "\nres = " <<  singulAR << "\n";

        mCD[0] = Zx[0]; mCD[1] = Zx[1];
        mCD[2] = Zy[0]; mCD[3] = Zy[1];
//	qDebug() << "mCD\n" << mCD[0] << " " << mCD[1] << "\n" << mCD [2] << " " << mCD[3] << "\n";

        shC[0]=-Zx[2];shC[1]=-Zy[2];//п▒п⌡п∙п╞п═п² п╞п▒п²п░п²п�� п°п²п╕п² п·п╝п÷п╝п⌡п∙п═п÷п╝ пёп÷п╝п°п╙п═п╞п╙ п≥п²п²п÷п�� п╔п°п╝п═п╗ п²п·п²п÷п°п²п╕п² (п√п∙п°п═п÷п╝п п╜п°п²п╕п²) п·п╔п≥п╞п∙п п╝
//   qDebug() << "shC\n" << shC[0] << " " << shC[1] << "\n";


        singulAR *= invgj(ImCD, mCD, 2);
        qDebug() << "\ninvgj res = " <<  singulAR << "\n";
 //  qDebug() << "ImCD\n" << ImCD[0] << " " << ImCD[1] << "\n" << ImCD [2] << " " << ImCD[3] << "\n";
        singulAR *= mvprod(rPix, ImCD, shC, 2, 2);
        qDebug() << "\nmvprod res = " <<  singulAR << "\n";
//	qDebug() << "\nrPix\t" << rPix[0] << " " << rPix[1] << "\n";
/*
        for(int j=0; j<3; j++)
        {
                Zx[j] /= 3600.0; Zy[j] /= 3600.0;
        }
        */
//  	double *WCSdata = new double[12];

        WCSdata[12]=singulAR;//п·п²п≥п╝п�� п╗п▒п╝п∙п═, п▓п═п² WCS п░п║п�� п∙п═ п▒п·п╔п╞п╝п°п╝
        WCSdata[0]=rPix[0];//+curF->imgArr->naxes[0]/2;//п²п·п²п÷п°п╗п≤ п·п╔п≥п╞п∙п п╜ п·п÷п╔ п╞п⌡п∙п╚п∙п°п╔п╔ п°п╝п▓п╝п п╝ п≥п²п²п÷п�� п╔п°п╝п═ п▒ п п∙п▒п╗п≤ п▒п∙п÷пёп°п╔п≤ п║п╕п²п  п≥п╝п�� п÷п╝ (п·п÷п╝п▒п╔п п² п═п╝п≥п²п∙ п∙п╞п═п╜ п·п÷п╔ п▒п·п╔п╞п╗п▒п╝п°п╔п╔ WCS)
        WCSdata[1]=rPix[1];//+curF->imgArr->naxes[1]/2;
        WCSdata[2]=OCp0;// RA, DEC п²п·п²п÷п°п²п╕п² п·п╔п≥п╞п∙п п╝ п▒ п╕п÷п╝п�� п║п╞п╝пё
        WCSdata[3]=OCp1;
        WCSdata[4]=sqrt(pow(Zx[0],2)+pow(Zy[0],2));// п÷п╝п╞п▓п∙п═ п⌡п╝п╞п╛п═п╝п░п²п▒ п▒п�� п²п п╜ п²п╞п∙п≤ (п╕п÷п╝п�� п║п╞п╗ п°п╝ п·п╔п≥п╞п∙п п╜ - п°п∙п║п�� п²п░п°п², п╝ п▓п═п² п�� п∙п п╝п═п╜ - п╞п═п╝п°п�� п╝п÷п═ п°п╝п�� п² п║п▒п╝п╓п╝п═п╜)
        WCSdata[5]=sqrt(pow(Zx[1],2)+pow(Zy[1],2));
        double rho1 =180*atan2(-Zy[0]/WCSdata[4],Zx[0]/WCSdata[4])/(atan(1)*4);//п÷п╝п╞п▓п∙п═ п║п╕п п²п▒ п⌡п∙п╓п�� п║ п²п╞п╙п⌡п╔ п═п╝п°п╕п∙п°п√п╔п╝п п╜п°п╗пё п╔ п·п╔п≥п╞п∙п п╜п°п╗пё п≥п²п²п÷п�� п╔п°п╝п═
        double rho2 = 180*atan2(Zx[1]/WCSdata[5],Zy[1]/WCSdata[5])/(atan(1)*4);// п║п╕п п²п▒ п�� п▒п╝, п═п╝п≥ п≥п╝п≥ п▒ п²п░п╚п∙п⌡ п╞п п║п▓п╝п∙ п▒п²п�� п⌡п²п╓п°п╝ п≥п²п╞п²п║п╕п²п п╜п°п²п╞п═п╜
        if(rho1<0) rho1 = 360+rho1;
        if(rho2<0) rho2 = 360+rho2;
        WCSdata[6]=rho1;//п�� п╝п·п╔п╞п╗п▒п╝п∙п⌡ п║п╕п п╗
        WCSdata[7]=rho2;
        WCSdata[8]=Zx[0];//п�� п╝п·п╔п╞п╗п▒п╝п∙п⌡ п·п²п╞п═п²п╙п°п°п╗п∙ п≥п╝п�� п÷п╝ (п⌡п╝п╞п╛п═п╝п░+п·п²п▒п²п÷п²п═)
        WCSdata[9]=Zx[1];
        WCSdata[10]=Zy[0];
        WCSdata[11]=Zy[1];

        qDebug() << "\nWCSdata[12]\t" << WCSdata[12] << "\n";
        qDebug() << "\nWCSdata[0]\t" << WCSdata[0] << "\n";
        qDebug() << "\nWCSdata[1]\t" << WCSdata[1] << "\n";
        qDebug() << "\nWCSdata[2]\t" << WCSdata[2] << "\n";
        qDebug() << "\nWCSdata[3]\t" << WCSdata[3] << "\n";
        qDebug() << "\nWCSdata[4]\t" << WCSdata[4] << "\n";
        qDebug() << "\nWCSdata[5]\t" << WCSdata[5] << "\n";
        qDebug() << "\nWCSdata[6]\t" << WCSdata[6] << "\n";
        qDebug() << "\nWCSdata[7]\t" << WCSdata[7] << "\n";
        qDebug() << "\nWCSdata[8]\t" << WCSdata[8] << "\n";
        qDebug() << "\nWCSdata[9]\t" << WCSdata[9] << "\n";
        qDebug() << "\nWCSdata[10]\t" << WCSdata[10] << "\n";
        qDebug() << "\nWCSdata[11]\t" << WCSdata[11] << "\n";

        return singulAR;

}

void fitsdata::dropWCS()
{
    WCSdata[0] = workFrame.center().x();
    WCSdata[1] = workFrame.center().y();
    for(int i=4; i<13; i++) WCSdata[i] = 0;
}


int fitsdata::saveWCS()
{
	 ///////////////////////////////////п÷п╝п░п²п═п╝п∙п⌡ п╞ fits-п╒п╝п≤п п²п⌡,  п�� п╝п·п╔п╞п╗п▒п╝п∙п⌡ п▒п╞п∙ п≥п п╖п▓п╔ п╔п��  п⌡п╝п╞п╞п╔п▒п╝ imgF.wcsD
   QString mstr(fileName);
   QByteArray curr_fn = mstr.toAscii();
   char *ffname = curr_fn.data();
   fitsfile *fptR;
   int statuS = 0;//status

   fits_open_file(&fptR, ffname , READWRITE,&statuS);
   if(statuS) return 1;
   mstr = "RA---TAN";
   QByteArray baStr1 = mstr.toAscii();
   char *chStr1 = baStr1.data();
   fits_update_key(fptR, TSTRING, "CTYPE1", chStr1, " ", &statuS);
   statuS = 0;
   mstr = "DEC--TAN";
   QByteArray baStr2 = mstr.toAscii();
   char *chStr2 = baStr2.data();
   fits_update_key(fptR, TSTRING, "CTYPE2", chStr2, " ", &statuS);
   statuS = 0;
   double prToFit = WCSdata[0]; 
   fits_update_key(fptR, TDOUBLE, "CRPIX1", &prToFit, "ref pix 1", &statuS);
   statuS = 0;
   prToFit = WCSdata[1]; 
   fits_update_key(fptR, TDOUBLE, "CRPIX2", &prToFit, "ref pix 2", &statuS);
   statuS = 0;
   prToFit = WCSdata[2]; 
   fits_update_key(fptR, TDOUBLE, "CRVAL1", &prToFit, "RA at ref pix", &statuS);
   statuS = 0;
   prToFit = WCSdata[3]; 
   fits_update_key(fptR, TDOUBLE, "CRVAL2", &prToFit, "DEC at ref pix", &statuS);
   statuS = 0;
   prToFit = WCSdata[4]; 
   fits_update_key(fptR, TDOUBLE, "CDELT1", &prToFit, "scale1 [deg/pix]", &statuS);
   statuS = 0;
   prToFit = WCSdata[5]; 
   fits_update_key(fptR, TDOUBLE, "CDELT2", &prToFit, "scale2 [deg/pix]", &statuS);
   statuS = 0;
   prToFit = WCSdata[6]; 
   fits_update_key(fptR, TDOUBLE, "CROTA1", &prToFit, "rotation1 [deg]", &statuS);
   statuS = 0;
   prToFit = WCSdata[7]; 
   fits_update_key(fptR, TDOUBLE, "CROTA2", &prToFit, "rotation2 [deg]", &statuS);
   statuS = 0;
   prToFit = WCSdata[8]; 
   fits_update_key(fptR, TDOUBLE, "CD1_1", &prToFit, "WCS CD matrix", &statuS);
   statuS = 0;
   prToFit = WCSdata[9]; 
   fits_update_key(fptR, TDOUBLE, "CD1_2", &prToFit, "WCS CD matrix", &statuS);
   statuS = 0;
   prToFit = WCSdata[10]; 
   fits_update_key(fptR, TDOUBLE, "CD2_1", &prToFit, "WCS CD matrix", &statuS);
   statuS = 0;
   prToFit = WCSdata[11]; 
   fits_update_key(fptR, TDOUBLE, "CD2_2", &prToFit, "WCS CD matrix", &statuS);
   statuS = 0;
   getCatName(&mstr, catMarks->ctype);
   QByteArray baStr3 = mstr.toAscii();
   char *chStr3 = baStr3.data();
   fits_update_key(fptR, TSTRING, "WCSRFCAT", chStr3, "wcs catalog", &statuS);
   statuS = 0;
   mstr = "deg";
   QByteArray baStr4 = mstr.toAscii();
   char *chStr4 = baStr4.data();
   fits_update_key(fptR, TSTRING, "CUNIT1", chStr4, "unit 1", &statuS);
   statuS = 0;
   QByteArray baStr5 = mstr.toAscii();
   char *chStr5 = baStr5.data();
   fits_update_key(fptR, TSTRING, "CUNIT2", chStr5, "unit 2", &statuS);
   statuS = 0;
   double val = grad_to_mas(Sx);
   fits_update_key(fptR, TDOUBLE, "WCSUWE1", &val, "unit weight error in mas", &statuS);
   statuS = 0;
   val = grad_to_mas(Sy);
   fits_update_key(fptR, TDOUBLE, "WCSUWE2", &val, "unit weight error in mas", &statuS);
   statuS = 0;
   fits_close_file(fptR, &statuS);
   
	return 0;
}
/*
void fitsdata::getMarksGrid(catFinder *sCat, double fovp, double minM, double maxM, int sNum)
{
	double ra, de, pmra, pmde, magn;
//	double *xy;
//	xy = new double[2];
	double *ekk, *tank;
	int *itank;
	/*double *ekk = new double[2];
	double *tank = new double[2];
        int *itank = new int[2]; /
	double ksi, eta;
        double fov, fov0;
	
	QString elS;
        marksP *mT;
        //ucac3Rec *u3rec;
	
	qDebug() << "\nra: " << mas_to_hms(grad_to_mas(WCSdata[2]), ":", 3) << "\nde: " << mas_to_damas(grad_to_mas(WCSdata[3]),":",2) << "\n";
	
                marksGrid *mGr;
//                if(WCSdata[12]) mGr = ipixMarks;
                mGr = new marksGrid;
//		mGr->clearMarks();
		if(WCSdata[12]){mGr->xc = mGr->yc = 0.0;}
		qDebug() << "\nMJD\t" << MJD << "\n";
		
//		QMessageBox::information(0, tr("info"), QString("%1").arg(fitsd->WCSdata[12]), QMessageBox::Ok);
                fov0 = detFov();
                fov = fovp*fov0/2.0;// 0.01*fitsd->imgArr->naxes[0]*mGr->scales[0]/60000.0/fitsd->scFactor;
		qDebug() << "\nfov\t" << fov << "\n";
		qDebug() << "\nfovp\t" << fovp << "\n";
//		QMessageBox::information(NULL, tr("fov"), QString("%1").arg(fov), QMessageBox::Ok);
                qDebug() << "sCat: " << sCat << "\n";
			
                        mGr->ctype = sCat->catType;
			QProcess outerProcess;
			QStringList outerArguments;
			outerArguments.clear();
			QString ucac2find_folder("");
			switch(mGr->ctype)
			{
                        case UCAC2_CAT_NUM://ucac2
                                outerProcess.setWorkingDirectory(sCat->exePath);
				outerProcess.setProcessChannelMode(QProcess::MergedChannels);
				outerProcess.setReadChannel(QProcess::StandardOutput);
                                outerArguments << "-r" << mas_to_hms(grad_to_mas(WCSdata[2]), ":", 3) << mas_to_damas(grad_to_mas(WCSdata[3]),":",2) << QString(" %1").arg(fov) << QString(" %1").arg(minM) << QString(" %1 ").arg(maxM) << "-stdout";
                                qDebug() << sCat->exeName << outerArguments.join(" ");
                                outerProcess.start(sCat->exeName, outerArguments);
				break;
                        case USNOB_CAT_NUM://usno-b1
                                outerProcess.setWorkingDirectory(sCat->exePath);
                                outerProcess.setProcessChannelMode(QProcess::MergedChannels);
                                outerProcess.setReadChannel(QProcess::StandardOutput);
                                outerArguments << "USNO-B1.0" << mas_to_hms(grad_to_mas(WCSdata[2]), ":", 3) << mas_to_damas(grad_to_mas(WCSdata[3]),":",2) << QString("b=%1").arg(fov/2.0);
                                qDebug() << sCat->exeName << outerArguments.join(" ");
                                outerProcess.start(sCat->exeName, outerArguments);
				break;
                        case UCAC3_CAT_NUM://ucac3
                                outerProcess.setWorkingDirectory(sCat->exePath);
                                outerProcess.setProcessChannelMode(QProcess::MergedChannels);
                                outerProcess.setReadChannel(QProcess::StandardOutput);
                                outerArguments << mas_to_hms(grad_to_mas(WCSdata[2]), ":", 3) << mas_to_damas(grad_to_mas(WCSdata[3]),":",2) << QString("r=%1").arg(fov/2.0) << QString("%1").arg(sCat->catPath);
                                qDebug() << sCat->exeName << outerArguments.join(" ");
                                outerProcess.start(sCat->exeName, outerArguments);

                                //u3Rec = new ucac3Rec();
                                break;
			}
//			outerProcess.waitForStarted(10);
			outerProcess.waitForFinished(-1);
			QTextStream catStream(outerProcess.readAllStandardOutput());

			QString catLine;
                        marksP *mkTemp;
                        int hlCount = 0;
					
			while (!catStream.atEnd())
			{
				catLine = catStream.readLine();

                                //qDebug() << QString("catLine:%1\n").arg(catLine);
//				QMessageBox::information(NULL, tr("Marks num"), catLine, QMessageBox::Ok);
                                ///qDebug() << QString("cl %1\n").arg(catLine);
                                /*if(catLine.size()<26)
				{
//s					QMessageBox::information(NULL, tr("Marks num"), "ucac2find error", QMessageBox::Ok);
                                    qDebug() << QString("cllen= %1\n").arg(catLine.size());
					continue;
                                }/
				switch(mGr->ctype)
				{
                                        case UCAC2_CAT_NUM://ucac2
					{
                                                elS = catLine.section('|',0,0);ra = hms_to_deg(elS, " ");
                                                elS = catLine.section('|',1,1);de = damas_to_deg(elS, " ");
						elS = catLine.section('|',11,11);pmra = elS.toDouble();
						elS = catLine.section('|',12,12);pmde = elS.toDouble();
						elS = catLine.section('|',2,2);magn = elS.toDouble();
//						QMessageBox::information(NULL, tr("catLine"), QString("%1").arg(catLine), QMessageBox::Ok);

                                                ra = ra +mas_to_grad(pmra*(getYearFromMJD(MJD)-2000));//taking proper motion into account
                                                de = de +mas_to_grad(pmde*(getYearFromMJD(MJD)-2000));//taking proper motion into account
                                //                qDebug() << "\nra\t" << ra << "\tde\t" << de << "\n";
						
						ekk = new double[2];
						tank = new double[2];
						itank = new int[2];
					
						ekk[0] = ra; ekk[1] = de;

//						QMessageBox::information(NULL, tr("Marks num"), QString("%1 %2").arg(ekk[0]).arg(ekk[1]), QMessageBox::Ok);
//						if(!fitsd->WCSdata[12])
//						{	
//							ekk[0] = ra; ekk[1] = de;
/*									
							mGr->mEkv.append(ekk);
							getRaDeToTang1(&ksi, &eta, ra, de, grad_to_mas(fitsd->WCSdata[2]), grad_to_mas(fitsd->WCSdata[3]));//п▒ mas'п╝пё
//							tank[0] = ksi; tank[1] = eta;
/*						}
						else
						{
							ekk = getCposFromWCS(ra, de, fitsd->WCSdata);
							QMessageBox::information(NULL, tr("Marks num"), QString("%1 %2").arg(ekk[0]).arg(ekk[1]), QMessageBox::Ok);
//							tank = getPixPosFromWCS(ra, de, fitsd->WCSdata);
                                                }/
                                                mT = mGr->addEkvMark(ra, de, magn);

                                                //mT->resUcac2->u2R = magn;

                                                /*mT->resUcac2->ra = ra;
                                                mT->resUcac2->de = de;
                                                mT->resUcac2->mag = magn;*/
                                                /*elS = catLine.section('|',18,18);mT->resUcac2->J=elS.toDouble();//Jmag from 2MASS
                                                elS = catLine.section('|',19,19);mT->resUcac2->H=elS.toDouble();//Hmag from 2MASS
                                                elS = catLine.section('|',20,20);mT->resUcac2->Ks=elS.toDouble();//Ks_mag from 2MASS*/



/*						mGr->mTan.append(tank);
						
						rot2D(tank, mGr->rotAn);
						itank[0] = tank[0]/mGr->scales[0] + fitsd->naxes[0]/2;
						itank[1] = tank[1]/mGr->scales[1] + fitsd->naxes[1]/2;
                                                mGr->mTanImg.append(itank);/
						
//						QMessageBox::information(0, tr("info"), QString("%1 %2 %3\n%4 %5\n%6 %7").arg(ekk[0]).arg(ekk[1]).arg(getYearFromMJD(fitsd->MJD)-2000).arg(tank[0]).arg(tank[1]).arg(itank[0]).arg(itank[1]), QMessageBox::Ok);
						break;
						}
                                        case USNOB_CAT_NUM://usno-b1
                                            {
                                                //qDebug() << QString("hlCount: %1\n").arg(hlCount);
                                                if(catLine.indexOf('#')==0) continue;
                                                else if(catLine.size()==0) continue;
                                                else if(hlCount<4)
                                                {
                                                    hlCount++;
                                                    continue;
                                                }

                                                mkTemp = new marksP(OBJ_TYPE_USNOB);
                                                //mkTemp->u3Rec = new ucac3Rec;



                                                mkTemp->usnobRec->s2rec(catLine);

                                                ra = mkTemp->usnobRec->ra;
                                                de = mkTemp->usnobRec->de;
                                                //qDebug() << QString("ra= %1\tdec= %2\tmagn= %3\tminM= %4\tmaxM= %5\n").arg(ra).arg(de).arg(magn).arg(minM).arg(maxM);
                                                pmra = mkTemp->usnobRec->pmRA;
                                                pmde = mkTemp->usnobRec->pmDE;

                                                if(sCat->catMagName=="B1mag") magn = mkTemp->usnobRec->B1mag;
                                                else if(sCat->catMagName=="B2mag") magn = mkTemp->usnobRec->B2mag;
                                                else if(sCat->catMagName=="R1mag") magn = mkTemp->usnobRec->R1mag;
                                                else if(sCat->catMagName=="R2mag") magn = mkTemp->usnobRec->R2mag;
                                                else if(sCat->catMagName=="Imag") magn = mkTemp->usnobRec->Imag;

                                                //qDebug() << QString("magn: %1\n").arg(magn);

                                                if((magn<minM)||(magn>maxM)) break;
                                                //qDebug() << QString("deb2\n");
                                                //mkTemp = new marksP;
                                                //qDebug() << QString("getYearFromMJD(MJD)-2000: %1\n").arg(getYearFromMJD(MJD)-2000);
                                                mkTemp->mEkv[0] = ra + mas_to_grad(pmra*(getYearFromMJD(MJD)-2000)/cos(grad_to_rad(de)));//taking proper motion into account
                                                mkTemp->mEkv[1] = de + mas_to_grad(pmde*(getYearFromMJD(MJD)-2000));//taking proper motion into account

                                                mkTemp->mEkv[2] = magn;
                                                mkTemp->catMagName = sCat->catMagName;
                                                mkTemp->catName = sCat->catName;

                                                mGr->addMark(mkTemp);

                                                break;
                                            }
                                        case UCAC3_CAT_NUM://ucac3
                                            {

                                                if(catLine.size()<26)
                                                                                {
                                                //s					QMessageBox::information(NULL, tr("Marks num"), "ucac2find error", QMessageBox::Ok);
                                                                                    qDebug() << QString("cllen= %1\n").arg(catLine.size());
                                                                                        continue;
                                                                                }

                                                mkTemp = new marksP(OBJ_TYPE_UCAC3);
                                                //mkTemp->u3Rec = new ucac3Rec;

                                                //qDebug() << QString("deb1\n");
                                                mkTemp->u3Rec->s2rec(catLine);

                                                ra = mkTemp->u3Rec->ra;
                                                de = mkTemp->u3Rec->dec;
                                                //qDebug() << QString("ra= %1\tdec= %2\tmagn= %3\tminM= %4\tmaxM= %5\n").arg(ra).arg(de).arg(magn).arg(minM).arg(maxM);
                                                pmra = mkTemp->u3Rec->pmrac;
                                                pmde = mkTemp->u3Rec->pmdc;
                                                magn = mkTemp->u3Rec->im1;

                                                if((magn<minM)||(magn>maxM)) break;
                                                //qDebug() << QString("deb2\n");
                                                //mkTemp = new marksP;
                                                //qDebug() << QString("getYearFromMJD(MJD)-2000: %1\n").arg(getYearFromMJD(MJD)-2000);
                                                mkTemp->mEkv[0] = ra + mas_to_grad(pmra*(getYearFromMJD(MJD)-2000)/cos(grad_to_rad(de)));//taking proper motion into account
                                                mkTemp->mEkv[1] = de + mas_to_grad(pmde*(getYearFromMJD(MJD)-2000));//taking proper motion into account
                                                //qDebug() << QString("deb4\n");
                                               //qDebug() << "\nmkTemp->mEkv[0]\t" << mkTemp->mEkv[0] << "\tmkTemp->mEkv[1]\t" << mkTemp->mEkv[1] << "\n";

                                                //ekk = new double[2];
                                                //tank = new double[2];
                                                //itank = new int[2];

                                                //ekk[0] = ra; ekk[1] = de;

                                                //mT = mGr->addEkvMark(ra, de, magn);
                                                //qDebug() << "resUcac2: " << mkTemp->resUcac2 << "\n";
                                                //mkTemp->resUcac2->ra = ra;
                                                //mkTemp->resUcac2->de = de;
                                                mkTemp->mEkv[2] = magn;
                                                //mkTemp->resRec = new residualsRec;
                                                mkTemp->catMagName = QString("im1");
                                                mkTemp->catName = QString("ucac3");

                                                /*mkTemp->resUcac2->J=u3rec->jmag;//Jmag from 2MASS
                                                mkTemp->resUcac2->H=u3rec->hmag;//Hmag from 2MASS
                                                mkTemp->resUcac2->Ks=u3rec->kmag;//Ks_mag from 2MASS/

                                                //qDebug() << QString("deb5\n");
                                                mGr->addMark(mkTemp);
                                                //qDebug() << QString("deb6\n");

                                                break;
                                            }
				}
			}
//		fitsd->detTan();
                if(sNum!=-1) mGr->sortMagn(0);

                qDebug() << QString("marks.size= %1\n").arg(mGr->marks.size());
                if((sNum>mGr->marks.size())||sNum==-1)sNum = mGr->marks.size();
                /*for(int i=0; i<sNum; i++)
                {
                    catMarks->addMark(mGr->marks.at(i));
                }/
                catMarks->copy(mGr);
                qDebug() << QString("catMarks.size= %1\n").arg(catMarks->marks.size());
                detTan();
                //catMarks->detTan(&WCSdata[0], &imgArr->naxes[0]);

}
*/

int detCtype(QString catName)
{
    if(QString().compare(catName, "usno-b1.0", Qt::CaseInsensitive)==0) return USNOB_CAT_NUM;
    else if(QString().compare(catName, "ucac3", Qt::CaseInsensitive)==0) return UCAC3_CAT_NUM;
    else if(QString().compare(catName, "ppmxl", Qt::CaseInsensitive)==0) return PPMXL_CAT_NUM;
    return -1;
}

int getMarksGrid(marksGrid *catMarks, catFinder *sCat, int catProgType, double mjd, double raOc, double deOc, double fov, double minM, double maxM, int sNum)
{
    /*
      fov - field of view max dim in arcmin


    */
        double ra, de, pmra, pmde, magn;



        QString catLine, headLine;
        marksP *mkTemp;
        int hlCount = 0;


        qDebug() << QString("catName: %1\n").arg(sCat->catName);
        int catType = detCtype(sCat->catName);
        if(catType==-1)
        {
            qDebug() << QString("Error: Unsupported cat name\n");
            return 1;
        }
        qDebug() << "\nra: " << mas_to_hms(grad_to_mas(raOc), ":", 3) << "\nde: " << mas_to_damas(grad_to_mas(deOc),":",2) << "\n";

                marksGrid *mGr;
                mGr = new marksGrid;

                qDebug() << "\nMJD\t" << mjd << "\n";

                    mGr->ctype = catType;
                    QProcess outerProcess;
                    QStringList outerArguments;
                    outerArguments.clear();

                    switch(catProgType)
                    {
                    case CDSFIND_PROG_TYPE://usno-b1
                            outerProcess.setWorkingDirectory(sCat->exePath);
                            outerProcess.setProcessChannelMode(QProcess::MergedChannels);
                            outerProcess.setReadChannel(QProcess::StandardOutput);
                            outerArguments << sCat->catName << mas_to_hms(grad_to_mas(raOc), ":", 3) << mas_to_damas(grad_to_mas(deOc),":",2) << QString("b=%1").arg(fov);//"USNO-B1.0"
                            qDebug() << sCat->exeName << outerArguments.join(" ");
                            outerProcess.start(sCat->exeName, outerArguments);
                            break;
                    case UCAC3FIND_PROG_TYPE://ucac3
                            outerProcess.setWorkingDirectory(sCat->exePath);
                            outerProcess.setProcessChannelMode(QProcess::MergedChannels);
                            outerProcess.setReadChannel(QProcess::StandardOutput);
                            outerArguments << mas_to_hms(grad_to_mas(raOc), ":", 3) << mas_to_damas(grad_to_mas(deOc),":",2) << QString("r=%1").arg(fov/2.0) << QString("%1").arg(sCat->catPath);
                            qDebug() << sCat->exeName << outerArguments.join(" ");
                            outerProcess.start(sCat->exeName, outerArguments);
                            break;
                    }

                    outerProcess.waitForFinished(-1);

                    QTextStream catStream(outerProcess.readAllStandardOutput());

                    while (!catStream.atEnd())
                    {
                            catLine = catStream.readLine();



                            switch(mGr->ctype)
                            {
                                    case USNOB_CAT_NUM://usno-b1
                                        {
                                            if(catLine.indexOf('#')==0) continue;
                                            else if(catLine.size()==0) continue;
                                            else if(hlCount<4)
                                            {
                                                hlCount++;
                                                continue;
                                            }

                                            mkTemp = new marksP(OBJ_TYPE_USNOB);
                                            //mkTemp->u3Rec = new ucac3Rec;



                                            mkTemp->usnobRec->s2rec(catLine);

                                            ra = mkTemp->usnobRec->ra;
                                            de = mkTemp->usnobRec->de;
                                            //qDebug() << QString("ra= %1\tdec= %2\tmagn= %3\tminM= %4\tmaxM= %5\n").arg(ra).arg(de).arg(magn).arg(minM).arg(maxM);
                                            pmra = mkTemp->usnobRec->pmRA;
                                            pmde = mkTemp->usnobRec->pmDE;

                                            mkTemp->catMagName = sCat->catMagName;
                                            if(mkTemp->catMagName=="B1mag") magn = mkTemp->usnobRec->B1mag;
                                            else if(mkTemp->catMagName=="B2mag") magn = mkTemp->usnobRec->B2mag;
                                            else if(mkTemp->catMagName=="R1mag") magn = mkTemp->usnobRec->R1mag;
                                            else if(mkTemp->catMagName=="R2mag") magn = mkTemp->usnobRec->R2mag;
                                            else if(mkTemp->catMagName=="Imag") magn = mkTemp->usnobRec->Imag;
                                            else
                                            {
                                                qDebug() << QString("Wrong catMagName, it changed to Imag\n");
                                                magn = mkTemp->usnobRec->Imag;
                                                mkTemp->catMagName = QString("Imag");
                                            }

                                            //qDebug() << QString("magn: %1\n").arg(magn);

                                            if((magn<minM)||(magn>maxM)) break;
                                            //qDebug() << QString("deb2\n");
                                            //mkTemp = new marksP;
                                            //qDebug() << QString("getYearFromMJD(MJD)-2000: %1\n").arg(getYearFromMJD(MJD)-2000);
                                            mkTemp->mEkv[0] = ra + mas_to_grad(pmra*(getYearFromMJD(mjd)-2000)/cos(grad_to_rad(de)));//taking proper motion into account
                                            mkTemp->mEkv[1] = de + mas_to_grad(pmde*(getYearFromMJD(mjd)-2000));//taking proper motion into account

                                            mkTemp->mEkv[2] = magn;
                                            mkTemp->catMagName = sCat->catMagName;
                                            mkTemp->catName = sCat->catName;

                                            mGr->addMark(mkTemp);

                                            break;
                                        }
                                    case UCAC3_CAT_NUM://ucac3
                                        {
                                            if(catLine.indexOf('#')==0) continue;
                                            else if(catLine.size()==0) continue;

                                            switch(catProgType)
                                            {
                                            case 1:

                                            if(catLine.size()<26)
                                            {
                                                qDebug() << QString("cllen= %1\n").arg(catLine.size());
                                                continue;
                                            }


                                            mkTemp = new marksP(OBJ_TYPE_UCAC3);
                                            mkTemp->u3Rec->s2rec(catLine);

                                                break;
                                            case 0:

                                                if(hlCount<3)
                                                {
                                                    if(hlCount<1)headLine = catLine;
                                                    hlCount++;
                                                    continue;
                                                }


                                                mkTemp = new marksP(OBJ_TYPE_UCAC3);
                                                mkTemp->u3Rec->s2recCDS(headLine, catLine);

                                                break;
                                            }

                                            ra = mkTemp->u3Rec->ra;
                                            de = mkTemp->u3Rec->dec;
                                            pmra = mkTemp->u3Rec->pmrac;
                                            pmde = mkTemp->u3Rec->pmdc;
                                            mkTemp->catMagName = sCat->catMagName;
                                            if(mkTemp->catMagName.toLower()=="im1") magn = mkTemp->u3Rec->im1;
                                            else if(mkTemp->catMagName.toLower()=="im2") magn = mkTemp->u3Rec->im2;
                                            else if(mkTemp->catMagName.toLower()=="jmag") magn = mkTemp->u3Rec->jmag;
                                            else if(mkTemp->catMagName.toLower()=="kmag") magn = mkTemp->u3Rec->kmag;
                                            else if(mkTemp->catMagName.toLower()=="hmag") magn = mkTemp->u3Rec->hmag;
                                            else if(mkTemp->catMagName.toLower()=="smb") magn = mkTemp->u3Rec->smB;
                                            else if(mkTemp->catMagName.toLower()=="smr2") magn = mkTemp->u3Rec->smR2;
                                            else if(mkTemp->catMagName.toLower()=="smi") magn = mkTemp->u3Rec->smI;
                                            else
                                            {
                                                qDebug() << QString("Wrong catMagName, it changed to im1\n");
                                                magn = mkTemp->u3Rec->im1;
                                                mkTemp->catMagName = QString("im1");
                                            }

                                            //mkTemp->u3Rec->

                                            if((magn<minM)||(magn>maxM)) break;

                                            mkTemp->mEkv[0] = ra + mas_to_grad(pmra*(getYearFromMJD(mjd)-2000)/cos(grad_to_rad(de)));//taking proper motion into account
                                            mkTemp->mEkv[1] = de + mas_to_grad(pmde*(getYearFromMJD(mjd)-2000));//taking proper motion into account

                                            mkTemp->mEkv[2] = magn;

                                            //mkTemp->catMagName = QString("im1");
                                            mkTemp->catName = sCat->catName;

                                            mGr->addMark(mkTemp);

                                            break;
                                        }
                            case PPMXL_CAT_NUM://ppmxl
                                {
                                    if(catLine.indexOf('#')==0) continue;
                                    else if(catLine.size()==0) continue;


                                    if(hlCount<3)
                                    {
                                        if(hlCount<1)headLine = catLine;
                                        hlCount++;
                                        continue;
                                    }

                                    mkTemp = new marksP(OBJ_TYPE_PPMXL);
                                    //mkTemp->u3Rec = new ucac3Rec;



                                    mkTemp->ppmxlRec->s2recCDS(headLine, catLine);

                                    ra = mkTemp->ppmxlRec->ra;
                                    de = mkTemp->ppmxlRec->de;
                                    //qDebug() << QString("ra= %1\tdec= %2\tmagn= %3\tminM= %4\tmaxM= %5\n").arg(ra).arg(de).arg(magn).arg(minM).arg(maxM);
                                    pmra = mkTemp->ppmxlRec->pmRA;
                                    pmde = mkTemp->ppmxlRec->pmDE;

                                    mkTemp->catMagName = sCat->catMagName;
                                    if(mkTemp->catMagName=="Jmag") magn = mkTemp->ppmxlRec->Jmag;
                                    else if(mkTemp->catMagName=="Hmag") magn = mkTemp->ppmxlRec->Hmag;
                                    else if(mkTemp->catMagName=="Kmag") magn = mkTemp->ppmxlRec->Kmag;
                                    else if(mkTemp->catMagName=="b1mag") magn = mkTemp->ppmxlRec->b1mag;
                                    else if(mkTemp->catMagName=="b2mag") magn = mkTemp->ppmxlRec->b2mag;
                                    else if(mkTemp->catMagName=="imag") magn = mkTemp->ppmxlRec->imag;
                                    else
                                    {
                                        qDebug() << QString("Wrong catMagName, it changed to imag\n");
                                        magn = mkTemp->ppmxlRec->imag;
                                        mkTemp->catMagName = QString("imag");
                                    }

                                    //qDebug() << QString("magn: %1\n").arg(magn);

                                    if((magn<minM)||(magn>maxM)) break;
                                    //qDebug() << QString("deb2\n");
                                    //mkTemp = new marksP;
                                    //qDebug() << QString("getYearFromMJD(MJD)-2000: %1\n").arg(getYearFromMJD(MJD)-2000);
                                    mkTemp->mEkv[0] = ra + mas_to_grad(pmra*(getYearFromMJD(mjd)-2000)/cos(grad_to_rad(de)));//taking proper motion into account
                                    mkTemp->mEkv[1] = de + mas_to_grad(pmde*(getYearFromMJD(mjd)-2000));//taking proper motion into account

                                    mkTemp->mEkv[2] = magn;
                                    mkTemp->catMagName = sCat->catMagName;
                                    mkTemp->catName = sCat->catName;

                                    mGr->addMark(mkTemp);

                                    break;
                                }
                            }
                    }

            if(sNum!=-1) mGr->sortMagn(0);

            qDebug() << QString("marks.size= %1\n").arg(mGr->marks.size());
            if((sNum>mGr->marks.size())||sNum==-1)sNum = mGr->marks.size();

            catMarks->copy(mGr);
            qDebug() << QString("catMarks.size= %1\n").arg(catMarks->marks.size());

            return 0;
            //delete mGr;
}

void fitsdata::detIpixWorkFrame()
{
    if(ipixMarks!=NULL) ipixMarks->detRect(&workFrame);
}

void fitsdata::detCatWorkFrame()
{
    if(catMarks!=NULL) catMarks->detRect(&workFrame);
}

void fitsdata::setRpix()
{
    WCSdata[0] = workFrame.center().x();
    WCSdata[1] = workFrame.center().y();
}

int fitsdata::loadIpixMarks(QString fName, QString mSep, QString mCol)
{
    fileName = fName;
    return(ipixMarks->loadTanImg(fName, mSep, mCol));
}

int fitsdata::saveIpixMarks(QString fName, QString mSep, QString mCol)
{
    return(ipixMarks->saveTanImg(fName, mSep, mCol));
}

int fitsdata::saveWCSFile(QString headFName)
{
    QFile oFile(headFName);
    oFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    QTextStream ostr(&oFile);

 //   QStringList wStr;
    for(int i=0; i<11; i++) ostr << QString("%1|").arg(WCSdata[i], 15, 'f', 10);
    ostr << QString("%1").arg(WCSdata[11], 15, 'f', 10);
    oFile.close();

//   QString mstr(headFName.left(headFName.lastIndexOf("/")+1)+QString("&")+headFName.right(headFName.size()-headFName.lastIndexOf("/")-1));
/*    QString mstr(headFName);
   QByteArray curr_fn = mstr.toAscii();
   char *ffname = curr_fn.data();
   fitsfile *fptR;
   int statuS = 0;//status

   fits_create_file(&fptR, ffname, &statuS);
   if(statuS)
   {


       mstr = QString(QString("!")+headFName);

       curr_fn = mstr.toAscii();
       ffname = curr_fn.data();
       statuS = 0;
       fits_create_file(&fptR, ffname, &statuS);

       if(statuS)
       {
           qDebug() << QString("\nFile %1 dont created\n").arg(ffname);
           return 1;
       }
   }

   long nxs[2];


   fits_create_hdu(fptR, &statuS);

   QByteArray baStr1;
   char *chStr1;

 /*   mstr = "F";
   baStr1 = mstr.toAscii();
   chStr1 = baStr1.data();
   fits_write_key(fptR, TLOGICAL, "SIMPLE", chStr1, " ", &statuS);
   statuS = 0;
   mstr = "16";
   int iv = 16;
   baStr1 = mstr.toAscii();
   chStr1 = baStr1.data();
   fits_write_key(fptR, TSHORT, "BITPIX", &iv, " ", &statuS);
   statuS = 0;/
   prToFit = WCSdata[1];
   fits_write_key(fptR, TDOUBLE, "CRPIX2", &prToFit, "ref pix 2", &statuS);
   statuS = 0;/
   mstr = "PLATE";
   baStr1 = mstr.toAscii();
   chStr1 = baStr1.data();
   fits_write_key(fptR, TSTRING, "XTENSION", chStr1, " ", &statuS);
   statuS = 0;
   mstr = "RA---TAN";
   baStr1 = mstr.toAscii();
   chStr1 = baStr1.data();
   fits_write_key(fptR, TSTRING, "CTYPE1", chStr1, " ", &statuS);
   statuS = 0;
   mstr = "DEC--TAN";
   QByteArray baStr2 = mstr.toAscii();
   char *chStr2 = baStr2.data();
   fits_write_key(fptR, TSTRING, "CTYPE2", chStr2, " ", &statuS);
   statuS = 0;
   double prToFit = WCSdata[0];
   fits_write_key(fptR, TDOUBLE, "CRPIX1", &prToFit, "ref pix 1", &statuS);
   statuS = 0;
   prToFit = WCSdata[1];
   fits_write_key(fptR, TDOUBLE, "CRPIX2", &prToFit, "ref pix 2", &statuS);
   statuS = 0;
   prToFit = WCSdata[2];
   fits_write_key(fptR, TDOUBLE, "CRVAL1", &prToFit, "RA at ref pix", &statuS);
   statuS = 0;
   prToFit = WCSdata[3];
   fits_write_key(fptR, TDOUBLE, "CRVAL2", &prToFit, "DEC at ref pix", &statuS);
   statuS = 0;
   prToFit = WCSdata[4];
   fits_write_key(fptR, TDOUBLE, "CDELT1", &prToFit, "scale1 [deg/pix]", &statuS);
   statuS = 0;
   prToFit = WCSdata[5];
   fits_write_key(fptR, TDOUBLE, "CDELT2", &prToFit, "scale2 [deg/pix]", &statuS);
   statuS = 0;
   prToFit = WCSdata[6];
   fits_write_key(fptR, TDOUBLE, "CROTA1", &prToFit, "rotation1 [deg]", &statuS);
   statuS = 0;
   prToFit = WCSdata[7];
   fits_write_key(fptR, TDOUBLE, "CROTA2", &prToFit, "rotation2 [deg]", &statuS);
   statuS = 0;
   prToFit = WCSdata[8];
   fits_write_key(fptR, TDOUBLE, "CD1_1", &prToFit, "WCS CD matrix", &statuS);
   statuS = 0;
   prToFit = WCSdata[9];
   fits_write_key(fptR, TDOUBLE, "CD1_2", &prToFit, "WCS CD matrix", &statuS);
   statuS = 0;
   prToFit = WCSdata[10];
   fits_write_key(fptR, TDOUBLE, "CD2_1", &prToFit, "WCS CD matrix", &statuS);
   statuS = 0;
   prToFit = WCSdata[11];
   fits_write_key(fptR, TDOUBLE, "CD2_2", &prToFit, "WCS CD matrix", &statuS);
   statuS = 0;
   getCatName(&mstr, catMarks->ctype);
   QByteArray baStr3 = mstr.toAscii();
   char *chStr3 = baStr3.data();
   fits_write_key(fptR, TSTRING, "WCSRFCAT", chStr3, "wcs catalog", &statuS);
   statuS = 0;
   mstr = "deg";
   QByteArray baStr4 = mstr.toAscii();
   char *chStr4 = baStr4.data();
   fits_write_key(fptR, TSTRING, "CUNIT1", chStr4, "unit 1", &statuS);
   statuS = 0;
   QByteArray baStr5 = mstr.toAscii();
   char *chStr5 = baStr5.data();
   fits_write_key(fptR, TSTRING, "CUNIT2", chStr5, "unit 2", &statuS);
   statuS = 0;
   fits_write_key(fptR, TDOUBLE, "WCSUWE1", &Sx, "unit weight error in mas", &statuS);
   statuS = 0;
   fits_write_key(fptR, TDOUBLE, "WCSUWE2", &Sy, "unit weight error in mas", &statuS);
   statuS = 0;
   fits_close_file(fptR, &statuS);
*/




    return 0;
}

int fitsdata::loadHeaderFile1(QString headFName)
{
    qDebug() << "\nloadHeaderFile\n";


    if (!headFName.isEmpty())
    {
        int i, sz;
        QString kName, kVal, tstr;
        QStringList headVals, keyVals;
        HeadRecord *headRec;
        int pos0, pos1;

        char *str = new char[1024];
        char *str_name = new char[1024];
        char *str_value = new char[1024];

        QByteArray qbarr;
        QByteArray fheadfn = fileName.toAscii();
        FILE *FI = fopen(fheadfn.data(), "r");
        if (FI==NULL)
        {
            /*QMessageBox msgBox(QMessageBox::Warning, tr("Can't open Header file"), "Mentioned FITS Header file has't exist", 0, this);
            msgBox.addButton(QMessageBox::Ok);
            msgBox.exec();*/
            qDebug() << QString("Can't open Header file\nMentioned FITS Header file has't exist\n");
            return 1;
        }

        qDebug() << "\nREADING HEADER...\n";

        QString *qstr = new QString;

        QTextCodec *codec1 = QTextCodec::codecForName("windows-1251");
        QTextCodec *codec2 = QTextCodec::codecForName("IBM 866");
        /*	QTextDecoder *decoder = codec1->makeDecoder();
                        QTextEncoder *encoder = codec2->makeEncoder();
        */
        QString string;
        QChar *cstr = new QChar;
        int endKey = 0;
        QString dateObs, timeObs, timeEnd;
        QStringList obsList;
        headList.clear();

        while (fgets(str, 1024, FI))
        {
            qDebug() << "string is: _" << str << "_" << endl;
            qbarr.clear();
            qbarr.insert(0, str);

            string = codec1->toUnicode(qbarr);

            headVals << string;
        }
        fclose(FI);

        readHeader(headVals);

    }
    else return 2;

    return 0;
}

int fitsdata::loadHeaderFile(QString headFName)
{
    qDebug() << "\nloadHeaderFile\n";


    if (!headFName.isEmpty())
    {
        QTextCodec *codec1 = QTextCodec::codecForName("windows-1251");
        QTextCodec *codec2 = QTextCodec::codecForName("IBM 866");
        QTextCodec *codec3 = QTextCodec::codecForName("KOI8-R");
        QStringList headVals;

        QFile hFile(headFName);

        if(!hFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << QString("Can't open Header file\nMentioned FITS Header file has't exist\n");
            return 1;
        }

        QTextStream hStream(&hFile);

        hStream.setCodec(codec1);

        qDebug() << "\nREADING HEADER...\n";

        QString string;

        headList.clear();

        while(!hStream.atEnd())
        {
            string = hStream.readLine();
            headVals << string;
        }

        hFile.close();

        readHeader(headVals);

    }
    else return 2;

    return 0;
}

int fitsdata::saveHeaderFile(QString headFName)
{
    return 0;
}

int fitsdata::saveFitsHeader()
{
    qDebug() << "saveFitsHeader\n";
    QString mstr(fileName);
    QByteArray curr_fn = mstr.toAscii();
    char *ffname = curr_fn.data();
    fitsfile *fptr_out;
    fitsfile *fptr;
    int statuS = 0;//status
    int status;
    int i;
    HeadRecord hRec;

    DATEOBS date_obs;
    //QByteArray baStr1, baStr2;
    //char *chStr1, *chStr2;
/*
    fits_open_file(&fptr, fname, READONLY,&status);fitsErrors[0]=status;//open file
    qDebug() << "\nfits_open_file: " << status << "\n";
    if(status) return 1;
    status = 0;
*/
    fits_open_file(&fptr_out, ffname , READWRITE,&statuS);
    qDebug() << QString("open_old %1").arg(statuS) << endl;

    //its_delete_hdu(&fptr_out, &hdut, &status);

    //fits_copy_header(&fptr, fptr_out, &status);
    char strkey[FLEN_CARD];
    //fits_update_key(fptr_out, TSTRING, hRec.keyName.toAscii().constData(), (char*)hRec.keyValue.toAscii().constData(), " ", &statuS);
    //timeFromStrFTN(&date_obs, ing strT);
    QString strT;
    getStrFTNfromMJD(&strT, MJD, exptime);
    strncpy(&strkey[0], strT.toAscii().constData(), 81);
    fits_update_key(fptr_out, TSTRING, "DATE-OBS", strkey, NULL, &status);status = 0;//read "DATE-OBS"
    fits_update_key(fptr_out, TDOUBLE, "EXPTIME", &exptime, NULL, &status);status = 0;//read "EXPTIME"
    deg_to_hms(&strT, WCSdata[2], ":", 3);
    strncpy(&strkey[0], strT.toAscii().constData(), 81);
    fits_update_key(fptr_out, TSTRING, "RA", strkey, NULL, &status);status = 0;//read "DATE-OBS"
    deg_to_damas(&strT, WCSdata[3], ":", 2);
    strncpy(&strkey[0], strT.toAscii().constData(), 81);
    fits_update_key(fptr_out, TSTRING, "DEC", strkey, NULL, &status);status = 0;//read "DATE-OBS"
    /*

    fits_get_hdrspace(fptr, &nkeys, NULL, &status);fitsErrors[3]=status;status = 0;//determination of number of line of the header
    qDebug() << QString("header nkeys: %1\n").arg(nkeys);
    char card[FLEN_CARD];//buffer to read keys from header
    fitsHeader="";//strings from header
    for (int k = 1; k <= nkeys; k++)//reading strings from header
    {
       fits_read_record(fptr, k, card, &status);// read separate record
       qDebug() << QString("read header key: %1\n").arg(QString(card));
       //fitsHeader = fitsHeader + QString(card)+"\n";// add record to fitsHeader
       fits_update_card()

    }



    if(statuS)
    {

        statuS = 0;
        fits_delete_file(fptr_out, &statuS);
        qDebug() << QString("delete %1").arg(statuS) << endl;
        statuS = 0;
        fits_create_file(&fptr_out, ffname, &statuS);
        qDebug() << QString("create %1").arg(statuS) << endl;
        statuS = 0;
        fits_create_hdu(fptr_out, &statuS);
        qDebug() << QString("create_hdu %1").arg(statuS) << endl;
        statuS = 0;
    }

    int szH = headList.headList.size();
    qDebug() << QString("headList size: %1\n").arg(szH);
    for(i=0; i<szH; i++)
    {
        headList.getKeyNum(&hRec, i);
        fits_update_key(fptr_out, TSTRING, hRec.keyName.toAscii().constData(), (char*)hRec.keyValue.toAscii().constData(), " ", &statuS);
        qDebug() << QString("_%1_%2_%3\n").arg(hRec.keyName.toAscii().constData()).arg(hRec.keyValue.toAscii().constData()).arg(statuS);
        statuS = 0;
    }
/*
    mstr = "RA---TAN";
    QByteArray baStr1 = mstr.toAscii();
    char *chStr1 = baStr1.data();
    fits_update_key(fptR, TSTRING, "CTYPE1", chStr1, " ", &statuS);
    statuS = 0;
    mstr = "DEC--TAN";
    QByteArray baStr2 = mstr.toAscii();
    char *chStr2 = baStr2.data();
    fits_update_key(fptR, TSTRING, "CTYPE2", chStr2, " ", &statuS);
    statuS = 0;
    double prToFit = WCSdata[0];
    fits_update_key(fptR, TDOUBLE, "CRPIX1", &prToFit, "ref pix 1", &statuS);
    statuS = 0;
    /
    fits_close_file(fptr_out, &statuS);
    qDebug() << QString("fits_close_file: %1\n").arg(statuS);
*/
    return 0;
}

int fitsdata::saveFits()
{


    long nelements;
    fitsfile *fptr_out;
    int status = 0;

    char *fname = new char[256];
    strcpy(fname, fileName.toAscii().data());

    //QFile().remove(fileName);

    fits_open_file(&fptr_out, fname, READWRITE, &status);
    qDebug() << QString("open_old %1\n").arg(status);
            //status = 0;

    if(status)
    {
        //fits_open_file(&fptr_out, fname, READWRITE, &status);
        //qDebug() << QString("open_old %1\n").arg(status);
        //status = 0;
        //fits_delete_file(fptr_out, &status);
        //qDebug() << QString("delete %1\n").arg(status);
        //status = 0;
        fits_create_file(&fptr_out, fname, &status);
        qDebug() << QString("create %1\n").arg(status);
        status = 0;
        fits_create_hdu(fptr_out, &status);
        qDebug() << QString("create_hdu %1\n").arg(status);
        status = 0;
    }


    long naxes[2];
    naxes[0] = imgArr->naxes[0];
    naxes[1] = imgArr->naxes[1];




    //naxes[1] = Height;
    //naxes[0] = Width;
    nelements = imgArr->getNelements();

    //fits_write_img(fptr_out, USHORT_IMG, 1, nelements, imgArr->ushD, &status);
    fits_create_img(fptr_out, USHORT_IMG, 2, naxes, &status);
    qDebug() << QString("create_img %1\n").arg(status) << endl;
    status = 0;


    fits_write_img(fptr_out, TUSHORT, 1, naxes[0]*naxes[1]+1, (void*) imgArr->ushD, &status);
    qDebug() << QString("write_img %1\n").arg(status);
    status=0;
    fits_close_file(fptr_out, &status);

    saveFitsHeader();
    if(WCSdata[12]) saveWCS();
}

int fitsdata::saveFitsAs(QString fitsFileName)
{
    qDebug() << "\nfitsdata::saveFitsAs\n";
    //fileName = fitsFileName;
    //saveFits();

    long nelements;
    fitsfile *fptr;
    fitsfile *fptr_out;
    int status = 0;

    char *fname = new char[256];
    strcpy(fname, fileName.toAscii().data());

    char *fname1 = new char[256];
    strcpy(fname1, fitsFileName.toAscii().data());

    QFile().remove(fitsFileName);

    fits_open_file(&fptr, fname, READWRITE, &status);
    qDebug() << QString("open_old %1\n").arg(status);
            status = 0;
/*
    if(status)
    {*/
        //fits_open_file(&fptr_out, fname, READWRITE, &status);
        //qDebug() << QString("open_old %1\n").arg(status);
        //status = 0;
        //fits_delete_file(fptr_out, &status);
        //qDebug() << QString("delete %1\n").arg(status);
        //status = 0;
        fits_create_file(&fptr_out, fname1, &status);
        qDebug() << QString("create %1\n").arg(status);
        status = 0;
        /*fits_create_hdu(fptr_out, &status);
        qDebug() << QString("create_hdu %1\n").arg(status);
        status = 0;*/
    //}

        fits_copy_hdu(fptr, fptr_out, 0, &status);
        status = 0;


    long naxes[2];
    naxes[0] = imgArr->naxes[0];
    naxes[1] = imgArr->naxes[1];
    nelements = imgArr->getNelements();//naxes[0]*naxes[1];

    //fits_write_img(fptr_out, USHORT_IMG, 1, nelements, imgArr->ushD, &status);
    /*fits_create_img(fptr_out, USHORT_IMG, 2, naxes, &status);
    qDebug() << QString("create_img %1\n").arg(status) << endl;
    status = 0;
*/

    fits_write_img(fptr_out, TUSHORT, 1, naxes[0]*naxes[1]+1, (void*) imgArr->ushD, &status);
    qDebug() << QString("write_img %1\n").arg(status);
    status=0;
    fits_close_file(fptr_out, &status);

    fileName = fitsFileName;

    saveFitsHeader();
    if(WCSdata[12]) saveWCS();
}

int fitsdata::readHttpHeader(QString httpStr)
{
    //int i, sz;
    //QString kName, kVal, tstr;
    fitsHeader.clear();
    fitsHeader.append(httpStr);
    QStringList headVals;//, keyVals;
    //HeadRecord *headRec;
    //int pos0, pos1;
    //QTextCodec *codec1 = QTextCodec::codecForName("windows-1251");
    //QTextCodec *codec2 = QTextCodec::codecForName("IBM 866");

    //QString str = codec1->toUnicode(httpData);
    qDebug() << QString("httpData: %1\n").arg(httpStr);

    //headList.clear();

    headVals  = httpStr.split("\n");

    if(readHeader(headVals)==-1) return 1;

    return 0;
}

int fitsdata::getApeSize(double apeSec)
{
    apeSec /= 3600.0;
    qDebug() << QString("apeSec: %1\n").arg(apeSec);
    double meanSc = getMeanScale();

    double res = apeSec/meanSc;
    qDebug() << QString("res: %1\n").arg(res);
    return((int)res);
}

double fitsdata::getScaleX()
{
    if(WCSdata[12])
    {
        return fabs(WCSdata[8]);
    }
    else
    {
        return WCSdata[4];
    }
}

double fitsdata::getScaleY()
{
    if(WCSdata[12])
    {
        return fabs(WCSdata[9]);
    }
    else
    {
        return WCSdata[5];
    }
}

double fitsdata::getMeanScale()
{
    double msc = (getScaleX()+getScaleY())/2.0;
    qDebug() << QString("msc: %1\n").arg(msc, 'e');
    return(msc);
}

int fitsdata::readHeader(QStringList headVals)
{
    int pos0, pos1;
    headList.clear();
    int sz = headVals.size();
    //qDebug() << "headVals.size= " << sz << "\n";
    if(sz<10) return 1;
    HeadRecord *headRec;
    QTextCodec *codec1 = QTextCodec::codecForName("windows-1251");

    for(int i=0; i<sz; i++)
    {
        headRec = new HeadRecord;
        pos0 = headVals.at(i).indexOf(codec1->toUnicode("="));
        //qDebug() << QString("pos0= %1\n").arg(pos0);
        if(pos0==-1) continue;
            //headRec->keyName = QString("COMMENT");
        else headRec->keyName = headVals.at(i).left(pos0);
        pos0 = headVals.at(i).indexOf(codec1->toUnicode("'"));
        pos1 = headVals.at(i).indexOf(codec1->toUnicode("'"), pos0+1);
        if(pos0==-1||pos1==-1)
        {
            //headList << headRec;
            delete headRec;
            continue;
        }
        //qDebug() << QString("pos0= %1\t pos1= %2\n").arg(pos0).arg(pos1);
        headRec->keyValue = QString(headVals.at(i).mid(pos0+1, pos1-pos0-1));
        //qDebug() << QString("keyValue: %1").arg(headRec->keyValue);
        headList.headList << headRec;
    }

    //initExpList();
    //if(expList->exps.size()==0) return -1;
    //setExposure(expList->exps.size());
    initRaDeHeadList();
    return headList.size();
}

int fitsdata::initHeadList()
{
    int pos0, pos1;
    headList.clear();
    QStringList headVals;
    headVals = fitsHeader.split("\n");
    int sz = headVals.size();
    qDebug() << "headVals.size= " << sz << "\n";
    if(sz<10) return 1;
    HeadRecord *headRec;
    QTextCodec *codec1 = QTextCodec::codecForName("windows-1251");
    headRec = new HeadRecord;
    for(int i=0; i<sz; i++)
    {

        //if(headVals.at(i).indexOf(codec1->toUnicode("="))=)
        pos0 = headVals.at(i).indexOf(codec1->toUnicode("="));
        //qDebug() << QString("pos0= %1\n").arg(pos0);
        if(pos0==-1)
        {
            headRec->keyName = "";
            headRec->keyValue = "";
            headRec->keyComment =  QString(headVals.at(i));
            //continue;
        }
        else
        {
            headRec->keyName = headVals.at(i).left(pos0);
            if(headRec->keyName.indexOf("DATE-OBS")!=-1) continue;
            if(headRec->keyName.indexOf("TIME-OBS")!=-1) continue;
            if(headRec->keyName.indexOf("EXPTIME")!=-1) continue;
            if(headRec->keyName.indexOf("DATE-OBS")!=-1) continue;

        //pos0 = headVals.at(i).indexOf(codec1->toUnicode("'"));
            pos1 = headVals.at(i).indexOf(codec1->toUnicode("/"), pos0+1);
            headRec->keyValue = QString(headVals.at(i).mid(pos0+1, pos1-pos0-1));
            headRec->keyComment = QString(headVals.at(i).section("/", -1, -1));
    /*
        if(pos0==-1||pos1==-1)
        {
            //headList << headRec;
            delete headRec;
            continue;
        }*/
        //qDebug() << QString("pos0= %1\t pos1= %2\n").arg(pos0).arg(pos1);

        }

        qDebug() << QString("keyName: %1\n").arg(headRec->keyName);
        qDebug() << QString("keyValue: %1\n").arg(headRec->keyValue);
        qDebug() << QString("keyComment: %1\n").arg(headRec->keyComment);
        headList.headList << headRec;

        headRec = new HeadRecord;

    }
}

void fitsdata::readWCShead()
{

}

int fitsdata::loadWCSFile(QString headFName)
{
    int i;
    QFile iFile(headFName);
    if(!iFile.open(QIODevice::ReadOnly | QIODevice::Text)) return 1;
    QTextStream ostr(&iFile);

    QString dStr = ostr.readLine();
    QStringList dList = dStr.split("|");

    for(i=0; i<12; i++)
    {
        WCSdata[i] = dList.at(i).toDouble();
        qDebug() << QString("WCSdata[%1]= %2\n").arg(i).arg(WCSdata[i]);
    }

    WCSdata[12] = 1;
    return 0;
}

int identAuto(marksGrid *resGrid, marksGrid *ekvGr, marksGrid *ipixGr, double *WCSdata, int targNum, int pType, int maxNum)
{

        qDebug() << "\nOtogdestv\n";
        int nSize, kSize, oSize;
        int i, k;

        double OCp0, OCp1;
        double *ksiW, *etaW, *raW, *deW, *magnW;
        marksP *mN;
        double *x, *y, *ksiI, *etaI, *ksi, *eta, *magn, *ra, *de;
        bool fvm;
        int *sootv;
        int *tangN;
        int isMatched;
        double xscale, yscale;

        qDebug() << QString("ekvGr size: %1\tipixGr size: %2\n").arg(ekvGr->marks.size()).arg(ipixGr->marks.size());

        if(ekvGr->marks.size()<targNum)
        {
            qDebug() << QString("ekvGr marks num is too small\n");
            return 1;
        }

        remCrossedMarks(ipixGr, 5);

        if(ipixGr->marks.size()<targNum)
        {
            qDebug() << QString("ipixGr marks num is too small\n");
            return 1;
        }


        ekvGr->sortMagn();
        ipixGr->sortAper(1);

        OCp0 = WCSdata[2];
        OCp1 = WCSdata[3];
        double ang = WCSdata[6];

        tangN = NULL;


        nSize = ipixGr->marks.size();
        kSize = ekvGr->marks.size();

        int fpos;
        int nfSize = nSize;
        int ipos;
        ipos = 0;
        int kfSize = kSize;

        QVector <int> nInd, kInd;
        QStringList indList;

        xscale = WCSdata[4];
        yscale = WCSdata[5];


        do
        {

            fpos = 0;
            do
            {
                //nSize -= fpos;
                //nfSize = nSize - fpos;
                if(nfSize>20&&pType==0) nfSize = 20;
                if(nfSize>20&&pType==1) nfSize = 20;
                if(kfSize>100&&pType==0) kfSize = 100;
                if(kfSize>100&&pType==1) kfSize = 100;

                nInd.clear();
                kInd.clear();


                qDebug() << QString("nSize= %1\tkSize= %2\tfpos= %3\tnfSize= %4\tipos= %5\tkfSize=%6\n").arg(nSize).arg(kSize).arg(fpos).arg(nfSize).arg(ipos).arg(kfSize);

                x = new double[nfSize];
                y = new double[nfSize];

                ksiI = new double[kfSize];
                etaI = new double[kfSize];
                ksi = new double[kfSize];
                eta = new double[kfSize];
                magn = new double[kfSize];
                ra = new double[kfSize];
                de = new double[kfSize];

                qDebug() << "\nx y\n";
                for(i = 0; i< nfSize; i++)
                {
                        x[i] = ipixGr->marks[i+fpos]->mTanImg[0];
                        y[i] = ipixGr->marks[i+fpos]->mTanImg[1];
                        qDebug() << "\n" << x[i] << "\t" << y[i] << "\t" << ipixGr->marks[i+fpos]->mTanImg[2];
                }
                qDebug() << "\nksi eta\n";
                for(i = 0; i< kfSize; i++)
                {
                        ksi[i] = ekvGr->marks[i+ipos]->mTan[0];
                        eta[i] = ekvGr->marks[i+ipos]->mTan[1];
                        ksiI[i] = ekvGr->marks[i+ipos]->mTanImg[0];
                        etaI[i] = ekvGr->marks[i+ipos]->mTanImg[1];
                        ra[i] = ekvGr->marks[i+ipos]->mEkv[0];
                        de[i] = ekvGr->marks[i+ipos]->mEkv[1];
                        magn[i] = ekvGr->marks[i]->mEkv[2];
                        qDebug() << "\n" << ksi[i] << "\t" << eta[i] << "\t" << magn[i];
                }

                qDebug() << "\nxscale " << xscale << "\tyscale " << yscale;

                fvm=true;
                sootv = new int[nfSize];

                qDebug() << QString("pType: %1\n").arg(pType);
                mN = new marksP;
                switch(pType)
                {
                case 0:
                    {
                        otogdestv(xscale, yscale, kfSize, nfSize, fvm, ksi, eta, x, y, sootv, 1, ang, 5);
                        k = 0;



                        qDebug() << "\nresGrig\n";

                        for(i = 0; i< nfSize; i++)
                        {
                                qDebug() << "\n" << i << ":" << sootv[i] << "\n";
                                if(sootv[i]!=-1)
                                {

                                    //qDebug() << "copy\n";
                                    mN->copy(ekvGr->marks.at(sootv[i]+ipos));
                                    kInd << sootv[i]+ipos;
                                    //qDebug() << "copyImg\n";
                                    mN->copyImg(ipixGr->marks.at(i+fpos));
                                    nInd << i+fpos;
                                    resGrid->addMark(mN);
                                }
                        }



                    }
                    break;
                case 1:
                    {
                        tangN = new int[kfSize];
                        for(i=0; i<kfSize; i++) tangN[i] = -1;
                        for(i=0; i<nfSize; i++) sootv[i] = i;

                        qDebug() << "\ndo match\n";

                        for(int i=0;i<3;i++)
                        {
                            if(i==0){isMatched = matchstars(tangN,ksi,eta,sootv,x,y, kfSize, nfSize, 1, 0.01, 0.005,targNum+1); if(isMatched)break;}
                            if(i==1){isMatched = matchstars(tangN,ksi,eta,sootv,x,y, kfSize, nfSize, 2, 0.02, 0.010,targNum); if(isMatched)break;}
                            if(i==2){isMatched = matchstars(tangN,ksi,eta,sootv,x,y, kfSize, nfSize, 2, 0.03, 0.015,targNum); if(isMatched)break;}
                        }

                        qDebug() << QString("isMatched: %1\tshots: %2\n").arg(isMatched).arg(i);

                       // oSize = 0;
                        if(isMatched)
                        {
                            for(int i=0;i<kfSize;i++)
                            {
                                qDebug() << QString("%1:\ttangN:\t%2\tsootv:\t%3\n").arg(i).arg(tangN[i]).arg(sootv[i]);
                                if(tangN[i]>0)
                                {
                                    qDebug() << "copy\n";
                                    mN->copy(ekvGr->marks.at(tangN[i]+ipos));
                                    qDebug() << "copyImg\n";
                                    mN->copyImg(ipixGr->marks.at(fpos+sootv[i]));
                                    //if(mN->P!=NULL) qDebug() << QString("P9= %1\tP10= %2\n").arg(mN->P[9]).arg(mN->P[10]);
                                    //else qDebug() << "mN->P == NULL\n";
                                    //qDebug() << QString("tan:\t%1\t%2\timg:\t%3\t%4\n").arg(mN->mTan[0]).arg(mN->mTan[1]).arg(mN->mTanImg[0]).arg(mN->mTanImg[1]);
                                    resGrid->addMark(mN);

                                  // oSize++;
                                }
                            }
                        }
                    }
                    break;

                }

                oSize = resGrid->marks.size();
                qDebug() << "\noSize\t" << oSize << "\n";

                //if(oSize<targNum)
                //{
                    delete [] x;
                    delete [] y;

                    delete [] ksiI;
                    delete [] etaI;
                    delete [] ksi;
                    delete [] eta;
                    delete [] magn;
                    delete [] ra;
                    delete [] de;
                    delete [] sootv;
                    //resGrid->clearMarks();
                    if(pType==1&&tangN!=NULL) delete [] tangN;

                //}

                //nSize = ipixGr->marks.size();
                //kSize = ekvGr->marks.size();

                if(pType==0) fpos += 20;
                if(pType==1) fpos += 20;
                //{

                    nfSize = nSize - fpos;
                //}

                    qDebug() << QString("refMarks.size: %1\n").arg(resGrid->marks.size());

            }while(nfSize>20&&oSize<maxNum);//}while(resGrid->marks.size()<targNum&&nfSize>20);

            for(i=0; i<nInd.size(); i++) indList << QString("%1").arg(nInd.at(i));
            qDebug() << QString("nInd: %1").arg(indList.join(" ")) << "\n";
            indList.clear();
            for(i=0; i<kInd.size(); i++) indList << QString("%1").arg(kInd.at(i));
            qDebug() << QString("kInd: %1").arg(indList.join(" ")) << "\n";

            ipos += 100;
            kfSize = kSize - ipos;
            fpos = 0;
            nfSize = nSize - fpos;
        }while(kfSize>20&&oSize<maxNum);//}while(resGrid->marks.size()<targNum&&kfSize>20);

        resGrid->ctype = ekvGr->ctype;

        oSize = resGrid->marks.size();
        for(i = 0; i< oSize; i++)
        {
            mN = resGrid->marks.at(i);
            //qDebug() << "\n" << QString("%1\t%2\t").arg(resGrid->marks[i]->mEkv[0], 12, 'f', 2).arg(resGrid->marks[i]->mEkv[1], 12, 'f', 2) << resGrid->marks[i]->mEkv[2] << "\t" << resGrid->marks[i]->mTan[0] << "\t" << resGrid->marks[i]->mTan[1] << "\t" << resGrid->marks[i]->mTanImg[0] << "\t" << resGrid->marks[i]->mTanImg[1] << "\n";
            qDebug() << QString("img: %1\t%2\ntan: %3\t%4\n").arg(mN->mTanImg[0]).arg(mN->mTanImg[1]).arg(mN->mTan[0]).arg(mN->mTan[1]);
            qDebug() << "mN->P: " << mN->P << "\n";

            if(mN->P!=NULL) qDebug() << QString("P9= %1\tP10= %2\n").arg(mN->P[9]).arg(mN->P[10]);

//            qDebug() << "sdata" << resGrid->marks[i]->sdata << "\n";
        }

        qDebug() << QString("oSize= %1\ttargNum= %2\tfpos= %3\n").arg(oSize).arg(targNum).arg(fpos);
        if(oSize<targNum) return 1;
        return 0;

        delete [] ksiW;
        delete [] etaW;
        delete [] raW;
        delete [] deW;
        delete [] magnW;

        return 0;
}

void fitsdata::measureMarksGrid(marksGrid *mgr, measureParam params)
{
    qDebug() << "\nslotMeasureMarkedButtonClicked\n";
/*    int model = settings->value("psf/model").toInt();//PSF model: 0 - Lorentz PSF, 1 - Gauss PSF, 2 - Moffat PSF
    if(lorentzRadioButton->isChecked()) model = 0;
    if(gaussianRadioButton->isChecked()) model = 1;
    if(moffatRadioButton->isChecked()) model = 2;*/
    int i, t;
    qDebug() << QString("\nmeasureParams:\nmodel: %1\nnofit: %2\ndelta: %3\napRadius: %4\nsubgrad: %5\nringradius: %6\nringwidth: %7\nlb: %8\n").arg(params.model).arg(params.nofit).arg(params.delta).arg(params.apRadius).arg(params.sg).arg(params.ringradius).arg(params.ringwidth).arg(params.lb);

    if(params.model!=3)
    {
 //       int nofit = inSpinBox->value();

        int sz = mgr->marks.size();
        //double *P = new double[21];
      //  t=0;
        for(i=sz-1; i>=0; i--)
        {
            qDebug() << "\ni: " << mgr->marks.size() << "\tX: " << mgr->marks[i]->mTanImg[0] << "\tY: " << mgr->marks[i]->mTanImg[1] << "\n";


            if(!measureMark(imgArr, mgr->marks.at(i), params)) mgr->marks.removeAt(i);
        }
    }
    else
    {
        qDebug() << "centre of Mass";
        detPpix(mgr, params.apRadius*2);
        moveMassCenter(mgr, params.apRadius*2);
  //      slotCenterMass(mgr);
    }


 //       slotScrUpdate();
}

int measureMark(img2d *imgArr, marksP *mP, measureParam params)
{
    qDebug() << QString("\nmeasureMark\n");
    mP->createP();
    //mP->P = new double[21];
    /*qDebug() << "imgArr->ushD " << imgArr->ushD << "\n";
    qDebug() << "imgArr->naxes[0] " << imgArr->naxes[0] << "\n";
    qDebug() << "imgArr->naxes[1] " << imgArr->naxes[1] << "\n";
    qDebug() << "fpix " << fpix << "\n";
    qDebug() << "mP->mTanImg[0] " << mP->mTanImg[0] << "\n";
    qDebug() << "mP->mTanImg[1] " << mP->mTanImg[1] << "\n";*/
    //if(getStimp(mP->P, imgArr->ushD, imgArr->naxes[0], imgArr->naxes[1], fpix, (int)mP->mTanImg[0], (int) mP->mTanImg[1], params))
    if(imgArr->getStimp(mP->P, imgArr->naxes[0], imgArr->naxes[1], imgArr->histD->fpix, (int)mP->mTanImg[0], (int) mP->mTanImg[1], params))
    {
        mP->mTanImg[0] = mP->P[0];
        mP->mTanImg[1] = mP->P[1];
        mP->mTanImg[2] = mP->P[18];
        qDebug() << "\nXn: " << mP->mTanImg[0] << "\tYn: " << mP->mTanImg[1] << "\tMagn: " << mP->mTanImg[2] << "\n";
        return 1;
    }
    qDebug() << QString("\nmeasureMark error\n");
    return 0;
}

/*
int fitsdata::findObj(QString objName, int objType)
{


    return 0;
}
*/
/*old
int fitsdata::ruler3(QString iniFile)
{
qDebug() << QString("\nruler3\n\n");
///////// 1. Reading settings ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //stream << "started....\n";
        //BEGIN settings
        QSettings *settings = new QSettings("./conf/ruler3.ini",QSettings::IniFormat);
        //[%general]
        QString outfolder = settings->value("general/outfolder").toString();//output folder
        qDebug() << QString("outfolder: %1\n").arg(outfolder);
        QString obscode = settings->value("general/obscode").toString();//MPC code of Observatory
        int objType = settings->value("general/objtype").toInt();//type of object: 0 - asteroid, 1 - planetary satellite, 2 - star like object
        int timescale = settings->value("general/timescale").toInt();//timescale: 0 - UTC, 1 - GPS
        double timescalediff = settings->value("general/timescalediff").toDouble();//difference between time scales
        int endmessage = settings->value("general/endmessage").toInt();//make windows standard message about end of work
        int outtospfolder= settings->value("general/outtospfolder").toInt();
        //if (outtospfolder) outfolder = QString("./");
        if (outtospfolder) outfolder = fileName.left(fileName.lastIndexOf("/"));
        //stream <<  outfolder<<"\n";
        //[psf]
        int model = settings->value("psf/model").toInt();//PSF model: 0 - Lorentz PSF, 1 - Gauss PSF, 2 - Moffat PSF
        double delta = settings->value("psf/delta").toDouble();//initial power in Lorentz PSF or Moffat PSF
        int aperture = settings->value("psf/aperture").toInt();//aperture to get pixels values to PSF fitting
        int subgrad = settings->value("psf/subgrad").toInt();
        int ringradius = settings->value("psf/ringradius").toInt();
        int ringwidth = settings->value("psf/ringwidth").toInt();
        int lb = settings->value("psf/lb").toInt();
        //[reductions]
        double maxres = settings->value("reductions/maxres").toDouble();//maximum of residuals for positional reduction in mas
        double maxresMAG = settings->value("reductions/maxresMAG").toDouble();//maximum of residuals for photometric reduction
        double minrefMag = settings->value("reductions/minrefMag").toDouble();//bottom limit for magnitude for reduction
        double maxrefMag = settings->value("reductions/maxrefMag").toDouble();//upper limit for magnitude for reduction
        int syscorr = settings->value("reductions/syscorr").toInt();//1 - taking systematic into account
        double distfromplanet = settings->value("reductions/distfromplanet").toDouble();
        double distfromsmear=settings->value("reductions/distfromsmear").toDouble();
        double planetKsi,planetEta;
        int weights = settings->value("psf/weights").toInt();
        //[outputlimits]
        double maxOCMAG = settings->value("outputlimits/maxOCMAG").toDouble();//maximum of (O-C) in magnitude for output
        double maxOCRA = settings->value("outputlimits/maxOCRA").toDouble();//maximum of (O-C) in RA for output
        double maxOCDE = settings->value("outputlimits/maxOCDE").toDouble();//maximum of (O-C) in DEC for output
        double maxOCMAGrc = settings->value("outputlimits/maxOCMAGrc").toDouble();//maximum of residuals in magnitude for output
        double maxOCRArc = settings->value("outputlimits/maxOCRArc").toDouble();//maximum of residuals in RA for output
        double maxOCDErc = settings->value("outputlimits/maxOCDErc").toDouble();//maximum of residuals in DEC for output
        //[whatoutput] all settings within this section must be equal to 1 if you want to use them
        int mpcreport=settings->value("whatoutput/mpcreport").toInt();//make MPC report
        int residuals=settings->value("whatoutput/residuals").toInt();//make RESIDUALS report
        int ocreject=settings->value("whatoutput/ocreject").toInt();//if ocreject=0 then results with BAD (O-C) (see maxOCMAG, maxOCRA and maxOCDE) will be inluded to output
        int astreport=settings->value("whatoutput/astreport").toInt();//make ASTROREPORT
        int errorreport=settings->value("whatoutput/errorreport").toInt();//make ERRORREPORT
        int relativepos=settings->value("whatoutput/relativepos").toInt();//relative positions of the satellites
        //[outputforms]
        int time_form = settings->value("outputforms/time_form").toInt();//set form for output moments of time: 0 - YYYY MM DD.DDDD, 1 - MJD, 2 - (MJD - MJDo) in minutes
        int pos_form = settings->value("outputforms/pos_form").toInt();//set form for positional data: 0 - (h,m,s; deg, arcmin, arcsec); 1 - mas; 2 - degrees.
        int phot_form = settings->value("outputforms/phot_form").toInt();//set form for photometric data
        QString sepsymb = settings->value("outputforms/sepsymb").toString();//symbol for separation between records
        if (sepsymb=="space") sepsymb=" ";
        double mjd0 = settings->value("outputforms/mjd0").toDouble();//zero point of MJD to output (MJDo). See time_form
        //[catalogue]
        QString cat = settings->value("catalogue/refcatalogue").toString();// reference catalogue name (ucac2 or 2mass)
        int catindex = -1;// more useful integer value to indicate reference catalogue
        if(cat=="ucac2")catindex=0;
        if(cat=="2mass")catindex=1;
        QString fov = settings->value("catalogue/fov").toString();// field of view size in arcmin (radius of selection)
        QString minMag = settings->value("catalogue/minMag").toString();//bottom limit for magnitude
        QString maxMag = settings->value("catalogue/maxMag").toString();//upper limit for magnitude
        //[programs]
        QString ast_eph_prog = settings->value("programs/ast_eph_prog").toString();//soft to find asteroids
        QString ast_eph_prog_folder = settings->value("programs/ast_eph_prog_folder").toString();//working folder of soft to find asteroids
        QString ucac2findn = settings->value("programs/ucac2findn").toString();//soft to find ucac2 data
        QString ucac2find_folder = settings->value("programs/ucac2find_folder").toString();//working folder for soft to find ucac2 data
        QString cdsfindn = settings->value("programs/cdsfindn").toString();//soft to find 2mass data
        QString cdsfind_folder = settings->value("programs/cdsfind_folder").toString();//working folder for soft to find 2mass data
        QString sateph_prog = settings->value("programs/sateph_prog").toString();//soft to find satellites
        QString sateph_prog_folder = settings->value("programs/sateph_prog_folder").toString();//working folder of soft to find satellites
        //marks
        QString mSep = settings->value("marks/marksFileSep").toString();
        QString mCol = settings->value("marks/marksLoadFileColumn").toString();
        int isMove2corner = settings->value("marks/isMove2corner").toInt();
        //smartrefstars
        int isSmartRef = settings->value("smartrefstars/isSmartRef").toInt();
        int sNumMin = settings->value("smartrefstars/sNumMin").toInt();
        double magnDiap = settings->value("smartrefstars/magnDiap").toDouble();
//END settings

    double ra_c,de_c;
    double *sdata, *objdata;
    double mjd;
   // double planetKsi, planetEta;

    int sz, i;
 //   params->catindex = 0;
  //  int objType = 0;

    if(!WCSdata[12]) return 1;

    QString msgstr;
    mjd = MJD;

    ra_c = WCSdata[2]*3600000;//coordinates of the optical center
    de_c = WCSdata[3]*3600000;


///////////3. Reading objects data//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//        QProcess outerProcess;
  //      QStringList outerArguments;
        QString ObjNum;//number of object (for example, MPC number of asteroid)
   /*     QString sMJD = QString( "%1" ).arg( mjd,17,'f',10,QLatin1Char( ' ' ));
        sMJD = sMJD.trimmed();
        //BEGIN read objects positions
        switch(objType)
        {
            case 0://asteroids with mpeph.exe program
            {
                    QFileInfo fileinfo(fname);
                    msgstr = fileinfo.fileName();
                    ObjNum =  msgstr.section('_',0,0);
                    outerProcess.setWorkingDirectory(ast_eph_prog_folder);
                    outerProcess.setProcessChannelMode(QProcess::MergedChannels);
                    outerProcess.setReadChannel(QProcess::StandardOutput);
                    outerArguments<<"-num" <<ObjNum<<sMJD;
                    outerProcess.start(ast_eph_prog, outerArguments);
                    break;
            }
            case 1://saturnian satellites with sateph.exe program
            {
                    outerProcess.setWorkingDirectory(sateph_prog_folder);
                    outerProcess.setProcessChannelMode(QProcess::MergedChannels);
                    outerProcess.setReadChannel(QProcess::StandardOutput);
                    outerArguments << sMJD;
                    outerProcess.start(sateph_prog, outerArguments);
                    break;
            }
        }
        outerProcess.waitForFinished(-1);
        QTextStream objStream(outerProcess.readAllStandardOutput());/
        QString objDataStr,elS;
   //     double *objdata;
        QList<double*> objects;
        //
        double ra,de,pmra,pmde,mag,dist,numb;
        double xy[2];
        double tp[2];
        int nofit;
        //
//        while (!objStream.atEnd())
        sz = objMarks->marks.size();
        ocRec *curObj;
        marksP *mT;

        for(i=0; i<sz; i++)
        {

            mT = objMarks->marks.at(i);

            mT->ocObj->ra_oc = ra_c;
            mT->ocObj->dec_oc = de_c;
            getRaDeToTang1(&tp[0], &tp[1], mT->ocObj->ra, mT->ocObj->de, ra_c, de_c);
            //tp[0] = mT->mTan[0];
            //tp[1] = mT->mTan[1];
            //
    //        if((objType)&&(objDataStr.section('|',7,7).toInt()==0)){planetKsi = tp[0];planetEta = tp[1];}

                objdata = new double[18];
                objdata[0]= mT->ocObj->ra;//RA from theory of motion or catalogue
                objdata[1]=mT->ocObj->de;//DEC from theory of motion  or catalogue
                objdata[2]=tp[0];//tangential position KSI based on RA (C)
                objdata[3]=tp[1];//tangential position ETA based on DE (C)
                objdata[4]=mT->mTanImg[0] - WCSdata[0];//measured pixel position X (O)
                objdata[5]=mT->mTanImg[1] - WCSdata[1];//measured pixel position Y (1)
                objdata[6]=mT->P[18];//log10 of summ of pixels within aperture

                objdata[7]=mT->P[8];//unut weight error of PSF fitting
                objdata[16]=mT->P[9];//unut weight error of PSF fitting
                objdata[17]=mT->P[10];//unut weight error of PSF fitting
                //////////////////////////////////////////////////////
                switch(objType)
                {
                    case 0://asteroids
                        {
                            objdata[8] = mT->ocObj->mag0;//magnitude from theory of motion or catalogue

                            objdata[9]= mT->ocObj->topDist;//topocentric distance from theory of motion or catalogue
                            objdata[10]= mT->ocObj->muRaCosDe;//mu_ra*cos(dec) [mas/min]
                            objdata[11]=mT->ocObj->muDe;//mu_de [mas/min]
                            objdata[12]=mT->ocObj->Vr;//v_r [km/s]
                            objdata[13]=mT->ocObj->phase;//phase [deg]
                            objdata[14]=mT->ocObj->elong;//elongation [deg]
                            objdata[15]=mT->ocObj->name.toInt();//object number

                             objects << objdata;
                            break;
                        }
                    case 1://saturnian satellite
                        {
                          /*  elS = objDataStr.section('|',2,2);objdata[8]=elS.toDouble();//Obj.mag. - satellite magnitude (V)
                            elS = objDataStr.section('|',3,3);objdata[9]=elS.toDouble();//Sun-plan[deg] -  Apparent topocentric distance Sun-Planet
                            elS = objDataStr.section('|',4,4);objdata[10]=elS.toDouble();//Phase[deg] - phase angle
                            elS = objDataStr.section('|',5,5);objdata[11]=elS.toDouble();//Rad.plan. - apparent planetary radius [arcesec]
                            elS = objDataStr.section('|',7,7);objdata[12]=elS.toDouble();//satellite number
                            dist = sqrt(pow(tp[0]-planetKsi,2)+pow(tp[1]-planetEta,2));
                            if((dist>distfromplanet)&&(fabs(tp[1]-planetEta)>distfromsmear))  objects << objdata;
                            /
                            break;
                        }
                }
                //////////////////////////////////////////////////////
            }

        qDebug() << QString("object num: %1\n").arg(objects.size());
          //  delete[] P;
       // }
//END read objects positions
/////////4. Reading data from reference catalogue ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BEGIN load data from catalogue


        QList<double*> refstars;
        //double *sdata;
        sz = refMarks->marks.size();
//        while (!catStream.atEnd())
        for(i=0; i<sz; i++)
        {
                //catLine = catStream.readLine();
                //QMessageBox::information(0,"debug",catLine,QMessageBox::Ok,0,0);
             mT = refMarks->marks.at(i);
                switch(catindex)
                {
                        case 0://ucac2
                        {
                                /*
                                elS = catLine.section('|',0,0);ra = hms_to_mas(elS, " ");
                                elS = catLine.section('|',1,1);de = damas_to_mas(elS, " ");
                                elS = catLine.section('|',11,11);pmra = elS.toDouble();
                                elS = catLine.section('|',12,12);pmde = elS.toDouble();
                                ra = ra +pmra*(getYearFromMJD(mjd)-2000);//taking proper motion into account
                                de = de +pmde*(getYearFromMJD(mjd)-2000);//taking proper motion into account
                                getPixPosFromWCS(ra, de, wcs_data,xy);//return X,Y of pixel in xy[0],xy[1]
                                double *P = new double[21];
                                if(getStimp(P, fits_buff, naxes[0], naxes[1], (double) fpv, (int) xy[0], (int) xy[1],
                                                 aperture, ringradius, ringwidth, subgrad, lb, model, nofit, delta))

                                {/

                                    sdata = new double[14];
                                    sdata[0]=mT->resUcac2->ra;//RA from UCAC2
                                    sdata[1]=mT->resUcac2->de;//DEC from UCAC2
                                    //qDebug() << QString("sdata: ra= %1\tde= %2\n").arg(sdata[0]).arg(sdata[1]);
                                    getRaDeToTang1(&tp[0], &tp[1], mT->resUcac2->ra, mT->resUcac2->de, ra_c, de_c);
                                    sdata[2]=tp[0];//tangential position KSI based on catalogue RA
                                    sdata[3]=tp[1];//tangential position ETA based on catalogue DE
                                    sdata[4]=mT->mTanImg[0] - WCSdata[0];//measured pixel position X (O)
                                    sdata[5]=mT->mTanImg[1] - WCSdata[1];//measured pixel position Y (1)
                                    sdata[6]=mT->P[18];//mean value of pixels within aperture
                                    //qDebug() << QString("sdata: mT->P[18] = %1\n").arg(mT->P[18]);
                                    sdata[7]=mT->P[8];//unut weight error of PSF fitting
                                    sdata[8]=mT->resUcac2->u2R;//UCAC2 Rmag
                                    sdata[9]=mT->resUcac2->J;//Jmag from 2MASS
                                    sdata[10]=mT->resUcac2->H;//Hmag from 2MASS
                                    sdata[11]=mT->resUcac2->Ks;//Ks_mag from 2MASS
                                    sdata[12]=mT->P[9];//standard error of pixel coordinate X
                                    sdata[13]=mT->P[10];//standard error of pixel coordinate Y
                                    if(objType==1)
                                    {
                                            /*dist = sqrt(pow(tp[0]-planetKsi,2)+pow(tp[1]-planetEta,2));
                                            if((dist>distfromplanet)&&(fabs(tp[1]-planetEta)>distfromsmear))
                                                                            refstars << sdata;/
                                    }
                                    refstars << sdata;
                                //}

                                break;
                        }
                        case 1://2mass
                        /*{
                                if ((catLine[0]!='#')||(catLine[0]!='_')||(catLine[0]!='-')||(catLine[0]!='a'))
                                {
                                        elS = catLine.section('|',1,1);ra = elS.toDouble()*3600000;
                                        elS = catLine.section('|',2,2);de = elS.toDouble()*3600000;
                                        getPixPosFromWCS(ra, de, wcs_data,xy);//return X,Y of pixel in xy[0],xy[1]
                                        double *P = new double[21];
                                        if(getStimp(P, fits_buff, naxes[0], naxes[1], (double) fpv, (int) xy[0], (int) xy[1],
                                                 aperture, ringradius, ringwidth, subgrad, lb, model, nofit, delta))
                                        {
                                            sdata = new double[15];
                                            sdata[0]=ra;//RA from 2MASS
                                            sdata[1]=de;//DEC from 2MASS
                                            getRaDeToTang(ra, de, ra_c, de_c,tp);
                                            sdata[2]=tp[0];//tangential position KSI based on catalogue RA
                                            sdata[3]=tp[1];//tangential position ETA based on catalogue DE
                                            sdata[4]=P[0] - wcs_data[0];//measured pixel position X (O)
                                            sdata[5]=P[1] - wcs_data[1];//measured pixel position Y (O)
                                            sdata[6]=P[18];//mean value of pixels within aperture
                                            sdata[7]=P[8];//unut weight error of PSF fitting
                                            elS = catLine.section('|',54,54);sdata[8] = elS.toDouble();//Bmag from USNO A2.0
                                            elS = catLine.section('|',55,55);sdata[9] = elS.toDouble();//Rmag from USNO A2.0
                                            elS = catLine.section('|',7,7);sdata[10] = elS.toDouble();//Jmag
                                            elS = catLine.section('|',11,11);sdata[11] = elS.toDouble();//Hmag
                                            elS = catLine.section('|',15,15);sdata[12] = elS.toDouble();//Ks_mag
                                            sdata[13]=P[9];//standard error of pixel coordinate X
                                            sdata[14]=P[10];//standard error of pixel coordinate Y
                                            if(objType==1)
                                            {
                                                    dist = sqrt(pow(tp[0]-planetKsi,2)+pow(tp[1]-planetEta,2));
                                                    if((dist>distfromplanet)&&(fabs(tp[1]-planetEta)>distfromsmear))
                                                                                    refstars << sdata;
                                            }
                                            else refstars << sdata;
                                        }
                                        delete[] P;
                                }

                        }/
                            break;
                            case 2://ucac2
                        {
                                /*
                                elS = catLine.section('|',0,0);ra = hms_to_mas(elS, " ");
                                elS = catLine.section('|',1,1);de = damas_to_mas(elS, " ");
                                elS = catLine.section('|',11,11);pmra = elS.toDouble();
                                elS = catLine.section('|',12,12);pmde = elS.toDouble();
                                ra = ra +pmra*(getYearFromMJD(mjd)-2000);//taking proper motion into account
                                de = de +pmde*(getYearFromMJD(mjd)-2000);//taking proper motion into account
                                getPixPosFromWCS(ra, de, wcs_data,xy);//return X,Y of pixel in xy[0],xy[1]
                                double *P = new double[21];
                                if(getStimp(P, fits_buff, naxes[0], naxes[1], (double) fpv, (int) xy[0], (int) xy[1],
                                                 aperture, ringradius, ringwidth, subgrad, lb, model, nofit, delta))
                                {/

                                    sdata = new double[14];
                                    sdata[0]=mT->resUcac2->ra;//RA from UCAC2
                                    sdata[1]=mT->resUcac2->de;//DEC from UCAC2
                                    //qDebug() << QString("sdata: ra= %1\tde= %2\n").arg(sdata[0]).arg(sdata[1]);
                                    getRaDeToTang1(&tp[0], &tp[1], mT->resUcac2->ra, mT->resUcac2->de, ra_c, de_c);
                                    sdata[2]=tp[0];//tangential position KSI based on catalogue RA
                                    sdata[3]=tp[1];//tangential position ETA based on catalogue DE
                                    sdata[4]=mT->mTanImg[0] - WCSdata[0];//measured pixel position X (O)
                                    sdata[5]=mT->mTanImg[1] - WCSdata[1];//measured pixel position Y (1)
                                    sdata[6]=mT->P[18];//mean value of pixels within aperture
                                    //qDebug() << QString("sdata: mT->P[18] = %1\n").arg(mT->P[18]);
                                    sdata[7]=mT->P[8];//unut weight error of PSF fitting
                                    sdata[8]=mT->resUcac2->u2R;//UCAC2 Rmag
                                    sdata[9]=mT->resUcac2->J;//Jmag from 2MASS
                                    sdata[10]=mT->resUcac2->H;//Hmag from 2MASS
                                    sdata[11]=mT->resUcac2->Ks;//Ks_mag from 2MASS
                                    sdata[12]=mT->P[9];//standard error of pixel coordinate X
                                    sdata[13]=mT->P[10];//standard error of pixel coordinate Y
                                    if(objType==1)
                                    {
                                            /*dist = sqrt(pow(tp[0]-planetKsi,2)+pow(tp[1]-planetEta,2));
                                            if((dist>distfromplanet)&&(fabs(tp[1]-planetEta)>distfromsmear))
                                                                            refstars << sdata;/
                                    }
                                    refstars << sdata;
                                //}

                                break;
                        }
                 }
        }
        qDebug() << QString("refstars num: %1\n").arg(refstars.size());
        //END load data from catalogue
/////////5. Reduction procedures ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //BEGIN reduction
        QVector<int> rsindex;
        double *KSI, *ETA, *MAG, *WEksi, *WEeta, *C, *CMAG;
        double minErrKsi=1000, minErrEta=1000;
        double errksi,erreta;

        rsindex.clear();
        for (int i=0;i<refstars.count();i++)
        {
           sdata = refstars.at(i);
           qDebug() << QString("sdata8[%1]= %2\n").arg(i).arg(sdata[8]);
           if((sdata[8]>=minrefMag)&&(sdata[8]<=maxrefMag)) rsindex << i;
        }
        KSI = new double [rsindex.count()];
        ETA = new double [rsindex.count()];
        MAG = new double [rsindex.count()];
        WEksi  = new double [rsindex.count()];
        WEeta  = new double [rsindex.count()];
        C = new double [rsindex.count()*3];
        CMAG = new double [rsindex.count()*2];
        //////////////////////////////////////

        qDebug() << QString("ref star good num: %1\n").arg(rsindex.count());
        for (int i=0;i<rsindex.count();i++)
        {
            sdata = refstars.at(rsindex[i]);
            switch(catindex)
            {
                case 0:{errksi = sdata[12];erreta = sdata[13];break;}
                case 1:{errksi = sdata[13];erreta = sdata[14];break;}
            }
            if(errksi<minErrKsi)minErrKsi = errksi;
            if(erreta<minErrEta)minErrEta = erreta;
        }
        //////////////////////////////////////
        for (int i=0;i<rsindex.count();i++)
        {
                sdata = refstars.at(rsindex[i]);
                KSI[i] = sdata[2];
                ETA[i] = sdata[3];
                MAG[i] = sdata[8];
                C[i*3+0] = sdata[4];C[i*3+1] = sdata[5];C[i*3+2] = 1;
                qDebug() << QString("A*%1 + B*%2 + C*%3 = %4 | %5\n").arg(C[i*3+0]).arg(C[i*3+1]).arg(C[i*3+2]).arg(KSI[i]).arg(ETA[i]);
                CMAG[i*2+0] = sdata[6];CMAG[i*2+1] = 1;
                qDebug() << QString("A*%1 + B*%2 = %3\n").arg(CMAG[i*2+0]).arg(CMAG[i*2+1]).arg(MAG[i]);
                switch(catindex)
                {
                    case 0:{errksi = sdata[12];erreta = sdata[13];break;}
                    case 1:{errksi = sdata[13];erreta = sdata[14];break;}
                }
                if(weights) {WEksi[i]=pow(minErrKsi/errksi,2);WEeta[i]=pow(minErrEta/erreta,2);} else {WEksi[i]=1;WEeta[i]=1;}
        }
        int *EXCLIND = new int [rsindex.count()];
        double *ZKSI=new double[3];
        double *DKSI=new double[3*3];
        double UWEKSI;
        int RNKSI=0;
        iLSM(3, rsindex.count(), maxres/1000, EXCLIND, ZKSI, C, KSI, UWEKSI, DKSI, 3, RNKSI, WEksi);
        double *ZETA=new double[3];
        double *DETA=new double[3*3];
        double UWEETA;
        int RNETA=0;
        iLSM(3, rsindex.count(), maxres/1000, EXCLIND, ZETA, C, ETA, UWEETA, DETA, 3, RNETA, WEeta);
        double *ZMAG=new double[2];
        double *DMAG=new double[2*2];
        double UWEMAG;
        int RNMAG=0;
        iLSM(2, rsindex.count(), maxresMAG, EXCLIND, ZMAG, CMAG, MAG, UWEMAG, DMAG, 3, RNMAG);
        //END reduction
        //report
        //QMessageBox::information(0, "Reduction complited", QString("Nx= %1\tNy= %2\nUWEx= %3\tUWEy= %4").arg(RNKSI).arg(RNETA).arg(sqrt(UWEKSI)*1000).arg(sqrt(UWEETA)*1000));
        //
/////////6. Calculation of objects positions//////////////////////////////////////////////////////////////////////////////////////////////////
        //BEGIN of calculation of objects positions
        QList<double*> outlist;
        double *outdata;
        double x,y,pixmag,ksi,eta;
        double *cp = new double[2];
        //stream << QString( "%1" ).arg( objects.count(),5,10,QLatin1Char( ' ' )) <<'\n';
        for (int i=0;i<objects.count();i++)
        {
            objdata = objects.at(i);
            x = objdata[4];
            y = objdata[5];
            pixmag = objdata[6];
            ksi = ZKSI[0]*x+ZKSI[1]*y+ZKSI[2];
            eta = ZETA[0]*x+ZETA[1]*y+ZETA[2];
            //BEGIN add systematic corrections
            /*if (syscorr==1)
            {
                ksi = ksi - syscorr_ra(objdata[8])/1000;
                eta = eta - syscorr_de(objdata[8])/1000;
            }/
            //END add systematic corrections
            getTangToRaDe1(&cp[0], &cp[1], ksi,eta,ra_c,de_c);
            mag = ZMAG[0]*pixmag+ZMAG[1];//syscorr_mag
            //BEGIN add systematic corrections to mag
            /*if (syscorr==1)
            {
               mag = mag - syscorr_mag(objdata[8]);
            }/
            //END add systematic corrections  to mag
            //
            outdata = new double[22];
            outdata[0] = mjd;//MJD
            outdata[1] = cp[0];//RA(O)
            outdata[2] = cp[1];//DEC(O)
            outdata[3] = mag;//MAG(O)
            outdata[4] = ksi - objdata[2];//O-C in RA
            outdata[5] = eta - objdata[3];//O-C in DEC
            outdata[6] = mag - objdata[8];//O-C in MAG

            outdata[20] = ra_c;//RA optical center
            outdata[21] = de_c;//DEC optical center
            //
            switch(objType)
            {
                case 0:
                {
                    outdata[7] = objdata[9];//topocentric distance [a.e.]
                    outdata[8] = objdata[10];//mu_ra*cos(dec) [mas/min]
                    outdata[9] = objdata[11];//mu_de [mas/min]
                    outdata[10] = objdata[12];//v_r [km/s]
                    outdata[11] = objdata[13];//phase [deg]
                    outdata[12] = objdata[14];//elongation [deg]
                    outdata[13] = objdata[15];//number of the asteroid
                    outdata[14] = objdata[2];//ksi(c)
                    outdata[15] = objdata[3];//eta(c)
                    outdata[16] = ksi;//ksi(o)
                    outdata[17] = eta;//eta(o)
                    outdata[18] = objdata[8];//mag(c)
                    break;
                }
                case 1:
                {
                    outdata[7] = objdata[9];//Sun-plan[deg]
                    outdata[8] = objdata[10];//Phase[deg]
                    outdata[9] = objdata[11];//Rad.plan. - apparent planetary radius [arcesec]
                    outdata[10] = objdata[12];//sat number
                    outdata[11] = objdata[8];//mag
                    outdata[12] = objdata[2] - planetKsi;//planetocentric position in Ksi
                    outdata[13] = objdata[3] - planetEta;//planetocentric position in Eta
                    outdata[14] = sqrt(pow(objdata[2] - planetKsi,2)+pow(objdata[3] - planetEta,2));//angular distance from planet
                    outdata[15] = ksi;//ksi(o)
                    outdata[16] = eta;//eta(o)
                    break;
                }
            }
            //
            if((fabs(mag - objdata[8])<=maxOCMAG)&&
                    (1000*fabs(ksi - objdata[2])<=maxOCRA)&&
                    (1000*fabs(eta - objdata[3])<=maxOCDE))
            {
                outlist << outdata;

                //QMessageBox::information(0, "Object pos", QString("Object[%1]\nOMCx= %2\tOMCy= %3").arg(i).arg(outdata[4]*1000,8,'f',1,QLatin1Char( ' ' )).arg(outdata[5]*1000,8,'f',1,QLatin1Char( ' ' )));
            }

        }
        //END of calculation of objects positions
/////////7. OUTPUT of the results to file(s) //////////////////////////////////////////////////////////////////////////////////////////////////
                //BEGIN output
                DATEOBS dateObs;
                QString outstr;
                double tTm;
                //BEGIN astreport
                if((astreport==1)&&(outlist.count()!=0))
                {
                    for (int i=0;i<outlist.count();i++)
                    {
                        outdata = outlist.at(i);
                        outstr ="";
                        tTm = outdata[0];
                        switch(time_form)
                                {
                                         case 0:{dateObs = getDATEOBSfromMJD(outdata[0]); outstr = outstr + getStrFromDATEOBS(dateObs," ", 0, 5)+sepsymb; break;}
                                         case 1:{outstr = outstr + QString( "%1" ).arg(outdata[0],17,'f',10,QLatin1Char( ' ' ))+sepsymb; break;}
                                         case 2:{outstr = outstr + QString( "%1" ).arg( (mjd-mjd0)*1440,8,'f',3,QLatin1Char( ' ' ))+sepsymb; break;}
                                }
                        switch(pos_form)
                                {
                                        case 0:{outstr = outstr + mas_to_hms(outdata[1], " ", 4)+ sepsymb + mas_to_damas(outdata[2], " ", 3)+sepsymb;break;}
                                        case 1:{outstr = outstr + QString( "%1" ).arg( outdata[1],12,'f',1,QLatin1Char( ' ' ))+ sepsymb + QString( "%1" ).arg( outdata[2],12,'f',1,QLatin1Char( ' ' ))+sepsymb;break;}
                                        case 2:{outstr = outstr + QString( "%1" ).arg( outdata[1]/3600000,12,'f',8,QLatin1Char( ' ' ))+ sepsymb + QString( "%1" ).arg( outdata[2]/3600000,12,'f',8,QLatin1Char( ' ' ))+sepsymb;break;}
                                }
                        outstr = outstr + QString( "%1" ).arg(outdata[3],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
                                        QString( "%1" ).arg(outdata[4]*1000,8,'f',1,QLatin1Char( ' ' ))+sepsymb+
                                        QString( "%1" ).arg(outdata[5]*1000,8,'f',1,QLatin1Char( ' ' ))+sepsymb+
                                        QString( "%1" ).arg(outdata[6],8,'f',1,QLatin1Char( ' ' ))+sepsymb;

                        switch(pos_form)
                        {
                                case 0:{outstr = outstr + mas_to_hms(outdata[20], " ", 4)+ sepsymb + mas_to_damas(outdata[21], " ", 3)+sepsymb;break;}
                                case 1:{outstr = outstr + QString( "%1" ).arg( outdata[20],12,'f',1,QLatin1Char( ' ' ))+ sepsymb + QString( "%1" ).arg( outdata[21],12,'f',1,QLatin1Char( ' ' ))+sepsymb;break;}
                                case 2:{outstr = outstr + QString( "%1" ).arg( outdata[20]/3600000,12,'f',8,QLatin1Char( ' ' ))+ sepsymb + QString( "%1" ).arg( outdata[21]/3600000,12,'f',8,QLatin1Char( ' ' ))+sepsymb;break;}
                        }
                        //
                        switch(objType)
                        {
                            case 0://asteroids
                            {
                                outstr = outstr +
                                QString( "%1" ).arg(outdata[7],12,'f',9,QLatin1Char( ' ' ))+sepsymb+//topocentric distance [a.e.]
                                QString( "%1" ).arg(outdata[8],9,'f',2,QLatin1Char( ' ' ))+sepsymb+//mu_ra*cos(dec) [mas/min]
                                QString( "%1" ).arg(outdata[9],9,'f',2,QLatin1Char( ' ' ))+sepsymb+//mu_de [mas/min]
                                QString( "%1" ).arg(outdata[10],6,'f',2,QLatin1Char( ' ' ))+sepsymb+//v_r [km/s]
                                QString( "%1" ).arg(outdata[11],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//phase [deg]
                                QString( "%1" ).arg(outdata[12],8,'f',3,QLatin1Char( ' ' ))+sepsymb;//elongation [deg]
                                msgstr = QString( "%1" ).arg((int)outdata[13],5,10,QLatin1Char( '0' ));
                                msgstr = msgstr.trimmed();
                                outstr = outstr + msgstr + sepsymb + QString( "%1" ).arg(catindex,1,10,QLatin1Char( ' ' ))+sepsymb+
                                QString( "%1" ).arg(exptime,8,'f',3,QLatin1Char( ' ' ))+"\n";
                                break;
                            }
                            case 1://saturnian satellites
                            {
                                outstr = outstr +
                                QString( "%1" ).arg(outdata[11],6,'f',2,QLatin1Char( ' ' ))+sepsymb+//mag
                                QString( "%1" ).arg(outdata[7],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//Sun-plan[deg]
                                QString( "%1" ).arg(outdata[8],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//Phase[deg]
                                QString( "%1" ).arg(outdata[9],6,'f',3,QLatin1Char( ' ' ))+sepsymb+//Rad.plan. - apparent planetary radius [arcesec]
                                QString( "%1" ).arg(outdata[12],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//Ksi referred to planet
                                QString( "%1" ).arg(outdata[13],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//Eta referred to planet
                                QString( "%1" ).arg(outdata[14],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//angular distance from planet
                                QString( "%1" ).arg((int)outdata[10],3,10,QLatin1Char( '0' ));//satellite number
                                msgstr = "S"+QString( "%1" ).arg((int)outdata[10],3,10,QLatin1Char( '0' ));
                                msgstr = msgstr.trimmed();
                                outstr = outstr  + sepsymb + QString( "%1" ).arg(catindex,1,10,QLatin1Char( ' ' ))+sepsymb+
                                QString( "%1" ).arg(exptime,8,'f',3,QLatin1Char( ' ' ))+"\n";
                                break;
                            }
                        }
                        ///////////////////////////////////////////////////
                        QFile astFile(outfolder+"/"+msgstr+"_eq.txt");
                        astFile.open(QIODevice::Append| QIODevice::Text);
                        QTextStream astStream;
                        astStream.setDevice(&astFile);
                        astStream << outstr;
                        astFile.close();
                    }
                }
                //END astreport
                //BEGIN relative positions of satellites
                if((relativepos==1)&&(outlist.count()>1))
                {
                    double *outdata1;
                    double *outdata2;
                    for (int i=0;i<outlist.count();i++)
                    {
                        for (int j=i+1;j<outlist.count();j++)
                        {
                            outdata1 = outlist.at(i);
                            outdata2 = outlist.at(j);
                            outstr ="";
                            switch(time_form)
                                    {
                                         case 0:{dateObs = getDATEOBSfromMJD(outdata1[0]); outstr = outstr + getStrFromDATEOBS(dateObs," ", 0, 5)+sepsymb; break;}
                                         case 1:{outstr = outstr + QString( "%1" ).arg(outdata1[0],17,'f',10,QLatin1Char( ' ' ))+sepsymb; break;}
                                         case 2:{outstr = outstr + QString( "%1" ).arg( (mjd-mjd0)*1440,8,'f',3,QLatin1Char( ' ' ))+sepsymb; break;}
                                    }
                            switch(objType)
                            {
                                case 0://asteroids
                                {
                                    outstr = outstr +
                                    QString( "%1" ).arg(outdata2[16]-outdata1[16],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//delta ksi (o) [arcsec]
                                    QString( "%1" ).arg(outdata2[17]-outdata1[17],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//delta eta (o) [arcsec]
                                    QString( "%1" ).arg(outdata2[3]-outdata1[3],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//delta mag
                                    QString( "%1" ).arg(((outdata2[16]-outdata1[16])-(outdata2[14]-outdata1[14]))*1000,8,'f',3,QLatin1Char( ' ' ))+sepsymb+//(o-c) ksi [mas]
                                    QString( "%1" ).arg(((outdata2[17]-outdata1[17])-(outdata2[15]-outdata1[15]))*1000,8,'f',3,QLatin1Char( ' ' ))+sepsymb+//o-c) eta [mas]
                                    QString( "%1" ).arg((outdata2[3]-outdata1[3])-(outdata2[18]-outdata1[18]),8,'f',3,QLatin1Char( ' ' ))+sepsymb+//o-c)  mag
                                    QString( "%1" ).arg((int)outdata2[13],3,10,QLatin1Char( '0' ))+"-"+
                                    QString( "%1" ).arg((int)outdata1[13],3,10,QLatin1Char( '0' ));//satellite numbers
                                    outstr = outstr  + sepsymb + QString( "%1" ).arg(catindex,1,10,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(exptime,8,'f',3,QLatin1Char( ' ' ))+"\n";
                                    msgstr = QString( "%1" ).arg((int)outdata2[13],3,10,QLatin1Char( '0' ))+"-"+
                                    QString( "%1" ).arg((int)outdata1[13],3,10,QLatin1Char( '0' ));
                                    msgstr = msgstr.trimmed();
                                    break;
                                }
                                case 1://saturnian satellites
                                {
                                    outstr = outstr +
                                    QString( "%1" ).arg(outdata2[15]-outdata1[15],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//delta ksi (o) [arcsec]
                                    QString( "%1" ).arg(outdata2[16]-outdata1[16],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//delta eta (o) [arcsec]
                                    QString( "%1" ).arg(outdata2[3]-outdata1[3],8,'f',3,QLatin1Char( ' ' ))+sepsymb+//delta mag
                                    QString( "%1" ).arg(((outdata2[15]-outdata1[15])-(outdata2[12]-outdata1[12]))*1000,8,'f',3,QLatin1Char( ' ' ))+sepsymb+//(o-c) ksi [mas]
                                    QString( "%1" ).arg(((outdata2[16]-outdata1[16])-(outdata2[13]-outdata1[13]))*1000,8,'f',3,QLatin1Char( ' ' ))+sepsymb+//o-c) eta [mas]
                                    QString( "%1" ).arg((outdata2[3]-outdata1[3])-(outdata2[11]-outdata1[11]),8,'f',3,QLatin1Char( ' ' ))+sepsymb+//o-c)  mag
                                    QString( "%1" ).arg((int)outdata2[10],3,10,QLatin1Char( '0' ))+"-"+
                                    QString( "%1" ).arg((int)outdata1[10],3,10,QLatin1Char( '0' ));//satellite numbers
                                    outstr = outstr  + sepsymb + QString( "%1" ).arg(catindex,1,10,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(exptime,8,'f',3,QLatin1Char( ' ' ))+"\n";
                                    msgstr = "S"+QString( "%1" ).arg((int)outdata2[10],3,10,QLatin1Char( '0' ))+"-S"+
                                    QString( "%1" ).arg((int)outdata1[10],3,10,QLatin1Char( '0' ));
                                    msgstr = msgstr.trimmed();
                                    break;
                                }
                            }
                            ///////////////////////////////////////////////////
                            QFile relFile(outfolder+"/"+msgstr+"_rel.txt");
                            relFile.open(QIODevice::Append| QIODevice::Text);
                            QTextStream relStream;
                            relStream.setDevice(&relFile);
                            relStream << outstr;
                            relFile.close();
                        }
                    }
                }
                //END relative positions of satellites
                //BEGIN errorreport
                if(errorreport)
                {
                    outstr = QString( "%1" ).arg(RNKSI,3,10,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(sqrt(UWEKSI)*1000,6,'f',1,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(ZKSI[0],10,'f',5,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(sqrt(DKSI[0]),10,'f',5,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(ZKSI[1],10,'f',5,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(sqrt(DKSI[4]),10,'f',5,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(ZKSI[2],10,'f',5,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(sqrt(DKSI[8]),10,'f',5,QLatin1Char( ' ' ))+sepsymb;
                    outstr = outstr + QString( "%1" ).arg(RNETA,3,10,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(sqrt(UWEETA)*1000,6,'f',1,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(ZETA[0],10,'f',5,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(sqrt(DETA[0]),10,'f',5,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(ZETA[1],10,'f',5,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(sqrt(DETA[4]),10,'f',5,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(ZETA[2],10,'f',5,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(sqrt(DETA[8]),10,'f',5,QLatin1Char( ' ' ))+sepsymb;
                    outstr = outstr + QString( "%1" ).arg(RNMAG,3,10,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(sqrt(UWEMAG),5,'f',2,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(ZMAG[0],10,'f',5,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(sqrt(DMAG[0]),10,'f',5,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(ZMAG[1],10,'f',5,QLatin1Char( ' ' ))+sepsymb+
                                    QString( "%1" ).arg(sqrt(DMAG[3]),10,'f',5,QLatin1Char( ' ' ))+"\n";
                    QFile errFile(outfolder+"/err_budget.txt");
                    errFile.open(QIODevice::Append| QIODevice::Text);
                    QTextStream errStream;
                    errStream.setDevice(&errFile);
                    errStream << outstr;
                    errFile.close();
                }
                //END astreport
                //BEGIN residuals
                if(residuals)
                {
                        outstr = "";
                        double a1 = ZKSI[0];
                        double b1 = ZKSI[1];
                        double c1 = ZKSI[2];
                        double a2 = ZETA[0];
                        double b2 = ZETA[1];
                        double c2 = ZETA[2];
                        double am = ZMAG[0];
                        double bm = ZMAG[1];
                        double xc,yc,pmag;
                        for (int i=0;i<refstars.count();i++)
                        {
                                sdata = refstars.at(i);
                                x = sdata[4];
                                y = sdata[5];
                                pixmag = sdata[6];
                                ksi = a1*x+b1*y+c1;
                                eta = a2*x+b2*y+c2;
                                //BEGIN add systematic corrections
                                /*if (syscorr==1)
                                {
                                                ksi = ksi - syscorr_ra(sdata[8])/1000;
                                                eta = eta - syscorr_de(sdata[8])/1000;
                                }/
                                //END add systematic corrections
                                mag = am*pixmag+bm;
                                //BEGIN add systematic corrections to mag
                                /*if (syscorr==1)
                                {
                                                mag = mag - syscorr_mag(sdata[8]);
                                }/
                                //END add systematic corrections  to mag
                                if((fabs(mag - sdata[8])<=maxOCMAGrc)&&(1000*fabs(ksi - sdata[2])<=maxOCRArc)&&(1000*fabs(eta - sdata[3])<=maxOCDErc))
                                {
                                        yc = ((sdata[3]-c2)-a2*(sdata[2]-c1)/a1)/(b2-a2*b1/a1);
                                        xc = ((sdata[2]-c1)-b1*yc)/a1;
                                        pmag = (sdata[8]-bm)/am;
                                        outstr = outstr + QString( "%1" ).arg(mjd, 11, 'f', 3, QLatin1Char(' '))+sepsymb+
                                                                          QString("%1" ).arg(sdata[0]/3600000,12,'f',8,QLatin1Char( ' ' ))+sepsymb+
                                                                          QString( "%1" ).arg(sdata[1]/3600000,12,'f',8,QLatin1Char( ' ' ))+sepsymb+
                                                                          QString( "%1" ).arg(ksi,10,'f',4,QLatin1Char( ' ' ))+sepsymb+
                                                                          QString( "%1" ).arg(eta,10,'f',4,QLatin1Char( ' ' ))+sepsymb+
                                                                          QString( "%1" ).arg(mag,5,'f',2,QLatin1Char( ' ' ))+sepsymb+
                                                                          QString( "%1" ).arg((ksi - sdata[2])*1000,7,'f',1,QLatin1Char( ' ' ))+sepsymb+
                                                                          QString( "%1" ).arg((eta - sdata[3])*1000,7,'f',1,QLatin1Char( ' ' ))+sepsymb+
                                                                          QString( "%1" ).arg(mag - sdata[8],5,'f',2,QLatin1Char( ' ' ))+sepsymb+
                                                                          QString( "%1" ).arg(x,10,'f',4,QLatin1Char( ' ' ))+sepsymb+
                                                                          QString( "%1" ).arg(y,10,'f',4,QLatin1Char( ' ' ))+sepsymb+
                                                                          QString( "%1" ).arg(pixmag,10,'f',6,QLatin1Char( ' ' ))+sepsymb+
                                                                          QString( "%1" ).arg((x - xc)*1000,7,'f',1,QLatin1Char( ' ' ))+sepsymb+
                                                                          QString( "%1" ).arg((y - yc)*1000,7,'f',1,QLatin1Char( ' ' ))+sepsymb+
                                                                          QString( "%1" ).arg(pixmag - pmag,9,'f',6,QLatin1Char( ' ' ))+sepsymb;
                                        switch(catindex)
                                        {
                                                 case 0:
                                                 {
                                                        outstr = outstr + QString( "%1" ).arg(sdata[8],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(sdata[9],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(sdata[10],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(sdata[11],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(sdata[8]-sdata[11],7,'f',3,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(catindex,1,10,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(exptime,8,'f',3,QLatin1Char( ' ' ))+"\n";
                                                        break;
                                                 }
                                                 case 1:
                                                 {
                                                        outstr = outstr + QString( "%1" ).arg(sdata[8],4,'f',1,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(sdata[9],4,'f',1,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(sdata[10],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(sdata[11],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(sdata[12],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(sdata[8]-sdata[12],7,'f',3,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(catindex,1,10,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(exptime,8,'f',3,QLatin1Char( ' ' ))+"\n";
                                                        break;
                                                 }
                                                 case 2:
                                                 {
                                                        outstr = outstr + QString( "%1" ).arg(sdata[8],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(sdata[9],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(sdata[10],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(sdata[11],6,'f',3,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(sdata[8]-sdata[11],7,'f',3,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(catindex,1,10,QLatin1Char( ' ' ))+sepsymb+
                                                                                          QString( "%1" ).arg(exptime,8,'f',3,QLatin1Char( ' ' ))+"\n";
                                                        break;
                                                 }
                                        }
                                }
                        }
                        QFile resFile(outfolder+"/residuals.txt");
                        resFile.open(QIODevice::Append| QIODevice::Text);
                        QTextStream resStream;
                        resStream.setDevice(&resFile);
                        resStream << outstr;
                        resFile.close();
                }
                //END residuals
                //BEGIN MPC report
                if((objType==0)&&(mpcreport==1))
                {
                    outstr ="";
                    int obj_num;
                    for (int i=0;i<outlist.count();i++)
                    {
                        outdata = outlist.at(i);
                        dateObs = getDATEOBSfromMJD(outdata[0]);
                        obj_num = (int)outdata[13];
                        outstr = outstr +QString( "%1" ).arg( obj_num,5,10,QLatin1Char( '0' ))+"         C";
                        outstr = outstr +getStrFromDATEOBS(dateObs," ", 0, 5) +" ";
                        outstr = outstr +mas_to_hms(outdata[1], " ",3);
                        outstr = outstr +mas_to_damas(outdata[2], " ",2)+"         ";
                        outstr = outstr  +QString( "%1" ).arg( outdata[3],3,'f',1,QLatin1Char( '0' ));
                        outstr = outstr  +"        "+obscode+"\n";
                        msgstr = QString( "%1" ).arg(obj_num,5,10,QLatin1Char( '0' ));
                        msgstr = msgstr.trimmed();
                        //QMessageBox::information(0,"debug",outfolder+"/"+msgstr+"_mpc.txt",QMessageBox::Ok,0,0);
                        QFile mpcFile(outfolder+"/"+msgstr+"_mpc.txt");
                        mpcFile.open(QIODevice::Append| QIODevice::Text);
                        QTextStream mpcStream;
                        mpcStream.setDevice(&mpcFile);
                        mpcStream << outstr;
                        mpcFile.close();
                    }
                }
                //END MPC report
                //END output
/////////HAPPY END////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




    return 0;
}
/
int fitsdata::makeReduction(QString iniFile, QString resFolder)
{
    int i, sz;
    //double dist;
    marksP *mT;
    QSettings *sett;
    measureParam params;

    if(!is_empty)
    {
        sett = new QSettings(iniFile, QSettings::IniFormat);

        params.model = sett->value("psf/model", 3).toInt();//settings->value("psf/model").toInt();//PSF model: 0 - Lorentz PSF, 1 - Gauss PSF, 2 - Moffat PSF, 3 - CoM
        params.nofit = sett->value("psf/nofit", 10).toInt();
        params.delta = sett->value("psf/delta", 1.0).toDouble();
        params.ringradius = sett->value("psf/ringradius", 10).toInt();
        params.ringwidth = sett->value("psf/ringwidth", 6).toInt();
        params.lb = sett->value("psf/lb", 1).toInt();
        params.sg = sett->value("psf/subgrad", 1).toInt();
        params.aperture = sett->value("psf/aperture", 10).toInt();

        ipixMarks->clearMarks();
        sz = catMarks->marks.size();
        mT = new marksP();

//        marksP *mDist = new marksP;
        for(i=sz-1; i>=0; i--)
        {

            mT->copyImg(catMarks->marks.at(i));

            if(markMassCenter(mT, params.aperture)||(!measureMark(mT, params))||(marksImgDist(catMarks->marks.at(i), mT)>(params.aperture/2.0)))
            //if(markMassCenter(mT, params.aperture)||(!measureMark(mT, params)))
            {
                //catMarks->marks.removeAt(i);
                continue;
            }
            ipixMarks->marks << mT;
            mT = new marksP();
        }
    }
    findCloserStars(params.aperture);
    if(!is_empty)
    {
        ipixMarks->clearMarks();
        sz = objMarks->marks.size();

        qDebug() << QString("\nObject num: %1\n").arg(sz);
//        marksP *mDist = new marksP;
        for(i=sz-1; i>=0; i--)
        {
            mT = new marksP;
            qDebug() << QString("%1:%2\n").arg(i).arg(sz);
            mT->copyImg(objMarks->marks.at(i));
            //markMassCenter(mT, params.aperture);

            if(markMassCenter(mT, params.aperture)||(!measureMark(mT, params)))
            {
                //objMarks->remMark(i);
                continue;
            }
            ipixMarks->marks << mT;
            //mT = new marksP;
            //catMarks->marks.at(i)->copyImg(mT);
        }
        qDebug() << QString("ipixMarks num: %1\n").arg(ipixMarks->marks.size());
    }
    findCloserObjects(params.aperture);

    ruler3(iniFile, resFolder);

    //delete mT;
}
*/
/*
int fitsdata::makeReduction(QString iniFile, QString resFolder)
{
    qDebug() << QString("Start makeReduction1\n");
    int i, sz;
    marksP *mT;
    QSettings *sett;
    measureParam params;
    sett = new QSettings(iniFile, QSettings::IniFormat);

    QStringList allSett;
    allSett << sett->allKeys();
    sz = allSett.size();
    qDebug() << QString("\n\n%1:\n").arg(iniFile);
    for(i=0; i<sz; i++) qDebug() << QString("%1=%2\n").arg(allSett.at(i)).arg(sett->value(allSett.at(i)).toString());
    qDebug() << "\n\n";
    int isObj = sett->value("general/doObjRed", 0).toInt();
    int isBase = sett->value("general/doBaseRed", 0).toInt();

    if((!isBase)&&(objMarks->marks.size()==0)) return 0;

    params.model = sett->value("psf/model", 3).toInt();
    params.nofit = sett->value("psf/nofit", 10).toInt();
    params.delta = sett->value("psf/delta", 1.2).toDouble();
    params.ringradius = sett->value("psf/ringradius", 10).toInt();
    params.ringwidth = sett->value("psf/ringwidth", 6).toInt();
    params.lb = sett->value("psf/lb", 1).toInt();
    params.sg = sett->value("psf/subgrad", 1).toInt();
    params.aperture = sett->value("psf/aperture", 10).toInt();
    cleanObjects(params.aperture);
    if(!is_empty)
    {
        ipixMarks->clearMarks();
        sz = objMarks->marks.size();

        for(i=sz-1; i>=0; i--)
        {
            mT = new marksP;
            //mT->P
            qDebug() << QString("%1:%2\n").arg(i).arg(sz);
            mT->copyImg(objMarks->marks.at(i));
            if(markMassCenter(mT, params.aperture)||(!measureMark(mT, params)))
            {
                continue;
            }
            ipixMarks->marks << mT;
        }
    }
    findCloserObjects(params.aperture);
    if(!is_empty)
    {
        ipixMarks->clearMarks();
        copyImgGrid(catMarks, ipixMarks);

        moveMassCenter(ipixMarks, params.aperture);
        measureMarksGrid(ipixMarks, params);
    }
    findCloserStars(params.aperture);

    qDebug() << QString("refStars: %1\n").arg(refMarks->marks.size());


    return(ruler3(iniFile, resFolder));

}
/*
void fitsdata::prerObjects()
{
    double tp[2];
    int szObj = objMarks->marks.size();
    int i;
    marksP *mT;
    double ra, de;
    //double OC0, OC1;
    double dX, dY;
    long ni;


    qDebug() << QString("object num: %1\n").arg(szObj);
    qDebug() << "refMaker: " << refMaker << "\n";
    for(i=0; i<szObj; i++)
    {
        mT = objMarks->marks.at(i);
       /* if(refMaker!=NULL) refMaker->forvRefRel(&ra, &de, mT->mEkv[0], mT->mEkv[1]);
        else
        {
            ra = mT->mEkv[0];
            de = mT->mEkv[1];
        }/
        if(refMaker!=NULL)
        {
            refMaker->forvRef(&ra, &de, mT->mEkv[0], mT->mEkv[1]);
            getDegToTang(&tp[0], &tp[1], ra, de, WCSdata[2], WCSdata[3]);
        }
        else
        {
            getDegToTang(&tp[0], &tp[1], mT->mEkv[0], mT->mEkv[1], WCSdata[2], WCSdata[3]);
        }
        //getDegToTang(&tp[0], &tp[1], ra, de, oc0, oc1);
        qDebug() << QString("tp: %1\t%2\n").arg(tp[0]).arg(tp[1]);
        if(mT->objdata==NULL) mT->objdata = new double[18];
        mT->objdata[0]= mT->mEkv[0];//RA from theory of motion or catalogue
        mT->objdata[1]= mT->mEkv[1];//DEC from theory of motion  or catalogue
        mT->objdata[2]=tp[0];//tangential position KSI based on RA (C)
        mT->objdata[3]=tp[1];//tangential position ETA based on DE (C)
        if(isComaCorr)
        {
            mT->objdata[2] += comaCorr(mT->objdata[2], mT->mEkv[2], comaKsi, comaMag0);
            mT->objdata[3] += comaCorr(mT->objdata[3], mT->mEkv[2], comaEta, comaMag0);
        }
        mT->objdata[4]=mT->mTanImg[0];// - WCSdata[0];//measured pixel position X (O)
        mT->objdata[5]=mT->mTanImg[1];// - WCSdata[1];//measured pixel position Y (1)

        if(vfCorr0!=NULL)
        {
            if(vfCorr0->isBC)
            {
                //if(!vectFcorr->int2Drad(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, vfRmax, nMin))
                if(!vfCorr0->vectFcorr->int2D(mT->objdata[4], mT->objdata[5], mT->mEkv[2], &dX, &dY, &ni))
                {
   //                 dDec = -0.5082077*resRec->de + 18.1648793;
                        //dDec = 0.0;
                        //resRec->Dx-= dKsi;
                        //resRec->Dy-= (dEta+dDec);

                        mT->objdata[4]-= dX;
                        mT->objdata[5]-= dY;//+dDec;
                }
            }
            else
            {
                if(!vfCorr0->vectFcorr->int2Drad(mT->objdata[4], mT->objdata[5], mT->mEkv[2], &dX, &dY, vfCorr0->vfRmax, vfCorr0->nMin))
                //if(!vfCorr0->vectFcorr->int2D(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, &ni))
                {
   //                 dDec = -0.5082077*resRec->de + 18.1648793;
                        //dDec = 0.0;
                        //resRec->Dx-= dKsi;
                        //resRec->Dy-= (dEta+dDec);

                        mT->objdata[4]-= dX;
                        mT->objdata[5]-= dY;//+dDec;
                }
            }
        }


        if(isMagEqCorr)
        {
            mT->objdata[4] -= magEqCorr(mT->mEkv[2], mT->objdata[4], mCoefX, mCoefDeg);
            mT->objdata[5] -= magEqCorr(mT->mEkv[2], mT->objdata[5], mCoefY, mCoefDeg);
        }

        if(vfCorr1!=NULL)
        {
            if(vfCorr1->isBC)
            {
                //if(!vectFcorr->int2Drad(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, vfRmax, nMin))
                if(!vfCorr1->vectFcorr->int2D(mT->objdata[4], mT->objdata[5], mT->mEkv[2], &dX, &dY, &ni))
                {
   //                 dDec = -0.5082077*resRec->de + 18.1648793;
                        //dDec = 0.0;
                        //resRec->Dx-= dKsi;
                        //resRec->Dy-= (dEta+dDec);

                        mT->objdata[4]-= dX;
                        mT->objdata[5]-= dY;//+dDec;
                }
            }
            else
            {
                if(!vfCorr1->vectFcorr->int2Drad(mT->objdata[4], mT->objdata[5], mT->mEkv[2], &dX, &dY, vfCorr1->vfRmax, vfCorr1->nMin))
                //if(!vfCorr0->vectFcorr->int2D(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, &ni))
                {
   //                 dDec = -0.5082077*resRec->de + 18.1648793;
                        //dDec = 0.0;
                        //resRec->Dx-= dKsi;
                        //resRec->Dy-= (dEta+dDec);

                        mT->objdata[4]-= dX;
                        mT->objdata[5]-= dY;//+dDec;
                }
            }
        }

        mT->objdata[6]= mT->mTanImg[2];//mT->P[18];//log10 of summ of pixels within aperture
        if(mT->P!=NULL) mT->objdata[7]= mT->P[8];//unut weight error of PSF fitting
        else mT->objdata[7]= 0.0;
        if(mT->P!=NULL) mT->objdata[16]=mT->P[9];//unut weight error of PSF fitting
        else mT->objdata[16]= 0.0;
        if(mT->P!=NULL) mT->objdata[17]=mT->P[10];//unut weight error of PSF fitting
        else mT->objdata[17]= 0.0;
        //////////////////////////////////////////////////////
        mT->objdata[8] = mT->mEkv[2];//magnitude from theory of motion or catalogue
        switch(mT->objType)
        {
            case OBJ_TYPE_MPEPH://asteroids
            {
                mT->objdata[9]= mT->mpcObj->topDist;//topocentric distance from theory of motion or catalogue
                mT->objdata[10] = mT->mpcObj->muRaCosDe;//mu_ra*cos(dec) [mas/min]
                mT->objdata[11] = mT->mpcObj->muDe;//mu_de [mas/min]
                mT->objdata[12] = mT->mpcObj->Vr;//v_r [km/s]
                mT->objdata[13] = mT->mpcObj->phase;//phase [deg]
                mT->objdata[14] = mT->mpcObj->elong;//elongation [deg]
                break;
            }
            case OBJ_TYPE_SSTAR://saturnian satellite
            {
                mT->objdata[10] = mT->sstar->pmRA;//mu_ra*cos(dec) [mas/min]
                mT->objdata[11] = mT->sstar->pmDE;//mu_de [mas/min]
                break;
            }
            case OBJ_TYPE_SKYBOT://asteroids
            {
                mT->objdata[9]= mT->sbot->Dg;
                mT->objdata[10] = mT->sbot->dRA*1000.0/60.0;//mas/min
                mT->objdata[11] = mT->sbot->dDEC*1000.0/60.0;//mas/min
                mT->objdata[12] = 0.0;//v_r [km/s]
                mT->objdata[13] = mT->sbot->Phase;
                mT->objdata[14] = mT->sbot->SunElong;
                break;
            }
        }
            //////////////////////////////////////////////////////
    }

    qDebug() << QString("object num: %1\n").arg(szObj);
}

void fitsdata::prepRefStars()
{
    marksP *mT;
    int szRef = refMarks->marks.size();
    double tp[2];
    int i;
    double ra, de;
    qDebug() << QString("szRef= %1\n").arg(szRef);
    double cCorr;
    double dX, dY;
    long ni;

    QVector <double> dCoefX, dCoefY;

    if(isMagEqCorr) qDebug() << QString("\nmagnitude correction Enabled\n");
    else qDebug() << QString("\nmagnitude correction Disabled\n");
    if(isComaCorr) qDebug() << QString("\ncoma correction Enabled\n");
    else qDebug() << QString("\ncoma correction Disabled\n");

    if(vfCorr0!=NULL) qDebug() << QString("\nvfCorr0 correction Enabled\n");
    else qDebug() << QString("\nvfCorr0 correction Disabled\n");
    if(vfCorr1!=NULL) qDebug() << QString("\nvfCorr1 correction Enabled\n");
    else qDebug() << QString("\nvfCorr1 correction Disabled\n");
    if(mdcParam!=NULL) qDebug() << QString("\nmdcParam correction Enabled\n");
    else qDebug() << QString("\nmdcParam correction Disabled\n");
/*
    if(refMaker!=NULL)
    {
        refMaker->forvRef(&oc0, &oc1, WCSdata[2], WCSdata[3]);
        qDebug() << QString("oc0= %1\toc1= %2\n").arg(oc0).arg(oc1);
    }
    else qDebug() << QString("oc0= %1\toc1= %2\n").arg(WCSdata[2]).arg(WCSdata[3]);/
//    qDebug() << QString("WCSdata[0]= %1\tWCSdata[1]= %2\n").arg(WCSdata[0]).arg(WCSdata[1]);
    for(i=0; i<szRef; i++)
    {
        mT = refMarks->marks.at(i);
        qDebug() << QString("ctype: %1").arg(refMarks->ctype);
        /*switch(refMarks->ctype)
        {
            case 0://ucac2
            {
                    //qDebug() << "sdata:" << mT->sdata << "\n";
                if(mT->sdata==NULL) mT->sdata = new double[15];
                //qDebug() << "sdata:" << mT->sdata << "\n";
                mT->sdata[0]=mT->mEkv[0];//RA from UCAC2
                mT->sdata[1]=mT->mEkv[1];//DEC from UCAC2
                //qDebug() << QString("sdata: ra= %1\tde= %2\n").arg(sdata[0]).arg(sdata[1]);
                if(refMaker!=NULL)
                {
                    refMaker->forvRef(&ra, &de, mT->mEkv[0], mT->mEkv[1]);
                    getDegToTang(&tp[0], &tp[1], ra, de, oc0, oc1);
                }
                else
                {
                    getDegToTang(&tp[0], &tp[1], mT->mEkv[0], mT->mEkv[1], WCSdata[2], WCSdata[3]);
                }

                mT->sdata[2]=tp[0];//tangential position KSI based on catalogue RA
                mT->sdata[3]=tp[1];//tangential position ETA based on catalogue DE
                mT->sdata[4] = mT->mTanImg[0] - WCSdata[0];//measured pixel position X (O)
                mT->sdata[5] = mT->mTanImg[1] - WCSdata[1];//measured pixel position Y (1)
                mT->sdata[6] = mT->mTanImg[2];//mT->P[18];//mean value of pixels within aperture
                //qDebug() << QString("sdata: mT->P[18] = %1\n").arg(mT->P[18]);
                if(mT->P!=NULL) mT->sdata[7] = mT->P[8];//unut weight error of PSF fitting
                else mT->sdata[7] = 0.0;
                mT->sdata[8]=mT->mEkv[2];//UCAC2 Rmag
                /*sdata[9]=mT->resRec->J;//Jmag from 2MASS
                sdata[10]=mT->resRec->H;//Hmag from 2MASS
                sdata[11]=mT->resRec->Ks;//Ks_mag from 2MASS/
                if(mT->P!=NULL) mT->sdata[12] = mT->P[9];//standard error of pixel coordinate X
                else mT->sdata[12] = 0.0;
                if(mT->P!=NULL) mT->sdata[13] = mT->P[10];//standard error of pixel coordinate Y
                else mT->sdata[13] = 0.0;

                break;
            }
            case 1://usno-b1
            {
            //qDebug() << "sdata:" << mT->sdata << "\n";
                if(mT->sdata==NULL) mT->sdata = new double[15];
             //   qDebug() << "sdata:" << mT->sdata << "\n";
                //qDebug() << "mT->mEkv:" << mT->mEkv << "\n";
                mT->sdata[0]=mT->mEkv[0];//RA from UCAC2
                mT->sdata[1]=mT->mEkv[1];//DEC from UCAC2
                //qDebug() << QString("sdata: ra= %1\tde= %2\n").arg(mT->sdata[0]).arg(mT->sdata[1]);
                if(refMaker!=NULL)
                {
                    refMaker->forvRef(&ra, &de, mT->mEkv[0], mT->mEkv[1]);
                    getDegToTang(&tp[0], &tp[1], ra, de, oc0, oc1);
                }
                else
                {
                    getDegToTang(&tp[0], &tp[1], mT->mEkv[0], mT->mEkv[1], WCSdata[2], WCSdata[3]);
                }
                mT->sdata[2]=tp[0];//tangential position KSI based on catalogue RA
                mT->sdata[3]=tp[1];//tangential position ETA based on catalogue DE
                if(isComaCorr)
                {
                    cCorr = comaCorr(mT->sdata[2], mT->mEkv[2], comaKsi, comaMag0);
                    qDebug() << QString("ksi= %1\tcomaCorrKsi = %2\n").arg(mT->sdata[2]).arg(cCorr);
                    mT->sdata[2] -= cCorr;
                    cCorr = comaCorr(mT->sdata[3], mT->mEkv[2], comaEta, comaMag0);
                    qDebug() << QString("eta= %1\tcomaCorrEta = %2\n").arg(mT->sdata[3]).arg(cCorr);
                    mT->sdata[3] -= cCorr;
                }
                mT->sdata[4] = mT->mTanImg[0];// - WCSdata[0];//measured pixel position X (O)
                mT->sdata[5] = mT->mTanImg[1];// - WCSdata[1];//measured pixel position Y (1)

                qDebug() << QString("sdata[2]= %1\tsdata[3]= %2\n").arg(mT->sdata[2]).arg(mT->sdata[3]);
                qDebug() << QString("mTanImg[0]= %1\tmT->mTanImg[1]= %2\n").arg(mT->mTanImg[0]).arg(mT->mTanImg[1]);
                qDebug() << QString("sdata[4]= %1\tsdata[5]= %2\n").arg(mT->sdata[4]).arg(mT->sdata[5]);

                if(vfCorr0!=NULL)
                {
                    if(vfCorr0->isBC)
                    {
                        //if(!vectFcorr->int2Drad(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, vfRmax, nMin))
                        if(!vfCorr0->vectFcorr->int2D(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, &ni))
                        {
           //                 dDec = -0.5082077*resRec->de + 18.1648793;
                                //dDec = 0.0;
                                //resRec->Dx-= dKsi;
                                //resRec->Dy-= (dEta+dDec);

                                mT->sdata[4]-= dX;
                                mT->sdata[5]-= dY;//+dDec;
                        }
                    }
                    else
                    {
                        if(!vfCorr0->vectFcorr->int2Drad(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, vfCorr0->vfRmax, vfCorr0->nMin))
                        //if(!vfCorr0->vectFcorr->int2D(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, &ni))
                        {
           //                 dDec = -0.5082077*resRec->de + 18.1648793;
                                //dDec = 0.0;
                                //resRec->Dx-= dKsi;
                                //resRec->Dy-= (dEta+dDec);

                                mT->sdata[4]-= dX;
                                mT->sdata[5]-= dY;//+dDec;
                        }
                    }
                }


                if(isMagEqCorr)
                {

                    mT->sdata[4] -= magEqCorr(mT->mEkv[2], mT->sdata[4], mCoefX, mCoefDeg);
                    mT->sdata[5] -= magEqCorr(mT->mEkv[2], mT->sdata[5], mCoefY, mCoefDeg);
                }

                if(vfCorr1!=NULL)
                {
                    if(vfCorr1->isBC)
                    {
                        //if(!vectFcorr->int2Drad(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, vfRmax, nMin))
                        if(!vfCorr1->vectFcorr->int2D(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, &ni))
                        {
           //                 dDec = -0.5082077*resRec->de + 18.1648793;
                                //dDec = 0.0;
                                //resRec->Dx-= dKsi;
                                //resRec->Dy-= (dEta+dDec);

                                mT->sdata[4]-= dX;
                                mT->sdata[5]-= dY;//+dDec;
                        }
                    }
                    else
                    {
                        if(!vfCorr1->vectFcorr->int2Drad(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, vfCorr1->vfRmax, vfCorr1->nMin))
                        //if(!vfCorr0->vectFcorr->int2D(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, &ni))
                        {
           //                 dDec = -0.5082077*resRec->de + 18.1648793;
                                //dDec = 0.0;
                                //resRec->Dx-= dKsi;
                                //resRec->Dy-= (dEta+dDec);

                                mT->sdata[4]-= dX;
                                mT->sdata[5]-= dY;//+dDec;
                        }
                    }
                }

                dX = 0.0;
                dY = 0.0;
                dCoefX.clear();
                dCoefY.clear();
                if(mdcParam!=NULL)
                {
                    detCoefM(&dCoefX, mdcParam->mdCoefX, mdcParam->mdCoefXsz, mT->mEkv[2]);
                    detCoefM(&dCoefY, mdcParam->mdCoefY, mdcParam->mdCoefYsz, mT->mEkv[2]);
                    detDcorr(mdcParam->corrModel, mT->sdata[4], mT->sdata[5], &dX, &dY, dCoefX, dCoefY);

                    mT->sdata[4]-= dX;
                    mT->sdata[5]-= dY;
                }

                //qDebug() << "mT->P:" << mT->P << "\n";
                mT->sdata[6] =mT->mTanImg[2];// mT->P[18];//mean value of pixels within aperture
                //qDebug() << QString("sdata: mT->P[18] = %1\n").arg(mT->P[18]);
                if(mT->P!=NULL) mT->sdata[7] = mT->P[8];//unut weight error of PSF fitting
                else mT->sdata[7] = 0.0;
                mT->sdata[8] = mT->mEkv[2];//UCAC2 Rmag
                /*sdata[9]=mT->resRec->J;//Jmag from 2MASS
                sdata[10]=mT->resRec->H;//Hmag from 2MASS
                sdata[11]=mT->resRec->Ks;//Ks_mag from 2MASS/
                if(mT->P!=NULL) mT->sdata[12] = mT->P[9];//standard error of pixel coordinate X
                else mT->sdata[12] = 0.0;
                if(mT->P!=NULL) mT->sdata[13] = mT->P[10];//standard error of pixel coordinate Y

                else mT->sdata[13] = 0.0;
                break;
            }
            case 2://ucac3
            {
            /
            //qDebug() << "sdata:" << mT->sdata << "\n";
                if(mT->sdata==NULL) mT->sdata = new double[15];
             //   qDebug() << "sdata:" << mT->sdata << "\n";
                //qDebug() << "mT->mEkv:" << mT->mEkv << "\n";
                mT->sdata[0]=mT->mEkv[0];//RA from UCAC2
                mT->sdata[1]=mT->mEkv[1];//DEC from UCAC2
                //qDebug() << QString("sdata: ra= %1\tde= %2\n").arg(mT->sdata[0]).arg(mT->sdata[1]);
                if(refMaker!=NULL)
                {
                    refMaker->forvRef(&ra, &de, mT->mEkv[0], mT->mEkv[1]);
                    getDegToTang(&tp[0], &tp[1], ra, de, WCSdata[2], WCSdata[3]);
                }
                else
                {
                    getDegToTang(&tp[0], &tp[1], mT->mEkv[0], mT->mEkv[1], WCSdata[2], WCSdata[3]);
                }
                mT->sdata[2]=tp[0];//tangential position KSI based on catalogue RA
                mT->sdata[3]=tp[1];//tangential position ETA based on catalogue DE
                if(isComaCorr)
                {
                    cCorr = comaCorr(mT->sdata[2], mT->mEkv[2], comaKsi, comaMag0);
                    qDebug() << QString("ksi= %1\tcomaCorrKsi = %2\n").arg(mT->sdata[2]).arg(cCorr);
                    mT->sdata[2] -= cCorr;
                    cCorr = comaCorr(mT->sdata[3], mT->mEkv[2], comaEta, comaMag0);
                    qDebug() << QString("eta= %1\tcomaCorrEta = %2\n").arg(mT->sdata[3]).arg(cCorr);
                    mT->sdata[3] -= cCorr;
                }
                mT->sdata[4] = mT->mTanImg[0];// - WCSdata[0];//measured pixel position X (O)
                mT->sdata[5] = mT->mTanImg[1];// - WCSdata[1];//measured pixel position Y (1)

                qDebug() << QString("sdata[2]= %1\tsdata[3]= %2\n").arg(mT->sdata[2]).arg(mT->sdata[3]);
                qDebug() << QString("mTanImg[0]= %1\tmT->mTanImg[1]= %2\n").arg(mT->mTanImg[0]).arg(mT->mTanImg[1]);
                qDebug() << QString("sdata[4]= %1\tsdata[5]= %2\n").arg(mT->sdata[4]).arg(mT->sdata[5]);

                if(vfCorr0!=NULL)
                {
                    if(vfCorr0->isBC)
                    {
                        //if(!vectFcorr->int2Drad(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, vfRmax, nMin))
                        if(!vfCorr0->vectFcorr->int2D(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, &ni))
                        {
           //                 dDec = -0.5082077*resRec->de + 18.1648793;
                                //dDec = 0.0;
                                //resRec->Dx-= dKsi;
                                //resRec->Dy-= (dEta+dDec);

                                mT->sdata[4]-= dX;
                                mT->sdata[5]-= dY;//+dDec;
                        }
                    }
                    else
                    {
                        if(!vfCorr0->vectFcorr->int2Drad(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, vfCorr0->vfRmax, vfCorr0->nMin))
                        //if(!vfCorr0->vectFcorr->int2D(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, &ni))
                        {
           //                 dDec = -0.5082077*resRec->de + 18.1648793;
                                //dDec = 0.0;
                                //resRec->Dx-= dKsi;
                                //resRec->Dy-= (dEta+dDec);

                                mT->sdata[4]-= dX;
                                mT->sdata[5]-= dY;//+dDec;
                        }
                    }
                }


                if(isMagEqCorr)
                {

                    mT->sdata[4] -= magEqCorr(mT->mEkv[2], mT->sdata[4], mCoefX, mCoefDeg);
                    mT->sdata[5] -= magEqCorr(mT->mEkv[2], mT->sdata[5], mCoefY, mCoefDeg);
                }

                if(vfCorr1!=NULL)
                {
                    if(vfCorr1->isBC)
                    {
                        //if(!vectFcorr->int2Drad(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, vfRmax, nMin))
                        if(!vfCorr1->vectFcorr->int2D(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, &ni))
                        {
           //                 dDec = -0.5082077*resRec->de + 18.1648793;
                                //dDec = 0.0;
                                //resRec->Dx-= dKsi;
                                //resRec->Dy-= (dEta+dDec);

                                mT->sdata[4]-= dX;
                                mT->sdata[5]-= dY;//+dDec;
                        }
                    }
                    else
                    {
                        if(!vfCorr1->vectFcorr->int2Drad(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, vfCorr1->vfRmax, vfCorr1->nMin))
                        //if(!vfCorr0->vectFcorr->int2D(mT->sdata[4], mT->sdata[5], mT->mEkv[2], &dX, &dY, &ni))
                        {
           //                 dDec = -0.5082077*resRec->de + 18.1648793;
                                //dDec = 0.0;
                                //resRec->Dx-= dKsi;
                                //resRec->Dy-= (dEta+dDec);

                                mT->sdata[4]-= dX;
                                mT->sdata[5]-= dY;//+dDec;
                        }
                    }
                }

                dX = 0.0;
                dY = 0.0;
                dCoefX.clear();
                dCoefY.clear();
                if(mdcParam!=NULL)
                {
                    detCoefM(&dCoefX, mdcParam->mdCoefX, mdcParam->mdCoefXsz, mT->mEkv[2]);
                    detCoefM(&dCoefY, mdcParam->mdCoefY, mdcParam->mdCoefYsz, mT->mEkv[2]);
                    detDcorr(mdcParam->corrModel, mT->sdata[4], mT->sdata[5], &dX, &dY, dCoefX, dCoefY);

                    mT->sdata[4]-= dX;
                    mT->sdata[5]-= dY;
                }

                //qDebug() << "mT->P:" << mT->P << "\n";
                mT->sdata[6] =mT->mTanImg[2];// mT->P[18];//mean value of pixels within aperture
                //qDebug() << QString("sdata: mT->P[18] = %1\n").arg(mT->P[18]);
                if(mT->P!=NULL) mT->sdata[7] = mT->P[8];//unut weight error of PSF fitting
                else mT->sdata[7] = 0.0;
                mT->sdata[8] = mT->mEkv[2];//UCAC2 Rmag
                /*sdata[9]=mT->resRec->J;//Jmag from 2MASS
                sdata[10]=mT->resRec->H;//Hmag from 2MASS
                sdata[11]=mT->resRec->Ks;//Ks_mag from 2MASS/
                if(mT->P!=NULL) mT->sdata[12] = mT->P[9];//standard error of pixel coordinate X
                else mT->sdata[12] = 0.0;
                if(mT->P!=NULL) mT->sdata[13] = mT->P[10];//standard error of pixel coordinate Y
                else mT->sdata[13] = 0.0;
            /*    break;
            }
        }/
    }
    qDebug() << QString("refstars num: %1\n").arg(szRef);
}

*/

/*current
int fitsdata::ruler3(QString iniFile, QString resFolder)
{
        qDebug() << QString("\nruler3\n\n");
///////// 1. Reading settings ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //stream << "started....\n";
        //BEGIN settings
        QSettings *settings = new QSettings(iniFile, QSettings::IniFormat);

        reductionParams redParam;
        //[%general]
        int doBaseRed = settings->value("general/doBaseRed", 1).toInt();
        int doObjRed = settings->value("general/doObjRed", 1).toInt();
        QString outfolder = resFolder;
        qDebug() << QString("outfolder: %1\n").arg(outfolder);

        QString obscode;
        obscode = QString("%1").arg(obsPos->num, 3);//settings->value("general/obscode").toString();//MPC code of Observatory
        qDebug() << QString("obscode: %1\n").arg(obscode);

        //[psf]
        //int aperture = settings->value("psf/aperture").toInt();//aperture to get pixels values to PSF fitting
        /*int model = settings->value("psf/model").toInt();//PSF model: 0 - Lorentz PSF, 1 - Gauss PSF, 2 - Moffat PSF
        double delta = settings->value("psf/delta").toDouble();//initial power in Lorentz PSF or Moffat PSF
        int aperture = settings->value("psf/aperture").toInt();//aperture to get pixels values to PSF fitting
        int subgrad = settings->value("psf/subgrad").toInt();
        int ringradius = settings->value("psf/ringradius").toInt();
        int ringwidth = settings->value("psf/ringwidth").toInt();
        int lb = settings->value("psf/lb").toInt();/
        //[reductions]
        redParam.redType = settings->value("reductions/redType", 0).toInt();
        redParam.maxres = settings->value("reductions/maxres", 300).toDouble();//maximum of residuals for positional reduction in mas
        redParam.maxresMAG = settings->value("reductions/maxresMAG", 30).toDouble();//maximum of residuals for photometric reduction
        //redParam.minRefMag = settings->value("reductions/minrefMag").toDouble();//bottom limit for magnitude for reduction
        //redParam.maxRefMag = settings->value("reduc   tions/maxrefMag").toDouble();//upper limit for magnitude for reduction
//        int syscorr = settings->value("reductions/syscorr").toInt();//1 - taking systematic into account
//        double distfromplanet = settings->value("reductions/distfromplanet").toDouble();
//        double distfromsmear=settings->value("reductions/distfromsmear").toDouble();
        redParam.sigma=settings->value("reductions/sigma", 3.0).toDouble();
        redParam.sMax = settings->value("reductions/sMax", 500).toDouble();
        redParam.minRefStars = settings->value("reductions/minRefStars", 5).toDouble();
//        double planetKsi,planetEta;
        redParam.weights = settings->value("reductions/weights", 0).toInt();
        redParam.uweMax = settings->value("reductions/uweMax", 500).toDouble();
        redParam.uweReduceMinStars = settings->value("reductions/uweReduceMinStars", redParam.minRefStars).toInt();


        qDebug() << QString("\nredParam.redType: %1\n").arg(redParam.redType);
        qDebug() << QString("redParam.maxres: %1\n").arg(redParam.maxres);
        qDebug() << QString("redParam.maxresMAG: %1\n").arg(redParam.maxresMAG);
        //qDebug() << QString("redParam.minRefMag: %1\n").arg(redParam.minRefMag);
        //qDebug() << QString("redParam.maxRefMag: %1\n").arg(redParam.maxRefMag);
        qDebug() << QString("redParam.sigma: %1\n").arg(redParam.sigma);
        qDebug() << QString("redParam.sMax: %1\n").arg(redParam.sMax);
        qDebug() << QString("redParam.minRefStars: %1\n").arg(redParam.minRefStars);
        qDebug() << QString("redParam.weights: %1\n").arg(redParam.weights);
        qDebug() << QString("redParam.uweMax: %1\n").arg(redParam.uweMax);


        //[outputlimits]
        double maxOCMAG = settings->value("outputlimits/maxOCMAG", 30).toDouble();//maximum of (O-C) in magnitude for output
        double maxOCRA = settings->value("outputlimits/maxOCRA", 10000).toDouble();//maximum of (O-C) in RA for output
        double maxOCDE = settings->value("outputlimits/maxOCDE", 10000).toDouble();//maximum of (O-C) in DEC for output
        double maxOCMAGrc = settings->value("outputlimits/maxOCMAGrc", 30).toDouble();//maximum of residuals in magnitude for output
        double maxOCRArc = settings->value("outputlimits/maxOCRArc", 10000).toDouble();//maximum of residuals in RA for output
        double maxOCDErc = settings->value("outputlimits/maxOCDErc", 10000).toDouble();//maximum of residuals in DEC for output
        int resSigma = settings->value("outputlimits/resSigma", -1).toDouble();//maximum of residuals in DEC for output
        //[whatoutput] all settings within this section must be equal to 1 if you want to use them
        int mpcreport=settings->value("whatoutput/mpcreport").toInt();//make MPC report
        int residuals=settings->value("whatoutput/residuals").toInt();//make RESIDUALS report
//        int ocreject=settings->value("whatoutput/ocreject").toInt();//if ocreject=0 then results with BAD (O-C) (see maxOCMAG, maxOCRA and maxOCDE) will be inluded to output
        int astreport=settings->value("whatoutput/astreport").toInt();//make ASTROREPORT
        int sumreport=settings->value("whatoutput/sumreport").toInt();//make ASTROREPORT
        int errorreport=settings->value("whatoutput/errorreport").toInt();//make ERRORREPORT
        int objresreport=settings->value("whatoutput/objresreport", 1).toInt();//make ERRORREPORT
//        int relativepos=settings->value("whatoutput/relativepos").toInt();//relative positions of the satellites
        //[outputforms]
 //       int time_form = settings->value("outputforms/time_form").toInt();//set form for output moments of time: 0 - YYYY MM DD.DDDD, 1 - MJD, 2 - (MJD - MJDo) in minutes
  //      int pos_form = settings->value("outputforms/pos_form").toInt();//set form for positional data: 0 - (h,m,s; deg, arcmin, arcsec); 1 - mas; 2 - degrees.
  //      int phot_form = settings->value("outputforms/phot_form").toInt();//set form for photometric data
        QString sepsymb = settings->value("outputforms/sepsymb").toString();//symbol for separation between records
        QString mpcObsType = settings->value("outputParams/mpcObsType").toString();
        if (sepsymb=="space") sepsymb=" ";
   //     double mjd0 = settings->value("outputforms/mjd0").toDouble();//zero point of MJD to output (MJDo). See time_form
        //[catalogue]
  //      QString cat = settings->value("catalogue/refcatalogue").toString();// reference catalogue name (ucac2 or 2mass)
 //       redParam.catindex = refMarks->ctype;// more useful integer value to indicate reference catalogue
//        if(cat=="ucac2")catindex=0;
//        if(cat=="2mass")catindex=1;
  //      QString fov = settings->value("catalogue/fov").toString();// field of view size in arcmin (radius of selection)
   //     QString minMag = settings->value("catalogue/minMag").toString();//bottom limit for magnitude
   //     QString maxMag = settings->value("catalogue/maxMag").toString();//upper limit for magnitude
        //[programs]
   //     QString ast_eph_prog = settings->value("programs/ast_eph_prog").toString();//soft to find asteroids
   //     QString ast_eph_prog_folder = settings->value("programs/ast_eph_prog_folder").toString();//working folder of soft to find asteroids
   //     QString ucac2findn = settings->value("programs/ucac2findn").toString();//soft to find ucac2 data
    //    QString ucac2find_folder = settings->value("programs/ucac2find_folder").toString();//working folder for soft to find ucac2 data
    //    QString cdsfindn = settings->value("programs/cdsfindn").toString();//soft to find 2mass data
    //    QString cdsfind_folder = settings->value("programs/cdsfind_folder").toString();//working folder for soft to find 2mass data
   //     QString sateph_prog = settings->value("programs/sateph_prog").toString();//soft to find satellites
    //    QString sateph_prog_folder = settings->value("programs/sateph_prog_folder").toString();//working folder of soft to find satellites
        //marks
   //     QString mSep = settings->value("marks/marksFileSep").toString();
   //     QString mCol = settings->value("marks/marksLoadFileColumn").toString();
   //     int isMove2corner = settings->value("marks/isMove2corner").toInt();
        //refStarSelector
        QStringList baseSelChain, objSelChain;
        baseSelChain = settings->value("refStarSelector/baseSelChain", "").toString().split(",");
        objSelChain = settings->value("refStarSelector/objSelChain", "").toString().split(",");
        qDebug() << QString("baseChain: %1\n").arg(baseSelChain.join("|"));
        qDebug() << QString("objChain: %1\n").arg(objSelChain.join("|"));

        rsSelector0Sett sel0;
        sel0.minMag = settings->value("rsSelector0/minMag", 0).toDouble();
        sel0.maxMag = settings->value("rsSelector0/maxMag", 16).toDouble();
        sel0.minStarsNum = settings->value("rsSelector0/minStarsNum", 0).toInt();

        rsSelector1Sett sel1;
        sel1.cDist = settings->value("rsSelector1/cDist", 0).toDouble();
        sel1.minStarsNum = settings->value("rsSelector1/minStarsNum", 0).toInt();

        rsSelector2Sett sel2;
        sel2.aper = settings->value("rsSelector2/aper", 0).toDouble();
        sel2.minStarsNum = settings->value("rsSelector2/minStarsNum", 0).toInt();

        rsSelector3Sett sel3;
        sel3.magDiap = settings->value("rsSelector3/magDiap", 0).toDouble();
        sel3.minStarsNum = settings->value("rsSelector3/minStarsNum", 0).toInt();

        rsSelector4Sett sel4;
        sel4.muMax = settings->value("rsSelector4/muMax", 0).toDouble();
        sel4.minStarsNum = settings->value("rsSelector4/minStarsNum", 0).toInt();

        rsSelector5Sett sel5;
        sel5.dMax = settings->value("rsSelector5/dMax", 10000).toDouble();
        sel5.minStarsNum = settings->value("rsSelector5/minStarsNum", 0).toInt();
        sel5.maxStarsNum = settings->value("rsSelector5/maxStarsNum", 100).toInt();

        rsSelector6Sett sel6;
        sel6.targUs1 = settings->value("rsSelector6/targUs1", 0).toInt();
        sel6.minStarsNum = settings->value("rsSelector6/minStarsNum", 0).toInt();



//END settings

    double ra_c,de_c;
    double *sdata, *objdata;
    QVector<int> rsindex;
    QString mTimeCode;
    QString outstr;
    double x,y,pixmag,ksi,eta;
    double xc,yc,pmag, mag;
    double *cp;
    int resNum;
    DATEOBS dateObs;
    cp = new double[2];

    QTime cT;
    marksP *mObj;
    int ko;

    residualFile resFile;

    int szRef, szObj;
    double ra0, de0, ra1, de1, x0, y0;
    int selNum;
   // double mjd;
   // double planetKsi, planetEta;

    int i, j;
 //   params->catindex = 0;
  //  int objType = 0;

//    if(!WCSdata[12]) return 1;

    QString msgstr;
 //   mjd = MJD;

    ra_c = WCSdata[2];//coordinates of the optical center
    de_c = WCSdata[3];//deg

    //errBudgetFile ebFile;
    //errBudgetRec* ebRec;
    //if(refMarks->ebRec==NULL) refMarks->ebRec = new errBudgetRec;

double *mEkv;
double OC0, OC1;

 //   refMarks->detOCmean(&ra_c, &de_c);


QList <double*> meList;
qDebug() << "refParam: " << refParam << "\n";

    if(refParam!=NULL)
    {
        refMaker = new refractionMaker(*refParam);
        refMaker->forvRef(&WCSdata[2], &WCSdata[3], ra_c, de_c);

    }




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



        prerObjects();
//END read objects positions
/////////4. Reading data from reference catalogue ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BEGIN load data from catalogue
        prepRefStars();
        if(refMarks->marks.size()<redParam.minRefStars)
        {
            qDebug() << QString("ref stars is not enough: %1\tit is necessary: %2\n").arg(refMarks->marks.size()).arg(redParam.minRefStars);
        }
        //END load data from catalogue
/////////5. Reduction procedures ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //BEGIN reduction

        refMarks->ebRec = new errBudgetRec;
        qDebug() << QString("redMake: %1\n").arg(redParam.redType);
        reductionMaker *redMake = new reductionMaker(redParam.redType);

        //detNaxes(&naxes[0], &naxes[1]);


//default red/////////////////////////////////
        if(doBaseRed)
        {
            refMarks->ebRec->clear();
            szRef = refMarks->marks.size();

            refMarks->ebRec->MJD = MJD;
            refMarks->ebRec->RAoc = ra_c;
            refMarks->ebRec->DEoc = de_c;
            refMarks->ebRec->originName = fileName;
            rsindex.clear();

            for(i=0; i<szRef; i++)
            {
                rsindex << i;
            }
            qDebug() << QString("rsindex Before= %1\n").arg(rsindex.count());

            selNum = baseSelChain.size();
            for(i=0; i<selNum; i++)
            {
                switch(baseSelChain.at(i).toInt())
                {
                case 0:
                    refStarSelector0(refMarks, rsindex, sel0);
                    break;
                case 1:
                    refStarSelector1(refMarks, rsindex, sel1.cDist, workFrame, WCSdata[0], WCSdata[1]);
                    break;
                case 4:
                    refStarSelector4(refMarks, rsindex, sel4.muMax, sel4.minStarsNum);
                    break;
                case 6:
                    refStarSelector6(refMarks, rsindex, sel6.targUs1, sel6.minStarsNum);
                    break;

                }
            }

            qDebug() << QString("rsindex after All refStarSelectors = %1\n").arg(rsindex.count());

            if(rsindex.count()>=redParam.minRefStars)
            {

                //break;


                qDebug() << "refParam: " << refParam << "\n";
                redMake->makeReduction(mTimeCode, refMarks, rsindex, redParam);
                if((redMake->RNKSI<redParam.minRefStars)||(redMake->RNETA<redParam.minRefStars))
                {
                    qDebug() << QString("\nWARN: reduction stars num is too small\nrnksi: %1\trneta: %2\n").arg(redMake->RNKSI).arg(redMake->RNETA);
                    //return 1;
                }
                if((grad_to_mas(sqrt(redMake->UWEKSI))>redParam.uweMax)||((grad_to_mas(sqrt(redMake->UWEETA))>redParam.uweMax)))
                {
                    qDebug() << QString("\nWARN: uwe is too hi\nUWEksi: %1\tUWEeta: %2\n").arg(grad_to_mas(sqrt(redMake->UWEKSI))).arg(grad_to_mas(sqrt(redMake->UWEETA)));
                    //return 1;
                }
                //if((redMake->RNKSI>=redParam.minRefStars)&&(redMake->RNETA>=redParam.minRefStars))
                //{
                    //getTimeCode(mTimeCode);
                    refMarks->ebRec->mesureTimeCode = mTimeCode;
                    qDebug() << QString("default red mTimeCode: %1").arg(mTimeCode);
                    qDebug() << "redMake->ZKSI: " << redMake->ZKSI << "\n";
                    qDebug() << QString("RN: %1\t%2\t%3\n").arg(redMake->RNKSI).arg(redMake->RNETA).arg(redMake->RNMAG);

                    redMake->makeErrorBudget(refMarks->ebRec);

                    qDebug() << QString("errorreport: %1\n").arg(errorreport);
                    if(errorreport)
                    {
                        outstr.clear();
                        refMarks->ebRec->rec2s(&outstr);

                        QFile errFile(outfolder+"/err_budget.txt");
                        errFile.open(QIODevice::Append| QIODevice::Text);
                        QTextStream errStream;
                        errStream.setDevice(&errFile);
                        errStream << outstr << "\n";
                        //errStream << "\n";
                        errFile.close();
                    }

                    QStringList outsl;
                    qDebug() << QString("residuals: %1\n").arg(residuals);
                    if(residuals)
                    {
                        outsl.clear();
                        residualsRec* resRec;
                        outstr.clear();


                        qDebug() << QString("rsindex.count= %1\n").arg(rsindex.count());

                        resFile.resList.clear();
                        /*QFile resFile(outfolder+"/residuals.txt");
                        resFile.open(QIODevice::Append| QIODevice::Text);
                        QTextStream resStream;
                        resStream.setDevice(&resFile);/
                        //int resNum = refMarks->marks.size();
                        resNum = refMarks->marks.size();
                        for (i=0;i<resNum;i++)
                        {
                            //outstr.clear();
                            //j = rsindex.at(i);
                            sdata = refMarks->marks.at(i)->sdata;
                            refMarks->marks.at(i)->resRec = new residualsRec;
                            resRec = refMarks->marks.at(i)->resRec;
                                //sdata = refstars.at(i);
                                x = sdata[4];
                                y = sdata[5];
                                pixmag = sdata[6];
                                redMake->detTan(&ksi, &eta, x, y);

                                redMake->detMagn(&mag, pixmag);
                                //if((fabs(mag - sdata[8])<=maxOCMAGrc)&&(grad_to_mas(fabs(ksi - sdata[2]))<=maxOCRArc)&&(grad_to_mas(fabs(eta - sdata[3]))<=maxOCDErc))
                                if((grad_to_mas(fabs(ksi - sdata[2]))<=maxOCRArc)&&(grad_to_mas(fabs(eta - sdata[3]))<=maxOCDErc))
                                {
                                        redMake->detXcYc(&xc, &yc, sdata[2], sdata[3]);
                                        //pmag = (sdata[8]-bm)/am;
                                        resRec->mJD = MJD;
                                        resRec->ra = sdata[0];
                                        resRec->de = sdata[1];
                                        resRec->ksi = ksi;
                                        resRec->eta = eta;
                                        resRec->mag = mag;
                                        resRec->ksiOC = grad_to_mas(ksi - sdata[2]);
                                        resRec->etaOC = grad_to_mas(eta - sdata[3]);
                                        resRec->magOC = mag - sdata[8];
                                        resRec->x = x;
                                        resRec->y = y;
                                        resRec->pixmag = pixmag;
                                        resRec->Dx = (x - xc);
                                        resRec->Dy = (y - yc);
                                        redMake->detPmag(&pmag, sdata[8]);
                                        resRec->Dpixmag = pixmag - pmag;

                                        resRec->isRefKsi = 0;
                                        resRec->isRefEta = 0;
                                        resRec->isRefMag = 0;
                                        for(j=0;j<rsindex.count();j++)
                                        {
                                            if(i==rsindex[j])
                                            {
                                                resRec->isRefKsi = (int)(redMake->EXCLINDKSI[j]>=0);
                                                resRec->isRefEta = (int)(redMake->EXCLINDETA[j]>=0);
                                                resRec->isRefMag = (int)(redMake->EXCLINDMAG[j]>=0);
                                            }
                                        }

                                        resRec->mesureTimeCode = mTimeCode;
                                        resRec->catName = refMarks->marks.at(i)->catName;
                                        resRec->catMagName = refMarks->marks.at(i)->catMagName;
                                        resRec->setMesParams(refMarks->marks.at(i)->P, 21);
                                        resRec->rec2s(&outstr);
                                        //outstr += "\n";

                                        resFile.resList << resRec;
                                        //resStream << outstr << "\n";
                                        //resStream ;
                                        //resStream << "\n";
                                }
                        }

                        qDebug() << QString("resList.size= %1\n").arg(resFile.resList.size());


                        if(resSigma>0.0) resFile.remSigma(resSigma);
                        resFile.saveAs(outfolder+"/residuals.txt");
            //                        resStream << outsl.join("\n");
                        //resFile.close();
                    }
                }
            else qDebug() << QString("\nNumber of reference stars (%1) less than minRefStarNum (%2). No reduction.\n").arg(rsindex.count()).arg(redParam.minRefStars);
            //}
            //else qDebug() << QString("too small RN\n");

            //QTimer tsl;//sleep(1);

            //tsl.start();
        }



        if(doObjRed)
        {

///////////////////////////////////////////////
            szObj = objMarks->marks.size();


            QFile oresFile;
            QTextStream orStm;

            objResRec orRec;


            qDebug() << QString("szObj= %1\n").arg(szObj);

            for(ko=szObj-1; ko>=0; ko--)
            {
                cT.restart();
                for(;cT.elapsed() < 1;);


                qDebug() << QString("ko= %1\n").arg(ko);
                mObj = objMarks->marks.at(ko);
                refMarks->ebRec->clear();
                szRef = refMarks->marks.size();
                refMarks->ebRec->MJD = MJD;
                refMarks->ebRec->RAoc = ra_c;
                refMarks->ebRec->DEoc = de_c;
                refMarks->ebRec->originName = fileName;

                rsindex.clear();
                for(i=0; i<szRef; i++) rsindex << i;

                selNum = objSelChain.size();
                for(i=0; i<selNum; i++)
                {
                    switch(objSelChain.at(i).toInt())
                    {
                    case 0:
                        refStarSelector0(refMarks, rsindex, sel0);
                        break;
                    case 1:
                        refStarSelector1(refMarks, rsindex, sel1.cDist, workFrame, WCSdata[0], WCSdata[1]);
                        break;
                    case 2:
                        refStarSelector2(refMarks, rsindex, mObj, sel2.aper);
                        break;
                    case 3:
                        refStarSelector3(refMarks, rsindex, mObj->mEkv[2], sel3.magDiap, sel3.minStarsNum);
                        break;
                    case 4:
                        refStarSelector4(refMarks, rsindex, sel4.muMax, sel4.minStarsNum);
                        break;
                    case 5:
                        refStarSelector5(refMarks, rsindex, mObj, sel5.dMax, sel5.minStarsNum, sel5.maxStarsNum);
                        break;
                    case 6:
                        refStarSelector6(refMarks, rsindex, sel6.targUs1, sel6.minStarsNum);
                        break;

                    }
                }

                if(rsindex.count()<redParam.minRefStars)
                {
                    qDebug() << QString("\nNumber of reference stars (%1) is less than minRefStarNum (%2). No reduction.\n").arg(rsindex.count()).arg(redParam.minRefStars);
                    continue;
                }

    /////////////////////////////////////////////////////////

                redMake->makeReduction(mTimeCode, refMarks, rsindex, redParam, refParam);

    /////////////////////////////////////////////////////////

                if((redMake->RNKSI<redParam.minRefStars)||(redMake->RNETA<redParam.minRefStars))
                {
                    qDebug() << QString("\nWARN: reduction stars num is too small\nrnksi: %1\trneta: %2\n").arg(redMake->RNKSI).arg(redMake->RNETA);
                    //continue;
                }
                if((grad_to_mas(sqrt(redMake->UWEKSI))>redParam.uweMax)||((grad_to_mas(sqrt(redMake->UWEETA))>redParam.uweMax)))
                {
                    qDebug() << QString("\nWARN: uwe is too hi\nUWEksi: %1\tUWEeta: %2\n").arg(grad_to_mas(sqrt(redMake->UWEKSI))).arg(grad_to_mas(sqrt(redMake->UWEETA)));
                    //continue;
                }

                //if((redMake->RNKSI>=redParam.minRefStars)&&(redMake->RNETA>=redParam.minRefStars))
                //{

                    //getTimeCode(mTimeCode);
                    refMarks->ebRec->mesureTimeCode = mTimeCode;
                    qDebug() << QString("mTimeCode: %1").arg(mTimeCode);
                    redMake->makeErrorBudget(refMarks->ebRec);

                    refMarks->ebRec->exptime = exptime;

    /////////////////////////////////////////////////////////

                    objdata = mObj->objdata;

                    x = objdata[4];
                    y = objdata[5];
                    pixmag = objdata[6];
                    redMake->detTan(&ksi, &eta, x, y);

                   getTangToDeg(&cp[0], &cp[1], ksi, eta, ra_c, de_c);

                    redMake->detMagn(&mag, pixmag);

                    qDebug() << QString("obj: ksi= %1\tobjdata[2]= %2\t%3 <? %4\n").arg(ksi).arg(objdata[2]).arg(grad_to_mas(fabs(ksi - objdata[2]))).arg(maxOCRA);
                    qDebug() << QString("obj: eta= %1\tobjdata[3]= %2\t%3 <? %4\n").arg(eta).arg(objdata[3]).arg(grad_to_mas(fabs(eta - objdata[3]))).arg(maxOCDE);
                    qDebug() << QString("obj: mag= %1\tobjdata[8]= %2\n").arg(mag).arg(objdata[8]);

                    //if(!((fabs(mag-objdata[8])<=maxOCMAG)&&(grad_to_mas(fabs(ksi-objdata[2]))<=maxOCRA)&&                        (grad_to_mas(fabs(eta-objdata[3]))<=maxOCDE)))
                    if(!((grad_to_mas(fabs(ksi-objdata[2]))<=maxOCRA)&&(grad_to_mas(fabs(eta-objdata[3]))<=maxOCDE)))
                    {
                        qDebug() << QString("objError too big\n");
                        objMarks->remMark(ko);
                        qDebug() << QString("continue\n");
                        continue;
                    }

//////////////////////

                    //oresFile.clear();
                    //orRec = new objResRec;
                    orRec.catName = mObj->catName;
                    orRec.catMagName = mObj->catMagName;
                    orRec.mJD = MJD;
                    orRec.ksi = ksi;
                    orRec.eta = eta;
                    orRec.ksiOC = (ksi - objdata[2]);
                    orRec.etaOC = (eta - objdata[3]);
                    orRec.ra = objdata[0] + orRec.ksiOC*cos(grad_to_rad(objdata[1]));
                    orRec.de = objdata[1] + orRec.etaOC;
                    orRec.mag = mag;

                    orRec.ksiOC = grad_to_mas(orRec.ksiOC);
                    orRec.etaOC = grad_to_mas(orRec.etaOC);
                    orRec.magOC = mag - objdata[8];
                    orRec.x = x;
                    orRec.y = y;
                    orRec.pixmag = pixmag;

                    redMake->detXcYc(&xc, &yc, objdata[2], objdata[3]);
                    orRec.Dx = (x - xc);
                    orRec.Dy = (y - yc);
                    redMake->detPmag(&pmag, objdata[8]);
                    orRec.Dpixmag = pixmag - pmag;

                    orRec.mesureTimeCode = mTimeCode;

                    if(mObj->P!=NULL) orRec.setMesParams(mObj->P, 21);

//////////////////////

                    qDebug() << QString("objType: %1\n").arg(mObj->objType);



                    switch(mObj->objType)
                    {
                    case OBJ_TYPE_UCAC3:
                        break;
                    case OBJ_TYPE_MPEPH:
                        {
                            mObj->ocObj = new ocRec;
                            ocRec *oc = mObj->ocObj;
                            orRec.name = mObj->mpcObj->name;

                            orRec.toOcRec(oc);

                            oc->expTime = exptime;
                 /*           //oc->name = mObj->mpcObj->name;





                        //outdata[20] = ra_c;//RA optical center
                        //outdata[21] = de_c;//DEC optical center
                        //
                            oc->MJday = MJD;//MJD

                            oc->mag0 = mag;//MAG(O)
                            oc->ocRaCosDe = (ksi - objdata[2])*cos(grad_to_rad(objdata[1]));//O-C in RA * cosDe
                            oc->ocDe = (eta - objdata[3]);//O-C in DEC
                            oc->ra = objdata[0] + oc->ocRaCosDe;//RA(O)
                            oc->de = objdata[1] + oc->ocDe;//DEC(O)
                            oc->ocRaCosDe = grad_to_mas(oc->ocRaCosDe);
                            oc->ocDe = grad_to_mas(oc->ocDe);
                            oc->ocMag = mag - objdata[8];//O-C in MAG

                            oc->x = x;
                            oc->y = y;
                            oc->pixmag = pixmag;

                            redMake->detXcYc(&xc, &yc, objdata[2], objdata[3]);
                            oc->Dx = (x - xc);
                            oc->Dy = (y - yc);
                            redMake->detPmag(&pmag, objdata[8]);
                            oc->Dpixmag = pixmag - pmag;
/

                            oc->topDist = mObj->mpcObj->topDist;//topocentric distance from theory of motion or catalogue
                            oc->muRaCosDe = mObj->mpcObj->muRaCosDe;//mu_ra*cos(dec) [mas/min]
                            oc->muDe = mObj->mpcObj->muDe;//mu_de [mas/min]
                            oc->Vr = mObj->mpcObj->Vr;//v_r [km/s]
                            oc->phase = mObj->mpcObj->phase;//phase [deg]
                            oc->elong = mObj->mpcObj->elong;//elongation [deg]
                            qDebug() << QString("mTimeCode: %1\n").arg(mTimeCode);
                           /* oc->mesureTimeCode = mTimeCode;
                            qDebug() << QString("mObj->catName: %1\n").arg(mObj->catName);
                            oc->catName = mObj->catName;
                            qDebug() << QString("mObj->catMagName: %1\n").arg(mObj->catMagName);
                            oc->catMagName = mObj->catMagName;
                            /

                            qDebug() << QString("astreport: %1\n").arg(astreport);

                            if(astreport==1)
                            {
                                oc->rec2s(&outstr);
                                qDebug() << QString("astFile: %1\n").arg(outfolder+"/"+mObj->mpcObj->name+"_eq.txt");
                                QFile astFile(outfolder+"/"+mObj->mpcObj->name+"_eq.txt");
                                astFile.open(QIODevice::Append| QIODevice::Text);
                                QTextStream astStream;
                                astStream.setDevice(&astFile);
                                astStream << outstr << "\n";
                                astFile.close();

                                if(sumreport==1)
                                {
                                    qDebug() << QString("sumAstFile: %1\n").arg(outfolder+"/"+"aggregate_eq.txt");
                                    QFile astFile(outfolder+"/"+"aggregate_eq.txt");
                                    astFile.open(QIODevice::Append| QIODevice::Text);
                                    QTextStream astStream;
                                    astStream.setDevice(&astFile);
                                    astStream << outstr << "\n";
                                    astFile.close();
                                }
                            }

                            qDebug() << QString("mpcreport: %1\n").arg(mpcreport);
                            if(mpcreport==1)
                            {
                                outstr ="";
                                //int obj_num;
                                oc->rec2MPC(&outstr, obscode, mObj->mpcObj->num.toInt(), mpcObsType);
/*
                                dateObs = getDATEOBSfromMJD(oc->MJday);
                                obj_num = oc->name.toInt();
                                outstr = outstr +QString( "%1" ).arg( mObj->mpcObj->num.toInt(),5,10,QLatin1Char( '0' ))+"         " + mpcObsType;
                                outstr = outstr +getStrFromDATEOBS(dateObs," ", 0, 5) +" ";
                                outstr = outstr +deg_to_hms(oc->ra, " ",2)+" ";
                                outstr = outstr +deg_to_damas(oc->de, " ",1)+"          ";
                                outstr = outstr  +QString( "%1" ).arg(oc->mag0,5,'f',2);
                                outstr = outstr  +"       "+obscode;//obscode;/
                                msgstr = QString( "%1" ).arg(mObj->mpcObj->name);
                                msgstr = msgstr.trimmed();
                                QFile mpcFile(outfolder+"/"+msgstr+"_mpc.txt");
                                mpcFile.open(QIODevice::Append| QIODevice::Text);
                                QTextStream mpcStream;
                                mpcStream.setDevice(&mpcFile);
                                mpcStream << outstr << "\n";
                                mpcFile.close();

                                if(sumreport==1)
                                {
                                    qDebug() << QString("sumMpcFile: %1\n").arg(outfolder+"/"+"aggregate_mpc.txt");
                                    QFile astFile(outfolder+"/"+"aggregate_mpc.txt");
                                    astFile.open(QIODevice::Append| QIODevice::Text);
                                    QTextStream astStream;
                                    astStream.setDevice(&astFile);
                                    astStream << outstr << "\n";
                                    astFile.close();
                                }
                            }

                            break;
                        }
                        case OBJ_TYPE_SSTAR:
                        {
                            mObj->ssResRec = new sstarRes;

                            orRec.name = QString(mObj->sstar->getLSPM()).simplified();

                            orRec.toSstarRes(mObj->ssResRec);

                            /*
                            mObj->ssResRec->mJD = MJD;

                            mObj->ssResRec->mag = mag;

                            //resRec->ra = sdata[0];
                            //resRec->de = sdata[1];
                            mObj->ssResRec->ksi = ksi;
                            mObj->ssResRec->eta = eta;
                            //qDebug() << QString("ksi= %1\teta= %2\n").arg(mObj->ssResRec->ksiOC).arg(mObj->ssResRec->etaOC);
                            mObj->ssResRec->ksiOC = (ksi - objdata[2]);
                            mObj->ssResRec->etaOC = (eta - objdata[3]);
                            //qDebug() << QString("ksiOC= %1\tetaOC= %2\n").arg(mObj->ssResRec->ksiOC).arg(mObj->ssResRec->etaOC);

                            mObj->ssResRec->ra = objdata[0] + mObj->ssResRec->ksiOC*cos(grad_to_rad(objdata[1]));
                            mObj->ssResRec->de = objdata[1] + mObj->ssResRec->etaOC;
                            mObj->ssResRec->ksiOC = grad_to_mas(mObj->ssResRec->ksiOC);
                            mObj->ssResRec->etaOC = grad_to_mas(mObj->ssResRec->etaOC);
                            mObj->ssResRec->magOC = mag - objdata[8];
                            mObj->ssResRec->x = x;
                            mObj->ssResRec->y = y;
                            mObj->ssResRec->pixmag = pixmag;

                            redMake->detXcYc(&xc, &yc, objdata[2], objdata[3]);
                            mObj->ssResRec->Dx = (x - xc);
                            mObj->ssResRec->Dy = (y - yc);
                            redMake->detPmag(&pmag, objdata[8]);
                            mObj->ssResRec->Dpixmag = pixmag - pmag;
                            mObj->ssResRec->lspmName = QString(mObj->sstar->getLSPM()).simplified();
                            mObj->ssResRec->mesureTimeCode = mTimeCode;
                            mObj->ssResRec->catName = mObj->catName;
                            mObj->ssResRec->catMagName = mObj->catMagName;/

                            mObj->ssResRec->exptime = exptime;

                            if(astreport==1)
                            {
                                mObj->ssResRec->rec2s(&outstr);

                                QFile astFile(outfolder+"/"+mObj->ssResRec->lspmName+"_sstar.txt");
                                astFile.open(QIODevice::Append| QIODevice::Text);
                                QTextStream astStream;
                                astStream.setDevice(&astFile);
                                astStream << outstr << "\n";
                                astFile.close();

                                if(sumreport==1)
                                {
                                    QFile astFile(outfolder+"/aggregate_sstar.txt");
                                    astFile.open(QIODevice::Append| QIODevice::Text);
                                    QTextStream astStream;
                                    astStream.setDevice(&astFile);
                                    astStream << outstr << "\n";
                                    astFile.close();
                                }
                            }

                            break;
                        }
                    case OBJ_TYPE_SKYBOT:
                        {
                            mObj->ocObj = new ocRec;
                            ocRec *oc = mObj->ocObj;

                            orRec.name =  mObj->sbot->Name;

                            orRec.toOcRec(oc);

                            oc->expTime = exptime;

                            /*oc->name = mObj->sbot->Name;

                            oc->MJday = MJD;//MJD
                            //oc->ra = cp[0];//RA(O)
                            //oc->de = cp[1];//DEC(O)
                            oc->mag0 = mag;//MAG(O)
                            oc->ocRaCosDe = (ksi - objdata[2])*cos(grad_to_rad(objdata[1]));//O-C in RA
                            oc->ocDe = (eta - objdata[3]);//O-C in DEC
                            oc->ra = objdata[0] + oc->ocRaCosDe;//RA(O)
                            oc->de = objdata[1] + oc->ocDe;//DEC(O)
                            oc->ocRaCosDe = grad_to_mas(oc->ocRaCosDe);
                            oc->ocDe = grad_to_mas(oc->ocDe);

                            oc->ocMag = mag - objdata[8];//O-C in MAG

                            oc->x = x;
                            oc->y = y;
                            oc->pixmag = pixmag;
                            redMake->detXcYc(&xc, &yc, objdata[2], objdata[3]);
                            oc->Dx = (x - xc);
                            oc->Dy = (y - yc);
                            redMake->detPmag(&pmag, objdata[8]);
                            oc->Dpixmag = pixmag - pmag;
/
                            oc->topDist = mObj->sbot->Dg;//topocentric distance from theory of motion or catalogue
                            oc->muRaCosDe = objdata[10];//mObj->sbot->dRA*1000.0/60.0;//mu_ra*cos(dec) [mas/min]
                            oc->muDe = objdata[11];//mObj->sbot->dDEC*1000.0/60.0;//mu_de [mas/min]
                            oc->Vr = 0.0;//v_r [km/s]
                            oc->phase = mObj->sbot->Phase;//phase [deg]
                            oc->elong = mObj->sbot->SunElong;//elongation [deg]
                            qDebug() << QString("mTimeCode: %1\n").arg(mTimeCode);
                            /*
                            oc->mesureTimeCode = mTimeCode;
                            qDebug() << QString("mObj->catName: %1\n").arg(mObj->catName);
                            oc->catName = mObj->catName;
                            qDebug() << QString("mObj->catMagName: %1\n").arg(mObj->catMagName);
                            oc->catMagName = mObj->catMagName;
/
                            qDebug() << QString("astreport: %1\n").arg(astreport);

                            if(astreport==1)
                            {
                                oc->rec2s(&outstr);
                                qDebug() << QString("astFile: %1\n").arg(outfolder+"/"+mObj->sbot->Name+"_eq.txt");
                                QFile astFile(outfolder+"/"+mObj->sbot->Name+"_eq.txt");
                                astFile.open(QIODevice::Append| QIODevice::Text);
                                QTextStream astStream;
                                astStream.setDevice(&astFile);
                                astStream << outstr << "\n";
                                astFile.close();

                                if(sumreport==1)
                                {
                                    qDebug() << QString("sumAstFile: %1\n").arg(outfolder+"/"+"aggregate_eq.txt");
                                    QFile astFile(outfolder+"/"+"aggregate_eq.txt");
                                    astFile.open(QIODevice::Append| QIODevice::Text);
                                    QTextStream astStream;
                                    astStream.setDevice(&astFile);
                                    astStream << outstr << "\n";
                                    astFile.close();
                                }
                            }

                            qDebug() << QString("mpcreport: %1\n").arg(mpcreport);
                            if(mpcreport==1)
                            {
                                outstr ="";
                                oc->rec2MPC(&outstr, obscode, mObj->mpcObj->num.toInt(), mpcObsType);
                                /*int obj_num;

                                dateObs = getDATEOBSfromMJD(oc->MJday);
                                obj_num = oc->name.toInt();
                                outstr = outstr +QString( "%1" ).arg( mObj->sbot->Num.toInt(),5,10,QLatin1Char( '0' ))+"         C";
                                outstr = outstr +getStrFromDATEOBS(dateObs," ", 0, 5) +" ";
                                outstr = outstr +deg_to_hms(oc->ra, " ",3);
                                outstr = outstr +deg_to_damas(oc->de, " ",2)+"         ";
                                outstr = outstr  +QString( "%1" ).arg(oc->mag0,3,'f',1,QLatin1Char( '0' ));
                                outstr = outstr  +"        "+obscode;/
                                msgstr = mObj->sbot->Name;
                                msgstr = msgstr.trimmed();
                                //QMessageBox::information(0,"debug",outfolder+"/"+msgstr+"_mpc.txt",QMessageBox::Ok,0,0);
                                QFile mpcFile(outfolder+"/"+msgstr+"_mpc.txt");
                                mpcFile.open(QIODevice::Append| QIODevice::Text);
                                QTextStream mpcStream;
                                mpcStream.setDevice(&mpcFile);
                                mpcStream << outstr << "\n";
                                mpcFile.close();

                                if(sumreport==1)
                                {
                                    qDebug() << QString("sumMpcFile: %1\n").arg(outfolder+"/"+"aggregate_mpc.txt");
                                    QFile astFile(outfolder+"/"+"aggregate_mpc.txt");
                                    astFile.open(QIODevice::Append| QIODevice::Text);
                                    QTextStream astStream;
                                    astStream.setDevice(&astFile);
                                    astStream << outstr << "\n";
                                    astFile.close();
                                }
                            }

                            break;
                        }
                    case OBJ_TYPE_USNOB:
                        break;
                    }
                        //

                if(objresreport)
                {
                    oresFile.setFileName(outfolder+"/objRes.txt");
                    oresFile.open(QIODevice::WriteOnly | QIODevice::Append);
                    orStm.setDevice(&oresFile);

                    orRec.rec2s(&outstr);
                    orStm << outstr << "\n";

                    oresFile.close();
                }


                            //BEGIN errorreport
                    qDebug() << QString("errorreport: %1\n").arg(errorreport);
                            if(errorreport)
                            {
                                outstr.clear();
                                refMarks->ebRec->rec2s(&outstr);

                                QFile errFile(outfolder+"/err_budget_obj.txt");
                                errFile.open(QIODevice::Append| QIODevice::Text);
                                QTextStream errStream;
                                errStream.setDevice(&errFile);
                                errStream << outstr << "\n";
                                //errStream << "\n";
                                errFile.close();
                            }
                            //END astreport
                            //BEGIN residuals

                            QStringList outsl;
                            qDebug() << QString("residuals: %1\n").arg(residuals);
                            if(residuals)
                            {
                                outsl.clear();
                                //residualFile resData;
                                residualsRec* resRec;
                                outstr.clear();

                                resFile.resList.clear();
                                qDebug() << QString("rsindex.count= %1\n").arg(rsindex.count());
                                /*QFile resFile(outfolder+"/residuals_obj.txt");
                                resFile.open(QIODevice::Append| QIODevice::Text);
                                QTextStream resStream;
                                resStream.setDevice(&resFile);/
                                //int resNum = refMarks->marks.size();
                                resNum = refMarks->marks.size();
                                for (i=0;i<resNum;i++)
                                {
                                    outstr.clear();
                                    //j = rsindex.at(i);
                                    sdata = refMarks->marks.at(i)->sdata;
                                    refMarks->marks.at(i)->resRec = new residualsRec;
                                    resRec = refMarks->marks.at(i)->resRec;
                                        //sdata = refstars.at(i);
                                        x = sdata[4];
                                        y = sdata[5];
                                        pixmag = sdata[6];
                                        redMake->detTan(&ksi, &eta, x, y);

                                        //BEGIN add systematic corrections
                                        /*if (syscorr==1)
                                        {
                                                        ksi = ksi - syscorr_ra(sdata[8])/1000;
                                                        eta = eta - syscorr_de(sdata[8])/1000;
                                        }/
                                        //END add systematic corrections

                                        redMake->detMagn(&mag, pixmag);
                                        //BEGIN add systematic corrections to mag
                                        /*if (syscorr==1)
                                        {
                                                        mag = mag - syscorr_mag(sdata[8]);
                                        }/
                                        //END add systematic corrections  to mag
                                        //qDebug() << QString("mag= %1 sdata[8]= %2 maxOCMAGrc= %3\n").arg(mag).arg(sdata[8]).arg(maxOCMAGrc);
                                        //qDebug() << QString("ksi= %1 sdata[2]= %2 maxOCRArc= %3\n").arg(ksi).arg(sdata[2]).arg(maxOCRArc);
                                        //qDebug() << QString("eta= %1 sdata[3]= %2 maxOCDErc= %3\n").arg(eta).arg(sdata[3]).arg(maxOCDErc);
                                        if((fabs(mag - sdata[8])<=maxOCMAGrc)&&(grad_to_mas(fabs(ksi - sdata[2]))<=maxOCRArc)&&(grad_to_mas(fabs(eta - sdata[3]))<=maxOCDErc))
                                        {
                                                redMake->detXcYc(&xc, &yc, sdata[2], sdata[3]);
                                                //pmag = (sdata[8]-bm)/am;
                                                resRec->mJD = MJD;
                                                resRec->ra = sdata[0];
                                                resRec->de = sdata[1];
                                                resRec->ksi = ksi;
                                                resRec->eta = eta;
                                                resRec->mag = mag;
                                                resRec->ksiOC = grad_to_mas(ksi - sdata[2]);
                                                resRec->etaOC = grad_to_mas(eta - sdata[3]);
                                                resRec->magOC = mag - sdata[8];
                                                resRec->x = x;
                                                resRec->y = y;
                                                resRec->pixmag = pixmag;
                                                resRec->Dx = (x - xc);
                                                resRec->Dy = (y - yc);
                                                redMake->detPmag(&pmag, sdata[8]);
                                                resRec->Dpixmag = pixmag - pmag;

                                                resRec->isRefKsi = 0;
                                                resRec->isRefEta = 0;
                                                resRec->isRefMag = 0;
                                                for(j=0;j<rsindex.count();j++)
                                                {
                                                    if(i==rsindex[j])
                                                    {
                                                        resRec->isRefKsi = (int)(redMake->EXCLINDKSI[j]>=0);
                                                        resRec->isRefEta = (int)(redMake->EXCLINDETA[j]>=0);
                                                        resRec->isRefMag = (int)(redMake->EXCLINDMAG[j]>=0);
                                                    }
                                                }


                                                resRec->mesureTimeCode = mTimeCode;
                                                resRec->catName = refMarks->marks.at(i)->catName;
                                                resRec->catMagName = refMarks->marks.at(i)->catMagName;
                                                resRec->setMesParams(refMarks->marks.at(i)->P, 21);
                                                //resRec->exptime = exptime;
                                                resRec->rec2s(&outstr);
                                                //outstr += "\n";

                                                resFile.resList << resRec;
                                                //resStream << outstr << "\n";
                                                //resStream << "\n";
                                                //resStream << "\n";
                                        }
                                }

                                if(resSigma) resFile.remSigma(resSigma);
                                resFile.saveAs(outfolder+"/residuals_obj.txt");

            //                        resStream << outsl.join("\n");
                                //resFile.close();
                            }
                    //}
                    //else qDebug() << QString("too small RN\n");
                }

            //if(objresreport) oresFile.saveAs(outfolder+"/objRes.txt");
        }
/////////HAPPYEND////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




    return objMarks->marks.size();
}

*/


void copyGrid(marksGrid *mgSource, marksGrid *mgRes)
{
    int i, sz;
    sz = mgSource->marks.size();
    marksP *mkNew;

    for(i=0; i<sz; i++)
    {
        mkNew = new marksP;
        mkNew->copy(mgSource->marks.at(i));
        mgRes->marks << mkNew;
    }
}

void copyImgGrid(marksGrid *mgSource, marksGrid *mgRes)
{
    int i, sz;
    sz = mgSource->marks.size();
    marksP *mkNew;

    for(i=0; i<sz; i++)
    {
        mkNew = new marksP;
        mkNew->copyImg(mgSource->marks.at(i));
        mgRes->marks << mkNew;
    }
}

void findCloserMarks(marksGrid *mgIP, marksGrid *mgEkv, marksGrid *mgRes, double dMax)
{
    qDebug() << "findCloserMarks\n";
    int i, j, szE, szIP;
    marksP *mkNew;


    mgRes->clearMarks();
    mgRes->ctype = mgEkv->ctype;
 //   qDebug() << QString("mgRes size = %1\n").arg(mgRes->marks.size());
    szE = mgEkv->marks.size();
    szIP = mgIP->marks.size();
    double dist, distMin;//, distMax;
    int posMin;
    mkNew = new marksP;
    qDebug() << QString("mgEkv size = %1\n").arg(mgEkv->marks.size());
    qDebug() << QString("mgIP size = %1\n").arg(mgIP->marks.size());

    for(i=szIP-1; i>=0; i--)
    {
        //qDebug() << QString("i: %1\n").arg(i);
        distMin = dMax;
        posMin = -1;
        szE = mgEkv->marks.size();

        //qDebug() << QString(" mgIP->marks.at(i)->mTanImg: %1\t%2\n").arg(mgIP->marks.at(i)->mTanImg[0]).arg(mgIP->marks.at(i)->mTanImg[1]);

        for(j=0; j<szE; j++)
        {
            //qDebug() << QString("j: %1\n").arg(j);
            dist = marksImgDist(mgIP->marks.at(i), mgEkv->marks.at(j));
            //dist = sqrt(pow(mgEkv->marks.at(j)->mTanImg[0] - mgIP->marks.at(i)->mTanImg[0], 2.0) + pow(mgEkv->marks.at(j)->mTanImg[1] - mgIP->marks.at(i)->mTanImg[1], 2.0));
            //qDebug() << QString("Ekv[%1]: %2\t%3\tdist= %4\n").arg(j).arg(mgEkv->marks.at(j)->mTanImg[0]).arg(mgEkv->marks.at(j)->mTanImg[1]).arg(dist);
            if(dist<distMin)
            {
                distMin = dist;
                posMin = j;
            }
        }
        //qDebug() << QString("posMin: %1\tdistMin: %2\tdMax: %3\n").arg(posMin).arg(distMin).arg(dMax);
        if((posMin!=-1)&&(distMin<dMax))
        {
            mkNew->copy(mgEkv->marks.at(posMin));
            mkNew->copyImg(mgIP->marks.at(i));
            mgRes->addMark(mkNew);
            mgEkv->marks.removeAt(posMin);
        }
    }
    qDebug() << QString("mgRes size = %1\n").arg(mgRes->marks.size());

}

void fitsdata::findCloserStars(double dMax)
{
    qDebug() << "findCloserStars:\n";
    findCloserMarks(ipixMarks, catMarks, refMarks, dMax);
    catMarks->clearMarks();
}

void fitsdata::findCloserObjects(double dMax)
{
    qDebug() << QString("findCloserObjects\n");
    int i, j, szE, szIP;


   // mgRes->clearMarks();
 //   qDebug() << QString("mgRes size = %1\n").arg(mgRes->marks.size());
    szE = objMarks->marks.size();
    szIP = ipixMarks->marks.size();
    double dist, distMin;//, distMax;
    int posMin;

    qDebug() << QString("mgEkv size = %1\n").arg(szE);
    qDebug() << QString("mgIP size = %1\n").arg(szIP);

    for(i=szE-1; i>=0; i--)
    {
   //     qDebug() << QString("i: %1\n").arg(i);
        distMin = dMax;
        posMin = -1;
        szIP = ipixMarks->marks.size();
        //qDebug() << QString("objpos: %1\t%2\n").arg(objMarks->marks.at(i)->mTanImg[0]).arg(objMarks->marks.at(i)->mTanImg[1]);

        for(j=0; j<szIP; j++)
        {
            //qDebug() << QString("j: %1\n").arg(j);
            dist = sqrt(pow(objMarks->marks.at(i)->mTanImg[0] - ipixMarks->marks.at(j)->mTanImg[0], 2.0) + pow(objMarks->marks.at(i)->mTanImg[1] - ipixMarks->marks.at(j)->mTanImg[1], 2.0));
            //qDebug() << QString("markpos: %1\t%2\n").arg(ipixMarks->marks.at(j)->mTanImg[0]).arg(ipixMarks->marks.at(j)->mTanImg[1]);
            //qDebug() << QString("dist= %1\n").arg(dist);
            if(dist<distMin)
            {
                distMin = dist;
                posMin = j;
            }
        }
        //qDebug() << QString("posMin: %1\tdistMin: %2\tdMax: %3\n").arg(posMin).arg(distMin).arg(dMax);
        if((posMin!=-1)&&(distMin<dMax))
        {
            //mkNew = new marksP;

   //         qDebug() << "mkNew: " << mkNew << ":";
            objMarks->marks.at(i)->copyImg(ipixMarks->marks.at(posMin));
            //qDebug() << QString("objMarks: %1 %2 %3\n").arg(objMarks->marks.at(i)->mTanImg[0]).arg(objMarks->marks.at(i)->mTanImg[1]).arg(objMarks->marks.at(i)->mTanImg[2]);
            //if(!is_empty) ipixMarks->remMark(posMin);
        }
        else objMarks->remMark(i);


        //qDebug() << QString("mgIP->marks.removeAt(i)\n");

    }
    //if(!is_empty) ipixMarks->clearMarks();

    qDebug() << QString("\nfindCloserObjects result: %1 objects remain\n").arg(objMarks->marks.size());
}
/*
void fitsdata::initExpList()
{
    qDebug() << "\ninitExpList\n";
    expList = new ExposureList;
    expList->exps.clear();

    int i;

    int sz = headList.size();
    QStringList tList;
    int endObs = 0;
    double tObs0, tObs1;
    DATEOBS dateObs0, dateObs1;
    HeadRecord *hRec;
    ExposureRec* eRec;
    double Long;
    qDebug() << "obsPos: " << obsPos << "\n";
    if(obsPos!=NULL) Long = obsPos->Long/(2.0*PI);
    else Long = grad_to_rad(30.3274)/(2.0*PI);//day
    //Long = 0.0;
    double sTime, gm0, gm1, utc, jDay, jdUtc;
    int jDN;

    for(i=0; i<sz; i++)
    {
        hRec = headList.headList.at(i);
        qDebug() << QString("%1: %2 = %3\n").arg(i).arg(hRec->keyName).arg(hRec->keyValue);
        int jdNum;
        double T, s0, s1, dS, dS1, gm2, s2;

        if(hRec->keyName.lastIndexOf("DATE-OBS")==0)
        {
            //objLE->setText(fitsd->headList.at(i)->keyValue);
            //dateStr = QString(fitsd->headList.at(i)->keyValue);
            tList = hRec->keyValue.split(" ");
            dateObs0.year = dateObs1.year = tList.at(0).toInt();
            dateObs0.month = dateObs1.month = tList.at(1).toInt();
            dateObs0.day = dateObs1.day = tList.at(2).toInt();
        }
        else if(hRec->keyName.lastIndexOf("TIME-OBS")==0)
        {
            tList = hRec->keyValue.split(" ");
    //        time = tList.at(0).toInt()/24.0 + tList.at(1).toInt()/24.0/60.0 + tList.at(1).toDouble()/24.0/60.0/60.0;
            if(endObs)
            {
                qDebug() << QString("\nendObs\n");
                /*
                dateObs1.hour = tList.at(0).toInt();
                dateObs1.min = tList.at(1).toInt();
                dateObs1.sec = tList.at(2).toDouble();/

                sTime = tList.at(0).toInt()/24.0 + tList.at(1).toInt()/1440.0 + tList.at(2).toInt()/86400.0;
                gm1 = sTime - Long;
                dat2JD(&jDay, dateObs0.year, dateObs0.month, dateObs0.day);
                //jDN = dat2JDN(dateObs0.year, dateObs0.month, dateObs0.day);

                //jDN = dat2JDN(dateObs0.year, dateObs0.month, dateObs0.day);
                //T = detT(jDN);
                //s0 = gmst0(T);
                s0 = gmst0jd(jDay);
                s1 = gmst0jd(jDay+1);
                dS = gm1 - s0;
                dS1 = gm1 - s1;
                //qDebug() << QString("dS1= %1\n").arg(dS1);
                //s1 = gmst1(0.0, s0);
                if(dS<0.0) dS +=1.0;
                if(dS<0.5) jDay+=1.0;

                else if(dS<0.5) jDay+=1.0;
                qDebug() << QString("jDay= %1\ts0= %2\ts1= %3\tdS= %4\tdS1= %5\n").arg(jDay, 10, 'f', 4).arg(s0).arg(s1).arg(dS).arg(dS1);
                //if(dS<0.0) dS+=1.0;
                //if(dS<0.5) jDay+=1.0;


                qDebug() << QString("dS= %1\tjDay= %2\n").arg(dS).arg(jDay, 10, 'f', 4);

                GMST1_to_jdUT1(&jdUtc, gm1, jDay);
                qDebug() << QString("jdUTC= %1\tdS= %2\tdS1= %3\n").arg(jdUtc, 10, 'f', 4).arg(dS).arg(dS1);
                if(dS1<0.0)dS1+=1.0;
                qDebug() << QString("dS= %1\tdS1= %2\n").arg(dS).arg(dS1);
                if((dS1>0.5)&&(dS<0.5)) jdUtc-=(1.0-VIU);

                tObs1 = jd2mjd(jdUtc);
                jdUT1_to_GMST1(&gm2, mjd2jd(tObs1));
                s2 = gm1 + Long;
                qDebug() << QString("jDay= %1\tutc= %2\ttObs1= %3\n").arg(jDay, 10, 'f', 3).arg(jdUtc, 10, 'f', 3).arg(tObs1, 10, 'f', 3);
                qDebug() << QString("s2= %1\tsTime= %2\n").arg(s2).arg(sTime);
                //tObs0 = jd2mjd(getJD(dateObs0));
                //tObs1 = jd2mjd(getJD(dateObs1));
                //dat2JD_time(&tObs0, dateObs0.year, dateObs0.month, dateObs0.day, dateObs0.hour, dateObs0.min, dateObs0.sec);
                //dat2JD_time(&tObs1, dateObs1.year, dateObs1.month, dateObs1.day, dateObs1.hour, dateObs1.min, dateObs1.sec);

                qDebug() << QString("tObs0= %1\ttObs1= %2\n").arg(tObs0, 12, 'f', 5).arg(tObs1, 12, 'f', 5);

                if(tObs1<tObs0) tObs1++;

                eRec = new ExposureRec;
                eRec->expVal = (tObs1 - tObs0)*86400.0;
                eRec->expTime = (tObs0+tObs1)/2.0;
                qDebug() << QString("getDATEOBSfromMJD: %1\n").arg(eRec->timeStr());

                expList->exps << eRec;

                endObs = 0;
            }
            else
            {
                qDebug() << QString("\nstartObs\n");
                sTime = tList.at(0).toInt()/24.0 + tList.at(1).toInt()/1440.0 + tList.at(2).toInt()/86400.0;
                gm1 = sTime - Long;
                qDebug() << QString("sTime= %1\tgm1= %2\tLong= %3\n").arg(sTime).arg(gm1).arg(Long);
                dat2JD(&jDay, dateObs0.year, dateObs0.month, dateObs0.day);

                //jDN = dat2JDN(dateObs0.year, dateObs0.month, dateObs0.day);
                //T = detT(jDN);
                //s0 = gmst0(T);
                //s1 = gmst1(0.0, s0);

                s0 = gmst0jd(jDay);
                s1 = gmst0jd(jDay+1);
                dS = gm1 - s0;
                dS1 = gm1 - s1;
                qDebug() << QString("jDay= %1\ts0= %2\ts1= %3\tdS= %4\tdS1= %5\n").arg(jDay, 10, 'f', 4).arg(s0).arg(s1).arg(dS).arg(dS1);
                if(dS<0.0) dS +=1.0;
                if(dS<0.5) jDay+=1.0;

                qDebug() << QString("dS= %1\tjDay= %2\n").arg(dS).arg(jDay, 10, 'f', 4);

                GMST1_to_jdUT1(&jdUtc, gm1, jDay);
                qDebug() << QString("jdUTC= %1\tdS= %2\tdS1= %3\n").arg(jdUtc, 10, 'f', 4).arg(dS).arg(dS1);
                if(dS1<0.0)dS1+=1.0;
                qDebug() << QString("dS= %1\tdS1= %2\n").arg(dS).arg(dS1);
                if((dS1>0.5)&&(dS<0.5)) jdUtc-=(1.0-VIU);



                //dateObs0.hour = utc*24.0;
                //dateObs0.min = tList.at(1).toInt();
                //dateObs0.sec = tList.at(2).toDouble();
                tObs0 = jd2mjd(jdUtc);

                qDebug() << QString("jDay= %1\tutc= %2\ttObs0= %3\n").arg(jDay, 10, 'f', 3).arg(jdUtc, 10, 'f', 3).arg(tObs0, 10, 'f', 3);
                endObs = 1;
            }

        }
    }



}
*/
void fitsdata::setMJD(double mjd)
{
    qDebug() << QString("setMJD: %1\n").arg(mjd);
    MJD = mjd;
}
/*
void fitsdata::setSpecTime(int isConvertToUTC, int isSpec, double Long)
{
    double sJD, eJD, d_day, uTimeJD, utc, jd0, uTimeMJD;
    jd0 = UT0JD(mjd2jd(MJD), &sJD);
    utc = sJD;
    double gm1;
    qDebug() << QString("jd0: %1\ttime: %2\n").arg(jd0).arg(utc);
    qDebug() << QString("sJD= %1\n").arg(sJD);
    qDebug() << QString("Long= %1\n").arg(Long);
    if(isConvertToUTC)
    {
        /*qDebug() << QString("obsNum %1\n").arg(obsCode);
        if(obsList->getobsynumO(obsCode.toAscii().data()))
        {
            qDebug() << "obs not found\n";
            QDir(platePath).remove(wcsFileName);
            return 2;
        }/

        sJD2UT1(jd0, sJD, Long, &utc);
        uTimeJD = jd0 + utc;
        MJD = uTimeMJD = jd2mjd(uTimeJD);
    }
    qDebug() << QString("utc= %1\n").arg(utc);
    qDebug() << QString("uTimeJD= %1\n").arg(uTimeJD);
    if(isSpec)

    {
        d_day=MJD*2.0;
        d_day-=chet(d_day);
        eJD = ((double)(int)d_day)/2.0;
        d_day = (MJD - eJD) - 0.5;
        qDebug() << QString("MJD= %1\teJD = %2\td_day = %3\n").arg(MJD).arg(eJD).arg(d_day);
        if(d_day>0.0&&d_day<0.2917)
        {
            //curF->MJD += 1.0;
            jd0+=1.0;
            //jd0 = UT0JD(mjd2jd(curF->MJD), &sJD);
            sJD2UT1(jd0, sJD, Long, &utc);
            //curF->MJD = jd2mjd(jd0 + utc);
        }


    }
    QString uTimeLE;
    headList.getKeyName("U", &uTimeLE);
    MJD = jd2mjd(jd0 + utc);
    //qDebug() << QString("uTime: %1\n").arg(uTimeLE->text().trimmed().toDouble());
    MJD += uTimeLE.trimmed().toDouble()/86400.0;
}
*/
void fitsdata::initRaDeHeadList()
{
    qDebug() << "\ninitRaDeHeadList\n";
    QString tStr;

    if(!headList.getKeyName("RA2000", &tStr))  WCSdata[2] = hms_to_deg(tStr," ");
    else if(!headList.getKeyName("RA", &tStr))  WCSdata[2] = hms_to_deg(tStr," ");
    if(!headList.getKeyName("DEC2000", &tStr)) WCSdata[3] = damas_to_deg(tStr," ");
    else if(!headList.getKeyName("DEC", &tStr)) WCSdata[3] = damas_to_deg(tStr," ");

    qDebug() << "\nEND initRaDeHeadList\n";
}
/*
void fitsdata::getMpephObject(QStringList outerArguments, QString ast_eph_prog, QString ast_eph_prog_folder, double mag0, double mag1)
{
    QProcess outerProcess;
    //QString ast_eph_prog;
    outerProcess.setWorkingDirectory(ast_eph_prog_folder);
    outerProcess.setProcessChannelMode(QProcess::MergedChannels);
    outerProcess.setReadChannel(QProcess::StandardOutput);

    qDebug() << "outerArguments: " << ast_eph_prog << " " << outerArguments.join("|") << "\n";
    outerProcess.start(ast_eph_prog, outerArguments);

    outerProcess.waitForFinished(mpeWaitTime);
    QTextStream objStream(outerProcess.readAllStandardOutput());

    marksP *mT;

    ////
    QString objDataStr, elS;
    QStringList opers;
    mT = new marksP(OBJ_TYPE_MPEPH);
    while (!objStream.atEnd())
    {
        objDataStr = objStream.readLine();
        qDebug() << QString("objDataStr: %1").arg(objDataStr);


        //mT->mpcObj = new mpephRec;
        //mT->objType = 0;
        qDebug() << "mpcObj: " << mT->mpcObj << "\n";
        if(mT->mpcObj->fromString(objDataStr))
        {
            qDebug() << "\nfromString error\n";
            continue;
        }
        if((mT->mpcObj->Vmag<mag0)||(mT->mpcObj->Vmag>mag1))
        {
            qDebug() << QString("\nVmag problem: %1\n").arg(mT->mpcObj->Vmag);
            continue;
        }
        mT->mEkv[0] = mT->mpcObj->ra;//fitsd->objMarks->addEkvMark(ra, de, mag);
        mT->mEkv[1] = mT->mpcObj->de;
        mT->mEkv[2] = mT->mpcObj->Vmag;
        mT->catName = QString("mpeph");
        mT->catMagName = QString("Vmag");

        qDebug() << QString("Object: %1\tra: %2\tde:%3\tmagn:%4\n").arg(mT->mpcObj->name).arg(deg_to_hms(mT->mEkv[0], " ", 5)).arg(deg_to_damas(mT->mEkv[1], " ", 5)).arg(mT->mEkv[2]);

        objMarks->marks << mT;
        mT = new marksP(OBJ_TYPE_MPEPH);
    }

    qDebug() << QString("getMpephObject find %1 objects\n").arg(objMarks->marks.size());

    qDebug() << "end getMpephObject\n";
    detTanObj();
}

void fitsdata::getMpephNum(QString mpeNum, QString ast_eph_prog, QString ast_eph_prog_folder, double mag0, double mag1)
{
    QStringList outerArguments;
    outerArguments << QString("-num");
    outerArguments << mpeNum;
    QString sMjd = QString("%1").arg(MJD, 11, 'f',7);
    outerArguments << QString("%1").arg(sMjd);
    getMpephObject(outerArguments, ast_eph_prog, ast_eph_prog_folder, mag0, mag1);
}

void fitsdata::getMpephName(QString mpeName, QString ast_eph_prog, QString ast_eph_prog_folder, double mag0, double mag1)
{
    QStringList outerArguments;
    outerArguments << QString("-name");
    outerArguments << mpeName;
    QString sMjd = QString("%1").arg(MJD, 11, 'f',7);
    outerArguments << QString("%1").arg(sMjd);
    getMpephObject(outerArguments, ast_eph_prog, ast_eph_prog_folder, mag0, mag1);
}

void fitsdata::getLspmObject(QString lspmName, QString lspm_prog, QString lspm_prog_folder, QString lspm_cat_file)
{
    QProcess outerProcess;
    //QString ast_eph_prog;
    outerProcess.setWorkingDirectory(lspm_prog_folder);
    outerProcess.setProcessChannelMode(QProcess::MergedChannels);
    outerProcess.setReadChannel(QProcess::StandardOutput);

    QStringList outerArguments;
    outerArguments << lspmName << lspm_cat_file;

    qDebug() << "outerArguments: " << lspm_prog << " " << outerArguments.join("|") << "\n";
    outerProcess.start(lspm_prog, outerArguments);

    outerProcess.waitForFinished(-1);
    QTextStream objStream(outerProcess.readAllStandardOutput());

    marksP *mT;
 //   long naxes[2];

    ////
    QString objDataStr, elS;
    QStringList opers;
    char *strin = new char[1024];
    while (!objStream.atEnd())
    {
        objDataStr = objStream.readLine();
        qDebug() << QString("objDataStr: %1").arg(objDataStr);

        mT = new marksP(OBJ_TYPE_SSTAR);
        //mT->mpcObj = new mpephRec;
        //mT->objType = 0;
        //qDebug() << "mpcObj: " << mT->sstar << "\n";
        sprintf(strin, "%s\0", objDataStr.toAscii().data());
        if(mT->sstar->fromString(strin)) continue;
        //qDebug() << "mT->mEkv: " << mT->mEkv << "\n";
        qDebug() << QString("pm: %1\t%2\n").arg(mT->sstar->pmRA).arg(mT->sstar->pmDE);
        mT->mEkv[0] = mT->sstar->RAdeg + mas_to_grad(1000*mT->sstar->pmRA*(getYearFromMJD(MJD)-2000));//taking proper motion into account
        mT->mEkv[1] = mT->sstar->DEdeg + mas_to_grad(1000*mT->sstar->pmDE*(getYearFromMJD(MJD)-2000));//taking proper motion into account
        //mT->mEkv[0] = mT->sstar->RAdeg;
        //mT->mEkv[1] = mT->sstar->DEdeg;
        mT->mEkv[2] = mT->sstar->BJmag;
        mT->catName = QString("LSPM");
        mT->catMagName = QString("BJmag");

        qDebug() << QString("Object: %1\tra: %2\tde:%3\tmagn:%4\n").arg(mT->sstar->getLSPM()).arg(mas_to_hms(mT->mEkv[0], " ", 5)).arg(mas_to_damas(mT->mEkv[1], " ", 5)).arg(mT->mEkv[2]);

        addObjMark(mT);
  //      objMarks->marks << mT;
    }

    qDebug() << "end\n";
    detTanObj();

}

void fitsdata::findLspmCat(QString lspm_prog, QString lspm_prog_folder, QString lspm_cat_file, double fovp, double mag0, double mag1)
{
    QProcess outerProcess;
    //QString ast_eph_prog;
    outerProcess.setWorkingDirectory(lspm_prog_folder);
    outerProcess.setProcessChannelMode(QProcess::MergedChannels);
    outerProcess.setReadChannel(QProcess::StandardOutput);

    double dFov = detFov();

    qDebug() << QString("dFov= %1\tfovp= %2\n").arg(dFov).arg(fovp);
    double fov = fovp*dFov*0.01;

    QStringList outerArguments;
    //outerArguments << lspmName << lspm_cat_file;
    outerArguments << deg_to_hms(WCSdata[2], ":", 3) << deg_to_damas(WCSdata[3],":",2) << QString("r=%1").arg(fov) << QString("%1").arg(lspm_cat_file);

    qDebug() << "outerArguments: " << lspm_prog << " " << outerArguments.join("|") << "\n";
    outerProcess.start(lspm_prog, outerArguments);

    outerProcess.waitForFinished(-1);
    QTextStream objStream(outerProcess.readAllStandardOutput());

    marksP *mT;
 //   long naxes[2];

    ////
    QString objDataStr, elS;
    QStringList opers;
    char *strin = new char[1024];
    mT = new marksP(OBJ_TYPE_SSTAR);
    while (!objStream.atEnd())
    {
        objDataStr = objStream.readLine();
        qDebug() << QString("\nobjDataStr|%1|\n").arg(objDataStr);

        sprintf(strin, "%s", objDataStr.toAscii().data());
        if(mT->sstar->fromString(strin))
        {
            qDebug() << "wrong string\n";
            continue;
        }
        //qDebug() << "mT->mEkv: " << mT->mEkv << "\n";
        if((mT->sstar->BJmag<mag0)||(mT->sstar->BJmag>mag1))
        {
            qDebug() << QString("wrong mag interval. BJmag: %1\tobjMag0: %2\tobjMag1:%3\n").arg(mT->sstar->BJmag).arg(mag0).arg(mag1);
            continue;
        }
        qDebug() << QString("pm: %1\t%2\n").arg(mT->sstar->pmRA).arg(mT->sstar->pmDE);
        mT->mEkv[0] = mT->sstar->RAdeg + mas_to_grad(1000*mT->sstar->pmRA*(getYearFromMJD(MJD)-2000));//taking proper motion into account
        mT->mEkv[1] = mT->sstar->DEdeg + mas_to_grad(1000*mT->sstar->pmDE*(getYearFromMJD(MJD)-2000));//taking proper motion into account
        //mT->mEkv[0] = mT->sstar->RAdeg;
        //mT->mEkv[1] = mT->sstar->DEdeg;
        mT->mEkv[2] = mT->sstar->BJmag;
        mT->catName = QString("LSPM");
        mT->catMagName = QString("BJmag");

        qDebug() << QString("Object: %1\tra: %2\tde:%3\tmagn:%4\n").arg(mT->sstar->getLSPM()).arg(deg_to_hms(mT->mEkv[0], " ", 5)).arg(deg_to_damas(mT->mEkv[1], " ", 5)).arg(mT->mEkv[2]);

        addObjMark(mT);
        mT = new marksP(OBJ_TYPE_SSTAR);
  //      objMarks->marks << mT;
    }
    qDebug() << QString("objMarks size: %1\n").arg(objMarks->marks.size());
    qDebug() << "end\n";
    detTanObj();

}

void fitsdata::findSkybot(QString prog, QString progFolder, double fovp, QString obsCode, double mag0, double mag1)
{
    QProcess outerProcess;
    //QString ast_eph_prog;
    outerProcess.setWorkingDirectory(progFolder);
    outerProcess.setProcessChannelMode(QProcess::MergedChannels);
    outerProcess.setReadChannel(QProcess::StandardOutput);

    double dFov = detFov();

    qDebug() << QString("dFov= %1\tfovp= %2\n").arg(dFov).arg(fovp);
    double fov = fovp*dFov*0.01;

    QStringList outerArguments;
    //outerArguments << lspmName << lspm_cat_file;
    outerArguments << QString("%1").arg(mjd2jd(MJD), 14, 'f', 5).simplified() << QString("%1").arg(WCSdata[2], 13, 'f', 8).simplified() << QString("%1").arg(WCSdata[3], 13, 'f', 8).simplified() << QString("%1").arg(fov, 6, 'f', 2).simplified() << obsCode;

    qDebug() << "outerArguments: " << prog << " " << outerArguments.join("|") << "\n";
    outerProcess.start(prog, outerArguments);

    outerProcess.waitForFinished(sbWaitTime);
    QTextStream objStream(outerProcess.readAllStandardOutput());

    marksP *mT;
 //   long naxes[2];

    ////
    QString objDataStr, elS;
    QStringList opers;
    char *strin = new char[1024];
    int headNum = 0;
    mT = new marksP(OBJ_TYPE_SKYBOT);

    while (!objStream.atEnd())
    {
        objDataStr = objStream.readLine();
        qDebug() << QString("objDataStr: %1").arg(objDataStr);
        if(headNum<3)
        {
            headNum++;
            continue;
        }


        //mT->mpcObj = new mpephRec;
        //mT->objType = 0;
        //qDebug() << "mpcObj: " << mT->sstar << "\n";
//        sprintf(strin, "%s\0", objDataStr.toAscii().data());
        if(mT->sbot->fromString(objDataStr)) continue;
        qDebug() << QString("mag0: %1\tMv: %2\tmag1: %3\n").arg(mag0).arg(mT->sbot->Mv).arg(mag1);
        if((mT->sbot->Mv<mag0)||(mT->sbot->Mv>mag1)) continue;
        qDebug() << "mT->mEkv: " << mT->mEkv << "\n";
//        qDebug() << QString("pm: %1\t%2\n").arg(mT->sstar->pmRA).arg(mT->sstar->pmDE);
        mT->mEkv[0] = mT->sbot->RA;// + mas_to_grad(1000*mT->sstar->pmRA*(getYearFromMJD(MJD)-2000));//taking proper motion into account
        mT->mEkv[1] = mT->sbot->DE;// + mas_to_grad(1000*mT->sstar->pmDE*(getYearFromMJD(MJD)-2000));//taking proper motion into account
        //mT->mEkv[0] = mT->sstar->RAdeg;
        //mT->mEkv[1] = mT->sstar->DEdeg;
        mT->mEkv[2] = mT->sbot->Mv;
        mT->catName = QString("skybot");
        mT->catMagName = QString("Mv");

            qDebug() << QString("Object: %1\tra: %2\tde:%3\tmagn:%4\n").arg(mT->sbot->Name).arg(deg_to_hms(mT->mEkv[0], " ", 5)).arg(deg_to_damas(mT->mEkv[1], " ", 5)).arg(mT->mEkv[2]);

        addObjMark(mT);
        mT = new marksP(OBJ_TYPE_SKYBOT);
  //      objMarks->marks << mT;
    }

    qDebug() << QString("findSkybot objects selected num: %1").arg(objMarks->marks.size());

    detTanObj();
    qDebug() << "end findSkybot\n";
}

void fitsdata::findSkybotNamesList(QStringList *namesList, QString prog, QString progFolder, double fovp, QString obsCode, double mag0, double mag1)
{
    QProcess outerProcess;
    //QString ast_eph_prog;
    outerProcess.setWorkingDirectory(progFolder);
    outerProcess.setProcessChannelMode(QProcess::MergedChannels);
    outerProcess.setReadChannel(QProcess::StandardOutput);

    double dFov = detFov();

    qDebug() << QString("dFov= %1\tfovp= %2\n").arg(dFov).arg(fovp);
    double fov = fovp*dFov*0.01;

    QStringList outerArguments;
    //outerArguments << lspmName << lspm_cat_file;
    outerArguments << QString("%1").arg(mjd2jd(MJD), 14, 'f', 5).simplified() << QString("%1").arg(WCSdata[2], 13, 'f', 8).simplified() << QString("%1").arg(WCSdata[3], 13, 'f', 8).simplified() << QString("%1").arg(fov, 6, 'f', 2).simplified() << obsCode;

    qDebug() << "outerArguments: " << prog << " " << outerArguments.join("|") << "\n";
    outerProcess.start(prog, outerArguments);

    outerProcess.waitForFinished(sbWaitTime);
    QTextStream objStream(outerProcess.readAllStandardOutput());

    marksP *mT;
 //   long naxes[2];

    ////
    QString objDataStr, elS;
    QStringList opers;
    char *strin = new char[1024];
    int headNum = 0;
    skybotRec *sbRec = new skybotRec;
    while (!objStream.atEnd())
    {
        objDataStr = objStream.readLine();
        qDebug() << QString("objDataStr: %1").arg(objDataStr);
        if(headNum<3)
        {
            headNum++;
            continue;
        }

        if(sbRec->fromString(objDataStr)) continue;
        if((sbRec->Mv>=mag0)&&(sbRec->Mv<=mag1))
        {
            namesList->append(sbRec->Name);
            //namesList->append(sbRec->Num);
        }
    }


}
*/
void fitsdata::addObjMark(marksP *mP)
{
    objMarks->marks << mP;
}

void fitsdata::detTanObj()
{
//    long naxes[2];
    qDebug() << "\ndetTanObj\n";
    //detNaxes(&naxes[0], &naxes[1]);
    objMarks->detTan(&WCSdata[0]);
}

void remCrossedMarks(marksGrid *mg0, marksGrid *mgRem, double dMin)
{
    qDebug() << "remCrossedMarks\n";
    int i, j, sz0, szRem;

    sz0 = mg0->marks.size();
    double dist;

    for(i=0; i<sz0; i++)
    {
        qDebug() << QString("i: %1\n").arg(i);

        szRem = mgRem->marks.size();

        for(j=szRem-1; j>=0; j--)
        {
            dist = sqrt(pow(mg0->marks.at(i)->mTanImg[0] - mgRem->marks.at(j)->mTanImg[0], 2.0) + pow(mg0->marks.at(i)->mTanImg[1] - mgRem->marks.at(j)->mTanImg[1], 2.0));
            if(dist<dMin)
            {
                qDebug() << QString("remove %1 mark\n").arg(j);
                mgRem->remMark(j);
            }
        }
    }

}

void remCrossedMarks(marksGrid *mg0, double dMin)
{
    qDebug() << "remCrossedMarks\n";
    int i, j, sz0, szRem;

    sz0 = mg0->marks.size();
    double dist;

    for(i=0; i<mg0->marks.size()-1; i++)
    {


  //      szRem = mgRem->marks.size();

        for(j=i+1; j<mg0->marks.size(); j++)
        {
            dist = sqrt(pow(mg0->marks.at(i)->mTanImg[0] - mg0->marks.at(j)->mTanImg[0], 2.0) + pow(mg0->marks.at(i)->mTanImg[1] - mg0->marks.at(j)->mTanImg[1], 2.0));
            if(dist<dMin)
            {
                qDebug() << QString("i: %1\tj: %2\n").arg(i).arg(j);
                mg0->remMark(j);
                j--;
            }
        }
    }

}

void fitsdata::cleanObjects(int aper)
{
    remCrossedMarks(objMarks, catMarks, aper);
}
/*
void fitsdata::cleanEkvMarks(marksGrid *oGr, int aper)
{
    qDebug() << "remCrossedMarks\n";
    int i, j, szObj, szCat;
    double dist;

    dist = aper*((WCSdata[4]+WCSdata[5])/2.0);

    marksGrid *cGr = catMarks;
    marksGrid *oGr = objMarks;

    szObj = oGr->getNum();


    for(i=szObj-1; i>=0; i--)
    {
        szCat = cGr->getNum();
        for(j=szCat-1; j>=0; j++)
        {

        }
    }

}
*/

/*
void fitsdata::initRefractParam()
{
    qDebug() << "initRefractParam\n";
    refParam = new refractionParam;
    refParam->utc = MJD;

    refParam->ra0 = WCSdata[2];
    refParam->de0 = WCSdata[3];

    if(obsPos!=NULL) refParam->Fi = obsPos->getFi();
    else refParam->Fi = 0.0;
    if(obsPos!=NULL) refParam->Long = obsPos->Long;
    else refParam->Long = 0.0;
    refParam->lam = 0.575;

    QString kVal;
    int p0, p1;
    double temp;

    if(is_empty)
    {
        if(!headList.getKeyName("PRESSURE", &kVal))
        {
            qDebug() << QString("\nPRESSURE |%1|\n").arg(kVal);
            refParam->press = kVal.toDouble();
        }
        else refParam->press = 760.0;
        if(refParam->press<700.0)refParam->press = 760.0;
        if(!headList.getKeyName("TEMPERAT", &kVal))
        {
            qDebug() << QString("\nTEMPERAT |%1|\n").arg(kVal);
            refParam->temp = kVal.toDouble();
        }
        else refParam->temp = 0.0;
    }
    else
    {
        refParam->press = 760.0;
        p0 = fitsHeader.lastIndexOf("AIRTEMP =");
        if(p0==-1)
        {
            temp = 0.0;
        }
        else
        {
            p1 = fitsHeader.indexOf("/", p0+ 9);
            kVal = fitsHeader.mid(p0+10, p1-p0-10);
            temp = kVal.simplified().toDouble();
        }
        refParam->temp = temp;
    }

    qDebug() << "refParam: " << refParam << "\n";
    qDebug() << QString("utc: %1\nfi: %2\nLong= %3\nlam = %4\npress = %5\ntemp = %6\n").arg(refParam->utc).arg(refParam->Fi).arg(refParam->Long).arg(refParam->lam).arg(refParam->press).arg(refParam->temp);
}
*/
void fitsdata::initObsPos(obsy *obsp)
{
    obsPos = new obsy;
    obsp->copyTo(obsPos);
}

int fitsdata::findHstars(int apeDiam, int targNum)
{
    QVector <double> cmX, cmY, flux;
    int i, sz;
    //qDebug() << QString("naxes: %1\t%2\n").arg(naxes[0]).arg(naxes[1]);

    cmX.clear();
    cmY.clear();
    flux.clear();
    findStars(imgArr, cmX, cmY, flux, apeDiam, 50);

    sz = cmX.size();
    qDebug() << QString("first run res: %1\n").arg(sz);
    if(sz<targNum)
    {
        cmX.clear();
        cmY.clear();
        flux.clear();
        findStars(imgArr, cmX, cmY, flux, apeDiam, 100);
    }
    qDebug() << QString("second run res: %1\n").arg(sz);
    if(sz<targNum) return 1;

    for(i=0; i<sz; i++)
    {
        ipixMarks->addImgMark(cmX.at(i), cmY[i], flux.at(i));
    }

    qDebug() << QString("sz= %1\n").arg(sz);
    return (sz<targNum);
}

void fitsdata::setPos(double mJD, double ra, double de)
{
    MJD = mJD;
    WCSdata[2] = ra;
    WCSdata[3] = de;
}
/*
void fitsdata::initMagEqXDX(QString xCoefFileName, QString yCoefFileName, QString dataSep)
{
    qDebug() << "\ninitMagEqXDX\n";
    int i, j, szi;
    int dNum;
    double *x;

    QString dataStr;
    QStringList dataStrL, opers;

    QFile rFile;
    QTextStream dataStream;

    rFile.setFileName(xCoefFileName);
    if(!rFile.open(QIODevice::ReadOnly| QIODevice::Text)) qDebug() << "\nCoef File not open\n";
    dataStream.setDevice(&rFile);

    mCoefX.clear();
    while(!dataStream.atEnd())
    {
        dataStr = dataStream.readLine();
        opers = dataStr.split(dataSep);
        mCoefDeg = opers.size();
        x = new double[mCoefDeg];
        for(i=0; i<mCoefDeg;i++) x[i] = opers.at(i).toDouble();
        mCoefX << x;
    }
    rFile.close();

    rFile.setFileName(yCoefFileName);
    rFile.open(QIODevice::ReadOnly| QIODevice::Text);
    dataStream.setDevice(&rFile);

    mCoefY.clear();
    while(!dataStream.atEnd())
    {
        dataStr = dataStream.readLine();
        opers = dataStr.split(dataSep);
        mCoefDeg = opers.size();
        x = new double[mCoefDeg];
        for(i=0; i<mCoefDeg;i++) x[i] = opers.at(i).toDouble();
        mCoefY << x;
    }
    rFile.close();

    QStringList dsList;
    szi = mCoefX.size();
    for(i=0; i<szi; i++)
    {
        dsList.clear();
        for(j=0; j<mCoefDeg; j++) dsList << QString("%1").arg(mCoefX[i][j], 12, 'e');
        qDebug() << QString("mCoefX[%1]: %2\n").arg(i).arg(dsList.join("|"));
    }

    szi = mCoefY.size();
    for(i=0; i<szi; i++)
    {
        dsList.clear();
        for(j=0; j<mCoefDeg; j++) dsList << QString("%1").arg(mCoefY[i][j], 12, 'e');
        qDebug() << QString("mCoefY[%1]: %2\n").arg(i).arg(dsList.join("|"));
    }

    isMagEqCorr = 1;
}

void fitsdata::initComaCorr(double cKsi, double cEta, double cMag0)
{
    comaKsi = cKsi;
    comaEta = cEta;
    comaMag0 = cMag0;

    isComaCorr = 1;
}

void fitsdata::initVFcorr0(QString vfFileName, double rmax, int nmin, int isBC)
{
    vfCorr0 = new vfParam;
    vfCorr0->vectFcorr = new vectGrid3D;
    QByteArray tmp = vfFileName.toAscii();
    vfCorr0->vectFcorr->init(tmp.data());
    vfCorr0->vectFcorr->initVF();
    vfCorr0->vfRmax = rmax;
    vfCorr0->nMin = nmin;
    vfCorr0->isBC = isBC;
}

void fitsdata::initVFcorr1(QString vfFileName, double rmax, int nmin, int isBC)
{
    vfCorr1 = new vfParam;
    vfCorr1->vectFcorr = new vectGrid3D;
    QByteArray tmp = vfFileName.toAscii();
    vfCorr1->vectFcorr->init(tmp.data());
    vfCorr1->vectFcorr->initVF();
    vfCorr1->vfRmax = rmax;
    vfCorr1->nMin = nmin;
    vfCorr1->isBC = isBC;
}

void fitsdata::initMdCorr(mdCorrParam param)
{
    mdcParam = new mdCorr;
    mdcParam->colSep = QString(param.colSep);
    mdcParam->corrModel = param.corrModel;
    mdcParam->mdCorrX = QString(param.mdCorrX);
    mdcParam->mdCorrY = QString(param.mdCorrY);

    initMdCoef(mdcParam->mdCorrX, &mdcParam->mdCoefX, &mdcParam->mdCoefXsz, mdcParam->colSep);
    initMdCoef(mdcParam->mdCorrY, &mdcParam->mdCoefY, &mdcParam->mdCoefYsz, mdcParam->colSep);

    mdcParam->dCoefXsz = mdcParam->mdCoefX.size();
    mdcParam->dCoefYsz = mdcParam->mdCoefY.size();
}
*/
///////////////////////////////////////////////////////

marksGrid::marksGrid()
{
        marks.clear();
	num = 0;
	xc = 0.0;
	yc = 0.0;
        ebRec = NULL;
}

marksGrid::~marksGrid()
{
    if(ebRec!=NULL) delete ebRec;
	clearMarks();
}

void marksGrid::clearMarks()
{
	marksP *xt;
        //int *xti;
	int sz = marks.size();
	for(int i=sz-1; i>=0; i--)
	{
		
                xt = marks.at(i);
                delete xt;
                marks.removeAt(i);
	}
	
        //marks.clear();
}


marksP* marksGrid::addEkvMark(double ra, double de, double magn)
{
	marksP *mP = new marksP();
//	double *xt= new double[16];
	mP->mEkv[0] = ra;
	mP->mEkv[1] = de;
	mP->mEkv[2] = magn;
	
        marks.append(mP);
	
	num++;

        return mP;
	
}

marksP* marksGrid::addImgMark(double i, double j, double flux)
{
	marksP *mP = new marksP();
	mP->mTanImg[0] = i;
	mP->mTanImg[1] = j;
        mP->mTanImg[2] = flux;
	marks << mP;
	num++;
        return mP;
}

marksP* marksGrid::addMark(marksP *source)
{
    marksP *mP = new marksP();
    mP->copy(source);
    marks.append(mP);
    num++;
    return mP;
}
/*
void marksGrid:: setInstrProp(double xSc, double ySc, double ang)
{
	scales[0] = xSc;
	scales[1] = ySc;
	rotAn = ang;
//	qDebug() << "\nscales: " << xSc << "\t" << ySc;
//	qDebug() << "\nang: " << ang << "\n";
	
}

void marksGrid::setScales(double xSc, double ySc)
{
	scales[0] = xSc;
	scales[1] = ySc;
}
*/

//void marksGrid::detTan(double *WCSdata, long int *naxes)
void marksGrid::detTan(double *WCSdata)
{

    qDebug() << "\n\ndetTan\n\n";
	

        double *xtf, *xef, *xti;
	double x0, x1;
        double ra0, dec0;
	int i;
	
	marksP *mks;
	
	xtf = new double[2];
        xti = new double[2];
	
        qDebug() << "\nxc\t" << xc << "\tyc " << yc << "\n";
	int num = marks.size();
        qDebug() << "\nnum: " << num << "\n";
	for(i=0;i<num; i++)
	{
		mks = marks.at(i);

                ra0 = mks->mEkv[0];
                dec0 = mks->mEkv[1];

                getDegToTang(&x0, &x1, ra0, dec0, WCSdata[2], WCSdata[3]);

                mks->mTan[0] = x0;
                mks->mTan[1] = x1;

		if(!WCSdata[12])
		{
                    xti[0] = WCSdata[0] + (mks->mTan[0]*cos(WCSdata[6]) - mks->mTan[1]*sin(WCSdata[6]))/WCSdata[4] + xc;
                    xti[1] = WCSdata[1] + -(mks->mTan[0]*sin(WCSdata[6]) + mks->mTan[1]*cos(WCSdata[6]))/WCSdata[5] + yc;
			
                }
		else
		{
                    //getDegToTang(double *ksi, double *eta, double ra, double de, double ra_c, double de_c);
                        getDegToPixPosWCS(&x0, &x1, ra0, dec0, WCSdata);
                        //xtf[0] = x0;
                        //xtf[1] = x1;
			
                        xti[0] = x0;// + WCSdata[0];// + xc;// + naxes[0]/2.0;
                        xti[1] = x1;// + WCSdata[1];// + yc;// + naxes[1]/2.0;
			
		}

		mks->mTanImg[0] = xti[0];
		mks->mTanImg[1] = xti[1];
                //qDebug() << QString("\nmTan: %1\t%2\nmTanImg: %3\t%4\nscales: %5\t%6\n\n").arg(mks->mTan[0]).arg(mks->mTan[1]).arg(mks->mTanImg[0]).arg(mks->mTanImg[1]).arg(scales[0]).arg(scales[1]);
	}

}

void marksGrid::moveMarks(int dx, int dy)
{
	xc += dx;
	yc += dy;
}

void marksGrid::setMarksCenter(int x0, int y0)
{
	xc = x0;
	yc = y0;
}

int marksGrid::saveTanImg(QString fName, QString mSep, QString mCol)
{
    qDebug() << QString("\nsaveTanImg: %1\n").arg(fName);
	QString mLine;
	int i, j, k, cSz, sz;
	int maxCol = 0;
	QFile fM(fName);
        bool isOpen = fM.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
	if(isOpen)
	{
		QTextStream out(&fM);
		sz = marks.size();
		
		QStringList colList = mCol.split(",", QString::SkipEmptyParts);
		cSz = colList.size();
		qDebug() << "\ncSz " << cSz << "\n";
                if(cSz>5) cSz = 5;
		int *iCol = new int[cSz];
		for(i=0; i<cSz; i++)
		{
			iCol[i] = colList[i].toInt()-1;
			if(iCol[i]>maxCol) maxCol = iCol[i];
			qDebug() << "\niCol[" << i << "] = " << iCol[i] << "\n";
		}
		qDebug() << "\nmaxCol" << maxCol << "\n";
		double val;
                double P0, P1;
	
		for(i=0; i<sz; i++)
		{
			mLine.clear();
                        //qDebug() << QString("i:%1\n").arg(i);
                        //qDebug() << QString("mTanImg: %1\t%2\t%3\n").arg(marks[i]->mTanImg[0]).arg(marks[i]->mTanImg[1]).arg(marks[i]->mTanImg[2]);
                        //qDebug() << "P: " << marks[i]->P;
                        if(marks[i]->P==NULL)
                        {
                            P0 = 0;
                            P1 = 0;
                        }
                        else
                        {
                            P0 = marks[i]->P[19];
                            P1 = marks[i]->P[3];
                        }

			for(j=0;j<maxCol+1;j++)
			{
				val = 0;
                                for(k=0; k<cSz; k++)
                                {
                                    //qDebug() << QString("j:iCol[k]\t%1:%2\n").arg(j).arg(iCol[k]);


                                    if(j==iCol[k])
                                    {
                                        if(k==3)
                                        {
                                            val = P0;

                                        }
                                        else if(k==4)
                                        {
                                            val = P1;

                                        }
                                        else val = marks[i]->mTanImg[k];
                                        break;
                                    }
                                }
                               // qDebug() << QString("%1:%2\t%3\n").arg(j).arg(k).arg(val);
				mLine.append(QString("%1%2").arg(val, 12, 'f').arg(mSep));
			}
                        //qDebug() << "\n" << mLine << "\n";
//			out <<  QString("%1 %2 %3\n").arg(marks[i]->mTanImg[0], 12, 'f').arg(marks[i]->mTanImg[1], 12, 'f').arg(marks[i]->mTanImg[2], 12, 'f');
			out << mLine << "\n";
		}
		
		fM.close();
	}
	else qDebug() << "\nFile " << fName << " is not open\n";
}

int marksGrid::loadTanImg(QString fName, QString mSep, QString mCol)
{
	qDebug() << "\nloadTanImg\n";
	qDebug() << "\ncfName " << fName << "\n";
        qDebug() << QString("mCol: %1\tmSep: %2\n").arg(mCol).arg(mSep);
	int i, j, cSz;
        QStringList mLineParts;
	QFile fM(fName);
	marksP* mks;
	QString mLine, mVal;
	bool isOpen = fM.open(QIODevice::ReadOnly | QIODevice::Truncate | QIODevice::Text);
        if(isOpen)
	{
		QTextStream fin(&fM);
                //clearMarks();

		QStringList colList = mCol.split(",", QString::SkipEmptyParts);
		cSz = colList.size();
        //	qDebug() << "\ncSz " << cSz << "\n";
                if(cSz>5) cSz = 5;
		int *iCol = new int[cSz];
		for(i=0; i<cSz; i++)
		{
			iCol[i] = colList[i].toInt()-1;
 //                       qDebug() << "\niCol[" << i << "] = " << iCol[i] << "\n";
		}

//		int sz = marks.size();
		while(!fin.atEnd())
		{
       //             qDebug() << QString("marks.size= %1\n").arg(marks.size());
			mLine = fin.readLine();

                        //mks->mTanImg = new double[3];
                        mLineParts = mLine.split(" ", QString::SkipEmptyParts);
                        if(mLineParts.size()<3||mLineParts.size()>5) continue;
                        mks = new marksP();
             //           if(mks==NULL) qDebug() << "NULL\n";
             //           else qDebug() << "mks " << mks << "\n";
                        for(j=0; j<3; j++)
			{
   //                         qDebug() << QString("j= %1: iCol[j] %2\n").arg(j).arg(iCol[j]);

				mVal = mLine.section(mSep, iCol[j], iCol[j], QString::SectionSkipEmpty);
                                //qDebug() << mVal.toDouble() << "\n";

                                if(j==2) mks->mTanImg[j] = log(mVal.toDouble());
                                else mks->mTanImg[j] = mVal.toDouble();

			}
                        mks->P = new double[21];
                        mks->P[18] = mks->mTanImg[2];
              //          qDebug() << QString("P[18] = %1\n").arg(mks->P[18]);
                        if(cSz==5)
                        {
                            mVal = mLine.section(mSep, iCol[3], iCol[3], QString::SectionSkipEmpty);
              //              qDebug() << mVal.toDouble() << "\t";
                            mks->P[19] = mVal.toDouble();
                            mVal = mLine.section(mSep, iCol[4], iCol[4], QString::SectionSkipEmpty);
               //             qDebug() << mVal.toDouble() << "\t";
                            mks->P[3] = mVal.toDouble();
                        }
                        //qDebug() << "\n";

                        //qDebug() << QString("mTanImg: %1\t%2\t%3\n").arg(mks->mTanImg[0]).arg(mks->mTanImg[1]).arg(mks->mTanImg[2]);
                        //marks << mks;
                        addMark(mks);
                        //delete mks;
		}
         //       qDebug() << "fin = atEnd\n";
		
		
		fM.close();



	}
	else qDebug() << "\nFile " << fName << " is not open\n";

        qDebug() << QString("marks num: %1\n").arg(marks.size());
	
	return 0;	
}

void marksGrid::moveToCorner(long *naxes)
{
    long *nxs = new long[2];

//    detNaxes(&nxs[0], &nxs[1]);
    nxs[0] = naxes[0]/2;
    nxs[1] = naxes[1]/2;

   // double d0, d1;
    int i, sz;
    sz = marks.size();

    for(i=0; i<sz; i++)
    {
        marks.at(i)->mTanImg[0] += nxs[0];
        marks.at(i)->mTanImg[1] += nxs[1];
    }


}

void marksGrid::copy(marksGrid* source)
{
    marks.clear();
    int i, sz;
    ctype = source->ctype;
    sz = source->marks.size();
    qDebug() << "copy marks\n";
    for(i=0; i<sz; i++)
    {
        addMark(source->marks.at(i));
    }
}

void marksGrid::remMark(int i)
{
        marksP *cMark = marks.at(i);
        //delete cMark;
	marks.removeAt(i);

	num--;
}

long marksGrid::detNaxes(long *naxes0, long *naxes1)
{
	double max[2];
	
	max[0] = max[1] = 0;
	
	for(int i=0; i<marks.size(); i++)
	{
		if(max[0]<marks[i]->mTanImg[0]) max[0] = marks[i]->mTanImg[0];
		if(max[1]<marks[i]->mTanImg[1]) max[1] = marks[i]->mTanImg[1];
	}
	qDebug() << "\nmax naxes\n";
	qDebug() << max[0] << "\t" << max[1] << "\n";

	if(naxes0!=NULL) *naxes0 = max[0];
	if(naxes1!=NULL) *naxes1 = max[1];
	if(max[0]>max[1]) return (long)max[0];
	return (long)max[1]; 
}

int marksGrid::detRect(QRect *iRect)
{
    qDebug() << "marksGrid::detRect\n";
        iRect->setCoords(DEFAULTIMAGE_X, DEFAULTIMAGE_Y, -DEFAULTIMAGE_X, -DEFAULTIMAGE_Y);

        for(int i=0; i<marks.size(); i++)
        {
            if(iRect->left()>marks[i]->mTanImg[0]) iRect->setLeft(marks[i]->mTanImg[0]);
                if(iRect->right()<marks[i]->mTanImg[0]) iRect->setRight(marks[i]->mTanImg[0]);

                if(iRect->top()>marks[i]->mTanImg[1]) iRect->setTop(marks[i]->mTanImg[1]);
                if(iRect->bottom()<marks[i]->mTanImg[1]) iRect->setBottom(marks[i]->mTanImg[1]);

        }
        qDebug() << QString("iRect: %1\t%2\t%3\t%4\n").arg(iRect->left()).arg(iRect->top()).arg(iRect->right()).arg(iRect->bottom());

        return 0;
}

void marksGrid::detOCmean(double *oc0, double *oc1)
{

    int sz = marks.size();

    double x, y;
    x = y = 0;

    for(int i=0; i<sz; i++)
    {
        x += marks.at(i)->mEkv[0];
        y += marks.at(i)->mEkv[1];
    }
    *oc0 = x/(sz*1.0);
    *oc1 = y/(sz*1.0);
}

int marksGrid::getNum()
{
    return marks.size();
}

void marksGrid::detXYmean(double *xMean, double *yMean)
{

    int sz = marks.size();

    double x, y;
    x = y = 0;

    for(int i=0; i<sz; i++)
    {
        x += marks.at(i)->mTanImg[0];
        y += marks.at(i)->mTanImg[1];
    }
    *xMean = x/(sz*1.0);
    *yMean = y/(sz*1.0);
}

void marksGrid::sortMagn(int direction)
{
	int sz = marks.size();
	double vMin;
	int pMin;
	int sn = pow(-1, direction);
	
	for(int i=0; i<sz-1; i++)
	{
		pMin = i;
		vMin = marks[i]->mEkv[2];
		for(int j=i+1; j<sz; j++)
		{
			if(vMin*sn>marks[j]->mEkv[2]*sn)
			{
				pMin = j;
				vMin = marks[j]->mEkv[2];
			}
		}
		marks.swap(i, pMin);
	}
}

void marksGrid::sortAper(int direction)
{
	int sz = marks.size();
	double vMin;
	int pMin;
	int sn = pow(-1, direction);
	
	for(int i=0; i<sz-1; i++)
	{
		pMin = i;
		vMin = marks[i]->mTanImg[2];
		for(int j=i+1; j<sz; j++)
		{
			if(sn*vMin>sn*marks[j]->mTanImg[2])
			{
				pMin = j;
				vMin = marks[j]->mTanImg[2];
			}
		}
		marks.swap(i, pMin);
	}
}

void marksGrid::detCenter(double *xc, double *yc)
{
	double sx, sy;
	sx = sy = 0.0;
	int num = marks.size();
	for(int i=0; i<num; i++)
	{
		sx += marks[i]->mTanImg[0];
		sy += marks[i]->mTanImg[1];
	}
	*xc = sx/num;
	*yc = sy/num;
}

void marksGrid::flipH()
{
	double xm, ym;
	detCenter(&xm, &ym);
	int num = marks.size();
	for(int i=0; i<num; i++) marks[i]->mTanImg[0] = 2*xm - marks[i]->mTanImg[0];
}

void marksGrid::flipV()
{
	double xm, ym;
	detCenter(&xm, &ym);
	int num = marks.size();
	for(int i=0; i<num; i++) marks[i]->mTanImg[1] = 2*ym - marks[i]->mTanImg[1];
}

/////////////////////////////////////////


//////////////////insSettings	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

insSettings::insSettings(QString sFname)
{
//	curName = new char[64];
//	strncpy(curName, "\0", 64);
    scy = scx = 0.0;
    isEdt = 0;

    sett = new QSettings(sFname, QSettings::IniFormat);
    QString namesStr = sett->value("telescopes/telescope_list").toString();
    QString scxStr = sett->value("telescopes/scx_list").toString();
    QString scyStr = sett->value("telescopes/scy_list").toString();
    QString fovStr = sett->value("telescopes/fov_list").toString();
    QString rangStr = sett->value("telescopes/rang_list").toString();

    namesList = namesStr.split("|");
    scxList = scxStr.split("|");
    scyList = scyStr.split("|");
    fovList = fovStr.split("|");
    rangList = rangStr.split("|");

    curInst = sett->value("telescopes/activ_telescope").toInt();
    if (curInst<namesList.size())
    {
        curName = namesList[curInst];
        scx = scxList[curInst].toDouble();
        scy = scyList[curInst].toDouble();
        fov = fovList[curInst].toDouble();
        rang = rangList[curInst].toDouble();
    }
}

insSettings::~insSettings()
{
   // delete sett;
}

void insSettings::getNumIns(int num)
{
//	curName.clear();
//	curName.insert(0, namesList[num]);
    if(num<0) return;
    if (num<namesList.size())
    {
        curInst = num;
        sett->setValue("telescopes/activ_telescope", curInst);
        curName = namesList[num];
        scx = scxList[num].toDouble();
        scy = scyList[num].toDouble();
        fov = fovList[num].toDouble();
        rang = rangList[num].toDouble();
        isEdt = 0;
    }
}

void insSettings::getNameIns(char *iname)
{}

int insSettings::getNameIns(QString iname)
{
    int i;
    int res = 1;
    for (i=0; i<namesList.size(); i++)
    {
        if (iname==namesList[i])
        {
//			curName.clear();
//			curName.insert(0, namesList[i]);
            curInst = i;
            sett->setValue("telescopes/activ_telescope", curInst);
            curName = namesList[i];
            scx = scxList[i].toDouble();
            scy = scyList[i].toDouble();
            fov = fovList[i].toDouble();
            rang = rangList[i].toDouble();
//			QMessageBox::information(0, "inf", QString("%1 %2 %3 %4").arg(iname).arg(curName).arg(scx).arg(scy));
            res = 0;
            isEdt = 0;
            break;
        }
    }
    return res;
}

void insSettings::addIns(QString name)
{
    if (getNameIns(name))
    {
        namesList << name;
        scxList << QString("%1").arg (scx, 10, 'f', 5);
        scyList << QString("%1").arg (scy, 10, 'f', 5);
        fovList << QString("%1").arg (fov, 10, 'f', 5);
        rangList << QString("%1").arg (rang, 10, 'f', 5);
        curInst = namesList.size()-1;
        isEdt = 1;
        save();
    }
//	getNameIns(name);
}

void insSettings::delIns(int num)
{
    if (num>namesList.size()-1) return;

    namesList.removeAt(num);
    scxList.removeAt(num);
    scyList.removeAt(num);
    fovList.removeAt(num);
    rangList.removeAt(num);
    if (curInst>namesList.size()-1) curInst>namesList.size()-1;
    save();
    getNumIns(curInst);
}

void insSettings::delIns(QString name)
{}

void insSettings::save()
{
    qDebug() << "\nIsEdt " << isEdt << "\n";
    if (isEdt)
    {
        scxList[curInst] = QString("%1").arg (scx, 10, 'f', 5);
        scyList[curInst] = QString("%1").arg (scy, 10, 'f', 5);
        fovList[curInst] = QString("%1").arg (fov, 10, 'f', 5);
        rangList[curInst] = QString("%1").arg (rang, 10, 'f', 5);

        sett->setValue("telescopes/telescope_list", namesList.join("|"));
        sett->setValue("telescopes/scx_list", scxList.join("|"));
        sett->setValue("telescopes/scy_list", scyList.join("|"));
        sett->setValue("telescopes/fov_list", fovList.join("|"));
        sett->setValue("telescopes/rang_list", rangList.join("|"));
    }
    isEdt = 0;
}

bool findStars(img2d *imgArr, QVector<double>& cmX, QVector<double>&cmY, QVector<double>& flux, int rho, int bsize)
{
    //unsigned short *fd = imgArr->ushD;
    cmX.clear();cmY.clear();flux.clear();
    //int bsize = 100;
    qDebug() << QString("bsize = %1\n").arg(bsize);
    int n = imgArr->naxes[0]/bsize;
    int m = imgArr->naxes[1]/bsize;
    int flag;
    int N=8;
    int XM[N];
    int YM[N];
    int xm,ym;
    double xmean,ymean,ex,ey,D,r;
    qDebug() << QString("n= %1\tm= %2\n").arg(n).arg(m);
    //
    for(int i=0;i<m;i++)
    {
        for(int j=0;j<n;j++)
        {
           for(int k=0;k<N;k++)
            {
               ym = i*bsize;xm =j*bsize;
               for(int y=i*bsize;y<(i+1)*bsize;y++)
                   for(int x=j*bsize;x<(j+1)*bsize;x++)
                       if(imgArr->getImgPix(x, y)>imgArr->getImgPix(xm, ym))
                        {
                            flag=0; for(int q=0;q<k;q++) if((XM[q]==x)&&(YM[q]==y)) flag=1;
                            if(flag==0){xm=x;ym=y;}
                        }
               XM[k] = xm; YM[k] = ym;
               //cmX<<xm;cmY<<ym;
            }
            xmean = 0; ymean = 0;for(int k=0;k<N;k++){xmean+=XM[k]; ymean+=YM[k];}
            xmean = xmean/N; ymean = ymean/N;
            ex = 0; ey = 0;for(int k=0;k<N;k++){ex+=(XM[k]-xmean)*(XM[k]-xmean); ey+=(YM[k]-ymean)*(YM[k]-ymean);}
            D = sqrt(ex/(N-1)+ey/(N-1));
            if(D<2)
            {
                flag=0;
                for(int k=0;k<cmX.count();k++)
                {
                    r=sqrt((xmean-cmX[k])*(xmean-cmX[k])+(ymean-cmY[k])*(ymean-cmY[k]));
                    if(r<bsize/4){cmX[k]=(cmX[k]+xmean)/2; cmY[k]=(cmY[k]+ymean)/2; flag=1; break;}
                }
                if(flag==0){cmX<<xmean;cmY<<ymean;}
            }
            //for(int k=0;){cmX<<xm;cmY<<ym;}
        }
    }
    qDebug() << "begin estimate flux\n";
    int s;
    int nel = 4*rho*rho;
    //double* fda = new double[nel];
    double flu;
    //double cmass[2];
    imgAperture *iApe = new imgAperture(nel);
    STIMG cmass;
    for(int i=0;i<cmX.count();i++)
    {
        imgArr->getAperture(iApe, (int)cmX[i], (int)cmY[i], rho);

        //getAperture(fd,fda, (int)cmX[i], (int)cmY[i], rho, naxes);
        //centerofmass(cmass,fda,rho);
        cmass = CenterOfMas(iApe);
        //qDebug() << QString("cmX= %1\tcmY= %2\ncmass.X= %3\tcmass.Y= %4\n").arg(cmX[i]).arg(cmY[i]).arg(cmass.X).arg(cmass.Y);
        //cmX[i] = (int)cmX[i]+cmass.X-rho;cmY[i] = (int)cmY[i]+cmass.Y-rho;
        cmX[i] = cmass.X;
        cmY[i] = cmass.Y;
        //qDebug() << QString("cmX= %1\tcmY= %2\trho= %3\n").arg(cmX[i]).arg(cmY[i]).arg(rho);
        //
        flu = 0;
        s=0;for(int j=0;j<nel;j++) if(iApe->buffer[j].I!=-10E-9){flu+=iApe->buffer[j].I;s++;}
        flux << flu/s;
    }
    //delete[] fda;
    qDebug() << "end estimate flux\n";
    /*m=0;
    while(m<cmX.count())
    {
       n=0;
        while(n<cmX.count())
        {
            r=sqrt((cmX[m]-cmX[n])*(cmX[m]-cmX[n])+(cmY[m]-cmY[n])*(cmY[m]-cmY[n]));
            if((m!=n)&&(r<bsize/4)){cmX[m]=(cmX[m]+cmX[n])/2; cmY[m]=(cmY[n]+cmY[m])/2;cmX.remove(n);cmY.remove(n);flux.remove(n);}
            else {n++;}
        }
        m++;
    }*/
    int sz0;
    sz0 = cmX.count();
    qDebug() << QString("cmX.count= %1\tcmY.count= %2\tflux.count= %3\n").arg(cmX.count()).arg(cmY.count()).arg(flux.count());
    for(m=sz0-1; m>0; m--)
    {
        for(n=m-1; n>=0; n--)
        {
            //qDebug() << QString("m= %1\tn= %2\n").arg(m).arg(n);
            r=sqrt((cmX[m]-cmX[n])*(cmX[m]-cmX[n])+(cmY[m]-cmY[n])*(cmY[m]-cmY[n]));
            //qDebug() << QString("r= %1\n").arg(r);
            if((m!=n)&&(r<(bsize/4))){cmX[m]=(cmX[m]+cmX[n])/2; cmY[m]=(cmY[n]+cmY[m])/2;cmX.remove(n);cmY.remove(n);flux.remove(n);break;}
        }
        sz0 = cmX.count();
    }
    qDebug() << QString("end findStars\n");
    return true;
}

int TAImUTC::initTmsFile(QString fName)
{
    QFile inFile(fName);
    if(!inFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "tmu file not open\n";
        return 1;
    }
    QTextStream inStm(&inFile);

    tmuStrL = inStm.readAll().split("\n");
    inFile.close();

    return 0;
}

double TAImUTC::getTmu(double jd)
{
    qDebug() << "getTmu\n";
    tmuRec r0, r1;

    int sz, i;

    sz = tmuStrL.size();
    qDebug() << QString("tmuStrL.size: %1\n").arg(sz);
    r1.fromStr(tmuStrL.at(0));
    i=1;
  /*
    for(i=0; i<sz-1; i++)
    {
        r0.fromStr(tmuStrL.at(i));
        r1.fromStr(tmuStrL.at(i+1));
    }
    */
    do
    {
        r0 = r1;
        if(r1.fromStr(tmuStrL.at(i))) break;
        qDebug() << QString("jd= %1\tepoch= %2\n").arg(jd).arg(r1.epoch);
        i++;
    }while((jd>r1.epoch)&&(i<sz));


    return(r0.smul + (jd2mjd(jd)-r0.epoch0)*r0.smul0);

}

int tmuRec::fromStr(QString dstr)
{
    //qDebug() << QString("tmuStr size: %1\n").arg(dstr.size());
    if(dstr.size()!=80) return 1;
    epoch = dstr.mid(17, 9).toDouble();//.section(" ", 4, 4, QString::SectionSkipEmpty);
    smul = dstr.mid(37,11).toDouble();// .section(" ", 6, 6, QString::SectionSkipEmpty);
    epoch0 = dstr.mid(60, 6).toDouble();//.section(" ", 11, 11, QString::SectionSkipEmpty).chop(1);
    smul0 = dstr.mid(70, 9).toDouble();//.section(" ", 4, 4, QString::SectionSkipEmpty);
    return 0;
}

/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////

marksP::marksP()
{
        mEkv = new double[3];
        mTan = new double[2];
        mTanImg = new double[3];
        mTanImg[0] = mTanImg[1] = mTanImg[2] = 0;
        mPpix = NULL;
        //objdata = NULL;
        data = new double[11];
        ocObj = NULL;
        resRec = NULL;
        u3Rec = NULL;
        usnobRec = NULL;
        P = NULL;
        mpcObj = NULL;
        sstar = NULL;
        ssResRec = NULL;
        sbot = NULL;
        ppmxlRec = NULL;
        /*
        mPpix = new imgAperture();
        objdata = new double[18];
        sdata = new double[14];
        ocObj = new ocRec;
        resRec = new residualsRec;
        P = new double[21];*/
}

marksP::marksP(int oType)
{
    mEkv = new double[3];
    mTan = new double[2];
    mTanImg = new double[3];
    mTanImg[0] = mTanImg[1] = mTanImg[2] = 0;
    mPpix = NULL;
    //objdata = NULL;
    data = new double[11];
    ocObj = NULL;
    resRec = NULL;
    u3Rec = NULL;
    usnobRec = NULL;
    P = NULL;
    mpcObj = NULL;
    sstar = NULL;
    objType = oType;
    sbot = NULL;
    ssResRec = NULL;
    ppmxlRec = NULL;
    switch(objType)
    {
    case OBJ_TYPE_UCAC3:
        u3Rec = new ucac3Rec;
        break;
    case OBJ_TYPE_MPEPH:
        mpcObj = new mpephRec;
        break;
    case OBJ_TYPE_SSTAR:
        sstar = new sscat_rec;
        break;
    case OBJ_TYPE_SKYBOT:
        sbot = new skybotRec;
        break;
    case OBJ_TYPE_USNOB:
        usnobRec = new usnoRec;
        break;
    case OBJ_TYPE_PPMXL:
        ppmxlRec = new ppmxl_rec;
        break;
    }
}

marksP::marksP(marksP *sMark)
{
    marksP();
    copy(sMark);
}
/*
void marksP::copy(const marksP &source)
{
    int i;
    mEkv[0] = source.mEkv[0];
    mEkv[1] = source.mEkv[1];
    mEkv[2] = source.mEkv[2];

    mTan[0] = source.mTan[0];
    mTan[1] = source.mTan[1];

    mTanImg[0] = source.mTanImg[0];
    mTanImg[1] = source.mTanImg[1];
    mTanImg[2] = source.mTanImg[2];

    objType = source.objType;
    catName = source.catName;
    catMagName = source.catMagName;

    if(source.mPpix!=NULL)
    {
        if(this->mPpix==NULL) this->mPpix = new imgAperture(source.mPpix->Size);
        source.mPpix->copyTo(this->mPpix);
    }
    if(source.ocObj!=NULL)
    {
        if(ocObj==NULL) ocObj = new ocRec;
        ocObj->copy(*source.ocObj);
    }
    if(source.resRec!=NULL)
    {
        if(resRec==NULL) resRec = new residualsRec;
        resRec->copy(*source.resRec);
    }

    if(source.u3Rec!=NULL)
    {
        if(u3Rec==NULL) u3Rec = new ucac3Rec;
        u3Rec->copy(*source.u3Rec);
    }

    if(source.sstar!=NULL)
    {
        if(sstar==NULL) sstar = new sscat_rec;
        source.sstar->copyTo(sstar);
    }
    if(source.ssResRec!=NULL)
    {
        if(ssResRec==NULL) ssResRec = new sstarRes;
        ssResRec->copy(*source.ssResRec);
    }
    if(source.sbot!=NULL)
    {
        if(sbot==NULL) sbot = new skybotRec;
        sbot->copy(*source.sbot);
    }
    if(source.P!=NULL)
    {
        if(P==NULL) P = new double[21];
        for(i=0; i<21; i++) P[i] = source.P[i];
    }
    if(source.usnobRec!=NULL)
    {
        if(usnobRec==NULL) usnobRec = new usnoRec;
        usnobRec->copy(*source.usnobRec);
    }
}
*/
void marksP::copy(const marksP *source)
{
    int i;
    mEkv[0] = source->mEkv[0];
    mEkv[1] = source->mEkv[1];
    mEkv[2] = source->mEkv[2];

    mTan[0] = source->mTan[0];
    mTan[1] = source->mTan[1];

    mTanImg[0] = source->mTanImg[0];
    mTanImg[1] = source->mTanImg[1];
    mTanImg[2] = source->mTanImg[2];

    objType = source->objType;
    catName = source->catName;
    catMagName = source->catMagName;
    if(source->data!=NULL)
    {
        if(data==NULL) data = new double[11];
        for(i=0; i<11; i++) data[i] = source->data[i];
    }

    if(source->mPpix!=NULL)
    {
        if(this->mPpix==NULL) this->mPpix = new imgAperture(source->mPpix->Size);
        source->mPpix->copyTo(this->mPpix);
    }
    if(source->ocObj!=NULL)
    {
        if(ocObj==NULL) ocObj = new ocRec;
        ocObj->copy(*source->ocObj);
    }
    if(source->resRec!=NULL)
    {
        if(resRec==NULL) resRec = new residualsRec;
        resRec->copy(*source->resRec);
    }

    if(source->u3Rec!=NULL)
    {
        if(u3Rec==NULL) u3Rec = new ucac3Rec;
        u3Rec->copy(*source->u3Rec);
    }

    if(source->sstar!=NULL)
    {
        if(sstar==NULL) sstar = new sscat_rec;
        source->sstar->copyTo(sstar);
    }
    if(source->mpcObj!=NULL)
    {
        if(mpcObj==NULL) mpcObj = new mpephRec;
        source->mpcObj->copyTo(mpcObj);
    }
    if(source->ssResRec!=NULL)
    {
        if(ssResRec==NULL) ssResRec = new sstarRes;
        ssResRec->copy(*source->ssResRec);
    }
    if(source->sbot!=NULL)
    {
        if(sbot==NULL) sbot = new skybotRec;
        sbot->copy(*source->sbot);
    }
    if(source->P!=NULL)
    {
        if(P==NULL) P = new double[21];
        for(i=0; i<21; i++) P[i] = source->P[i];
    }
    if(source->usnobRec!=NULL)
    {
        if(usnobRec==NULL) usnobRec = new usnoRec;
        usnobRec->copy(*source->usnobRec);
    }

    if(source->ppmxlRec!=NULL)
    {
        if(ppmxlRec==NULL) ppmxlRec = new ppmxl_rec;
        ppmxlRec->copy(*source->ppmxlRec);
    }
}
/*
void marksP::copyImg(const marksP &source)
{
    mTanImg[0] = source.mTanImg[0];
    mTanImg[1] = source.mTanImg[1];
    mTanImg[2] = source.mTanImg[2];

    if(source.P!=NULL)
    {
        if(P==NULL) P = new double[21];
        for(int i=0; i<21; i++) P[i] = source.P[i];
    }
}
*/
void marksP::copyImg(const marksP *source)
{
    mTanImg[0] = source->mTanImg[0];
    mTanImg[1] = source->mTanImg[1];
    mTanImg[2] = source->mTanImg[2];

    if(source->P!=NULL)
    {
        if(P==NULL) P = new double[21];
        for(int i=0; i<21; i++) P[i] = source->P[i];
    }
}

marksP::~marksP()
{
        delete [] mEkv;
        delete [] mTan;
        delete [] mTanImg;
 //       //qDebug() << QString("mPpix ") << mPpix << "\n";
        if(mPpix!=NULL) delete mPpix;
        ////qDebug() << QString("sdata ") << sdata << "\n";
        if(data!=NULL) delete [] data;
        //////qDebug() << QString("objdata") << objdata << "\n";
        //if(objdata!=NULL) delete [] objdata;
        ////qDebug() << QString("P ") << P << "\n";
        if(P!=NULL) delete [] P;
        //qDebug() << QString("ocObj ") << ocObj << "\n";
        if(mpcObj!=NULL) delete mpcObj;
        if(ocObj!=NULL) delete ocObj;
        //qDebug() << QString("resRec ") << resRec << "\n";
        if(resRec!=NULL) delete resRec;
        //qDebug() << QString("u3Rec ") << u3Rec << "\n";
        if(u3Rec!=NULL) delete u3Rec;
        //qDebug() << QString("sstar ") << sstar << "\n";
        if(sstar!=NULL) delete sstar;
        //qDebug() << QString("ssResRec ") << ssResRec << "\n";
        if(ssResRec!=NULL) delete ssResRec;
        //qDebug() << QString("sbot ") << sbot << "\n";
        if(sbot!=NULL) delete sbot;
        //qDebug() << QString("mpcObj ") << mpcObj << "\n";
        //if(mpcObj!=NULL) delete mpcObj;
        if(usnobRec!=NULL) delete usnobRec;
        if(ppmxlRec!=NULL) delete ppmxlRec;

        objType = -1;
}

marksP& marksP::operator=(const marksP &source)
{
    copy(&source);

    return *this;
}

void marksP::createP()
{
    P = new double[21];
}


////////////////////////////////////////////////



int fitsdata::ruler3(QString iniFile, QString resFolder, refractionParam *refParam, sysCorrParam *sysCorr)
{
        qDebug() << QString("\nruler3\n\n");

        qDebug() << QString("resFolder: %1\n").arg(resFolder);

        double ra_c,de_c;
        QVector<int> rsindex;
        QString mTimeCode, outstr;
        int szRef, szObj;
        int selNum;
        int i, sz;
        QTime cT;
        marksP *mObj;
        int ko;

        QString obscode = QString(obsPos->num);

        //BEGIN settings
        QSettings *settings = new QSettings(iniFile, QSettings::IniFormat);

//[general]
        int objSepRed = settings->value("general/objSepRed", 0).toInt();

//[reduction]
        reductionParams redParam;
        redParam.redType = settings->value("reduction/redType", 0).toInt();
        redParam.maxres = settings->value("reduction/maxres", 500).toDouble();//maximum of residuals for positional reduction in mas
        redParam.maxresMAG = settings->value("reduction/maxresMAG", 30).toDouble();//maximum of residuals for photometric reduction
        redParam.sigma=settings->value("reduction/sigma", 3.0).toDouble();
        redParam.sMax = settings->value("reduction/sMax", 500).toDouble();
        redParam.minRefStars = settings->value("reduction/minRefStars", 5).toDouble();
        redParam.weights = settings->value("reduction/weights", 0).toInt();
        redParam.uweMax = settings->value("reduction/uweMax", 500).toDouble();
        redParam.maxRefStars = settings->value("reduction/maxRefStars", -1).toInt();

//[outputLimits]

        outputLimits outLim = {};

        outLim.maxOCMAG = settings->value("outputLimits/maxOCMAG", 30).toDouble();//maximum of (O-C) in magnitude for output
        outLim.maxOCRA = settings->value("outputLimits/maxOCRA", 10000).toDouble();//maximum of (O-C) in RA for output
        outLim.maxOCDE = settings->value("outputLimits/maxOCDE", 10000).toDouble();//maximum of (O-C) in DEC for output
        outLim.maxOCMAGrc = settings->value("outputLimits/maxOCMAGrc", 30).toDouble();//maximum of residuals in magnitude for output
        outLim.maxOCRArc = settings->value("outputLimits/maxOCRArc", 10000).toDouble();//maximum of residuals in RA for output
        outLim.maxOCDErc = settings->value("outputLimits/maxOCDErc", 10000).toDouble();//maximum of residuals in DEC for output
        outLim.resSigma = settings->value("outputLimits/resSigma", -1).toDouble();

//[whatOutput] all settings within this section must be equal to 1 if you want to use them

        whatOutput whatOut;
        whatOut.errorreport=settings->value("whatOutput/errorreport", 0).toInt();//make ERRORREPORT
        whatOut.residuals=settings->value("whatOutput/residuals", 0).toInt();//make RESIDUALS report
        whatOut.eqreport=settings->value("whatOutput/eqreport", 0).toInt();
        whatOut.mpcreport=settings->value("whatOutput/mpcreport", 0).toInt();//make MPC report
        whatOut.sstarreport=settings->value("whatOutput/sstarreport", 0).toInt();
        whatOut.objresreport=settings->value("whatOutput/objresreport", 0).toInt();//make ERRORREPORT
        whatOut.namereport=settings->value("whatOutput/namereport", 0).toInt();
        whatOut.ocreject=settings->value("whatOutput/ocreject", 0).toInt();//if ocreject=0 then results with BAD (O-C) (see maxOCMAG, maxOCRA and maxOCDE) will be inluded to output
        whatOut.ipixPos=settings->value("whatOutput/ipixPos", 0).toInt();

//outputParams
        ouputParams outPar;
        outPar.mpcObsType = settings->value("outputParams/mpcObsType", " ").toString();

        //refStarSelector
        QStringList selChain;
        selChain = settings->value("refStarSelector/selChain", "").toString().split(",");

        qDebug() << QString("selChain: %1\n").arg(selChain.join("|"));

        rsSelector0Sett sel0;
        sel0.minMag = settings->value("rsSelector0/minMag", 0).toDouble();
        sel0.maxMag = settings->value("rsSelector0/maxMag", 16).toDouble();
        sel0.minStarsNum = settings->value("rsSelector0/minStarsNum", 0).toInt();

        rsSelector1Sett sel1;
        sel1.cDist = settings->value("rsSelector1/cDist", 0).toDouble();
        sel1.minStarsNum = settings->value("rsSelector1/minStarsNum", 0).toInt();

        rsSelector2Sett sel2;
        sel2.aper = settings->value("rsSelector2/aper", 0).toDouble();
        sel2.minStarsNum = settings->value("rsSelector2/minStarsNum", 0).toInt();

        rsSelector3Sett sel3;
        sel3.magDiap = settings->value("rsSelector3/magDiap", 0).toDouble();
        sel3.minStarsNum = settings->value("rsSelector3/minStarsNum", 0).toInt();

        rsSelector4Sett sel4;
        sel4.muMax = settings->value("rsSelector4/muMax", 0).toDouble();
        sel4.minStarsNum = settings->value("rsSelector4/minStarsNum", 0).toInt();

        rsSelector5Sett sel5;
        sel5.dMax = settings->value("rsSelector5/dMax", 10000).toDouble();
        sel5.minStarsNum = settings->value("rsSelector5/minStarsNum", 0).toInt();
        sel5.maxStarsNum = settings->value("rsSelector5/maxStarsNum", 100).toInt();

        rsSelector6Sett sel6;
        sel6.targEnum = settings->value("rsSelector6/targEnum", 0).toInt();
        sel6.minStarsNum = settings->value("rsSelector6/minStarsNum", 0).toInt();

////
        QStringList allSett;
        allSett << settings->allKeys();
        sz = allSett.size();
        qDebug() << QString("\n\nredParam.ini: %1\n").arg(iniFile);
        for(i=0; i<sz; i++) qDebug() << QString("%1=%2\n").arg(allSett.at(i)).arg(settings->value(allSett.at(i)).toString());
        qDebug() << "\n\n";

//END settings

    ra_c = WCSdata[2];//coordinates of the optical center
    de_c = WCSdata[3];//deg

    qDebug() << "refParam: " << refParam << "\n";

    refractionMaker *refMaker = NULL;

    if(refParam!=NULL) refMaker = new refractionMaker(*refParam);



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        prerDataVect(objMarks, WCSdata[2], WCSdata[3], refMaker, sysCorr);
        prerDataVect(refMarks, WCSdata[2], WCSdata[3], refMaker, sysCorr);

        if(refMarks->marks.size()<redParam.minRefStars)
        {
            qDebug() << QString("ref stars is not enough: %1\tit is necessary: %2\n").arg(refMarks->marks.size()).arg(redParam.minRefStars);
        }

        refMarks->ebRec = new errBudgetRec;
        qDebug() << QString("redMake: %1\n").arg(redParam.redType);
        reductionMaker *redMake = new reductionMaker(redParam.redType);


///////////////////////////////////////////////
            szObj = objMarks->marks.size();

            qDebug() << QString("szObj= %1\n").arg(szObj);
            ko = -1;
            if(szObj>0) mObj = objMarks->marks.at(0);
            else mObj = NULL;
            //qDebug() << "mobj: " << mObj << "\n";

            while(ko<szObj)
            {
                ko++;
                cT.restart();
                for(;cT.elapsed() < 1;);


                qDebug() << QString("ko= %1\n").arg(ko++);

                refMarks->ebRec->clear();
                szRef = refMarks->marks.size();


                rsindex.clear();
                for(i=0; i<szRef; i++) rsindex << i;

                selNum = selChain.size();
                for(i=0; i<selNum; i++)
                {
                    switch(selChain.at(i).toInt())
                    {
                    case 0:
                        rsSelector0(refMarks, rsindex, sel0);
                        break;
                    case 1:
                        rsSelector1(refMarks, rsindex, sel1.cDist, workFrame, WCSdata[0], WCSdata[1]);
                        break;
                    case 2:
                        if((mObj!=NULL)&&objSepRed) rsSelector2(refMarks, rsindex, mObj, sel2.aper);
                        break;
                    case 3:
                        if((mObj!=NULL)&&objSepRed) rsSelector3(refMarks, rsindex, mObj->mEkv[2], sel3.magDiap, sel3.minStarsNum);
                        break;
                    case 4:
                        rsSelector4(refMarks, rsindex, sel4.muMax, sel4.minStarsNum);
                        break;
                    case 5:
                        if((mObj!=NULL)&&objSepRed) rsSelector5(refMarks, rsindex, mObj, sel5.dMax, sel5.minStarsNum, sel5.maxStarsNum);
                        break;
                    case 6:
                        rsSelector6(refMarks, rsindex, sel6.targEnum, sel6.minStarsNum);
                        break;

                    }
                }

                if(rsindex.count()<redParam.minRefStars)
                {
                    qDebug() << QString("\nNumber of reference stars (%1) is less than minRefStarNum (%2). No reduction.\n").arg(rsindex.count()).arg(redParam.minRefStars);
                    //ko++;
                    continue;
                }

    /////////////////////////////////////////////////////////

                if(redMake->makeReduction(mTimeCode, refMarks, rsindex, redParam, refParam))
                {
                    qDebug() << QString("\nReduction error. Skip results\n");
                    continue;
                }
                qDebug() << QString("\nReduction success\n");

    /////////////////////////////////////////////////////////
/*
                if((redMake->RNKSI<redParam.minRefStars)||(redMake->RNETA<redParam.minRefStars))
                {
                    qDebug() << QString("\nWARN: reduction stars num is too small\nrnksi: %1\trneta: %2\n").arg(redMake->RNKSI).arg(redMake->RNETA);
                    //ko++;
                    continue;
                }
                if((grad_to_mas(sqrt(redMake->UWEKSI))>redParam.uweMax)||((grad_to_mas(sqrt(redMake->UWEETA))>redParam.uweMax)))
                {
                    qDebug() << QString("\nWARN: uwe is too hi\nUWEksi: %1\tUWEeta: %2\n").arg(grad_to_mas(sqrt(redMake->UWEKSI))).arg(grad_to_mas(sqrt(redMake->UWEETA)));
                    //ko++;
                    continue;
                }
*/

                qDebug() << QString("mTimeCode: %1").arg(mTimeCode);
                redMake->makeErrorBudget(refMarks->ebRec);


   /////////////////////////////////////////////////////////

                    refMarks->ebRec->MJD = MJD;
                    refMarks->ebRec->RAoc = ra_c;
                    refMarks->ebRec->DEoc = de_c;
                    refMarks->ebRec->originName = fileName;
                    refMarks->ebRec->exptime = exptime;
                    refMarks->ebRec->mesureTimeCode = mTimeCode;
                    refMarks->ebRec->obsCode = obscode;

                    makeErrReports(refMarks, rsindex, redMake, refMarks->ebRec, resFolder, "", outLim,  whatOut, outPar);
                    if(whatOut.ipixPos) makeIpixReports(ipixMarks, redMake, refMarks->ebRec, resFolder, "");

                    //qDebug() << QString("objSepRed: %1\n").arg(objSepRed);
                    if(!objSepRed)
                    {
                        do
                        {
                            if(mObj!=NULL) makeObjErrReports(mObj, redMake, refMarks->ebRec, resFolder, outLim, whatOut, outPar);
                            ko++;
                            if(ko<szObj) mObj = objMarks->marks.at(ko);
                            else mObj=NULL;
                            //qDebug() << "mobj: " << mObj << "\n";
                        }while(ko<szObj);
                        break;
                    }
                    else
                    {
                        if(mObj!=NULL) makeObjErrReports(mObj, redMake, refMarks->ebRec, resFolder, outLim, whatOut, outPar);
                        ko++;
                        if(ko<szObj) mObj = objMarks->marks.at(ko);
                        else mObj=NULL;
                        //qDebug() << "mobj: " << mObj << "\n";
                    }


            };

            qDebug() << "\n\nEnd of ruler3\n\n";

    return 0;
}

int makeObjErrReports(marksP *mObj, reductionMaker *redMake, errBudgetRec* ebRec, QString resFolder, outputLimits outLim, whatOutput whatOut, ouputParams outPar)
{
    qDebug() << "\nmakeObjErrReports\n";
    //qDebug() << "mObj: " << mObj << "\n";
    //qDebug() << "data: " << mObj->data << "\n";

    QString outstr, msgstr;
    QFile oresFile;
    QTextStream orStm;

    objResRec orRec;
    int i, j;
    double *data;
    double x,y,pixmag,ksi,eta;
    double xc,yc,pmag, mag;

    //int rejected = 0;
    QString rejSuff = "";

    data = mObj->data;


    x = data[5];
    y = data[6];
    pixmag = data[7];
    redMake->detTan(&ksi, &eta, x, y);

   //getTangToDeg(&cp[0], &cp[1], ksi, eta, ra_c, de_c);

    redMake->detMagn(&mag, pixmag);

    qDebug() << QString("obj: ksi= %1\tobjdata[2]= %2\t%3 <? %4\n").arg(ksi).arg(data[3]).arg(grad_to_mas(fabs(ksi - data[3]))).arg(outLim.maxOCRA);
    qDebug() << QString("obj: eta= %1\tobjdata[3]= %2\t%3 <? %4\n").arg(eta).arg(data[4]).arg(grad_to_mas(fabs(eta - data[4]))).arg(outLim.maxOCDE);
    qDebug() << QString("obj: mag= %1\tobjdata[8]= %2\n").arg(mag).arg(data[2]);

    //if(!((fabs(mag-objdata[8])<=maxOCMAG)&&(grad_to_mas(fabs(ksi-objdata[2]))<=maxOCRA)&&                        (grad_to_mas(fabs(eta-objdata[3]))<=maxOCDE)))
    if(!((grad_to_mas(fabs(ksi-data[3]))<=outLim.maxOCRA)&&(grad_to_mas(fabs(eta-data[4]))<=outLim.maxOCDE)&&((fabs(mag-data[2])<=outLim.maxOCMAG)||!isfinite(mag)||!isfinite(data[2]))))
    {
        qDebug() << QString("objError too big\n");
        //objMarks->remMark(ko);
        qDebug() << QString("continue\n");
        if(!whatOut.ocreject)return 1;
        else rejSuff="_rejected";
    }

//////////////////////

    //oresFile.clear();
    //orRec = new objResRec;
    orRec.catName = mObj->catName;
    orRec.catMagName = mObj->catMagName;
    orRec.mJD = ebRec->MJD;
    orRec.ksi = ksi;
    orRec.eta = eta;
    orRec.ksiOC = (ksi - data[3]);
    orRec.etaOC = (eta - data[4]);
    orRec.ra = data[0] + orRec.ksiOC*cos(grad_to_rad(data[1]));
    orRec.de = data[1] + orRec.etaOC;
    orRec.mag = mag;

    orRec.ksiOC = grad_to_mas(orRec.ksiOC);
    orRec.etaOC = grad_to_mas(orRec.etaOC);
    orRec.magOC = mag - data[2];
    orRec.x = x;
    orRec.y = y;
    orRec.pixmag = pixmag;

    redMake->detXcYc(&xc, &yc, data[3], data[4]);
    orRec.Dx = (x - xc);
    orRec.Dy = (y - yc);
    redMake->detPmag(&pmag, data[2]);
    orRec.Dpixmag = pixmag - pmag;

    orRec.mesureTimeCode = ebRec->mesureTimeCode;

    if(mObj->P!=NULL) orRec.setMesParams(mObj->P, 21);

//////////////////////

    qDebug() << QString("objType: %1\n").arg(mObj->objType);
    qDebug() << QString("orRec OMC: %1\t%2\n").arg(orRec.ksiOC).arg(orRec.etaOC);



    switch(mObj->objType)
    {
    case OBJ_TYPE_UCAC3:
        break;
    case OBJ_TYPE_MPEPH:
        {
            mObj->ocObj = new ocRec;
            ocRec *oc = mObj->ocObj;

            //qDebug() << "mObj->mpcObj: " << mObj->mpcObj << "\n";
            orRec.name = mObj->mpcObj->name;

            orRec.toOcRec(oc);

            //qDebug() << "toOcRec\n";
            qDebug() << QString("eqRec OMC: %1\t%2\n").arg(oc->ocRaCosDe).arg(oc->ocDe);


            oc->expTime = ebRec->exptime;

            oc->topDist = mObj->mpcObj->topDist;//topocentric distance from theory of motion or catalogue
            oc->muRaCosDe = mObj->mpcObj->muRaCosDe;//mu_ra*cos(dec) [mas/min]
            oc->muDe = mObj->mpcObj->muDe;//mu_de [mas/min]
            oc->Vr = mObj->mpcObj->Vr;//v_r [km/s]
            oc->phase = mObj->mpcObj->phase;//phase [deg]
            oc->elong = mObj->mpcObj->elong;//elongation [deg]
            qDebug() << QString("mTimeCode: %1\n").arg(ebRec->mesureTimeCode);

            //qDebug() << QString("eqreport: %1\n").arg(eqreport);

            if(whatOut.eqreport==1)
            {
                oc->rec2s(&outstr);


                qDebug() << QString("eqFile: %1\n").arg(resFolder+"/"+"eq"+rejSuff+".txt");
                QFile astFile(resFolder+"/"+"eq"+rejSuff+".txt");
                astFile.open(QIODevice::Append| QIODevice::Text);
                QTextStream astStream;
                astStream.setDevice(&astFile);
                astStream << outstr << "\n";
                astFile.close();




                if(whatOut.namereport)
                {
                    qDebug() << QString("Named eqFile: %1\n").arg(resFolder+"/"+mObj->mpcObj->name+"_eq"+rejSuff+".txt");
                    QFile astFile(resFolder+"/"+mObj->mpcObj->name+"_eq"+rejSuff+".txt");
                    astFile.open(QIODevice::Append| QIODevice::Text);
                    QTextStream astStream;
                    astStream.setDevice(&astFile);
                    astStream << outstr << "\n";
                    astFile.close();
                }

            }

            qDebug() << QString("mpcreport: %1\n").arg(whatOut.mpcreport);
            if(whatOut.mpcreport==1)
            {
                outstr ="";
                //int obj_num;
                oc->rec2MPC(&outstr, ebRec->obsCode, mObj->mpcObj->num.toInt(), outPar.mpcObsType);

                qDebug() << QString("mpcFile: %1\n").arg(resFolder+"/"+"mpc"+rejSuff+".txt");
                QFile astFile(resFolder+"/"+"mpc.txt");
                astFile.open(QIODevice::Append| QIODevice::Text);
                QTextStream astStream;
                astStream.setDevice(&astFile);
                astStream << outstr << "\n";
                astFile.close();


                if(whatOut.namereport)
                {

                    msgstr = QString( "%1" ).arg(mObj->mpcObj->name);
                    msgstr = msgstr.trimmed();
                    QFile mpcFile(resFolder+"/"+msgstr+"_mpc"+rejSuff+".txt");
                    mpcFile.open(QIODevice::Append| QIODevice::Text);
                    QTextStream mpcStream;
                    mpcStream.setDevice(&mpcFile);
                    mpcStream << outstr << "\n";
                    mpcFile.close();
                }


            }

            break;
        }
    case OBJ_TYPE_SSTAR:
        {
            mObj->ssResRec = new sstarRes;

            orRec.name = QString(mObj->sstar->getLSPM()).simplified();

            orRec.toSstarRes(mObj->ssResRec);

            mObj->ssResRec->exptime = ebRec->exptime;

            if(whatOut.sstarreport==1)
            {
                mObj->ssResRec->rec2s(&outstr);

                QFile astFile(resFolder+"/sstar"+rejSuff+".txt");
                astFile.open(QIODevice::Append| QIODevice::Text);
                QTextStream astStream;
                astStream.setDevice(&astFile);
                astStream << outstr << "\n";
                astFile.close();

                if(whatOut.namereport)
                {

                    QFile astFile(resFolder+"/"+mObj->ssResRec->lspmName+"_sstar"+rejSuff+".txt");
                    astFile.open(QIODevice::Append| QIODevice::Text);
                    QTextStream astStream;
                    astStream.setDevice(&astFile);
                    astStream << outstr << "\n";
                    astFile.close();
                }


            }

            break;
        }
    case OBJ_TYPE_SKYBOT:
        {
            mObj->ocObj = new ocRec;
            ocRec *oc = mObj->ocObj;

            orRec.name =  mObj->sbot->Name;

            orRec.toOcRec(oc);

            oc->expTime = ebRec->exptime;

            oc->topDist = mObj->sbot->Dg;//topocentric distance from theory of motion or catalogue
            oc->muRaCosDe = mObj->sbot->dRA*1000.0/60.0;//objdata[10];//mObj->sbot->dRA*1000.0/60.0;//mu_ra*cos(dec) [mas/min]
            oc->muDe =mObj->sbot->dDEC*1000.0/60.0;;// objdata[11];//mObj->sbot->dDEC*1000.0/60.0;//mu_de [mas/min]
            oc->Vr = 0.0;//v_r [km/s]
            oc->phase = mObj->sbot->Phase;//phase [deg]
            oc->elong = mObj->sbot->SunElong;//elongation [deg]
            qDebug() << QString("mTimeCode: %1\n").arg(ebRec->mesureTimeCode);

            qDebug() << QString("eqreport: %1\n").arg(whatOut.eqreport);

            if(whatOut.eqreport==1)
            {
                oc->rec2s(&outstr);

                qDebug() << QString("sumAstFile: %1\n").arg(resFolder+"/"+"eq"+rejSuff+".txt");
                QFile astFile(resFolder+"/"+"eq"+rejSuff+".txt");
                astFile.open(QIODevice::Append| QIODevice::Text);
                QTextStream astStream;
                astStream.setDevice(&astFile);
                astStream << outstr << "\n";
                astFile.close();

                if(whatOut.namereport)
                {
                    qDebug() << QString("astFile: %1\n").arg(resFolder+"/"+mObj->sbot->Name+"_eq"+rejSuff+".txt");
                    QFile astFile(resFolder+"/"+mObj->sbot->Name+"_eq"+rejSuff+".txt");
                    astFile.open(QIODevice::Append| QIODevice::Text);
                    QTextStream astStream;
                    astStream.setDevice(&astFile);
                    astStream << outstr << "\n";
                    astFile.close();

                }
            }

            qDebug() << QString("mpcreport: %1\n").arg(whatOut.mpcreport);
            if(whatOut.mpcreport==1)
            {
                outstr ="";
                oc->rec2MPC(&outstr, ebRec->obsCode, mObj->mpcObj->num.toInt(), outPar.mpcObsType);

                qDebug() << QString("sumMpcFile: %1\n").arg(resFolder+"/"+"mpc"+rejSuff+".txt");
                QFile astFile(resFolder+"/"+"mpc"+rejSuff+".txt");
                astFile.open(QIODevice::Append| QIODevice::Text);
                QTextStream astStream;
                astStream.setDevice(&astFile);
                astStream << outstr << "\n";
                astFile.close();

                if(whatOut.namereport==1)
                {
                    msgstr = mObj->sbot->Name;
                    msgstr = msgstr.trimmed();
                    //QMessageBox::information(0,"debug",resFolder+"/"+msgstr+"_mpc.txt",QMessageBox::Ok,0,0);
                    QFile mpcFile(resFolder+"/"+msgstr+"_mpc"+rejSuff+".txt");
                    mpcFile.open(QIODevice::Append| QIODevice::Text);
                    QTextStream mpcStream;
                    mpcStream.setDevice(&mpcFile);
                    mpcStream << outstr << "\n";
                    mpcFile.close();
                }
            }

            break;
        }
    case OBJ_TYPE_USNOB:
        break;
    }

    if(whatOut.objresreport)
    {
        oresFile.setFileName(resFolder+"/objRes"+rejSuff+".txt");
        oresFile.open(QIODevice::WriteOnly | QIODevice::Append);
        orStm.setDevice(&oresFile);

        orRec.rec2s(&outstr);
        orStm << outstr << "\n";

        oresFile.close();
    }

    return 0;
}

int makeErrReports(marksGrid *refMarks, QVector<int> rsindex, reductionMaker *redMake, errBudgetRec* ebRec, QString resFolder, QString suff, outputLimits outLim, whatOutput whatOut, ouputParams outPar)
{
    qDebug() << "\nmakeErrReports\n";
    QString outstr;
    residualFile resFile;
    int resNum;
    int i, j;
    double *data;
    double x,y,pixmag,ksi,eta;
    double xc,yc,pmag, mag;
    QStringList outsl;
    qDebug() << QString("errorreport: %1\n").arg(whatOut.errorreport);
            if(whatOut.errorreport)
            {
                outstr.clear();
                refMarks->ebRec->rec2s(&outstr);

                QFile errFile(resFolder+"/err_budget"+suff+".txt");
                errFile.open(QIODevice::Append| QIODevice::Text);
                QTextStream errStream;
                errStream.setDevice(&errFile);
                errStream << outstr << "\n";
                //errStream << "\n";
                errFile.close();
            }

            qDebug() << QString("residuals: %1\n").arg(whatOut.residuals);
            if(whatOut.residuals)
            {
                outsl.clear();
                //residualFile resData;
                residualsRec* resRec;
                outstr.clear();

                resFile.resList.clear();
                qDebug() << QString("rsindex.count= %1\n").arg(rsindex.count());
                resNum = refMarks->marks.size();
                for (i=0;i<resNum;i++)
                {
                    outstr.clear();
                    //j = rsindex.at(i);
                    data = refMarks->marks.at(i)->data;
                    refMarks->marks.at(i)->resRec = new residualsRec;
                    resRec = refMarks->marks.at(i)->resRec;
                        //sdata = refstars.at(i);
                        x = data[5];
                        y = data[6];
                        pixmag = data[7];
                        redMake->detTan(&ksi, &eta, x, y);

                        redMake->detMagn(&mag, pixmag);
                        if((fabs(mag - data[2])<=outLim.maxOCMAGrc)&&(grad_to_mas(fabs(ksi - data[3]))<=outLim.maxOCRArc)&&(grad_to_mas(fabs(eta - data[4]))<=outLim.maxOCDErc))
                        {
                                redMake->detXcYc(&xc, &yc, data[3], data[4]);
                                //pmag = (sdata[8]-bm)/am;
                                resRec->mJD = ebRec->MJD;
                                resRec->ra = data[0];
                                resRec->de = data[1];
                                resRec->ksi = ksi;
                                resRec->eta = eta;
                                resRec->mag = mag;
                                resRec->ksiOC = grad_to_mas(ksi - data[3]);
                                resRec->etaOC = grad_to_mas(eta - data[4]);
                                resRec->magOC = mag - data[2];
                                resRec->x = x;
                                resRec->y = y;
                                resRec->pixmag = pixmag;
                                resRec->Dx = (x - xc);
                                resRec->Dy = (y - yc);
                                redMake->detPmag(&pmag, data[2]);
                                resRec->Dpixmag = pixmag - pmag;

                                resRec->isRefKsi = 0;
                                resRec->isRefEta = 0;
                                resRec->isRefMag = 0;
                                for(j=0;j<rsindex.count();j++)
                                {
                                    if(i==rsindex[j])
                                    {
                                        resRec->isRefKsi = (int)(redMake->EXCLINDKSI[j]>=0);
                                        resRec->isRefEta = (int)(redMake->EXCLINDETA[j]>=0);
                                        resRec->isRefMag = (int)(redMake->EXCLINDMAG[j]>=0);
                                    }
                                }

                                resRec->mesureTimeCode = ebRec->mesureTimeCode;
                                resRec->catName = refMarks->marks.at(i)->catName;
                                resRec->catMagName = refMarks->marks.at(i)->catMagName;
                                resRec->setMesParams(refMarks->marks.at(i)->P, 21);
                                resRec->rec2s(&outstr);

                                resFile.resList << resRec;
                        }
                }

                if(outLim.resSigma>0.0) resFile.remSigma(outLim.resSigma);
                resFile.saveAs(resFolder+"/residuals"+suff+".txt");

            }
}

int makeIpixReports(marksGrid *ipixMarks, reductionMaker *redMake, errBudgetRec* ebRec, QString resFolder, QString suff)
{
    qDebug() << "\nmakeIpixReports\n";
    QString outstr;
    int resNum;
    int i;
    double ra,de,x,y,pixmag,ksi,eta, mag;
    marksP *mT;

    QFile errFile(resFolder+"/ipixPos"+suff+".txt");
    errFile.open(QIODevice::Append| QIODevice::Text);
    QTextStream errStream;
    errStream.setDevice(&errFile);

    resNum = ipixMarks->marks.size();

    for(i=0; i<resNum; i++)
    {
        outstr.clear();

        mT = ipixMarks->marks.at(i);

        x = mT->mTanImg[0];
        y = mT->mTanImg[1];
        pixmag = mT->mTanImg[2];
        redMake->detTan(&ksi, &eta, x, y);
        getTangToDeg(&ra, &de, ksi, eta, ebRec->RAoc, ebRec->DEoc);
        redMake->detMagn(&mag, pixmag);

        outstr = QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(ebRec->MJD, 15, 'f', 7, QLatin1Char(' ')).arg(ra, 13, 'f', 8, QLatin1Char(' ')).arg(de, 13, 'f', 8, QLatin1Char(' ')).arg(mag, 5, 'f', 2, QLatin1Char(' ')).arg(x, 10, 'f', 4, QLatin1Char(' ')).arg(y, 10, 'f', 4, QLatin1Char(' ')).arg(pixmag, 16, 'e', 8, QLatin1Char(' ')).arg(ebRec->mesureTimeCode);
        errStream << outstr;
    }



}

/////////////////////////////////////


int initCatList(QList <catFinder*> *starCatList, QString catiniFileName)
{
    catFinder *starCat;
    QSettings *sett = new QSettings(catiniFileName, QSettings::IniFormat);
/*
    starCat = new catFinder;
    starCat->exeName = sett->value("ucac2/exeName").toString();
    starCat->exePath = sett->value("ucac2/exePath").toString();
    starCat->catType = sett->value("ucac2/catType").toInt();
    starCat->catName = sett->value("ucac2/catName").toString();
    starCat->catMagName = sett->value("ucac2/catMagName").toString();
    starCat->catPath = sett->value("ucac2/catPath").toString();
    starCatList->append(starCat);
    */
    starCat = new catFinder;
    starCat->exeName = sett->value("cdsfind/exeName").toString();
    starCat->exePath = sett->value("cdsfind/exePath").toString();
    //starCat->catType = sett->value("cdsfind/catType").toInt();
    starCat->catName = sett->value("cdsfind/catName").toString();
    starCat->catMagName = sett->value("cdsfind/catMagName").toString();
    starCat->catPath = sett->value("cdsfind/catPath").toString();
    starCatList->append(starCat);
    //
    starCat = new catFinder;
    starCat->exeName = sett->value("ucac3/exeName").toString();
    starCat->exePath = sett->value("ucac3/exePath").toString();
    //starCat->catType = sett->value("ucac3/catType").toInt();
    starCat->catName = sett->value("ucac3/catName").toString();
    starCat->catMagName = sett->value("ucac3/catMagName").toString();
    starCat->catPath = sett->value("ucac3/catPath").toString();
    starCatList->append(starCat);
    /*
    starCat = new catFinder;
    starCat->exeName = sett->value("lspm/exeName").toString();
    starCat->exePath = sett->value("lspm/exePath").toString();
    starCat->catType = sett->value("lspm/catType").toInt();
    starCat->catName = sett->value("lspm/catName").toString();
    starCat->catMagName = sett->value("lspm/catMagName").toString();
    starCat->catPath = sett->value("lspm/catPath").toString();
    starCatList->append(starCat);
    */
    starCat = new catFinder;
    starCat->exeName = sett->value("lspmFind/exeName").toString();
    starCat->exePath = sett->value("lspmFind/exePath").toString();
    //starCat->catType = sett->value("lspmFind/catType").toInt();
    starCat->catName = sett->value("lspmFind/catName").toString();
    starCat->catMagName = sett->value("lspmFind/catMagName").toString();
    starCat->catPath = sett->value("lspmFind/catPath").toString();
    starCatList->append(starCat);
    //
    //qDebug() << QString("starCatList count: %1\n").arg(starCatList->count());
}



void prerDataVect(marksGrid *mGr, double oc0, double oc1, refractionMaker *refMaker, sysCorrParam *sysCorr)
{
    double tp[2];
    int sz = mGr->marks.size();
    int i;
    marksP *mT;
    double ra, de;
    //double OC0, OC1;
    double dX, dY;
    long ni;
    double *data;



    qDebug() << QString("object num: %1\n").arg(sz);
    qDebug() << "refMaker: " << refMaker << "\n";
    qDebug() << "sysCorr: " << sysCorr << "\n";
    if(refMaker!=NULL)
    {
        refMaker->forvRef(&oc0, &oc1, oc0, oc1);
    }

    for(i=0; i<sz; i++)
    {
        mT = mGr->marks.at(i);

        if(refMaker!=NULL)
        {
            refMaker->forvRef(&ra, &de, mT->mEkv[0], mT->mEkv[1]);
            getDegToTang(&tp[0], &tp[1], ra, de, oc0, oc1);
        }
        else
        {
            getDegToTang(&tp[0], &tp[1], mT->mEkv[0], mT->mEkv[1], oc0, oc1);
        }
        //getDegToTang(&tp[0], &tp[1], ra, de, oc0, oc1);
        //qDebug() << QString("tp: %1\t%2\n").arg(tp[0]).arg(tp[1]);
        if(mT->data==NULL) mT->data = new double[11];
        data = mT->data;
        data[0]= mT->mEkv[0];//RA from theory of motion or catalogue
        data[1]= mT->mEkv[1];//DEC from theory of motion  or catalogue
        data[2]= mT->mEkv[2];
        data[3]= tp[0];//tangential position KSI based on RA (C)
        data[4]= tp[1];//tangential position ETA based on DE (C)

        if(sysCorr!=NULL)
        {
            if(sysCorr->isComaCorr)
            {
                data[3] += sysCorr->comaCorr->dKsi(data[3], data[2]);
                data[4] += sysCorr->comaCorr->dKsi(data[4], data[2]);
            }
        }
        data[5]=mT->mTanImg[0];// - WCSdata[0];//measured pixel position X (O)
        data[6]=mT->mTanImg[1];// - WCSdata[1];//measured pixel position Y (1)

        //qDebug() << "sysCorr: " << sysCorr << "\n";

        if(sysCorr!=NULL)
        {
            //qDebug() << "sysCorr->isVfCorr0: " << sysCorr->isVfCorr0 << "\n";
            if(sysCorr->isVfCorr0)
            {
                if(!sysCorr->vfCorr0->detCorr(&dX, &dY, &ni, data[5], data[6], data[2]))
                {
                    data[5]-= dX;
                    data[6]-= dY;
                }
            }

            //qDebug() << "sysCorr->isMagEqCorr: " << sysCorr->isMagEqCorr << "\n";

            if(sysCorr->isMagEqCorr)
            {
                data[5] -= sysCorr->magEqCorr->corrX(data[5], data[2]);//magEqCorr(mT->mEkv[2], mT->objdata[4], mCoefX, mCoefDeg);
                data[6] -= sysCorr->magEqCorr->corrY(data[6], data[2]);//magEqCorr(mT->mEkv[2], mT->objdata[5], mCoefY, mCoefDeg);
            }

            //qDebug() << "sysCorr->isVfCorr1: " << sysCorr->isVfCorr1 << "\n";

            if(sysCorr->isVfCorr1)
            {
                if(!sysCorr->vfCorr1->detCorr(&dX, &dY, &ni, data[5], data[6], data[2]))
                {
                    data[5]-= dX;
                    data[6]-= dY;
                }
            }

        }

        data[7]= mT->mTanImg[2];//mT->P[18];//log10 of summ of pixels within aperture
        if(mT->P!=NULL) data[8]= mT->P[8];//unut weight error of PSF fitting
        else data[8]= 0.0;
        if(mT->P!=NULL) data[9]=mT->P[9];//unut weight error of PSF fitting
        else data[9]= 0.0;
        if(mT->P!=NULL) data[10]=mT->P[10];//unut weight error of PSF fitting
        else data[10]= 0.0;
        //////////////////////////////////////////////////////
    }

}

//objects   ////////////////////////////////
void getMpephObject(marksGrid *objMarks, QStringList outerArguments, QString ast_eph_prog, QString ast_eph_prog_folder, double mag0, double mag1, int mpeWaitTime)
{
    QProcess outerProcess;
    //QString ast_eph_prog;
    outerProcess.setWorkingDirectory(ast_eph_prog_folder);
    outerProcess.setProcessChannelMode(QProcess::MergedChannels);
    outerProcess.setReadChannel(QProcess::StandardOutput);

    qDebug() << "outerArguments: " << ast_eph_prog << " " << outerArguments.join("|") << "\n";
    outerProcess.start(ast_eph_prog, outerArguments);

    outerProcess.waitForFinished(mpeWaitTime);
    QTextStream objStream(outerProcess.readAllStandardOutput());

    marksP *mT;

    ////
    QString objDataStr;
    mT = new marksP(OBJ_TYPE_MPEPH);
    while (!objStream.atEnd())
    {
        objDataStr = objStream.readLine();
        qDebug() << QString("objDataStr: %1").arg(objDataStr);


        //mT->mpcObj = new mpephRec;
        //mT->objType = 0;
        qDebug() << "mpcObj: " << mT->mpcObj << "\n";
        if(mT->mpcObj->fromString(objDataStr))
        {
            qDebug() << "\nfromString error\n";
            continue;
        }
        if((mT->mpcObj->Vmag<mag0)||(mT->mpcObj->Vmag>mag1))
        {
            qDebug() << QString("\nVmag problem: %1\n").arg(mT->mpcObj->Vmag);
            continue;
        }
        mT->mEkv[0] = mT->mpcObj->ra;//fitsd->objMarks->addEkvMark(ra, de, mag);
        mT->mEkv[1] = mT->mpcObj->de;
        mT->mEkv[2] = mT->mpcObj->Vmag;
        mT->catName = QString("mpeph");
        mT->catMagName = QString("Vmag");

        qDebug() << QString("Object: %1\tra: %2\tde:%3\tmagn:%4\n").arg(mT->mpcObj->name).arg(deg_to_hms(mT->mEkv[0], " ", 5)).arg(deg_to_damas(mT->mEkv[1], " ", 5)).arg(mT->mEkv[2]);

        //objMarks->marks << mT;
        qDebug() << "obj mpc: " << objMarks->addMark(mT)->mpcObj << "\n";
        //mT = new marksP(OBJ_TYPE_MPEPH);
    }

    qDebug() << QString("getMpephObject find %1 objects\n").arg(objMarks->marks.size());

    qDebug() << "end getMpephObject\n";
    //detTanObj();
}

void getMpephNum(marksGrid *objMarks, double MJD, QString mpeNum, QString ast_eph_prog, QString ast_eph_prog_folder, double mag0, double mag1, int mpeWaitTime)
{
    QStringList outerArguments;
    outerArguments << QString("-num");
    outerArguments << mpeNum;
    QString sMjd = QString("%1").arg(MJD, 11, 'f',7);
    outerArguments << QString("%1").arg(sMjd);
    getMpephObject(objMarks, outerArguments, ast_eph_prog, ast_eph_prog_folder, mag0, mag1, mpeWaitTime);
}

void getMpephName(marksGrid *objMarks, double MJD, QString mpeName, QString ast_eph_prog, QString ast_eph_prog_folder, double mag0, double mag1, int mpeWaitTime)
{
    QStringList outerArguments;
    outerArguments << QString("-name");
    outerArguments << mpeName;
    QString sMjd = QString("%1").arg(MJD, 11, 'f',7);
    outerArguments << QString("%1").arg(sMjd);
    getMpephObject(objMarks, outerArguments, ast_eph_prog, ast_eph_prog_folder, mag0, mag1, mpeWaitTime);
}

void getLspmObject(marksGrid *objMarks, double MJD, QString lspmName, QString lspm_prog, QString lspm_prog_folder, QString lspm_cat_file, int lspmWaitTime)
{
    QProcess outerProcess;
    outerProcess.setWorkingDirectory(lspm_prog_folder);
    outerProcess.setProcessChannelMode(QProcess::MergedChannels);
    outerProcess.setReadChannel(QProcess::StandardOutput);

    QStringList outerArguments;
    outerArguments << lspmName << lspm_cat_file;

    qDebug() << "outerArguments: " << lspm_prog << " " << outerArguments.join("|") << "\n";
    outerProcess.start(lspm_prog, outerArguments);

    outerProcess.waitForFinished(lspmWaitTime);
    QTextStream objStream(outerProcess.readAllStandardOutput());

    marksP *mT;

    ////
    QString objDataStr, elS;
    QStringList opers;
    char *strin = new char[1024];
    while (!objStream.atEnd())
    {
        objDataStr = objStream.readLine();
        qDebug() << QString("objDataStr: %1").arg(objDataStr);

        mT = new marksP(OBJ_TYPE_SSTAR);
        sprintf(strin, "%s\0", objDataStr.toAscii().data());
        if(mT->sstar->fromString(strin)) continue;
        qDebug() << QString("pm: %1\t%2\n").arg(mT->sstar->pmRA).arg(mT->sstar->pmDE);
        mT->mEkv[0] = mT->sstar->RAdeg + mas_to_grad(1000*mT->sstar->pmRA*(getYearFromMJD(MJD)-2000));//taking proper motion into account
        mT->mEkv[1] = mT->sstar->DEdeg + mas_to_grad(1000*mT->sstar->pmDE*(getYearFromMJD(MJD)-2000));//taking proper motion into account
        mT->mEkv[2] = mT->sstar->BJmag;
        mT->catName = QString("LSPM");
        mT->catMagName = QString("BJmag");

        qDebug() << QString("Object: %1\tra: %2\tde:%3\tmagn:%4\n").arg(mT->sstar->getLSPM()).arg(mas_to_hms(mT->mEkv[0], " ", 5)).arg(mas_to_damas(mT->mEkv[1], " ", 5)).arg(mT->mEkv[2]);

        objMarks->addMark(mT);
  //      objMarks->marks << mT;
    }

    qDebug() << "end\n";
    //detTanObj();

}

void findLspmCat(marksGrid *objMarks, double ra, double de, double MJD, QString lspm_prog, QString lspm_prog_folder, QString lspm_cat_file, double fov, double mag0, double mag1, int lspmWaitTime)
{
    QProcess outerProcess;
    outerProcess.setWorkingDirectory(lspm_prog_folder);
    outerProcess.setProcessChannelMode(QProcess::MergedChannels);
    outerProcess.setReadChannel(QProcess::StandardOutput);

    QStringList outerArguments;
    outerArguments << deg_to_hms(ra, ":", 3) << deg_to_damas(de,":",2) << QString("r=%1").arg(fov) << QString("%1").arg(lspm_cat_file);

    qDebug() << "outerArguments: " << lspm_prog << " " << outerArguments.join("|") << "\n";
    outerProcess.start(lspm_prog, outerArguments);

    outerProcess.waitForFinished(lspmWaitTime);
    QTextStream objStream(outerProcess.readAllStandardOutput());

    marksP *mT;

    ////
    QString objDataStr;
    char *strin = new char[1024];
    mT = new marksP(OBJ_TYPE_SSTAR);
    while (!objStream.atEnd())
    {
        objDataStr = objStream.readLine();
        qDebug() << QString("\nobjDataStr|%1|\n").arg(objDataStr);

        sprintf(strin, "%s", objDataStr.toAscii().data());
        if(mT->sstar->fromString(strin))
        {
            qDebug() << "wrong string\n";
            continue;
        }

        if((mT->sstar->BJmag<mag0)||(mT->sstar->BJmag>mag1))
        {
            qDebug() << QString("wrong mag interval. BJmag: %1\tobjMag0: %2\tobjMag1:%3\n").arg(mT->sstar->BJmag).arg(mag0).arg(mag1);
            continue;
        }
        qDebug() << QString("pm: %1\t%2\n").arg(mT->sstar->pmRA).arg(mT->sstar->pmDE);
        mT->mEkv[0] = mT->sstar->RAdeg + mas_to_grad(1000*mT->sstar->pmRA*(getYearFromMJD(MJD)-2000));//taking proper motion into account
        mT->mEkv[1] = mT->sstar->DEdeg + mas_to_grad(1000*mT->sstar->pmDE*(getYearFromMJD(MJD)-2000));//taking proper motion into account
        mT->mEkv[2] = mT->sstar->BJmag;
        mT->catName = QString("LSPM");
        mT->catMagName = QString("BJmag");

        qDebug() << QString("Object: %1\tra: %2\tde:%3\tmagn:%4\n").arg(mT->sstar->getLSPM()).arg(deg_to_hms(mT->mEkv[0], " ", 5)).arg(deg_to_damas(mT->mEkv[1], " ", 5)).arg(mT->mEkv[2]);

        objMarks->addMark(mT);
        mT = new marksP(OBJ_TYPE_SSTAR);
    }
    qDebug() << QString("objMarks size: %1\n").arg(objMarks->marks.size());
    qDebug() << "end\n";
//    detTanObj();

}

void findSkybot(marksGrid *objMarks, double ra, double de, double MJD, QString prog, QString progFolder, double fov, QString obsCode, double mag0, double mag1, int sbWaitTime)
{
    QProcess outerProcess;
    outerProcess.setWorkingDirectory(progFolder);
    outerProcess.setProcessChannelMode(QProcess::MergedChannels);
    outerProcess.setReadChannel(QProcess::StandardOutput);

    QStringList outerArguments;
    outerArguments << QString("%1").arg(mjd2jd(MJD), 14, 'f', 5).simplified() << QString("%1").arg(ra, 13, 'f', 8).simplified() << QString("%1").arg(de, 13, 'f', 8).simplified() << QString("%1").arg(fov, 6, 'f', 2).simplified() << obsCode;

    qDebug() << "outerArguments: " << prog << " " << outerArguments.join("|") << "\n";
    outerProcess.start(prog, outerArguments);

    outerProcess.waitForFinished(sbWaitTime);
    QTextStream objStream(outerProcess.readAllStandardOutput());

    marksP *mT;
    ////
    QString objDataStr, elS;
    QStringList opers;
    int headNum = 0;
    mT = new marksP(OBJ_TYPE_SKYBOT);

    while (!objStream.atEnd())
    {
        objDataStr = objStream.readLine();
        qDebug() << QString("objDataStr: %1").arg(objDataStr);
        if(headNum<3)
        {
            headNum++;
            continue;
        }

        if(mT->sbot->fromString(objDataStr)) continue;
        qDebug() << QString("mag0: %1\tMv: %2\tmag1: %3\n").arg(mag0).arg(mT->sbot->Mv).arg(mag1);
        if((mT->sbot->Mv<mag0)||(mT->sbot->Mv>mag1)) continue;
        qDebug() << "mT->mEkv: " << mT->mEkv << "\n";
        mT->mEkv[0] = mT->sbot->RA;// + mas_to_grad(1000*mT->sstar->pmRA*(getYearFromMJD(MJD)-2000));//taking proper motion into account
        mT->mEkv[1] = mT->sbot->DE;// + mas_to_grad(1000*mT->sstar->pmDE*(getYearFromMJD(MJD)-2000));//taking proper motion into account
        mT->mEkv[2] = mT->sbot->Mv;
        mT->catName = QString("skybot");
        mT->catMagName = QString("Mv");

            qDebug() << QString("Object: %1\tra: %2\tde:%3\tmagn:%4\n").arg(mT->sbot->Name).arg(deg_to_hms(mT->mEkv[0], " ", 5)).arg(deg_to_damas(mT->mEkv[1], " ", 5)).arg(mT->mEkv[2]);

        objMarks->addMark(mT);
        mT = new marksP(OBJ_TYPE_SKYBOT);
    }

    qDebug() << QString("findSkybot objects selected num: %1").arg(objMarks->marks.size());

    qDebug() << "end findSkybot\n";
}

void findSkybotNamesList(QStringList *namesList, double ra, double de, double MJD, QString prog, QString progFolder, double fov, QString obsCode, double mag0, double mag1, int sbWaitTime)
{
    QProcess outerProcess;
    outerProcess.setWorkingDirectory(progFolder);
    outerProcess.setProcessChannelMode(QProcess::MergedChannels);
    outerProcess.setReadChannel(QProcess::StandardOutput);
/*
    double dFov = detFov();

    qDebug() << QString("dFov= %1\tfovp= %2\n").arg(dFov).arg(fovp);
    double fov = fovp*dFov*0.01;
*/
    QStringList outerArguments;
    outerArguments << QString("%1").arg(mjd2jd(MJD), 14, 'f', 5).simplified() << QString("%1").arg(ra, 13, 'f', 8).simplified() << QString("%1").arg(de, 13, 'f', 8).simplified() << QString("%1").arg(fov, 6, 'f', 2).simplified() << obsCode;

    qDebug() << "outerArguments: " << prog << " " << outerArguments.join("|") << "\n";
    outerProcess.start(prog, outerArguments);

    outerProcess.waitForFinished(sbWaitTime);
    QTextStream objStream(outerProcess.readAllStandardOutput());

    marksP *mT;
    QString objDataStr, elS;
    QStringList opers;
    char *strin = new char[1024];
    int headNum = 0;
    skybotRec *sbRec = new skybotRec;
    while (!objStream.atEnd())
    {
        objDataStr = objStream.readLine();
        qDebug() << QString("objDataStr: %1").arg(objDataStr);
        if(headNum<3)
        {
            headNum++;
            continue;
        }

        if(sbRec->fromString(objDataStr)) continue;
        if((sbRec->Mv>=mag0)&&(sbRec->Mv<=mag1))
        {
            namesList->append(sbRec->Name);
        }
    }


}


//  selectors


void rsSelector0(marksGrid *refMarks, QVector<int> &rsindex, rsSelector0Sett rss0sett)
{
    qDebug() << QString("refStarSelector0\n");

 //   rsindex.clear();
    //
    int i, j;
    int szRef = rsindex.count();
    double *data;

    qDebug() << QString("szRef before= %1\n").arg(szRef);

    qDebug() << QString("minMag= %1\tmaxMag= %2\tminStarsNum= %3\n").arg(rss0sett.minMag).arg(rss0sett.maxMag).arg(rss0sett.minStarsNum);

    for(i=szRef-1;i>=0 && rsindex.count()>rss0sett.minStarsNum;i--)
    {
       //qDebug() << QString("i: %1\n").arg(i);
       j = rsindex.at(i);
       //qDebug() << QString("j: %1\n").arg(j);
       //qDebug() << "sdata[] " << refMarks->marks.at(j)->sdata << "\n";
       data = refMarks->marks.at(j)->data;
       //qDebug() << QString("sdata[8]= %1\n").arg(sdata[8]);
       if((data[2]<rss0sett.minMag)||(data[2]>rss0sett.maxMag))
       {
           //qDebug() << QString("remove mag: %1\n").arg(sdata[8]);
           rsindex.remove(i);
       }
    }
    qDebug() << QString("szRef after= %1\n").arg(rsindex.count());
}

void rsSelector1(marksGrid *refMarks, QVector<int> &rsindex, double cDist, QRect wFrame, double xc, double yc)
{
    qDebug() << QString("refStarSelector1\n");

    int szRef = rsindex.count();
    int i, j;
    double d0, d1, d2, d3;
    double x, y;

    //wFrame.adjust();
    QRect tFrame(wFrame);
    tFrame.adjust(cDist, cDist, -cDist, -cDist);

    qDebug() << QString("szRef before= %1\n").arg(szRef);
    //qDebug() << QString("naxes: %1\t%2\n").arg(naxes[0]).arg(naxes[1]);

    for(i=szRef-1;i>=0;i--)
    {
       j = rsindex.at(i);
       /*d0 = refMarks->marks.at(j)->mTanImg[0];
       d1 = naxes[0] - refMarks->marks.at(j)->mTanImg[0];
       d2 = refMarks->marks.at(j)->mTanImg[1];
       d3 = naxes[1] - refMarks->marks.at(j)->mTanImg[1];*/
       x = refMarks->marks.at(j)->mTanImg[0];
       y = refMarks->marks.at(j)->mTanImg[1];
       //qDebug() << QString("cDist: %1\ndi: %2\t%3\t%4\t%5\n").arg(cDist).arg(d0).arg(d1).arg(d2).arg(d3);
       if(!tFrame.contains(x, y)) rsindex.remove(i);
       //if((d0<cDist)||(d1<cDist)||(d2<cDist)||(d3<cDist)) rsindex.remove(i);
    }

    qDebug() << QString("szRef after= %1\n").arg(rsindex.count());
}

void rsSelector2(marksGrid *refMarks, QVector<int> &rsindex, marksP *mP, double aper)
{
    qDebug() << QString("refStarSelector2\n");
    int szRef = rsindex.count();
    int i, j;
    double iDist;
    //double *data;
    double objMag = mP->mEkv[2];

    qDebug() << QString("szRef before= %1\n").arg(szRef);
    qDebug() << QString("objMag= %1\n").arg(objMag);

    for(i=szRef-1;i>=0;i--)
    {
       j = rsindex.at(i);
       //data = refMarks->marks.at(j)->data;
       iDist = marksImgDist(mP, refMarks->marks.at(j));
       if(iDist<aper) rsindex.remove(i);
    }

qDebug() << QString("szRef after= %1\n").arg(rsindex.count());
}

void rsSelector3(marksGrid *refMarks, QVector<int> &rsindex, double objMag, double magDiap, int minStarsNum)
{
    qDebug() << QString("refStarSelector3\n");
//    rsindex.clear();

    double mag0, mag1;

    int szRef = rsindex.count();
    int i, j;

    double dMax;
    int posMax;
    double *data;
    double minMag, maxMag;
    //double objMag = mP->mEkv[2];

    qDebug() << QString("szRef before= %1\n").arg(szRef);



    mag0=objMag-magDiap/2.0;
    mag1=objMag+magDiap/2.0;

   qDebug() << QString("objMag= %1\tmag0= %2\tmag1= %3\n").arg(objMag).arg(mag0).arg(mag1);


  //  qDebug() << QString("while= %1").arg((rsindex.count()<minStarsNum)&&(rsindex.count()<szRef));
    while(rsindex.count()>minStarsNum)
    {
        dMax = 0.0;
        posMax = -1;
        minMag = 40.0;
        maxMag = -40.0;
 //       qDebug() << QString("rsindex.count= %1\n").arg(rsindex.count());
        for(i=0; i<rsindex.count(); i++)
        {
           j = rsindex.at(i);
           data = refMarks->marks.at(j)->data;
            if(fabs(data[2]-objMag)>dMax)
            {
                dMax = fabs(data[2]-objMag);
                posMax = i;
            }
            if(data[2]>maxMag) maxMag = data[2];
            if(data[2]<minMag) minMag = data[2];
        }
        if((minMag>=mag0)&&(maxMag<=mag1)) break;
        //qDebug() << QString("posMax= %1\n").arg(posMax);
        if(posMax!=-1) rsindex.remove(posMax);
        else break;
        //
    }
    qDebug() << QString("szRef after= %1\n").arg(rsindex.count());
}

void rsSelector4(marksGrid *refMarks, QVector<int> &rsindex, double muMax, int minRefStars)
{
    qDebug() << QString("refStarSelector4\n");

    int szRef = rsindex.count();
    int i, j;

    double mu;
    double pmra, pmde;
    //double *sdata;
    //double objMag = mP->mEkv[2];

    //qDebug() << QString("objMag= %1\n").arg(objMag);
    qDebug() << QString("szRef before= %1\n").arg(szRef);


    for(i=szRef-1;i>=0;i--, rsindex.size()>minRefStars)
    {
       j = rsindex.at(i);
       //sdata = refMarks->marks.at(j)->sdata;

       switch(refMarks->ctype)
       {
       case UCAC3_CAT_NUM:
           pmra = refMarks->marks.at(j)->u3Rec->pmrac;
           pmde = refMarks->marks.at(j)->u3Rec->pmdc;
           break;
       case USNOB_CAT_NUM:
           pmra = refMarks->marks.at(j)->usnobRec->pmRA;
           pmde = refMarks->marks.at(j)->usnobRec->pmDE;
           break;
       default :
                pmra = 0.0;
                pmde = 0.0;
               break;
       }

       //qDebug() << QString("cType: %1\tpmra= %2\tpmde= %3\n").arg(refMarks->ctype).arg(pmra).arg(pmde);

       mu = sqrt(pow(pmra, 2.0) + pow(pmde, 2.0));
       //qDebug() << QString("mu=%1\tmuMax= %2\n").arg(mu).arg(muMax);
       if(mu>muMax) rsindex.remove(i);
    }

qDebug() << QString("szRef after= %1\n").arg(rsindex.count());
}

void rsSelector5(marksGrid *refMarks, QVector<int> &rsindex, marksP *mP, double dMax, double nMin, double nMax)
{
    qDebug() << QString("refStarSelector5\n");
    int szRef;
    int i, j;
    double dm, iDist;
    int posMax;
    szRef = rsindex.count();

    qDebug() << QString("szRef before= %1\n").arg(szRef);

    for(i=szRef-1;i>=0&&rsindex.count()>nMin;i--)
    {
       j = rsindex.at(i);
       qDebug() << QString("i= %1\tj= %2\n").arg(i).arg(j);
       //sdata = refMarks->marks.at(j)->sdata;
       iDist = marksImgDist(mP, refMarks->marks.at(j));
       qDebug() << QString("iDist= %1\n").arg(iDist);
       if(iDist>dMax) rsindex.remove(i);
    }
    szRef = rsindex.count();
    qDebug() << QString("rsindex.count0= %1\n").arg(szRef);
    while(szRef>nMax)
    {
        posMax = -1;
        dm = 0;
        for(i=0;i<szRef;i++)
        {
            j = rsindex.at(i);
            iDist = marksImgDist(mP, refMarks->marks.at(j));
            if(iDist>dm)
            {
                dm = iDist;
                posMax = i;
            }

        }
        if(posMax!=-1)rsindex.remove(posMax);

        szRef = rsindex.count();
    }

    qDebug() << QString("szRef after= %1\n").arg(rsindex.count());
}

void rsSelector6(marksGrid *refMarks, QVector<int> &rsindex, int targ_us1, int minRefStarsNum)
{
    qDebug() << QString("refStarSelector6\n");

 //   rsindex.clear();
    //
    int i, j;
    int szRef = rsindex.count();

    qDebug() << QString("szRef before= %1\n").arg(szRef);

    for(i=szRef-1;i>=0 && rsindex.count()>minRefStarsNum;i--)
    {
      // qDebug() << QString("i: %1\n").arg(i);
       j = rsindex.at(i);
       //qDebug() << QString("j: %1\n").arg(j);
       //qDebug() << "sdata[] " << refMarks->marks.at(j)->sdata << "\n";
       if(refMarks->marks.at(j)->u3Rec!=NULL)
       {
           if(refMarks->marks.at(j)->u3Rec->us1<targ_us1)
           {
               //qDebug() << QString("us1= %1\tRemove at: %2\n").arg(refMarks->marks.at(j)->u3Rec->us1).arg(j);
               rsindex.remove(i);
           }
       }
       else if(refMarks->marks.at(j)->usnobRec!=NULL)
       {
           if(refMarks->marks.at(j)->usnobRec->Ndet<targ_us1)
           {
               //qDebug() << QString("us1= %1\tRemove at: %2\n").arg(refMarks->marks.at(j)->u3Rec->us1).arg(j);
               rsindex.remove(i);
           }
       }
    }
    qDebug() << QString("szRef after= %1\n").arg(rsindex.count());
}

//////////////////////////////////////////


///////////////////////////////////////////////////////////////////

int initExpList(ExposureList *expList, HeadList headList, obsy *obsPos)
{
    qDebug() << "\ninitExpList\n";
    //expList = new ExposureList;
    expList->exps.clear();

    int i;

    int sz = headList.size();
    QStringList tList;
    int endObs = 0;
    double tObs0, tObs1;
    DATEOBS dateObs0, dateObs1;
    HeadRecord *hRec;
    ExposureRec* eRec;
    double Long;
    qDebug() << "obsPos: " << obsPos << "\n";
    if(obsPos!=NULL) Long = obsPos->Long/(2.0*PI);
    else Long = grad_to_rad(30.3274)/(2.0*PI);//day
    //Long = 0.0;
    double sTime, gm0, gm1, utc, jDay, jdUtc;
    int jDN;

    for(i=0; i<sz; i++)
    {
        hRec = headList.headList.at(i);
        qDebug() << QString("%1: %2 = %3\n").arg(i).arg(hRec->keyName).arg(hRec->keyValue);
        int jdNum;
        double T, s0, s1, dS, dS1, gm2, s2;

        if(hRec->keyName.lastIndexOf("DATE-OBS")==0)
        {
            //objLE->setText(fitsd->headList.at(i)->keyValue);
            //dateStr = QString(fitsd->headList.at(i)->keyValue);
            tList = hRec->keyValue.split(" ");
            dateObs0.year = dateObs1.year = tList.at(0).toInt();
            dateObs0.month = dateObs1.month = tList.at(1).toInt();
            dateObs0.day = dateObs1.day = tList.at(2).toInt();
        }
        else if(hRec->keyName.lastIndexOf("TIME-OBS")==0)
        {
            tList = hRec->keyValue.split(" ");
    //        time = tList.at(0).toInt()/24.0 + tList.at(1).toInt()/24.0/60.0 + tList.at(1).toDouble()/24.0/60.0/60.0;
            if(endObs)
            {
                qDebug() << QString("\nendObs\n");
                /*
                dateObs1.hour = tList.at(0).toInt();
                dateObs1.min = tList.at(1).toInt();
                dateObs1.sec = tList.at(2).toDouble();*/

                sTime = tList.at(0).toInt()/24.0 + tList.at(1).toInt()/1440.0 + tList.at(2).toInt()/86400.0;
                gm1 = sTime - Long;
                dat2JD(&jDay, dateObs0.year, dateObs0.month, dateObs0.day);
                //jDN = dat2JDN(dateObs0.year, dateObs0.month, dateObs0.day);

                //jDN = dat2JDN(dateObs0.year, dateObs0.month, dateObs0.day);
                //T = detT(jDN);
                //s0 = gmst0(T);
                s0 = gmst0jd(jDay);
                s1 = gmst0jd(jDay+1);
                dS = gm1 - s0;
                dS1 = gm1 - s1;
                //qDebug() << QString("dS1= %1\n").arg(dS1);
                //s1 = gmst1(0.0, s0);
                if(dS<0.0) dS +=1.0;
                if(dS<0.5) jDay+=1.0;

                else if(dS<0.5) jDay+=1.0;
                qDebug() << QString("jDay= %1\ts0= %2\ts1= %3\tdS= %4\tdS1= %5\n").arg(jDay, 10, 'f', 4).arg(s0).arg(s1).arg(dS).arg(dS1);
                //if(dS<0.0) dS+=1.0;
                //if(dS<0.5) jDay+=1.0;


                qDebug() << QString("dS= %1\tjDay= %2\n").arg(dS).arg(jDay, 10, 'f', 4);

                GMST1_to_jdUT1(&jdUtc, gm1, jDay);
                qDebug() << QString("jdUTC= %1\tdS= %2\tdS1= %3\n").arg(jdUtc, 10, 'f', 4).arg(dS).arg(dS1);
                if(dS1<0.0)dS1+=1.0;
                qDebug() << QString("dS= %1\tdS1= %2\n").arg(dS).arg(dS1);
                if((dS1>0.5)&&(dS<0.5)) jdUtc-=(1.0-VIU);

                tObs1 = jd2mjd(jdUtc);
                jdUT1_to_GMST1(&gm2, mjd2jd(tObs1));
                s2 = gm1 + Long;
                qDebug() << QString("jDay= %1\tutc= %2\ttObs1= %3\n").arg(jDay, 10, 'f', 3).arg(jdUtc, 10, 'f', 3).arg(tObs1, 10, 'f', 3);
                qDebug() << QString("s2= %1\tsTime= %2\n").arg(s2).arg(sTime);
                //tObs0 = jd2mjd(getJD(dateObs0));
                //tObs1 = jd2mjd(getJD(dateObs1));
                //dat2JD_time(&tObs0, dateObs0.year, dateObs0.month, dateObs0.day, dateObs0.hour, dateObs0.min, dateObs0.sec);
                //dat2JD_time(&tObs1, dateObs1.year, dateObs1.month, dateObs1.day, dateObs1.hour, dateObs1.min, dateObs1.sec);

                qDebug() << QString("tObs0= %1\ttObs1= %2\n").arg(tObs0, 12, 'f', 5).arg(tObs1, 12, 'f', 5);

                if(tObs1<tObs0) tObs1++;

                eRec = new ExposureRec;
                eRec->expVal = (tObs1 - tObs0)*86400.0;
                eRec->expTime = (tObs0+tObs1)/2.0;
                qDebug() << QString("getDATEOBSfromMJD: %1\n").arg(eRec->timeStr());

                expList->exps << eRec;

                endObs = 0;
            }
            else
            {
                qDebug() << QString("\nstartObs\n");
                sTime = tList.at(0).toInt()/24.0 + tList.at(1).toInt()/1440.0 + tList.at(2).toInt()/86400.0;
                gm1 = sTime - Long;
                qDebug() << QString("sTime= %1\tgm1= %2\tLong= %3\n").arg(sTime).arg(gm1).arg(Long);
                dat2JD(&jDay, dateObs0.year, dateObs0.month, dateObs0.day);

                //jDN = dat2JDN(dateObs0.year, dateObs0.month, dateObs0.day);
                //T = detT(jDN);
                //s0 = gmst0(T);
                //s1 = gmst1(0.0, s0);

                s0 = gmst0jd(jDay);
                s1 = gmst0jd(jDay+1);
                dS = gm1 - s0;
                dS1 = gm1 - s1;
                qDebug() << QString("jDay= %1\ts0= %2\ts1= %3\tdS= %4\tdS1= %5\n").arg(jDay, 10, 'f', 4).arg(s0).arg(s1).arg(dS).arg(dS1);
                if(dS<0.0) dS +=1.0;
                if(dS<0.5) jDay+=1.0;

                qDebug() << QString("dS= %1\tjDay= %2\n").arg(dS).arg(jDay, 10, 'f', 4);

                GMST1_to_jdUT1(&jdUtc, gm1, jDay);
                qDebug() << QString("jdUTC= %1\tdS= %2\tdS1= %3\n").arg(jdUtc, 10, 'f', 4).arg(dS).arg(dS1);
                if(dS1<0.0)dS1+=1.0;
                qDebug() << QString("dS= %1\tdS1= %2\n").arg(dS).arg(dS1);
                if((dS1>0.5)&&(dS<0.5)) jdUtc-=(1.0-VIU);



                //dateObs0.hour = utc*24.0;
                //dateObs0.min = tList.at(1).toInt();
                //dateObs0.sec = tList.at(2).toDouble();
                tObs0 = jd2mjd(jdUtc);

                qDebug() << QString("jDay= %1\tutc= %2\ttObs0= %3\n").arg(jDay, 10, 'f', 3).arg(jdUtc, 10, 'f', 3).arg(tObs0, 10, 'f', 3);
                endObs = 1;
            }

        }
    }



}

