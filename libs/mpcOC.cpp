#include "stdafx.h"
#include "mpcOC.h"

mpcOC::mpcOC(char *fn) : fbStreamLine(fn)
{
	this->record = new recOC;
	this->init_str(mpcOC_LEN, mpcOC_NAME_KPOS);
	
	sep = new char[1];
}

mpcOC::mpcOC() : fbStreamLine()
{
	this->record = new recOC;
	this->init_str(mpcOC_LEN, mpcOC_NAME_KPOS);
	sep = new char[1];
}

mpcOC::~mpcOC()
{
	delete(this->record);
	delete [] sep;
}

int mpcOC::init(char *fname)
{
	this->init_str(mpcOC_LEN, mpcOC_NAME_KPOS);
	return(fbStreamLine::init(fname));
}

int mpcOC::GetRec(int pos)
{
	GetLine(pos);
	s2rec(str);
}

int mpcOC::AddRec(int pos)
{
	char *tstr = new char[mpcOC_LEN+1];
	rec2s(tstr);
	AddLine(tstr, pos);
	delete [] tstr;
}

int mpcOC::AddRec()
{
	char *tstr = new char[mpcOC_LEN+1];
	rec2s(tstr);
	PushLine(tstr);
	delete [] tstr;
}


void s2rec(char *str_in)
{
	int pose, posb;
	char *slo = new char[128];
	
	posb = 0;
	slovo_stopsim(str_in, slo, sep, posb, &pose, 34);
	this->record->mJD = atof(slo);
	posb = pose+1;
	slovo_stopsim(str_in, slo, sep, posb, &pose, 34);
	this->record->ra = atof(slo);
	posb = pose+1;
	slovo_stopsim(str_in, slo, sep, posb, &pose, 34);
	this->record->de = atof(slo);
	posb = pose+1;
	slovo_stopsim(str_in, slo, sep, posb, &pose, 34);
	this->record->phase = atof(slo);
	posb = pose+1;
	slovo_stopsim(str_in, slo, sep, posb, &pose, 34);
	this->record->ocRa = atof(slo);
	posb = pose+1;
	slovo_stopsim(str_in, slo, sep, posb, &pose, 34);
	this->record->ocDe = atof(slo);
	posb = pose+1;
	slovo_stopsim(str_in, slo, sep, posb, &pose, 34);
	this->record->ocMuRa = atof(slo);
	posb = pose+1;
	slovo_stopsim(str_in, slo, sep, posb, &pose, 34);
	this->record->ocMuDe = atof(slo);
	
	delete [] slo;
}

void rec2s(char *str_out)
{
	sprintf(str_out, "%16.10f\t%15.10f\t%+15.10f\t%8.3f\t%+10.2f\t%+10.2f\t%+10.2f\t%+10.2f", record->mJD, record->ra, record->de, record->phase, record->ocRa, record->ocDE, record->ocMuRa, record->ocMuDe);
}