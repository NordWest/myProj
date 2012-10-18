#include "calc_epm.h"

int jdb[11];       //initial Julian date (int part)
double djb[11];    //initial Julian date (frac part)
double   delta[11];     //approximation subinterval
int ncoef[11];    //number of (polunomial degrees + free term)
int ndim[11];     //number of dimensions
int nbl[11];       //number of subintervals
int shift[11];     //shift in the coeef array 
double coef[0x4000000];    //array of coefficiens for 11 objects
char names[11][40];   //file names
double rho = 82.3005676536174207; //const Earth/Moon + 1

int InitTxt(char* dir)
{
//Read 11 ephemeride text files into arrays
//Input: path to the directory '\TEXT\'
//Output: logical
    int i, coef_size, shift_, j;
	size_t dir_l;
	FILE *lfn;
	int result = 1;
	dir_l = strlen(dir);
	for(i = 0; i < 11; i++)
      strcpy(names[i], dir);
    strcat(names[0], "text/mercury.08t\0");
    strcat(names[1], "text/venus.08t\0");
    strcat(names[2], "text/earth_m.08t\0");
    strcat(names[3], "text/mars.08t\0");
    strcat(names[4], "text/jupiter.08t\0");
    strcat(names[5], "text/saturn.08t\0");
    strcat(names[6], "text/uranus.08t\0");
    strcat(names[7], "text/neptune.08t\0");
    strcat(names[8], "text/pluto.08t\0");
    strcat(names[9], "text/moon.08t\0");
    strcat(names[10], "text/sun.08t\0");
       
    shift_ = 0;
    for(i = 0; i < 11; i++)
	{
		printf("%s\n", names[i]);
        if((lfn = fopen(names[i], "rb")) == NULL) 
		  { printf("error\n");
            return 0;
           }
        printf(".....");
        fscanf(lfn, "%i", &jdb[i]);
		fscanf(lfn, "%lE", &djb[i]);
		fscanf(lfn, "%lE", &delta[i]);
		fscanf(lfn, "%i", &ncoef[i]);
		fscanf(lfn, "%i", &ndim[i]);
		fscanf(lfn, "%i", &nbl[i]);
        coef_size = ncoef[i] * ndim[i] * nbl[i];
		for(j = 0; j < coef_size; j++)
		   {fscanf(lfn, "%lE", &coef[shift_ + j]);
		   }
        shift[i] = shift_;
        shift_ = shift_ + coef_size;
        fclose(lfn); 
        printf( "Ok\n");
 }
 return result;
}
int InitBin(char* dir)
{
//Read 11 ephemeride binary files into arrays
//Output: logical
    int i, coef_size, shift_;
	size_t dir_l;
	FILE *lfn;
	int result = 1;
	dir_l = strlen(dir);
	for(i = 0; i < 11; i++)
      strcpy(names[i], dir);
    strcat(names[0], "bin/mercury.08b\0");
    strcat(names[1], "bin/venus.08b\0");
    strcat(names[2], "bin/earth_m.08b\0");
    strcat(names[3], "bin/mars.08b\0");
    strcat(names[4], "bin/jupiter.08b\0");
    strcat(names[5], "bin/saturn.08b\0");
    strcat(names[6], "bin/uranus.08b\0");
    strcat(names[7], "bin/neptune.08b\0");
    strcat(names[8], "bin/pluto.08b\0");
    strcat(names[9], "bin/moon.08b\0");
    strcat(names[10], "bin/sun.08b\0");
       
    shift_ = 0;
    for(i = 0; i < 11; i++)
	{
		printf("%s\n", names[i]);
        if((lfn = fopen(names[i], "rb")) == NULL) 
		  { printf("error\n");
            return 0;
           }
        printf(".....");
		fread(&jdb[i], sizeof(jdb[i]), 1, lfn);
		fread(&djb[i], sizeof(djb[i]), 1, lfn);
		fread(&delta[i], sizeof(delta[i]), 1, lfn);
		fread(&ncoef[i], sizeof(ncoef[i]), 1, lfn);
		fread(&ndim[i], sizeof(ndim[i]), 1, lfn);
		fread(&nbl[i], sizeof(nbl[i]), 1, lfn);
        coef_size = ncoef[i] * ndim[i] * nbl[i];
		fread(&coef[shift_], sizeof(coef[shift_]), coef_size, lfn);
        shift[i] = shift_;
        shift_ = shift_ + coef_size;
        fclose(lfn); 
        printf( "Ok\n");
 }
 return result;
}

double calc_vel(unsigned char ncoefm, int c_ptr, double* tt)
// Calculates value of velocity's component.                        
// Input
//       int ncoefm  - number of coefficients
//       int c_ptr   - position of the current interval in the array coef
//       double* tt  - array of values of Chebyshev's polynomials
// Output
//       double v    - velocity's component

{double v = 0.0;
 int i;
 for(i = ncoefm; i > 1; i--)
   v = v = v + coef[c_ptr + i - 1] * tt[i - 1];
 v = v + coef[c_ptr];
 return v;
}

double calc_pos(unsigned char ncoefm, int c_ptr, double* xx, double dlt)
// Calculates value of object coordinate
// Input
//       int ncoefm  - number of coefficients
//       int  c_ptr  - position of the current interval in the array coef
//       double* xx  - array of integrals of Chebyshev's polynomials
//       double dlt  - approximation interval
// Output 
//       double x    - object coordinate       
{int i;
 double x = 0.0;
 for (i = ncoefm; i >= 1; i--) 
	 x = x + coef[i + c_ptr - 1] * xx[i - 1]; 
 x = 0.5 * dlt * x + coef[c_ptr + ncoefm];
 return x;
}

void calc_cheb_pol(int ncoefm, double td, double *tt, double *xx)
// Calculates values of Chebyshev's polynomials along with their    
// integrals.                                                                 
// Input
//       int ncoefm   - polynomial power
//       double td    - argument [-1 .. 1]
// Output
//       double* tt   - array of values of Chebyshev polynomials 
//       double* xx   - array of integrals
{       
 double d;                   //2.0 * td
 int i, j;                // counters
 int flag;                  // (-1)^n
 tt[0] = 1.0;	
 tt[1] = td; 
 d = 2.0 * td; 
 for (i = 3; i <= ncoefm + 1; i++) 
	 tt[i-1] = tt[i - 2] * d - tt[i - 3]; 
 xx[0] = td;
 xx[1] = (tt[2] + tt[0]) * 0.25;
 for (i = 3; i <= ncoefm; i++) 
	 xx[i-1] = 0.5 * (tt[i] / i - tt[i - 2] / (i - 2)); 
 j = 0; 
 flag = 0; 
 i = 4; 
 while (i <= ncoefm) 
  { 
   j = j + 1; 
   d = 0.25 / j + 0.25 / (j + 1); 
   flag = 1 - flag; 
   if (flag) d = -d; 
   xx[i-1] = xx[i-1] + d; 
   i = i + 2; 
  }
}


int calc(int pl_num, int jd, double dj, double* xv, double *vv)
//Calculates coordinates and velocities of the following oblects:  
// 1- Mercury(bar), 2- Venus(bar)  , 3- Earth+Moon(bar),  4- Mars(bar),  5- Jupiter(bar) 
// 6- Saturn(bar),  7- Uranus(bar) , 8- Neptune(bar), 9 - Pluto(bar) ,     
// 10- Moon(geo), 11- Sun(bar).
// Input
//    int pl_num      - number of object
//    int jd          - Julian date (integral part) at which interpolation is wanted
//    double dj       - Julian date (frac part)
// Output
//    double* xv      - object positions
//    double* vv      - object velocities
//    int status      - status of output data
{ 
 int status;
 int jdb_;              // - initial Julian date (int part)
 double  djb_;          // - initial Julian date (frac part)
 double  delta_;        // - approximation subinterval
 int nbl_;              // - number of subintervals
 int ncoef_;            // - number of (polunomial degrees + free term)
 int ndim_;             // - number of dimensions
 int shift_;            // - position of coeffiients of current object in the array coef    
    
 double td;             // - Julian in [initial date, final date] -> td in [-1, 1]
 int cur_bl;            // - number of approximation interval for current Julian date     
 double tt[32], xx[32]; // - arrays of values and integrals of Chebyshev polynomials
 int coef_ptr;          // - position of the current interval in the array coef
 int i;                 // - counter
  
 pl_num--;              //!!!! C-array [0..10]
 status = 1;
 jdb_ = jdb[pl_num];
 djb_ = djb[pl_num];
 nbl_ = nbl[pl_num];
 ndim_ = ndim[pl_num];
 delta_ = delta[pl_num];
 ncoef_ = ncoef[pl_num];
 shift_ = shift[pl_num];  
   
 td = ((jd - jdb_) + (dj - djb_)) / delta_;
 cur_bl = (int)td;
 td = td - cur_bl;
 td = 2.0 * td - 1.0;
 cur_bl = cur_bl + 1;
 if((cur_bl <= 0) || (cur_bl >nbl_)) 
    return 0;
 shift_ = shift_ + (cur_bl - 1) * (ncoef_ * ndim_);
 calc_cheb_pol(ncoef_ - 1, td, tt, xx);    
 for(i = 0; i < ndim_; i++)
   { coef_ptr = shift_ + ncoef_ * i;          
     xv[i] = calc_pos(ncoef_ - 1, coef_ptr, xx, delta_);
     vv[i] = calc_vel(ncoef_ - 1, coef_ptr, tt);
   }
 return status;
}
int calc_bar(int pl_num, int jd, double dj, double* xv, double* vv)
//Calculates barycentric coordinates and velocities of the following objects
// 1- Mercury, 2- Venus  , 3- Earth,  4- Mars,  5- Jupiter 
// 6- Saturn,  7- Uranus , 8- Neptune, 9 - Pluto,     
// 10- Moon, 11- Sun, 13 - Earth + Moon 
// Input
//    int pl_num      - number of object
//    int jd          - Julian date (integral part) at which interpolation is wanted
//    double dj       - Julian date (frac part)
// Output
//    double* xv      - object positions
//    double* vv      - object velocities
//    int status      - status of output data
{double xv_add[3];      // - additional positions
 double vv_add[3];      // - additional velocities  
 int i;

 if(pl_num == 3) //Earth
   {//Earth(bar) = (Earth + Moon)(bar) - Moon(geo)/rho
      if(!calc(3, jd, dj, xv, vv)) return 0;
      if(!calc(10, jd, dj, xv_add, vv_add)) return 0;
	  for(i = 0; i < 3; i++)
	   {xv_add[i] = xv_add[i] / rho;
	    vv_add[i] = vv_add[i] / rho;
	    xv[i] = xv[i] - xv_add[i];
        vv[i] = vv[i] - vv_add[i];       
	   }
	  return 1;
   }
 if(pl_num == 10) //Moon
   {//Moon(bar) = Earth(bar) + Moon(geo)
      if(!calc(3, jd, dj, xv, vv)) return 0;
      if(!calc(10, jd, dj, xv_add, vv_add)) return 0;
	  for(i = 0; i < 3; i++)
	   {xv_add[i] = xv_add[i] / rho;
        xv[i] = xv[i] - xv_add[i];
	    vv_add[i] = vv_add[i] / rho;
        vv[i] = vv[i] - vv_add[i];       
	   }
      if(!calc(10, jd, dj, xv_add, vv_add)) return 0;
	  for(i = 0; i < 3; i++)
	   {xv[i] = xv[i] + xv_add[i];
        vv[i] = vv[i] + vv_add[i];       
	   }
      return 1;
   }
 if(pl_num == 13) //Earth + Moon
     return calc(3, jd, dj, xv, vv);
 // neither Earth, nor Moon
 return calc(pl_num, jd, dj, xv, vv);
}
int calc_EPM(int pl_num, int centr_num, int jd, double dj, double* xv, double* vv)
//Calculates coordinates and velocities of the oblect 'PL_NUM'
//with respect to the point 'CENTR_NUM'
//The numbering convention for 'PL_NUM' and 'CENTR_NUM':
//                                                                               
//                1 = MERCURY           8 = NEPTUNE                              
//                2 = VENUS             9 = PLUTO                                
//                3 = EARTH            10 = MOON                                 
//                4 = MARS             11 = SUN                                  
//                5 = JUPITER          12 = SOLAR-SYSTEM BARYCENTER              
//                6 = SATURN           13 = EARTH-MOON BARYCENTER                
//                7 = URANUS                               
// Input
//    int pl_num      - number of object
//    int centr_num   - number of object
//    int jd          - Julian date (integral part) at which interpolation is wanted
//    double dj       - Julian date (frac part)
// Output
//    double* xv      - object positions
//    double* vv      - object velocities
//    int status      - status of output data
{double xv_bar[3];      // - barycentric positions of the point 'CENTR_NUM'
 double vv_bar[3];      // -  -"- velocities  -"-
 int i;
 for(i = 0; i < 3; i++)
  {xv[i] = 0;
   vv[i] = 0;       
  }
 if((pl_num == centr_num) || (pl_num == 12)) 
   return 1;
 if((centr_num == 3) && (pl_num == 10)) // Moon(geo)
   return calc(pl_num, jd, dj, xv, vv);
 if(centr_num == 12) //barycentric
   return calc_bar(pl_num, jd, dj, xv, vv);
// general case 
 if(!calc_bar(pl_num, jd, dj, xv, vv)) return 0;
 if(!calc_bar(centr_num, jd, dj, xv_bar, vv_bar)) return 0;
 for(i = 0; i < 3; i++)
  {xv[i] = xv[i] - xv_bar[i];
   vv[i] = vv[i] - vv_bar[i];       
  }
 return 1;
}