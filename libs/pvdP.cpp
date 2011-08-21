#include "stdafx.h"

#include "pvdP.h"

#define PI 3.141592653589
#define ka 0.017202098955
#define EPS 1.0e-8
#define DEPTHSCAN 40

#ifndef EKV
#define EKV 0.4093197552027299
#endif


double modVect(double *vect)
{
	return(sqrt(vect[0]*vect[0] + vect[1]*vect[1] + vect[2]*vect[2]));
}

pvdP::pvdP()
{
//	this->sun = NULL;
	this->vpos = new pvdK();

	this->comvar = NULL;
	this->res = NULL;
	this->slog = NULL;

	this->resCat = NULL;
	this->obs = NULL;

	this->klog = 0;
	this->fini = 0;

	is_init_files = 0;
	is_init_data = 0;
	is_init_params = 0;

	this->iniPath = new char[256];
	this->suff = new char[256];
//	this->mpcFile = new char[FNLEN];

	strcpy(this->iniPath, "");
	strcpy(this->suff, "none");

	this->comvar = new ComVar;
	comvar->obj_name = new char[16];
	strcpy(comvar->obj_name, "      ");

	this->comvar->numOfPR = 0;

	comvar->testVal = new double[38];
}

pvdP::~pvdP()
{
//	delete(this->sun);
	delete(obs);
	delete(resCat);
	delete(this->vpos);
	delete(this->res);
	delete(this->slog);
	delete [] this->iniPath;
	delete [] this->suff;
//	delete [] this->mpcFile;
	delete [] comvar->testVal;

	delete [] this->comvar->obj_name;
	delete this->comvar;

	if(this->baseCat!=NULL) delete baseCat;
}
/*
pvdP::pvdP(int log)
{
//	this->sun = NULL;
	this->vpos = NULL;
	this->comvar = NULL;
	this->res = NULL;

	this->resCat = NULL;
	this->obs = NULL;

	this->slog = NULL;
	this->klog = log;
	this->fini = 0;
/*

	this->klog = 0;
	if(this->klog)
	{
		int len;
		char *flogn;

		flogn = new char[25];

		len = strlen(this->suff);

		strcpy(flogn, "");

		strncpy(flogn, this->suff, len);
		strcpy(&flogn[len], "\0");
		strcat(flogn, "_pP.log");
		strcpy(&flogn[len+7], "\0");

		this->FLOG = fopen(flogn, "w");
	}
/
}
/*
pvdP::pvdP(char *fn_dele_head, char *fn_dele, char *fn_obs, char *fnpvd)
{
//	this->sun = NULL;
	this->vpos = NULL;
	this->comvar = NULL;
	this->res = NULL;
	this->slog = NULL;

	this->resCat = NULL;
	this->obs = NULL;

	this->klog = 0;
	this->fini = 0;
	this->logging((char*)NULL);

	this->initFiles(fn_dele_head, fn_dele, fn_obs, fnpvd, NULL);
}
/*
pvdP::pvdP(char *fn_dele_head, char *fn_dele, char *fn_obs, char *fnpvd, char *fnini, int log)
{
//	this->sun = NULL;
	this->vpos = NULL;
	this->comvar = NULL;
	this->res = NULL;
	this->slog = NULL;

	this->resCat = NULL;
	this->obs = NULL;

	this->klog = log;
	this->logging(fnpvd);

	this->initFiles(fn_dele_head, fn_dele, fn_obs, fnpvd, fnini);
}
*/
int pvdP::init()
{

	return 0;
}

int pvdP::initFiles(char *fn_dele_head, char *fn_dele, char *fn_obs)
{
	if(!this->is_init_params) return 1;

	this->obs = new observ(fn_dele_head);
	this->vpos = new pvdK();

//	this->coord = new List4(3);

	if(this->obs->init(fn_obs, fn_dele_head, fn_dele)) return 1;
	
	this->obs->set_obs_parpam(13, CENTER_SUN, SK_EKVATOR, "000"); 

	is_init_files = 1;

	return 0;
}

int pvdP::initFiles()
{
	if(!this->is_init_params) return 1;

	char *fn_dele_head, *fn_dele, *fn_obs;
	fn_dele_head = new char[FNLEN];
	fn_dele = new char[FNLEN];
	fn_obs = new char[FNLEN];

	sprintf(fn_dele_head, "%sheader.405", this->iniPath);
	sprintf(fn_dele, "%sascp2000.405", this->iniPath);
	sprintf(fn_obs, "%sObs.txt", this->iniPath);

	printf("dele_head %s\n", fn_dele_head);
	printf("dele %s\n", fn_dele);
	printf("obs %s\n", fn_obs);

	this->obs = new observ(fn_dele_head);
//	this->vpos = new pvdK();

//	this->coord = new List4(3);

	if(this->obs->init(fn_obs, fn_dele_head, fn_dele)) return 1;
	
	this->obs->set_obs_parpam(13, CENTER_SUN, SK_EKVATOR, "000"); 

	is_init_files = 1;

	return 0;
}

int pvdP::initAddFiles()
{
	this->baseCat = new CommetOrbCat;
	char *fn_bc;
	fn_bc = new char[256];
	sprintf(fn_bc, "%se5.elm", this->iniPath);
	this->baseCat->init(fn_bc);

	delete [] fn_bc;
	return 0;
}

int pvdP::detMode(char *fname)
{
	if(fname==NULL) return 1;
	char *fn, *ext;
	fn = new char[256];
	strcpy(fn, "");
	ext = new char[256];
	strcpy(ext, "");
	int len = strlen(fname);

	strncpy(fn, fname, len-4);
	strcpy(&fn[len-4], "\0");
	strcpy(ext, &fname[len-3]);
	strcpy(&fn[3], "\0");

	if(streqv(ext, "ast")) this->idatamode = 0;
	if(streqv(ext, "gmt")) this->idatamode = 1;
	if(streqv(ext, "mpc")) this->idatamode = 2;

	return 0;
}

int pvdP::initData(char *fn_pvd)
{
	if((!this->is_init_params)||(!this->is_init_files)) return 1;
	if(fn_pvd==NULL) return 1;

	this->detMode(fn_pvd);

	switch(this->idatamode)
	{
	case 2:
		mpcs *fini;
		fini = new mpcs;
		fini->init(fn_pvd);
		fini->GetCoordList(this->vpos->coords);
		
		fini->getmpc(0);
		if(fini->record->head->numType) sprintf(this->comvar->obj_name, "%8s", fini->record->head->SnumTemp);
		else sprintf(this->comvar->obj_name, "%5s ", fini->record->head->Snum);
		break;
	default:
		ast *fcs;
		fcs = new ast;

		if(fcs->initFile(fn_pvd)) return 1;
		fcs->GetCoordList(this->vpos->coords);
		if(this->vpos->initDT()) return 1;

//		sprintf(this->comvar->obj_name, "%6s", fcs->hat->name);

		break;
	}

	is_init_data = 1;

	return 0;
}

int pvdP::initDataF(char *fn_pvd)
{
	strcpy(this->vpos->fname, fn_pvd);

	this->set_suff(fn_pvd);
	this->logging(fn_pvd);

	this->res = new orbit(0);
	this->resCat = new CommetOrbCat();
	char *fn_rc;
	fn_rc = new char[256];
	sprintf(fn_rc, "%s.elm", suff);
	this->resCat->init(fn_rc);

	if(this->klog>0)
	{
		fprintf(this->slog->FLOG, "Initial observations\n\n");
		int num = this->vpos->coords->GetNum();
		int dim = this->vpos->coords->GetDim();
		double *x = new double[dim];
		char *t_str = new char[32];
		char *ra_str = new char[32];
		char *de_str = new char[32];
		for(int i=0; i<num; i++)
		{
			this->vpos->coords->Get(x, i);
			dat2YMD_str(x[0], t_str);
			rad2hms_str(x[1], ra_str);
			rad2gms_str(x[2], de_str);
			fprintf(this->slog->FLOG, "%s %s\t%s\t%s\n", this->comvar->obj_name, t_str, ra_str, de_str);
		}
		fprintf(this->slog->FLOG, "\n");

		delete [] t_str;
		delete [] ra_str;
		delete [] de_str;
		delete [] x;
	}

//	is_init_data = 1;
	delete [] fn_rc;

	return 0;
}

int pvdP::writePVD()
{
	int y, m, sn;
	char *ssn;
	ssn = new char[256];
	double d;

	char *str;
	str = new char[STRLEN];
//	this->dtr->x->print(str);
//	printf("\n%s\n", str);

	if(this->klog>0) fprintf(this->slog->FLOG, "\n\nВычисление ПВД\n\n");

	dat2YMD(this->vpos->t, &y, &m, &d);
	if(this->klog>0) fprintf(this->slog->FLOG, "t	%4d %02d %0f", y, m, d);

	rad2hms(this->vpos->r->val, &sn, &y, &m, &d);
	if(sn==0) strcpy(ssn, "-");
	else strcpy(ssn, "+");
	if(this->klog>0)fprintf(this->slog->FLOG, "\nRA	%s%02d %02d %9.6f", ssn, y ,m ,d);

	rad2hms(this->vpos->r->acc, &sn, &y, &m, &d);
	fprintf(this->slog->FLOG, "	+-%f", this->vpos->r->acc*206265.0/15.0);

	rad2gms(this->vpos->d->val, &sn, &y, &m, &d);
	if(sn==0) strcpy(ssn, "-");
	else strcpy(ssn, "+");
	fprintf(this->slog->FLOG, "\nDE	%s%02d %02d %9.6f", ssn, y ,m ,d);

	rad2hms(this->vpos->d->acc, &sn, &y, &m, &d);
	fprintf(this->slog->FLOG, "	+-%f", this->vpos->d->acc*206265.0);

	fprintf(this->slog->FLOG, "\nm	%15.10f\t\t%e\npsi	%15.10f\nmt	%15.10f\t\t%e\nc	%15.10f\n", this->vpos->m->val*206265.0, this->vpos->m->acc*206265.0, this->vpos->psi->val*180.0/PI, this->vpos->mt->val*206265.0, this->vpos->mt->acc*206265.0, this->vpos->c->val);

//	fclose(this->FC);
	delete [] str;
//	delete [] fn;
	delete [] ssn;
	return 0;
}

int pvdP::initParams(char *fnini)
{
	if(fnini!=NULL)
	{
		this->fini = 1;
		this->FINI = fopen(fnini, "r");
	}
	else this->fini = 0;
	
	if(!this->fini)
	{
		this->detCkey();
		this->comvar->inird = 0.1;
		strcpy(this->iniPath, "");
	}
	else this->initC();

	if(klog>2) initVarTest("model.ini"); //инициализация контрольных переменных

	is_init_params = 1;

	return 0;
}

int pvdP::initParams(int ckey, double inird, int tlen)
{
	this->fini = 0;
	this->comvar = new ComVar;

	this->comvar->Ckey = ckey;
	this->comvar->inird = inird;

	is_init_params = 1;

	return 0;
}

int pvdP::initVarTest(char *testVarFN)
{
	FILE *fT = fopen(testVarFN, "r");

	char *sVar = new char[1024];
	char *slo = new char[64];
	int pose;
	int posb = 0;
//первая строка - ПВД
	//pvd
	fgets(sVar, 1024, fT);
	//tJD
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[0] = str2JD(slo);
	//ra
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[1] = hms2rad(slo);
	//de
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[2] = gms2rad(slo);
	//m
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[3] = atof(slo)/206265.0;
	//mt
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[4] = atof(slo)/206265.0;
	//psi
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[5] = grad2rad(atof(slo));
	//c
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[6] = atof(slo);
//вторая строка - гелиоцентрические/барицентрические координаты 
	//r
	fgets(sVar, 1024, fT);
	//r0
	posb = 0;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[7] = atof(slo);
	//r1
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[8] = atof(slo);
	//r2
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[9] = atof(slo);
	//r
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[10] = atof(slo);
	//rt0
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[11] = atof(slo);
	//rt1
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[12] = atof(slo);
	//rt2
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[13] = atof(slo);
	//rtt0
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[14] = atof(slo);
	//rtt1
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[15] = atof(slo);
	//rtt2
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[16] = atof(slo);
//наблюдателецентрические положения
	//d
	fgets(sVar, 1024, fT);
	//d0
	posb = 0;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[17] = atof(slo);
	//d1
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[18] = atof(slo);
	//d2
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[19] = atof(slo);
	//d
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[20] = atof(slo);
	//dt0
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[21] = atof(slo);
	//dt1
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[22] = atof(slo);
	//dt2
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[23] = atof(slo);
	//dtt0
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[24] = atof(slo);
	//dtt1
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[25] = atof(slo);
	//dtt2
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[26] = atof(slo);

//положение Земли
	//g
	fgets(sVar, 1024, fT);
	//g0
	posb = 0;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[27] = atof(slo);
	//g1
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[28] = atof(slo);
	//g2
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[29] = atof(slo);
	//g
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[30] = atof(slo);
	//gt0
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[31] = atof(slo);
	//gt1
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[32] = atof(slo);
	//gt2
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[33] = atof(slo);
	//gtt0
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[34] = atof(slo);
	//gtt1
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[35] = atof(slo);
	//gtt2
	posb = pose+1;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[36] = atof(slo);

//dt
	//g
	fgets(sVar, 1024, fT);
	//g0
	posb = 0;
	slovo_stopsim(sVar, slo, "|", posb, &pose, 64);
	comvar->testVal[37] = atof(slo);



	delete [] sVar;
	delete [] slo;

	return 0;
}

int pvdP::logging(char *basename)
{
	this->slog = new logstuff;

	sprintf(this->slog->suff, "%s_pP%d.log", this->suff, this->comvar->Ckey);

	if(klog>0) this->slog->FLOG = fopen(this->slog->suff, "w");

	return 0;
}

void pvdP::set_suff(char *fn)
{
	this->suff = new char[256];
	int len = strlen(fn);
//	for(int i=0; i<128; i++) printf("%d %c\n", i, i);
	char *pos = strrchr(fn, 46);
	int len1 = strlen(pos);
	strcpy(this->suff, "");
	strncpy(this->suff, fn, len-len1);
	strcpy(&this->suff[len-len1], "\0");
}


int pvdP::compute()
{
	if(!this->is_init_data) return 1;

	this->vpos->findPVD();
//	this->vpos->writePVD();
	if(this->klog>0) this->writePVD();
	if(klog>2)
	{
		this->vpos->t = comvar->testVal[0];
//		this->vpos->r->val = comvar->testVal[1];
//		this->vpos->d->val = comvar->testVal[2];
		this->vpos->m->val = comvar->testVal[3];
		this->vpos->mt->val = comvar->testVal[4];
		this->vpos->psi->val = comvar->testVal[5];
		this->vpos->c->val = comvar->testVal[6];

		this->vpos->D->set(0, this->comvar->testVal[17]/this->comvar->testVal[20]);
		this->vpos->D->set(1, this->comvar->testVal[18]/this->comvar->testVal[20]);
		this->vpos->D->set(2, this->comvar->testVal[19]/this->comvar->testVal[20]);
/*
		double dt = sqrt(this->comvar->testVal[21]*this->comvar->testVal[21] + this->comvar->testVal[22]*this->comvar->testVal[22] + this->comvar->testVal[23]*this->comvar->testVal[23]);
		this->vpos->Dt->set(0, comvar->testVal[21]/dt);
		this->vpos->Dt->set(1, comvar->testVal[22]/dt);
		this->vpos->Dt->set(2, comvar->testVal[23]/dt);

		double dtt = sqrt(this->comvar->testVal[24]*this->comvar->testVal[24] + this->comvar->testVal[25]*this->comvar->testVal[25] + this->comvar->testVal[26]*this->comvar->testVal[26]);
		this->vpos->Dtt->set(0, comvar->testVal[24]/dtt);
		this->vpos->Dtt->set(1, comvar->testVal[24]/dtt);
		this->vpos->Dtt->set(2, comvar->testVal[24]/dtt);
/*
		this->vpos->T->set(0, (-sin(this->vpos->psi->val)*sin(this->vpos->r->val) - cos(this->vpos->psi->val)*sin(this->vpos->d->val)*cos(this->vpos->r->val)));
		this->vpos->T->set(1, (sin(this->vpos->psi->val)*cos(this->vpos->r->val) - cos(this->vpos->psi->val)*sin(this->vpos->d->val)*sin(this->vpos->r->val)));
		this->vpos->T->set(2, (cos(this->vpos->psi->val)*cos(this->vpos->d->val)));

		this->vpos->M->set(0, (cos(this->vpos->psi->val)*sin(this->vpos->r->val) - sin(this->vpos->psi->val)*sin(this->vpos->d->val)*cos(this->vpos->r->val)));
		this->vpos->M->set(1, (-cos(this->vpos->psi->val)*cos(this->vpos->r->val) - sin(this->vpos->psi->val)*sin(this->vpos->d->val)*sin(this->vpos->r->val)));
		this->vpos->M->set(2, (sin(this->vpos->psi->val)*cos(this->vpos->d->val)));
		*/
		this->writePVD();
	}

	this->initVarConst();
//	this->vpos->findPVD();
	this->initVar();
	this->provTDM();

//	this->findSqr();
	
	if(this->appFerst1())
	{
//		this->aclose();
		return 1;
	}

//	this->appSecond();

	if(this->computeOrb2()) return 2;

//	if(this->ParallaxCorrection()) return 1;

	if(this->klog>0) fprintf(this->slog->FLOG, "\nПоправок за параллакс - %d\n\n", this->comvar->numOfPR);
	

	if(this->idatamode==2)this->proofSM();

	
	
	return 0;
}


int pvdP::AccDelive()
{
/*	this->comvar->Tsg = Smul(this->comvar->T, this->comvar->g);

	int n;
	double r;
	n = this->comvar->last;
	r = sqrt(this->comvar->chv[n].r[0]*this->comvar->chv[n].r[0] + this->comvar->chv[n].r[1]*this->comvar->chv[n].r[1] + this->comvar->chv[n].r[2]*this->comvar->chv[n].r[2]);
	this->vpos->mt->val = (-(ka*ka*this->comvar->Tsg)/(r*r*r) - this->comvar->gttsT - 2.0*this->vpos->m->val*this->comvar->chv[n].dt*this->comvar->chv[n].dt)/this->comvar->chv[n].d;
*/
	return 0;
}



int pvdP::appFerst1()
{
	char *pv;
	pv = new char[STRLEN];

	double r0, r, ret;
	int iter = 0;


	ret=1;
	switch(this->comvar->Ckey)
	{
	case 0:
		printf("\nИтерации по R, вариант по m\n\n");
		if(this->klog>0) fprintf(this->slog->FLOG, "\nИтерации по R, вариант по m\n\n");
		
		r = this->comvar->inird;
		if(klog>4) r = comvar->testVal[7];
		this->comvar->chv[this->comvar->Ckey].rnorm = 0.0;
		r0 = r;

		printf("inird\t%f\n\n", r0);
		if(this->klog>0) fprintf(this->slog->FLOG, "Первое приближение:\t%f\n\n", r0);

		while((r<DEPTHSCAN)&&ret)
		{
			ret = iterR(r0, &iter);
			printf("r= %f	r0= %f	%d iter\n", r, this->comvar->chv[this->comvar->Ckey].rnorm, iter);
			if(this->klog>1) fprintf(this->slog->FLOG, "r= %f	r0= %f	%d iter\n", r, this->comvar->chv[this->comvar->Ckey].rnorm, iter);
			r0 = r;
			r +=0.1;
			iter = 0;
		}
		printf("\nВ итоге: r0= %f за %d итераций\n", this->comvar->chv[this->comvar->Ckey].rnorm, this->comvar->chv[this->comvar->Ckey].iter);
		if(this->klog>0) fprintf(this->slog->FLOG, "\nВ итоге: r0= %f за %d итераций\n", this->comvar->chv[this->comvar->Ckey].rnorm, this->comvar->chv[this->comvar->Ckey].iter);
		break;
	case 1:
		printf("\nИтерации по R, вариант по c\n\n");
		if(this->klog>0) fprintf(this->slog->FLOG, "\nИтерации по R, вариант по c\n\n");

		r = this->comvar->inird;
		if(klog>4) r = comvar->testVal[10];
		this->comvar->chv[this->comvar->Ckey].rnorm = 0.0;
		r0 = r;

		printf("inird\t%f\n\n", r0);
		if(this->klog>0) fprintf(this->slog->FLOG, "Первое приближение:\t%f\n\n", r0);

		while((r<DEPTHSCAN)&&ret)
		{
			ret = iterRc(r0, &iter);
			printf("r= %f	r0= %f	%d iter\n", r, this->comvar->chv[this->comvar->Ckey].rnorm, iter);
			if(this->klog>1) fprintf(this->slog->FLOG, "r= %f	r0= %f	%d iter\n", r, this->comvar->chv[this->comvar->Ckey].rnorm, iter);
			r0 = r;
			r +=0.1;
			iter = 0;
		}
		printf("\nВ итоге: r0= %f за %d итераций\n", this->comvar->chv[this->comvar->Ckey].rnorm, this->comvar->chv[this->comvar->Ckey].iter);
		if(this->klog>0) fprintf(this->slog->FLOG, "\nВ итоге: r0= %f за %d итераций\n", this->comvar->chv[this->comvar->Ckey].rnorm, this->comvar->chv[this->comvar->Ckey].iter);
		break;
	case 2:
		printf("\nИтерации по D, вариант по m\n\n");
		if(this->klog>0) fprintf(this->slog->FLOG, "\nИтерации по D, вариант по m\n\n");

		r = this->comvar->inird;
		if(klog>4) r = comvar->testVal[8];
		this->comvar->chv[this->comvar->Ckey].d = 0.0;
		r0 = r;

		printf("inird\t%f\n\n", r0);
		if(this->klog>0) fprintf(this->slog->FLOG, "Первое приближение:\t%f\n\n", r0);

		while((r<DEPTHSCAN)&&ret)
		{
			ret = iterD(r0, &iter);
			printf("r= %f	d0= %f	%d iter\n", r, this->comvar->chv[this->comvar->Ckey].d, iter);
			if(this->klog>1) fprintf(this->slog->FLOG, "r= %f	r0= %f	%d iter\n", r, this->comvar->chv[this->comvar->Ckey].rnorm, iter);
			r0 = r;
			r +=0.1;
			iter = 0;
		}
		printf("\nВ итоге: r0= %f за %d итераций\n", this->comvar->chv[this->comvar->Ckey].d, this->comvar->chv[this->comvar->Ckey].iter);
		if(this->klog>0) fprintf(this->slog->FLOG, "\nВ итоге: r0= %f за %d итераций\n", this->comvar->chv[this->comvar->Ckey].rnorm, this->comvar->chv[this->comvar->Ckey].iter);
		break;
	case 3:
		printf("\nИтерации по D, вариант по c\n\n");
		if(this->klog>0) fprintf(this->slog->FLOG, "\nИтерации по D, вариант по c\n\n");
		r = this->comvar->inird;
		if(klog>4) r = comvar->testVal[8];
		this->comvar->chv[this->comvar->Ckey].d = 0.0;
		r0 = r;

		printf("inird\t%f\n\n", r0);
		if(this->klog>0) fprintf(this->slog->FLOG, "Первое приближение:\t%f\n\n", r0);

		while((r<DEPTHSCAN)&&ret)
		{
			ret = iterDc(r0, &iter);
			printf("r= %f	d0= %f	%d iter\n", r, this->comvar->chv[this->comvar->Ckey].d, iter);
			if(this->klog>1) fprintf(this->slog->FLOG, "r= %f	r0= %f	%d iter\n", r, this->comvar->chv[this->comvar->Ckey].d, iter);
			r0 = r;
			r +=0.1;
			iter = 0;
		}
		printf("\nВ итоге: r0= %f за %d итераций\n", this->comvar->chv[this->comvar->Ckey].d, this->comvar->chv[this->comvar->Ckey].iter);
		if(this->klog>0) fprintf(this->slog->FLOG, "\nВ итоге: r0= %f за %d итераций\n", this->comvar->chv[this->comvar->Ckey].d, this->comvar->chv[this->comvar->Ckey].iter);
		break;
	case 4:
		printf("\nИтерации по D, специальный вариант\n\n");
		if(this->klog>0) fprintf(this->slog->FLOG, "\nИтерации по D, специальный вариант\n\n");

		r = this->comvar->inird;
		
		this->comvar->chv[this->comvar->Ckey].d = 0.0;
		
		ret = iterDcD(r, &iter);
		
		printf("\nВ итоге: r0= %f за %d итераций\n", this->comvar->chv[this->comvar->Ckey].d, this->comvar->chv[this->comvar->Ckey].iter);
		if(this->klog>0) fprintf(this->slog->FLOG, "\nВ итоге: r0= %f за %d итераций\n", this->comvar->chv[this->comvar->Ckey].rnorm, this->comvar->chv[this->comvar->Ckey].iter);
		break;
	}

	if(ret)
	{
		printf("\n\n!!!!!!!!!Process iteracij ne shoditsya!!!!!!!!!\n\n");
		return 1;
	}
//
	printf("r:\t%f\t%f\t%f\n", this->comvar->chv[this->comvar->Ckey].r[0], this->comvar->chv[this->comvar->Ckey].r[1], this->comvar->chv[this->comvar->Ckey].r[2]);
	printf("rt:\t%f\t%f\t%f\n", this->comvar->chv[this->comvar->Ckey].rt[0], this->comvar->chv[this->comvar->Ckey].rt[1], this->comvar->chv[this->comvar->Ckey].rt[2]);
	printf("d:\t%f\n", this->comvar->chv[this->comvar->Ckey].d);
	printf("dt:\t%f\n", this->comvar->chv[this->comvar->Ckey].dt);

	if(this->klog>1)
	{
		fprintf(this->slog->FLOG, "r:\t%f\t%f\t%f\n", this->comvar->chv[this->comvar->Ckey].r[0], this->comvar->chv[this->comvar->Ckey].r[1], this->comvar->chv[this->comvar->Ckey].r[2]);
		fprintf(this->slog->FLOG, "rt:\t%10.7e\t%10.7e\t%10.7e\n", this->comvar->chv[this->comvar->Ckey].rt[0], this->comvar->chv[this->comvar->Ckey].rt[1], this->comvar->chv[this->comvar->Ckey].rt[2]);
		fprintf(this->slog->FLOG, "d:\t%f\n", this->comvar->chv[this->comvar->Ckey].d);
		fprintf(this->slog->FLOG, "dt:\t%10.7e\n", this->comvar->chv[this->comvar->Ckey].dt);
	}

	myvector *Recl, *Rtecl;
	Recl = new myvector(3);
	Rtecl = new myvector(3);

	Recl->set(0, this->comvar->chv[this->comvar->Ckey].r[0]);
	Recl->set(1, this->comvar->chv[this->comvar->Ckey].r[1]);
	Recl->set(2, this->comvar->chv[this->comvar->Ckey].r[2]);

	Rtecl->set(0, this->comvar->chv[this->comvar->Ckey].rt[0]);
	Rtecl->set(1, this->comvar->chv[this->comvar->Ckey].rt[1]);
	Rtecl->set(2, this->comvar->chv[this->comvar->Ckey].rt[2]);

	Recl->RotX(EKV);
	Rtecl->RotX(EKV);

	if(this->klog>1)
	{
		Recl->print(pv);
		fprintf(this->slog->FLOG, "\n\nRecl:\t%s\n", pv);
		Rtecl->print(pv);
		fprintf(this->slog->FLOG, "Rtecl:\t%s\n\n", pv);
	}

	this->proofPVD();

//	delete [] pv;


	return 0;
}

int pvdP::proofSM()
{
	
	this->initData(this->vpos->fname);
	record4L *rec = new record4L(this->vpos->coords->GetDim());

	int num = this->vpos->coords->GetNum();

	double x, y, z, t, dd;
	double oc;
	double P1, P2, P3, aV, dV;
	int i;
	double day;
	int year, mouth, sgn;
	char *ssgn;
	ssgn = new char[16];
	double aV0, dV0;

	double ocsumm = 0.0;


	printf("\n\nO-C\n");
	if(this->klog>0) fprintf(this->slog->FLOG, "\n\nO-C\n");


	mpc *ttt;
	ttt = new mpc();
	note *temp;
	temp = new note;

	double r, x_obs, y_obs, z_obs, a, d;

	for(i=0; i<vpos->nt; i++)
	{
//		this->vpos->obs->getmpc(ttt, i);
		this->vpos->coords->Get(rec, i);
		if(streqv(rec->centre_name, "")) continue;
		if(this->obs->obs->getobsynumO(rec->centre_name))
		{
			printf("\n%6s ", this->comvar->obj_name);
			if(this->klog>0) fprintf(this->slog->FLOG,"\n%6s ", this->comvar->obj_name);

			dat2YMD(rec->x[0], &year, &mouth, &day);
			printf("%4d %02d %09.6f\tobervatory not found", year, mouth, day);
			if(this->klog>0) fprintf(this->slog->FLOG,"%4d %02d %09.6f\tobervatory not found", year, mouth, day);
			continue;
		}
		t = rec->x[0];
//		this->obs->obs->det_state(t);
		this->res->detRecEkv(&x, &y, &z, t);
//		this->obs->set_obs_parpam();
		this->obs->det_observ(t);
		this->obs->obs->det_state(rec->x[0]);

		P1 = x - this->obs->ox + this->obs->obs->dcx;
		P2 = y - this->obs->oy + this->obs->obs->dcy;
		P3 = z - this->obs->oz + this->obs->obs->dcz;

		rdsys(&aV, &dV, P1, P2, P3);
/*
		x_obs = this->obs->obs->dcx + x;
		y_obs = this->obs->obs->dcy + y;
		z_obs = this->obs->obs->dcz + z;

		rdsys(&aV, &dV, x_obs, y_obs, z_obs);
*/

//		aV0 = this->vpos->dtr->f->get(i, 0);
//		dV0 = this->vpos->dtd->f->get(i, 0);
		aV0 = rec->x[1];
		dV0 = rec->x[2];

		this->printOC(this->comvar->obj_name, aV, dV, aV0, dV0, t, &oc);
		ocsumm +=oc;
	}

	this->comvar->chv[this->comvar->Ckey].oc = ocsumm;

	if(this->klog>1) fprintf(this->slog->FLOG, "\n\nOCsumm = %f\n", ocsumm);


	delete [] ssgn;
	delete temp;
	delete ttt;

	return 0;
}

int pvdP::appSecond()
{
	return 0;
}

int pvdP::ParallaxCorrection()
{
	this->initData(this->vpos->fname);
	record4L *rec = new record4L(this->vpos->coords->GetDim());

	int num = this->vpos->coords->GetNum();
	double r, x_obs, y_obs, z_obs, a, d;
	r = this->comvar->chv[this->comvar->Ckey].d;

	if(this->klog>1) fprintf(this->slog->FLOG, "\n\nParallax Correction\n\n");

	char *t_str = new char[32];
	char *ra_str = new char[32];
	char *de_str = new char[32];
	char *obs_str = new char[32];

	for(int i=0; i<num; i++)
	{
		this->vpos->coords->Get(rec, i);
		if(streqv(rec->centre_name, "")) continue;

		if(this->obs->obs->getobsynumO(rec->centre_name)) continue;
		this->obs->obs->det_state(rec->x[0]);
		
		x_obs = this->obs->obs->dcx + r*cos(rec->x[1])*cos(rec->x[2]);
		y_obs = this->obs->obs->dcy + r*sin(rec->x[1])*cos(rec->x[2]);
		z_obs = this->obs->obs->dcz + r*sin(rec->x[2]);

		rdsys(&a, &d, x_obs, y_obs, z_obs);

		rec->x[1] = a;
		rec->x[2] = d;
//		strcpy(rec->centre_name, "GEC");

		this->vpos->coords->Update(rec, i);

		dat2YMD_str(rec->x[0], t_str);
		rad2hms_str(rec->x[1], ra_str);
		rad2gms_str(rec->x[2], de_str);
		if(this->klog>1) fprintf(this->slog->FLOG, "%s\t%s\t%s\n", t_str, ra_str, de_str);
	}

	if(this->klog>1) fprintf(this->slog->FLOG, "\n");

	this->comvar->numOfPR++;

	return 0;
}

/*
int pvdP::findSqr()
{
	int i;
//	double x;
	double f, fmo, df, dfmo;
	myvector *sqrs;
	chvar ur;
	char *pv;

	sqrs = new myvector();
	pv = new char[1000];

	ur.d = 0.0;
	this->chain1(&ur);
	f =  ur.f[0];
	df = 0.0;

	printf("\nf		d		dt		r		rt\n");
	for(i=1; ur.d<DEPTHSCAN; i++)
	{
		ur.d = i*1.0;
		if(this->chain1(&ur)) continue;

		printf("%f	%f	%f	%f	%f\n", ur.f[0], ur.d, ur.dt, sqrt(ur.r[0]*ur.r[0] + ur.r[1]*ur.r[1] + ur.r[2]*ur.r[2]), sqrt(ur.rt[0]*ur.rt[0] + ur.rt[1]*ur.rt[1] + ur.rt[2]*ur.rt[2]));

		fmo = f;
		f = ur.f[0];
		dfmo = df;
		df = f - fmo;

//		if((df*dfmo<0.0)||(f*fmo<0.0))
//		{
			sqrs->push(ur.d);
			sqrs->print(pv);
			printf("\nsqrs: %s\n", pv);
//		}

	}

	sqrs->print(pv);
	printf("\nsqrs: %s\n", pv);




	return 0;
}
*/
int pvdP::iterR(double r, int *niter)
{
	if(*niter>MAX_ITER||(r<0.0)) return 1;
	++*niter;
	int sekey, ret;
	double d, d1, d2, dt, rr;
	double A, B, C;

	ret = 0;

	A = 1.0;
	B = 2.0*this->comvar->gsD;
	C = this->comvar->g2 - r*r;
	sekey = sqEqv(A, B, C, &d1, &d2);

	d = d1;
//	d = -d2;
	if(sekey==2)
	{
		this->comvar->chv[this->comvar->Ckey].rnorm = 0.0;
		return 1;
	}

	this->comvar->R->set(0, (this->comvar->g->get(0) + d*this->vpos->D->get(0))/r);
	this->comvar->R->set(1, (this->comvar->g->get(1) + d*this->vpos->D->get(1))/r);
	this->comvar->R->set(2, (this->comvar->g->get(2) + d*this->vpos->D->get(2))/r);

	this->comvar->RsM = Smul(this->comvar->R, this->comvar->M);

//test

	if(this->provRGD()&&klog>1) fprintf(slog->FLOG, "provRGD - bad\n");;
	

	this->comvar->RsT = Smul(this->comvar->R, this->comvar->T);
	

//(7)
	dt = (-(ka*ka*this->comvar->gsT)/(r*r*r))/(2.0*this->vpos->m->val) - this->comvar->gttsT/(2.0*this->vpos->m->val) - (this->vpos->mt->val/(2.0*this->vpos->m->val))*d;
//(6)
//	dt = (-(ka*ka*this->comvar->RsT)/(r*r) - this->comvar->gttsT - this->vpos->mt->val*d)/(2.0*this->vpos->m->val);

	double S;
//	S = 2.0*ka*ka/r;
//(5)
	S  = this->vpos->m->val*this->vpos->m->val*d*d + 2.0*this->vpos->m->val*d*this->comvar->gtsT + dt*dt + 2.0*dt*this->comvar->gtsD + this->comvar->gt2;
	rr = 2.0*ka*ka/S;

	if(klog>1) fprintf(this->slog->FLOG, "%f %f\n", r, rr);


	if(fabs(r-rr)>EPS) ret = iterR(rr, niter);
	else
	{
		this->iterEnd(rr, d, dt, this->comvar->R, *niter, this->comvar->Ckey);
		ret = proofR();
	}
	
//	*r = rr;
	return ret;
}

int pvdP::iterRc(double r, int *niter)
{
	if((*niter>MAX_ITER)||(r<0.0)) return 1;
	++*niter;
	int sekey, ret;
	double d, d1, d2, dt, rr;
	double A, B, C;
	double S;
	double dt1, dt2, dd1, dd2;

	ret = 0;

	S = -this->comvar->signGM*this->vpos->m->val*this->vpos->m->val*sqrt(this->vpos->c->val*this->vpos->c->val - 1.0);
	d = (ka*ka*this->comvar->gsM/(r*r*r) + this->comvar->gttsM)/S;

	if(d<0.0) d = fabs(d);

	if(klog>5) d = comvar->testVal[20];

	this->comvar->R->set(0, (this->comvar->g->get(0) + d*this->vpos->D->get(0))/r);
	this->comvar->R->set(1, (this->comvar->g->get(1) + d*this->vpos->D->get(1))/r);
	this->comvar->R->set(2, (this->comvar->g->get(2) + d*this->vpos->D->get(2))/r);

	this->comvar->RsM = Smul(this->comvar->R, this->comvar->M);

	this->provRGD();

	this->comvar->RsT = Smul(this->comvar->R, this->comvar->T);
	
//	S = this->vpos->m->val*this->vpos->m->val*d*d + 2.0*this->vpos->m->val*d*this->comvar->gtsT + dt*dt + 2.0*dt*this->comvar->gtsD + this->comvar->gt2;
//	S = this->comvar->gttsM - sign(this->comvar->RsT)*this->vpos->m->val*this->vpos->m->val*d*sqrt(this->vpos->c->val*this->vpos->c->val - 1.0);
//	rr = pow(-ka*ka*this->comvar->RsM/S, 1.0/2.0);
//	S = this->comvar->gttsM + this->comvar->signGM*this->vpos->m->val*this->vpos->m->val*d*sqrt(this->vpos->c->val*this->vpos->c->val - 1.0);
//	rr = pow(-ka*ka*this->comvar->gsM/S, 1.0/3.0);

//1'
	rr = sqrt(this->comvar->g2 + d*d + 2.0*d*this->comvar->gsD);

	if(klog>1) fprintf(this->slog->FLOG, "%f %f\n", r, rr);
	if(klog>5) 
	{
		rr = comvar->testVal[10];
		fprintf(this->slog->FLOG, "%f %f\n", r, rr);
	}


	if(fabs(r-rr)>EPS) ret = iterRc(rr, niter);
	else
	{
//(2.8)
//		dt0 = (-(ka*ka*this->comvar->gsT)/(rr*rr*rr) - this->comvar->gttsT - this->vpos->mt->val*d)/(2.0*this->vpos->m->val);


		A = 1.0;
		B = 2.0*this->comvar->gtsD;
		C = this->vpos->m->val*this->vpos->m->val*d*d + 2.0*this->vpos->m->val*d*this->comvar->gtsT + this->comvar->gt2 - 2.0*ka*ka/rr;
		sekey = sqEqv(A, B, C, &dt1, &dt2);
		if(sekey==2)
		{
			this->comvar->chv[this->comvar->Ckey].dt = 0.0;
			return 1;
		}
		else if(sekey==1)
		{
			dt = dt1;
//			dd = (-(ka*ka*this->comvar->gsT)/(r*r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt;
		}
		else
		{ 	
//(6)
//			dd = (-(ka*ka*this->comvar->RsT)/(r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt;
//(7)
			dd1 = (-(ka*ka*this->comvar->gsT)/(r*r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt1;
//(6)
//			dd = (-(ka*ka*this->comvar->RsT)/(r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt;
//(7)
			dd2 = (-(ka*ka*this->comvar->gsT)/(r*r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt2;
			if(dd1<0.0) dt = dt2;
			else if(dd2<0.0) dt = dt1;
			else if(fabs(d-dd1)<fabs(d-dd2))
			{
				dt = dt1;
//				dd = dd1;
			}
			else
			{
				dt = dt2;
//				dd = dd2;
			}
//			fprintf(this->slog->FLOG, "dt1 dt2:\t%f %f %f\n", dt1, dt2, dt);
		}

//		if(klog>5) dt = comvar->testVal[37];

		this->iterEnd(rr, d, dt, this->comvar->R, *niter, this->comvar->Ckey);
		ret = proofR();
	}
	
	return ret;
}

int pvdP::iterDcD(double r, int *niter)
{
	if((*niter>MAX_ITER)||(r<0.0)) return 1;
	++*niter;
	int sekey, ret;
	double d, d1, d2, dt, rr;
	double A, B, C;
	double S;
	double dt1, dt2, dd1, dd2;

	double kappa, em;
	ret = 0;

	kappa = -this->comvar->GsD + sqrt(r*r/this->comvar->g2 - (1.0 - this->comvar->GsD*this->comvar->GsD));
	em = sqrt(1.0 + kappa*kappa + 2.0*kappa*this->comvar->GsD);

	this->comvar->R->set(0, (this->comvar->G->get(0) + kappa*this->vpos->D->get(0))/em);
	this->comvar->R->set(1, (this->comvar->G->get(1) + kappa*this->vpos->D->get(1))/em);
	this->comvar->R->set(2, (this->comvar->G->get(2) + kappa*this->vpos->D->get(2))/em);

	this->comvar->RsM = Smul(this->comvar->R, this->comvar->M);

	rr = -(ka*ka*this->comvar->RsM)/(this->comvar->gttsM);

	rr = sqrt(rr);

	this->provRGD();

	if(fabs(r-rr)>EPS) ret = iterDcD(rr, niter);
	else
	{
//(2.8)
//		dt0 = (-(ka*ka*this->comvar->gsT)/(rr*rr*rr) - this->comvar->gttsT - this->vpos->mt->val*d)/(2.0*this->vpos->m->val);

		A = 1.0;
		B = 2.0*this->comvar->gsD;
		C = this->comvar->g2 - r*r;
		sekey = sqEqv(A, B, C, &d1, &d2);

		d = d1;
//	d = -d2;
		if(sekey==2)
		{
			this->comvar->chv[this->comvar->Ckey].rnorm = 0.0;
			return 1;
		}

		A = 1.0;
		B = 2.0*this->comvar->gtsD;
		C = this->vpos->m->val*this->vpos->m->val*d*d + 2.0*this->vpos->m->val*d*this->comvar->gtsT + this->comvar->gt2 - 2.0*ka*ka/rr;
		sekey = sqEqv(A, B, C, &dt1, &dt2);
		if(sekey==2)
		{
			this->comvar->chv[this->comvar->Ckey].dt = 0.0;
			return 1;
		}
		else if(sekey==1)
		{
			dt = dt1;
//			dd = (-(ka*ka*this->comvar->gsT)/(r*r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt;
		}
		else
		{ 	
//(6)
//			dd = (-(ka*ka*this->comvar->RsT)/(r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt;
//(7)
			dd1 = (-(ka*ka*this->comvar->gsT)/(r*r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt1;
//(6)
//			dd = (-(ka*ka*this->comvar->RsT)/(r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt;
//(7)
			dd2 = (-(ka*ka*this->comvar->gsT)/(r*r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt2;
			if(fabs(d-dd1)<fabs(d-dd2))
			{
				dt = dt1;
//				dd = dd1;
			}
			else
			{
				dt = dt2;
//				dd = dd2;
			}
		}

		this->iterEnd(rr, d, dt, this->comvar->R, *niter, this->comvar->Ckey);
		ret = proofR();


	}
	
	return ret;
}


int pvdP::iterD(double d, int *niter)
{
	if((*niter>MAX_ITER)||(d<0.0)) return 1;
	++*niter;
	int sekey, ret;
	double g2, dt, dt1, dt2, r, gt2, dd1, dd2, dd;
	double A, B, C;

	ret = 0;

	r = sqrt(this->comvar->g2 + d*d + 2.0*d*this->comvar->gsD);

	this->comvar->R->set(0, (this->comvar->g->get(0) + d*this->vpos->D->get(0))/r);
	this->comvar->R->set(1, (this->comvar->g->get(1) + d*this->vpos->D->get(1))/r);
	this->comvar->R->set(2, (this->comvar->g->get(2) + d*this->vpos->D->get(2))/r);
	this->provRGD();

	this->comvar->RsT = Smul(this->comvar->R, this->comvar->T);

	A = 1.0;
	B = 2.0*this->comvar->gtsD;
	C = this->vpos->m->val*this->vpos->m->val*d*d + 2.0*this->vpos->m->val*d*this->comvar->gtsT + this->comvar->gt2 - 2.0*ka*ka/r;
	sekey = sqEqv(A, B, C, &dt1, &dt2);
//
	if(sekey==2)
	{
		this->comvar->chv[this->comvar->Ckey].dt = 0.0;
		return 1;
	}
	else if(sekey==1)
	{
		dt = dt1;
		dd = (-(ka*ka*this->comvar->gsT)/(r*r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt;
	}
	else
	{ 	
//(6)
//		dd1 = (-(ka*ka*this->comvar->RsT)/(r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt1;
//(7)
		dd1 = -(((ka*ka*this->comvar->gsT)/(this->vpos->mt->val*r))/(r*r)) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt1;
//(6)
//		dd2 = (-(ka*ka*this->comvar->RsT)/(r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt2;
//(7)
		dd2 = (-(ka*ka*this->comvar->gsT)/(r*r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt2;
		if(fabs(d-dd1)<fabs(d-dd2))
		{
			dt = dt1;
			dd = dd1;
		}
		else
		{
			dt = dt2;
			dd = dd2;
		}
	}

	if(klog>1) fprintf(this->slog->FLOG, "%f %f\n", d, dd);

	if(fabs(d-dd)>EPS) ret = iterD(dd, niter);
	else
	{
		this->iterEnd(r, dd, dt, this->comvar->R, *niter, this->comvar->Ckey);
		ret = proofR();
	}
	return ret;
}

int pvdP::iterDc(double d, int *niter)
{
	if((*niter>MAX_ITER)||(d<0.0)) return 1;
	++*niter;
	int sekey, ret;
	double dt, r, dd;
	double A, B, C;
	double dt1, dt2, dd1, dd2;

	ret = 0;

	r = sqrt(this->comvar->g2 + d*d + 2.0*d*this->comvar->gsD);

	this->comvar->R->set(0, (this->comvar->g->get(0) + d*this->vpos->D->get(0))/r);
	this->comvar->R->set(1, (this->comvar->g->get(1) + d*this->vpos->D->get(1))/r);
	this->comvar->R->set(2, (this->comvar->g->get(2) + d*this->vpos->D->get(2))/r);

	this->comvar->RsM = Smul(this->comvar->R, this->comvar->M);

	this->provRGD();

	this->comvar->RsT = Smul(this->comvar->R, this->comvar->T);

	double S;
//	S = this->vpos->m->val*this->vpos->m->val*d*d + 2.0*this->vpos->m->val*d*this->comvar->gtsT + dt*dt + 2.0*dt*this->comvar->gtsD + this->comvar->gt2;
//	S = this->comvar->gttsM - sign(this->comvar->RsT)*this->vpos->m->val*this->vpos->m->val*d*sqrt(this->vpos->c->val*this->vpos->c->val - 1.0);
//	rr = pow(-ka*ka*this->comvar->RsM/S, 1.0/2.0);


	S = -this->comvar->signGM*this->vpos->m->val*this->vpos->m->val*sqrt(this->vpos->c->val*this->vpos->c->val - 1.0);
	dd = (ka*ka*this->comvar->gsM/(r*r*r) + this->comvar->gttsM)/S;

	if(klog>1) fprintf(this->slog->FLOG, "%f %f\n", d, dd);

	if(fabs(d-dd)>EPS) ret = iterDc(dd, niter);
	else
	{
//		dt = (-(ka*ka*this->comvar->gsT)/(r*r*r) - this->comvar->gttsT - this->vpos->mt->val*d)/(2.0*this->vpos->m->val);

		A = 1.0;
		B = 2.0*this->comvar->gtsD;
		C = this->vpos->m->val*this->vpos->m->val*d*d + 2.0*this->vpos->m->val*d*this->comvar->gtsT + this->comvar->gt2 - 2.0*ka*ka/r;
		sekey = sqEqv(A, B, C, &dt1, &dt2);
		if(sekey==2)
		{
			this->comvar->chv[this->comvar->Ckey].dt = 0.0;
			return 1;
		}
		else if(sekey==1)
		{ 
			dt = dt1;
//			dd = (-(ka*ka*this->comvar->gsT)/(r*r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt;
		}
		else
		{ 	
//(6)
//			dd = (-(ka*ka*this->comvar->RsT)/(r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt;
//(7)
			dd1 = (-(ka*ka*this->comvar->gsT)/(r*r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt1;
//(6)
//			dd = (-(ka*ka*this->comvar->RsT)/(r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt;
//(7)
			dd2 = (-(ka*ka*this->comvar->gsT)/(r*r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt2;
			if(fabs(d-dd1)<fabs(d-dd2))
			{
				dt = dt1;
//				dd = dd1;
			}
			else
			{
				dt = dt2;
//				dd = dd2;
			}
		}

		this->iterEnd(r, dd, dt, this->comvar->R, *niter, this->comvar->Ckey);

		ret = proofR();
//
		
	}
	
//	*r = rr;
	return ret;
}

int pvdP::computeOrb()
{
	char *str = new char[64];
	myvector *r, *rt;
	r = new myvector(3);
	rt = new myvector(3);
	int n;
	n = this->comvar->Ckey;
//	n = 0;
	r->set(0, this->comvar->chv[n].r[0]);
	r->set(1, this->comvar->chv[n].r[1]);
	r->set(2, this->comvar->chv[n].r[2]);
	rt->set(0, this->comvar->chv[n].rt[0]);
	rt->set(1, this->comvar->chv[n].rt[1]);
	rt->set(2, this->comvar->chv[n].rt[2]);

//
	char *pv;
	pv = new char[STRLEN];
	r->print(pv);
	printf("\n\nr ecl:	%s	%f", pv, r->norm());
	rt->print(pv);
	printf("\nrt ecl:	%s	%f", pv, rt->norm());
//

	r->RotX(-EKV);
	rt->RotX(-EKV);

//
	r->print(pv);
	printf("\n\nr ekv:	%s	%f", pv, r->norm());
	rt->print(pv);
	printf("\nrt ekv:	%s	%f", pv, rt->norm());
//

//	this->res = new orbit(this->klog);
//	this->res->FindOrb(r, rt, this->vpos->t, 0);
//
	double P1, P2, P3;
	double p, inc, Node, TT;

	P1 = r->get(1)*rt->get(2) - r->get(2)*rt->get(1);
	P2 = r->get(0)*rt->get(2) - r->get(2)*rt->get(0);
	P3 = r->get(0)*rt->get(1) - r->get(1)*rt->get(0);

	piNsys(&p, &inc, &Node, P1, P2, P3);

	double ecc, q, B, sg0, r0rt0, v0, r0;

	q = 0.5*p;
	ecc = 1.0;

	r0rt0 = Smul(r, rt);

	sg0 = r0rt0/(ka*sqrt(2.0*q));

	v0 = 2.0*atan2(r0rt0, ka*sqrt(2.0*q));

	B = sqrt(2.0)*(sg0 + sg0*sg0*sg0/3.0)/ka;

	TT = this->vpos->t - pow(q, 1.5)*B;

	r0 = sqrt(r->get(0)*r->get(0) + r->get(1)*r->get(1) + r->get(2)*r->get(2));

	P1 = r->get(2)/sin(inc);
	P2 = r->get(0)*cos(Node) + r->get(1)*sin(Node);

	double u0;

	u0 = atan2(P1, P2);

	double w;

	w = u0 - v0;
//

//
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	double M0, eJD, ss, dT, sign;
	M0 = 0.0;
	sign = 1.0;
//	eJD = (double)(int)this->appcnt->t;
	TT*=2.0;
	TT+=chet(TT);
	eJD = ((double)(int)TT)/2.0;
	TT/=2.0;
	dT = eJD - TT;
//	dT = TT - eJD;
//	eJD = eJD - 0.5;
	B = pow(q, -1.5)*dT;
	if(B<0.0) 
	{
		B = fabs(B);
		sign = -1.0;
	}
	pKepler(&ss, B);
	ss = sign*ss;
	M0 = 2.0*atan(ss);
	if(M0<0.0) M0+=2.0*PI;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//
	if(w<0.0) w+=2.0*PI;
//	if(Node<0.0) Node+=2.0*PI;
//	if(inc<0.0) inc+=2.0*PI;

	double ei, eN, eP;
/*
	Ekv2Ecl(&ei, &eN, &eP, inc, Node, w);

	w = eP;
	Node = eN;
	inc = ei;
*/

		dat2YMD_str(TT, str);
		printf("\n\nOrb Elsements\n\nT     \t%s\nPer.  \t%f\nNode  \t%f\ninc   \t%f\nq     \t%f\n", str, w*180.0/PI, Node*180.0/PI, inc*180.0/PI, q);
		if(this->klog>0) fprintf(this->slog->FLOG, "\n\nOrb Elsements\n\nT\t%s\nPer.\t%f\nNode\t%f\ninc\t%f\nq\t%f\n\n", str, w*180.0/PI, Node*180.0/PI, inc*180.0/PI, q);
//	}

	if(this->klog>0)
	{
		fprintf(this->slog->FLOG, "eJD\t\t\t\tM0\t\t\tw\t\t\tNode\t\tinc\t\t\tecc\t\t\tq\n");
		fprintf(this->slog->FLOG, "%f\t%f\t%f\t%f\t%f\t%f\t%f\n\n", eJD, rad2grad(M0), rad2grad(w), rad2grad(Node), rad2grad(inc), ecc, q);
	}

	this->res->initElem(eJD, TT, w, Node, inc, ecc, q);
	this->res->set(this->resCat);
	char *orbName;
	orbName = new char[STRLEN];
	sprintf(orbName, "%s_%d", this->suff, this->comvar->Ckey);
	resCat->record->set_name(orbName);
	resCat->record->set_author("pvdPorabola");
	resCat->record->set_number(resCat->nstr+1);
	this->resCat->AddRec();//
//
	delete [] pv;
	delete [] str;
	return 0;
}

int pvdP::computeOrb1()
{
	double P1, P2, P3;
	double p, pp, inc, Node, TT, T;
	double ecc, q, B, sg0, r0rt0, v0, r0, nn;
	double u0;
	double w;
	double M0, eJD, ss, dT, Sign;
	double ei, eN, eP;
	char *str = new char[64];

	

	myvector *r, *rt;
	r = new myvector(3);
	rt = new myvector(3);
	int n;
	n = this->comvar->Ckey;
//	n = 0;
	r->set(0, this->comvar->chv[n].r[0]);
	r->set(1, this->comvar->chv[n].r[1]);
	r->set(2, this->comvar->chv[n].r[2]);
	rt->set(0, this->comvar->chv[n].rt[0]);
	rt->set(1, this->comvar->chv[n].rt[1]);
	rt->set(2, this->comvar->chv[n].rt[2]);

/*
	r->set(0, this->comvar->chv[n].ksi[0]);
	r->set(1, this->comvar->chv[n].ksi[1]);
	r->set(2, this->comvar->chv[n].ksi[2]);
	rt->set(0, this->comvar->chv[n].ksit[0]);
	rt->set(1, this->comvar->chv[n].ksit[1]);
	rt->set(2, this->comvar->chv[n].ksit[2]);
*/
	char *pv;
	pv = new char[STRLEN];
	r->print(pv);
	if(klog>1) fprintf(this->slog->FLOG, "\n\nr ecl:	%s	%f", pv, r->norm());
	rt->print(pv);
	if(klog>1) fprintf(this->slog->FLOG, "\nrt ecl:	%s	%f", pv, rt->norm());
//
/*
	r->RotX(EKV);
	rt->RotX(EKV);

//
	r->print(pv);
	if(klog) fprintf(this->slog->FLOG, "\n\nr ekv:	%s	%f", pv, r->norm());
	rt->print(pv);
	if(klog) fprintf(this->slog->FLOG, "\nrt ekv:	%s	%f", pv, rt->norm());
*/

//	this->res = new orbit(this->klog);
//	this->res->FindOrb(r, rt, this->vpos->t, 0);
//
	r0rt0 = Smul(r, rt);

	p = 2.0*r->norm() - (r0rt0*r0rt0)/(ka*ka);

	sg0 = (r0rt0)/(ka*sqrt(p));

	TT = this->vpos->t - (pow(p, 1.5)*(sg0 + sg0*sg0*sg0/3.0))/(2.0*ka);

	P1 = r->get(1)*rt->get(2) - r->get(2)*rt->get(1);
	P2 = r->get(0)*rt->get(2) - r->get(2)*rt->get(0);
	P3 = r->get(0)*rt->get(1) - r->get(1)*rt->get(0);

	piNsys(&pp, &inc, &Node, P1, P2, P3);

	q = 0.5*p;
	ecc = 1.0;

	P1 = sqrt(p)*r0rt0;
	P2 = ka*(p-r->norm());

	v0 = atan2(P1, P2);

	P1 = r->get(2)/sin(inc);
	P2 = r->get(0)*cos(Node) + r->get(1)*sin(Node);

	u0 = atan2(P1, P2);

	w = u0 - v0;

	if(w<0.0) w+=2.0*PI;

	Ekv2Ecl(&ei, &eN, &eP, inc, Node, w);
//	Ecl2Ekv(&ei, &eN, &eP, inc, Node, w);
	w = eP;
	Node = eN;
	inc = ei;
	if(w<0.0) w+=2.0*PI;
//
//
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	M0 = 0.0;
	Sign = 1.0;
//	eJD = (double)(int)this->appcnt->t;
	T=TT*2.0;
	T-=chet(T);
	eJD = ((double)(int)T)/2.0+1.0;
	dT = eJD - TT;// + 1;
//	dT = TT - eJD;

	nn = ka*pow(q, -1.5)/sqrt(2.0);

	M0 = nn*dT;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//	eJD += 1.0;
//	orbit *baseOrb;
//	baseOrb = new orbit;
/*
	if(this->baseCat->GetRecNum(atoi(this->suff))!=-1)
	{	
		baseOrb->get(baseCat);
		printf("\n\nOrb Elsements\t%d\t%s\n\n", baseCat->record->number, baseCat->record->name);
		printf("T\t\t%f\n", TT);
		printf("Per.\t\t%f\t%f\n", w*180.0/PI, rad2grad(baseOrb->elem->w));
		printf("Node\t\t%f\t%f\n", Node*180.0/PI, rad2grad(baseOrb->elem->Node));
		printf("inc\t\t%f\t%f\n", inc*180.0/PI, rad2grad(baseOrb->elem->inc));
		printf("q\t\t%f\t%f\n", q, baseOrb->elem->q);
		printf("ecc\t\t1.0\t\t%f\n", baseOrb->elem->ec);
		if(this->klog)
		{
			fprintf(this->slog->FLOG, "\n\nOrb Elsements\t%d\t%s\n\n", baseCat->record->number, baseCat->record->name);
			fprintf(this->slog->FLOG, "T\t\t%f\n", TT);
			fprintf(this->slog->FLOG, "Per.\t\t%f\t%f\n", w*180.0/PI, rad2grad(baseOrb->elem->w));
			fprintf(this->slog->FLOG, "Node\t\t%f\t%f\n", Node*180.0/PI, rad2grad(baseOrb->elem->Node));
			fprintf(this->slog->FLOG, "inc\t\t\t%f\t%f\n", inc*180.0/PI, rad2grad(baseOrb->elem->inc));
			fprintf(this->slog->FLOG, "q\t\t\t%f\t%f\n", q, baseOrb->elem->q);
			fprintf(this->slog->FLOG, "ecc\t\t\t1.0\t\t\t%f\n", baseOrb->elem->ec);
		}
	}
	else 
	{*/
		dat2YMD_str(TT, str);
		printf("\n\nOrb Elsements\n\nT     \t%s\nPer.  \t%f\nNode  \t%f\ninc   \t%f\nq     \t%f\n", str, w*180.0/PI, Node*180.0/PI, inc*180.0/PI, q);
		if(this->klog>0) fprintf(this->slog->FLOG, "\n\nOrb Elsements\n\nT\t%s\nPer.\t%f\nNode\t%f\ninc\t%f\nq\t%f\n\n", str, w*180.0/PI, Node*180.0/PI, inc*180.0/PI, q);
//	}

	if(this->klog>0)
	{
		fprintf(this->slog->FLOG, "eJD\t\t\t\tM0\t\t\tw\t\t\tNode\t\tinc\t\t\tecc\t\t\tq\n");
		fprintf(this->slog->FLOG, "%f\t%f\t%f\t%f\t%f\t%f\t%f\n\n", eJD, rad2grad(M0), rad2grad(w), rad2grad(Node), rad2grad(inc), ecc, q);
	}

	this->res->initElem(eJD, TT, w, Node, inc, ecc, q);
	this->res->set(this->resCat);
	char *orbName;
	orbName = new char[STRLEN];
	sprintf(orbName, "%s_%d", this->suff, this->comvar->Ckey);
	resCat->record->set_name(orbName);
	resCat->record->set_author("pvdPorabola");
	resCat->record->set_number(resCat->nstr+1);
	this->resCat->AddRec();

//
	delete [] pv;
	delete [] str;
	return 0;
}

int pvdP::computeOrb2()
{
	double P1, P2, P3;
	double p, pp, inc, Node, TT, T;
	double ecc, q, B, sg0, r0rt0, v0, r0, nn;
	double u0;
	double w;
	double M0, eJD, ss, dT, Sign;
	double ei, eN, eP;
	char *str = new char[64];

	

	double *dr, *drt;
	dr = new double[3];
	drt = new double[3];
	double rnorm;

	myvector *r, *rt;
	r = new myvector(3);
	rt = new myvector(3);
	int n;
	n = this->comvar->Ckey;
//	n = 0;
	r->set(0, this->comvar->chv[n].r[0]);
	r->set(1, this->comvar->chv[n].r[1]);
	r->set(2, this->comvar->chv[n].r[2]);
	rt->set(0, this->comvar->chv[n].rt[0]);
	rt->set(1, this->comvar->chv[n].rt[1]);
	rt->set(2, this->comvar->chv[n].rt[2]);

	dr[0] = this->comvar->chv[n].r[0];
	dr[1] = this->comvar->chv[n].r[1];
	dr[2] = this->comvar->chv[n].r[2];
	drt[0] = this->comvar->chv[n].rt[0];
	drt[1] = this->comvar->chv[n].rt[1];
	drt[2] = this->comvar->chv[n].rt[2];

	if(klog>3)
	{
		dr[0] = this->comvar->testVal[7];
		dr[1] = this->comvar->testVal[8];
		dr[2] = this->comvar->testVal[9];
		drt[0] = this->comvar->testVal[11];
		drt[1] = this->comvar->testVal[12];
		drt[2] = this->comvar->testVal[13];
	}

//	RotX(dr, &this->comvar->chv[n].r[0], -EKV);
//	RotX(drt, &this->comvar->chv[n].rt[0], -EKV);

//	printf("\ndr %f %f %f\n\n", dr[0], dr[1], dr[2]);
/*
	r->set(0, this->comvar->chv[n].ksi[0]);
	r->set(1, this->comvar->chv[n].ksi[1]);
	r->set(2, this->comvar->chv[n].ksi[2]);
	rt->set(0, this->comvar->chv[n].ksit[0]);
	rt->set(1, this->comvar->chv[n].ksit[1]);
	rt->set(2, this->comvar->chv[n].ksit[2]);
*/
	char *pv;
	pv = new char[STRLEN];
	r->print(pv);
	if(klog>1) fprintf(this->slog->FLOG, "\n\nr ekv:\t%s\t%f", pv, r->norm());
	if(klog>7) fprintf(this->slog->FLOG, "\nr true:\t%f\t%f\t%f\t%f", this->comvar->testVal[7], this->comvar->testVal[8], this->comvar->testVal[9], sqrt(this->comvar->testVal[7]*this->comvar->testVal[7] + this->comvar->testVal[8]*this->comvar->testVal[8] + this->comvar->testVal[9]*this->comvar->testVal[9]));
	rt->print(pv);
	if(klog>1) fprintf(this->slog->FLOG, "\nrt ekv:\t%s	%f", pv, rt->norm());
	if(klog>7) fprintf(this->slog->FLOG, "\nr true:\t%f\t%f\t%f\t%f", this->comvar->testVal[11], this->comvar->testVal[12], this->comvar->testVal[13], sqrt(this->comvar->testVal[11]*this->comvar->testVal[11] + this->comvar->testVal[12]*this->comvar->testVal[12] + this->comvar->testVal[13]*this->comvar->testVal[13]));
//
/*
	r->RotX(EKV);
	rt->RotX(EKV);
/*
//
	r->print(pv);
	if(klog) fprintf(this->slog->FLOG, "\n\nr ekv:	%s	%f", pv, r->norm());
	rt->print(pv);
	if(klog) fprintf(this->slog->FLOG, "\nrt ekv:	%s	%f", pv, rt->norm());
*/

//	this->res = new orbit(this->klog);
//	this->res->FindOrb(r, rt, this->vpos->t, 0);
//
	r0rt0 = dr[0]*drt[0] + dr[1]*drt[1] + dr[2]*drt[2];

	rnorm = sqrt(dr[0]*dr[0] + dr[1]*dr[1] + dr[2]*dr[2]);
	p = 2.0*(rnorm) - (r0rt0*r0rt0)/(ka*ka);

	sg0 = (r0rt0)/(ka*sqrt(p));

	TT = this->vpos->t - (pow(p, 1.5)*(sg0 + sg0*sg0*sg0/3.0))/(2.0*ka);

	P1 = dr[1]*drt[2] - dr[2]*drt[1];
	P2 = -(dr[2]*drt[0] - dr[0]*drt[2]);
	P3 = dr[0]*drt[1] - dr[1]*drt[0];

	piNsys(&pp, &inc, &Node, P1, P2, P3);

	if(klog>1) fprintf(this->slog->FLOG, "\np %e\tpp %e\n", p, pp);

	q = 0.5*p;
//	q = p;
	ecc = 1.0;

	P1 = sqrt(p)*r0rt0;
	P2 = ka*(p-rnorm);

	v0 = atan2(P1, P2);

	P1 = dr[2]/sin(inc);
	P2 = dr[0]*cos(Node) + dr[1]*sin(Node);

	u0 = atan2(P1, P2);

	w = u0 - v0;

	if(w<0.0) w+=2.0*PI;

//
	Ekv2Ecl(&ei, &eN, &eP, inc, Node, w);
//	Ecl2Ekv(&ei, &eN, &eP, inc, Node, w);
	w = eP;
	Node = eN;
	inc = ei;
	if(w<0.0) w+=2.0*PI;
//
//
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/*	M0 = 0.0;
	Sign = 1.0;
//	eJD = (double)(int)this->appcnt->t;
	T=TT*2.0;
	T-=chet(T);
	eJD = ((double)(int)T)/2.0+1.0;
	dT = eJD - TT;// + 1;
//	dT = TT - eJD;

	nn = ka*pow(q, -1.5)/sqrt(2.0);

	M0 = nn*dT;*/
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	eJD = (double)(int)this->comvar->t + 0.5;

//	eJD += 1.0;
//	orbit *baseOrb;
//	baseOrb = new orbit;
/*
	if(this->baseCat->GetRecNum(atoi(this->suff))!=-1)
	{	
		baseOrb->get(baseCat);
		printf("\n\nOrb Elsements\t%d\t%s\n\n", baseCat->record->number, baseCat->record->name);
		printf("T\t\t%f\n", TT);
		printf("Per.\t\t%f\t%f\n", w*180.0/PI, rad2grad(baseOrb->elem->w));
		printf("Node\t\t%f\t%f\n", Node*180.0/PI, rad2grad(baseOrb->elem->Node));
		printf("inc\t\t%f\t%f\n", inc*180.0/PI, rad2grad(baseOrb->elem->inc));
		printf("q\t\t%f\t%f\n", q, baseOrb->elem->q);
		printf("ecc\t\t1.0\t\t%f\n", baseOrb->elem->ec);
		if(this->klog)
		{
			fprintf(this->slog->FLOG, "\n\nOrb Elsements\t%d\t%s\n\n", baseCat->record->number, baseCat->record->name);
			fprintf(this->slog->FLOG, "T\t\t%f\n", TT);
			fprintf(this->slog->FLOG, "Per.\t\t%f\t%f\n", w*180.0/PI, rad2grad(baseOrb->elem->w));
			fprintf(this->slog->FLOG, "Node\t\t%f\t%f\n", Node*180.0/PI, rad2grad(baseOrb->elem->Node));
			fprintf(this->slog->FLOG, "inc\t\t\t%f\t%f\n", inc*180.0/PI, rad2grad(baseOrb->elem->inc));
			fprintf(this->slog->FLOG, "q\t\t\t%f\t%f\n", q, baseOrb->elem->q);
			fprintf(this->slog->FLOG, "ecc\t\t\t1.0\t\t\t%f\n", baseOrb->elem->ec);
		}
	}
	else 
	{*/
		dat2YMD_str(TT, str);
		printf("\n\nOrb Elsements\n\nT     \t%s\nPer.  \t%f\nNode  \t%f\ninc   \t%f\nq     \t%f\n", str, w*180.0/PI, Node*180.0/PI, inc*180.0/PI, q);
		if(this->klog>0) fprintf(this->slog->FLOG, "\n\nOrb Elsements\n\nT\t%s\nPer.\t%f\nNode\t%f\ninc\t%f\nq\t%f\n\n", str, w*180.0/PI, Node*180.0/PI, inc*180.0/PI, q);
//	}

	double *P, *Q;
	P = new double[3];
	Q = new double[3];

	P[0] = cos(w)*cos(Node) - sin(w)*sin(Node)*cos(inc);
	P[1] = cos(w)*sin(Node) + sin(w)*cos(Node)*cos(inc);
	P[2] = sin(w)*sin(inc);
	Q[0] = -sin(w)*cos(Node) - cos(w)*sin(Node)*cos(inc);
	Q[1] = -sin(w)*sin(Node) + cos(w)*cos(Node)*cos(inc);
	Q[2] = cos(w)*sin(inc);

	if(this->klog>0)
	{
//		fprintf(this->slog->FLOG, "eJD\t\t\t\tM0\t\t\tw\t\t\tNode\t\tinc\t\t\tecc\t\t\tq\n");
//		fprintf(this->slog->FLOG, "%f\t%f\t%f\t%f\t%f\t%f\t%f\n\n", eJD, rad2grad(M0), rad2grad(w), rad2grad(Node), rad2grad(inc), ecc, q);
		fprintf(this->slog->FLOG, "P\tQ\n%f\t%f\n%f\t%f\n%f\t%f\n\n", P[0], P[1], P[2], Q[0], Q[1], Q[2]);
	}

	this->res->initElem(eJD, TT, w, Node, inc, ecc, q);
	this->res->set(this->resCat);
	char *orbName;
	orbName = new char[STRLEN];
	sprintf(orbName, "%s_%d", this->suff, this->comvar->Ckey);
	resCat->record->set_name(orbName);
	resCat->record->set_author("pvdPorabola");
	resCat->record->set_number(resCat->nstr+1);
	this->resCat->AddRec();


	delete [] orbName;
	delete [] P;
	delete [] Q;
	delete [] pv;
	delete [] str;
	delete [] dr;
	delete [] drt;
	return 0;
}

/*
int proofRV(myvector *r, myvector *v)
{
	double 
	return 0;
}
*/

int pvdP::initVarConst()
{
	double norm;
	double *x;
	x = new double[4];
	char *str;
	str = new char[STRLEN];

	this->vpos->coords->Get(x, 0);
	this->comvar->t0 = x[0];
	this->obs->det_observ(this->comvar->t0);
/*
	this->vpos->obs->getmpc(tmpc, 0);
	this->sun->getNoteTime(temp, tmpc->eJD);
	this->comvar->g0 = new myvector(3);
	this->comvar->g0->set(0, temp->x);
	this->comvar->g0->set(1, temp->y);
	this->comvar->g0->set(2, temp->z);
*/
	this->comvar->g0 = new myvector(3);
	this->comvar->g0->set(0, this->obs->ox);
	this->comvar->g0->set(1, this->obs->oy);
	this->comvar->g0->set(2, this->obs->oz);

//	this->coord->Get(x, 0);
	this->comvar->t = this->vpos->t;
	this->obs->det_observ(this->comvar->t);
	
	this->comvar->g = new myvector(3);
	this->comvar->g->set(0, this->obs->ox);
	this->comvar->g->set(1, this->obs->oy);
	this->comvar->g->set(2, this->obs->oz);

	if(klog>3)
	{
		this->comvar->g->set(0, this->comvar->testVal[27]);
		this->comvar->g->set(1, this->comvar->testVal[28]);
		this->comvar->g->set(2, this->comvar->testVal[29]);
	}

//	this->comvar->t = temp->eJD;

	this->comvar->G = new myvector(3);
	norm = this->comvar->g->norm(); 
	this->comvar->G->set(0, this->comvar->g->get(0)/norm);
	this->comvar->G->set(1, this->comvar->g->get(1)/norm);
	this->comvar->G->set(2, this->comvar->g->get(2)/norm);
	if(this->klog>1)
	{
		fprintf(this->slog->FLOG, "\nINIT_VAR:\n\n", str);
		this->comvar->g->print(str);
		fprintf(this->slog->FLOG, "g:\n%s\n", str);
		this->comvar->G->print(str);
		fprintf(this->slog->FLOG, "G:\n%s	%f\n", str, this->comvar->G->norm());
	}
//gt
	this->comvar->gt = new myvector(3);
	this->comvar->gt->set(0, this->obs->ovx);
	this->comvar->gt->set(1, this->obs->ovy);
	this->comvar->gt->set(2, this->obs->ovz);

	if(klog>3)
	{
		this->comvar->gt->set(0, this->comvar->testVal[31]);
		this->comvar->gt->set(1, this->comvar->testVal[32]);
		this->comvar->gt->set(2, this->comvar->testVal[33]);
	}

	if(this->klog>1)
	{
		this->comvar->gt->print(str);
		fprintf(this->slog->FLOG, "gt:\n%s\n", str);
	}
//gtt
	this->comvar->gtt = new myvector(3);
	this->comvar->gtt->set(0, this->obs->ovxt);
	this->comvar->gtt->set(1, this->obs->ovyt);
	this->comvar->gtt->set(2, this->obs->ovzt);

	if(klog>3)
	{
		this->comvar->gt->set(0, this->comvar->testVal[34]);
		this->comvar->gt->set(1, this->comvar->testVal[35]);
		this->comvar->gt->set(2, this->comvar->testVal[36]);
	}

	if(this->klog>1)
	{
		this->comvar->gtt->print(str);
		fprintf(this->slog->FLOG, "gtt:\n%s\n", str);
	}
	
	this->comvar->g2 = this->comvar->g->get(0)*this->comvar->g->get(0) + this->comvar->g->get(1)*this->comvar->g->get(1) + this->comvar->g->get(2)*this->comvar->g->get(2);
	this->comvar->gt2 = this->comvar->gt->get(0)*this->comvar->gt->get(0) + this->comvar->gt->get(1)*this->comvar->gt->get(1) + this->comvar->gt->get(2)*this->comvar->gt->get(2);
	this->comvar->gtt2 = this->comvar->gtt->get(0)*this->comvar->gtt->get(0) + this->comvar->gtt->get(1)*this->comvar->gtt->get(1) + this->comvar->gtt->get(2)*this->comvar->gtt->get(2);
/*
	if(this->klog)
	{
		this->vpos->D->print(str);
		fprintf(this->slog->FLOG, "D:\n%s\n", str);
	}
*/
	this->vpos->coords->Get(x, this->vpos->coords->GetNum());
	this->comvar->t1 = x[0];
	this->obs->det_observ(this->comvar->t1);

	this->comvar->g1 = new myvector(3);
	this->comvar->g1->set(0, this->obs->ox);
	this->comvar->g1->set(1, this->obs->oy);
	this->comvar->g1->set(2, this->obs->oz);

	delete [] x;
	delete [] str;

	return 0;
}

int pvdP::initVar()
{
	char *str;
	str = new char[STRLEN];
	double norm;
	double *x;
	x = new double[4];

	this->comvar->T = new myvector(3);
	this->comvar->T->set(0, -sin(this->vpos->psi->val)*sin(this->vpos->r->val) - cos(this->vpos->psi->val)*sin(this->vpos->d->val)*cos(this->vpos->r->val));
	this->comvar->T->set(1, sin(this->vpos->psi->val)*cos(this->vpos->r->val) - cos(this->vpos->psi->val)*sin(this->vpos->d->val)*sin(this->vpos->r->val));
	this->comvar->T->set(2, cos(this->vpos->psi->val)*cos(this->vpos->d->val));

	if(this->klog>1)
	{
		this->comvar->T->print(str);
		fprintf(this->slog->FLOG, "T:\n%s\n", str);
	}
//M
	this->comvar->M = new myvector(3);
	*this->comvar->M = Vmul(this->vpos->D, this->comvar->T);

	if(this->klog>1)
	{
		this->comvar->M->print(str);
		fprintf(this->slog->FLOG, "M:\n%s\n", str);
	}
//R
	this->comvar->R = new myvector(3);
	this->comvar->chv[this->comvar->Ckey].R = new myvector(3);

	this->comvar->GsD = Smul(this->comvar->G, this->vpos->D);
	this->comvar->gsD = Smul(this->comvar->g, this->vpos->D);
	this->comvar->gtsDt = Smul(this->comvar->gt, this->vpos->Dt);
	this->comvar->gtsD = Smul(this->comvar->gt, this->vpos->D);
	this->comvar->gtsT = Smul(this->comvar->gt, this->comvar->T);
	this->comvar->gttsT = Smul(this->comvar->gtt, this->comvar->T);

	this->comvar->gsT = Smul(this->comvar->g, this->comvar->T);
	this->comvar->gttsM = Smul(this->comvar->gtt, this->comvar->M);
	this->comvar->gsM = Smul(this->comvar->g, this->comvar->M);
	this->comvar->GsM = Smul(this->comvar->G, this->comvar->M);
	this->comvar->signGM = sign(this->comvar->GsM);
/*
	if(this->klog)
	{
		fprintf(this->slog->FLOG, "gsD\t%e\ngtsDt\t%e\ngtsT\t%e\ngttsT\t%e\n", this->comvar->gsD, this->comvar->gtsDt, this->comvar->gtsT, this->comvar->gttsT);
		fprintf(this->slog->FLOG, "gsT\t%e\n", this->comvar->gsT);
		fprintf(this->slog->FLOG, "gttsM\t%e\n", this->comvar->gttsM);
		fprintf(this->slog->FLOG, "gsM\t%e\n", this->comvar->gsM);
		fprintf(this->slog->FLOG, "GsM\t%e\n", this->comvar->GsM);
		fprintf(this->slog->FLOG, "sign(GsM)\t%e\n\n", this->comvar->signGM);
	}
*/
//
	this->vpos->coords->Get(x, 0);
//	this->comvar->t0 = x[0];
	this->comvar->r0 = x[1];
	this->comvar->d0 = x[2];

//	this->vpos->obs->getmpc(ttt, (this->vpos->obs->nmpcs-1)/2);
	this->comvar->r = this->vpos->r->val;
	this->comvar->d = this->vpos->d->val;

	this->vpos->coords->Get(x, this->vpos->coords->GetNum());
	this->comvar->r1 = x[1];
	this->comvar->d1 = x[2];

	delete [] x;
	delete [] str;

	return 0;
}

int pvdP::provTDM()
{
	double *SM = new double [5];

	int res = 0;

	SM[0] = sqrt(this->comvar->T->get(0)*this->comvar->T->get(0) + this->comvar->T->get(1)*this->comvar->T->get(1) + this->comvar->T->get(2)*this->comvar->T->get(2));
	if(fabs(SM[0]-1.0)>EPS) res += 1;
	SM[1] = sqrt(this->vpos->D->get(0)*this->vpos->D->get(0) + this->vpos->D->get(1)*this->vpos->D->get(1) + this->vpos->D->get(2)*this->vpos->D->get(2));
	if(fabs(SM[1]-1.0)>EPS) res += 10;
	SM[2] = Smul(this->vpos->D, this->comvar->T);
	if(fabs(SM[2])>EPS) res += 100;

	SM[3] = Smul(this->vpos->T, this->comvar->M);
	if(fabs(SM[3])>EPS) res += 1000;

	SM[4] = Smul(this->vpos->M, this->vpos->D);
	if(fabs(SM[4])>EPS) res += 10000;

	if(klog>1)
	{
		fprintf(slog->FLOG, "\nproofTDM\n\n");

		fprintf(slog->FLOG, "Tnorm\t=\t%e\t?= 1.0\n", SM[0]);
		fprintf(slog->FLOG, "Dnorm\t=\t%e\t?= 1.0\n", SM[1]);
		fprintf(slog->FLOG, "DT\t\t=\t%e\t?= 0.0\n", SM[2]);
		fprintf(slog->FLOG, "TM\t\t=\t%e\t?= 0.0\n", SM[3]);
		fprintf(slog->FLOG, "MD\t\t=\t%e\t?= 0.0\n", SM[4]);

		if(!res) fprintf(slog->FLOG, "Проверка пройдена успешно\n");
		else fprintf(slog->FLOG, "Проверка с ошибкой!!!\n");
		fprintf(slog->FLOG, "res = %d\n\n", res);
	}

	delete [] SM;
	return(res);
}

int pvdP::provRGD()
{
	double SM;
	SM = sqrt(this->comvar->R->get(0)*this->comvar->R->get(0) + this->comvar->R->get(1)*this->comvar->R->get(1) + this->comvar->R->get(2)*this->comvar->R->get(2));

//	if(fabs(sqrt(this->comvar->R->get(0)*this->comvar->R->get(0) + this->comvar->R->get(1)*this->comvar->R->get(1) + this->comvar->R->get(2)*this->comvar->R->get(2))-1.0)>EPS) return 1;

	SM = Smul(&Vmul(this->vpos->D, this->comvar->R), this->comvar->g);
	return(fabs(Smul(&Vmul(this->vpos->D, this->comvar->R), this->comvar->g))>EPS);
}

int pvdP::proofR()
{
//	if(this->comvar->chv[this->comvar->last].iter > 20) return 1;

//	double f = ka*ka/(this->comvar->chv[this->comvar->last].rnorm*this->comvar->chv[this->comvar->last].rnorm) - Smul(this->comvar->gtt, this->comvar->R) + (this->comvar->chv[this->comvar->last].dtt - this->vpos->m->val*this->vpos->m->val*this->comvar->chv[this->comvar->last].d)*Smul(this->vpos->D, this->comvar->R) + (this->vpos->mt->val*this->comvar->chv[this->comvar->last].d+2.0*this->vpos->m->val*this->comvar->chv[this->comvar->last].dt)*Smul(this->comvar->T, this->comvar->R) - sign(Smul(this->comvar->g, &Vmul(this->vpos->D, this->comvar->T)))*this->vpos->m->val*this->vpos->m->val*this->comvar->chv[this->comvar->last].d*sqrt(this->vpos->c->val*this->vpos->c->val - 1.0)*Smul(&Vmul(this->vpos->D, this->comvar->T), this->comvar->R);
//	printf("\nf	%f\n", f);

	return 0;
}
/*
double slagA()
{
	return(ka*ka/(this->comvar->chv[this->comvar->last].rnorm*this->comvar->chv[this->comvar->last].rnorm));
}

double slagB()
{
	return(-Smul(this->comvar->gtt, this->comvar->R));
}

double slagC()
{
	return((this->comvar->chv[this->comvar->last].dtt);
}
*/

int pvdP::aclose()
{
	fclose(this->slog->FLOG);
	return 0;
}

int pvdP::detCkey()
{
	this->comvar->Ckey = 1;
//	if(fabs(this->vpos->c->val-1.0)<CKEY_RANGE) this->comvar->Ckey = 1;
	return 0;
}

int pvdP::proofPVD()
{
	double day;
	int year, mouth, sgn;
	char *ssgn;
	myvector *T, *M;
	char *pv;
	double aV, dV, oc;
	myvector *D, *Dt, *Dtt;
	double d, dt, dtt;
	myvector *vdtt;
	double m, psi;
	double mt, c;
	double val;
	

	vdtt = new myvector(3);
	ssgn = new char[16];
	T = new myvector(3);
	M = new myvector(3);
	pv = new char[1000];
	D = new myvector(3);
	Dt = new myvector(3);
	Dtt = new myvector(3);

	

//

	printf("\nproofPVD\n");
	printf("\nparam\tcount\t\ttrue\t\t\tO-C\n");
	if(this->klog>0) 
	{
		fprintf(this->slog->FLOG, "\nПроверка ПВД\n");
		fprintf(this->slog->FLOG, "\nparam\tЗасечка\t\tПВД\t\t\t\tO-C\n");
	}

	

//	d = sqrt(this->comvar->chv[this->comvar->Ckey].ksi[0]*this->comvar->chv[this->comvar->Ckey].ksi[0] + this->comvar->chv[this->comvar->Ckey].ksi[1]*this->comvar->chv[this->comvar->Ckey].ksi[1] + this->comvar->chv[this->comvar->Ckey].ksi[2]*this->comvar->chv[this->comvar->Ckey].ksi[2]);
//	dt = (this->comvar->chv[this->comvar->Ckey].ksit[0]*this->comvar->chv[this->comvar->Ckey].ksi[0] + this->comvar->chv[this->comvar->Ckey].ksit[1]*this->comvar->chv[this->comvar->Ckey].ksi[1] + this->comvar->chv[this->comvar->Ckey].ksit[2]*this->comvar->chv[this->comvar->Ckey].ksi[2])/d;
	d = this->comvar->chv[this->comvar->Ckey].d;
	dt = this->comvar->chv[this->comvar->Ckey].dt;

	D->set(0, this->comvar->chv[this->comvar->Ckey].ksi[0]/d);
	D->set(1, this->comvar->chv[this->comvar->Ckey].ksi[1]/d);
	D->set(2, this->comvar->chv[this->comvar->Ckey].ksi[2]/d);

	aV = atan2(D->get(1), D->get(0));
	if(aV<0.0) aV+=2.0*PI;
	dV = atan2(D->get(2), sqrt(D->get(0)*D->get(0)+ D->get(1)*D->get(1)));

	rad2hms(aV, &sgn, &year, &mouth, &day);
	if(sgn==0) strcpy(ssgn, "-");
	else strcpy(ssgn, "");
	printf("\nRA\t%s%d %d %9.6f", ssgn, year ,mouth ,day);
	if(this->klog>0) fprintf(this->slog->FLOG, "\nRA\t%s%d %d %9.6f", ssgn, year ,mouth ,day);
	rad2hms(this->vpos->r->val, &sgn, &year, &mouth, &day);
	if(sgn==0) strcpy(ssgn, "-");
	else strcpy(ssgn, "");
	printf("\t%s%d %d %9.6f", ssgn, year ,mouth ,day);
	if(this->klog>0) fprintf(this->slog->FLOG, "\t%s%d %d %9.6f", ssgn, year ,mouth ,day);

	rad2gms(dV, &sgn, &year, &mouth, &day);
	if(sgn==0) strcpy(ssgn, "-");
	else strcpy(ssgn, "");
	printf("\nDE\t%s%d %d %9.6f", ssgn, year ,mouth ,day);
	if(this->klog>0) fprintf(this->slog->FLOG, "\nDE\t%s%d %d %9.6f", ssgn, year ,mouth ,day);
	rad2gms(this->vpos->d->val, &sgn, &year, &mouth, &day);
	if(sgn==0) strcpy(ssgn, "-");
	else strcpy(ssgn, "");
	printf("\t%s%d %d %9.6f", ssgn, year ,mouth ,day);
	if(this->klog>0) fprintf(this->slog->FLOG, "\t%s%d %d %9.6f", ssgn, year ,mouth ,day);

	oc = sqrt((aV - this->vpos->r->val)*(aV - this->vpos->r->val) + (dV - this->vpos->d->val)*(dV - this->vpos->d->val));

	printf("\t%e", oc*SECINRAD);
	if(this->klog>0) fprintf(this->slog->FLOG, "\t%e", oc*SECINRAD);

	Dt->set(0, this->comvar->chv[this->comvar->Ckey].ksit[0]/d - D->get(0)*dt/d);
	Dt->set(1, this->comvar->chv[this->comvar->Ckey].ksit[1]/d - D->get(1)*dt/d);
	Dt->set(2, this->comvar->chv[this->comvar->Ckey].ksit[2]/d - D->get(2)*dt/d);
	

	m = sqrt(Dt->get(0)*Dt->get(0) + Dt->get(1)*Dt->get(1) + Dt->get(2)*Dt->get(2));

	printf("\nm\t%14f\t%14f", m*SECINRAD, this->vpos->m->val*SECINRAD);
	if(this->klog>0) fprintf(this->slog->FLOG, "\nm\t%14f\t%14f", m*SECINRAD, this->vpos->m->val*SECINRAD);

	oc = fabs(m - this->vpos->m->val)*SECINRAD;
	printf("\t%e", oc);
	if(this->klog>0) fprintf(this->slog->FLOG, "\t%e", oc);

	T->set(0, this->comvar->chv[this->comvar->Ckey].ksit[0]/(d*m) - D->get(0)*dt/(d*m));
	T->set(1, this->comvar->chv[this->comvar->Ckey].ksit[1]/(d*m) - D->get(1)*dt/(d*m));
	T->set(2, this->comvar->chv[this->comvar->Ckey].ksit[2]/(d*m) - D->get(2)*dt/(d*m));

	*M = Vmul(D, T);

//	M3 = D[0]*T[1] - D[1]*T[0];

	psi = atan2(M->get(2), T->get(2));
	if(psi<0.0) psi+=2.0*PI;

	printf("\npsi\t%14f\t%14f", psi*180.0/PI, this->vpos->psi->val*180.0/PI);
	if(this->klog>0) fprintf(this->slog->FLOG, "\npsi\t%14f\t%14f", psi*180.0/PI, this->vpos->psi->val*180.0/PI);

	oc = fabs(psi - this->vpos->psi->val)*180.0/PI;
	printf("\t%e", oc);
	if(this->klog>0) fprintf(this->slog->FLOG, "\t%e", oc);


//	dtt = (this->comvar->chv[this->comvar->Ckey].ksitt[0]*D->get(0) + this->comvar->chv[this->comvar->Ckey].ksitt[1]*D->get(1) + this->comvar->chv[this->comvar->Ckey].ksitt[2]*D->get(2)) + d*m*m;
	dtt = this->vpos->Dtt->norm();

	Dtt->set(0, (this->comvar->chv[this->comvar->Ckey].ksitt[0] - dtt*D->get(0) - 2.0*dt*Dt->get(0))/d);
	Dtt->set(1, (this->comvar->chv[this->comvar->Ckey].ksitt[1] - dtt*D->get(1) - 2.0*dt*Dt->get(1))/d);
	Dtt->set(2, (this->comvar->chv[this->comvar->Ckey].ksitt[2] - dtt*D->get(2) - 2.0*dt*Dt->get(2))/d);

	vdtt->set(0, -ka*ka*(this->comvar->chv[this->comvar->Ckey].R->get(0)/(this->comvar->chv[this->comvar->Ckey].rnorm*this->comvar->chv[this->comvar->Ckey].rnorm) - this->comvar->G->get(0)/this->comvar->g2));

	

//	mt = (dtt*Smul(Dtt, T) - 2.0*m*dt)/d;
	mt = Smul(Dt, Dtt)/m;
//	mt = ((this->comvar->chv[this->comvar->Ckey].ksitt[0]*T->get(0) + this->comvar->chv[this->comvar->Ckey].ksitt[1]*T->get(1) + this->comvar->chv[this->comvar->Ckey].ksitt[2]*T->get(2)) - 2.0*m*dt)/d;
//	mt = (Smul(vdtt, T)+2.0*m*dt)/d;

	printf("\nmt\t%14.4f\t%14.4f", mt*SECINRAD, this->vpos->mt->val*SECINRAD);
	if(this->klog>0) fprintf(this->slog->FLOG, "\nmt\t%14.4f\t%14.4f", mt*SECINRAD, this->vpos->mt->val*SECINRAD);

	oc = fabs(mt - this->vpos->mt->val)*SECINRAD;
	printf("\t%e", oc);
	if(this->klog>0) fprintf(this->slog->FLOG, "\t%e", oc);

	c = sqrt(1.0 + pow(((this->comvar->chv[this->comvar->Ckey].ksitt[0]*M->get(0) + this->comvar->chv[this->comvar->Ckey].ksitt[1]*M->get(1) + this->comvar->chv[this->comvar->Ckey].ksitt[2]*M->get(2)))/(m*m*d), 2.0));

	printf("\nc\t%14f\t%14f", c, this->vpos->c->val);
	if(this->klog>0) fprintf(this->slog->FLOG, "\nc\t%14f\t%14f", c, this->vpos->c->val);

	oc = fabs(c - this->vpos->c->val);
	printf("\t%e", oc);
	if(this->klog>0) fprintf(this->slog->FLOG, "\t%e", oc);

//
//	c = this->vpos->c->val;

	D->print(pv);
	printf("\n\nD\t\t%s", pv);
	if(this->klog>1)fprintf(this->slog->FLOG, "\n\nD\t\t\t%s", pv);
	strcpy(pv, "");

	this->vpos->D->print(pv);
	printf("\nvposD\t\t%s", pv);
	if(this->klog>1)fprintf(this->slog->FLOG, "\nvposD\t\t%s", pv);
	strcpy(pv, "");

	Dt->print(pv);
	printf("\nDt\t\t%s", pv);
	if(this->klog>1)fprintf(this->slog->FLOG, "\nDt\t\t\t%s", pv);
	strcpy(pv, "");

	this->vpos->Dt->print(pv);
	printf("\nvposDt\t\t%s", pv);
	if(this->klog>1)fprintf(this->slog->FLOG, "\nvposDt\t\t%s", pv);
	strcpy(pv, "");

	Dtt->print(pv);
	printf("\nDtt\t\t%s", pv);
	if(this->klog>1)fprintf(this->slog->FLOG, "\nDtt\t\t\t%s", pv);
	strcpy(pv, "");

	this->vpos->Dtt->print(pv);
	printf("\nvposDtt\t\t%s\n", pv);
	if(this->klog>1)fprintf(this->slog->FLOG, "\nvposDtt\t\t%s\n", pv);
	strcpy(pv, "");

	if(this->klog>1) fprintf(this->slog->FLOG, "\ncontrol\n");
	val = Smul(D, Dtt);
	printf("\ncontrol\n%e\t%e", val, -m*m);
	if(this->klog>1)fprintf(this->slog->FLOG, "D*Dtt ?= -m^2\t\t\t\t%e\t%e\n", val, -m*m);

	val = Smul(Dt, Dtt);
	printf("\n%e\t%e", val, mt*m);
	if(this->klog>1)fprintf(this->slog->FLOG, "Dt*Dtt ?= mt*m\t\t\t\t%e\t%e\n", val, mt*m);

	val = Smul(Dtt, Dtt);
	printf("\n%e\t%e\n\n", Smul(Dtt, Dtt), mt*mt + m*m*m*m*c*c);
	if(this->klog>1)fprintf(this->slog->FLOG, "Dtt*Dtt ?= mt^2 + m^4*c^2\t%e\t%e\n\n", val, mt*mt + m*m*m*m*c*c);

	printf("\n");
	if(this->klog>0) fprintf(this->slog->FLOG, "\n");

	delete [] pv;
	delete [] ssgn;
	delete vdtt;
	
	return 0;
}

int pvdP::initC()
{
	char *str, *nstr;
	int Strlen = 1024;
	int npos, pos;

	str = new char[Strlen];
	nstr = new char[Strlen];

	char *elem, *sval;

	elem = new char[Strlen];
	sval = new char[Strlen];

	double val;

	strcpy(str, "");
	strcpy(nstr, "");
	strcpy(elem, "");
	strcpy(sval, "");

	while(!feof(this->FINI))
	{
		if(fgets(str, Strlen, this->FINI)==NULL) break;

		pos = 0;
		slovo(str, nstr, pos, &npos);
		strcpy(elem, nstr);

		pos = npos;
		slovo(str, nstr, pos, &npos);

		strcpy(sval, nstr);

		if(streqv(elem, "inird"))
		{
			this->comvar->inird = atof(sval);
		}
		else if(streqv(elem, "itype"))
		{
			this->comvar->Ckey = atoi(sval);
		}
		else if(streqv(elem, "iniPath"))
		{
			strcpy(this->iniPath, sval);
		}
		else if(streqv(elem, "tlen"))
		{
			this->vpos->tlen = atoi(sval);
		}
		else if(streqv(elem, "logging"))
		{
			this->klog = atoi(sval);
		}
	}

	fclose(FINI);

	return 0;
}

int pvdP::printOC(char *name, double aV, double dV, double a, double d, double t, double *ocsumm)
{
	double day, oc;
	int year, mouth, sgn;
	char *ssgn;
	ssgn = new char[1];

	printf("\n%6s ", name);
	if(this->klog>0) fprintf(this->slog->FLOG,"\n%6s ", name);

	dat2YMD(t, &year, &mouth, &day);
	printf("%4d %02d %09.6f", year, mouth, day);
	if(this->klog>0) fprintf(this->slog->FLOG,"%4d %02d %09.6f ", year, mouth, day);

	rad2hms(aV, &sgn, &year, &mouth, &day);
	if(sgn==0) strcpy(ssgn, "-");
	else strcpy(ssgn, " ");
	printf("%s%02d %02d %09.6f ", ssgn, year ,mouth ,day);
	if(this->klog>0) fprintf(this->slog->FLOG, "%s%02d %02d %09.6f ", ssgn, year ,mouth ,day);

	oc = (aV - a)*SECINRAD*cos(dV);
	*ocsumm = fabs(oc);
	if((fabs(oc)>999.999999)||(fabs(oc)<0.000001))
	{
		printf("xxx.xxxxxx ", oc);
		if(this->klog>0) fprintf(this->slog->FLOG, "xxx.xxxxxx ", oc);
	}
	else
	{
		printf("%+010.6f ", oc);
		if(this->klog>0) fprintf(this->slog->FLOG, "%+010.6f ", oc);
	}

	rad2gms(dV, &sgn, &year, &mouth, &day);
	if(sgn==0) strcpy(ssgn, "-");
	else strcpy(ssgn, " ");
	printf("%s%02d %02d %09.6f ", ssgn, year ,mouth ,day);
	if(this->klog>0) fprintf(this->slog->FLOG, "%s%02d %02d %09.6f ", ssgn, year ,mouth ,day);

	oc = (dV - d)*SECINRAD;
	*ocsumm += fabs(oc);
/*	printf("%+0e ", oc);
	if(this->klog) fprintf(this->slog->FLOG, "%+0e ", oc);*/

	if((fabs(oc)>999.999999)||(fabs(oc)<0.000001))
	{
		printf("xxx.xxxxxx ", oc);
		if(this->klog>0) fprintf(this->slog->FLOG, "xxx.xxxxxx ", oc);
	}
	else
	{
		printf("%+010.6f ", oc);
		if(this->klog>0) fprintf(this->slog->FLOG, "%+010.6f ", oc);
	}

	oc = sqrt((aV - a)*(aV - a) + (dV - d)*(dV - d))*SECINRAD;
/*	printf("%+0e ", oc);
	if(this->klog) fprintf(this->slog->FLOG, "%+0e ", oc);*/

	if((fabs(oc)>999.999999)||(fabs(oc)<0.000001))
	{
		printf("xxx.xxxxxx ", oc);
		if(this->klog>0) fprintf(this->slog->FLOG, "xxx.xxxxxx ", oc);
	}
	else
	{
		printf("%+010.6f ", oc);
		if(this->klog>0) fprintf(this->slog->FLOG, "%+010.6f ", oc);
	}
	

	return 0;
}

int pvdP::iterEnd(double r, double d, double dt, myvector *R, int niter, int nitype)
{
	double ksd;

	this->comvar->chv[nitype].r[0] = r*R->get(0);
	this->comvar->chv[nitype].r[1] = r*R->get(1);
	this->comvar->chv[nitype].r[2] = r*R->get(2);
	this->comvar->chv[nitype].rnorm = r;

	this->comvar->chv[nitype].R->set(0, R->get(0));
	this->comvar->chv[nitype].R->set(1, R->get(1));
	this->comvar->chv[nitype].R->set(2, R->get(2));

//	dt = (-(ka*ka*this->comvar->gsT)/(r*r*r) - this->comvar->gttsT - this->vpos->mt->val*d)/(2.0*this->vpos->m->val);

//	this->provTDM();


	this->comvar->chv[nitype].rt[0] = this->comvar->gt->get(0) + dt*this->vpos->D->get(0) + d*this->vpos->m->val*this->comvar->T->get(0);
	this->comvar->chv[nitype].rt[1] = this->comvar->gt->get(1) + dt*this->vpos->D->get(1) + d*this->vpos->m->val*this->comvar->T->get(1);
	this->comvar->chv[nitype].rt[2] = this->comvar->gt->get(2) + dt*this->vpos->D->get(2) + d*this->vpos->m->val*this->comvar->T->get(2);
/*
	this->comvar->chv[nitype].rt[0] = this->comvar->gt->get(0) + dt*(this->comvar->chv[nitype].r[0] - this->comvar->g->get(0))/d + d*this->vpos->m->val*this->comvar->T->get(0);
	this->comvar->chv[nitype].rt[1] = this->comvar->gt->get(1) + dt*(this->comvar->chv[nitype].r[1] - this->comvar->g->get(1))/d + d*this->vpos->m->val*this->comvar->T->get(1);
	this->comvar->chv[nitype].rt[2] = this->comvar->gt->get(2) + dt*(this->comvar->chv[nitype].r[2] - this->comvar->g->get(2))/d + d*this->vpos->m->val*this->comvar->T->get(2);
*/
	this->comvar->chv[nitype].d = d;
	this->comvar->chv[nitype].dt = dt;

	this->comvar->chv[nitype].rtt[0] = -ka*ka*this->comvar->chv[nitype].R->get(0)/(this->comvar->chv[nitype].rnorm*this->comvar->chv[nitype].rnorm);
	this->comvar->chv[nitype].rtt[1] = -ka*ka*this->comvar->chv[nitype].R->get(1)/(this->comvar->chv[nitype].rnorm*this->comvar->chv[nitype].rnorm);
	this->comvar->chv[nitype].rtt[2] = -ka*ka*this->comvar->chv[nitype].R->get(2)/(this->comvar->chv[nitype].rnorm*this->comvar->chv[nitype].rnorm);
		
	this->comvar->chv[nitype].f[0] = 0.0;
	this->comvar->last = 0;
	this->comvar->chv[nitype].iter = niter;

	if(klog>1)
	{
		fprintf(this->slog->FLOG, "\nr:\t\t%15.12f\t%15.12f\t%15.12f\t%15.12f", comvar->chv[nitype].r[0], comvar->chv[nitype].r[1], comvar->chv[nitype].r[2], r);
		if(klog>6)fprintf(this->slog->FLOG, "\nr_true:\t%15.12f\t%15.12f\t%15.12f\t%15.12f", comvar->testVal[7], comvar->testVal[8], comvar->testVal[9], comvar->testVal[10]);

		ksd = sqrt(comvar->chv[nitype].rt[0]*comvar->chv[nitype].rt[0] + comvar->chv[nitype].rt[1]*comvar->chv[nitype].rt[1] + comvar->chv[nitype].rt[2]*comvar->chv[nitype].rt[2]);
		fprintf(this->slog->FLOG, "\nrt:\t\t\t%15.12f\t%15.12f\t%15.12f\t%15.12f", comvar->chv[nitype].rt[0], comvar->chv[nitype].rt[1], comvar->chv[nitype].rt[2], ksd);
		if(klog>6)
		{
			ksd = sqrt(comvar->testVal[11]*comvar->testVal[11] + comvar->testVal[12]*comvar->testVal[12] + comvar->testVal[13]*comvar->testVal[13]);
			fprintf(this->slog->FLOG, "\nrt_true:\t%15.12f\t%15.12f\t%15.12f\t%15.12f", comvar->testVal[11], comvar->testVal[12], comvar->testVal[13], ksd);
		}

		ksd = sqrt(comvar->chv[nitype].rtt[0]*comvar->chv[nitype].rtt[0] + comvar->chv[nitype].rtt[1]*comvar->chv[nitype].rtt[1] + comvar->chv[nitype].rtt[2]*comvar->chv[nitype].rtt[2]);
		fprintf(this->slog->FLOG, "\nrtt:\t\t%15.12f\t%15.12f\t%15.12f\t%15.12f", comvar->chv[nitype].rtt[0], comvar->chv[nitype].rtt[1], comvar->chv[nitype].rtt[2], ksd);
		if(klog>6)
		{
			ksd = sqrt(comvar->testVal[14]*comvar->testVal[14] + comvar->testVal[15]*comvar->testVal[15] + comvar->testVal[16]*comvar->testVal[16]);
			fprintf(this->slog->FLOG, "\nrtt_true:\t%15.12f\t%15.12f\t%15.12f\t%15.12f", comvar->testVal[14], comvar->testVal[15], comvar->testVal[16], ksd);
		}
	}

//
	this->comvar->chv[nitype].ksi[0] = this->comvar->chv[nitype].r[0] - this->comvar->g->get(0);
	this->comvar->chv[nitype].ksi[1] = this->comvar->chv[nitype].r[1] - this->comvar->g->get(1);
	this->comvar->chv[nitype].ksi[2] = this->comvar->chv[nitype].r[2] - this->comvar->g->get(2);

	this->comvar->chv[nitype].ksit[0] = this->comvar->chv[nitype].rt[0] - this->comvar->gt->get(0);
	this->comvar->chv[nitype].ksit[1] = this->comvar->chv[nitype].rt[1] - this->comvar->gt->get(1);
	this->comvar->chv[nitype].ksit[2] = this->comvar->chv[nitype].rt[2] - this->comvar->gt->get(2);

	this->comvar->chv[nitype].ksitt[0] = this->comvar->chv[nitype].rtt[0] - this->comvar->gtt->get(0);
	this->comvar->chv[nitype].ksitt[1] = this->comvar->chv[nitype].rtt[1] - this->comvar->gtt->get(1);
	this->comvar->chv[nitype].ksitt[2] = this->comvar->chv[nitype].rtt[2] - this->comvar->gtt->get(2);

	if(klog>1)
	{
		fprintf(this->slog->FLOG, "\n\nksi");
		ksd = sqrt(comvar->chv[nitype].ksi[0]*comvar->chv[nitype].ksi[0] + comvar->chv[nitype].ksi[1]*comvar->chv[nitype].ksi[1] + comvar->chv[nitype].ksi[2]*comvar->chv[nitype].ksi[2]);
		fprintf(this->slog->FLOG, "\nksi:\t%9.6f\t%9.6f\t%9.6f\t%9.6f ?= %9.6f", comvar->chv[nitype].ksi[0], comvar->chv[nitype].ksi[1], comvar->chv[nitype].ksi[2], ksd, d);
		if(klog>2) fprintf(this->slog->FLOG, "\nd:\t\t%9.6f\t%9.6f\t%9.6f\t%9.6f", comvar->testVal[17], comvar->testVal[18], comvar->testVal[19], comvar->testVal[20]);
/*		comvar->chv[nitype].ksi[0] = comvar->testVal[17];
		comvar->chv[nitype].ksi[1] = comvar->testVal[18];
		comvar->chv[nitype].ksi[2] = comvar->testVal[19];
		d = sqrt(comvar->chv[nitype].ksi[0]*comvar->chv[nitype].ksi[0] + comvar->chv[nitype].ksi[1]*comvar->chv[nitype].ksi[1] + comvar->chv[nitype].ksi[2]*comvar->chv[nitype].ksi[2]);
*/
		ksd = sqrt(comvar->chv[nitype].ksit[0]*comvar->chv[nitype].ksit[0] + comvar->chv[nitype].ksit[1]*comvar->chv[nitype].ksit[1] + comvar->chv[nitype].ksit[2]*comvar->chv[nitype].ksit[2]);
		fprintf(this->slog->FLOG, "\nksit:\t%15.12f\t%15.12f\t%15.12f\t%15.12f ?= %15.12f", comvar->chv[nitype].ksit[0], comvar->chv[nitype].ksit[1], comvar->chv[nitype].ksit[2], ksd, dt);
		if(klog>2) ksd = sqrt(comvar->testVal[21]*comvar->testVal[21] + comvar->testVal[22]*comvar->testVal[22] + comvar->testVal[23]*comvar->testVal[23]);
		if(klog>2) fprintf(this->slog->FLOG, "\ndt:\t\t%15.12f\t%15.12f\t%15.12f\t%15.12f", comvar->testVal[21], comvar->testVal[22], comvar->testVal[23], ksd);
/*
		comvar->chv[nitype].ksit[0] = comvar->testVal[21];
		comvar->chv[nitype].ksit[1] = comvar->testVal[22];
		comvar->chv[nitype].ksit[2] = comvar->testVal[23];
		dt = ksd;
*/
		ksd = sqrt(comvar->chv[nitype].ksitt[0]*comvar->chv[nitype].ksitt[0] + comvar->chv[nitype].ksitt[1]*comvar->chv[nitype].ksitt[1] + comvar->chv[nitype].ksitt[2]*comvar->chv[nitype].ksitt[2]);
		fprintf(this->slog->FLOG, "\nksit:\t%15.12f\t%15.12f\t%15.12f\t%15.12f", comvar->chv[nitype].ksitt[0], comvar->chv[nitype].ksitt[1], comvar->chv[nitype].ksitt[2], ksd);
		if(klog>2) ksd = sqrt(comvar->testVal[24]*comvar->testVal[24] + comvar->testVal[25]*comvar->testVal[25] + comvar->testVal[26]*comvar->testVal[26]);
		if(klog>2) fprintf(this->slog->FLOG, "\ndtt:\t%15.12f\t%15.12f\t%15.12f\t%15.12f\n\n", comvar->testVal[24], comvar->testVal[25], comvar->testVal[26], ksd);
/*
		comvar->chv[nitype].ksitt[0] = comvar->testVal[24];
		comvar->chv[nitype].ksitt[1] = comvar->testVal[25];
		comvar->chv[nitype].ksitt[2] = comvar->testVal[26];
//		dtt = ksd;*/
	}


//D, Dt, Dtt
	double ksiM = modVect(this->comvar->chv[nitype].ksi);
	for(int i=0; i<3; i++)
	{
		comvar->chv[nitype].D[i] = this->comvar->chv[nitype].ksi[i]/d;
		comvar->chv[nitype].Dt[i] = (this->comvar->chv[nitype].rt[i] - dt*comvar->chv[nitype].D[i] - this->comvar->gt->get(i))/d;
//		comvar->chv[nitype].Dtt[i] = 
	}


	return 0;
}
/*
int pvdP::writeCat()
{
	this->res->initElem(eJD, M0, w, Node, inc, ecc, q);
	this->res->set(this->resCat);
	this->resCat->AddRec();


	return 0;
}
*/