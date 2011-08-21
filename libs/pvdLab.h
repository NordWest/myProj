#pragma once

#ifndef REC_H
#include "rec.h"
#endif

#ifndef COORD_H
#include "coord.h"
#endif

#ifndef ORBCAT_H
#include "orbcat.h"
#endif

#ifndef PVD_KRUG_ORB_H
#include "pvdKrugOrb.h"
#endif


class pvdLab
{
public:

	int is_coords_loaded;
	int ctype;

	List4 *obsdata;
	OrbCat *orbs;

	char *fn_odata;
	char *fn_suff;
	char *fn_log;

	char *path_to_inifiles;
	char *path_to_resfiles;


	pvdKrugOrb *korb;	//ctype = 0
//	pvdP *porb;			//ctype = 1;

	pvdLab();
	~pvdLab();
};