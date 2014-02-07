#include "stdafx.h"
#include "mpccat.h"
/////////////////	Bowell	////////////////////////////

mpcrec::mpcrec()
{
	provnum = new char[8];
	epoch = new char[6];
	ref = new char[11];
	flag_multi = new char[2];
	days = new char[5];
	indCoarse = new char[4];
	indPrecide = new char[4];
	compName = new char[11];
	flags = new char[5];
	number = new char[9];
	name = new char[19];
	lastObs = new char[9];


	this->set_default();
}

mpcrec::~mpcrec()
{
	delete [] provnum;
	delete [] epoch;
	delete [] ref;
	delete [] flag_multi;
	delete [] days;
	delete [] indCoarse;
	delete [] indPrecide;
	delete [] flags;
	delete [] number;
	delete [] name;
	delete [] lastObs;
}

void mpcrec::copyTo(mpcrec *nrec)
{
	strncpy(nrec->provnum, this->provnum, 7);
	strcpy(&nrec->provnum[7], "\0");

	nrec->H = this->H;
	nrec->G = this->G;

	strncpy(nrec->epoch, this->epoch, 5);
	strcpy(&nrec->epoch[5], "\0");

	nrec->meanA = this->meanA;
	nrec->w = this->w;
	nrec->Node = this->Node;
	nrec->inc = this->inc;
    nrec->ecc = this->ecc;
    nrec->n = this->n;
    nrec->a = this->a;
    nrec->U = this->U;

	strncpy(nrec->ref, this->ref, 10);
	strcpy(&nrec->ref[10], "\0");

    nrec->obsnum = this->obsnum;
    nrec->oppnum = this->oppnum;
	strncpy(nrec->flag_multi, this->flag_multi, 1);
	strcpy(&nrec->flag_multi[1], "\0");
	strncpy(nrec->days, this->days, 4);
	strcpy(&nrec->days[4], "\0");

    nrec->obs_fst = this->obs_fst;
    nrec->obs_lst = this->obs_lst;

    nrec->arclen = this->arclen;
//	nrec->days = days;

    nrec->rms = this->rms;

	strncpy(nrec->indCoarse, this->indCoarse, 3);
	strcpy(&nrec->indCoarse[3], "\0");

	strncpy(nrec->indPrecide, this->indPrecide, 3);
	strcpy(&nrec->indPrecide[3], "\0");

	strncpy(nrec->compName, this->compName, 10);
	strcpy(&nrec->compName[10], "\0");

	strncpy(nrec->flags, this->flags, 4);
	strcpy(&nrec->flags[4], "\0");

	strncpy(nrec->number, this->number, 8);
	strcpy(&nrec->number[8], "\0");

	strncpy(nrec->name, this->name, 18);
	strcpy(&nrec->name[18], "\0");

}

void mpcrec::set_default()
{
	strncpy(this->provnum, "       ", 7);
	strcpy(&this->provnum[7], "\0");

	this->H = 0.0;
	this->G = 0.0;

	strncpy(this->epoch, "     ", 5);
	strcpy(&this->epoch[5], "\0");

	this->meanA = 0.0;
	this->w = 0.0;
	this->Node = 0.0;
	this->inc = 0.0;
	this->ecc = 0.0;
	this->n = 0.0;
	this->a = 0.0;
	this->U = 0.0;

	strncpy(this->ref, "          ", 10);
	strcpy(&this->ref[10], "\0");

	this->obsnum = 0;
	this->oppnum = 0;
	strncpy(this->flag_multi, " \0", 2);
	strncpy(this->days, "days\0", 5);
	
	this->obs_fst = 0;
	this->obs_lst = 0;

	this->arclen = 0;

	this->rms = 0.0;

	strncpy(this->indCoarse, "   ", 3);
	strcpy(&this->indCoarse[3], "\0");

	strncpy(this->indPrecide, "   ", 3);
	strcpy(&this->indPrecide[3], "\0");

	strncpy(this->compName, "          ", 10);
	strcpy(&this->compName[10], "\0");

	strncpy(this->flags, "    ", 4);
	strcpy(&this->flags[4], "\0");

	strncpy(this->number, "        ", 8);
	strcpy(&this->number[8], "\0");

	strncpy(this->name, "                  ", 18);
	strcpy(&this->name[18], "\0");

	strncpy(this->lastObs, "        ", 8);
	strcpy(&this->lastObs[8], "\0");
}

void mpcrec::copyTo(eposrec *nrec)
{
	nrec->number = this->getNum();
	nrec->eJD = this->getEpoch();
	nrec->epoha = 2000;

	
	nrec->M0 = this->meanA;
	nrec->w = this->w;
	nrec->Node = this->Node;
	nrec->inc = this->inc;
    nrec->ecc = this->ecc;
	nrec->q = this->a*(1.0 - this->ecc);
	nrec->H = this->H;
	nrec->G = this->G;

//	nrec->perturb = this->perturb;
	nrec->set_pr("1111111111111");

	nrec->opp = this->oppnum;
	nrec->obs = this->obsnum;
	nrec->obsY = this->obs_fst;
	if(streqv(this->flag_multi, "-")) nrec->regY = this->obs_lst;
	else nrec->regY = 0;

	nrec->set_un(" .00");
	
	nrec->set_name(this->name);
	nrec->set_author(this->compName);

	nrec->set_makeD(&this->lastObs[2]);
	nrec->diam = 0.0;


}

int mpcrec::getProvnum()
{
	char *sim = new char[2];
	strncpy(sim, "\0", 1);
	strncpy(&sim[0], "\0", 1);
        char *sim1 = new char[3];
	strncpy(sim1, "\0", 1);
	strncpy(&sim1[0], "\0", 1);
	char *upstr = new char[16];
	strncpy(upstr, "\0", 1);
	strncpy(&upstr[0], "\0", 1);

	for(int i=0; i<7; i++)
	{
                strncpy(sim, &this->provnum[i], 1);
		strcpy(&sim[1], "\0");

                upackDigStr(sim, sim1);
		strcat(upstr, sim1);

	}

	int res = atoi(upstr);

	delete [] sim;
	delete [] sim1;
	delete [] upstr;


	return res;
	
}

void mpcrec::getProvnumStr(char *rstr)
{
/*	char *sim = new char[2];
        strncpy(sim, "\0", 1);
        strncpy(&sim[0], "\0", 1);
        char *sim1 = new char[3];
        strncpy(sim1, "\0", 1);
        strncpy(&sim1[0], "\0", 1);
        char *upstr = new char[16];
        strncpy(upstr, "\0", 1);
        strncpy(&upstr[0], "\0", 1);

        for(int i=0; i<7; i++)
        {
                strncpy(sim, &this->provnum[i], 1);
                strcpy(&sim[1], "\0");

                upackDigStr(sim, sim1);
                strcat(upstr, sim1);

        }
*/
        strcpy(rstr, this->provnum);
/*
        delete [] sim;
        delete [] sim1;
        delete [] upstr;


        return res;
*/
}

int mpcrec::getNum()
{
	int res, i;
	char *sim = new char[2];
	strcpy(sim, "\0");

	for(i=0; i<8; i++)
	{
		strncpy(sim, &number[i], 1);
		strcpy(&sim[1], "\0");
		if(streqv(sim, "(")) return atoi(&number[i+1]);
	}

	return 0;
}

void mpcrec::getNumStr(char *rstr)
{
        int res, i, p;
        char *tstr = new char[8];
        char *sim = new char[2];
        strcpy(sim, "\0");
        strcpy(tstr, "");
        p=0;
        for(i=0; i<8; i++)
        {
                strncpy(sim, &number[i], 1);
                strcpy(&sim[1], "\0");
                if((!streqv(sim, "("))&&(!streqv(sim, ")")))
                {
                    strncat(tstr, sim, 1);
                    p++;
                }
        }
        sprintf(rstr, "%d\0", atoi(tstr));

        delete [] tstr;
        delete [] sim;
}

double mpcrec::getEpoch()
{
	char *sim = new char[20];
	strncpy(sim, "\0", 1);
	strncpy(&sim[0], "\0", 1);
	char *sim1 = new char[20];
	strncpy(sim1, "\0", 1);
	strncpy(&sim1[0], "\0", 1);
	char *upstr = new char[16];
	strncpy(upstr, "\0", 1);
	strncpy(&upstr[0], "\0", 1);

        char *yearStr = new char[4];
        strcpy(yearStr,"");

	for(int i=0; i<5; i++)
	{
		strncpy(sim, &this->epoch[i], 1);
		strcpy(&sim[1], "\0");

                upackDigStr(sim, sim1);
		if(i>2)
		{
			sprintf(sim, "%02d", atoi(sim1));
			strcat(upstr, sim);
		}
		else strcat(upstr, sim1);

	}

        strncpy(sim, &this->epoch[0], 1);
        strcpy(&sim[1], "\0");
        upackDigStr(sim, sim1);
        strncat(yearStr, sim1, 2);
        strncat(yearStr, &this->epoch[1], 2);
        int year = atoi(yearStr);

        strcpy(yearStr,"");
        strncpy(sim, &this->epoch[3], 1);
        strcpy(&sim[1], "\0");
        upackDigStr(sim, sim1);
        //strncat(yearStr, sim1, 2);
        int month = atoi(sim1);//strncat(yearStr, &this->epoch[1], 2);

        strcpy(yearStr,"");
        strncpy(sim, &this->epoch[4], 1);
        strcpy(&sim[1], "\0");
        upackDigStr(sim, sim1);
        double day = atoi(sim1);//-0.5;

        double jday;

	dat2JD(&jday, year, month, day);

	delete [] sim;
	delete [] sim1;
	delete [] upstr;
        delete [] yearStr;

	return jday;
}
////////////////////////////////////////////////////////////////////////////////////

mpccat::mpccat(char *fn) : fbStreamStr(fn)
{
	this->record = new mpcrec;
	this->init_str(MPCCAT_LEN, MPCCAT_NAME_KPOS);
}

mpccat::mpccat() : fbStreamStr()
{
	this->record = new mpcrec;
	this->init_str(MPCCAT_LEN, MPCCAT_NAME_KPOS);
}

mpccat::~mpccat()
{
	delete(this->record);
}

int mpccat::init(char *fname)
{
    this->init_str(MPCCAT_LEN, MPCCAT_NAME_KPOS);
        return(fbStreamStr::init(fname));
}

int mpccat::init(char *fname, int catKpos)
{
	this->init_str(MPCCAT_LEN, catKpos);
        return(fbStreamStr::init(fname));
}
/*
int mpccat::GetRec(int pos)
{
	if(this->ReadStr(pos)) return 1;
	this->s2rec(this->str);
	return 0;
}
*/
int mpccat::GetRecName(char *name)
{
	if(this->GetStr(name)) return 1;
	this->s2rec(this->str);
	
    return 0;
}

int mpccat::GetRecNum(char *name)
{
    char *brname = new char[256];
    sprintf(brname, "(%s)", name);
    if(this->Get(str, brname, MPCCAT_NUM_KPOS, MPCCAT_LEN, MPCCAT_NAME_BPOS)) return 1;
    this->s2rec(this->str);

    return 0;
}

int mpccat::GetProvDest(char *name)
{
    //printf("name: %s", name);
    if(this->Get(str, name, MPCCAT_PDEST_KPOS, MPCCAT_LEN, MPCCAT_NAME_BPOS)) return 1;
    //printf("str: %s\n", str);
    this->s2rec(this->str);

    return 0;
}

int mpccat::getRegExpName(QString name, QList<mpcrec *> recL)
{

}
/*
int mpccat::GetRecName(char *name)
{
	if(this->GetStr(name)) return 1;
	this->s2rec(this->str);
	
	return 0;
}
/

int mpccat::GetRecNum(int num)
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
/*
int mpccat::AddRec(int pos)
{
	char *new_str;
	new_str = new char[STRLEN];
	this->rec2s(new_str);
	this->AddStr(new_str, pos);

	delete [] new_str;
	return 0;
}

int mpccat::AddRec()
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
*/
/*
void mpccat::rec2s(char *str_out)
{
	char *tstr;
	tstr = new char[254];

	strcpy(tstr, "");
	strcpy(str_out, "");

	sprintf(tstr, "%8d", this->record->number);
	strncat(str_out, tstr, 8);

//	sprintf(tstr, "%4d", this->record->epoha);
	strncat(str_out, tstr, 4);

//	sprintf(tstr, "%9.1f", this->record->eJD);
	strncat(str_out, tstr, 9);

	sprintf(tstr, "%10.6f", this->record->M0);
	strncat(str_out, tstr, 10);

	sprintf(tstr, "%10.6f", this->record->w);
	strncat(str_out, tstr, 10);

	sprintf(tstr, "%10.6f", this->record->Node);
	strncat(str_out, tstr, 10);

	sprintf(tstr, "%10.6f", this->record->inc);
	strncat(str_out, tstr, 10);

//	sprintf(tstr, "%010.8f", this->record->ec);
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

//	sprintf(tstr, "%s", this->record->author);
	strncat(str_out, tstr, 16);

	sprintf(tstr, "%s", this->record->makeD);
	strncat(str_out, tstr, 6);

//	sprintf(tstr, "%7.2f", this->record->diam);
	strncat(str_out, tstr, 7);

	strcat(str_out, this->record->tail);

	delete [] tstr;
}
*/

void mpccat::s2rec(char *str_in)
{
	char *tstr;
	tstr = new char[60];
	strcpy(tstr, "\0");

	slovoG(str_in, tstr, 0, 7);
	strncpy(this->record->provnum, tstr, 7);
	strcpy(&this->record->provnum[7], "\0");

	slovoG(str_in, tstr, 8, 13);
	this->record->H = atof(tstr);

	slovoG(str_in, tstr, 14, 19);
	this->record->G = atof(tstr);

	slovoG(str_in, tstr, 20, 25);
	strncpy(this->record->epoch, tstr, 5);
	strcpy(&this->record->epoch[5], "\0");

	slovoG(str_in, tstr, 26, 35);
	this->record->meanA = atof(tstr);

	slovoG(str_in, tstr, 36, 46);
	this->record->w = atof(tstr);

	slovoG(str_in, tstr, 47, 57);
	this->record->Node = atof(tstr);

	slovoG(str_in, tstr, 58, 68);
	this->record->inc = atof(tstr);

	slovoG(str_in, tstr, 69, 79);
	this->record->ecc = atof(tstr);

	slovoG(str_in, tstr, 80, 91);
	this->record->n = atof(tstr);

	slovoG(str_in, tstr, 92, 103);
	this->record->a = atof(tstr);

	slovoG(str_in, tstr, 104, 106);
	this->record->U = atoi(tstr);

	slovoG(str_in, tstr, 107, 117);
	strncpy(this->record->ref, tstr, 10);
	strcpy(&this->record->ref[10], "\0");

	slovoG(str_in, tstr, 118, 122);
	this->record->obsnum = atoi(tstr);

	slovoG(str_in, tstr, 123, 126);
	this->record->oppnum = atoi(tstr);

	slovoG(str_in, tstr, 127, 131);
	this->record->arclen = this->record->obs_fst = atoi(tstr);

	slovoG(str_in, tstr, 131, 132);
	strncpy(this->record->flag_multi, tstr, 1);
	strcpy(&this->record->flag_multi[1], "\0");

	slovoG(str_in, tstr, 132, 136);
	this->record->obs_lst = atoi(tstr);
	strncpy(this->record->days, tstr, 4);
	strcpy(&this->record->days[4], "\0");

	slovoG(str_in, tstr, 137, 141);
	this->record->rms = atof(tstr);

	slovoG(str_in, tstr, 142, 145);
	strncpy(this->record->indCoarse, tstr, 3);
	strcpy(&this->record->indCoarse[3], "\0");

	slovoG(str_in, tstr, 146, 149);
	strncpy(this->record->indPrecide, tstr, 3);
	strcpy(&this->record->indPrecide[3], "\0");

	slovoG(str_in, tstr, 150, 160);
	strncpy(this->record->compName, tstr, 10);
	strcpy(&this->record->compName[10], "\0");

	slovoG(str_in, tstr, 161, 165);
	strncpy(this->record->flags, tstr, 4);
	strcpy(&this->record->flags[4], "\0");

	slovoG(str_in, tstr, 166, 174);
	strncpy(this->record->number, tstr, 8);
	strcpy(&this->record->number[8], "\0");

	slovoG(str_in, tstr, 175, 193);
	strncpy(this->record->name, tstr, 18);
	strcpy(&this->record->name[18], "\0");

	slovoG(str_in, tstr, 194, 202);
	strncpy(this->record->lastObs, tstr, 8);
	strcpy(&this->record->lastObs[8], "\0");

	delete [] tstr;
}

///////////////////////////////////////////////////////////////

mpccatL::mpccatL(char *fn) : fbStreamLine(fn)
{
    this->record = new mpcrec;
    init_line(MPCCAT_NAME_BPOS);
    this->init_str(MPCCAT_LEN, MPCCAT_NAME_KPOS);
}

mpccatL::mpccatL() : fbStreamLine()
{
    this->record = new mpcrec;
    init_line(MPCCAT_NAME_BPOS);
    this->init_str(MPCCAT_LEN, MPCCAT_NAME_KPOS);
}

mpccatL::~mpccatL()
{
    delete(this->record);
}

int mpccatL::init(char *fname)
{
    init_line(MPCCAT_NAME_BPOS);
    this->init_str(MPCCAT_LEN, MPCCAT_NAME_KPOS);
        return(fbStreamStr::init(fname));
}


void mpccatL::s2rec(char *str_in)
{
    char *tstr;
    tstr = new char[60];
    strcpy(tstr, "\0");

    slovoG(str_in, tstr, 0, 7);
    strncpy(this->record->provnum, tstr, 7);
    strcpy(&this->record->provnum[7], "\0");

    slovoG(str_in, tstr, 8, 13);
    this->record->H = atof(tstr);

    slovoG(str_in, tstr, 14, 19);
    this->record->G = atof(tstr);

    slovoG(str_in, tstr, 20, 25);
    strncpy(this->record->epoch, tstr, 5);
    strcpy(&this->record->epoch[5], "\0");

    slovoG(str_in, tstr, 26, 35);
    this->record->meanA = atof(tstr);

    slovoG(str_in, tstr, 36, 46);
    this->record->w = atof(tstr);

    slovoG(str_in, tstr, 47, 57);
    this->record->Node = atof(tstr);

    slovoG(str_in, tstr, 58, 68);
    this->record->inc = atof(tstr);

    slovoG(str_in, tstr, 69, 79);
    this->record->ecc = atof(tstr);

    slovoG(str_in, tstr, 80, 91);
    this->record->n = atof(tstr);

    slovoG(str_in, tstr, 92, 103);
    this->record->a = atof(tstr);

    slovoG(str_in, tstr, 104, 106);
    this->record->U = atoi(tstr);

    slovoG(str_in, tstr, 107, 117);
    strncpy(this->record->ref, tstr, 10);
    strcpy(&this->record->ref[10], "\0");

    slovoG(str_in, tstr, 118, 122);
    this->record->obsnum = atoi(tstr);

    slovoG(str_in, tstr, 123, 126);
    this->record->oppnum = atoi(tstr);

    slovoG(str_in, tstr, 127, 131);
    this->record->arclen = this->record->obs_fst = atoi(tstr);

    slovoG(str_in, tstr, 131, 132);
    strncpy(this->record->flag_multi, tstr, 1);
    strcpy(&this->record->flag_multi[1], "\0");

    slovoG(str_in, tstr, 132, 136);
    this->record->obs_lst = atoi(tstr);
    strncpy(this->record->days, tstr, 4);
    strcpy(&this->record->days[4], "\0");

    slovoG(str_in, tstr, 137, 141);
    this->record->rms = atof(tstr);

    slovoG(str_in, tstr, 142, 145);
    strncpy(this->record->indCoarse, tstr, 3);
    strcpy(&this->record->indCoarse[3], "\0");

    slovoG(str_in, tstr, 146, 149);
    strncpy(this->record->indPrecide, tstr, 3);
    strcpy(&this->record->indPrecide[3], "\0");

    slovoG(str_in, tstr, 150, 160);
    strncpy(this->record->compName, tstr, 10);
    strcpy(&this->record->compName[10], "\0");

    slovoG(str_in, tstr, 161, 165);
    strncpy(this->record->flags, tstr, 4);
    strcpy(&this->record->flags[4], "\0");

    slovoG(str_in, tstr, 166, 174);
    strncpy(this->record->number, tstr, 8);
    strcpy(&this->record->number[8], "\0");

    slovoG(str_in, tstr, 175, 193);
    strncpy(this->record->name, tstr, 18);
    strcpy(&this->record->name[18], "\0");

    slovoG(str_in, tstr, 194, 202);
    strncpy(this->record->lastObs, tstr, 8);
    strcpy(&this->record->lastObs[8], "\0");

    delete [] tstr;
}

int mpccatL::GetRec(int pos)
{
    if(GetLine(pos)) return 1;
    s2rec(this->str);
}

int mpccatL::PushRec()
{

}

//int mpccatL::GetRecName(char *name);


