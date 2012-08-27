#include "stdafx.h"
#include "mpcs.h"

mpc_head::mpc_head()
{
	this->Snum = new char[5];
	this->SnumTemp = new char[7];
	this->ss1 = new char[1];
	this->ss2 = new char[1];
	this->tipOfObs = new char[1];

	strcpy(this->Snum, "");
	strcpy(this->SnumTemp, "");
	strcpy(this->ss1, "");
	strcpy(this->ss2, "");
	strcpy(this->tipOfObs, "");

}

mpc_head::~mpc_head()
{
	delete [] this->Snum;
	delete [] this->SnumTemp;
	delete [] this->ss1;
	delete [] this->ss2;
	delete [] this->tipOfObs;
}

int mpc_head::get_head(char *head)
{
	if(head==NULL) return 1;

	sprintf(head, "%5s%7s%1s%1s%1s", this->Snum, this->SnumTemp, this->ss1, this->ss2, this->tipOfObs);

	return 0;
}

int mpc_head::cut_head(char *head)
{
	if(head==NULL) return 1;

	slovoG(head, this->Snum, 0, 5);
        slovoG(head, this->SnumTemp, 5, 12);
	slovoG(head, this->ss1, 12, 13);
	slovoG(head, this->ss2, 13, 14);
	slovoG(head, this->tipOfObs, 14, 15);

	if(strstr(this->Snum, " ")==NULL) this->numType = 0;
	if(strstr(this->SnumTemp, " ")==NULL) this->numType = 1;

	return 0;
}

int mpc_head::set_head(mpc_head *donor)
{
	if(donor==NULL) return 1;

	strcpy(this->Snum, donor->Snum);
	strcpy(this->SnumTemp, donor->SnumTemp);
	strcpy(this->ss1, donor->ss1);
	strcpy(this->ss2, donor->ss2);
	strcpy(this->tipOfObs, donor->tipOfObs);
}

void mpc_head::set_typeOfObs(char *otype)
{
    strncpy(tipOfObs, otype, 1);
    strcpy(&tipOfObs[1], "\0");
}

void mpc_head::set_Snum(int num)
{
    sprintf(Snum, "%05d", num);
}


/////////////mpc_tail

mpc_tail::mpc_tail()
{
        this->Smagn = new char[5];
	this->bukva = new char[25];
	this->SnumOfPubl = new char[25];
	this->SnumOfObservatory = new char[25];
		
	strcpy(this->Smagn, "");
	strcpy(this->bukva, "");
	strcpy(this->SnumOfPubl, "");
	strcpy(this->SnumOfObservatory, "");
}

mpc_tail::~mpc_tail()
{
	delete [] this->Smagn;
	delete [] this->bukva;
	delete [] this->SnumOfPubl;
	delete [] this->SnumOfObservatory;
}


int mpc_tail::get_tail(char *tail)
{
	if(tail==NULL) return 1;

        sprintf(tail, "         %5s%1s%6s%3s", this->Smagn, this->bukva, this->SnumOfPubl, this->SnumOfObservatory);

	return 0;
}
int mpc_tail::cut_tail(char *tail)
{
	if(tail==NULL) return 1;

	slovoG(tail, this->Smagn, 9, 14);
	slovoG(tail, this->bukva, 14, 15);
	slovoG(tail, this->SnumOfPubl, 15, 21);
	slovoG(tail, this->SnumOfObservatory, 21, 25);

	return 0;
}

int mpc_tail::set_tail(mpc_tail *donor)
{
	if(donor==NULL) return 1;

	strcpy(this->bukva, donor->bukva);
	strcpy(this->Smagn, donor->Smagn);
	strcpy(this->SnumOfObservatory, donor->SnumOfObservatory);
	strcpy(this->SnumOfPubl, donor->SnumOfPubl);

	return 0;
}

void mpc_tail::set_numOfObs(char *noobs)
{
    strncpy(SnumOfObservatory, noobs, 3);
    strcpy(&SnumOfObservatory[3], "\0");
}

void mpc_tail::set_magn(double magn)
{
    sprintf(Smagn, "%5.1f", magn);
}



//////////////mpc



mpc::mpc()
{
	this->head = new mpc_head;
	this->tail = new mpc_tail;

	tnull = new char[32];
	strcpy(tnull, "\x0D");
//	strcpy(tnull, "");

}

mpc::~mpc()
{
	delete(this->head);
	delete(this->tail);
	delete [] tnull;
}

int mpc::set_mpc(mpc *donor)
{
	if(donor==NULL) return 1;

	this->head->set_head(donor->head);
	this->tail->set_tail(donor->tail);

	this->eJD = donor->eJD;
	this->r = donor->r;
	this->d = donor->d;
	this->magn = donor->magn;
	this->numOfObservatory = donor->numOfObservatory;
	this->numOfPubl = donor->numOfPubl;

	return 0;
}
/*
int mpc::initstr(char *str)
{
	char *slo, *s;

	slo = new char[20];
	s = new char[1];

	int sign;
	double numb;

	int year, mth;

	slovoG(str, slo, 0, 15);
	strcpy(head, slo);
	strcpy(&head[15], "\0");

	slovoG(str, slo, 15, 19);
	year = atoi(slo);
	slovoG(str, slo, 20, 22);
	mth = atoi(slo);
	slovoG(str, slo, 23, 31);
	numb = atof(slo);
	dat2JD(&eJD, year, mth, numb);

	slovoG(str, s, 31, 32);
	slovoG(str, slo, 32, 34);
	year = atoi(slo);
	slovoG(str, slo, 35, 37);
	mth = atoi(slo);
	slovoG(str, slo, 38, 43);
	numb = atof(slo);

	sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
	hms2rad(&r, sign, year, mth, numb);		

	slovoG(str, s, 44, 45);
	slovoG(str, slo, 45, 47);
	year = atoi(slo);
	slovoG(str, slo, 48, 50);
	mth = atoi(slo);
	slovoG(str, slo, 51, 55);
	numb = atof(slo);

	sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
	gms2rad(&d, sign, year, mth, numb);

	slovoG(str, slo, 55, 80);
	strcpy(tail, slo);
	strcpy(&tail[26], "\0");

	return 0;
}
/
int mpc::headcut()
{
	slovoG(this->head, this->Snum, 0, 5);
	slovoG(this->head, this->SnumTemp, 5, 12);
	slovoG(this->head, this->ss1, 12, 13);
	slovoG(this->head, this->ss2, 13, 14);
	slovoG(this->head, this->tipOfObs, 14, 15);

	slovoG(this->tail, this->Smagn, 10, 14);
	slovoG(this->tail, this->bukva, 15, 16);
	slovoG(this->tail, this->SnumOfPubl, 16, 22);
	slovoG(this->tail, this->SnumOfObservatory, 22, 25);
	
	this->num = str2int(this->Snum);
	this->numTemp = str2int(this->SnumTemp);
	this->magn = atof(this->Smagn);
	this->numOfPubl = str2int(this->SnumOfPubl);
	this->numOfObservatory = str2int(this->SnumOfObservatory);

	return 0;
}
*/

/////////////////mpcL
mpcL::mpcL()
{
	this->head = new mpc_head;
	this->tail = new mpc_tail;

	tnull = new char[32];
	strcpy(tnull, "\x0D");
//	strcpy(tnull, "");

}

mpcL::~mpcL()
{
	delete(this->head);
	delete(this->tail);
	delete [] tnull;
}

int mpcL::set_mpc(mpcL *donor)
{
	if(donor==NULL) return 1;

	this->head->set_head(donor->head);
	this->tail->set_tail(donor->tail);

	this->eJD = donor->eJD;
	this->r = donor->r;
	this->d = donor->d;
	this->magn = donor->magn;
	this->numOfObservatory = donor->numOfObservatory;
	this->numOfPubl = donor->numOfPubl;

	return 0;
}

/////////////////mpcs
mpcs::mpcs()
{
	this->record = new mpc();
}

mpcs::~mpcs()
{
	delete(this->record);
}

int mpcs::init(char *fname)
{
    return(FileDynStrBin::init(fname));
}

int mpcs::init(char *fname, int sizestr)
{
    return(FileDynStrBin::init(fname));
}


int mpcs::addmpc(mpc *nrec, int pos)
{
	char *slo;
	slo = new char[128];
	strcpy(this->str, "");
	strcpy(slo, "");

	if(this->record->set_mpc(nrec)) return 1;

	this->rec2s(slo);
	if(this->AddStr(slo, pos)) return 1;

	return 0;
}

int mpcs::addmpc(mpc *nrec)
{
	char *slo;
	slo = new char[128];
	strcpy(this->str, "");
	strcpy(slo, "");

	if(this->record->set_mpc(nrec)) return 1;

	this->rec2s(slo);
    if(this->AddStr(slo, this->nstr())) return 1;

	return 0;
}

int mpcs::addmpc(int pos)
{
	char *slo;
	slo = new char[128];
	strcpy(this->str, "");
	strcpy(slo, "");

//	if(this->record->set_mpc(nrec)) return 1;

	this->rec2s(slo);
	if(this->AddStr(slo, pos)) return 1;

	return 0;
}

int mpcs::addmpc()
{
	char *slo;
	slo = new char[128];
	strcpy(this->str, "");
	strcpy(slo, "");

	this->rec2s(slo);
    if(this->AddStr(slo, this->nstr())) return 1;

	return 0;
}

int mpcs::delmpc(int pos)
{
	this->DelStr(pos);

	return 0;
}

int mpcs::getmpc(int pos)
{
    if(pos>this->nstr()-1) return 1;
	if(this->ReadStr(pos)) return 1;
	this->s2rec(this->str);
//	this->record->head->cut_head(this->record->head-

	return 0;
}

void mpcs::s2rec(char *arec)
{
	char *slo, *s;

	slo = new char[32];
	s = new char[32];

	int sign;
	double numb;

	int year, mth;

	slovoG(arec, slo, 0, 15);
	this->record->head->cut_head(slo);
	this->record->num = atoi(this->record->head->Snum);


	slovoG(arec, slo, 15, 19);
	year = atoi(slo);
	slovoG(arec, slo, 20, 22);
	mth = atoi(slo);

        //printf("%d %d\n", year, mth);

    /*switch(this->sizestr)
	{
    case 81:*/
        slovoG(arec, slo, 23, 32);
		numb = atof(slo);

		dat2JD(&this->record->eJD, year, mth, numb);

                //slovoG(arec, s, 31, 32);
		slovoG(arec, slo, 32, 34);
		year = atoi(slo);
		slovoG(arec, slo, 35, 37);
		mth = atoi(slo);
		slovoG(arec, slo, 38, 44);
		numb = atof(slo);

		sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
		hms2rad(&this->record->r, sign, year, mth, numb);		

		slovoG(arec, s, 44, 45);
		slovoG(arec, slo, 45, 47);
		year = atoi(slo);
		slovoG(arec, slo, 48, 50);
		mth = atoi(slo);
		slovoG(arec, slo, 51, 56);
		numb = atof(slo);

		sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
		gms2rad(&this->record->d, sign, year, mth, numb);

		slovoG(arec, slo, 56, 80);
		this->record->tail->cut_tail(slo);
		this->record->magn = atof(this->record->tail->Smagn);
		this->record->numOfObservatory = atoi(this->record->tail->SnumOfObservatory);
		this->record->numOfPubl = atoi(this->record->tail->SnumOfPubl);

		slovoG(arec, slo, 80, 81);
		strcpy(this->record->tnull, slo);
/*
		break;
    case 64:
		slovoG(arec, slo, 23, 34);
		numb = atof(slo);
		dat2JD(&this->record->eJD, year, mth, numb);

		slovoG(arec, s, 34, 35);
		slovoG(arec, slo, 35, 37);
		year = atoi(slo);
		slovoG(arec, slo, 38, 40);
		mth = atoi(slo);
		slovoG(arec, slo, 41, 49);
		numb = atof(slo);

		sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
		hms2rad(&this->record->r, sign, year, mth, numb);		

		slovoG(arec, s, 49, 50);
		slovoG(arec, slo, 50, 52);
		year = atoi(slo);
		slovoG(arec, slo, 53, 55);
		mth = atoi(slo);
		slovoG(arec, slo, 56, 63);
		numb = atof(slo);

		sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
		gms2rad(&this->record->d, sign, year, mth, numb);

		slovoG(arec, slo, 63, 63);
		this->record->tail->cut_tail(slo);
		this->record->magn = atof(this->record->tail->Smagn);
		this->record->numOfObservatory = atoi(this->record->tail->SnumOfObservatory);
		this->record->numOfPubl = atoi(this->record->tail->SnumOfPubl);

		slovoG(arec, slo, 63, 64);
		strcpy(this->record->tnull, slo);
        break;
	}
    */





	delete [] slo;
	delete [] s;

}

void mpcs::rec2s(char *arec)
{
//	if(arec==NULL) return 1;

	strcpy(arec, "");

	char *zero1, *zero2, *zero3, *zero4, *zero5, *s2;

	zero1 = new char[32];
	zero2 = new char[32];
	zero3 = new char[32];
	zero4 = new char[32];
	zero5 = new char[32];

	s2 = new char[32];

	strcpy(zero1, "");
	strcpy(zero2, "");
	strcpy(zero3, "");
	strcpy(zero4, "");
	strcpy(zero5, "");
	strcpy(s2, "+");

	

	int y, m;
	double d;

	int hour, min, grad, l;
	double sec, ll;
	int sign1, sign2;

        dat2YMD(this->record->eJD, &y, &m, &d);
	rad2hms(this->record->r, &sign1, &hour, &min, &sec);
	rad2gms(this->record->d, &sign2, &grad, &l, &ll);

	if(m<10) strcpy(zero1, "0");
	if(hour<10) strcpy(zero2, "0");
	if(min<10) strcpy(zero3, "0");
	if(grad<10) strcpy(zero4, "0");
	if(l<10) strcpy(zero5, "0");

	if(!sign2) strcpy(s2, "-");

	char *hd, *tl;
	hd = new char[128];
	tl = new char[128];

	this->record->head->get_head(hd);
	this->record->tail->get_tail(tl);


        sprintf(arec, "%s%4d %02d %09.6f%02d %02d %06.3f%s%02d %02d %05.2f%23s%s", hd, y, m, d, hour, min, sec, s2, grad, l, ll, tl, this->record->tnull);

	delete [] zero1;
	delete [] zero2;
	delete [] zero3;
	delete [] zero4;
	delete [] zero5;
	delete [] s2;
	delete [] hd;
	delete [] tl;

	
}

int mpcs::average(double *vra, double *vdec)
{
	int i, j, tlen, nt;
	tlen = 2;
    nt = this->nstr();

	myvector *xt, *xr, *xd, *xmagn;
	xt = new myvector(nt);
	xr = new myvector(nt);
	xd = new myvector(nt);
	xmagn = new myvector(nt);

	double d, t;
	t = 0.0;
    for(int i=0; i<this->nstr(); i++)
	{
		this->getmpc(i);
		xt->set(i, this->record->eJD);
		xr->set(i, this->record->r);
		xd->set(i, this->record->d);
		xmagn->set(i, this->record->magn);
	}

	t = det_average(xt);



	if(nt>=tlen)
	{	

		myvector *yVr, *xVr;
		myvector *yVd, *xVd;
		mymatrix *aM;
		myvector *sgr, *sgd;


		yVr = new myvector(nt);
		xVr = new myvector(tlen);
		sgr = new myvector(tlen);

		yVd = new myvector(nt);
		xVd = new myvector(tlen);
		sgd = new myvector(tlen);

		aM = new mymatrix(nt, tlen);

		int k;
		k = 0;

		for(i=0; i<nt; i++)
		{
			this->getmpc(i);
			yVr->set(k, this->record->r);
			yVd->set(k, this->record->d);
			for(j=0; j<tlen; j++) aM->set(k, j, pow(this->record->eJD - t, j)/fakt(j));
			k++;
		}

		MNK(aM, yVr, xVr, sgr);
		MNK(aM, yVd, xVd, sgd);


		this->record->eJD = t;
		this->record->r = xVr->get(0);
		this->record->d = xVd->get(0);
	//	this->r->acc = sgr->get(0);
		*vra = xVr->get(1);
		*vdec = xVd->get(1);
	//	this->rt->acc = sgr->get(1);
	//	this->rtt->val = xV->get(2);
	//	this->rtt->acc = sgr->get(2);

	//	rt = xV->get(0);
	//	rtt = xV->get(1);
		delete yVr;
		delete xVr;
		delete yVd;
		delete xVd;
		delete aM;
		delete sgr;
		delete sgd;

	}
	else
	{
		this->record->r = det_average(xr);
		this->record->d = det_average(xd);
	}

	this->record->magn = det_average(xmagn);
//	this->record->


	delete xt;
	delete xr;
	delete xd;
	delete xmagn;

	return 0;
}

int mpcs::GetCoordList(List4 *coord)
{
	if(coord==NULL) return 1;

	double *x;
	x = new double[3];

	coord->Free();

    for(int i=0; i<this->nstr(); i++)
	{
		if(this->getmpc(i)) return 2;

//		UTC2TDT(this->record->eJD, &x[0]);
		x[0] = this->record->eJD;
		x[1] = this->record->r;
		x[2] = this->record->d;

		this->record->tail->SnumOfObservatory;

		coord->Push(x, this->record->tail->SnumOfObservatory, -1, -1, 0);


	}

	return 0;
}


/*
int mpcs::delmpc(int num)
{
	if(num>=this->nmpcs) return 2;
	mpcs *before, *after;

	before = NULL;
	after = this->lmpc;

	int i;

	i=0;

	while((i<num))
	{
		before = after;
		if(before==NULL) return 1;

		after = after->next;

		i++;
	}

	if(before!=NULL)
	{
		before->next = after->next;
		delete(after);
	}
	else if(num==0) this->lmpc = this->lmpc->next;
		else return 1;

	this->nmpcs--;

	return 0;
}

int mpcs::selbynum(int num)
{
	mpc *temp;

	temp = new mpc(this->Fkey);
	
	int i = 0;
	
	while(!this->getmpc(temp, i))
	{
//		this->getmpc(temp, i);
		if(num!=temp->num)	this->delmpc(i);
		else i++;
	}

	return 0;
}

int mpcs::Uselbynum(int num)
{
	mpc *temp;

	temp = new mpc(this->Fkey);
	
	int i = 0;
	
	while(!this->getmpc(temp, i))
	{
//		this->getmpc(temp, i);
		if(num==temp->num)	this->delmpc(i);
		else i++;
	}

	return 0;
}


int mpcs::selbynumT(int numT)
{
	mpc *temp;

	temp = new mpc(this->Fkey);

//	temp = new mpc;

//	temp = this->lmpc;
	int i = 0;

	while(!this->getmpc(temp, i))
	{
		temp->headcut();
//		this->getmpc(temp, i);
		if(numT!=temp->numTemp)
		{
//			temp = temp->next->next;
			this->delmpc(i);
		}
		else i++;

//		temp = temp->next;
//		i++;
	}

	return 0;
}

int mpcs::UselbynumT(int numT)
{
	mpc *temp;

	temp = new mpc(this->Fkey);

//	temp = new mpc;

//	temp = this->lmpc;
	int i = 0;

	while(!this->getmpc(temp, i))
	{
		temp->headcut();
//		this->getmpc(temp, i);
		if(numT==temp->numTemp)
		{
//			temp = temp->next->next;
			this->delmpc(i);
		}
		else i++;

//		temp = temp->next;
//		i++;
	}

	return 0;
}

int mpcs::selbytime(double time, double dtime)
{
	mpc *temp;

	temp = new mpc(this->Fkey);

//	temp = this->lmpc;
	int i = 0;

	while(!this->getmpc(temp, i))
	{
//		this->getmpc(temp, i);
		if(fabs(temp->eJD - time)>dtime) this->delmpc(i);
		else i++;
//		temp = temp->next;
//		i++;
	}

	return 0;
}

int mpcs::Uselbytime(double time, double dtime)
{
	mpc *temp;

	temp = new mpc(this->Fkey);

//	temp = this->lmpc;
	int i = 0;

	while(!this->getmpc(temp, i))
	{
//		this->getmpc(temp, i);
		if(fabs(temp->eJD - time)<=dtime) this->delmpc(i);
		else i++;
//		temp = temp->next;
//		i++;
	}

	return 0;
}
*/
/*
int mpcs::initFile(char *fname)
{
	if(this->init(fname, MPC_SIZE);


	if(this->detFkey(fname)) return 1;
	FILE *FN;
	char *str, *nstr;

//	int STRLEN;
//	STRLEN = 1000;

	str = new char[STRLEN];
	nstr = new char[STRLEN];

	FN = fopen(fname, "r");
	if(FN==NULL) return 1;

	int i;
	i=0;

	strcpy(str, "");
	strcpy(nstr, "");

	int year, mth;
	int sign;
	double numb;
	char *slo, *s;

	char *head, *tail;

	double eJD, r, d;

	slo = new char[20];
	s = new char[1];

	switch(this->Fkey)
	{
	case 0:
		head = new char[14];
		tail = new char[25];
		strcpy(nstr, str);
		fgets(str, STRLEN, FN);

		while(strcmp(str, nstr)!=0)
		{
			//
			slovoG(str, slo, 0, 15);
			strcpy(head, slo);
			strcpy(&head[15], "\0");

			slovoG(str, slo, 15, 19);
			year = atoi(slo);
			slovoG(str, slo, 20, 22);
			mth = atoi(slo);
			slovoG(str, slo, 23, 31);
			numb = atof(slo);
			dat2JD(&eJD, year, mth, numb);

			slovoG(str, s, 31, 32);
			slovoG(str, slo, 32, 34);
			year = atoi(slo);
			slovoG(str, slo, 35, 37);
			mth = atoi(slo);
			slovoG(str, slo, 38, 43);
			numb = atof(slo);

			sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
			hms2rad(&r, sign, year, mth, numb);		

			slovoG(str, s, 44, 45);
			slovoG(str, slo, 45, 47);
			year = atoi(slo);
			slovoG(str, slo, 48, 50);
			mth = atoi(slo);
			slovoG(str, slo, 51, 55);
			numb = atof(slo);

			sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
			gms2rad(&d, sign, year, mth, numb);

			slovoG(str, slo, 55, 80);
			strcpy(tail, slo);
			strcpy(&tail[26], "\0");

			strcpy(nstr, str);
			fgets(str, STRLEN, FN);
			//

			if(strcmp(str, nstr)==0) return 0;
			addmpc(head, eJD, r, d, tail);
//			if(streqv(str, "")) return 0;
//			if(strcmp(str, nstr)==0) return 0;

		}
		break;
	case 1:
		this->hat = new char[1000];
		strcpy(this->hat, "");
		for(i=0; i<6; i++)
		{
			strcpy(nstr, str);
			fgets(str, STRLEN, FN);
			strcat(this->hat, str);
		}
		strcpy(nstr, str);
		fgets(str, STRLEN, FN);

		head = new char[2];
		tail = new char[42];

		while(strcmp(str, nstr)!=0)
		{
			slovoG(str, slo, 0, 2);
			strcpy(head, slo);
			strcpy(&head[3], "\0");

			slovoG(str, slo, 2, 6);
			year = atoi(slo);
			slovoG(str, slo, 7, 9);
			mth = atoi(slo);
			slovoG(str, slo, 10, 20);
			numb = atof(slo);
			dat2JD(&eJD, year, mth, numb);

			slovoG(str, s, 21, 22);
			slovoG(str, slo, 22, 24);
			year = atoi(slo);
			slovoG(str, slo, 25, 27);
			mth = atoi(slo);
			slovoG(str, slo, 28, 35);
			numb = atof(slo);

			sign = (streqv(s, " "))||(streqv(s, "+"));
			hms2rad(&r, sign, year, mth, numb); 

			slovoG(str, s, 36, 37);
			slovoG(str, slo, 37, 39);
			year = atoi(slo);
			slovoG(str, slo, 40, 42);
			mth = atoi(slo);
			slovoG(str, slo, 43, 49);
			numb = atof(slo);

			sign = (streqv(s, " "))||(streqv(s, "+"));
			gms2rad(&d, sign, year, mth, numb);

			slovoG(str, slo, 49, 91);
			strcpy(tail, slo);
			strcpy(&tail[42], "\0");

			strcpy(nstr, str);
			fgets(str, STRLEN, FN);	

			if(strcmp(str, nstr)==0) return 0;
			addmpc(head, eJD, r, d, tail);
		}
		break;
	case 2:
		this->hat = new char[1000];
		strcpy(this->hat, "");
		for(i=0; i<6; i++)
		{
			strcpy(nstr, str);
			fgets(str, STRLEN, FN);
			strcat(this->hat, str);
		}
		strcpy(nstr, str);
		fgets(str, STRLEN, FN);

		head = new char[2];
		tail = new char[42];

		while(strcmp(str, nstr)!=0)
		{
			slovoG(str, slo, 0, 2);
			strcpy(head, slo);
			strcpy(&head[3], "\0");

			slovoG(str, slo, 2, 6);
			year = atoi(slo);
			slovoG(str, slo, 7, 9);
			mth = atoi(slo);
			slovoG(str, slo, 10, 20);
			numb = atof(slo);
			dat2JD(&eJD, year, mth, numb);

			slovoG(str, s, 21, 22);
			slovoG(str, slo, 22, 24);
			year = atoi(slo);
			slovoG(str, slo, 25, 27);
			mth = atoi(slo);
			slovoG(str, slo, 28, 35);
			numb = atof(slo);

			sign = (streqv(s, " "))||(streqv(s, "+"));
			hms2rad(&r, sign, year, mth, numb); 

			slovoG(str, s, 36, 37);
			slovoG(str, slo, 37, 39);
			year = atoi(slo);
			slovoG(str, slo, 40, 42);
			mth = atoi(slo);
			slovoG(str, slo, 43, 49);
			numb = atof(slo);

			sign = (streqv(s, " "))||(streqv(s, "+"));
			gms2rad(&d, sign, year, mth, numb);

			slovoG(str, slo, 49, 91);
			strcpy(tail, slo);
			strcpy(&tail[42], "\0");

			strcpy(nstr, str);
			fgets(str, STRLEN, FN);	

			if(strcmp(str, nstr)==0) return 0;
			addmpc(head, eJD, r, d, tail);
		}
		break;
	}


	delete(slo);
	delete(s);
//	delete(sign);
	delete(str);
	delete(nstr);
	fclose(FN);
	return 0;
}
*/
