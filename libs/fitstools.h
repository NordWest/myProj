#ifndef FITSDATA_H
#include "fitsdata.h"
#endif



#ifndef FITSTOOLS_H
#define FITSTOOLS_H

#include <QtCore>
#include "math.h"
#include "comfunc.h"
//edited






#ifndef VECTGRID3D_H
#include "./vectGrid3D.h"
#endif
#include "fitsio.h"







int make_fits_header(fitsfile *fptr, char *fheadfn);
void make_date_obs(char *str_date_obs, char *str_time_obs, char *str_date_end, char *str_time_end);
void make_good_RADEC(char *str_ra, int type);
int make_good_fits_header(char *fheader_name);








#endif
