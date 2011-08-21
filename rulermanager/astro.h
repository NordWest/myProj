#include <QtCore>
#include "math.h"
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>



using namespace boost::numeric::ublas;

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

typedef struct __CPOS__
{
	double ra;
	double de;
}CPOS,*PCPOS;


double mas_to_rad(double angle);
double rad_to_mas(double angle);
double ang_b_dir(double a1, double d1, double a2, double d2);
double hms_to_mas(QString ra, QString spl_symb);
double damas_to_mas(QString de, QString spl_symb);
QString mas_to_hms(double ra, QString spl_symb, int ndig);
QString mas_to_damas(double de, QString spl_symb, int ndig);
DATEOBS timeFromStrNA(QString strT);
DATEOBS timeFromStrFTN(QString strT);
DATEOBS timeFromStrSDSS(QString strT1, QString strT2);
double getJD(DATEOBS date_obs);
double getMJDfromStrNA(QString strT, double exp_time);
double getMJDfromStrFTN(QString strT, double exp_time);
double getMJDfromStrSDSS(QString strT1, QString strT2, double exp_time);
double getYearFromMJD(double mjd);
DATEOBS getDATEOBSfromMJD(double mjd);
QString getStrFromDATEOBS(DATEOBS date_obs, QString spl_symb, int format, int ndig);
double getMJDfromYMD(QString strT);
double getAngleDeg(double cos_x, double sin_x);
matrix<double> getUCAC2data(double ra_c, double de_c, double rho, int epobs, double mjd, QString path, double mag1, double mag2);
matrix<double> getUSNOBdata(double ra_c, double de_c, double rho, int epobs, double mjd, QString path, double mag1, double mag2);
matrix<double> getTangPos(matrix<double> rc, double ra_c, double de_c, int Nra, int Nde, int Nmag);
vector<double> getTangToRaDe(double ksi, double eta, double ra_c, double de_c);
vector<double> getRaDeToTang(double ra, double de, double ra_c, double de_c);
CPOS getCposFromWCS(double x, double y, double *WCSD);
vector<double> getPixPosFromWCS(double ra, double de, double *WCSD);
