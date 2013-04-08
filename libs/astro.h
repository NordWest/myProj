#include <QtCore>
#include "math.h"

#ifndef COMF_H
//#include "./../libs/comfunc.h"
#endif
//#include "fitsio.h"

#ifndef ASTRO_H

#define PI 3.141592653589793238462
#define SECINRAD 206264.806
#define MAS_IN_GRAD 3600000.0
//#define PI 3.141592653589
//#define PI atan(1)*4
#define AU_CONST 1
#define AUKM 1.49597870691000015e+8
#define AUSM 1.49597870691000015e+13
typedef struct __DATEOBS__
{
	int year;
	int month;
	int day;
	int hour;
	int min;
	double sec;
	double pday;
}DATEOBS,*PDATEOBS;

double mas_to_rad(double angle);
double mas_to_grad(double angle);
//double rad_to_mas(double angle);
//double rad_to_grad(double angle);
double grad_to_mas(double angle);
double grad_to_rad(double angle);
double ang_b_dir(double a1, double d1, double a2, double d2);
double hms_to_mas(QString ra, QString spl_symb);
double damas_to_mas(QString de, QString spl_symb);
QString mas_to_hms(double ra, QString spl_symb, int ndig);
QString mas_to_damas(double de, QString spl_symb, int ndig);

DATEOBS timeFromStrNA(QString strT);
DATEOBS timeFromStrFTN(QString strT);
int timeFromStrFTN(DATEOBS *date_obs, QString strT);
DATEOBS timeFromStrSDSS(QString strT1, QString strT2);
double getJD(DATEOBS date_obs);
double getMJDfromStrNA(QString strT, double exp_time);
double getMJDfromStrFTN(QString strT, double exp_time);
double getMJDfromStrSDSS(QString strT1, QString strT2, double exp_time);
double getMJDfromStrSDSS(QString strT, double exp_time);

double getMJDfromStrT(QString strT);

double getYearFromMJD(double mjd);
double getMJDFromYear(double year);
double getMJDfromYMD(QString strT);
double jd2mjd(double jd);
double mjd2jd(double mjd);
DATEOBS getDATEOBSfromMJD(double mjd, int rnsec = 5);
void getDATEOBSfromMJD(DATEOBS *date_obs, double mjd, int rnsec = 5);
int roundDATEOBS(DATEOBS *date_obs, int nsec);
QString getStrFromDATEOBS(DATEOBS date_obs, QString spl_symb, int format, int ndig);
void getStrFromDATEOBS(QString *rstr, DATEOBS date_obs, QString spl_symb, int format, int ndig);

double getAngleDeg(double cos_x, double sin_x);
int timeFromStrFTN(DATEOBS *date_obs, QString strT);
int getMJDfromStrFTN(double *mjd, QString strT, double exp_time);
int getStrFTNfromMJD(QString *strT, double mjd, double exp_time);
int getStrTfromMJD(QString *strT, double mjd);

double getS(DATEOBS date_obs);
QString getStrFromS(double s, QString spl_symb, int ndig);
QString getStrFromDE(double de, QString spl_symb);
QString getRaDecMag(QString str);

void mjdDateCode(QString *dateCode, double mJD);
void mjdDateCode_file(QString *dateCode, double mJD);
/*mas
double* getTangToRaDe(double ksi, double eta, double ra_c, double de_c);
void getTangToRaDe1(double *rd0, double *rd1, double ksi, double eta, double ra_c, double de_c);
        void getTangToRaDe(double ksi, double eta, double ra_c, double de_c, double *const ep);
double* getRaDeToTang(double ra, double de, double ra_c, double de_c);
void getRaDeToTang1(double *ksi, double *eta, double ra, double de, double ra_c, double de_c);
        void getRaDeToTang(double ra, double de, double ra_c, double de_c, double *const tang);
double* getCposFromWCS(double x, double y, double *WCSD);
void getCposFromWCS1(double *ra, double *de, double x, double y, double *WCSD);
double* getPixPosFromWCS(double ra, double de, double *WCSD);
void getPixPosFromWCS(double *x, double *y, double ra, double de, double *WCSD);
	void getPixPosFromWCS(double ra, double de, double *WCSD, double *const pixp);
*/
//Degree
void getDegToTang(double *ksi, double *eta, double ra, double de, double ra_c, double de_c);
void getTangToDeg(double *rd0, double *rd1, double ksi, double eta, double ra_c, double de_c);
void getPixPosToDegWCS(double *ra, double *de, double x, double y, double *WCSD);
void getDegToPixPosWCS(double *x, double *y, double ra, double de, double *WCSD);

double hms_to_deg(QString ra, QString spl_symb);
double damas_to_deg(QString de, QString spl_symb);
QString deg_to_hms(double ra, QString spl_symb, int ndig);
QString deg_to_damas(double de, QString spl_symb, int ndig);
void deg_to_hms(QString *str, double ra, QString spl_symb, int ndig);
void deg_to_damas(QString *str, double de, QString spl_symb, int ndig);
int hms_to_deg(double *raDeg, QString ra, QString spl_symb);
int damas_to_deg(double *deDeg, QString de, QString spl_symb);

int isVes(int year);
double dinm(int mounth, int isves);


/*
 int make_fits_header(fitsfile *fptr, char *fheadfn);
 void make_date_obs(char *str_date_obs, char *str_time_obs, char *str_date_end, char *str_time_end);
 void make_good_RADEC(char *str_ra, int type);
 int make_good_fits_header(char *fheader_name);
 */
 


int UCAC2dataToFile(double ra_c, double de_c, double rho_x, double rho_y, double mag1, double mag2, int shape, QString path, QString savePath, QString ufname, QString sepS, int pForm);



double delta_ra(double a1, double a2);
void rot2D(double *r, double ang);

void desc2NumName(QString desc, int *num, QString *name);

double rad_to_mas(double angle);

double grad2rad(double grad);
double rad2grad(double rad);

double rad2mas(double rad);
double mas2rad(double mas);
/*
#ifndef COMF_H
int fakt(int n);
double grad2rad(double grad);
double rad2grad(double rad);
int streqv(char *str1, char *str2);
int slovo_stopsim(char *str, char *slo, char* stopsim, int posb, int *pose, int nmax);
int dat2JD(double *Jday, int year, int mth, double day);
int dat2YMD(double Jday, int *year, int *mth, double *day);
void day2HMS(double day, int *iday, int *hour, int *min, double *sec);
void HMS2day(double *day, int hour, int min, double sec);
int isVes(int year);
double dinm(int mounth, int isves);
void dat2YMD_str(double JD, char *str);

#endif
*/
//double phaseCorr(doubl
#define ASTRO_H
#endif
