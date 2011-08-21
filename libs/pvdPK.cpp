#include "pvdPK.h"

#define PI 3.141592653589
#define ka 0.017202098955
#define EPS 1.0e-8
#define DEPTHSCAN 40
#define EKV 0.4093197552027299

pvdP::pvdP()
{
	this->sun = NULL;
	this->vpos = NULL;
	this->comvar = NULL;
	this->res = NULL;
	this->slog = NULL;

	this->klog = 0;
	this->fini = 0;
}

pvdP::pvdP(int log)
{
	this->sun = NULL;
	this->vpos = NULL;
	this->comvar = NULL;
	this->res = NULL;

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
*/
}

pvdP::pvdP(char *fnrec, char *fnpvd)
{
	this->sun = NULL;
	this->vpos = NULL;
	this->comvar = NULL;
	this->res = NULL;
	this->slog = NULL;

	this->klog = 0;
	this->fini = 0;
	this->logging((char*)NULL);

	this->initFiles(fnrec, fnpvd, NULL);
}

pvdP::pvdP(char *fnrec, char *fnpvd, char *fnini, int log)
{
	this->sun = NULL;
	this->vpos = NULL;
	this->comvar = NULL;
	this->res = NULL;
	this->slog = NULL;

	this->klog = log;
	this->logging(fnpvd);

	this->initFiles(fnrec, fnpvd, fnini);
}

int pvdP::initFiles(char *fnrec, char *fnpvd, char *fnini)
{
	this->sun = new rec;
	this->vpos = new pvdK(this->klog);

	this->logging(fnpvd);

	if(this->sun->initFile(fnrec)) return 1;
	if(this->vpos->initMPC(fnpvd)) return 1;
	if(fnini!=NULL)
	{
		this->fini = 1;
		this->FINI = fopen(fnini, "r");
//		this->initC();
	}
	else this->fini = 0;

	return 0;
}


pvdP::~pvdP()
{
	delete(this->sun);
	delete(this->vpos);
	delete(this->res);
	delete(this->slog);
}

int pvdP::logging(char *basename)
{
	if(this->klog)
	{
		this->slog = new logstuff;

		this->slog->suff = new char[25];
		int len = strlen(basename);
		strcpy(this->slog->suff, "");
		strncpy(this->slog->suff, basename, len-4);
		strcpy(&this->slog->suff[len-4], "\0");

//		char *flogn;
//		flogn = new char[25];
//		strcpy(flogn, "");
		len = strlen(this->slog->suff);

//		strncpy(flogn, this->slog->suff, len);
//		strcpy(&flogn[len], "\0");
		strcat(this->slog->suff, "_pP.log");
		strcpy(&this->slog->suff[len+7], "\0");

		this->slog->FLOG = fopen(this->slog->suff, "w");
	}

	return 0;
}


int pvdP::compute()
{
	this->initVar();
	this->provTDM();

//	this->findSqr();
	if(this->appFerst1())
	{
		this->aclose();
		return 1;
	}
//	this->appSecond();

	if(this->computeOrb1()) return 2;

	this->proofSM();
	
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
	pv = new char[1000];

	double r0, r, ret;
	int iter = 0;

//	if(this->klog) fprintf(this->slog->FLOG, "\nAPP_FERST:\n\n");
/*
	r =  18.997;
	if(iterD(r, &iter)) return 1;
*/	
/*
	r =  3.707;
	if(iterDc(r, &iter)) return 1;
*/
/*
	r = 5.51190713;
	if(iterD(r, &iter)) return 1;
*/
//	r = 3.0;
//	iterD(r, &iter);
	
//	r = 5.949;
//	iterRc(r, &iter);
/*	printf("\niterRc\n");
	r = 2.1;
	do
	{
		printf("r= %f	r0= %f	%d iter\n", r, this->comvar->chv[this->comvar->Ckey].rnorm, iter);
		r0 = r;
		r +=0.1;
		iter = 0;
//		iterR(r0, &iter);
//		findRt();
//		proofR();
	}while((r<20.0)&&iterD(r0, &iter));
	printf("\nfinal: r0= %f	%d iter\n", this->comvar->chv[this->comvar->Ckey].d, iter);
*/
//
	ret=1;
	switch(this->comvar->Ckey)
	{
	case 0:
		printf("\niterR\n\n");
		if(this->klog) fprintf(this->slog->FLOG, "\niterR\n\n");
		
		r = this->comvar->inird;
		this->comvar->chv[this->comvar->Ckey].rnorm = 0.0;
		r0 = r;

		printf("inird\t%f\n\n", r0);
		if(this->klog) fprintf(this->slog->FLOG, "inird\t%f\n\n", r0);

		while((r<DEPTHSCAN)&&ret)
		{
			ret = iterR(r0, &iter);
			printf("r= %f	r0= %f	%d iter\n", r, this->comvar->chv[this->comvar->Ckey].rnorm, iter);
			if(this->klog) fprintf(this->slog->FLOG, "r= %f	r0= %f	%d iter\n", r, this->comvar->chv[this->comvar->Ckey].rnorm, iter);
			r0 = r;
			r +=0.1;
			iter = 0;
		}
		printf("\nfinal: r0= %f	%d iter\n", this->comvar->chv[this->comvar->Ckey].rnorm, this->comvar->chv[this->comvar->Ckey].iter);
		if(this->klog) fprintf(this->slog->FLOG, "\nfinal: r0= %f	%d iter\n", this->comvar->chv[this->comvar->Ckey].rnorm, this->comvar->chv[this->comvar->Ckey].iter);
		break;
	case 1:
		printf("\niterRc\n\n");
		if(this->klog) fprintf(this->slog->FLOG, "\niterRc\n\n");

		r = this->comvar->inird;
		this->comvar->chv[this->comvar->Ckey].rnorm = 0.0;
		r0 = r;

		printf("inird\t%f\n\n", r0);
		if(this->klog) fprintf(this->slog->FLOG, "inird\t%f\n\n", r0);

		while((r<DEPTHSCAN)&&ret)
		{
			ret = iterRc(r0, &iter);
			printf("r= %f	r0= %f	%d iter\n", r, this->comvar->chv[this->comvar->Ckey].rnorm, iter);
			if(this->klog) fprintf(this->slog->FLOG, "r= %f	r0= %f	%d iter\n", r, this->comvar->chv[this->comvar->Ckey].rnorm, iter);
			r0 = r;
			r +=0.1;
			iter = 0;
		}
		printf("\nfinal: r0= %f	%d iter\n", this->comvar->chv[this->comvar->Ckey].rnorm, this->comvar->chv[this->comvar->Ckey].iter);
		if(this->klog) fprintf(this->slog->FLOG, "\nfinal: r0= %f	%d iter\n", this->comvar->chv[this->comvar->Ckey].rnorm, this->comvar->chv[this->comvar->Ckey].iter);
		break;
	case 2:
		printf("\niterD\n\n");
		if(this->klog) fprintf(this->slog->FLOG, "\niterD\n\n");

		r = this->comvar->inird;
		this->comvar->chv[this->comvar->Ckey].d = 0.0;
		r0 = r;

		printf("inird\t%f\n\n", r0);
		if(this->klog) fprintf(this->slog->FLOG, "inird\t%f\n\n", r0);

		while((r<DEPTHSCAN)&&ret)
		{
			ret = iterD(r0, &iter);
			printf("r= %f	d0= %f	%d iter\n", r, this->comvar->chv[this->comvar->Ckey].d, iter);
			if(this->klog) fprintf(this->slog->FLOG, "r= %f	r0= %f	%d iter\n", r, this->comvar->chv[this->comvar->Ckey].rnorm, iter);
			r0 = r;
			r +=0.1;
			iter = 0;
		}
		printf("\nfinal: d0= %f	%d iter\n", this->comvar->chv[this->comvar->Ckey].d, this->comvar->chv[this->comvar->Ckey].iter);
		if(this->klog) fprintf(this->slog->FLOG, "\nfinal: r0= %f	%d iter\n", this->comvar->chv[this->comvar->Ckey].rnorm, this->comvar->chv[this->comvar->Ckey].iter);
		break;
	case 3:
		printf("\niterDc\n\n");
		if(this->klog) fprintf(this->slog->FLOG, "\niterDc\n\n");
		r = this->comvar->inird;
		this->comvar->chv[this->comvar->Ckey].d = 0.0;
		r0 = r;

		printf("inird\t%f\n\n", r0);
		if(this->klog) fprintf(this->slog->FLOG, "inird\t%f\n\n", r0);

		while((r<DEPTHSCAN)&&ret)
		{
			ret = iterDc(r0, &iter);
			printf("r= %f	d0= %f	%d iter\n", r, this->comvar->chv[this->comvar->Ckey].d, iter);
			if(this->klog) fprintf(this->slog->FLOG, "r= %f	r0= %f	%d iter\n", r, this->comvar->chv[this->comvar->Ckey].d, iter);
			r0 = r;
			r +=0.1;
			iter = 0;
		}
		printf("\nfinal: d0= %f	%d iter\n", this->comvar->chv[this->comvar->Ckey].d, this->comvar->chv[this->comvar->Ckey].iter);
		if(this->klog) fprintf(this->slog->FLOG, "\nfinal: r0= %f	%d iter\n", this->comvar->chv[this->comvar->Ckey].d, this->comvar->chv[this->comvar->Ckey].iter);
		break;
	case 4:
		printf("\niterDcD\n\n");
		if(this->klog) fprintf(this->slog->FLOG, "\niterDcD\n\n");
		
		this->comvar->chv[this->comvar->Ckey].d = 0.0;
		
		ret = iterDcD(&iter);
		
		printf("\nfinal: d0= %f	%d iter\n", this->comvar->chv[this->comvar->Ckey].d, this->comvar->chv[this->comvar->Ckey].iter);
		if(this->klog) fprintf(this->slog->FLOG, "\nfinal: r0= %f	%d iter\n", this->comvar->chv[this->comvar->Ckey].rnorm, this->comvar->chv[this->comvar->Ckey].iter);
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

	if(this->klog)
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

	Recl->RotX(-EKV);
	Rtecl->RotX(-EKV);

	if(this->klog)
	{
		Recl->print(pv);
		fprintf(this->slog->FLOG, "\n\nRecl:\t%s\n", pv);
		Rtecl->print(pv);
		fprintf(this->slog->FLOG, "Rtecl:\t%s\n\n", pv);
	}

	this->proofPVD();


	return 0;
}

int pvdP::proofSM()
{
	double x, y, z, t, dd;
	double oc;
	double P1, P2, P3, aV, dV;
	int i;
	double day;
	int year, mouth, sgn;
	char *ssgn;
	ssgn = new char[1];


	printf("\n\nO-C\n");
	if(this->klog) fprintf(this->slog->FLOG, "\n\nO-C\n");


	t = this->comvar->t0;
	this->res->detRecEkv(&x, &y, &z, t);

	P1 = x - this->comvar->g0->get(0);
	P2 = y - this->comvar->g0->get(1);
	P3 = z - this->comvar->g0->get(2);

	rdsys(&aV, &dV, P1, P2, P3);

	this->printOC(aV, dV, this->comvar->r0, this->comvar->d0, t);
//
	t = this->comvar->t;
	this->res->detRecEkv(&x, &y, &z, t);

	P1 = x - this->comvar->g->get(0);
	P2 = y - this->comvar->g->get(1);
	P3 = z - this->comvar->g->get(2);

	rdsys(&aV, &dV, P1, P2, P3);

	this->printOC(aV, dV, this->comvar->r, this->comvar->d, t);
//
	t = this->comvar->t1;
	this->res->detRecEkv(&x, &y, &z, t);

	P1 = x - this->comvar->g1->get(0);
	P2 = y - this->comvar->g1->get(1);
	P3 = z - this->comvar->g1->get(2);

	rdsys(&aV, &dV, P1, P2, P3);

	this->printOC(aV, dV, this->comvar->r1, this->comvar->d1, t);

	return 0;
}

int pvdP::appSecond()
{
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

	this->provRGD();

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


	if(fabs(r-rr)>EPS) ret = iterR((rr+r)/2.0, niter);
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


	if(fabs(r-rr)>EPS) ret = iterRc((rr+r)/2.0, niter);
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

int pvdP::iterDcD(int *niter)
{
	int sekey, ret;
	double d, d1, d2, dt, dd, r;
	double A, B, C;
	double S;

	ret = 0;

	S = this->comvar->gttsM;
	r = pow(-ka*ka*this->comvar->gsM/S,1.0/3.0);

	A = 1.0;
	B = 2.0*this->comvar->gsD;
	C = this->comvar->g2 - r*r;
	sekey = sqEqv(A, B, C, &d1, &d2);

	d = d1;
	if(sekey==2)
	{
		this->comvar->chv[this->comvar->Ckey].rnorm = 0.0;
		return 1;
	}

	do
	{
		dd = d;
		dt = (-(ka*ka*this->comvar->gsT)/(r*r*r) - this->comvar->gttsT - this->vpos->mt->val*d)/(2.0*this->vpos->m->val);

		A = this->vpos->m->val*this->vpos->m->val;
		B = 2.0*this->vpos->m->val*this->comvar->gtsT;
		C = dt*dt + 2.0*dt*this->comvar->gtsD + this->comvar->gt2 - 2.0*ka*ka/r;
		sekey = sqEqv(A, B, C, &d1, &d2);

 		d = d1;
		if(sekey==2)
		{
			this->comvar->chv[this->comvar->Ckey].dt = 0.0;
			return 1;
		}
		if((*niter>MAX_ITER)||(d<0.0)) return 1;
		*niter++;
	}while(fabs(d-dd)>EPS);

	this->comvar->R->set(0, (this->comvar->g->get(0) + d*this->vpos->D->get(0))/r);
	this->comvar->R->set(1, (this->comvar->g->get(1) + d*this->vpos->D->get(1))/r);
	this->comvar->R->set(2, (this->comvar->g->get(2) + d*this->vpos->D->get(2))/r);

	this->comvar->RsM = Smul(this->comvar->R, this->comvar->M);

	this->provRGD();

	this->comvar->RsT = Smul(this->comvar->R, this->comvar->T);
	
	this->iterEnd(r, d, dt, this->comvar->R, *niter, this->comvar->Ckey);
	ret = proofR();

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
//		dd = (-(ka*ka*this->comvar->RsT)/(r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt;
//(7)
		dd1 = (-(ka*ka*this->comvar->gsT)/(r*r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt1;
//(6)
//		dd = (-(ka*ka*this->comvar->RsT)/(r*r))/(this->vpos->mt->val) - this->comvar->gttsT/(this->vpos->mt->val) - 2.0*(this->vpos->m->val/this->vpos->mt->val)*dt;
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

	if(fabs(d-dd)>EPS) ret = iterD((d+dd)/2.0, niter);
	else
	{
		this->iterEnd(r, dd, dt, this->comvar->R, *niter, this->comvar->Ckey);
		ret = proofR();
/*
		this->comvar->chv[this->comvar->Ckey].r[0] = r*this->comvar->R->get(0);
		this->comvar->chv[this->comvar->Ckey].r[1] = r*this->comvar->R->get(1);
		this->comvar->chv[this->comvar->Ckey].r[2] = r*this->comvar->R->get(2);
		this->comvar->chv[this->comvar->Ckey].rnorm = r;

		this->comvar->chv[this->comvar->Ckey].rt[0] = this->comvar->gt->get(0) + dt*this->vpos->D->get(0) + dd*this->vpos->m->val*this->comvar->T->get(0);
		this->comvar->chv[this->comvar->Ckey].rt[1] = this->comvar->gt->get(1) + dt*this->vpos->D->get(1) + dd*this->vpos->m->val*this->comvar->T->get(1);
		this->comvar->chv[this->comvar->Ckey].rt[2] = this->comvar->gt->get(2) + dt*this->vpos->D->get(2) + dd*this->vpos->m->val*this->comvar->T->get(2);
		this->comvar->chv[this->comvar->Ckey].d = dd;
		this->comvar->chv[this->comvar->Ckey].dt = dt;

		this->comvar->chv[this->comvar->Ckey].rtt[0] = -ka*ka*this->comvar->chv[this->comvar->Ckey].R->get(0)/(this->comvar->chv[this->comvar->Ckey].rnorm*this->comvar->chv[this->comvar->Ckey].rnorm);
		this->comvar->chv[this->comvar->Ckey].rtt[1] = -ka*ka*this->comvar->chv[this->comvar->Ckey].R->get(1)/(this->comvar->chv[this->comvar->Ckey].rnorm*this->comvar->chv[this->comvar->Ckey].rnorm);
		this->comvar->chv[this->comvar->Ckey].rtt[2] = -ka*ka*this->comvar->chv[this->comvar->Ckey].R->get(2)/(this->comvar->chv[this->comvar->Ckey].rnorm*this->comvar->chv[this->comvar->Ckey].rnorm);

		
		this->comvar->chv[this->comvar->Ckey].f[0] = 0.0;
		this->comvar->last = 0;
		this->comvar->chv[this->comvar->Ckey].iter = *niter;
		ret = proofR();

		this->comvar->chv[this->comvar->Ckey].ksi[0] = this->comvar->chv[this->comvar->Ckey].r[0] - this->comvar->g->get(0);
		this->comvar->chv[this->comvar->Ckey].ksi[1] = this->comvar->chv[this->comvar->Ckey].r[1] - this->comvar->g->get(1);
		this->comvar->chv[this->comvar->Ckey].ksi[2] = this->comvar->chv[this->comvar->Ckey].r[2] - this->comvar->g->get(2);

		this->comvar->chv[this->comvar->Ckey].ksit[0] = this->comvar->chv[this->comvar->Ckey].rt[0] - this->comvar->gt->get(0);
		this->comvar->chv[this->comvar->Ckey].ksit[1] = this->comvar->chv[this->comvar->Ckey].rt[1] - this->comvar->gt->get(1);
		this->comvar->chv[this->comvar->Ckey].ksit[2] = this->comvar->chv[this->comvar->Ckey].rt[2] - this->comvar->gt->get(2);

		this->comvar->chv[this->comvar->Ckey].ksitt[0] = this->comvar->chv[this->comvar->Ckey].rtt[0] - this->comvar->gtt->get(0);
		this->comvar->chv[this->comvar->Ckey].ksitt[1] = this->comvar->chv[this->comvar->Ckey].rtt[1] - this->comvar->gtt->get(1);
		this->comvar->chv[this->comvar->Ckey].ksitt[2] = this->comvar->chv[this->comvar->Ckey].rtt[2] - this->comvar->gtt->get(2);
*/

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


	if(fabs(d-dd)>EPS) ret = iterDc((dd+d)/2.0, niter);
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
	pv = new char[100];
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

	this->res = new orbit(this->klog);

	this->res->initElem(eJD, M0, w, Node, inc, ecc, q);
//
//
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
	pv = new char[100];
	r->print(pv);
	printf("\n\nr ecl:	%s	%f", pv, r->norm());
	rt->print(pv);
	printf("\nrt ecl:	%s	%f", pv, rt->norm());
//

//	r->RotX(-EKV);
//	rt->RotX(-EKV);

//
	r->print(pv);
	printf("\n\nr ekv:	%s	%f", pv, r->norm());
	rt->print(pv);
	printf("\nrt ekv:	%s	%f", pv, rt->norm());
//

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
//
	Ekv2Ecl(&ei, &eN, &eP, inc, Node, w);
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
	eJD = ((double)(int)T)/2.0;
	dT = eJD - TT;
//	dT = TT - eJD;

	nn = ka*pow(q, -1.5)/sqrt(2.0);

	M0 = nn*dT;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	printf("\n\nOrb Elsements\n\nT\t%f\nPer.\t%f\nNode\t%f\ninc\t%f\nq\t%f\n", TT, w*180.0/PI, Node*180.0/PI, inc*180.0/PI, q);
	if(this->klog)
	{
		fprintf(this->slog->FLOG, "\n\nOrb Elsements\n\nT\t%f\nPer.\t%f\nNode\t%f\ninc\t%f\nq\t%f\n", TT, w*180.0/PI, Node*180.0/PI, inc*180.0/PI, q);
	}

	this->res = new orbit(0);

	this->res->initElem(eJD, M0, w, Node, inc, ecc, q);
//
//
	return 0;
}

/*
int proofRV(myvector *r, myvector *v)
{
	double 
	return 0;
}
*/

int pvdP::initVar()
{
	char *str;
	str = new char[1000];
	double norm;

	this->vpos->findPVD();
	this->vpos->writePVD();

//test
//
	this->comvar = new ComVar;

	if(!this->fini)
	{
		this->detCkey();
		this->comvar->inird = 0.1;
	}
	else this->initC();

//g
	note *temp;
	temp = new note;

	this->sun->getNote(temp, 0);
	this->comvar->g0 = new myvector(3);
	this->comvar->g0->set(0, temp->x);
	this->comvar->g0->set(1, temp->y);
	this->comvar->g0->set(2, temp->z);

	this->comvar->t0 = temp->eJD;

	this->sun->getNote(temp, 1);
	this->comvar->g = new myvector(3);
	this->comvar->g->set(0, temp->x);
	this->comvar->g->set(1, temp->y);
	this->comvar->g->set(2, temp->z);

	this->comvar->t = temp->eJD;

	this->comvar->G = new myvector(3);
	norm = this->comvar->g->norm(); 
	this->comvar->G->set(0, this->comvar->g->get(0)/norm);
	this->comvar->G->set(1, this->comvar->g->get(1)/norm);
	this->comvar->G->set(2, this->comvar->g->get(2)/norm);
	if(this->klog)
	{
		fprintf(this->slog->FLOG, "\nINIT_VAR:\n\n", str);
		this->comvar->g->print(str);
		fprintf(this->slog->FLOG, "g:\n%s\n", str);
		this->comvar->G->print(str);
		fprintf(this->slog->FLOG, "G:\n%s	%f\n", str, this->comvar->G->norm());
	}
//gt
	this->comvar->gt = new myvector(3);
	this->comvar->gt->set(0, temp->xt);
	this->comvar->gt->set(1, temp->yt);
	this->comvar->gt->set(2, temp->zt);
	if(this->klog)
	{
		this->comvar->gt->print(str);
		fprintf(this->slog->FLOG, "gt:\n%s\n", str);
	}
//gtt
	this->comvar->gtt = new myvector(3);
	this->comvar->gtt->set(0, temp->xtt);
	this->comvar->gtt->set(1, temp->ytt);
	this->comvar->gtt->set(2, temp->ztt);

	if(this->klog)
	{
		this->comvar->gtt->print(str);
		fprintf(this->slog->FLOG, "gtt:\n%s\n", str);
	}
	
	this->comvar->g2 = this->comvar->g->get(0)*this->comvar->g->get(0) + this->comvar->g->get(1)*this->comvar->g->get(1) + this->comvar->g->get(2)*this->comvar->g->get(2);
	this->comvar->gt2 = this->comvar->gt->get(0)*this->comvar->gt->get(0) + this->comvar->gt->get(1)*this->comvar->gt->get(1) + this->comvar->gt->get(2)*this->comvar->gt->get(2);
	this->comvar->gtt2 = this->comvar->gtt->get(0)*this->comvar->gtt->get(0) + this->comvar->gtt->get(1)*this->comvar->gtt->get(1) + this->comvar->gtt->get(2)*this->comvar->gtt->get(2);

	if(this->klog)
	{
		this->vpos->D->print(str);
		fprintf(this->slog->FLOG, "D:\n%s\n", str);
	}

	this->sun->getNote(temp, 2);
	this->comvar->g1 = new myvector(3);
	this->comvar->g1->set(0, temp->x);
	this->comvar->g1->set(1, temp->y);
	this->comvar->g1->set(2, temp->z);

	this->comvar->t1 = temp->eJD;

//T
	this->comvar->T = new myvector(3);
//	double T0, T1, T2;
//	T0 = -sin(this->vpos->psi->val)*sin(this->vpos->r->val);
//	T1 = -sin(this->vpos->psi->val);//*sin(this->vpos->r);// - cos(this->vpos->psi)*sin(this->vpos->d)*cos(this->vpos->r);
	this->comvar->T->set(0, -sin(this->vpos->psi->val)*sin(this->vpos->r->val) - cos(this->vpos->psi->val)*sin(this->vpos->d->val)*cos(this->vpos->r->val));
	this->comvar->T->set(1, sin(this->vpos->psi->val)*cos(this->vpos->r->val) - cos(this->vpos->psi->val)*sin(this->vpos->d->val)*sin(this->vpos->r->val));
	this->comvar->T->set(2, cos(this->vpos->psi->val)*cos(this->vpos->d->val));

	if(this->klog)
	{
		this->comvar->T->print(str);
		fprintf(this->slog->FLOG, "T:\n%s\n", str);
	}
//M
	this->comvar->M = new myvector(3);
	*this->comvar->M = Vmul(this->vpos->D, this->comvar->T);

	if(this->klog)
	{
		this->comvar->M->print(str);
		fprintf(this->slog->FLOG, "M:\n%s\n", str);
	}
//R
	this->comvar->R = new myvector(3);
	this->comvar->chv[this->comvar->Ckey].R = new myvector(3);

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

	if(this->klog)
	{
		fprintf(this->slog->FLOG, "gsD\t%e\ngtsDt\t%e\ngtsT\t%e\ngttsT\t%e\n", this->comvar->gsD, this->comvar->gtsDt, this->comvar->gtsT, this->comvar->gttsT);
		fprintf(this->slog->FLOG, "gsT\t%e\n", this->comvar->gsT);
		fprintf(this->slog->FLOG, "gttsM\t%e\n", this->comvar->gttsM);
		fprintf(this->slog->FLOG, "gsM\t%e\n", this->comvar->gsM);
		fprintf(this->slog->FLOG, "GsM\t%e\n", this->comvar->GsM);
		fprintf(this->slog->FLOG, "sign(GsM)\t%e\n\n", this->comvar->signGM);
	}

//
	mpc *ttt;
	ttt = new mpc(this->vpos->obs->Fkey);

	this->vpos->obs->getmpc(ttt, 0);
	this->comvar->r0 = ttt->r;
	this->comvar->d0 = ttt->d;

	this->vpos->obs->getmpc(ttt, (this->vpos->obs->nmpcs-1)/2);
	this->comvar->r = ttt->r;
	this->comvar->d = ttt->d;

	this->vpos->obs->getmpc(ttt, this->vpos->obs->nmpcs-1);
	this->comvar->r1 = ttt->r;
	this->comvar->d1 = ttt->d;


	return 0;
}

int pvdP::provTDM()
{
	double SM;

	SM = sqrt(this->comvar->T->get(0)*this->comvar->T->get(0) + this->comvar->T->get(1)*this->comvar->T->get(1) + this->comvar->T->get(2)*this->comvar->T->get(2));
	if(fabs(SM-1.0)>EPS) return 1;
	SM = sqrt(this->vpos->D->get(0)*this->vpos->D->get(0) + this->vpos->D->get(1)*this->vpos->D->get(1) + this->vpos->D->get(2)*this->vpos->D->get(2));
	if(fabs(SM-1.0)>EPS) return 2;
	SM = Smul(this->vpos->D, this->comvar->T);
	return(fabs(SM)>EPS);
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
	ssgn = new char[1];

	printf("\nproofPVD\n");
	printf("\nparam\tcount\t\ttrue\t\tO-C\n");
	if(this->klog) 
	{
		fprintf(this->slog->FLOG, "\nproofPVD\n");
		fprintf(this->slog->FLOG, "\nparam\tcount\t\ttrue\t\tO-C\n");
	}


	//
	char *pv;
	pv = new char[1000];

	double aV, dV, oc;

	myvector *D, *Dt, *Dtt;
	D = new myvector(3);
	Dt = new myvector(3);
	Dtt = new myvector(3);

	double d, dt, dtt;

	d = sqrt(this->comvar->chv[this->comvar->Ckey].ksi[0]*this->comvar->chv[this->comvar->Ckey].ksi[0] + this->comvar->chv[this->comvar->Ckey].ksi[1]*this->comvar->chv[this->comvar->Ckey].ksi[1] + this->comvar->chv[this->comvar->Ckey].ksi[2]*this->comvar->chv[this->comvar->Ckey].ksi[2]);
	dt = (this->comvar->chv[this->comvar->Ckey].ksit[0]*this->comvar->chv[this->comvar->Ckey].ksi[0] + this->comvar->chv[this->comvar->Ckey].ksit[1]*this->comvar->chv[this->comvar->Ckey].ksi[1] + this->comvar->chv[this->comvar->Ckey].ksit[2]*this->comvar->chv[this->comvar->Ckey].ksi[2])/d;

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
	if(this->klog) fprintf(this->slog->FLOG, "\nRA\t%s%d %d %9.6f", ssgn, year ,mouth ,day);
	rad2hms(this->vpos->r->val, &sgn, &year, &mouth, &day);
	if(sgn==0) strcpy(ssgn, "-");
	else strcpy(ssgn, "");
	printf("\t%s%d %d %9.6f", ssgn, year ,mouth ,day);
	if(this->klog) fprintf(this->slog->FLOG, "\t%s%d %d %9.6f", ssgn, year ,mouth ,day);

	rad2gms(dV, &sgn, &year, &mouth, &day);
	if(sgn==0) strcpy(ssgn, "-");
	else strcpy(ssgn, "");
	printf("\nDEC\t%s%d %d %9.6f", ssgn, year ,mouth ,day);
	if(this->klog) fprintf(this->slog->FLOG, "\nRA\t%s%d %d %9.6f", ssgn, year ,mouth ,day);
	rad2gms(this->vpos->d->val, &sgn, &year, &mouth, &day);
	if(sgn==0) strcpy(ssgn, "-");
	else strcpy(ssgn, "");
	printf("\t%s%d %d %9.6f", ssgn, year ,mouth ,day);
	if(this->klog) fprintf(this->slog->FLOG, "\t%s%d %d %9.6f", ssgn, year ,mouth ,day);

	oc = sqrt((aV - this->vpos->r->val)*(aV - this->vpos->r->val) + (dV - this->vpos->d->val)*(dV - this->vpos->d->val));

	printf("\t%e", oc*SECINRAD);
	if(this->klog) fprintf(this->slog->FLOG, "\t%e", oc*SECINRAD);

	Dt->set(0, this->comvar->chv[this->comvar->Ckey].ksit[0]/d - D->get(0)*dt/d);
	Dt->set(1, this->comvar->chv[this->comvar->Ckey].ksit[1]/d - D->get(1)*dt/d);
	Dt->set(2, this->comvar->chv[this->comvar->Ckey].ksit[2]/d - D->get(2)*dt/d);

	double m, psi;

	m = sqrt(Dt->get(0)*Dt->get(0) + Dt->get(1)*Dt->get(1) + Dt->get(2)*Dt->get(2));

	printf("\nm\t%f\t%f", m*SECINRAD, this->vpos->m->val*SECINRAD);
	if(this->klog) fprintf(this->slog->FLOG, "\nm\t%f\t%f", m*SECINRAD, this->vpos->m->val*SECINRAD);

	oc = fabs(m - this->vpos->m->val)*SECINRAD;
	printf("\t%e", oc);
	if(this->klog) fprintf(this->slog->FLOG, "\t%e", oc);


	myvector *T, *M;

	T = new myvector(3);
	M = new myvector(3);

	T->set(0, this->comvar->chv[this->comvar->Ckey].ksit[0]/(d*m) - D->get(0)*dt/(d*m));
	T->set(1, this->comvar->chv[this->comvar->Ckey].ksit[1]/(d*m) - D->get(1)*dt/(d*m));
	T->set(2, this->comvar->chv[this->comvar->Ckey].ksit[2]/(d*m) - D->get(2)*dt/(d*m));

	*M = Vmul(D, T);

//	M3 = D[0]*T[1] - D[1]*T[0];

	psi = atan2(M->get(2), T->get(2));
	if(psi<0.0) psi+=2.0*PI;

	printf("\npsi\t%f\t%f", psi*180.0/PI, this->vpos->psi->val*180.0/PI);
	if(this->klog) fprintf(this->slog->FLOG, "\npsi\t%f\t%f", psi*180.0/PI, this->vpos->psi->val*180.0/PI);

	oc = fabs(psi - this->vpos->psi->val)*180.0/PI;
	printf("\t%e", oc);
	if(this->klog) fprintf(this->slog->FLOG, "\t%e", oc);


	dtt = (this->comvar->chv[this->comvar->Ckey].ksitt[0]*D->get(0) + this->comvar->chv[this->comvar->Ckey].ksitt[1]*D->get(1) + this->comvar->chv[this->comvar->Ckey].ksitt[2]*D->get(2)) + d*m*m;

	Dtt->set(0, (this->comvar->chv[this->comvar->Ckey].ksitt[0] - dtt*D->get(0) - 2.0*dt*Dt->get(0))/d);
	Dtt->set(1, (this->comvar->chv[this->comvar->Ckey].ksitt[1] - dtt*D->get(1) - 2.0*dt*Dt->get(1))/d);
	Dtt->set(2, (this->comvar->chv[this->comvar->Ckey].ksitt[2] - dtt*D->get(2) - 2.0*dt*Dt->get(2))/d);

	double mt, c;

//	mt = (dtt*Smul(Dtt, T) + 2.0*m*dt)/d;
//	mt = Smul(Dt, Dtt)/m;
	mt = ((this->comvar->chv[this->comvar->Ckey].ksitt[0]*T->get(0) + this->comvar->chv[this->comvar->Ckey].ksitt[1]*T->get(1) + this->comvar->chv[this->comvar->Ckey].ksitt[2]*T->get(2)) - 2.0*m*dt)/d;

	printf("\nmt\t%f\t%f", mt*SECINRAD, this->vpos->mt->val*SECINRAD);
	if(this->klog) fprintf(this->slog->FLOG, "\nmt\t%f\t%f", mt*SECINRAD, this->vpos->mt->val*SECINRAD);

	oc = fabs(mt - this->vpos->mt->val)*SECINRAD;
	printf("\t%e", oc);
	if(this->klog) fprintf(this->slog->FLOG, "\t%e", oc);

	c = sqrt(1.0 + pow(((this->comvar->chv[this->comvar->Ckey].ksitt[0]*M->get(0) + this->comvar->chv[this->comvar->Ckey].ksitt[1]*M->get(1) + this->comvar->chv[this->comvar->Ckey].ksitt[2]*M->get(2)))/(m*m*d), 2.0));

	printf("\nc\t%f\t%f", c, this->vpos->c->val);
	if(this->klog) fprintf(this->slog->FLOG, "\nc\t%f\t%f", c, this->vpos->c->val);

	oc = fabs(c - this->vpos->c->val);
	printf("\t%e", oc);
	if(this->klog) fprintf(this->slog->FLOG, "\t%e", oc);


//	c = this->vpos->c->val;

	D->print(pv);
	printf("\n\nD\t%s", pv);
	this->vpos->D->print(pv);
	printf("\nvposD\t%s", pv);
	Dt->print(pv);
	printf("\nDt\t%s", pv);
	this->vpos->Dt->print(pv);
	printf("\nvposDt\t%s", pv);
	Dtt->print(pv);
	printf("\nDtt\t%s", pv);
	this->vpos->Dtt->print(pv);
	printf("\nvposDtt\t%s\n", pv);

	printf("\ncontrol\n%e\t%e", Smul(D, Dtt), -m*m);
	printf("\n%e\t%e", Smul(Dt, Dtt), mt*m);
	printf("\n%e\t%e\n\n", Smul(Dtt, Dtt), mt*mt + m*m*m*m*c*c);
/*
	if(this->klog)
	{
		D->print(pv);
		fprintf(this->slog->FLOG, "\n\nD\t%s", pv);
		this->vpos->D->print(pv);
		fprintf(this->slog->FLOG, "\nvposD\t%s", pv);
		Dt->print(pv);
		fprintf(this->slog->FLOG, "\nDt\t%s", pv);
		this->vpos->Dt->print(pv);
		fprintf(this->slog->FLOG, "\nvposDt\t%s", pv);
		Dtt->print(pv);
		fprintf(this->slog->FLOG, "\nDtt\t%s", pv);
		this->vpos->Dtt->print(pv);
		fprintf(this->slog->FLOG, "\nvposDtt\t%s\n", pv);

		fprintf(this->slog->FLOG, "\ncontrol\n%e\t%e", Smul(D, Dtt), -m*m);
		fprintf(this->slog->FLOG, "\n%e\t%e", Smul(Dt, Dtt), mt*m);
		fprintf(this->slog->FLOG, "\n%e\t%e\n\n", Smul(Dtt, Dtt), mt*mt + m*m*m*m*c*c);
	}
*/
	printf("\n");
	if(this->klog) fprintf(this->slog->FLOG, "\n");
//
	return 0;
}

int pvdP::initC()
{
	char *str, *nstr;
	int Strlen = 100;
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
		fgets(str, Strlen, this->FINI);

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
	}

	return 0;
}

int pvdP::printOC(double aV, double dV, double a, double d, double t)
{
	double day, oc;
	int year, mouth, sgn;
	char *ssgn;
	ssgn = new char[1];

	dat2YMD(t, &year, &mouth, &day);
	printf("\n%d %d %f", year, mouth, day);
	if(this->klog) fprintf(this->slog->FLOG,"\n%d %d %f", year, mouth, day);

	rad2hms(aV, &sgn, &year, &mouth, &day);
	if(sgn==0) strcpy(ssgn, "-");
	else strcpy(ssgn, " ");
	printf(" %s%d %d %9.6f", ssgn, year ,mouth ,day);
	if(this->klog) fprintf(this->slog->FLOG, " %s%d %d %9.6f", ssgn, year ,mouth ,day);
	rad2gms(dV, &sgn, &year, &mouth, &day);
	if(sgn==0) strcpy(ssgn, "-");
	else strcpy(ssgn, " ");
	printf("\t%s%d %d %9.6f", ssgn, year ,mouth ,day);
	if(this->klog) fprintf(this->slog->FLOG, "\t%s%d %d %9.6f", ssgn, year ,mouth ,day);

	rad2hms(a, &sgn, &year, &mouth, &day);
	if(sgn==0) strcpy(ssgn, "-");
	else strcpy(ssgn, " ");
	printf("\n\t\t %s%d %d %9.6f", ssgn, year ,mouth ,day);
	if(this->klog) fprintf(this->slog->FLOG, "\n\t\t %s%d %d %9.6f", ssgn, year ,mouth ,day);
	rad2gms(d, &sgn, &year, &mouth, &day);
	if(sgn==0) strcpy(ssgn, "-");
	else strcpy(ssgn, " ");
	printf("\t%s%d %d %9.6f", ssgn, year ,mouth ,day);
	if(this->klog) fprintf(this->slog->FLOG, "\t%s%d %d %9.6f", ssgn, year ,mouth ,day);

	oc = (aV - a)*SECINRAD/15.0;
	printf("\n\nO-C\"\t\t %e", oc);
	if(this->klog) fprintf(this->slog->FLOG, "\n\nO-C\"\t\t %e", oc);
	oc = (dV - d)*SECINRAD;
	printf("\t\t%e", oc);
	if(this->klog) fprintf(this->slog->FLOG, "\t\t%e", oc);
	oc = sqrt((aV - a)*(aV - a) + (dV - d)*(dV - d))*SECINRAD;
	printf("\t%e\n", oc);
	if(this->klog) fprintf(this->slog->FLOG, "\t%e\n", oc);
	oc = (aV - a)*SECINRAD/15.0;
	printf("\t\t %f", oc);
	if(this->klog) fprintf(this->slog->FLOG, "\t\t %f", oc);
	oc = (dV - d)*SECINRAD;
	printf("\t\t%f", oc);
	if(this->klog) fprintf(this->slog->FLOG, "\t\t%f", oc);
	oc = sqrt((aV - a)*(aV - a) + (dV - d)*(dV - d))*SECINRAD;
	printf("\t%f\n", oc);
	if(this->klog) fprintf(this->slog->FLOG, "\t%f\n", oc);


	return 0;
}

int pvdP::iterEnd(double r, double d, double dt, myvector *R, int niter, int nitype)
{

	this->comvar->chv[nitype].r[0] = r*R->get(0);
	this->comvar->chv[nitype].r[1] = r*R->get(1);
	this->comvar->chv[nitype].r[2] = r*R->get(2);
	this->comvar->chv[nitype].rnorm = r;

	this->comvar->chv[nitype].R->set(0, R->get(0));
	this->comvar->chv[nitype].R->set(1, R->get(1));
	this->comvar->chv[nitype].R->set(2, R->get(2));

//	dt = (-(ka*ka*this->comvar->gsT)/(r*r*r) - this->comvar->gttsT - this->vpos->mt->val*d)/(2.0*this->vpos->m->val);


	this->comvar->chv[nitype].rt[0] = this->comvar->gt->get(0) + dt*this->vpos->D->get(0) + d*this->vpos->m->val*this->comvar->T->get(0);
	this->comvar->chv[nitype].rt[1] = this->comvar->gt->get(1) + dt*this->vpos->D->get(1) + d*this->vpos->m->val*this->comvar->T->get(1);
	this->comvar->chv[nitype].rt[2] = this->comvar->gt->get(2) + dt*this->vpos->D->get(2) + d*this->vpos->m->val*this->comvar->T->get(2);
	this->comvar->chv[nitype].d = d;
	this->comvar->chv[nitype].dt = dt;

	this->comvar->chv[nitype].rtt[0] = -ka*ka*this->comvar->chv[nitype].R->get(0)/(this->comvar->chv[nitype].rnorm*this->comvar->chv[nitype].rnorm);
	this->comvar->chv[nitype].rtt[1] = -ka*ka*this->comvar->chv[nitype].R->get(1)/(this->comvar->chv[nitype].rnorm*this->comvar->chv[nitype].rnorm);
	this->comvar->chv[nitype].rtt[2] = -ka*ka*this->comvar->chv[nitype].R->get(2)/(this->comvar->chv[nitype].rnorm*this->comvar->chv[nitype].rnorm);
		
	this->comvar->chv[nitype].f[0] = 0.0;
	this->comvar->last = 0;
	this->comvar->chv[nitype].iter = niter;

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

	return 0;
}
