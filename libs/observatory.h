//#pragma once
#ifndef OBSRY_H
#define OBSRY_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "comfunc.h"
#include "filedynstr.h"

#define OBS_SIZE 255

class obsy
{
public:
	char *num;
        double Long;        //[rad]
        double Cos;         //
        double Sin;         //

	char *name;

	char *tail;

        double getFi();     //[rad]

	obsy();
	~obsy();
        int copyTo(obsy *dest);
};

class observatory : public FileDynStr
{
public:
	observatory();
	~observatory();

     //   int init(char *fn);

	int rec2s(char *arec);
	int s2rec(char *arec);

	double stime, dcx, dcy, dcz, h;
    double vx, vy, vz;
    double state[6];

	obsy *record;

	int addobsy(char *num, double Long, double Cos, double Sin, char *name, int pos);
	int getobsynum(int nofobsy);
        int getobsynumO(char *nofobsy); //right variant of pos determination!
	int getobsyname(char *name);

        int getobsynumO_alt(char *nofobsy);
        int getobsyname_alt(char *name);

	int det_state(double tUTC);
};
#endif

