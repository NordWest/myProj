#include <QtCore>
#include "math.h"
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include "fitsio.h"

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


double mas_to_rad(double angle);
double rad_to_mas(double angle);
double ang_b_dir(double a1, double d1, double a2, double d2);
double hms_to_mas(QString ra, QString spl_symb);
double damas_to_mas(QString de, QString spl_symb);
QString mas_to_hms(double ra, QString spl_symb, int ndig);
QString mas_to_damas(double de, QString spl_symb, int ndig);
DATEOBS timeFromStrNA(QString strT);
DATEOBS timeFromStrFTN(QString strT);
double getJD(DATEOBS date_obs);
double getMJDfromStrNA(QString strT, double exp_time);
double getMJDfromStrFTN(QString strT, double exp_time);
double getYearFromMJD(double mjd);
DATEOBS getDATEOBSfromMJD(double mjd);
QString getStrFromDATEOBS(DATEOBS date_obs, QString spl_symb, int format, int ndig);
double getAngleDeg(double cos_x, double sin_x);
matrix<double> getUCAC2data(double ra_c, double de_c, double rho, int epobs, double mjd, QString path, double mag1, double mag2);
matrix<double> getTangPos(matrix<double> rc, double ra_c, double de_c, int Nra, int Nde, int Nmag);
vector<double> getTangToRaDe(double ksi, double eta, double ra_c, double de_c);
vector<double> getRaDeToTang(double ra, double de, double ra_c, double de_c);

//My

int make_fits_header(fitsfile *fptr, char *fheadfn);
void make_date_obs(char *str_date_obs, char *str_time_obs, char *str_date_end, char *str_time_end);
void make_good_RADEC(char *str_ra, int type);
int make_good_fits_header(char *fheader_name);
int streqv(char *str1, char *str2);
int slovo_stopsim(char *str, char *slo, char* stopsim, int posb, int *pose, int nmax);
int dat2JD(double *Jday, int year, int mth, double day);
int dat2YMD(double Jday, int *year, int *mth, double *day);
void day2HMS(double day, int *iday, int *hour, int *min, double *sec);
void HMS2day(double *day, int hour, int min, double sec);
int isVes(int year);
double dinm(int mounth, int isves);
