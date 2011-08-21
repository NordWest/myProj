#include <QtCore>
#include "math.h"
#include "astro.h"
#include "mb.h"
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

//////////////////////////////////////////////////////////////////////////////////////

//void detPlateName(QString *plateName, QString originName, int plNameType = 0);
int countColStat(colRec *cRec, double *vect, int sz);

void detVersName(QString *versName, QString originName);
void detTimeCode(QString &timeCode);
void detPlateName(QString *plateName, QString originName, int plNameType = 0);
//void getPlateName(QString origName, QString *plName, int plNameType);

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
        //double ra_oc, dec_oc;		//Р№РќРќРџР”РҐРњР®Р РЁ РќРћР РҐР’Р•РЇР™РќР¦Рќ Р–Р•РњР РџР®
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
        void rec2MPC(QString *str, QString obsName, int objNum, QString obsType = " ");

        void copy(const ocRec &source);
        ocRec& operator=(const ocRec &source);
	
	
        int vers;                                       //Р РҐРћ РЇР РџРќР™РҐ.
        //0 - MJday|ra|de|mag0|ocRaCosDe|ocDe|ocMag|topDist|muRaCosDec|muDe|Vr|phase|elong|name|catNum|expTime
        //1 - MJday|ra|de|mag0|ocRaCosDe|ocDe|ocMag|ra_oc|de_oc|topDist|muRaCosDec|muDe|Vr|phase|elong|name|catNum|expTime
	
};
//ocRec
//MJday		    |ra		         |de		   |mag0    |ocRaCosDe    |ocDe  |ocMag|ra_oc	       |de_oc		  |topDist	       |muRaCosDec	   |muDe     |Vr      |phase   |elong       |name   |catNum|expTime
//2008 05 01.93212|14 26 14.5889|+00 05 12.342|13.279|  -113.6|    28.7|    -1.6|14 26 15.2930|+00 04 53.780| 1.906076352|  -467.40|   219.00|  2.21|   5.420| 164.300|00890|0	   | 180.000

//MJday|ra|de|mag0|ocRaCosDe|ocDe|ocMag|x|y|pixmag|Dx|Dy|Dpixmag|topDist|muRaCosDe|muDe|Vr|phase|elong|expTime|name|catName|catMagName|mesureTimeCode
//2009 01 01.86517|07 58 32.9669|+56 55 15.006|13.883|   169.7|  -284.8|    -0.1|  640.2616|  727.4599|   1.122200e+05|  0.317784|  0.307834|        791.333| 0.032179483| -4032.00|  6960.00|  3.29|  35.130| 143.790|  30.000|        2008 EV5|   mpeph|    Vmag|20110118043313937
/*
    MJday               Р‘РџР•Р›РЄ РњР®РђРљР§Р”Р•РњРҐР