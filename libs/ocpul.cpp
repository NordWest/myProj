#include "stdafx.h"
#include "ocpul.h"

ocpul_head::ocpul_head()
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

ocpul_head::~ocpul_head()
{
	delete [] this->Snum;
	delete [] this->SnumTemp;
	delete [] this->ss1;
	delete [] this->ss2;
	delete [] this->tipOfObs;
}

int ocpul_head::get_head(char *head)
{
	if(head==NULL) return 1;

	sprintf(head, "%5s%7s%1s%1s%1s", this->Snum, this->SnumTemp, this->ss1, this->ss2, this->tipOfObs);

	return 0;
}

int ocpul_head::cut_head(char *head)
{
	if(head==NULL) return 1;

	slovoG(head, this->Snum, 0, 5);
	slovoG(head, this->SnumTemp, 4, 12);
	slovoG(head, this->ss1, 12, 13);
	slovoG(head, this->ss2, 13, 14);
	slovoG(head, this->tipOfObs, 14, 15);

	if(strstr(this->Snum, " ")==NULL) this->numType = 0;
	if(strstr(this->SnumTemp, " ")==NULL) this->numType = 1;

	return 0;
}

int ocpul_head::set_head(ocpul_head *donor)
{
	if(donor==NULL) return 1;

	strcpy(this->Snum, donor->Snum);
	strcpy(this->SnumTemp, donor->SnumTemp);
	strcpy(this->ss1, donor->ss1);
	strcpy(this->ss2, donor->ss2);
	strcpy(this->tipOfObs, donor->tipOfObs);
}


/////////////ocpul_tail

ocpul_tail::ocpul_tail()
{
	this->Smagn = new char[4];
	this->bukva = new char[25];
	this->SnumOfPubl = new char[25];
	this->SnumOfObservatory = new char[25];
		
	strcpy(this->Smagn, "");
	strcpy(this->bukva, "");
	strcpy(this->SnumOfPubl, "");
	strcpy(this->SnumOfObservatory, "");
}

ocpul_tail::~ocpul_tail()
{
	delete [] this->Smagn;
	delete [] this->bukva;
	delete [] this->SnumOfPubl;
	delete [] this->SnumOfObservatory;
}


int ocpul_tail::get_tail(char *tail)
{
	if(tail==NULL) return 1;

	sprintf(tail, "         %4s%1s%6s%3s", this->Smagn, this->bukva, this->SnumOfPubl, this->SnumOfObservatory);

	return 0;
}
int ocpul_tail::cut_tail(char *tail)
{
	if(tail==NULL) return 1;

	slovoG(tail, this->Smagn, 9, 14);
	slovoG(tail, this->bukva, 14, 15);
	slovoG(tail, this->SnumOfPubl, 15, 21);
	slovoG(tail, this->SnumOfObservatory, 21, 25);

	return 0;
}

int ocpul_tail::set_tail(ocpul_tail *donor)
{
	if(donor==NULL) return 1;

	strcpy(this->bukva, donor->bukva);
	strcpy(this->Smagn, donor->Smagn);
	strcpy(this->SnumOfObservatory, donor->SnumOfObservatory);
	strcpy(this->SnumOfPubl, donor->SnumOfPubl);

	return 0;
}


//////////////ocpul



ocpul::ocpul()
{
//	this->head = new ocpul_head;
//	this->tail = new ocpul_tail;

	this->catalog = new char[32];
	this->coorg_type = new char[32];
	this->instrument = new char[32];
	this->name = new char[32];
	this->rec_type = new char[32];
	tnull = new char[32];
	strcpy(tnull, "\x0D");
//	strcpy(tnull, "");

}

ocpul::~ocpul()
{
//	delete(this->head);
//	delete(this->tail);
	delete [] tnull;
}

int ocpul::set_ocpul(ocpul *donor)
{
	if(donor==NULL) return 1;

//	this->head->set_head(donor->head);
//	this->tail->set_tail(donor->tail);

	this->eJD = donor->eJD;
	this->r = donor->r;
	this->d = donor->d;
	this->d_oc = donor->d_oc;
	this->epoch = donor->epoch;
	strcpy(this->catalog, donor->catalog);
	strcpy(this->instrument, donor->instrument);
	strcpy(this->name, donor->name);
	strcpy(this->rec_type, donor->rec_type);
	strcpy(this->coorg_type, donor->coorg_type);
	
	return 0;
}
/*
int ocpul::initstr(char *str)
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
int ocpul::headcut()
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

/////////////////ocpulL
ocpulL::ocpulL()
{
	this->head = new ocpul_head;
	this->tail = new ocpul_tail;

	tnull = new char[32];
	strcpy(tnull, "\x0D");
//	strcpy(tnull, "");

}

ocpulL::~ocpulL()
{
	delete(this->head);
	delete(this->tail);
	delete [] tnull;
}

int ocpulL::set_ocpul(ocpulL *donor)
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

/////////////////ocpuls
ocpuls::ocpuls()
{
	this->record = new ocpul();
}

ocpuls::~ocpuls()
{
	delete(this->record);
}

int ocpuls::init(char *fname)
{
	return(FileDynStr::init(fname, OCPUL_REC));
}

int ocpuls::init(char *fname, int sizestr)
{
	return(FileDynStr::init(fname, sizestr));
}

/*
int ocpuls::add(ocpul *nrec, int pos)
{
	char *slo;
	slo = new char[128];
	strcpy(this->str, "");
	strcpy(slo, "");

	if(this->record->set_ocpul(nrec)) return 1;

	this->rec2s(slo);
	if(this->AddStr(slo, pos)) return 1;

	return 0;
}

int ocpuls::add(ocpul *nrec)
{
	char *slo;
	slo = new char[128];
	strcpy(this->str, "");
	strcpy(slo, "");

	if(this->record->set_ocpul(nrec)) return 1;

	this->rec2s(slo);
	if(this->AddStr(slo, this->nstr)) return 1;

	return 0;
}

int ocpuls::add(int pos)
{
	char *slo;
	slo = new char[128];
	strcpy(this->str, "");
	strcpy(slo, "");

//	if(this->record->set_ocpul(nrec)) return 1;

	this->rec2s(slo);
	if(this->AddStr(slo, pos)) return 1;

	return 0;
}

int ocpuls::add()
{
	char *slo;
	slo = new char[128];
	strcpy(this->str, "");
	strcpy(slo, "");

	this->rec2s(slo);
	if(this->AddStr(slo, this->nstr)) return 1;

	return 0;
}

int ocpuls::del(int pos)
{
	this->DelStr(pos);

	return 0;
}
*/
int ocpuls::get(int pos)
{
	if(pos>this->nstr-1) return 1;
	if(this->ReadStr(pos)) return 1;
	this->s2rec(this->str);
//	this->record->head->cut_head(this->record->head-

	return 0;
}

void ocpuls::s2rec(char *arec)
{
	char *slo, *s;

	slo = new char[32];
	s = new char[32];

	int arlen = strlen(arec);

	if(arlen==0) return;

	int sign;
	double numb;

	int year, mth;
	int posb, pose;

	slovo(arec, slo, 0, &pose);
	strcpy(this->record->name, slo);
	this->record->num = atoi(this->record->name);

	if(abs(pose)>arlen) return;

	posb = pose;
	slovo(arec, slo, posb, &pose);
	year = atoi(slo);
	if(abs(pose)>arlen) return;
	posb = pose;
	slovo(arec, slo, posb, &pose);
	
	mth = atoi(slo);
	if(abs(pose)>arlen) return;
	posb = pose;
	slovo(arec, slo, posb, &pose);
	
	numb = atof(slo);

	dat2JD(&this->record->eJD, year, mth, numb);

	if(abs(pose)>arlen) return;
	posb = pose;

	slovoG(arec, slo, posb, posb+1);
	strcpy(s, slo);

	slovo(arec, slo, posb+1, &pose);
	year = atoi(slo);

	if(abs(pose)>arlen) return;
	posb = pose;
	slovo(arec, slo, posb, &pose);
	mth = atoi(slo);

	if(abs(pose)>arlen) return;
	posb = pose;
	slovo(arec, slo, posb, &pose);
	numb = atof(slo);

//	sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
	hms2rad(&this->record->r, 1, year, mth, numb);		

	if(abs(pose)>arlen) return;
	posb = pose;

	slovoG(arec, slo, posb, posb+1);
	strcpy(s, slo);

	slovo(arec, slo, posb, &pose);
	year = atoi(slo);

	if(abs(pose)>arlen) return;
	posb = pose;
	slovo(arec, slo, posb, &pose);
	mth = atoi(slo);
	
	if(abs(pose)>arlen) return;
	posb = pose;
	slovo(arec, slo, posb, &pose);
	numb = atof(slo);

	sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
	gms2rad(&this->record->d, sign, year, mth, numb);

	if(abs(pose)>arlen) return;
	posb = pose;
	slovo(arec, slo, posb, &pose);
	this->record->r_oc = atof(slo);

	if(abs(pose)>arlen) return;
	posb = pose;
	slovo(arec, slo, posb, &pose);
	this->record->d_oc = atof(slo);

	if(abs(pose)>arlen) return;
	posb = pose;
	slovo(arec, slo, posb, &pose);
	this->record->epoch = atof(slo);

	if(abs(pose)>arlen) return;
	posb = pose;
	slovo(arec, slo, posb, &pose);
	strcpy(this->record->catalog, slo);

	if(abs(pose)>arlen) return;
	posb = pose;
	slovo(arec, slo, posb, &pose);
	strcpy(this->record->coorg_type, slo);

	if(abs(pose)>arlen) return;
	posb = pose;
	slovo(arec, slo, posb, &pose);
	strcpy(this->record->instrument, slo);

	if(abs(pose)>arlen) return;
	posb = pose;
	slovo(arec, slo, posb, &pose);
	strcpy(this->record->rec_type, slo);


	delete [] slo;
	delete [] s;

}
/*
void ocpuls::rec2s(char *arec)
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


	sprintf(arec, "%s%4d %s%d %8.5f %s%d %s%d %6.3f%s%s%d %s%d %5.2f%s%s", hd, y, zero1, m, d, zero2, hour, zero3, min, sec, s2, zero4, grad, zero5, l, ll, tl, this->record->tnull);

	delete [] zero1;
	delete [] zero2;
	delete [] zero3;
	delete [] zero4;
	delete [] zero5;
	delete [] s2;
	delete [] hd;
	delete [] tl;

	
}
*/
/*
int ocpuls::average(double *vra, double *vdec)
{
	int i, j, tlen, nt;
	tlen = 2;
	nt = this->nstr;

	myvector *xt, *xr, *xd, *xmagn;
	xt = new myvector(nt);
	xr = new myvector(nt);
	xd = new myvector(nt);
	xmagn = new myvector(nt);

	double d, t;
	t = 0.0;
	for(int i=0; i<this->nstr; i++)
	{
		this->get(i);
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
			this->get(i);
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
*/
int ocpuls::GetCoordList(List4 *coord)
{
	if(coord==NULL) return 1;

	double *x;
	x = new double[5];

	coord->Free();

	for(int i=0; i<this->nstr; i++)
	{
		if(this->get(i)) return 2;

//		UTC2TDT(this->record->eJD, &x[0]);
		x[0] = this->record->eJD;
		x[1] = this->record->r;
		x[2] = this->record->d;
		x[3] = this->record->r_oc;
		x[4] = this->record->d_oc;

//		this->record->tail->SnumOfObservatory;

		coord->Push(x, "84", -1, -1, 0);


	}

	return 0;
}


/*
int ocpuls::add(int num)
{
	if(num>=this->nocpuls) return 2;
	ocpuls *before, *after;

	before = NULL;
	after = this->locpul;

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
	else if(num==0) this->locpul = this->locpul->next;
		else return 1;

	this->nocpuls--;

	return 0;
}

int ocpuls::selbynum(int num)
{
	ocpul *temp;

	temp = new ocpul(this->Fkey);
	
	int i = 0;
	
	while(!this->get(temp, i))
	{
//		this->get(temp, i);
		if(num!=temp->num)	this->add(i);
		else i++;
	}

	return 0;
}

int ocpuls::Uselbynum(int num)
{
	ocpul *temp;

	temp = new ocpul(this->Fkey);
	
	int i = 0;
	
	while(!this->get(temp, i))
	{
//		this->get(temp, i);
		if(num==temp->num)	this->add(i);
		else i++;
	}

	return 0;
}


int ocpuls::selbynumT(int numT)
{
	ocpul *temp;

	temp = new ocpul(this->Fkey);

//	temp = new ocpul;

//	temp = this->locpul;
	int i = 0;

	while(!this->get(temp, i))
	{
		temp->headcut();
//		this->get(temp, i);
		if(numT!=temp->numTemp)
		{
//			temp = temp->next->next;
			this->add(i);
		}
		else i++;

//		temp = temp->next;
//		i++;
	}

	return 0;
}

int ocpuls::UselbynumT(int numT)
{
	ocpul *temp;

	temp = new ocpul(this->Fkey);

//	temp = new ocpul;

//	temp = this->locpul;
	int i = 0;

	while(!this->get(temp, i))
	{
		temp->headcut();
//		this->get(temp, i);
		if(numT==temp->numTemp)
		{
//			temp = temp->next->next;
			this->add(i);
		}
		else i++;

//		temp = temp->next;
//		i++;
	}

	return 0;
}

int ocpuls::selbytime(double time, double dtime)
{
	ocpul *temp;

	temp = new ocpul(this->Fkey);

//	temp = this->locpul;
	int i = 0;

	while(!this->get(temp, i))
	{
//		this->get(temp, i);
		if(fabs(temp->eJD - time)>dtime) this->add(i);
		else i++;
//		temp = temp->next;
//		i++;
	}

	return 0;
}

int ocpuls::Uselbytime(double time, double dtime)
{
	ocpul *temp;

	temp = new ocpul(this->Fkey);

//	temp = this->locpul;
	int i = 0;

	while(!this->get(temp, i))
	{
//		this->get(temp, i);
		if(fabs(temp->eJD - time)<=dtime) this->add(i);
		else i++;
//		temp = temp->next;
//		i++;
	}

	return 0;
}
*/
/*
int ocpuls::initFile(char *fname)
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
			add(head, eJD, r, d, tail);
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
			add(head, eJD, r, d, tail);
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
			add(head, eJD, r, d, tail);
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