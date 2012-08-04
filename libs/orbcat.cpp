//#include "stdafx.h"
#include "orbcat.h"
/////////////////	Bowell	////////////////////////////

eposrec::eposrec()
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

eposrec::~eposrec()
{
	delete [] name;
	delete [] author;
	delete [] un;
	delete [] perturb;
	delete [] makeD;

	delete [] tail;
}

void eposrec::copyTo(eposrec *nrec)
{
	nrec->number = this->number;
	nrec->epoha = this->epoha;
	nrec->eJD = this->eJD;
	nrec->M0 = this->M0;
	nrec->w = this->w;
	nrec->Node = this->Node;
	nrec->inc = this->inc;
	nrec->ec = this->ec;
	nrec->q = this->q;
	nrec->H = this->H;
	nrec->G = this->G;
	strncpy(nrec->perturb, this->perturb, 32);
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

void eposrec::set_number(int numb)
{
	this->number = numb;
}

void eposrec::set_epoha(int epo)
{
	this->epoha = epo;
}
void eposrec::set_eJD(double eJD)
{
	this->eJD = eJD;
}
void eposrec::set_M0(double m0)
{
	this->M0 = m0;
}
void eposrec::set_w(double w)
{
	this->w = w;
}
void eposrec::set_Node(double node)
{
	this->Node = Node;
}
void eposrec::set_ec(double ec)
{
	this->ec = ec;
}
void eposrec::set_inc(double inc)
{
	this->inc = inc;
}
void eposrec::set_q(double q)
{
	this->q = q;
}
void eposrec::set_H(float H)
{
	this->H = H;
}
void eposrec::set_G(float G)
{
	this->G = G;
}
void eposrec::set_pr(char *pr)
{
	index(this->perturb, pr, "0", 16);
}
void eposrec::set_opp(int opp)
{
	this->opp = opp;
}
void eposrec::set_obs(int obs)
{
	this->obs = obs;
}
void eposrec::set_obsY(int obsY)
{
	this->obsY = obsY;
}
void eposrec::set_regY(int regY)
{
	this->regY = regY;
}
void eposrec::set_un(char *un)
{
	indexB(this->un, un, " ", 17);
}
void eposrec::set_name(char *name)
{
	indexB(this->name, name, " ", 16);
}

void eposrec::set_author(char *author)
{
	indexB(this->author, author, " ", 16);
}

void eposrec::set_makeD(char *makeD)
{
	index(this->makeD, makeD, " ", 6);
}
void eposrec::set_diam(float diam)
{
	this->diam = diam;
}

void eposrec::set_default()
{
	this->set_diam(0.0);
	this->set_ec(0.0);
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

OrbCat::OrbCat(char *fn, int size) : fBuffer(fn, size)
{
	this->record = new eposrec;
}

OrbCat::OrbCat() : fBuffer(EPOS5_CAT)
{
	this->record = new eposrec;
}

OrbCat::~OrbCat()
{
//	delete [] this->str;
//	delete this->buffer;
//	fBuffer::~fBuffer();
//	if(record!=NULL) delete(this->record);
}

int OrbCat::init(char *fname)
{
	return(fBuffer::init(fname, EPOS5_CAT));
}

int OrbCat::GetRec(int pos)
{
	if(this->ReadStr(pos)) return 1;
	this->s2rec(this->str);
	return 0;
}

int OrbCat::GetRecName(char *name)
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

int OrbCat::GetRecNum(int num)
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

int OrbCat::AddRec(int pos)
{
	char *new_str;
	new_str = new char[STRLEN];
	this->rec2s(new_str);
	this->AddStr(new_str, pos);

	delete [] new_str;
	return 0;
}

int OrbCat::AddRec()
{
	int pos;
	char *new_str;
	new_str = new char[STRLEN];


	this->rec2s(new_str);

	pos = this->GetRecName(this->record->name);

	if(pos!=-1) this->Update(new_str, pos);
	else this->AddStr(new_str, this->nstr);

	delete [] new_str;
	return 0;
}

void OrbCat::rec2s(char *str_out)
{
	char *tstr;
	tstr = new char[254];

	strcpy(tstr, "");
	strcpy(str_out, "");

	sprintf(tstr, "%8d", this->record->number);
	strncat(str_out, tstr, 8);

	sprintf(tstr, "%4d", this->record->epoha);
	strncat(str_out, tstr, 4);

	sprintf(tstr, "%9.1f", this->record->eJD);
	strncat(str_out, tstr, 9);

	sprintf(tstr, "%10.6f", this->record->M0);
	strncat(str_out, tstr, 10);

	sprintf(tstr, "%10.6f", this->record->w);
	strncat(str_out, tstr, 10);

	sprintf(tstr, "%10.6f", this->record->Node);
	strncat(str_out, tstr, 10);

	sprintf(tstr, "%10.6f", this->record->inc);
	strncat(str_out, tstr, 10);

	sprintf(tstr, "%010.8f", this->record->ec);
	strncat(str_out, tstr, 10);

	sprintf(tstr, "%16.9f", this->record->q);
	strncat(str_out, tstr, 16);

	sprintf(tstr, "%5.2f ", this->record->H);
	strncat(str_out, tstr, 6);

	sprintf(tstr, "%4.2f", this->record->G);
	strncat(str_out, tstr, 4);

	sprintf(tstr, "%s", this->record->perturb);
	strncat(str_out, tstr, 16);

	sprintf(tstr, "%3d ", this->record->opp);
	strncat(str_out, tstr, 4);

	sprintf(tstr, "%4d", this->record->obs);
	strncat(str_out, tstr, 4);

	sprintf(tstr, "%4d", this->record->obsY);
	strncat(str_out, tstr, 4);

	sprintf(tstr, "%4d", this->record->regY);
	strncat(str_out, tstr, 4);

	sprintf(tstr, "%s", this->record->un);
	strncat(str_out, tstr, 17);

	sprintf(tstr, "%s", this->record->name);
	strncat(str_out, tstr, 16);

	sprintf(tstr, "%s", this->record->author);
	strncat(str_out, tstr, 16);

	sprintf(tstr, "%s", this->record->makeD);
	strncat(str_out, tstr, 6);

	sprintf(tstr, "%7.2f", this->record->diam);
	strncat(str_out, tstr, 7);

	strcat(str_out, this->record->tail);

	delete [] tstr;
}

void OrbCat::s2rec(char *str_in)
{
	char *tstr;
	tstr = new char[30];

	slovoG(str_in, tstr, 0, 8);
	this->record->number = atoi(tstr);

	slovoG(str_in, tstr, 8, 12);
	this->record->epoha = atoi(tstr);

	slovoG(str_in, tstr, 12, 21);
	this->record->eJD = atof(tstr);

	slovoG(str_in, tstr, 21, 31);
	this->record->M0 = atof(tstr);

	slovoG(str_in, tstr, 31, 41);
	this->record->w = atof(tstr);

	slovoG(str_in, tstr, 41, 51);
	this->record->Node = atof(tstr);

	slovoG(str_in, tstr, 51, 61);
	this->record->inc = atof(tstr);

	slovoG(str_in, tstr, 61, 71);
	this->record->ec = atof(tstr);

	slovoG(str_in, tstr, 71, 87);
	this->record->q = atof(tstr);

	slovoG(str_in, tstr, 87, 92);
	this->record->H = atof(tstr);

	slovoG(str_in, tstr, 93, 97);
	this->record->G = atof(tstr);

	slovoG(str_in, tstr, 97, 113);
	strcpy(this->record->perturb, tstr);
	strcpy(&this->record->perturb[16], "\0");

	slovoG(str_in, tstr, 113, 116);
	this->record->opp = atoi(tstr);

	slovoG(str_in, tstr, 117, 121);
	this->record->obs = atoi(tstr);

	slovoG(str_in, tstr, 121, 125);
	this->record->obsY = atoi(tstr);

	slovoG(str_in, tstr, 125, 129);
	this->record->regY = atoi(tstr);
	
	slovoG(str_in, tstr, 129, 146);
	strcpy(this->record->un, tstr);
	strcpy(&this->record->un[17], "\0");

	slovoG(str_in, tstr, 146, 162);
	strcpy(this->record->name, tstr);
	strcpy(&this->record->name[16], "\0");

	slovoG(str_in, tstr, 162, 178);
	strcpy(this->record->author, tstr);
	strcpy(&this->record->author[16], "\0");

	slovoG(str_in, tstr, 178, 184);
	strcpy(this->record->makeD, tstr);
	strcpy(&this->record->makeD[6], "\0");

	slovoG(str_in, tstr, 184, 191);
	this->record->diam = atof(tstr);

	slovoG(str_in, tstr, 191, EPOS5_CAT);
	strcpy(this->record->tail, tstr);

	delete [] tstr;
}
///////////////////////////////////////////////////////////////

