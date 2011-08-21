#ifndef COMF_H
#include "comfunc.h"
#endif

#ifndef FAMCAT_H

#define FAMCAT_LEN 63

class fc_rec
{
public:
	int ast_number;
	char *prov_id;
	float prop_a, prop_ecc, sine_prop_inc;
	char *family_name;
	int clump_flag;
	int confidence_level;
	
	fc_rec();
	~fc_rec();
	void copyTo(fc_rec *rec);
};

class famcat : public FileDynStrBin
{
public:

	fc_rec *record;

	famcat();
	~famcat();

	int init(char *fname);
	void rec2s(char *nnstr);
	void s2rec(char *nnstr);

	int GetRec(int pos);



};

#define FAMCAT_H

#endif