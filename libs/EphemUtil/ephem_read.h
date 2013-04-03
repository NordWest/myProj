/**==========================================================================**/
/**                                                                          **/
/**  SOURCE FILE: ephem_read.h                                               **/
/**                                                                          **/
/**     This file contains C prototype declarations for the functions that   **/
/**     are defined in the source file ephem_read.c; the purpose of each of  **/
/**     them is explained there.                                             **/
/**                                                                          **/
/**  Programmer:  David Hoffman/EG5                                          **/
/**               NASA, Johnson Space Center                                 **/
/**               Houston, TX 77058                                          **/
/**               e-mail: david.a.hoffman1@jsc.nasa.gov                      **/
/**                                                                          **/
/**==========================================================================**/

#include <stdio.h>
#include <math.h>
#ifndef TYPES_DEFINED
#include "ephem_types.h"
#endif
static headOneType  H1;
static headTwoType  H2;
static recOneType   R1;
static FILE        *Ephemeris_File;
static double       Coeff_Array[ARRAY_SIZE] , T_beg , T_end , T_span;
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

    void Interpolate_Position( double Time , int Target , double Position[3] );

/*----------------------------------------------------------------------------*/
/*  Interpolate_State                                                         */
/*----------------------------------------------------------------------------*/

    void Interpolate_State( double Time , int Target , stateType *Planet );

#define EPHEM_READ
