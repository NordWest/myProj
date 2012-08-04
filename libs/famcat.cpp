#include "stdafx.h"

#include "famcat.h"


//	fc_rec	////////////

fc_rec::fc_rec()
{
	this->prov_id = new char[];
	this->family_name = new char[];
}

fc_rec::~fc_rec()
{
	delete [] this->prov_id;
	delete [] this->family_name;
}

void fc_rec::copyTo(fc_rec *rec)
{
	rec->ast_number = this->ast_number;
	strcpy(rec->prov_id, this->prov_id);
	rec->prop_a = this->prop_a;
	rec->prop_ecc = this->prop_ecc;
	rec->sine_prop_inc = this->sine_prop_inc;
	strcpy(rec->family_name, this->family_name);
	rec->clump_flag = this->clump_flag;
	rec->confidence_level = this->confidence_level;
}

//	famcat	///////////

famcat::famcat() : FileDynStrBin(FAMCAT_LEN)
{
	this->record = new fc_rec;
}
famcat::~famcat()
{
	delete this->record;
}

int famcat::init(char *fname)
{
	return(FileDynStrBin::init(fname, FAMCAT_LEN));
}

void famcat::rec2s(char *nnstr)
{
//	sprintf(nnstr, "%6d %s %8.5f%7.4f%7.4f  %s%s  %1d", this->record->ast_number, this->record->prov_id, this->record->prop_a, this->record->prop_ecc, this->record->sine_prop_inc, this->record->family_name, this->record->tail);
}

void famcat::s2rec(char *nnstr)
{
	char *tempstr;
	tempstr = new char[255];

	slovoG(this->str, tempstr, 0, 6);
	this->record->ast_number = atoi(tempstr);

	slovoG(this->str, tempstr, 7, 16);
//	strcpy(this->record->prov_id, tempstr);
	indexB(this->record->prov_id, tempstr, " ", 16);
//	strcpy(&this->record->prov_id[9], "\0");

	slovoG(this->str, tempstr, 16, 24);
	this->record->prop_a = atof(tempstr);

	slovoG(this->str, tempstr, 24, 31);
	this->record->prop_ecc = atof(tempstr);

	slovoG(this->str, tempstr, 31, 38);
	this->record->sine_prop_inc = atof(tempstr);

	slovoG(this->str, tempstr, 40, 58);
	strcpy(this->record->family_name, tempstr);
	char *elen = strstr(this->record->family_name, " ");
	strcpy(elen, "\0");

	slovoG(this->str, tempstr, 58, 59);
	if(streqv(tempstr, "-")) this->record->clump_flag = 0;
	if(streqv(tempstr, "F")) this->record->clump_flag = 1;
	if(streqv(tempstr, "C")) this->record->clump_flag = 2;


	slovoG(this->str, tempstr, 61, 62);
	this->record->confidence_level = atoi(tempstr);

	delete [] tempstr;
	tempstr = NULL;
}

int famcat::GetRec(int pos)
{
	if(this->ReadStr(pos)) return 1;
	this->s2rec(this->str);

	return 0;
}