#include "stdafx.h"
#include "commetorbcat.h"
/////////////////	Bowell	////////////////////////////

come5_rec::come5_rec()
{
	name = new char[32];
	author = new char[32];
	un = new char[32];
	perturb = new char[32];
	makeD = new char[32];

	tail = new char[32];
	strcpy(tail, "\x0D");

	this->set_default();
}

come5_rec::~come5_rec()
{
	delete [] name;
	delete [] author;
	delete [] un;
	delete [] perturb;

	delete [] tail;
}

void come5_rec::copyTo(come5_rec *nrec)
{
	nrec->number = this->number;
	nrec->epoha = this->epoha;
	nrec->eJD = this->eJD;
	nrec->M0 = this->M0;
	nrec->w = this->w;
	nrec->Node = this->Node;
	nrec->inc = this->inc;
    nrec->ecc = this->ecc;
	nrec->q = this->q;
	nrec->H = this->H;
	nrec->G = this->G;
	nrec->perturb = this->perturb;
	nrec->opp = this->opp;
	nrec->obs = this->obs;
	nrec->obsY = this->obsY;
	nrec->regY = this->regY;
	nrec->un = this->un;
	
	strcpy(nrec->name, this->name);
	strcpy(nrec->author, this->author);

	nrec->makeD = this->makeD;
	nrec->diam = this->diam;

	strcpy(nrec->tail, this->tail);
}

void come5_rec::set_number(int numb)
{
	this->number = numb;
}

void come5_rec::set_epoha(int epo)
{
	this->epoha = epo;
}
void come5_rec::set_eJD(double eJD)
{
	this->eJD = eJD;
}
void come5_rec::set_M0(double m0)
{
	this->M0 = m0;
}
void come5_rec::set_w(double w)
{
	this->w = w;
}
void come5_rec::set_Node(double node)
{
	this->Node = Node;
}
void come5_rec::set_ecc(double ecc)
{
    this->ecc = ecc;
}
void come5_rec::set_inc(double inc)
{
	this->inc = inc;
}
void come5_rec::set_q(double q)
{
	this->q = q;
}
void come5_rec::set_H(float H)
{
	this->H = H;
}
void come5_rec::set_G(float G)
{
	this->G = G;
}
void come5_rec::set_pr(char *pr)
{
	index(this->perturb, pr, "0", 16);
}
void come5_rec::set_opp(int opp)
{
	this->opp = opp;
}
void come5_rec::set_obs(int obs)
{
	this->obs = obs;
}
void come5_rec::set_obsY(int obsY)
{
	this->obsY = obsY;
}
void come5_rec::set_regY(int regY)
{
	this->regY = regY;
}
void come5_rec::set_un(char *un)
{
	indexB(this->un, un, " ", 17);
}
void come5_rec::set_name(char *name)
{
	indexB(this->name, name, " ", 32);
}

void come5_rec::set_author(char *author)
{
	indexB(this->author, author, " ", 16);
}

void come5_rec::set_makeD(char *makeD)
{
	index(this->makeD, makeD, " ", 6);
}
void come5_rec::set_diam(float diam)
{
	this->diam = diam;
}

void come5_rec::set_default()
{
	this->set_diam(0.0);
    this->set_ecc(0.0);
	this->set_eJD(JDEPO);
	this->set_epoha(2000);
	this->set_G(0.0);
	this->set_H(0.0);
	this->set_inc(0.0);
	this->set_M0(0.0);
	this->set_makeD("070101");
	this->set_name("Noname");
	this->set_author("Nobody");
	this->set_Node(0.0);
	this->set_number(0);
	this->set_obs(0);
	this->set_obsY(0);
	this->set_opp(0);
	this->set_pr("1111111111111000");
	this->set_q(1.0);
	this->set_regY(0);
	this->set_un("  .00");
	this->set_w(0.0);
}

CommetOrbCat::CommetOrbCat(char *fn, int size) : FileDynStrBin(fn)
{
	this->record = new come5_rec;
}

CommetOrbCat::CommetOrbCat() : FileDynStrBin(COMET_E5_CAT)
{
	this->record = new come5_rec;
}

CommetOrbCat::~CommetOrbCat()
{
	delete(this->record);
}

int CommetOrbCat::init(char *fname)
{
        return(FileDynStrBin::init(fname));
}

int CommetOrbCat::GetRec(int pos)
{
	if(this->ReadStr(pos)) return 1;
	this->s2rec(this->str);
	return 0;
}

int CommetOrbCat::GetRecName(char *name)
{
	int pos = 0;

	while(!this->ReadStr(pos))
	{
		if(strstr(this->str, name)!=NULL)
		{
			this->s2rec(this->str);
			return pos;
		}

		pos++;
	}
	
	return -1;
}
/*
int CommetOrbCat::GetRecNum(int num)
{
	int pos = 0;

	while(!this->ReadStr(pos))
	{
		this->s2rec(this->str);
		if(this->record->number == num)
		{
//			this->s2rec(this->str);
			return pos;
		}

		pos++;
	}
	
	return -1;
}
*/
int CommetOrbCat::AddRec(int pos)
{
	char *new_str;
	new_str = new char[STRLEN];
	this->rec2s(new_str);
	this->AddStr(new_str, pos);

	delete [] new_str;
	return 0;
}

int CommetOrbCat::AddRec()
{
	int pos;
	char *new_str;
	new_str = new char[STRLEN];


	this->rec2s(new_str);

	pos = this->GetRecName(this->record->name);

	if(pos!=-1) this->Update(new_str, pos);
        else this->AddStr(new_str, this->nstr());

	delete [] new_str;
	return 0;
}

void CommetOrbCat::rec2s(char *str_out)
{
	char *tstr;
	tstr = new char[254];

	strcpy(tstr, "");
	strcpy(str_out, "");

	sprintf(tstr, "        ");
	strncat(str_out, tstr, 8);

	sprintf(tstr, "%4d", this->record->epoha);
	strncat(str_out, tstr, 4);

	sprintf(tstr, "%9.1f", this->record->eJD);
	strncat(str_out, tstr, 9);

	sprintf(tstr, "%13.5f", this->record->M0);
	strncat(str_out, tstr, 13);

	sprintf(tstr, "%10.6f", this->record->w);
	strncat(str_out, tstr, 10);

	sprintf(tstr, "%10.6f", this->record->Node);
	strncat(str_out, tstr, 10);

	sprintf(tstr, "%10.6f", this->record->inc);
	strncat(str_out, tstr, 10);

    sprintf(tstr, "%010.8f", this->record->ecc);
	strncat(str_out, tstr, 10);

	sprintf(tstr, "%16.9f", this->record->q);
	strncat(str_out, tstr, 16);

	sprintf(tstr, "%5.2f ", this->record->H);
	strncat(str_out, tstr, 5);

	sprintf(tstr, "%5.2f", this->record->G);
	strncat(str_out, tstr, 5);

	sprintf(tstr, "%16s", this->record->perturb);
	strncat(str_out, tstr, 16);
/*
	sprintf(tstr, "%3d ", this->record->opp);
	strncat(str_out, tstr, 4);
*/
	sprintf(tstr, "%8d", this->record->obs);
	strncat(str_out, tstr, 8);

	sprintf(tstr, "%4d", this->record->obsY);
	strncat(str_out, tstr, 4);

	sprintf(tstr, "%4d", this->record->regY);
	strncat(str_out, tstr, 4);

	sprintf(tstr, "%5s", this->record->un);
	strncat(str_out, tstr, 5);

	sprintf(tstr, "%12s", this->record->author);
	strncat(str_out, tstr, 12);

	sprintf(tstr, "%32s", this->record->name);
	strncat(str_out, tstr, 32);

	
	sprintf(tstr, "%6s", this->record->makeD);
	strncat(str_out, tstr, 6);

	sprintf(tstr, "       ");
	strncat(str_out, tstr, 7);

	strcat(str_out, this->record->tail);

	delete [] tstr;
}

void CommetOrbCat::s2rec(char *str_in)
{
	char *tstr;
	tstr = new char[30];

	slovoG(str_in, tstr, 0, 8);
	this->record->number = atoi(tstr);

	slovoG(str_in, tstr, 8, 12);
	this->record->epoha = atoi(tstr);

	slovoG(str_in, tstr, 12, 21);
	this->record->eJD = atof(tstr);

	slovoG(str_in, tstr, 21, 34);
	this->record->M0 = atof(tstr);

	slovoG(str_in, tstr, 34, 44);
	this->record->w = atof(tstr);

	slovoG(str_in, tstr, 44, 54);
	this->record->Node = atof(tstr);

	slovoG(str_in, tstr, 54, 64);
	this->record->inc = atof(tstr);

	slovoG(str_in, tstr, 64, 74);
    this->record->ecc = atof(tstr);

	slovoG(str_in, tstr, 74, 90);
	this->record->q = atof(tstr);

	slovoG(str_in, tstr, 90, 95);
	this->record->H = atof(tstr);

	slovoG(str_in, tstr, 95, 100);
	this->record->G = atof(tstr);

	slovoG(str_in, tstr, 100, 116);
	strcpy(this->record->perturb, tstr);
	strcpy(&this->record->perturb[16], "\0");

	slovoG(str_in, tstr, 116, 124);
	this->record->obs = atoi(tstr);

	slovoG(str_in, tstr, 124, 128);
	this->record->obsY = atoi(tstr);

	slovoG(str_in, tstr, 128, 132);
	this->record->regY = atoi(tstr);
/*
	slovoG(str_in, tstr, 132, 136);
	this->record->regY = atoi(tstr);
*/	
	slovoG(str_in, tstr, 132, 137);
	strcpy(this->record->un, tstr);
	strcpy(&this->record->un[5], "\0");

	slovoG(str_in, tstr, 137, 149);
	strcpy(this->record->author, tstr);
	strcpy(&this->record->author[16], "\0");

	slovoG(str_in, tstr, 149, 181);
	strcpy(this->record->name, tstr);
	strcpy(&this->record->name[32], "\0");

	slovoG(str_in, tstr, 181, 187);
	strcpy(this->record->makeD, tstr);
	strcpy(&this->record->makeD[6], "\0");

	slovoG(str_in, tstr, 187, 194);
	this->record->diam = atof(tstr);

	slovoG(str_in, tstr, 194, COMET_E5_CAT);
	strcpy(this->record->tail, tstr);

//	delete [] tstr;
}
///////////////////////////////////////////////////////////////

