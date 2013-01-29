#include <QtCore>
#include "math.h"
#include "./../astro/astro.h"
#include "./../mb/mb.h"
#include "comfunc.h"
//#include "multidim.h"
//#include "sscat.h"
/*
#ifndef FITSDATA_H
#include "fitsdata.h"
#endif
*/

#ifndef REDSTAT_H
#define REDSTAT_H
//////////////////////////////////////////////////////////////////////////////////////


#define OBJ_TYPE_UCAC3 0
#define OBJ_TYPE_MPEPH 1
#define OBJ_TYPE_SSTAR 2
#define OBJ_TYPE_SKYBOT 3
#define OBJ_TYPE_USNOB 4
#define OBJ_TYPE_PPMXL 5
#define OBJ_TYPE_UCAC4 6

class ocRec;
class colRec;
class moveModelRec;
class residualsRec;
class plateConstRec;
class errBudgetRec;
struct measurementRec;
//class ucac2Rec;
class usnoRec;
class ucac3Rec;
struct mpephRec;
struct skybotRec;
struct objEphRec;
class sstarRes;
class eqFile;
class residualFile;
class errBudgetFile;
class reductionStat;
struct measurementStatRec;
struct eqSeriesRec;
class objResRec;
class objResidualFile;
class objResultsRec;

//////////////////////////////////////////////////////////////////////////////////////

//void detPlateName(QString *plateName, QString originName, int plNameType = 0);
int countColStat(colRec *cRec, double *vect, int sz);

void detVersName(QString *versName, QString originName);
void detTimeCode(QString &timeCode);
void detPlateName(QString *plateName, QString originName, int plNameType = 0);
//void getPlateName(QString origName, QString *plName, int plNameType);

void detOrSeriesList(QList <objResRec*> orList, QList <objResidualFile*> *orSeriesList, double expMax, double expMin = 0);
void detErrBSeriesList(QList <errBudgetRec*> ebList, QList <errBudgetFile*> *ebSeriesList, double expMax);
void sortORList(QList <objResRec*> *orList, int dir = 0);
void sortErrBList(QList <errBudgetRec*> *ebList, int dir = 0);
int doSigmaMul(QList <double> valueList, double sigmaMul, double proofP, double *mean, double *rmsOne, double *rmsMean, int *numEnd);
int doWeghts(QList <double> valList, QList <double> weList, double *mean, double *rmsOne, double *rmsMean);


//////////////////////////////////////////////////////////////////////////////////////
class ocRec
{
public:
	double MJday;
        double ra, de;                  //[mas]
        float mag0;                     //[mag]
        double ocRaCosDe, ocDe, ocMag;	//OMC [mas]
/*
        double x, y;            //[pix]
        double pixmag;          //[]
        double Dx, Dy;          //[pix]
        double Dpixmag;         //[]
*/
        //double ra_oc, dec_oc;		//йННПДХМЮРШ НОРХВЕЯЙНЦН ЖЕМРПЮ
        //QString obsCode;
        double topDist;                 //[au]
        double muRaCosDe;                    //mu_ra*cos(dec) [mas/min]
        double muDe;                    //mu_de [mas/min]
        double Vr;                      //v_r [km/s]
        float phase, elong;             //[deg]
	QString name;
	int catNum;						//catalog
        double expTime;                  //[sec]
        QString catName, catMagName;     //name of catalog and name of magnitude param used
        QString mesureTimeCode;          //time of measurements [YYYYMMDDHHMMSSSS]
	
	ocRec();
	~ocRec();
	ocRec(QString str);
	void rec2s(QString *str);
        void rec2sBase(QString *str);
        void rec2sSeries(QString *str);
	void s2rec(QString str);
        void rec2MPC(QString *str, QString obsName, QString objNum, int provNum = 0, QString obsType = " ");

        void copy(const ocRec &source);
        ocRec& operator=(const ocRec &source);
	
        void fromMpeph(mpephRec *meRec);
	
        int vers;                                       //РХО ЯРПНЙХ.
        //0 - MJday|ra|de|mag0|ocRaCosDe|ocDe|ocMag|topDist|muRaCosDec|muDe|Vr|phase|elong|name|catNum|expTime
        //1 - MJday|ra|de|mag0|ocRaCosDe|ocDe|ocMag|ra_oc|de_oc|topDist|muRaCosDec|muDe|Vr|phase|elong|name|catNum|expTime
	
};

class ocRecO : public ocRec
{
public:
    QString obsCode;
    ocRecO();
    void rec2s(QString &str);
    void s2rec(QString str);
};

//ocRec
//MJday		    |ra		         |de		   |mag0    |ocRaCosDe    |ocDe  |ocMag|ra_oc	       |de_oc		  |topDist	       |muRaCosDec	   |muDe     |Vr      |phase   |elong       |name   |catNum|expTime
//2008 05 01.93212|14 26 14.5889|+00 05 12.342|13.279|  -113.6|    28.7|    -1.6|14 26 15.2930|+00 04 53.780| 1.906076352|  -467.40|   219.00|  2.21|   5.420| 164.300|00890|0	   | 180.000

//MJday|ra|de|mag0|ocRaCosDe|ocDe|ocMag|x|y|pixmag|Dx|Dy|Dpixmag|topDist|muRaCosDe|muDe|Vr|phase|elong|expTime|name|catName|catMagName|mesureTimeCode
//2009 01 01.86517|07 58 32.9669|+56 55 15.006|13.883|   169.7|  -284.8|    -0.1|  640.2616|  727.4599|   1.122200e+05|  0.317784|  0.307834|        791.333| 0.032179483| -4032.00|  6960.00|  3.29|  35.130| 143.790|  30.000|        2008 EV5|   mpeph|    Vmag|20110118043313937
/*
    MJday               БПЕЛЪ МЮАКЧДЕМХИ
    ra                  ОПЪЛНЕ БНЯУНФДЕМХЕ
    de                  ЯЙКНМЕМХЕ
    mag0                ГБ. БЕКХВХМЮ
    ocRaCosDe           (O-C) ОН RA * cosDec [mas]
    ocDe                (O-C) ОН Dec [mas]
    ocMag               (O-C) ОН ГБ. БЕКХВХМЕ [mag]
    x                   ХГЛЕПЕММЮЪ ЙННПДХМЮРЮ X [pix]
    y                   ХГЛЕПЕММЮЪ ЙННПДХМЮРЮ Y [pix]
    pixmag              ОХЙЯЕКЭМЮЪ ГБ. БЕКХВХМЮ
    Dx,Dy, Dpixmag      НЬХАЙХ ХГЛЕПЕММШУ ЙННПДХМЮР [pix]
    topDist             РНОНЖЕМРПХВЕЯЙНЕ ПЮЯЯРНЪМХЕ [AU]
    muRaCosDec          БХДХЛЮЪ СЦКНБЮЪ ЯЙНПНЯРЭ ЮЯРЕПНХДЮ ОН Ra * cosDe [mas/min]
    muDe                БХДХЛЮЪ СЦКНБЮЪ ЯЙНПНЯРЭ ЮЯРЕПНХДЮ ОН De [mas/min]
    Vr                  КСВЕБЮЪ ЯЙНПНЯРЭ ЮЯРЕПНХДЮ [km/s]
    phase               ТЮГЮ ЮЯРЕПНХДЮ [deg]
    elong               ЩКНМЦЮЖХЪ [deg]
    expTime             ДКХРЕКЭМНЯРЭ ЩЙЯОНГХЖХХ [sec]
    name                ХЛЪ НАЗЕЙРЮ
    catName             ХЛЪ ЙЮРЮКНЦЮ
    catMagName          ХЛЪ ГБ БЕКХВХМШ Б ЙЮРЮКНЦЕ
    mesureTimeCode      ЙНД ХГЛЕПЕМХЪ

*/

class colRec
{
public:
	int colNum, num;
        double mean, rmsMean, rmsOne;
	
	colRec();
	colRec(QString str);
	
	void rec2s(QString *str);
	void s2rec(QString str);
	
        colRec& operator=(const colRec &source);
};

class moveModelRec
{
public:
        int nPosX, nPosY;
	double Tm, xTm, yTm, uweX, uweY, rmsX, rmsY, Xdot, rmsXdot, XdotOC, Ydot, rmsYdot, YdotOC;
	
	moveModelRec();
	moveModelRec(QString str);
	
	void rec2s(QString *str);
	void s2rec(QString str);

        int fp;         //positions format (0 - RA,DEC in HMS,DMS, 1 - relative positions in arcsec)
        int ft;         //time format (0 - YYYY MM DD.DDDDDD)
        int vflag;      //МЮКХВХЕ ЙНКНМНЙ Я ЩТЕЛЕПХДМШЛХ ГМЮВЕМХЪЛХ ЯЙНПНЯРХ ДБХФЕМХЪ НАЗЕЙРЮ.
        int ndig;
};

//# Tm| X(Tm) | ВХЯКН ОНКНФЕМХИ| UWE - НЬХАЙЮ ЕДХМХЖШ БЕЯЮ|  яйн X(Tm)|XDOT|яйн XDOT|(O-C) ОН XDOT
//# Tm| Y(Tm) | ВХЯКН ОНКНФЕМХИ| UWE - НЬХАЙЮ ЕДХМХЖШ БЕЯЮ|  яйн Y(Tm)|YDOT|яйн YDOT|(O-C) ОН YDOT

//Tm|xTm|yTm|nPos|uweX|uweY|rmsX|rmsY|Xdot|rmsXdot|XdotOC|Ydot|rmsYdot|YdotOC
//#2007 01 17.95120|08 16 58.1650|+10 45 29.429|  4| 231.2| 147.7| 113.6|   -607.97|  33.88|  73.9|    284.14|  22.04
//Tm|xTm|yTm|nPosX|uweX|rmsX|Xdot|rmsXdot|XdotOC|nPosY|uweY|rmsY|Ydot|rmsYdot|YdotOC

//RA [deg]    | DEC [deg]  | ksi      |eta       |mag  |       (O-C)         |   x      |   y      | pixmag   | Dx    | Dy    |Dpixmag  |u2R   |  J   |  H   |  Ks  |OCmag  |c|exptime
//                          [arcsec]   [arcsec]           ksi    eta     mag         pix                          mpix                            mag                             sec
//                                                            [mas]
//345.14222346| 10.56331554| -588.3575| -588.9437|12.92|   38.6|  -34.5| 1.32| -634.2300|  601.6340|  2.312728|   39.6|   37.3|-0.144934|11.600|10.619|10.301|10.266|  1.334|0|  60.000


class residualsRec
{
public:
        double mJD;             //[day]
        double ra, de;          //[deg]
        double ksi, eta;        //[deg]
        double mag;             //[mag]
        double ksiOC, etaOC;    //[mas]
        double magOC;           //[mag]
        double x, y;            //[pix]
        double pixmag;          //[]
        double Dx, Dy;          //[pix]
        double Dpixmag;         //[]
        int isRefKsi, isRefEta, isRefMag;              //Star is used in reduction
 //       double exptime;
        QString catName, catMagName;        //name of catalog and name of magnitude param used
        QString mesureTimeCode;          //time of measurements [YYYYMMDDHHMMSSSS]
        QString mesParams;           //P[21]
	
	residualsRec();
	residualsRec(QString str);
	
	void rec2s(QString *str);
	void s2rec(QString str);

        void copy(const residualsRec &source);

        void setMesParams(double *P, int pSize);
        double catMag();
        double catX();
        double catY();
};


//MJD|RAoc|DECoc|Xoc|Yoc|mesureTimeCode#Nx|Ax|CKO Ax|Bx|CKO Bx|Cx|CKO Cx|...#Ny|Ay|CKO Ay|By|CKO By|Cy|CKO Cy|...#Nm|UWEm|Am|яйн Am|Bm|яйн Bm|...|originName

class plateConstRec
{
public:
    int N;
    double UWE;
    QVector <double> params;
    QVector <double> rms;

    QString toString();
    void fromString(QString source);
    void clear();
    void addParam(double val, double err);
    void setNparams(int Num, double uwe, double *pVect, int pNum);
    void set3params(int Num, double uwe, double A, double rmsA, double B, double rmsB, double C, double rmsC);
    void set2params(int Num, double uwe, double A, double rmsA, double B, double rmsB);
    void copy(const plateConstRec &source);
};

class errBudgetRec
{
public:
        double MJD;
        double RAoc, DEoc;                              //coords of optical center [deg]
        double Xoc, Yoc;                                //coords of optical center [pix]
        int redType;                                    //reduction type (0- 6const)
        QString obsCode;                                //code of observatory
        double exptime;                                 //exposure time [sec]

        plateConstRec xParams, yParams, mParams;
        QString mesureTimeCode;                         //time of measurements [YYYYMMDDHHMMSSSS]
        QString originName;                             //name of reducted source
	
	errBudgetRec();
	errBudgetRec(QString str);

        void clear();
	
	void rec2s(QString *str);
        int s2rec(QString str);

        void copy(const errBudgetRec &source);
};


/*
class ucac2Rec
{
    double ra, dec;             //Ra Dec [mas]


    ucac2Rec();
    ucac2Rec(QString str);

    void rec2s(QString *str);
    void s2rec(QString str);
};
*/

class usnoRec   //usno-b1.0
{
public:
    double r0;                      //_r	(F8.4)	Distance from center (RAJ2000=?, DEJ2000=?) at Epoch=J2000.0	[ucd=pos.angDistance]
    double x0;
    double y0;
    QString usnoName;               //USNO-B1.0	(a12)       Designation of the object (1)	[ucd=meta.id;meta.main]
    QString tychoName;              //Tycho-2	(a12)	Designation in the Tycho-2 Catalog <I/259>	[ucd=meta.id]
    double ra;                             //RAJ2000	(F10.6)	Right Ascension at Eq=J2000, Ep=J2000 (2)	[ucd=pos.eq.ra;meta.main]
    double de;                            //DEJ2000	(F10.6)	Declination at Eq=J2000, Ep=J2000 (2)	[ucd=pos.eq.dec;meta.main]
    double e_Ra;                         //e_RAJ2000	(I3)	Mean error on RAdeg*cos(DEdeg) at Epoch	[ucd=stat.error;pos.eq.ra]
    double e_De;                         //e_DEJ2000	(I3)	Mean error on DEdeg at Epoch	[ucd=stat.error;pos.eq.dec]
    double Epoch;                     //Epoch	(F6.1)	Mean epoch of observation (2)	[ucd=time.epoch;obs]
    double pmRA;                      //pmRA	(I6)	Proper motion in RA (relative to YS4.0)	[ucd=pos.pm;pos.eq.ra]
    double pmDE;                      //pmDE	(I6)	Proper motion in DE (relative to YS4.0)	[ucd=pos.pm;pos.eq.dec]
    double muPr;                       //muPr	(I1)	? Total Proper Motion probability (7)	[ucd=stat.probability]
    double e_pmRA;                  //e_pmRA	(I3)	Mean error on pmRA	[ucd=stat.error;pos.pm;pos.eq.ra]
    double e_pmDE;                  //e_pmDE	(I3)	Mean error on pmDE	[ucd=stat.error;pos.pm;pos.eq.dec]
    double fit_RA;                     //fit_RA	(I1)	Mean error on RA fit	[ucd=stat.error]
    double fit_DE;                     //fit_DE	(I1)	Mean error on DE fit	[ucd=stat.error]
    int Ndet;                             //Ndet	(I1)	[0,5] Number of detections (7)	[ucd=meta.number]
    QString Flags;                    //Flags	(A3)	[MsY.] Flags on object (3)	[ucd=meta.code]
    double B1mag;                   //B1mag	(F5.2)	? First blue magnitude	[ucd=phot.mag;em.opt.B]
    int B1C;                             //B1C	(I1)	? source of photometric calibration (4)	[ucd=instr.calib]
    int B1S;                             //B1S	(I1)	? Survey number (see "\Acat{I/284\#sRM99.1}{Surveys}" section below)	[ucd=meta.id]
    int B1f;                              //B1f	(I3)	? Field number in survey	[ucd=obs.field]
    int B1s_g;                          //B1s/g	(I2)	? Star-galaxy separation (6)	[ucd=src.class.starGalaxy]
    double B1xi;                      //B1xi	(F6.2)	? Residual in X direction (5)	[ucd=stat.fit.residual;pos.eq.ra;arith.diff]
    double B1eta;                    //B1eta	(F6.2)	? Residual in Y direction (5)	[ucd=stat.fit.residual;pos.eq.dec;arith.diff]
    double R1mag;                  //R1mag	(F5.2)	? First red magnitude	[ucd=phot.mag;em.opt.R]
    int R1C;                            //R1C	(I1)	? source of photometric calibration (4)	[ucd=instr.calib]
    int R1S;                            //R1S	(I1)	? Survey number (see "\Acat{I/284\#sRM99.1}{Surveys}" section below)	[ucd=meta.id]
    int R1f;                             //R1f	(I3)	? Field number in survey	[ucd=obs.field]
    int R1s_g;                         //R1s/g	(I2)	? Star-galaxy separation (6)	[ucd=src.class.starGalaxy]
    double R1xi;                     //R1xi	(F6.2)	? Residual in X direction (5)	[ucd=stat.fit.residual;pos.eq.ra;arith.diff]
    double R1eta;                   //R1eta	(F6.2)	? Residual in Y direction (5)	[ucd=stat.fit.residual;pos.eq.dec;arith.diff]
    double B2mag;                 //B2mag	(F5.2)	? Second blue magnitude	[ucd=phot.mag;em.opt.B]
    int B2C;                           //B2C	(I1)	? source of photometric calibration (4)	[ucd=instr.calib]
    int B2S;                           //B2S	(I1)	? Survey number (see "\Acat{I/284\#sRM99.1}{Surveys}" section below)	[ucd=meta.id]
    int B2f;                            //B2f	(I3)	? Field number in survey	[ucd=obs.field]
    int B2s_g;                        //B2s/g	(I2)	? Star-galaxy separation (6)	[ucd=src.class.starGalaxy]
    double B2xi;                     //B2xi	(F6.2)	? Residual in X direction (5)	[ucd=stat.fit.residual;pos.eq.ra;arith.diff]
    double B2eta;                   //B2eta	(F6.2)	? Residual in Y direction (5)	[ucd=stat.fit.residual;pos.eq.dec;arith.diff]
    double R2mag;                 //R2mag	(F5.2)	? Second red magnitude	[ucd=phot.mag;em.opt.R]
    int R2C;                           //R2C	(I1)	? source of photometric calibration (4)	[ucd=instr.calib]
    int R2S;                           //R2S	(I1)	? Survey number (see "\Acat{I/284\#sRM99.1}{Surveys}" section below)	[ucd=meta.id]
    int R2f;                            //R2f	(I3)	? Field number in survey	[ucd=obs.field]
    int R2s_g;                        //R2s/g	(I2)	? Star-galaxy separation (6)	[ucd=src.class.starGalaxy]
    double R2xi;                    //R2xi	(F6.2)	? Residual in X direction (5)	[ucd=stat.fit.residual;pos.eq.ra;arith.diff]
    double R2eta;                  //R2eta	(F6.2)	? Residual in Y direction (5)	[ucd=stat.fit.residual;pos.eq.dec;arith.diff]
    double Imag;                   //Imag	(F5.2)	? Infrared (N) magnitude	[ucd=phot.mag;em.opt.I]
    int IC;                             //IC	(I1)	? source of photometric calibration (4)	[ucd=instr.calib]
    int IS;                             //IS	(I1)	? Survey number (see "\Acat{I/284\#sRM99.1}{Surveys}" section below)	[ucd=meta.id]
    int If;                              //If	(I3)	? Field number in survey	[ucd=obs.field]
    int Is_g;                          //Is/g	(I2)	? Star-galaxy separation (6)	[ucd=src.class.starGalaxy]
    double Ixi;                     //Ixi	(F6.2)	? Residual in X direction (5)	[ucd=stat.fit.residual;pos.eq.ra;arith.diff]
    double Ieta;                    //Ieta	(F6.2)	? Residual in Y direction (5)	[ucd=stat.fit.residual;pos.eq.dec;arith.diff]



    usnoRec();
    usnoRec(QString str);

    void rec2s(QString *str);
    void s2rec(QString str);

    void copy(const usnoRec &source);
};

class abstractCatRec
{
    double ra;              //right ascension at  epoch J2000.0 (ICRS); [ra]=deg
    double dec;             //declination at  epoch J2000.0 (ICRS); [dec]=deg
    double magn;            //magnitude;
    double sigra;           //s.e. at central epoch in RA (*cos Dec); [mas]
    double sigdc;           //s.e. at central epoch in Dec; [mas]
    double sigmag;          //UCAC error on magnitude (larger of sc.mod); [sigmag]=mag
    double cepra;           //central epoch for mean RA, minus 1900; [yr]
    double cepdc;           //central epoch for mean Dec, minus 1900; [yr]
    double pmrac;           //proper motion in RA*cos(Dec); [mas/yr]
    double pmdc;            //proper motion in Dec; [mas/yr]
    double sigpmr;          //s.e. of pmRA * cos Dec; [mas/yr]
    double sigpmd;          //s.e. of pmDec; [mas/yr]
};

class ucac3Rec
{
 public:
    double ra;              //right ascension at  epoch J2000.0 (ICRS); [ra]=deg
    double dec;             //declination at  epoch J2000.0 (ICRS); [dec]=deg
    double im1;             //UCAC fit model magnitude; [im1]=mag
    double im2;             //UCAC aperture  magnitude; [im1]=mag
    double sigmag;          //UCAC error on magnitude (larger of sc.mod); [sigmag]=mag
    int objt;               //object type
    int dsf;                //double star flag

    double sigra;           //s.e. at central epoch in RA (*cos Dec); [mas]
    double sigdc;           //s.e. at central epoch in Dec; [mas]
    int na1;                //total # of CCD images of this star
    int nu1;                //# of CCD images used for this star
    int us1;                //# catalogs (epochs) used for proper motions
    int cn1;                //total numb. catalogs (epochs) initial match

    double cepra;           //central epoch for mean RA, minus 1900; [yr]
    double cepdc;           //central epoch for mean Dec, minus 1900; [yr]
    double pmrac;           //proper motion in RA*cos(Dec); [mas/yr]
    double pmdc;            //proper motion in Dec; [mas/yr]
    double sigpmr;          //s.e. of pmRA * cos Dec; [mas/yr]
    double sigpmd;          //s.e. of pmDec; [mas/yr]

    int id2m;               //2MASS pts_key star identifier
    double jmag;            //2MASS J  magnitude; [mag]
    double hmag;            //2MASS H  magnitude; [mag]
    double kmag;            //2MASS K_s magnitude; [mag]
    int icqflg;             //2MASS cc_flg*10 + phot.qual.flag
    int e2mpho;             //2MASS error photom. (1/100 mag)

    double smB;             //SuperCosmos Bmag; [mag]
    double smR2;            //SC R2mag; [mag]
    double smI;             //SC Imag; [mag]
    int clbl;               //SC star/galaxy classif./quality flag
    int qfB;                //SC quality flag Bmag
    int qfR2;               //SC quality flag R2mag
    int qfI;                //SC quality flag Imag

    int catflg;             //mmf flag for 10 major catalogs matched
    int g1;                 //Yale SPM object type (g-flag)
    int c1;                 //Yale SPM input cat.  (c-flag)
    int leda;               //LEDA galaxy match flag
    int x2m;                //2MASS extend.source flag
    int rn;                 //MPOS star number; identifies HPM stars



    ucac3Rec();
    ucac3Rec(QString str);

    void rec2s(QString *str);
    void s2rec(QString str);
    void s2recCDS(QString strNames, QString strVal);

    void copy(const ucac3Rec &source);
};

class ucac4Rec
{
public:
   double ra, dec;         /* RA/dec at J2000.0,  ICRS,  in milliarcsec */
   double mag1, mag2;     /* UCAC fit model & aperture mags, .001 mag */
   double mag_sigma;
   int obj_type, double_star_flag;
   double ra_sigma, dec_sigma;    /* sigmas in RA and dec at central epoch */
   int n_ucac_total;      /* Number of UCAC observations of this star */
   int n_ucac_used;      /* # UCAC observations _used_ for this star */
   int n_cats_used;      /* # catalogs (epochs) used for prop motion */
   double epoch_ra;        /* Central epoch for mean RA, minus 1900, .01y */
   double epoch_dec;       /* Central epoch for mean DE, minus 1900, .01y */
   double pm_ra;            /* prop motion, .1 mas/yr = .01 arcsec/cy */
   double pm_dec;           /* prop motion, .1 mas/yr = .01 arcsec/cy */
   double pm_ra_sigma;       /* sigma in same units */
   double pm_dec_sigma;
   int twomass_id;        /* 2MASS pts_key star identifier */
   double mag_j, mag_h, mag_k;  /* 2MASS J, H, K_s mags,  in millimags */
   int icq_flag[3];
   double e2mpho[3];          /* 2MASS error photometry (in centimags) */
   double apass_mag[5];      /* in millimags */
   double apass_mag_sigma[5]; /* also in millimags */
   int yale_gc_flags;      /* Yale SPM g-flag * 10 + c-flag */
   int catalog_flags;
   int leda_flag;          /* LEDA galaxy match flag */
   int twomass_ext_flag;   /* 2MASS extended source flag */
   int id_number;
   int ucac2_zone;
   int ucac2_number;

   ucac4Rec();
   ucac4Rec(QString str);
   ~ucac4Rec();
   int fromString(QString tStr);
   void toString(QString &tStr);
   void copy(const ucac4Rec &source);
};

class ppmxl_rec
{
public:
    QString ppmxl_id;       //PPMXL	(a19)	Identifier (5)	[ucd=meta.id;meta.main]
    double ra;              //RAJ2000	(F10.6)	Right Ascension J2000.0, epoch 2000.0	[ucd=pos.eq.ra;meta.main]
    double de;              //DEJ2000	(F10.6)	Declination J2000.0, epoch 2000.0	[ucd=pos.eq.dec;meta.main]
    double pmRA;            //pmRA	(F8.1)	Proper Motion in RA*cos(DEdeg)	[ucd=pos.pm;pos.eq.ra]
    double pmDE;            //pmDE	(F8.1)	Proper Motion in Dec	[ucd=pos.pm;pos.eq.dec]
    double epRA;            //epRA	(F7.2)	Mean Epoch (RA)	[ucd=time.epoch]
    double epDE;            //epDE	(F7.2)	Mean Epoch (Dec)	[ucd=time.epoch]
    double e_RA;            //e_RAJ2000	(I3)	Mean error in RA*cos(DEdeg) at mean epoch (6)	[ucd=stat.error;pos.eq.ra]
    double e_DE;            //e_DEJ2000	(I3)	Mean error in Dec at mean epoch (6)	[ucd=stat.error;pos.eq.dec]
    double e_pmRA;          //e_pmRA	(F4.1)	Mean error in pmRA*cos(DEdeg)	[ucd=stat.error;pos.pm;pos.eq.ra]
    double e_pmDE;          //e_pmDE	(F4.1)	Mean error in pmDec	[ucd=stat.error;pos.pm;pos.eq.dec]
    double Jmag;            //Jmag	(F6.3)	? J magnitude from 2MASS (Cat. II/246)	[ucd=phot.mag;em.IR.J]
    double e_Jmag;          //e_Jmag	(F6.3)	? J total magnitude uncertainty	[ucd=stat.error;phot.mag;em.IR.J]
    double Hmag;            //Hmag	(F6.3)	? H magnitude from 2MASS (Cat. II/246)	[ucd=phot.mag;em.IR.H]
    double e_Hmag;          //e_Hmag	(F6.3)	? H total magnitude uncertainty	[ucd=stat.error;phot.mag;em.IR.H]
    double Kmag;            //Kmag	(F6.3)	? Ks magnitude from 2MASS (Cat. II/246)	[ucd=phot.mag;em.IR.K]
    double e_Kmag;          //e_Kmag	(F6.3)	? Ks total magnitude uncertainty	[ucd=stat.error;phot.mag;em.IR.K]
    double b1mag;           //b1mag	(F5.2)	? B mag from USNO-B, first epoch \ifnum\Vphase>3{\em(in color when the magnitude has a special meaning, see flags)}\fi (1)	[ucd=phot.mag;em.opt.B]
    double b2mag;           //b2mag	(F5.2)	? B mag from USNO-B, second epoch \ifnum\Vphase>3{\em(in color when the magnitude has a special meaning, see flags)}\fi (1)	[ucd=phot.mag;em.opt.B]
    double r1mag;           //r1mag	(F5.2)	? R mag from USNO-B, first epoch \ifnum\Vphase>3{\em(in color when the magnitude has a special meaning, see flags)}\fi (1)	[ucd=phot.mag;em.opt.R]
    double r2mag;           //r2mag	(F5.2)	? R mag from USNO-B, second epoch \ifnum\Vphase>3{\em(in color when the magnitude has a special meaning, see flags)}\fi (1)	[ucd=phot.mag;em.opt.R]
    double imag;            //imag	(F5.2)	? I mag from USNO-B \ifnum\Vphase>3{\em(in color when the magnitude has a special meaning, see flags)}\fi (1)	[ucd=phot.mag;em.opt.I]
    QString Smags;          //Smags	(a5)	[0-8-] Surveys used for USNO-B magnitudes (2)	[ucd=meta.id]
    int No;                 //No	(I2)	? Number of observations used (4)	[ucd=meta.number]
    int fl;                 //fl	(I2)	Flags (3)	[ucd=meta.note]
    QString d2m;            //2M	(A2)	Display the 2MASS data (Cutri et al. 2003, Cat. II/246)	[ucd=meta.ref.url]
    QString Nomad;          //NOMAD	(a5)	Display the NOMAD data (Zacharias et al. 2005, Cat. I/297)	[ucd=meta.ref.url]

    ppmxl_rec();
    ppmxl_rec(QString str);

    void rec2s(QString *str);
    void s2rec(QString str);
    void s2recCDS(QString strNames, QString strVal);

    void copy(const ppmxl_rec &source);
};


//RA[HH:MM:SS.SSSSS]|DEC[signDD:MM:SS.SSSS]|top dist[a.u.]|Vmag|phase[deg]|elongation[deg]|RAmotion*CosDEC[mas/min]|DECmotion[mas/min]|Radial_Velocity[km/s]|MPCnumber

struct mpephRec
{
    double ra, de;          //[deg]
    double topDist;         //[au]
    double Vmag;            //[mag]
    double phase;           //[grad]
    double elong;           //[grad]
    double muRaCosDe, muDe;      //[mas/min]
    double Vr;              //[km/s]

    QString num, name;

    int fromString(QString inStr);
    int fromMiriStr(QString inStr);
    void copyTo(mpephRec *targ);
};

//# Num, Name, RA(h), DE(deg), Class, Mv, Err(arcsec), d(arcsec), dRA(arcsec/h), dDEC(arcsec/h), Dg(ua), Dh(ua), Phase(deg), SunElong(deg), x(au), y(au), z(au), vx(au/d), vy(au/d), vz(au/d), Ref. Epoch(JD)

struct skybotRec
{
    QString Num;            //Num
    QString Name;           //Name
    double RA;              //RA[deg]
    double DE;              //DEC[deg]
    QString Class;          //Class
    double Mv;              //Mv
    double Err;             //Err (arcsec)
    double d;               //d(arcsec)
    double dRA;             //dRA(arcsec/h)
    double dDEC;            //dDEC(arcsec/h)
    double Dg;              //Dg(ua)
    double Dh;              //Dh(ua)
    double Phase;           //Phase(deg)
    double SunElong;        //SunElong(deg)
    double x, y, z;         //x(au), y(au), z(au)
    double vx, vy, vz;      //vx(au/d), vy(au/d), vz(au/d)
    double Epoch;           //Epoch(JD)

    int fromString(QString iStr);
    void toString(QString *oStr);

    void copy(const skybotRec &source);

};

/*
1  	object number (blank if unnumbered)  	-  	x  	x  	x  	x
2 	object name (official or preliminary designation) 	- 	x 	x 	x 	x
3,4 	astrometric J2000 geocentric equatorial coordinates (?, ?) at the given epoch 	degree 	x 	x 	x 	x
5 	class 	- 	x 	x 	x 	x
6 	visual magnitude 	- 	x 	x 	x 	x
7 	error on the position 	arcsec 	x 	x 	x 	x
8 	body-to-center angular distance 	arcsec 	x 	x 	x 	x
9,10 	motion on the celestial sphere (?'cos(?), ?') 	arcsec/h 	  	x 	x 	x
11 	geocentric distance 	AU 	  	x 	x 	x
12 	heliocentric distance 	AU 	  	x 	x 	x
13 	Phase angle 	degree 	  	  	x 	x
14 	Solar elongation 	degree 	  	  	x 	x
15..20 	mean J2000 heliocentric vector position and motion at epoch T0 	AU and AU/d 	  	  	  	x
21 	T0, epoch of the position vector 	Julien Day 	  	  	  	x
*/

struct objEphRec
{
    QString objName;
    QString catName, magCatName;
};

class sstarRes
{
public:
    double mJD;             //[day]
    double ra, de;          //[deg]
    double ksi, eta;
    double mag;     //[mag]
    double ksiOC, etaOC;    //[]
    double magOC;           //[mag]
    double x, y;            //[pix]
    double pixmag;          //[]
    double Dx, Dy;          //[pix]
    double Dpixmag;         //[]
    double exptime;
    QString lspmName;                //LSPM star name (1)
    QString catName, catMagName;     //name of catalog and name of magnitude param used
    QString mesureTimeCode;          //time of measurements [YYYYMMDDHHMMSSSS]

    sstarRes();
    sstarRes(QString str);
    ~sstarRes();

    void rec2s(QString *str);
    int s2rec(QString str);

    void copy(const sstarRes &source);
};

class objResRec
{
public:
    double mJD;                     //[dateobs]
    double ra, de;                  //[hms gms]
    double ksi, eta;                //[deg]
    double mag;                     //[mag]
    double ksiOC, etaOC;            //[mas]
    double magOC;                   //[mag]
    double x, y;                    //[pix]
    double pixmag;                  //[]
    double Dx, Dy;                  //[pix]
    double Dpixmag;                 //[]
    //double exptime;                 //[sec]
    QString name;                   //obj name (1)
    QString catName, catMagName;    //name of catalog and name of magnitude param used
    QString mesureTimeCode;         //time of measurements [YYYYMMDDHHMMSSSS]
    QString mesParams;              //P[21]

    objResRec();
    objResRec(QString str);
    ~objResRec();

    void clear();

    void rec2s(QString *str);
    int s2rec(QString str);

    void setMesParams(double *P, int pSize);

    void copy(const objResRec &source);

    void fromColList(QList <colRec*> colList);

    void toOcRec(ocRec *rec);
    void toSstarRes(sstarRes *rec);
    void toResultsRec(objResultsRec *resRec);
};

class objResultsRec
{
public:
    double mJD;                     //[dateobs]
    double ra, de;                  //[hms gms]
    double ksi, eta;                //[deg]
    double mag;                     //[mag]
    double x, y;                    //[pix]
    double pixmag;                  //[]
    QString name;                   //obj name (1)

    objResultsRec();
    objResultsRec(QString str);
    ~objResultsRec();

    void clear();

    void rec2s(QString *str);
    int s2rec(QString str);

//    void setMesParams(double *P, int pSize);

    void copy(const objResultsRec &source);

//    void fromColList(QList <colRec*> colList);

//    void toOcRec(ocRec *rec);
//    void toSstarRes(sstarRes *rec);
};


////////////////////////////////////////////////////////////

class eqFile
{
public:
	QList <ocRec*> ocList;
	QList <colRec*> colList;
	moveModelRec* mmRec;
	
	int ndig, nfrac;
        //ndig - ВХЯКН ГМЮВЮЫХУ ЖХТП Б БШБНДХЛНЛ ПЕГСКЭРЮРЕ.
        //nfrac - ВХЯКН ГМЮВЮЫХУ ЖХТП ОНЯКЕ ГЮОЪРНИ Б БШБНДХЛНЛ ПЕГСКЭРЮРЕ.
        //vflag - МЮКХВХЕ ЙНКНМНЙ Я ЩТЕЛЕПХДМШЛХ ГМЮВЕМХЪЛХ ЯЙНПНЯРХ ДБХФЕМХЪ НАЗЕЙРЮ.

	
	QString fName;
	

	eqFile();
	~eqFile();
	void init(const char *fname);
	void initOld(const char *fname);
        void save(int saveType = 0);
        void saveAs(QString fName, int saveType = 0);
        //void saveAsMpc(QString fName, QString obsName, int objNum);
	void clear();

        void sortOClist();
	
	int getColRecNum(int colNum);
	int setColRec(colRec* cRec);
	void sortColList();

        colRec* getColNum(int cNum);

        int countCols(QString colNums);        //ЯДЕКЮРЭ НЯПЕДМЕМХЕ ОН ЯРНКАЖЮЛ ocList Я МНЛЕПЮЛХ, ГЮДЮММШЛХ Б ЯРПНЙЕ colNums (ПЮГДЕКХРЕКЭ - ',')
        int reCountCols();                     //ОЕПЕДЕКЮРЭ НЯПЕДМЕМХЕ ОН ЯРНКАЖЮЛ ocList МЮ НЯМНБЕ СФЕ ЯСЫЕЯРБСЧЫХУ colList
        void do3sigma(double proofP, double sigmaMul, QList <ocRec*> *rejList = NULL);                        //ЯДЕКЮРЭ НРАПЮЙНБЙС ОН 3*sigma ДКЪ OMC(ra) Х OMC(dec)
        int countMM(int fp = 0, int ft = 0, int vflag = 1);

        void findSeries(QList <eqFile*> *eqList);
	
        void delMMrec();

        //void getSeriesRec(eqSeriesRec *eqsRec);
        void getSeriesRec(ocRec *eqsRec);

        void removeMes(QString mesureTimeCode);
};

class objResidualFile
{
public:
    QList <objResRec*> ocList;
    //QList <colRec*> colList;
    //moveModelRec* mmRec;

    QString fName;

    objResidualFile();
    ~objResidualFile();

    void init(QString fname);

    void clear();
    void save();
    void saveAs(QString fname);

    void removeMes(QString mesureTimeCode);

    void do3sigma(double proofP, double sigmaMul, QList <objResRec*> *rejList = NULL);

    //int getColRecNum(int colNum);
    //int setColRec(colRec* cRec);
    //void sortColList();

    int getColNum(colRec* cRec, int cNum);

    int countCols(QList <colRec*> *colList, QString colNums);        //ЯДЕКЮРЭ НЯПЕДМЕМХЕ ОН ЯРНКАЖЮЛ ocList Я МНЛЕПЮЛХ, ГЮДЮММШЛХ Б ЯРПНЙЕ colNums (ПЮГДЕКХРЕКЭ - ',')
    int countMoveModel(moveModelRec* mmRec, int fp = 0, int ft = 0, int vflag = 0, int modelDeg = 2);

    //int reCountCols();
    //void delMMrec();

    //void getSeriesRec(objResRec *orsRec);

    /*
    int getColRecNum(int colNum);
    int setColRec(colRec* cRec);
    void sortColList();

    colRec* getColNum(int cNum);

    int countCols(QString colNums);        //ЯДЕКЮРЭ НЯПЕДМЕМХЕ ОН ЯРНКАЖЮЛ ocList Я МНЛЕПЮЛХ, ГЮДЮММШЛХ Б ЯРПНЙЕ colNums (ПЮГДЕКХРЕКЭ - ',')
    int reCountCols();                     //ОЕПЕДЕКЮРЭ НЯПЕДМЕМХЕ ОН ЯРНКАЖЮЛ ocList МЮ НЯМНБЕ СФЕ ЯСЫЕЯРБСЧЫХУ colList
    */
};

class objSeries
{
public:
    objSeries();
    ~objSeries();
    QList <objResidualFile*> serieList;

    QString objName;
    QString catName;
    QString catMagName;

    int saveAs_Full(QString fileName);
    int saveAs_Mean(QString fileName);
    int saveAs_MoveModel(QString fileName);

    void getMean(QList <objResRec*> *ocList);
    void getMoveModel(QList <ocRec*> *ocList);

//    void rec2s(QString *str);
//    void s2rec(QString str);
};

class moveModelFile
{
public:
    QList <moveModelRec*> mmList;

    moveModelFile();
    ~moveModelFile();

    int saveAs(QString fileName);
};

class sstarFile
{
public:
    QList <sstarRes*> ocList;
    QString fName;

    QList <colRec*> colList;
    //moveModelRec* mmRec;

    int ndig, nfrac;
    //ndig - ВХЯКН ГМЮВЮЫХУ ЖХТП Б БШБНДХЛНЛ ПЕГСКЭРЮРЕ.
    //nfrac - ВХЯКН ГМЮВЮЫХУ ЖХТП ОНЯКЕ ГЮОЪРНИ Б БШБНДХЛНЛ ПЕГСКЭРЮРЕ.
    //vflag - МЮКХВХЕ ЙНКНМНЙ Я ЩТЕЛЕПХДМШЛХ ГМЮВЕМХЪЛХ ЯЙНПНЯРХ ДБХФЕМХЪ НАЗЕЙРЮ.

    sstarFile();
    ~sstarFile();

    int init(QString fileName);

    void save();
    void saveAs(QString fName);
    void clear();

    int getColRecNum(int colNum);
    int setColRec(colRec* cRec);
    void sortColList();

    colRec* getColNum(int cNum);

    int countCols(QString colNums);        //ЯДЕКЮРЭ НЯПЕДМЕМХЕ ОН ЯРНКАЖЮЛ ocList Я МНЛЕПЮЛХ, ГЮДЮММШЛХ Б ЯРПНЙЕ colNums (ПЮГДЕКХРЕКЭ - ',')
    int reCountCols();                     //ОЕПЕДЕКЮРЭ НЯПЕДМЕМХЕ ОН ЯРНКАЖЮЛ ocList МЮ НЯМНБЕ СФЕ ЯСЫЕЯРБСЧЫХУ colList
    void do3sigma(double proofP, double sigmaMul);                        //ЯДЕКЮРЭ НРАПЮЙНБЙС ОН 3*sigma ДКЪ OMC(ra) Х OMC(dec)
    int countMM(int fp = 0, int ft = 0, int vflag = 1);
    void delMMrec();

    void removeMes(QString mesureTimeCode);
};

struct statResults2
{
    double meanX, meanY, rmsOneX, rmsOneY, rmsMeanX, rmsMeanY;
    int numX, numY;
};

class residualFile
{
public:
	QList <residualsRec*> resList;
        QList <colRec*> colList;
	
	QString fName;

        double meanKsi, meanEta, rmsOneKsi, rmsOneEta, rmsMeanKsi, rmsMeanEta;
        int numKsi, numEta;

	residualFile();
	~residualFile();
	void init(const char *fname);
        //void initOld(const char *fname);
	void clear();
	void save();
	void saveAs(QString fName);

        void detStat(int isRef = 0);
        void remSigma(double sg, double proofP=0.0, int isRef=0);

        void detStatXY(int isRef = 0);
        void remSigmaXY(double sg, double proofP=0.0, int isRef=0);

        void removeMes(QString mesureTimeCode);

};



class errBudgetFile
{
public:
        QList <errBudgetRec*> errList;

        QString fName;

	errBudgetFile();
	~errBudgetFile();
	void init(const char *fname);

        void save();
        void saveAs(QString fName);

        void removeMes(QString mesureTimeCode);
};

class reductionStat
{
public:
    errBudgetFile *ebFile;
    residualFile *resFile;
    objResidualFile *objFile;
    //eqFile *mpeFile;
    //sstarFile *ssFile;

    QList <measurementRec*> mesList;
    QList <measurementStatRec*> statList;

    reductionStat();
    ~reductionStat();

    int init(QString *ebFileName, QString *resFileName = NULL, QString *objFileName = NULL);

    void findMeasurements();

    measurementRec* getMeasurement(QString mesTimeCode);
    measurementStatRec* getMeasurementStat(QString mesTimeCode);

    void getMeasurementsList(QStringList mesCodeList, reductionStat *selRed);
    void getMeasurementsList(QList <measurementRec*> mesList, reductionStat *selRed);

    void saveMesList(QString ebFileName, QString resFileName, QString objFileName = "");

    int removeMes(QString mesTimeCode);
    void removeMesList(QStringList mesList);

    //void fromMesList()
};

struct measurementRec
{
    QString mesureTimeCode;
    errBudgetRec *errBud;
    QList <residualsRec*> resList;
    QList <objResRec*> objList;
    //QList <ocRec*> mpeList;
    //QList <sstarRes*> sstarList;

    //QList <marksP*> u3MarksList;

    void detStat(measurementStatRec *stRec);

    //void detMarksList(QList <marksP*> u3List);
};

struct measurementStatRec
{
    QString mesureTimeCode;
    QString originName;
    double UWEx, UWEy, UWEm;
    int Nx, Ny, Nm;
    int ocNum;
    double meanOCksi, meanOCeta;
    double rmsMeanOCksi, rmsMeanOCeta;
    double rmsOneOCksi, rmsOneOCeta;


    void copy(measurementStatRec *source);
};

struct plateStatRec
{
    QString plateName;
    QList <measurementStatRec*> mStatList;

    void getMinUWE(measurementStatRec* minRec);
    int getMinUWEpos();
    int getVersNamePos(QString versName);

};

void appendRep0Rec(QString *dataStr, measurementStatRec* msRec, measurementRec* mesRec, int plNameType);
void saveReport0(QString r0name, QList <measurementRec*> mesList, int plNameType);

struct platesStat
{
    QList <plateStatRec*> platesList;

    void init(QList <measurementStatRec*> mesStatList, int plNameType = 0);
    void initPlatesNamesFile(QString plnFileName);
    int pushPlateName(QString plName);

    void dropObj(reductionStat rStat);
    void selMinUWE(reductionStat *rStat, QList <measurementRec*> *mesList);
    void selVersSeq(QStringList versSeqList, reductionStat *rStat, QList <measurementRec*> *mesList, int minObj=0);
    void selMesList(reductionStat *rStat, QList <measurementRec*> *mesList);



    void saveReport0(QString r0name, int isMinUWE, int plNameType, reductionStat *rStat, QList <measurementRec*> *mesList = NULL);
    void saveReport0Seq(QString r0name, QString versSeq, QStringList excMes, int plNameType, reductionStat *rStat, QList <measurementRec*> *mesList = NULL);
};

struct objStatRec
{
    QString objName, catName, catMagName;
    platesStat *plStat;
    QList <objResRec*> objList;
    QList <measurementStatRec*> mStatList;
    void init(int plNameType);
    void do3Sigma(reductionStat *rStat, double sigma, QList <objResRec*> *rejList = NULL, double pfc = 0.05);
    void getOCList(reductionStat *rStat, QList <objResRec*> *ocList);
    //void getSstarList(reductionStat *rStat, QList <sstarRes*> *sstarList);
    void removeMes(QString mesTimeCode);
    //void init(QList <measurementStatRec*> mesStatList, int plNameType = 0);
};

struct objectsStat
{
   QList <objStatRec*> objList;
   void init(reductionStat *redStat, int plNameType);

};
/*
struct eqStatRec
{
    QString asterName;
    platesStat *plStat;
    QList <measurementStatRec*> mStatList;
    void init(int plNameType);
    //void init(QList <measurementStatRec*> mesStatList, int plNameType = 0);
};

struct eqPlatesStat
{
   QList <eqStatRec*> asterList;
   void init(reductionStat *redStat, int plNameType);

};

struct ssStatRec
{
    QString ssName;
    platesStat *plStat;
    QList <measurementStatRec*> mStatList;
    void init();
    //void init(QList <measurementStatRec*> mesStatList, int plNameType = 0);
};

struct ssPlatesStat
{
   QList <ssStatRec*> ssList;
   void init(reductionStat *redStat);
};
*/

/*
struct eqSeriesRec : public ocRec
{
    double epoch;
    QString catEphName;                 //name of (O-C) ephemeride source
   // QString mesureTimeCode;          //time of measurements [YYYYMMDDHHMMSSSS]

 //   void rec2sS(QString *recStr);
 //   void s2recS(QString recStr);

    void rec2sBase(QString *recStr);
    void s2recBase(QString recStr);

    eqSeriesRec(QString str);

    void copy(const eqSeriesRec &source);
    eqSeriesRec& operator=(const eqSeriesRec &source);
};
/*
class eqSeriesFile
{
public:
        QList <eqSeriesRec*> ocList;
        QList <colRec*> colList;
        moveModelRec* mmRec;

        int ndig, nfrac;
        //ndig - ВХЯКН ГМЮВЮЫХУ ЖХТП Б БШБНДХЛНЛ ПЕГСКЭРЮРЕ.
        //nfrac - ВХЯКН ГМЮВЮЫХУ ЖХТП ОНЯКЕ ГЮОЪРНИ Б БШБНДХЛНЛ ПЕГСКЭРЮРЕ.
        //vflag - МЮКХВХЕ ЙНКНМНЙ Я ЩТЕЛЕПХДМШЛХ ГМЮВЕМХЪЛХ ЯЙНПНЯРХ ДБХФЕМХЪ НАЗЕЙРЮ.


        QString fName;


        eqSeriesFile();
        ~eqSeriesFile();
        void init(const char *fname);
        void initOld(const char *fname);
        void save();
        void saveAs(QString fName);
        void clear();

        void sortOClist();

        int getColRecNum(int colNum);
        int setColRec(colRec* cRec);
        void sortColList();

        colRec* getColNum(int cNum);

        int countCols(QString colNums);        //ЯДЕКЮРЭ НЯПЕДМЕМХЕ ОН ЯРНКАЖЮЛ ocList Я МНЛЕПЮЛХ, ГЮДЮММШЛХ Б ЯРПНЙЕ colNums (ПЮГДЕКХРЕКЭ - ',')
        int reCountCols();                     //ОЕПЕДЕКЮРЭ НЯПЕДМЕМХЕ ОН ЯРНКАЖЮЛ ocList МЮ НЯМНБЕ СФЕ ЯСЫЕЯРБСЧЫХУ colList
        void do3sigma(double proofP, double sigmaMul);                        //ЯДЕКЮРЭ НРАПЮЙНБЙС ОН 3*sigma ДКЪ OMC(ra) Х OMC(dec)
        int countMM(int fp = 0, int ft = 0, int vflag = 1);

        void findSeries(QList <eqFile*> *eqList);

        void delMMrec();

        void getSeriesRec(eqSeriesRec *eqsRec);
};

*/

#endif
