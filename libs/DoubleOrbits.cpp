#include "DoubleOrbits.h"

DoubleOrbits::DoubleOrbits()
{
	this->params = NULL;
	this->vars = new Variables;

	this->fn_res = NULL;
	this->fn_varList = NULL;
	this->fn_log = NULL;

	this->nres = 0;

	this->FCFG = NULL;

	this->err_str = new char[124];
	strcpy(this->err_str, "Unnown error\n");
}

DoubleOrbits::~DoubleOrbits()
{
	delete(this->params);
	delete(this->vars);

	delete(this->fn_res);
	delete(this->fn_varList);

	this->nres = 0;

	if(this->FCFG!=NULL) fclose(this->FCFG);
	this->FCFG=NULL;
}

ProgFlags::ProgFlags()
{
	this->isDetective = 0;
	this->isStable = 0;
	this->isVisual = 0;
}

ProgFlags::~ProgFlags()
{
	this->isDetective = 0;
	this->isStable = 0;
	this->isVisual = 0;
}

///////////// RO ///////////////

Variables::Variables()
{
	this->s_vars = new StableVars;
	this->d_vars = new DetectiveVars;
	this->Name = new char[16];
	this->Obj = new note;

	strcpy(this->Name, "");

	this->num = 0.0;
	this->status = -1;
	this->obs = 0;

	this->isDetective = 0;
	this->isStable = 0;

	this->FGUD = NULL;
	this->FOBS = NULL;

	this->fn_obs = NULL;

	this->fn_gud = new char[];
	strcpy(this->fn_gud, "");
}

Variables::Variables(char *fn_gud)
{
	this->s_vars = new StableVars;
	this->d_vars = new DetectiveVars;
	this->Name = new char[16];
	this->Obj = new note;

	strcpy(this->Name, "");

	this->num = 0.0;
	this->status = -1;
	this->obs = 0;

	this->isDetective = 0;
	this->isStable = 0;

	this->FGUD = NULL;
	this->FOBS = NULL;
	this->fn_obs = new char[];
	this->fn_gud = new char[];

	strcpy(this->fn_gud, fn_gud);

	this->FGUD = fopen(fn_gud, "wb+");
}


Variables::~Variables()
{
	delete(this->Obj);
	delete(this->Name);

	this->num = 0.0;
	this->status = -1;

	fclose(FGUD);
	fclose(FOBS);
	FGUD = NULL;
	FOBS = NULL;

	delete(this->fn_gud);
	delete(this->fn_obs);
}

int Variables::init_obs_list(char *fn_obs_list)
{
	strcpy(this->fn_obs, fn_obs_list);
	FOBS = fopen(this->fn_obs, "rb+");
	if(FOBS==NULL) FOBS = fopen(this->fn_obs, "wb+");
	return 0;
}

int Variables::copyTo(Variables *targ)
{
	if(targ==NULL) return 1;

	targ->s_vars->A = this->s_vars->A;
	targ->s_vars->B = this->s_vars->B;
	targ->s_vars->C = this->s_vars->C;
	targ->s_vars->a = this->s_vars->a;
	targ->s_vars->b = this->s_vars->b;
	targ->s_vars->c = this->s_vars->c;
	targ->s_vars->E = this->s_vars->E;
	targ->d_vars->Edist = this->d_vars->Edist;
	targ->s_vars->Ip = this->s_vars->Ip;
	targ->s_vars->K = this->s_vars->K;
	targ->d_vars->m = this->d_vars->m;
	targ->s_vars->M1 = this->s_vars->M1;
	targ->s_vars->M2 = this->s_vars->M2;
	targ->s_vars->mju1 = this->s_vars->mju1;
	targ->s_vars->mju2 = this->s_vars->mju2;
	strcpy(targ->Name, this->Name);
	targ->s_vars->norm = this->s_vars->norm;
	targ->num = this->num;
	targ->d_vars->elongation = this->d_vars->elongation;
	targ->s_vars->r = this->s_vars->r;
	targ->s_vars->R1 = this->s_vars->R1;
	targ->s_vars->R2 = this->s_vars->R2;
	targ->s_vars->r2 = this->s_vars->r2;
	targ->s_vars->r1 = this->s_vars->r1;
	targ->d_vars->Sdist = this->d_vars->Sdist;
	targ->s_vars->T = this->s_vars->T;
	targ->s_vars->U = this->s_vars->U;
	targ->d_vars->Q = this->d_vars->Q;
	targ->d_vars->Qmax = this->d_vars->Qmax;

	targ->s_vars->r1min = this->s_vars->r1min;
	targ->s_vars->r1max = this->s_vars->r1max;
	targ->s_vars->dr1 = this->s_vars->dr1;

	targ->isDetective = this->isDetective;
	targ->isStable = this->isStable;

	targ->Obj->eJD = this->Obj->eJD;
	targ->Obj->R = this->Obj->R;
	targ->Obj->Rt = this->Obj->Rt;
	targ->Obj->V = this->Obj->V;
	targ->Obj->x = this->Obj->x;
	targ->Obj->xt = this->Obj->xt;
	targ->Obj->xtt = this->Obj->xtt;
	targ->Obj->y = this->Obj->y;
	targ->Obj->yt = this->Obj->yt;
	targ->Obj->z = this->Obj->z;
	targ->Obj->zt = this->Obj->zt;
	targ->Obj->ztt = this->Obj->ztt;

	return 0;
}

int Variables::get_best_name()
{
	int i=0;

	double maxnum = -1.0;

	int k=-1;

	while(!this->get_gud_num(i))
	{
		this->get_obs();
		if((!this->status)&&((this->num*(this->obs+1))>maxnum))
		{
				maxnum = this->num*(this->obs+1);
				k=i;
		}
		i++;
	}

	if(k==-1) return 1;

	this->get_gud_num(k);
	this->get_obs();

	return 0;
}

int Variables::get_gud_num(int pos)
{
	fseek(FGUD, pos*(sizeof(char)*32 + sizeof(int) + sizeof(double)*(D_VARS_LEN+S_VARS_LEN+OBJ_VARS_LEN+4)), SEEK_SET);

	char *tname = new char[32];

	if(fread(this->Name, sizeof(char)*32, 1, FGUD)==0) return 2;
	strcpy(&this->Name[32], "\0");
	if(fread(&this->num, sizeof(double), 1, FGUD)==0) return 3;

	if(fread(&this->status, sizeof(int), 1, FGUD)==0) return 3;

	if(fread(&this->isDetective, sizeof(double), 1, FGUD)==0) return 5;
	if(fread(&this->isStable, sizeof(double), 1, FGUD)==0) return 6;
	if(fread(this->d_vars, sizeof(double), D_VARS_LEN, FGUD)==0) return 8;
	if(fread(this->s_vars, sizeof(double), S_VARS_LEN, FGUD)==0) return 9;
	if(fread(this->Obj, sizeof(double), OBJ_VARS_LEN, FGUD)==0) return 10;

	return 0;
}

int Variables::get_gud_name(char *name)
{
	fseek(FGUD, 0, SEEK_SET);

	char *tname = new char[32];
	int noo;

	while(fread(tname, sizeof(char)*32, 1, FGUD)!=0)
	{
		strcpy(&tname[32], "\0");
		if(streqv(tname, name))
		{
			fseek(FGUD, -sizeof(char)*32, SEEK_CUR);

			fread(this->Name, sizeof(char)*32, 1, FGUD);
			fread(&this->num, sizeof(double), 1, FGUD);

			fread(&this->status, sizeof(int), 1, FGUD);

			fread(&this->isDetective, sizeof(double), 1, FGUD);
			fread(&this->isStable, sizeof(double), 1, FGUD);
			fread(this->d_vars, sizeof(double), D_VARS_LEN, FGUD);
			fread(this->s_vars, sizeof(double), S_VARS_LEN, FGUD);
			fread(this->Obj, sizeof(double), OBJ_VARS_LEN, FGUD);

			return 0;
		}

		strcpy(tname, "");
		fseek(FGUD, sizeof(int) + sizeof(double)*(D_VARS_LEN+S_VARS_LEN+OBJ_VARS_LEN+4), SEEK_CUR);
	}

	this->get_obs();
	return 1;
}

int Variables::add2gudlist(int status)
{
	this->status = status;
	this->add2gudlist();
	return 0;
}

int Variables::add2gudlist()
{
	char *t_name;
	int t_obs;
	t_name = new char[32];

	strcpy(t_name, "");

	long pos;

	fseek(FGUD, 0, SEEK_SET);

	while(fread(t_name, sizeof(char)*32, 1, FGUD)!=0)
	{
		pos = ftell(FGUD);
		strcpy(&t_name[32], "\0");
		if(streqv(t_name, this->Name))
		{
			fseek(FGUD, -sizeof(char)*32, SEEK_CUR);
			fwrite(this->Name, sizeof(char)*32, 1, FGUD);
			fwrite(&this->num, sizeof(double), 1, FGUD);
			fwrite(&this->status, sizeof(int), 1, FGUD);
			fwrite(&this->isDetective, sizeof(double), 1, FGUD);
			fwrite(&this->isStable, sizeof(double), 1, FGUD);
			fwrite(this->d_vars, sizeof(double), D_VARS_LEN, FGUD);
			fwrite(this->s_vars, sizeof(double), S_VARS_LEN, FGUD);
			fwrite(this->Obj, sizeof(double), OBJ_VARS_LEN, FGUD);

			return 0;
		}

		strcpy(t_name, "");

		fseek(FGUD, sizeof(int) + sizeof(double)*(D_VARS_LEN+S_VARS_LEN+OBJ_VARS_LEN+4), SEEK_CUR);
	}

	fwrite(this->Name, sizeof(char)*32, 1, FGUD);
	fwrite(&this->num, sizeof(double), 1, FGUD);
	fwrite(&this->status, sizeof(int), 1, FGUD);
	fwrite(&this->isDetective, sizeof(double), 1, FGUD);
	fwrite(&this->isStable, sizeof(double), 1, FGUD);
	fwrite(this->d_vars, sizeof(double), D_VARS_LEN, FGUD);
	fwrite(this->s_vars, sizeof(double), S_VARS_LEN, FGUD);
	fwrite(this->Obj, sizeof(double), OBJ_VARS_LEN, FGUD);

	return 0;
}

int Variables::get_name_stat(char *name)
{
	this->get_gud_name(name);
	return(this->status);
}

void Variables::set(Variables *st)
{
	st->copyTo(this);
}

void Variables::clear_gud()
{
	FGUD = freopen(this->fn_gud, "wb+", FGUD);
}


void Variables::clear_bad(int sup)
{
	int i=0;
	while(!this->get_gud_num(i))
	{
		if(this->status==sup)
		{
			this->status = 0;
			this->add2gudlist();
		}

		i++;
	}
}

int Variables::count_all()
{
	int i = 0;
	while(!this->get_gud_num(i)) i++;
	return i;
}

int Variables::count_stat(int st)
{
	int i = 0;
	int k = 0;
	while(!this->get_gud_num(i))
	{
		if(this->status==st) k++;
		i++;
	}
	return k;
}

int Variables::count_bad()
{
	int i = 0;
	int k = 0;
	while(!this->get_gud_num(i))
	{
		if(this->status) k++;
		i++;
	}
	return k;
}

int Variables::det_num(int isD, int isS)
{
	this->num = pow(this->isDetective, isD)*pow(this->isStable, isS);
	return 0;
}

int Variables::inc_obs_list()
{
	char *t_name;
	int t_num;
	int incn = 1;
	t_name = new char[32];

	strcpy(t_name, "");

	fseek(FOBS, 0, SEEK_SET);

	while((fread(t_name, sizeof(char)*32, 1, FOBS)!=0)&&(fread(&t_num, sizeof(int), 1, FOBS)!=0))
	{
		strcpy(&t_name[32], "\0");
		if(streqv(t_name, this->Name))
		{
			fseek(FOBS, -sizeof(int), SEEK_CUR);
			t_num++;
			fwrite(&t_num, sizeof(int), 1, FOBS);
			return 0;
		}

		strcpy(t_name, "");
	}

	fwrite(this->Name, sizeof(char)*32, 1, FOBS);
	fwrite(&incn, sizeof(int), 1, FOBS);

	return 0;
}

int Variables::get_obs()
{
	char *t_name;
	int t_num;
	t_name = new char[32];

	strcpy(t_name, "");

	fseek(FOBS, 0, SEEK_SET);

	while((fread(t_name, sizeof(char)*32, 1, FOBS)!=0)&&(fread(&t_num, sizeof(int), 1, FOBS)!=0))
	{
		strcpy(&t_name[32], "\0");
		if(streqv(t_name, this->Name))
		{
			this->obs = t_num;
			return 0;
		}

		strcpy(t_name, "");
	}

	this->obs = 0;

	return 0;
}

int DoubleOrbits::det_vars_r1_next()
{
	this->vars->s_vars->r1 += this->vars->s_vars->dr1;
	if(this->vars->s_vars->r>this->vars->s_vars->r1max) return 1;

	return 0;
}

void DoubleOrbits::det_vars_R1()
{
	double P = this->vars->d_vars->Sdist*sqrt(2.0*pow(2.512, SUN_MAGNITUDE-this->vars->d_vars->m)/this->params->Albedo);

	this->vars->s_vars->R1 = P;///(1.0 + pow(this->params->R2R1, 2.0));
}

void DoubleOrbits::det_vars_R2()
{
	this->vars->s_vars->R2 = this->vars->s_vars->R1*this->params->R2R1;
}

void DoubleOrbits::det_vars_bodies()
{
/*
	this->vars->s_vars->A = 2.0*this->vars->s_vars->R1*this->params->A;
	this->vars->s_vars->B = 2.0*this->vars->s_vars->R1*this->params->B;
	this->vars->s_vars->C = 2.0*this->vars->s_vars->R1*this->params->C;
*/
	this->vars->s_vars->B = this->vars->s_vars->A = this->vars->s_vars->R1/pow(1.716, 1/3.0);;
	this->vars->s_vars->C = this->vars->s_vars->B/1.716;

/*
	this->vars->s_vars->a = 2.0*this->vars->s_vars->R2*this->params->a;
	this->vars->s_vars->b = 2.0*this->vars->s_vars->R2*this->params->b;
	this->vars->s_vars->c = 2.0*this->vars->s_vars->R2*this->params->c;
*/
	this->vars->s_vars->b = this->vars->s_vars->a = this->vars->s_vars->R2/pow(1.716, 1/3.0);;
	this->vars->s_vars->c = this->vars->s_vars->b/1.716;

	this->vars->s_vars->M1 = pow(AUSM, 3.0)*VEll(this->vars->s_vars->A, this->vars->s_vars->B, this->vars->s_vars->C)*this->params->sigma;
	this->vars->s_vars->M2 = pow(AUSM, 3.0)*VEll(this->vars->s_vars->a, this->vars->s_vars->b, this->vars->s_vars->c)*this->params->sigma;
}

void DoubleOrbits::det_vars_system()
{
	//WARN!!!
	this->vars->s_vars->r2 = this->vars->s_vars->r1*sqrt(this->vars->s_vars->M2/this->vars->s_vars->M1);

	this->vars->s_vars->r = this->vars->s_vars->r1 + this->vars->s_vars->r2;

	this->vars->s_vars->T = 2.0*PI*pow(this->vars->s_vars->r1, 1.5)/(ka*sqrt((this->vars->s_vars->M2 + this->vars->s_vars->M1)/SUN_MASS_GRAM)*24.0);

	this->vars->s_vars->norm = this->vars->s_vars->r1*this->vars->s_vars->r1*(this->vars->s_vars->M2 + this->vars->s_vars->M1);
}

void DoubleOrbits::det_vars_normalized()
{
	this->vars->s_vars->A /= this->vars->s_vars->r1;
	this->vars->s_vars->B /= this->vars->s_vars->r1;
	this->vars->s_vars->C /= this->vars->s_vars->r1;
	this->vars->s_vars->a /= this->vars->s_vars->r1;
	this->vars->s_vars->b /= this->vars->s_vars->r1;
	this->vars->s_vars->c /= this->vars->s_vars->r1;

	this->vars->s_vars->r /= this->vars->s_vars->r1;

	this->vars->s_vars->mju1 = (this->vars->s_vars->A*this->vars->s_vars->B*this->vars->s_vars->C)/(this->vars->s_vars->A*this->vars->s_vars->B*this->vars->s_vars->C + this->vars->s_vars->a*this->vars->s_vars->b*this->vars->s_vars->c);
	this->vars->s_vars->mju2 = (this->vars->s_vars->a*this->vars->s_vars->b*this->vars->s_vars->c)/(this->vars->s_vars->A*this->vars->s_vars->B*this->vars->s_vars->C + this->vars->s_vars->a*this->vars->s_vars->b*this->vars->s_vars->c);
}

void DoubleOrbits::det_vars_others()
{
//E'
	this->vars->s_vars->E = -this->vars->s_vars->mju1*this->vars->s_vars->mju2*2.0*PI*this->vars->s_vars->r*this->vars->s_vars->r/(this->vars->s_vars->T*this->vars->s_vars->T);
	this->vars->s_vars->E += 2.0*PI*PI*this->vars->s_vars->mju1*this->vars->s_vars->mju1*(this->vars->s_vars->A*this->vars->s_vars->A + this->vars->s_vars->B*this->vars->s_vars->B)/(5.0*this->params->Tp*this->params->Tp);
	this->vars->s_vars->E += 2.0*PI*PI*this->vars->s_vars->mju2*this->vars->s_vars->mju2*(this->vars->s_vars->a*this->vars->s_vars->a + this->vars->s_vars->b*this->vars->s_vars->b)/(5.0*this->params->To*this->params->To);
	this->vars->s_vars->E /= 3600.000000000000*3600.00000000000;

//K'
	this->vars->s_vars->K = 2.0*PI*this->vars->s_vars->mju1*this->vars->s_vars->mju2*this->vars->s_vars->r*this->vars->s_vars->r/this->vars->s_vars->T;
	this->vars->s_vars->K += 2.0*PI*this->vars->s_vars->mju1*this->vars->s_vars->mju1*(this->vars->s_vars->A*this->vars->s_vars->A + this->vars->s_vars->B*this->vars->s_vars->B)/(5.0*this->params->Tp);
	this->vars->s_vars->K += 2.0*PI*this->vars->s_vars->mju2*this->vars->s_vars->mju2*(this->vars->s_vars->a*this->vars->s_vars->a + this->vars->s_vars->b*this->vars->s_vars->b)/(5.0*this->params->To);
	this->vars->s_vars->K /= 3600.00000000000;

//Ip'
	this->vars->s_vars->Ip = this->vars->s_vars->mju1*this->vars->s_vars->mju2*this->vars->s_vars->r*this->vars->s_vars->r;
	this->vars->s_vars->Ip += this->vars->s_vars->mju1*this->vars->s_vars->mju1*(this->vars->s_vars->A*this->vars->s_vars->A + this->vars->s_vars->B*this->vars->s_vars->B + this->vars->s_vars->C*this->vars->s_vars->C)/5.0;
	this->vars->s_vars->Ip += this->vars->s_vars->mju2*this->vars->s_vars->mju2*(this->vars->s_vars->a*this->vars->s_vars->a + this->vars->s_vars->b*this->vars->s_vars->b + this->vars->s_vars->c*this->vars->s_vars->c)/5.0;

//U'
	this->vars->s_vars->U = this->vars->s_vars->mju1*this->vars->s_vars->mju2*4.0*PI*PI*this->vars->s_vars->r*this->vars->s_vars->r/(this->vars->s_vars->T*this->vars->s_vars->T);
	this->vars->s_vars->U /= 3600.000000000*3600.0000000000000;
}

void DoubleOrbits::det_vars_phase()
{
	this->vars->d_vars->elongation = detElong(this->objorbs->params->Earth->x, this->objorbs->params->Earth->y, this->objorbs->params->Earth->z, this->vars->Obj->x, this->vars->Obj->y, this->vars->Obj->z);
}

void DoubleOrbits::det_vars_m()
{
	this->params->c1 = 0.0;
	this->vars->d_vars->m = det_m(atof(this->objorbs->curent->line->H), this->vars->d_vars->Sdist, this->vars->d_vars->Edist, this->params->c1, detPhase(this->objorbs->params->Earth->x, this->objorbs->params->Earth->y, this->objorbs->params->Earth->z, this->vars->Obj->x, this->vars->Obj->y, this->vars->Obj->z));
}

void DoubleOrbits::det_vars_Edist()
{
	this->vars->d_vars->Edist = sqrt((this->objorbs->params->Earth->x - this->vars->Obj->x)*(this->objorbs->params->Earth->x - this->vars->Obj->x) + (this->objorbs->params->Earth->y - this->vars->Obj->y)*(this->objorbs->params->Earth->y - this->vars->Obj->y) + (this->objorbs->params->Earth->z - this->vars->Obj->z)*(this->objorbs->params->Earth->z - this->vars->Obj->z));
}

void DoubleOrbits::det_vars_Sdist()
{
	this->vars->d_vars->Sdist = sqrt(this->vars->Obj->x*this->vars->Obj->x + this->vars->Obj->y*this->vars->Obj->y + this->vars->Obj->z*this->vars->Obj->z);
}

void DoubleOrbits::det_vars_Qmax()
{
	double r;

	r = this->objorbs->curent->elem->q*sqrt(this->vars->s_vars->M1/SUN_MASS_GRAM);

	this->vars->d_vars->Qmax = atan2(r, this->vars->d_vars->Edist);

}

int DoubleOrbits::det_vars()
{
	this->det_Obj();
	this->det_vars_Edist();
	this->det_vars_Sdist();
	this->det_vars_phase();
	this->det_vars_m();


 	this->det_vars_R1();
	this->det_vars_R2();
	this->det_vars_r1min();
//	this->det_vars_params_r1max();
	this->det_vars_bodies();
	this->det_vars_Qmax();
	this->det_vars_r1max();
	this->det_vars_dr1();

//	this->det_vars_R1();
//	this->det_vars_Q();

//	this->isVisual();
//	this->isDetective();
	this->isStable();
	this->isDetective();

	strcpy(this->vars->Name, this->objorbs->curent->line->name);

	return 0;
}

int DoubleOrbits::det_Obj()
{
	this->objorbs->curent->detRecEkv(&this->vars->Obj->x, &this->vars->Obj->y, &this->vars->Obj->z, this->objorbs->params->TDT);

	return 0;
}

int DoubleOrbits::isDetective()
{
	this->det_vars_Q();

	double phaseMO = (this->params->PhaseM - this->vars->d_vars->elongation)/this->params->PhaseM;
	if(phaseMO<0.0) phaseMO = 0.0;

	double mMO = -(this->vars->d_vars->m - this->params->Mm)/sqrt(this->params->Mm);
	if(mMO<0.0) mMO = 0.0;
	if(mMO>1.0) mMO = 1.0;

	this->vars->isDetective = phaseMO*mMO;

	return 0;
}

int DoubleOrbits::isStable()
{
	this->vars->isStable = 0;
	if(this->vars->s_vars->r1min>this->vars->s_vars->r1max)return 0;
	double ts = 0.0;

	StableVars *s_vars_tmp;
	s_vars_tmp = new StableVars;

	double Qtmp;
	double Emin;

	
	do
	{
		this->det_vars_system();
		this->det_vars_Q();
		this->det_vars_normalized();
		this->det_vars_others();

//		Emin = this->vars->s_vars->K*this->vars->s_vars->K/2.0/this->vars->s_vars->Ip - this->vars->s_vars->U;

		ts = ((this->vars->s_vars->r1 - this->vars->s_vars->r1min)/fabs(this->vars->s_vars->r1max - this->vars->s_vars->r1min))*(this->vars->s_vars->E<0.0);//*((fabs(Emin)/2.0)/(fabs(Emin)/2.0 - fabs(this->vars->s_vars->E) +1.0));
//		ts = (this->vars->s_vars->E<0.0)*((fabs(Emin)/2.0)/(fabs(Emin)/2.0 - fabs(this->vars->s_vars->E) +1.0));

		if(ts<0.0) ts = 0.0;

		this->det_vars_bodies();
		this->vars->s_vars->r *= this->vars->s_vars->r1;


		if(this->vars->isStable<ts)
		{
			this->vars->isStable = ts;

			Qtmp = this->vars->d_vars->Q;

			s_vars_tmp->R1 = this->vars->s_vars->R1;
			s_vars_tmp->R2 = this->vars->s_vars->R2;
			s_vars_tmp->r = this->vars->s_vars->r;
			s_vars_tmp->r1 = this->vars->s_vars->r1;
			s_vars_tmp->r2 = this->vars->s_vars->r2;
			s_vars_tmp->r1min = this->vars->s_vars->r1min;
			s_vars_tmp->r1max = this->vars->s_vars->r1max;
			s_vars_tmp->dr1 = this->vars->s_vars->dr1;
			s_vars_tmp->A = this->vars->s_vars->A;
			s_vars_tmp->B = this->vars->s_vars->B;
			s_vars_tmp->C = this->vars->s_vars->C;
			s_vars_tmp->a = this->vars->s_vars->a;
			s_vars_tmp->b = this->vars->s_vars->b;
			s_vars_tmp->c = this->vars->s_vars->c;
			s_vars_tmp->M1 = this->vars->s_vars->M1;
			s_vars_tmp->M2 = this->vars->s_vars->M2;
			s_vars_tmp->T = this->vars->s_vars->T;
			s_vars_tmp->norm = this->vars->s_vars->norm;
			s_vars_tmp->mju1 = this->vars->s_vars->mju1;
			s_vars_tmp->mju2 = this->vars->s_vars->mju2;
			s_vars_tmp->E = this->vars->s_vars->E;
			s_vars_tmp->K = this->vars->s_vars->K;
			s_vars_tmp->Ip = this->vars->s_vars->Ip;
			s_vars_tmp->U = this->vars->s_vars->U;
			
		}

	}while(!this->det_vars_r1_next());

	this->vars->d_vars->Q = Qtmp;

	this->vars->s_vars->R1 = s_vars_tmp->R1;
	this->vars->s_vars->R2 = s_vars_tmp->R2;
	this->vars->s_vars->r = s_vars_tmp->r;
	this->vars->s_vars->r1 = s_vars_tmp->r1;
	this->vars->s_vars->r2 = s_vars_tmp->r2;
	this->vars->s_vars->r1min = s_vars_tmp->r1min;
	this->vars->s_vars->r1max = s_vars_tmp->r1max;
	this->vars->s_vars->dr1 = s_vars_tmp->dr1;
	this->vars->s_vars->A = s_vars_tmp->A;
	this->vars->s_vars->B = s_vars_tmp->B;
	this->vars->s_vars->C = s_vars_tmp->C;
	this->vars->s_vars->a = s_vars_tmp->a;
	this->vars->s_vars->b = s_vars_tmp->b;
	this->vars->s_vars->c = s_vars_tmp->c;
	this->vars->s_vars->M1 = s_vars_tmp->M1;
	this->vars->s_vars->M2 = s_vars_tmp->M2;
	this->vars->s_vars->T = s_vars_tmp->T;
	this->vars->s_vars->norm = s_vars_tmp->norm;
	this->vars->s_vars->mju1 = s_vars_tmp->mju1;
	this->vars->s_vars->mju2 = s_vars_tmp->mju2;
	this->vars->s_vars->E = s_vars_tmp->E;
	this->vars->s_vars->K = s_vars_tmp->K;
	this->vars->s_vars->Ip = s_vars_tmp->Ip;
	this->vars->s_vars->U = s_vars_tmp->U;

	delete(s_vars_tmp);

	return 0;
}

int DoubleOrbits::save_vars()
{
	if(this->vars->num>0.0)
	{
		this->vars->add2gudlist(0);
	}
	return 0;
}

int DoubleOrbits::count(double Jday)
{
	if(this->init_Time(Jday))
	{
		strcat(this->err_str, "->count=1\n");
		return 1;
	}

	int k = 29284;

	for(int i=0; i<this->objorbs->norbs; i++)
	{

		this->objorbs->getorb(i);

		if(k==i)
		{
			k==1;
		}
		this->det_vars();
		this->vars->det_num(this->params->flags->isDetective, this->params->flags->isStable);
		this->save_vars();
	}

	this->nres = this->vars->count_all();

	return 0;
}

int DoubleOrbits::count_range(double Jday0, double Jday1, double step)
{
	if(Jday0>Jday1) return 1;
	if(step<=0.0) return 2;

	double Jday = Jday0;

	do
	{
		count(Jday);

		Jday+=step;

	}while(Jday<=Jday1);

	return 0;
}

int DoubleOrbits::print_vars()
{
	FILE *FOUT, *FRESVL;
//	Variables *temp;
//	temp = new Variables;

	char *s;
	int y, m, sgn;
	double d;
	s = new char[1];

	FOUT = fopen(this->fn_log, "w");

	fprintf(FOUT, "TOTAL:%d\n", this->nres);

	fprintf(FOUT, "Detective %d Stable %d\n\n", this->params->flags->isDetective, this->params->flags->isStable);

	fprintf(FOUT, "Detective Params\n\n");

	fprintf(FOUT, "Elong max:\t\t%f grad\n", rad2grad(this->params->PhaseM));
	fprintf(FOUT, "m max:\t\t\t%f magnitude\n", this->params->Mm);
	fprintf(FOUT, "Earth dist max:\t\t%f AU\n", this->params->EdistMax);
	fprintf(FOUT, "Q min:\t\t\t%f mas\n", rad2mas(this->params->Qmin));

	fprintf(FOUT, "\n////////////////////////////////////\n\n");

	orbit *torb;
	torb = new orbit;

	int k = 0;

	FILE *FTMP = fopen(this->fn_res, "w+");
	fclose(FTMP);

	FRESVL = fopen(this->fn_varList, "w+");

	int tasknum = 1;
	char *stmp, *stmp1;

	stmp = new char[128];
	stmp1 = new char[128];

	sprintf(stmp, "%d", tasknum);

	index(stmp1, stmp, "0", 5);


	for(int i=0; i<this->nres; i++)
	{
		this->vars->get_gud_num(i);

		fprintf(FRESVL, "%s%s%10e\n", stmp1, this->vars->Name, this->vars->num);

		fprintf(FOUT, "%e\t%s\n", this->vars->num, this->vars->Name);

		fprintf(FOUT, "\nDS: %e %e\n\n", this->vars->isDetective, this->vars->isStable);

		fprintf(FOUT, "\nDetective vars\n\n");

		fprintf(FOUT, "Elong:\t\t%f grad\n", rad2grad(this->vars->d_vars->elongation));
		fprintf(FOUT, "m:\t\t%f magnitude\n", this->vars->d_vars->m);
		fprintf(FOUT, "Earth dist:\t%f AU\n", this->vars->d_vars->Edist);
		fprintf(FOUT, "Sun dist:\t%f AU\n", this->vars->d_vars->Sdist);
		fprintf(FOUT, "Q: %f mas\t<\tQmax: %f\n", rad2mas(this->vars->d_vars->Q), rad2mas(this->vars->d_vars->Qmax));

		fprintf(FOUT, "\n////////////////////////////////////\n\n");

		this->objorbs->getorb_name(this->vars->Name);
//		this->orbroot->orb->copyTo(torb);

		this->objorbs->curent->writeCat(this->fn_res, 1);
	}

	fclose(FOUT);
	fclose(FRESVL);

	return 0;
}

void DoubleOrbits::det_vars_Q()
{
	this->vars->d_vars->Q = atan2(this->vars->s_vars->r1, this->vars->d_vars->Edist);
//	this->vars->d_vars->Q = atan2(this->params->R, this->vars->d_vars->Edist);
}

void DoubleOrbits::det_vars_r1min()
{
//	this->vars->s_vars->r1 = this->vars->s_vars->r1min = det_r(this->params->Qmin, this->vars->d_vars->Edist);
	if(!this->vars->isDetective) this->vars->s_vars->r1 = this->vars->s_vars->r1min = this->vars->s_vars->R1;
	else this->vars->s_vars->r1 = this->vars->s_vars->r1min = det_r(this->params->Qmin, this->vars->d_vars->Edist);
}

void DoubleOrbits::det_vars_r1max()
{
	this->vars->s_vars->r1max = det_r(this->vars->d_vars->Qmax, this->vars->d_vars->Edist);
}

void DoubleOrbits::det_vars_dr1()
{
	this->vars->s_vars->dr1 = fabs(this->vars->s_vars->r1max - this->vars->s_vars->r1min)/sqrt((float)this->objorbs->norbs);
}

int DoubleOrbits::initCFG(char *fn_cfg)
{
	if(fn_cfg==NULL) return 1;
	FCFG = fopen(fn_cfg, "rb");
	if(FCFG==NULL)
	{
		sprintf(this->err_str, "\nERROR:initCFG: Unable to open %s\n", fn_cfg);
		return 2;
	}

//	if(this->init_CPV()) return 5;
//	if(this->init_params()) return 3;
//	if(this->init_params()) return 4;


//	fread

	this->params = new DoubleOrbitsParams;
	this->params->flags = new ProgFlags;

	int len;
	double val;//, Jday;

//	fread(&val, sizeof(double), 1, FCFG);
//	Jday = val;

	char *fn_cat;
	fread(&len, sizeof(int), 1, FCFG);
	fn_cat = new char[len];
	fread(fn_cat, sizeof(char)*len, 1, FCFG);
	strcpy(&fn_cat[len], "\0");

	char *fn_obs;
	fread(&len, sizeof(int), 1, FCFG);
	fn_obs = new char[len];
	fread(fn_obs, sizeof(char)*len, 1, FCFG);
	strcpy(&fn_obs[len], "\0");

	fread(&len, sizeof(int), 1, FCFG);
	this->fn_res = new char[len];
	fread(this->fn_res, sizeof(char)*len, 1, FCFG);
	strcpy(&this->fn_res[len], "\0");

	fread(&len, sizeof(int), 1, FCFG);
	this->fn_varList = new char[len];
	fread(this->fn_varList, sizeof(char)*len, 1, FCFG);
 	strcpy(&this->fn_varList[len], "\0");

	char *fn_earth;
	fread(&len, sizeof(int), 1, FCFG);
	fn_earth = new char[len];
	fread(fn_earth, sizeof(char)*len, 1, FCFG);
	strcpy(&fn_earth[len], "\0");

	fread(&len, sizeof(int), 1, FCFG);
	this->fn_log = new char[len];
	fread(this->fn_log, sizeof(char)*len, 1, FCFG);
	strcpy(&this->fn_log[len], "\0");

	this->objorbs = new orbits;
	this->objorbs->init(fn_cat, fn_obs, fn_earth);

	int who_is_chk;

	fread(&who_is_chk, sizeof(int), 1, FCFG);

	if(!who_is_chk)
	{
		int obs_num;
		fread(&obs_num, sizeof(int), 1, FCFG);
		if(this->objorbs->set_observatory(obs_num)) return 7;
	}
	else
	{
		char *obs_name;

		fread(&len, sizeof(int), 1, FCFG);
		obs_name = new char[len];
		fread(obs_name, sizeof(char)*len, 1, FCFG);
		strcpy(&obs_name[len], "\0");
		if(this->objorbs->set_observatory(obs_name)) return 9;
	}

	int isV, isD, isS;

	fread(&isV, sizeof(int), 1, FCFG);
	fread(&isD, sizeof(int), 1, FCFG);
	fread(&isS, sizeof(int), 1, FCFG);

	this->params->flags->isDetective = isD;
	this->params->flags->isStable = isS;

//	this->set_OP_flags(isV, isD, isS);

//objorbs->params
/*
	if(this->objorbs->params==NULL) this->objorbs->params = new VisualParam;

	fread(&this->objorbs->params->ishmin, sizeof(int), 1, FCFG);
	fread(&val, sizeof(double), 1, FCFG);
	this->objorbs->params->hmin = grad2rad(val);

	fread(&this->objorbs->params->isFrame, sizeof(int), 1, FCFG);
	fread(&val, sizeof(double), 1, FCFG);
	this->objorbs->params->RAmax = grad2rad(val);
	fread(&val, sizeof(double), 1, FCFG);
	this->objorbs->params->RAmin = grad2rad(val);
	fread(&val, sizeof(double), 1, FCFG);
	this->objorbs->params->DECmax = grad2rad(val);
	fread(&val, sizeof(double), 1, FCFG);
	this->objorbs->params->DECmin = grad2rad(val);
*/
//params
//	if(this->params==NULL) this->params = new DetectiveParam;

	fread(&this->params->Albedo, sizeof(double), 1, FCFG);
	fread(&val, sizeof(double), 1, FCFG);
	this->params->dQ = mas2rad(val);
	fread(&this->params->EdistMax, sizeof(double), 1, FCFG);
	fread(&val, sizeof(double), 1, FCFG);
	this->params->PhaseM = grad2rad(val);
	fread(&this->params->Mm, sizeof(double), 1, FCFG);
	fread(&val, sizeof(double), 1, FCFG);
	this->params->Qmin = mas2rad(val);

//params
//	if(this->params==NULL) this->params = new StableParam;

	fread(&this->params->A, sizeof(double), 1, FCFG);
	fread(&this->params->a, sizeof(double), 1, FCFG);
	fread(&this->params->B, sizeof(double), 1, FCFG);
	fread(&this->params->b, sizeof(double), 1, FCFG);
	fread(&this->params->C, sizeof(double), 1, FCFG);
	fread(&this->params->c, sizeof(double), 1, FCFG);
	fread(&this->params->R2R1, sizeof(double), 1, FCFG);
	fread(&this->params->sigma, sizeof(double), 1, FCFG);
	fread(&val, sizeof(double), 1, FCFG);
	this->params->To = val*3600.0;
	fread(&val, sizeof(double), 1, FCFG);
	this->params->Tp = val*3600.0;

//	if(initFileCat(this->fn_cat)) return 6;

//	if(init_CPV(tUTC)) return 6;
//	if(init_VP_onum(this->fn_obs, onum)) return 7;
//	UTC2s(this->CP->UTC, this->objorbs->params->lam, &this->objorbs->params->s);

//	this->init_Time(Jday);

	fclose(FCFG);
	FCFG = NULL;

	char *rname;
	rname = new char[128];
	int rnlen;

	strcpy(rname, this->fn_varList);
	rnlen = strlen(this->fn_varList);
	strcpy(&rname[rnlen-3], "\0");
	strcat(rname, ".tmp");

	this->vars = new Variables(rname);

	return 0;
}

int DoubleOrbits::init_Time(double tUTC)
{
	if(this->objorbs->retime(tUTC))
	{
		strcpy(this->err_str, this->objorbs->err_str);
		strcat(this->err_str, " :init_Time:");
		return 1;
	}

	return 0;
}
/*
int DoubleOrbits::get_next_obj(char *pstr, double cTime)
{
	int i;

	char *slo;
	slo = new char[10];
	i=0;
	int sfilters = strlen(pstr)/5;
	int *filters;
	filters = new int[sfilters];
	while((!strnsep(pstr, i, "|", slo))&&(i<sfilters))
	{
		if(streqv(slo, "hmax")) filters[i] = 0;
		if(streqv(slo, "magn")) filters[i] = 1;
		if(streqv(slo, "hmmax")) filters[i] = 2;
		i++;
	}

	int nobj = this->nres;
	double hmax;
	int imax;

	while(!this->vars->get_best_name())
	{
		this->init_CPV_Time(cTime);
		this->init_VP_Time();
		this->objorbs->getorb_name(this->vars->Name);
		this->orbroot->orb->copyTo(this->CP->OrbT);
		this->det_vars();
		this->detRDnumGC(&this->vars->v_vars->RA, &this->vars->v_vars->DEC, cTime);
		this->detAhnumGC(&this->vars->v_vars->A, &this->vars->v_vars->h, cTime);

		if(this->vars->v_vars->h<this->objorbs->params->hmin)
		{
//			this->vars->status = 1;
			this->vars->add2gudlist(1);
		}
		else return(this->vars->obs);
	}

	return -1;
}
*/