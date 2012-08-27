//#include "stdafx.h"
#include "sscatFB.h"

sscatFB::sscatFB() : fbStreamLine()
{
	this->record = new sscat_rec;
}

sscatFB::~sscatFB()
{
	delete record;
}

int sscatFB::init(char *fname)
{
	init_str(SSCATFB_REC, SSCATFB_NAME_KPOS);
	init_line(0);
	return(fbStreamStr::init(fname));

}

void sscatFB::s2rec(char *str_in)
{
	char *tstr;
	tstr = new char[SIMPLE_STR];

	slovoG(str_in, tstr, 0, 4);
	strcpy(this->record->LPSM, tstr);
	strcpy(&this->record->LPSM[4], "\0");

	slovoG(str_in, tstr, 5, 16);
	this->record->setLSPM(tstr);
//	strcpy(&this->record->LSPM[11], "\0");

	slovoG(str_in, tstr, 17, 22);
	strcpy(this->record->LHS, tstr);
	strcpy(&this->record->LHS[5], "\0");

	slovoG(str_in, tstr, 23, 28);
	this->record->NLTT = atoi(tstr);

	slovoG(str_in, tstr, 29, 35);
	this->record->HIP = atoi(tstr);

	slovoG(str_in, tstr, 36, 40);
	this->record->TYC1 = atoi(tstr);

	slovoG(str_in, tstr, 41, 45);
	this->record->TYC2 = atoi(tstr);

	slovoG(str_in, tstr, 46, 47);
	this->record->TYC3 = atoi(tstr);

	slovoG(str_in, tstr, 48, 55);
	this->record->ASCC25 = atoi(tstr);

	slovoG(str_in, tstr, 56, 64);
	this->record->UCAC2 = atoi(tstr);

	slovoG(str_in, tstr, 65, 95);
	strcpy(this->record->OName, tstr);
	strcpy(&this->record->OName[30], "\0");

	slovoG(str_in, tstr, 96, 112);
	strcpy(this->record->MASS2, tstr);
	strcpy(&this->record->MASS2[16], "\0");

	slovoG(str_in, tstr, 113, 125);
	strcpy(this->record->USNOB1, tstr);
	strcpy(&this->record->USNOB1[12], "\0");

	slovoG(str_in, tstr, 127, 137);
	this->record->RAdeg = atof(tstr);

	slovoG(str_in, tstr, 138, 148);
	this->record->DEdeg = atof(tstr);

	slovoG(str_in, tstr, 149, 156);
	this->record->rPM = atof(tstr);

	slovoG(str_in, tstr, 157, 164);
	this->record->rPMRA = atof(tstr);

	slovoG(str_in, tstr, 165, 172);
	this->record->rPMDE = atof(tstr);

	slovoG(str_in, tstr, 173, 180);
	this->record->pm = atof(tstr);

	slovoG(str_in, tstr, 181, 188);
	this->record->pmRA = atof(tstr);

	slovoG(str_in, tstr, 189, 196);
	this->record->pmDE = atof(tstr);

	slovoG(str_in, tstr, 197, 199);
	strcpy(this->record->Aflag, tstr);
	strcpy(&this->record->Aflag[2], "\0");

	slovoG(str_in, tstr, 200, 205);
	this->record->Bmag = atof(tstr);

	slovoG(str_in, tstr, 206, 211);
	this->record->Vmag = atof(tstr);

	slovoG(str_in, tstr, 212, 216);
	this->record->BJmag = atof(tstr);

	slovoG(str_in, tstr, 217, 221);
	this->record->RFmag = atof(tstr);

	slovoG(str_in, tstr, 222, 226);
	this->record->INmag = atof(tstr);

	slovoG(str_in, tstr, 227, 232);
	this->record->Jmag = atof(tstr);

	slovoG(str_in, tstr, 233, 238);
	this->record->Hmag = atof(tstr);

	slovoG(str_in, tstr, 239, 244);
	this->record->Kmag = atof(tstr);

	slovoG(str_in, tstr, 245, 251);
	this->record->Vemag = atof(tstr);

	slovoG(str_in, tstr, 252, 257);
	this->record->VJ = atof(tstr);

	delete [] tstr;
}

void sscatFB::rec2s(char *str_out)
{
	char *tstr;
	tstr = new char[SIMPLE_STR];

	strcpy(tstr, "");
	strcpy(str_out, "");

	sprintf(tstr, "%s ", this->record->LPSM);
	strncat(str_out, tstr, 5);

	sprintf(tstr, "%s ", this->record->getLSPM());
	strncat(str_out, tstr, 12);

	sprintf(tstr, "%s ", this->record->LHS);
	strncat(str_out, tstr, 6);

	if(this->record->NLTT==0) sprintf(tstr, "      ");
	else sprintf(tstr, "%5d ", this->record->NLTT);
	strncat(str_out, tstr, 6);

	if(this->record->HIP==0) sprintf(tstr, "       ");
	else sprintf(tstr, "%6d ", this->record->HIP);
	strncat(str_out, tstr, 7);

	if(this->record->TYC1==0) sprintf(tstr, "     ");
	else sprintf(tstr, "%4d ", this->record->TYC1);
	strncat(str_out, tstr, 5);

	if(this->record->TYC2==0) sprintf(tstr, "     ");
	else sprintf(tstr, "%4d ", this->record->TYC2);
	strncat(str_out, tstr, 5);

	if(this->record->TYC3==0) sprintf(tstr, "  ");
	else sprintf(tstr, "%1d ", this->record->TYC3);
	strncat(str_out, tstr, 2);

	if(this->record->ASCC25==0) sprintf(tstr, "        ");
	else sprintf(tstr, "%7d ", this->record->ASCC25);
	strncat(str_out, tstr, 8);

	if(this->record->UCAC2==0) sprintf(tstr, "         ");
	else sprintf(tstr, "%8d ", this->record->UCAC2);
	strncat(str_out, tstr, 9);

	sprintf(tstr, "%s ", this->record->OName);
	strncat(str_out, tstr, 31);

	sprintf(tstr, "%s ", this->record->MASS2);
	strncat(str_out, tstr, 18);

	int len = strlen(this->record->USNOB1);
	sprintf(tstr, "%s  ", this->record->USNOB1);
	strncat(str_out, tstr, 15);

	sprintf(tstr, "%10.6f ", this->record->RAdeg);
	strncat(str_out, tstr, 11);

	sprintf(tstr, "%10.6f ", this->record->DEdeg);
	strncat(str_out, tstr, 11);

	sprintf(tstr, "%7.3f ", this->record->rPM);
	strncat(str_out, tstr, 8);

	sprintf(tstr, "%7.3f ", this->record->rPMRA);
	strncat(str_out, tstr, 8);

	sprintf(tstr, "%7.3f ", this->record->rPMDE);
	strncat(str_out, tstr, 8);

	sprintf(tstr, "%7.3f ", this->record->pm);
	strncat(str_out, tstr, 8);

	sprintf(tstr, "%7.3f ", this->record->pmRA);
	strncat(str_out, tstr, 8);

	sprintf(tstr, "%7.3f ", this->record->pmDE);
	strncat(str_out, tstr, 8);

	sprintf(tstr, "%s ", this->record->Aflag);
	strncat(str_out, tstr, 3);

	sprintf(tstr, "%5.2f ", this->record->Bmag);
	strncat(str_out, tstr, 6);

	sprintf(tstr, "%5.2f ", this->record->Vmag);
	strncat(str_out, tstr, 6);

	sprintf(tstr, "%4.1f ", this->record->BJmag);
	strncat(str_out, tstr, 5);

	sprintf(tstr, "%4.1f ", this->record->RFmag);
	strncat(str_out, tstr, 5);

	sprintf(tstr, "%4.1f ", this->record->INmag);
	strncat(str_out, tstr, 5);

	sprintf(tstr, "%5.2f ", this->record->Jmag);
	strncat(str_out, tstr, 6);

	sprintf(tstr, "%5.2f ", this->record->Hmag);
	strncat(str_out, tstr, 6);

	sprintf(tstr, "%5.2f ", this->record->Kmag);
	strncat(str_out, tstr, 6);
	
	sprintf(tstr, "%6.2f ", this->record->Vemag);
	strncat(str_out, tstr, 7);

	sprintf(tstr, "%5.2f", this->record->VJ);
	strncat(str_out, tstr, 6);

	strcat(&str_out[256], "\n\0");

	delete [] tstr;
}

int sscatFB::GetRec(int pos)
{
//	if(!is_istr||!is_iline) return 1;
	if(this->GetLine(pos)) return 1;
	this->s2rec(this->str);
	return 0;
}

int sscatFB::GetName(char *oname)
{
//	if(!is_istr) return 1;

	this->record->setLSPM(oname);
	char *tstr = new char[18];
	strncpy(tstr, this->record->getLSPM(), 11);
	strncpy(&tstr[11], "\0", 1);
	if(this->GetStr(tstr)) return 2;

	this->s2rec(str);
	delete [] tstr;
	return 0;
}

int sscatFB::DelRec(int pos)
{
	return(this->DelLine(pos));
}

int sscatFB::DelName(char *name)
{
	char *tstr = new char[12];
	strncpy(tstr, name, 11);
	strncpy(&tstr[11], "\0", 1);

	int res = this->DelStr(tstr);

	delete [] tstr;

	return res;
}

int sscatFB::AddRec(int pos)
{
//	if(!is_istr||!is_iline) return 1;
	char *new_str;
	new_str = new char[SSCATFB_REC];
	this->rec2s(new_str);
	this->AddLine(new_str, pos);

	delete [] new_str;
	return 0;
}

int sscatFB::AddRec()
{
	if(!is_istr||!is_iline) return 1;
	char *new_str;
	new_str = new char[STRLEN];
	this->rec2s(new_str);
	this->PushLine(new_str);

	delete [] new_str;
	return 0;
}

