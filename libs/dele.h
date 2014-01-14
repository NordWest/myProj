//#pragma once
#ifndef COMF_H
#include "comfunc.h"
#endif

#ifndef COORD_H
#include "coord.h"
#endif

#ifndef DA_H
#include "DynArr.h"
#endif

#ifndef TYPES_DEFINED
#include "ephem_types.h"
#endif

#ifndef EPHEM_UTIL
#include "ephem_util.h"
#endif

//#include "ephem_types.h"
//#include "ephem_read.h"

#ifndef DELE_H

#define DE405_CAT 192

#define MERCURY_NUM MERCURY
#define VENUS_NUM VENUS
#define EARTH_NUM EARTH
#define MARS_NUM MARS
#define JUPITER_NUM JUPITER
#define SATURN_NUM SATURN
#define URANUS_NUM URANUS
#define NEPTUNE_NUM NEPTUNE
#define PLUTO_NUM PLUTO
#define MOON_NUM MOON
#define SUN_NUM SUN
#define NUTATIONS_NUM NUTATIONS
#define LIBRATIONS_NUM LIBRATIONS
#define GEOCENTR_NUM 15


#define CENTER_BARY 0
#define CENTER_SUN 1

#define SK_EKVATOR 0
#define SK_ECLIPTIC 1


#define SCALE_KMSEC 1.377534792908385547227E+6
#define SCALE_AUDAY 2.3737097724907275533844871479929e+9

//static int Debug = FALSE;



int planet_num(char *pname);
double det_planet_H(int pl_num);

int body_num(QString pname)
{
    if(QString().compare(pname, "Mercury")==0) return 1;
    if(QString().compare(pname, "Venus")==0) return 2;
    if(QString().compare(pname, "Earth")==0) return 3;
    if(QString().compare(pname, "Mars")==0) return 4;
    if(QString().compare(pname, "Jupiter")==0) return 5;
    if(QString().compare(pname, "Saturn")==0) return 6;
    if(QString().compare(pname, "Uranus")==0) return 7;
    if(QString().compare(pname, "Neptune")==0) return 8;
    if(QString().compare(pname, "Pluto")==0) return 9;
    if(QString().compare(pname, "Sun")==0) return 10;


    return -1;
}


class dele
{
public:
 /*       int KSize, Ncoeff;

	group1010 *g1010;
	group1030 *g1030;
	group1040 *g1040;
	group1040 *g1041;
	group1050 *g1050;
	group1070 *g1070;
   */
        headOneType  H1;
        headTwoType  H2;
        recOneType   R1;
        FILE *Ephemeris_File;
        double Coeff_Array[ARRAY_SIZE] , T_beg , T_end , T_span;

        //char *fname_h;
        char *fileName;

	dele();
        dele(const char *fname_bin);
        //dele(const char *fname_h, const char *fname_bin);
	~dele();

        //int init_header(const char *fname_h);
        //int init_jpl(const char *jpl_name);

        int init(const char *jpl_bin_name);

	int detR(double *x, double *y, double *z, double Time, int nplanet, int proizv, int centr, int sk);
	int detR(double *x, double *y, double *z, double Time, char *planet, int proizv, int centr, int sk);
	int detRtt(double *x, double *y, double *z, double Time, int nplanet, int centr, int sk);
    int detState(double *x, double *y, double *z, double *vx, double *vy, double *vz, double Time, int nplanet, int centr, int sk);

    int headParam(QString name, double &res);
    double headParam(QString name);

	/*
		proiz: 0 - положения, 1 - скорости
		centr: 0 - barycenter SS, 1 - geliocentr
		sk: 0 - ekvator, 1 - ectiptic
	*/

//	int detState(double *x, double *y, double *z, double *vx, double *vy, double *vz, double time, int nplanet);
//	int get_rec(int nplanet, part_head *ph, double *granul);

private:

        int Read_Coefficients( double Time );

        /*----------------------------------------------------------------------------*/
        /*  Initialize_Ephemeris                                                      */
        /*----------------------------------------------------------------------------*/

            int Initialize_Ephemeris( char *fileName );

        /*----------------------------------------------------------------------------*/
        /*  Interpolate_Libration                                                     */
        /*----------------------------------------------------------------------------*/

            void Interpolate_Libration( double Time, int Target, double Libration[3] );

        /*----------------------------------------------------------------------------*/
        /*  Interpolate_Nutation                                                      */
        /*----------------------------------------------------------------------------*/

            void Interpolate_Nutation( double Time , int Target , double Nutation[2] );

        /*----------------------------------------------------------------------------*/
        /*  Interpolate_Position                                                      */
        /*----------------------------------------------------------------------------*/

            int Interpolate_Position( double Time , int Target , double Position[3] );

        /*----------------------------------------------------------------------------*/
        /*  Interpolate_State                                                         */
        /*----------------------------------------------------------------------------*/

            int Interpolate_State( double Time , int Target , stateType *Planet );



};


#define DELE_H
#endif
