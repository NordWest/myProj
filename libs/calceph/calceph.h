/*-----------------------------------------------------------------*/
/*! 
  \file calceph.h 
  \brief public API for calceph library
        access and interpolate INPOP and JPL Ephemeris data.

  \author  M. Gastineau 
           Astronomie et Systemes Dynamiques, IMCCE, CNRS, Observatoire de Paris. 

   Copyright, 2008, 2009, 2010, 2011, 2012, CNRS
   email of the author : gastineau@imcce.fr
*/
/*-----------------------------------------------------------------*/

/*-----------------------------------------------------------------*/
/* License  of this file :
 This file is "triple-licensed", you have to choose one  of the three licenses 
 below to apply on this file.
 
    CeCILL-C
    	The CeCILL-C license is close to the GNU LGPL.
    	( http://www.cecill.info/licences/Licence_CeCILL-C_V1-en.html )
   
 or CeCILL-B
        The CeCILL-B license is close to the BSD.
        (http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.txt)
  
 or CeCILL v2.0
      The CeCILL license is compatible with the GNU GPL.
      ( http://www.cecill.info/licences/Licence_CeCILL_V2-en.html )
 

This library is governed by the CeCILL-C, CeCILL-B or the CeCILL license under 
French law and abiding by the rules of distribution of free software.  
You can  use, modify and/ or redistribute the software under the terms 
of the CeCILL-C,CeCILL-B or CeCILL license as circulated by CEA, CNRS and INRIA  
at the following URL "http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-C,CeCILL-B or CeCILL license and that you accept its terms.
*/
/*-----------------------------------------------------------------*/


#ifndef __CALCEPH_H__
#define __CALCEPH_H__

/*----------------------------------------------------------------------------------------------*/
/* definition of the CALCEPH library version */
/*----------------------------------------------------------------------------------------------*/
/*! version : major number of CALCEPH library */
#define CALCEPH_VERSION_MAJOR 2
/*! version : minor number of CALCEPH library */
#define CALCEPH_VERSION_MINOR 0 
/*! version : patch number of CALCEPH library */
#define CALCEPH_VERSION_PATCH 0

/*----------------------------------------------------------------------------------------------*/
/* windows specific support */
/*----------------------------------------------------------------------------------------------*/
/* Support for WINDOWS Dll:
   Check if we are inside a CALCEPH build, and if so export the functions.
   Otherwise does the same thing as CALCEPH */
#if defined (__GNUC__)
#define __CALCEPH_DECLSPEC_EXPORT  __declspec(__dllexport__)
#define __CALCEPH_DECLSPEC_IMPORT  __declspec(__dllimport__)
#endif
#if defined (_MSC_VER) || defined (__BORLANDC__)
#define __CALCEPH_DECLSPEC_EXPORT  __declspec(dllexport)
#define __CALCEPH_DECLSPEC_IMPORT  __declspec(dllimport)
#endif
#ifdef __WATCOMC__
#define __CALCEPH_DECLSPEC_EXPORT  __export
#define __CALCEPH_DECLSPEC_IMPORT  __import
#endif
#ifdef __IBMC__
#define __CALCEPH_DECLSPEC_EXPORT  _Export
#define __CALCEPH_DECLSPEC_IMPORT  _Import
#endif

#if defined(__CALCEPH_LIBCALCEPH_DLL)
/* compile as a dll */
#if defined(__CALCEPH_WITHIN_CALCEPH)
#define __CALCEPH_DECLSPEC __CALCEPH_DECLSPEC_EXPORT
#else
#define __CALCEPH_DECLSPEC __CALCEPH_DECLSPEC_IMPORT
#endif
#else
/* other cases */
#define __CALCEPH_DECLSPEC 
#endif /*__CALCEPH_LIBCALCEPH_DLL*/

/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
#if defined (__cplusplus)
extern "C" {
#endif /*defined (__cplusplus)*/

/*----------------------------------------------------------------------------------------------*/
/* definition of some constants */
/*----------------------------------------------------------------------------------------------*/
/*! define the maximum number of characters (includeing the trailing '\0') 
 that the name of a constant could contain. */
#define CALCEPH_MAX_CONSTANTNAME 33

/*! define the offset value for asteroid for calceph_?compute */
#define CALCEPH_ASTEROID 2000000

/* unit for the output */
/*! outputs are in Astronomical Unit */
#define CALCEPH_UNIT_AU 1
/*! outputs are in kilometers */
#define CALCEPH_UNIT_KM 2
/*! outputs are in day */
#define CALCEPH_UNIT_DAY 4
/*! outputs are in seconds */
#define CALCEPH_UNIT_SEC 8
/*! outputs are in radians */
#define CALCEPH_UNIT_RAD 16

/*-----------------------------------------------------------------*/
/* error handler */
/*-----------------------------------------------------------------*/
/*! set the error handler */
__CALCEPH_DECLSPEC void calceph_seterrorhandler(int typehandler, void (*userfunc)(const char*));

/*-----------------------------------------------------------------*/
/* single access API per thread/process */
/*-----------------------------------------------------------------*/
/*! open an ephemeris data file */
__CALCEPH_DECLSPEC int calceph_sopen(const char *filename);

/*! compute the position <x,y,z> and velocity <xdot,ydot,zdot>
    for a given target and center */
__CALCEPH_DECLSPEC int calceph_scompute(double JD0, double time, int target, int center, double PV[6]);

/*! get constant value from the specified name in the ephemeris file */
__CALCEPH_DECLSPEC int calceph_sgetconstant(const char* name, double *value);

/*! return the number of constants available in the ephemeris file */
__CALCEPH_DECLSPEC int calceph_sgetconstantcount(void);

/*! return the name and the associated value of the constant available at some index in the ephemeris file */
__CALCEPH_DECLSPEC int calceph_sgetconstantindex(int index, char name[CALCEPH_MAX_CONSTANTNAME], double *value);

/*! close an ephemeris data file */
__CALCEPH_DECLSPEC void calceph_sclose(void);


/*-----------------------------------------------------------------*/
/* multiple access API per thread/process */
/*-----------------------------------------------------------------*/
/*! ephemeris descriptor */
typedef struct calcephbin t_calcephbin;

/*! open an ephemeris data file */
__CALCEPH_DECLSPEC t_calcephbin* calceph_open(const char *filename);

/*! open a list of ephemeris data file */
__CALCEPH_DECLSPEC t_calcephbin* calceph_open_array(int n, const char  * const filename[/*n*/]);

/*! compute the position <x,y,z> and velocity <xdot,ydot,zdot>
    for a given target and center. The output is in UA, UA/day, radians */
__CALCEPH_DECLSPEC int calceph_compute(t_calcephbin* eph, double JD0, double time, int target, 
                                       int center, double PV[6]);

/*! compute the position <x,y,z> and velocity <xdot,ydot,zdot>
    for a given target and center. The output is expressed according to unit */
__CALCEPH_DECLSPEC int calceph_compute_unit(t_calcephbin* eph, double JD0, double time, int target, 
                                       int center, int unit, double PV[6]);

/*! get constant value from the specified name in the ephemeris file */
__CALCEPH_DECLSPEC int calceph_getconstant(t_calcephbin* eph, const char* name, double *value);

/*! return the number of constants available in the ephemeris file */
__CALCEPH_DECLSPEC int calceph_getconstantcount(t_calcephbin* eph);

/*! return the name and the associated value of the constant available at some index in the ephemeris file */
__CALCEPH_DECLSPEC int calceph_getconstantindex(t_calcephbin* eph, int index, char name[CALCEPH_MAX_CONSTANTNAME], double *value);

/*! close an ephemeris data file and destroy the ephemeris descriptor */
__CALCEPH_DECLSPEC void calceph_close(t_calcephbin* eph);



/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
#if defined (__cplusplus)
}
#endif /*defined (__cplusplus)*/

#endif /*__CALCEPH_H__*/

