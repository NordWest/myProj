#include "sPorabola.h"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define AU 1
#define MS 1
#define VE 2979256.9529966
#define PI 3.141592653589
#define ka 0.017202098955
#define EPS 1.0e-3
#define xi ka*sqrt(MS)
#define EKV 0.4093197552027299

#define L 0.0057756

#define EPO 1900
#define JDEPO 2415020.5

sPorabola::sPorabola()
{
	this->sunpos = new rec;
	this->apppos = new ast;
	this->objorb = new orbit;

	this->eeq1cnt = NULL;
	this->appcnt = NULL;
	this->comvar = NULL;
}

int sPorabola::initFiles(char *fast, char *frec, int log)
{
	int fares, frres;
	fares = this->apppos->initFile(fast);// fprintf(FLOG, "\nfast init OK";

	frres = this->sunpos->initFile(frec);

	if(log)
	{
		fprintf(FLOG, "\nfast init: %d", fares);
		fprintf(FLOG, "\nfrec init: %d", frres);
	}

	return 0;
}

int sPorabola::compute(char *fast, char *frec, char *forb, char *fini, int iFlog, int MClog, int FAlog, int SAlog, int TAlog, int PSNlog, int COlog)
{
	if(fini==NULL)
	{
		printf("\nError file ini\n");
		return 10;
	}
	this->fini = new char[100];
	strcpy(this->fini, fini);

	int log;
	log = (iFlog*iFlog + FAlog*FAlog + SAlog*SAlog + TAlog*TAlog + PSNlog*PSNlog + COlog*COlog);
	if(log!=0) this->FLOG = fopen("sPorabola.log", "w");
	if(this->initFiles(fast, frec, iFlog))
	{
		fclose(this->FLOG);
		return 1;
	}
	if(this->makeConst(MClog))
	{
		fclose(this->FLOG);
		return 2;
	}
	if(this->ferstApp(FAlog))
	{
		fclose(this->FLOG);
		return 3;
	}


	if(this->apppos->hat->typeCoord==0)
	{
		double ro = this->comvar->ro1[this->comvar->last]*(this->appcnt->t2 - this->appcnt->t)/(this->appcnt->t2 - this->appcnt->t1) + this->comvar->ro2[this->comvar->last]*(this->appcnt->t - this->appcnt->t1)/(this->appcnt->t2 - this->appcnt->t1);

		this->appcnt->t1 = this->appcnt->t1 - L*this->comvar->ro1[this->comvar->last];
		this->appcnt->t = this->appcnt->t - L*ro;
		this->appcnt->t2 = this->appcnt->t2 - L*this->comvar->ro2[this->comvar->last];
		if(log)
		{
			fprintf(this->FLOG, "\n////////////////////////////////////////\n\nISPRAVLENIE ZA ABBERACIJU:\n");
			fprintf(this->FLOG, "\n-Lpo1	%7.4f", - L*this->comvar->ro1[this->comvar->last]);
			fprintf(this->FLOG, "\n-Lpo	%7.4f", - L*ro);
			fprintf(this->FLOG, "\n-Lpo2	%7.4f\n", - L*this->comvar->ro2[this->comvar->last]);

			fprintf(this->FLOG, "\nt01	%7.4f", this->appcnt->t1);
			fprintf(this->FLOG, "\nt0	%7.4f", this->appcnt->t);
			fprintf(this->FLOG, "\nt02	%7.4f\n", this->appcnt->t2);

		}
	}

	if(this->secondApp(SAlog))
	{
		fclose(this->FLOG);
		return 4;
	}
/*
	if(this->thirdApp(TAlog))
	{
		fclose(this->FLOG);
		return 5;
	}
*/
	if(this->PSN(PSNlog))
	{
		fclose(this->FLOG);
		return 6;
	}
	if(this->countOrb(COlog))
	{
		fclose(this->FLOG);
		return 7;
	}
/*	if(this->countOrb(COlog))
	{
		fclose(this->FLOG);
		return 7;
	}*/
	if(this->fulInfo())
	{
		fclose(this->FLOG);
		return 8;
	}
	if(this->orbWrite(forb))
	{
		fclose(this->FLOG);
		return 9;
	}
	if(this->proofSM()) return 10;
//	if(this->soPost()) return 10;

	return 0;
}











//private//////////////////////////////////////////////////////////

int sPorabola::makeConst(int log)
{

//AppConst
	this->appcnt = new AppConst;

	app *vid;
	vid = new app;
	this->apppos->getApp(vid, 0);
	this->appcnt->l1 = cos(vid->d)*cos(vid->r);
	this->appcnt->m1 = cos(vid->d)*sin(vid->r);
	this->appcnt->v1 = sin(vid->d);
	this->appcnt->t1 = vid->eJD;

	this->apppos->getApp(vid, 1);

	this->appcnt->l = cos(vid->d)*cos(vid->r);
	this->appcnt->m = cos(vid->d)*sin(vid->r);
	this->appcnt->v = sin(vid->d);
	this->appcnt->t = vid->eJD;

	this->apppos->getApp(vid, 2);

	this->appcnt->l2 = cos(vid->d)*cos(vid->r);
	this->appcnt->m2 = cos(vid->d)*sin(vid->r);
	this->appcnt->v2 = sin(vid->d);
	this->appcnt->t2 = vid->eJD;

/*model
	this->appcnt->l1 = 0.78203;
	this->appcnt->m1 = 0.37262;
	this->appcnt->v1 = 0.49960;
	this->appcnt->t1 = vid->eJD;

	this->apppos->getApp(vid, 1);

	this->appcnt->l = 0.74215;
	this->appcnt->m = 0.38154;
	this->appcnt->v = 0.55106;
	this->appcnt->t = vid->eJD;

	this->apppos->getApp(vid, 2);

	this->appcnt->l2 = 0.69146;
	this->appcnt->m2 = 0.39064;
	this->appcnt->v2 = 0.60767;
	this->appcnt->t2 = vid->eJD;
*/

//	dat2JD(&, 1909, 6, 0.0);


	if(log)
	{
		fprintf(this->FLOG, "\n////////////////////////////////////////\n\nMAKE CONST:\n");;
		fprintf(this->FLOG, "\nt, JD		l		m		v");
		fprintf(this->FLOG, "\n%f	%f	%f	%f", this->appcnt->t1, this->appcnt->l1, this->appcnt->m1, this->appcnt->v1);
		fprintf(this->FLOG, "\n%f	%f	%f	%f", this->appcnt->t, this->appcnt->l, this->appcnt->m, this->appcnt->v);
		fprintf(this->FLOG, "\n%f	%f	%f	%f\n", this->appcnt->t2, this->appcnt->l2, this->appcnt->m2, this->appcnt->v2);
	}

	delete vid;
	//test appcnt
	int tapp1, tapp, tapp2;

	tapp1 = (fabs((this->appcnt->l1*this->appcnt->l1 + this->appcnt->m1*this->appcnt->m1 + this->appcnt->v1*this->appcnt->v1)-1.0)>EPS);
	tapp = (fabs((this->appcnt->l*this->appcnt->l + this->appcnt->m*this->appcnt->m + this->appcnt->v*this->appcnt->v)-1.0)>EPS);
	tapp2 = (fabs((this->appcnt->l2*this->appcnt->l2 + this->appcnt->m2*this->appcnt->m2 + this->appcnt->v2*this->appcnt->v2)-1.0)>EPS);


	if(log)
	{
		if(!tapp1) fprintf(this->FLOG, "\nl1*l1 + m1*m1 + v1*v1 = 1.0 ?	OK");
		else fprintf(this->FLOG, "\nl1*l1 + m1*m1 + v1*v1 = 1.0 ?	ERR");
		if(!tapp) fprintf(this->FLOG, "\nl*l + m*m + v*v = 1.0 ?		OK");
		else fprintf(this->FLOG, "\nl*l + m*m + v*v = 1.0 ?		ERR");
		if(!tapp2) fprintf(this->FLOG, "\nl2*l2 + m2*m2 + v2*v2 = 1.0 ?	OK");
		else fprintf(this->FLOG, "\nl2*l2 + m2*m2 + v2*v2 = 1.0 ?	ERR");
	}

//
//eEq1Const
	this->eeq1cnt = new eEq1Const;

	note *s1, *s2, *s;
	s1 = new note;
	s2 = new note;
	s = new note;

	this->sunpos->getNote(s1, 0);
	this->sunpos->getNote(s, 1);
	this->sunpos->getNote(s2, 2);

	this->eeq1cnt->C1 = -(this->appcnt->l1*s1->x + this->appcnt->m1*s1->y + this->appcnt->v1*s1->z);
	this->eeq1cnt->C2 = -(this->appcnt->l2*s2->x + this->appcnt->m2*s2->y + this->appcnt->v2*s2->z);

	this->eeq1cnt->S12 = s1->x*s1->x + s1->y*s1->y + s1->z*s1->z - this->eeq1cnt->C1*this->eeq1cnt->C1;
	this->eeq1cnt->S22 = s2->x*s2->x + s2->y*s2->y + s2->z*s2->z - this->eeq1cnt->C2*this->eeq1cnt->C2;

	this->eeq1cnt->D1 = 2.0*(this->appcnt->l1*s2->x + this->appcnt->m1*s2->y + this->appcnt->v1*s2->z);
	this->eeq1cnt->D2 = 2.0*(this->appcnt->l2*s1->x + this->appcnt->m2*s1->y + this->appcnt->v2*s1->z);

	this->eeq1cnt->E = 2.0*(this->appcnt->l1*this->appcnt->l2 + this->appcnt->m1*this->appcnt->m2 + this->appcnt->v1*this->appcnt->v2);
	this->eeq1cnt->G = 2.0*(s1->x*s2->x + s1->y*s2->y + s1->z*s2->z);

	if(log)
	{
		fprintf(this->FLOG, "\nC1				%f", this->eeq1cnt->C1);
		fprintf(this->FLOG, "\nC2				%f", this->eeq1cnt->C2);

		fprintf(this->FLOG, "\nS12				%f", this->eeq1cnt->S12);
		fprintf(this->FLOG, "\nS22				%f", this->eeq1cnt->S22);

		fprintf(this->FLOG, "\nD1				%f", this->eeq1cnt->D1);
		fprintf(this->FLOG, "\nD2				%f", this->eeq1cnt->D2);

		fprintf(this->FLOG, "\nE				%f", this->eeq1cnt->E);
		fprintf(this->FLOG, "\nG				%f\n", this->eeq1cnt->G);
	}
//
//ComonVar
	this->comvar = new ComonVar;

	double zn1, zn2, zn3;

	zn1 = (this->appcnt->l*this->appcnt->m2 - this->appcnt->m*this->appcnt->l2);
	zn2 = (this->appcnt->l*this->appcnt->v2 - this->appcnt->v*this->appcnt->l2);
	zn3 = (this->appcnt->m*this->appcnt->v2 - this->appcnt->v*this->appcnt->m2);

	if(log)
	{
		fprintf(this->FLOG, "\nzn1				%f", zn1);
		fprintf(this->FLOG, "\nzn2				%f", zn2);
		fprintf(this->FLOG, "\nzn3				%f\n", zn3);
	}

	if((fabs(zn1)>fabs(zn2))&&(fabs(zn1)>fabs(zn3)))
	{
		this->comvar->K = -(this->appcnt->l*this->appcnt->m1 - this->appcnt->m*this->appcnt->l1)/zn1;
		this->comvar->L1 = (this->appcnt->l*s1->y - this->appcnt->m*s1->x)/zn1;
		this->comvar->L2 = -(this->appcnt->l*s->y - this->appcnt->m*s->x)/zn1;
		this->comvar->L3 = (this->appcnt->l*s2->y - this->appcnt->m*s2->x)/zn1;
	}

	if((fabs(zn2)>fabs(zn1))&&(fabs(zn2)>fabs(zn3)))
	{
		this->comvar->K = -(this->appcnt->l*this->appcnt->v1 - this->appcnt->v*this->appcnt->l1)/zn2;
		this->comvar->L1 = (this->appcnt->l*s1->z - this->appcnt->v*s1->x)/zn2;
		this->comvar->L2 = -(this->appcnt->l*s->z - this->appcnt->v*s->x)/zn2;
		this->comvar->L3 = (this->appcnt->l*s2->z - this->appcnt->v*s2->x)/zn2;
	}

	if((fabs(zn3)>fabs(zn1))&&(fabs(zn3)>fabs(zn2)))
	{
		this->comvar->K = -(this->appcnt->m*this->appcnt->v1 - this->appcnt->v*this->appcnt->m1)/zn3;
		this->comvar->L1 = (this->appcnt->m*s1->z - this->appcnt->v*s1->y)/zn3;
		this->comvar->L2 = -(this->appcnt->m*s->z - this->appcnt->v*s->y)/zn3;
		this->comvar->L3 = (this->appcnt->m*s2->z - this->appcnt->v*s2->y)/zn3;
	}

	if(log)
	{
		fprintf(this->FLOG, "\nK				%f", this->comvar->K);
		fprintf(this->FLOG, "\nL1				%f", this->comvar->L1);
		fprintf(this->FLOG, "\nL2				%f", this->comvar->L2);
		fprintf(this->FLOG, "\nL3				%f\n", this->comvar->L3);
	}

	this->comvar->tau = ka*(this->appcnt->t2 - this->appcnt->t1);

	if(log)
	{
		fprintf(this->FLOG, "\ntau				%f", this->comvar->tau);
		fprintf(this->FLOG, "\n2(tau)^2			%f", 4.0*(this->comvar->tau)*(this->comvar->tau));
	}

/*
	this->comvar->initpo1 = 18.8;
	this->comvar->initpo2 = 19.0;
*/
	this->initC();


	return(tapp1 + tapp*2 + tapp2*3);
}

int sPorabola::initC()
{
	FILE *FINI;
	char *str, *nstr;
	int Strlen = 100;
	int npos, pos;

	FINI = fopen(this->fini, "r");

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

	while(!feof(FINI))
	{
		fgets(str, Strlen, FINI);

		pos = 0;
		slovo(str, nstr, pos, &npos);
		strcpy(elem, nstr);

		pos = npos;
		slovo(str, nstr, pos, &npos);

		strcpy(sval, nstr);

		if(streqv(elem, "inipo1"))
		{
			this->comvar->initpo1 = atof(sval);
		}
		else if(streqv(elem, "inipo2"))
		{
			this->comvar->initpo2 = atof(sval);
		}
	}

	fclose(FINI);

	return 0;
}

//////////////////////////Approximation

int sPorabola::ferstApp(int log)
{
	this->comvar->n1 = (this->appcnt->t2 - this->appcnt->t)/(this->appcnt->t - this->appcnt->t1);
	this->comvar->n2 = (this->appcnt->t2 - this->appcnt->t1)/(this->appcnt->t - this->appcnt->t1);

	if(log)
	{
		fprintf(this->FLOG, "\n////////////////////////////////////////\n\nFERST APP\n");
		fprintf(this->FLOG, "\nn1/n2				%f", this->comvar->n1);
		fprintf(this->FLOG, "\n1/n2				%f", this->comvar->n2);
	}

	this->indexMm();

	if(log)
	{
		fprintf(this->FLOG, "\nM				%f\nm				%f\n", this->comvar->M, this->comvar->m);
	}

	this->comComp1(this->comvar->initpo1, this->comvar->initpo2, log);

	return 0;
}

int sPorabola::secondApp(int log)
{
	int n = this->comvar->last;
	double tau1, tau, tau2;

//	ro = this->comvar->ro1[n]*(this->appcnt->t2 - this->appcnt->t)/(this->appcnt->t2 - this->appcnt->t1) + this->comvar->ro2[n]*(this->appcnt->t - this->appcnt->t1)/(this->appcnt->t2 - this->appcnt->t1);

	this->comvar->tau = tau = ka*(this->appcnt->t2 - this->appcnt->t1);
	this->comvar->tau1 = tau1 = ka*(this->appcnt->t2 - this->appcnt->t);
	this->comvar->tau2 = tau2 = ka*(this->appcnt->t - this->appcnt->t1);

	if(log)
	{
		fprintf(this->FLOG, "\n////////////////////////////////////////\n\nSECOND APP\n");

		fprintf(this->FLOG, "\nt02				%f", this->appcnt->t1);
		fprintf(this->FLOG, "\nt0				%f", this->appcnt->t);
		fprintf(this->FLOG, "\nt01				%f\n", this->appcnt->t2);

		fprintf(this->FLOG, "\ntau1				%f", this->comvar->tau1);
		fprintf(this->FLOG, "\ntau2				%f", this->comvar->tau2);
		fprintf(this->FLOG, "\ntau				%f\n", this->comvar->tau);

		fprintf(this->FLOG, "\n(2tau)^2			%f\n", 4.0*this->comvar->tau*this->comvar->tau);

	}

	double ksi, eta;

	ksi = 4.0*pow(this->comvar->r1[n] + this->comvar->r2[n], -3.0)/3.0;
	eta = 3.0*(this->comvar->r2[n] - this->comvar->r1[n])/(this->comvar->r1[n] + this->comvar->r2[n]);

	this->comvar->n1 = tau1/tau2 + tau1*ksi*(tau*(1.0 - tau1/tau2) + tau1*eta);
	this->comvar->n2 = tau/tau2 - tau1*ksi*(tau*(1.0 + tau/tau2) - tau2*eta);

	if(log)
	{
		fprintf(this->FLOG, "\nn1/n2				%f", this->comvar->n1);
		fprintf(this->FLOG, "\n1/n2				%f\n", this->comvar->n2);
	}

	this->indexMm();

	if(log)
	{
		fprintf(this->FLOG, "\nM				%f\nm				%f\n", this->comvar->M, this->comvar->m);
	}

	this->comComp2(0, n, log);
/*
	double drodf;

	drodf = (this->comvar->ro1[n] - this->comvar->ro1[0])/(this->comvar->f[0][n] - this->comvar->f[0][0]);

	do
	{
		n++;
		this->comvar->ro1[n] = this->comvar->ro1[n-1] - this->comvar->f[0][n-1]*drodf;
		this->eilerEq1(&this->comvar->f[0][n], &this->comvar->r1[n], &this->comvar->r2[n], &this->comvar->s2[n], this->comvar->ro1[n], &this->comvar->ro2[n]);
	}while(fabs(this->comvar->f[0][n])>EPS);

	this->comvar->last = n;*/
	return 0;
}

int sPorabola::thirdApp(int log)
{
	if(log)
	{
		fprintf(this->FLOG, "\n////////////////////////////////////////\n\nTHIRD APP\n");
	}

	double r1, r, r2;

	r1 = this->comvar->r1[this->comvar->last];
	r2 = this->comvar->r2[this->comvar->last];

	this->detR(&r, r1, r2);

	this->comvar->mu = 4.0*this->comvar->tau*this->comvar->tau/pow(r1 + r2, 3.0);
	this->comvar->mu1 = 4.0*this->comvar->tau1*this->comvar->tau1/pow(r + r2, 3.0);
	this->comvar->mu2 = 4.0*this->comvar->tau2*this->comvar->tau2/pow(r1 + r, 3.0);

	double eta, eta1, eta2;

	eta = 1.0 - this->comvar->mu/3.0 - this->comvar->mu*this->comvar->mu/6.0 - this->comvar->mu*this->comvar->mu*this->comvar->mu/9.0 - this->comvar->mu*this->comvar->mu*this->comvar->mu*this->comvar->mu*3.0/648.0;
	eta1 = 1.0 - this->comvar->mu1/3.0 - this->comvar->mu1*this->comvar->mu1/6.0 - this->comvar->mu1*this->comvar->mu1*this->comvar->mu1/9.0 - this->comvar->mu1*this->comvar->mu1*this->comvar->mu1*this->comvar->mu1*3.0/648.0;
	eta2 = 1.0 - this->comvar->mu2/3.0 - this->comvar->mu2*this->comvar->mu2/6.0 - this->comvar->mu2*this->comvar->mu2*this->comvar->mu2/9.0 - this->comvar->mu2*this->comvar->mu2*this->comvar->mu2*this->comvar->mu2*3.0/648.0;

	eta = 1.0/eta;
	eta1 = 1.0/eta1;
	eta2 = 1.0/eta2;

	this->comvar->n1 = (this->comvar->tau1*eta2)/(this->comvar->tau2*eta1);
	this->comvar->n2 = (this->comvar->tau*eta2)/(this->comvar->tau2*eta);

	this->indexMm();

	if(this->comComp(this->comvar->ro1[this->comvar->last-1], this->comvar->ro1[this->comvar->last], log)) return 1;

	return 0;
}

/////////////////////////////////////////

int sPorabola::eilerEq1(double *f, double *r1, double *r2, double *s2, double po1, double *po2, int log)
{
	double r12, r22;
	double po1pC1, po2pC2, r1pr2, r1pr22, O02t2, s2mr1pr2;

	*po2 = this->detpo2(po1);

	po1pC1 = po1 + this->eeq1cnt->C1;
	po2pC2 = *po2 + this->eeq1cnt->C2;

	r12 = (po1pC1)*(po1pC1) + this->eeq1cnt->S12;
	r22 = (po2pC2)*(po2pC2) + this->eeq1cnt->S22;

	*s2 = r12 + r22 + this->eeq1cnt->D1*po1 + this->eeq1cnt->D2*(*po2) - this->eeq1cnt->E*po1*(*po2) - this->eeq1cnt->G;

	*r1 = sqrt(r12);
	*r2 = sqrt(r22);

	r1pr2 = *r1 + *r2;
	r1pr22 = r1pr2*r1pr2;

	double c, O;

	c = *s2/r1pr22;
	O = this->detO0(c);

	O02t2 = O*4.0*this->comvar->tau*this->comvar->tau;

	s2mr1pr2 = *s2*r1pr2;

	*f = O02t2 - s2mr1pr2;

	if(log)
	{
		fprintf(this->FLOG, "\n////////////////////////////");
		fprintf(this->FLOG, "");
		fprintf(this->FLOG, "\npo1				%f", po1);
		fprintf(this->FLOG, "\npo2				%f\n", *po2);
		fprintf(this->FLOG, "\npo1+C1				%7.5f", po1pC1);
		fprintf(this->FLOG, "\npo2+C2				%7.5f", po2pC2);
		fprintf(this->FLOG, "\nr12				%7.5f", r12);
		fprintf(this->FLOG, "\nr22				%7.5f", r22);
		fprintf(this->FLOG, "\ns2				%7.5f\n", *s2);
		fprintf(this->FLOG, "\nr1				%7.5f", *r1);
		fprintf(this->FLOG, "\nr2				%7.5f", *r2);
		fprintf(this->FLOG, "\nr1+r2				%7.5f", r1pr2);
		fprintf(this->FLOG, "\n(r1+r2)^2			%7.5f\n", r1pr22);
		fprintf(this->FLOG, "\nc				%7.5f", c);
		fprintf(this->FLOG, "\nO0				%7.5f", O);
		fprintf(this->FLOG, "\nO0(2t)^2			%7.5f", O02t2);
		fprintf(this->FLOG, "\ns2(r1+r2)			%7.5f", s2mr1pr2);
		fprintf(this->FLOG, "\nf(po1)				%e", *f);
	}

	return 0;
}

int sPorabola::indexMm()
{
	this->comvar->M = this->comvar->K*this->comvar->n1;
	this->comvar->m = this->comvar->L1*this->comvar->n1 + this->comvar->L2*this->comvar->n2 + this->comvar->L3;

	return 0;
}

double sPorabola::detpo2(double po1)
{
	return(this->comvar->M*po1 + this->comvar->m);
}

double sPorabola::detO0(double c)
{
	if(c==0.0) return(1.0);
	c = sqrt(c);
	return(pow((3.0*c)/(pow(1.0 + c, 1.5) - pow(1.0 - c, 1.5)), 2.0));
}

int sPorabola::comComp(double p1, double p2, int log)
{
	this->comvar->ro1[0] = p1;
	this->eilerEq1(&this->comvar->f[0][0], &this->comvar->r1[0], &this->comvar->r2[0], &this->comvar->s2[0], this->comvar->ro1[0], &this->comvar->ro2[0], log);

	this->comvar->ro1[1] = p2;
	this->eilerEq1(&this->comvar->f[0][1], &this->comvar->r1[1], &this->comvar->r2[1], &this->comvar->s2[1], this->comvar->ro1[1], &this->comvar->ro2[1], log);

	this->comvar->f[1][0] = (this->comvar->f[0][1] - this->comvar->f[0][0])/(this->comvar->ro1[1] - this->comvar->ro1[0]);
	this->comvar->ro1[2] = this->comvar->ro1[1] - this->comvar->f[0][1]/this->comvar->f[1][0];
	this->eilerEq1(&this->comvar->f[0][2], &this->comvar->r1[2], &this->comvar->r2[2], &this->comvar->s2[2], this->comvar->ro1[2], &this->comvar->ro2[2], log);

	this->comvar->f[1][1] = (this->comvar->f[0][2] - this->comvar->f[0][1])/(this->comvar->ro1[2] - this->comvar->ro1[1]);
	this->comvar->f[2][0] = (this->comvar->f[1][1] - this->comvar->f[1][0])/(this->comvar->ro1[2] - this->comvar->ro1[0]);

	double A, B, C, x1, x2;

	A = this->comvar->f[2][0];
	B = this->comvar->f[1][1] + (this->comvar->ro1[2] - this->comvar->ro1[1])*A;
	C = this->comvar->f[0][2];

	if(sqEqv(A, B, C, &x1, &x2)==2.0) 
	{
		printf("\n!!!!!!!!!!!!!!WARN D<0.0\n");
		return 1;
	}
	
	this->comvar->ro1[3] = this->comvar->ro1[2] + x2;
	this->eilerEq1(&this->comvar->f[0][3], &this->comvar->r1[3], &this->comvar->r2[3], &this->comvar->s2[3], this->comvar->ro1[3], &this->comvar->ro2[3], log);

	this->comvar->ro1[4] = this->comvar->ro1[2] + x1;
	this->eilerEq1(&this->comvar->f[0][4], &this->comvar->r1[4], &this->comvar->r2[4], &this->comvar->s2[4], this->comvar->ro1[4], &this->comvar->ro2[4], log);

	if(fabs(this->comvar->f[0][3])>=fabs(this->comvar->f[0][4]))
	{
		this->comvar->ro1[3] = this->comvar->ro1[4];
		this->comvar->ro2[3] = this->comvar->ro2[4];
		this->comvar->r1[3] = this->comvar->r1[4];
		this->comvar->r2[3] = this->comvar->r2[4];
		this->comvar->s2[3] = this->comvar->s2[4];
		this->comvar->f[0][3] = this->comvar->f[0][4];
	}
	this->comvar->last = 3;

	return 0;
}

int sPorabola::comComp1(double p1, double p2, int log)
{
	this->comvar->ro1[0] = p1;
	this->eilerEq1(&this->comvar->f[0][0], &this->comvar->r1[0], &this->comvar->r2[0], &this->comvar->s2[0], this->comvar->ro1[0], &this->comvar->ro2[0], log);

	this->comvar->ro1[1] = p2;
	this->eilerEq1(&this->comvar->f[0][1], &this->comvar->r1[1], &this->comvar->r2[1], &this->comvar->s2[1], this->comvar->ro1[1], &this->comvar->ro2[1], log);

	this->comvar->f[1][0] = (this->comvar->f[0][1] - this->comvar->f[0][0])/(this->comvar->ro1[1] - this->comvar->ro1[0]);
	this->comvar->ro1[2] = this->comvar->ro1[1] - this->comvar->f[0][1]/this->comvar->f[1][0];
	this->eilerEq1(&this->comvar->f[0][2], &this->comvar->r1[2], &this->comvar->r2[2], &this->comvar->s2[2], this->comvar->ro1[2], &this->comvar->ro2[2], log);

	this->comvar->f[1][1] = (this->comvar->f[0][2] - this->comvar->f[0][1])/(this->comvar->ro1[2] - this->comvar->ro1[1]);
	this->comvar->f[2][0] = (this->comvar->f[1][1] - this->comvar->f[1][0])/(this->comvar->ro1[2] - this->comvar->ro1[0]);

	this->comvar->last = 2;
/*
	double A, B, C, x1, x2;

	A = this->comvar->f[2][0];
	B = this->comvar->f[1][1] + (this->comvar->ro1[2] - this->comvar->ro1[1])*A;
	C = this->comvar->f[0][2];

	if(sqEqv(A, B, C, &x1, &x2)==2.0) 
	{
		printf("\n!!!!!!!!!!!!!!WARN D<0.0\n");
		return 1;
	}
	
	this->comvar->ro1[3] = this->comvar->ro1[2] + x2;
	this->eilerEq1(&this->comvar->f[0][3], &this->comvar->r1[3], &this->comvar->r2[3], &this->comvar->s2[3], this->comvar->ro1[3], &this->comvar->ro2[3], log);

	this->comvar->ro1[4] = this->comvar->ro1[2] + x1;
	this->eilerEq1(&this->comvar->f[0][4], &this->comvar->r1[4], &this->comvar->r2[4], &this->comvar->s2[4], this->comvar->ro1[4], &this->comvar->ro2[4], log);

	if(fabs(this->comvar->f[0][3])>=fabs(this->comvar->f[0][4]))
	{
		this->comvar->ro1[3] = this->comvar->ro1[4];
		this->comvar->ro2[3] = this->comvar->ro2[4];
		this->comvar->r1[3] = this->comvar->r1[4];
		this->comvar->r2[3] = this->comvar->r2[4];
		this->comvar->s2[3] = this->comvar->s2[4];
		this->comvar->f[0][3] = this->comvar->f[0][4];
	}

	this->comvar->last = 3;
*/
	return 0;
}

int sPorabola::comComp2(int n1, int n2, int log)
{
/*	this->comvar->ro1[0] = p1;
	this->eilerEq1(&this->comvar->f[0][0], &this->comvar->r1[0], &this->comvar->r2[0], &this->comvar->s2[0], this->comvar->ro1[0], &this->comvar->ro2[0], log);

	this->comvar->ro1[1] = p2;
	this->eilerEq1(&this->comvar->f[0][1], &this->comvar->r1[1], &this->comvar->r2[1], &this->comvar->s2[1], this->comvar->ro1[1], &this->comvar->ro2[1], log);
*/
	double dpodf, tochn;

	tochn = 2.1e-5;

	int n;
	n = n2;

	dpodf = (this->comvar->ro1[n1] - this->comvar->ro1[n2])/(this->comvar->f[0][n1] - this->comvar->f[0][n2]);

	this->eilerEq1(&this->comvar->f[0][n2], &this->comvar->r1[n2], &this->comvar->r2[n2], &this->comvar->s2[n2], this->comvar->ro1[n2], &this->comvar->ro2[n2], log);

	if(log) fprintf(this->FLOG, "\ndpodf	%f\n", dpodf);

	do
	{
		this->comvar->ro1[n+1] = this->comvar->ro1[n] - this->comvar->f[0][n]*dpodf;
		this->eilerEq1(&this->comvar->f[0][n+1], &this->comvar->r1[n+1], &this->comvar->r2[n+1], &this->comvar->s2[n+1], this->comvar->ro1[n+1], &this->comvar->ro2[n+1], log);
		n++;
	}while(fabs(this->comvar->f[0][n] - this->comvar->f[0][n-1])>tochn);

	

	this->comvar->last = n;

	return 0;
}

int sPorabola::PSN(int log)
{
	double x1, y1, z1;
	double x, y, z;
	double x2, y2, z2;
	int n = this->comvar->last;

	note *s1, *s2, *s;
	s1 = new note;
	s2 = new note;
	s = new note;

	this->sunpos->getNote(s1, 0);
	this->sunpos->getNote(s, 1);
	this->sunpos->getNote(s2, 2);

	x1 = this->appcnt->l1*this->comvar->ro1[n] - s1->x;
	y1 = this->appcnt->m1*this->comvar->ro1[n] - s1->y;
	z1 = this->appcnt->v1*this->comvar->ro1[n] - s1->z;

	x2 = this->appcnt->l2*this->comvar->ro2[n] - s2->x;
	y2 = this->appcnt->m2*this->comvar->ro2[n] - s2->y;
	z2 = this->appcnt->v2*this->comvar->ro2[n] - s2->z;

	double n1, n2;
	n1 = this->comvar->n1/this->comvar->n2;
	n2 = 1.0/this->comvar->n2;

	this->comvar->x = x = n1*x1 + n2*x2;
	this->comvar->y = y = n1*y1 + n2*y2;
	this->comvar->z = z = n1*z1 + n2*z2;

	if(log)
	{
		fprintf(this->FLOG, "\n////////////////////////////////////////\n\nPREDSTAVLENIE SREDNEGO NABLIUDENIYA\n");
		fprintf(this->FLOG, "\nx1				%f\ny1				%f\nz1				%f", x1, y1, z1);
		fprintf(this->FLOG, "\nx				%f\ny				%f\nz				%f", x, y, z);
		fprintf(this->FLOG, "\nx2				%f\ny2				%f\nz2				%f", x2, y2, z2);
		fprintf(this->FLOG, "\nr12\t\t\t\t%f\nr22\t\t\t\t%f\n", x1*x1 + y1*y1 + z1*z1, x2*x2 + y2*y2 + z2*z2);
	}

	double r1, d1, r, d, r2, d2;
	double P1, P2, P3;

//1
	P1 = x1 + s1->x;
	P2 = y1 + s1->y;
	P3 = z1 + s1->z;

	rdsys(&r1, &d1, P1, P2, P3);
//
//2
	P1 = x + s->x;
	P2 = y + s->y;
	P3 = z + s->z;

	if(log) fprintf(this->FLOG, "\nSREDNEE\n");
	rdsys(&r, &d, P1, P2, P3);
/*	if(log)
	{
		fprintf(this->FLOG, "\nl_init	%f", this->appcnt->l);
		fprintf(this->FLOG, "\nm_init	%f", this->appcnt->m);
		fprintf(this->FLOG, "\nv_init	%f\n", this->appcnt->v);
	}*/
//
//3
	P1 = x2 + s2->x;
	P2 = y2 + s2->y;
	P3 = z2 + s2->z;

	rdsys(&r2, &d2, P1, P2, P3);
//

	app *vid;
	vid = new app;
	this->apppos->getApp(vid, 1);

	this->comvar->psnaccR = fabs(r - vid->r);
	this->comvar->psnaccD = fabs(d - vid->d);

	if(log)
	{
		fprintf(this->FLOG, "\ndRA''\t\t\t\t%e\ndDEC''\t\t\t\t%e\n", this->comvar->psnaccR*206265.0, this->comvar->psnaccD*206265.0);
	}

	this->comvar->x1 = x1;
	this->comvar->y1 = y1;
	this->comvar->z1 = z1;

	this->comvar->x2 = x2;
	this->comvar->y2 = y2;
	this->comvar->z2 = z2;

//vozvrat

	if(log)
	{
		fprintf(this->FLOG, "\n\nVOZVRAT\n\n");
	}

	double v1, m1, l1;
	double v, m, l;
	double v2, m2, l2;
	double K, L1, L2, L3;

	l1 = cos(d1)*cos(r1);
	m1 = cos(d1)*sin(r1);
	v1 = sin(d1);

	l = cos(d)*cos(r);
	m = cos(d)*sin(r);
	v = sin(d);

	l2 = cos(d2)*cos(r2);
	m2 = cos(d2)*sin(r2);
	v2 = sin(d2);

	double zn1, zn2, zn3;

	zn1 = (l*m2 - m*l2);
	zn2 = (l*v2 - v*l2);
	zn3 = (m*v2 - v*m2);

	if(log)
	{
		fprintf(this->FLOG, "\nzn1				%f", zn1);
		fprintf(this->FLOG, "\nzn2				%f", zn2);
		fprintf(this->FLOG, "\nzn3				%f\n", zn3);
	}

	if((fabs(zn1)>fabs(zn2))&&(fabs(zn1)>fabs(zn3)))
	{
		K = -(l*m1 - m*l1)/zn1;
		L1 = (l*s1->y - m*s1->x)/zn1;
		L2 = -(l*s->y - m*s->x)/zn1;
		L3 = (l*s2->y - m*s2->x)/zn1;
	}

	if((fabs(zn2)>fabs(zn1))&&(fabs(zn2)>fabs(zn3)))
	{
		K = -(l*v1 - v*l1)/zn2;
		L1 = (l*s1->z - v*s1->x)/zn2;
		L2 = -(l*s->z - v*s->x)/zn2;
		L3 = (l*s2->z - v*s2->x)/zn2;
	}

	if((fabs(zn3)>fabs(zn1))&&(fabs(zn3)>fabs(zn2)))
	{
		K = -(m*v1 - v*m1)/zn3;
		L1 = (m*s1->z - v*s1->y)/zn3;
		L2 = -(m*s->z - v*s->y)/zn3;
		L3 = (m*s2->z - v*s2->y)/zn3;
	}

	if(log)
	{
		fprintf(this->FLOG, "\nK				%f	%f", K, this->comvar->K);
		fprintf(this->FLOG, "\nL1				%f	%f", L1, this->comvar->L1);
		fprintf(this->FLOG, "\nL2				%f	%f", L2, this->comvar->L2);
		fprintf(this->FLOG, "\nL3				%f	%f\n", L3, this->comvar->L3);
	}
/*
	this->comvar->tau = ka*(this->appcnt->t2 - this->appcnt->t1);

	if(log)
	{
		fprintf(this->FLOG, "\ntau				%f", this->comvar->tau);
		fprintf(this->FLOG, "\n2(tau)^2			%f", 2.0*(this->comvar->tau)*(this->comvar->tau));
	}
*/
	return 0;
}

int sPorabola::countOrbBan(int log)
{
	if(log) fprintf(this->FLOG, "\n////////////////////////////////////////\n\nCOUNT ORBIT BAN\n");

	double r1, r2, s;

	r1 = sqrt(this->comvar->x1*this->comvar->x1 + this->comvar->y1*this->comvar->y1 + this->comvar->z1*this->comvar->z1);
	r2 = sqrt(this->comvar->x2*this->comvar->x2 + this->comvar->y2*this->comvar->y2 + this->comvar->z2*this->comvar->z2);
	s = sqrt((this->comvar->x2 - this->comvar->x1)*(this->comvar->x2 - this->comvar->x1) + (this->comvar->y2 - this->comvar->y1)*(this->comvar->y2 - this->comvar->y1) + (this->comvar->z2 - this->comvar->z1)*(this->comvar->z2 - this->comvar->z1));

	double b, g;

	b = asin((r2 - r1)/s);
	g = asin(s/(r1+r2));

	double sg2, sg1;

	sg1 = tan(b) - tan(g/2.0)/cos(b);
	sg2 = tan(b) + tan(g/2.0)/cos(b);

	double q;

	q = ((r1/(1.0 + sg1*sg1))+(r2/(1.0 + sg2*sg2)))/2.0;

	double B1, B2, T;

	B1 = sqrt(2.0)*(sg1 + (sg1*sg1*sg1)/3.0)/ka;
	B2 = sqrt(2.0)*(sg2 + (sg2*sg2*sg2)/3.0)/ka;

	T = ((this->appcnt->t1 - pow(q, 1.5)*B1)+(this->appcnt->t2 - pow(q, 1.5)*B2))/2.0;

	double A, B, C, D;
	double Px, Py, Pz, Qx, Qy, Qz;
	double a00, a01;
	double a10, a11;
	double a20, a21;

	A = 1.0/((r1 + r2)*cos(g));
	B = A*tan(b);
	C = (sin(b)/cos(g) + sin(b))/s;
	D = (sin(b)/cos(g) - cos(b))/s;

	a00 = this->comvar->x1 + this->comvar->x2;
	a01 = this->comvar->x1 - this->comvar->x2;

	a10 = this->comvar->y1 + this->comvar->y2;
	a11 = this->comvar->y1 - this->comvar->y2;

	a20 = this->comvar->z1 + this->comvar->z2;
	a21 = this->comvar->z1 - this->comvar->z2;

	Px = a00*A + a01*C;
	Qx = a00*B + a01*D;

	Py = a10*A + a11*C;
	Qy = a10*B + a11*D;

	Pz = a20*A + a21*C;
	Qz = a20*B + a21*D;

	this->comvar->mx = q*Px;
	this->comvar->my = q*Py;
	this->comvar->mz = q*Pz;

	this->comvar->nx = 2.0*q*Qx;
	this->comvar->ny = 2.0*q*Qy;
	this->comvar->nz = 2.0*q*Qz;

	double P1, P2, P3, P4, L5;
	double se, ce, sw, cw;
	double w, Om, inc;

	se = sin(EKV);
	ce = sin(EKV);

	P1 = Pz*ce - Py*se;
	P2 = Qz*ce - Qy*se;

	w = atan2(P1, P2);
	if(w<0.0) w += 2.0*PI;

	sw = sin(w);
	cw = cos(w);

	P3 = (Py*cw - Qy*sw)/ce;
	P4 = Px*cw - Qx*sw;

	Om = atan2(P3, P4);
	if(Om<0.0) Om += 2.0*PI;

	L5 = -(Px*sw + Qx*cw)/sin(Om);

	P1 /= sw;

	inc = atan2(P1, L5);
	if(inc<0.0) inc += 2.0*PI;

	if(log)
	{
		int year, mth;
		double day;
		dat2YMD(T, &year, &mth, &day);
		fprintf(this->FLOG, "\nT	%d %d %f", year, mth, day);
		fprintf(this->FLOG, "\nq	%f", q);
		fprintf(this->FLOG, "\nw	%f", w*180.0/PI);
		fprintf(this->FLOG, "\nNode	%f", Om*180.0/PI);
		fprintf(this->FLOG, "\ni	%f", inc*180.0/PI);
	}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	double M0, eJD, ss;
	M0 = 0.0;
	eJD = (double)(int)T;
	T = T-eJD;
	B = pow(q, -1.5)*T;
	pKepler(&ss, B);
	M0 = 2.0*atan(ss);
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	this->objorb = new orbit;

	this->objorb->initElem(eJD, M0, w, Om, inc, 1.0, q);

//PN
	double sg;
	double r, d, sign;
	note *sn;
	sn = new note;
	this->sunpos->getNote(sn, 1);

	sign = 1.0;
	B = pow(q, -1.5)*(this->appcnt->t - T);


	if(B<0.0) 
	{
		B = fabs(B);
		sign = -1.0;
	}
	pKepler(&sg, B);
	sg = sign*sg;

	P1 = this->comvar->mx*(1.0 - sg*sg) + this->comvar->nx*sg + sn->x;
	P2 = this->comvar->my*(1.0 - sg*sg) + this->comvar->ny*sg + sn->y;
	P3 = this->comvar->mz*(1.0 - sg*sg) + this->comvar->nz*sg + sn->z;

	double xx1, yy1, zz1;//, r1, d1;
	this->objorb->detRecEcl(&xx1, &yy1, &zz1, this->appcnt->t);
/*
	xx1 += sn->x;
	yy1 += sn->y;
	zz1 += sn->z;
*/
	rdsys(&r, &d, P1, P2, P3);

//	rdsys(&r1, &d1, xx1, yy1, zz1);

	app *vid;
	vid = new app;
	this->apppos->getApp(vid, 1);

	this->comvar->psnaccR = fabs(r - vid->r);
	this->comvar->psnaccD = fabs(d - vid->d);

	if(log)
	{
		fprintf(this->FLOG, "\n\n!!!!!!Okonchatel'nye oshibki polozgeniya srednego:\ndR''\t\t\t\t%e\ndD''\t\t\t\t%e\ndelta\t%e", this->comvar->psnaccR*206265.0, this->comvar->psnaccD*206265.0, sqrt(this->comvar->psnaccR*206265.0*this->comvar->psnaccR*206265.0 + this->comvar->psnaccD*206265.0*this->comvar->psnaccD*206265.0));
	}


	return 0;
}

int sPorabola::countOrb(int log)
{
	double M0, eJD, ss, dT, Sign, TT, nn;

	if(log) fprintf(this->FLOG, "\n////////////////////////////////////////\n\nCOUNT ORBIT\n");

	int n = this->comvar->last;
	double sg1, sg2, sg;

	double Ex1x2;

	Ex1x2 = this->comvar->x1*this->comvar->x2 + this->comvar->y1*this->comvar->y2 + this->comvar->z1*this->comvar->z2;

	sg1 = Ex1x2/(this->comvar->r1[n]*this->comvar->r1[n]);
//	sg2 = (1.0 + (this->comvar->r2[n]*this->comvar->r2[n] - this->comvar->s2[n])/(this->comvar->r1[n]*this->comvar->r1[n]))/2.0;
//	if(fabs(sg1 - sg2)>EPS) return 1;
	sg = sg1;

	if(log)
	{
		fprintf(this->FLOG, "\nE(x1x2)\t\t\t\t%f", Ex1x2);
		fprintf(this->FLOG, "\nsg\t\t\t\t%f", sg);
	}

	double x0, y0, z0, r0;

	x0 = this->comvar->x2 - sg*this->comvar->x1;
	y0 = this->comvar->y2 - sg*this->comvar->y1;
	z0 = this->comvar->z2 - sg*this->comvar->z1;
	r0 = x0*x0 + y0*y0 + z0*z0;

	if(log)
	{
		fprintf(this->FLOG, "\nx0\t\t\t\t%f", x0);
		fprintf(this->FLOG, "\ny0\t\t\t\t%f", y0);
		fprintf(this->FLOG, "\nz0\t\t\t\t%f", z0);
		fprintf(this->FLOG, "\nr0^2\t\t\t\t%f", r0);
	}

	double r1r02, r1r0;

	r1r02 = r0*this->comvar->r1[n]*this->comvar->r1[n];
	r1r0 = sqrt(r1r02);

	r0 = sqrt(r0);

	if(log)
	{
		fprintf(this->FLOG, "\ns\t\t\t\t%f", sqrt(this->comvar->s2[n]));
		fprintf(this->FLOG, "\nr1\t\t\t\t%f", this->comvar->r1[n]);
		fprintf(this->FLOG, "\nr2\t\t\t\t%f", this->comvar->r2[n]);
		fprintf(this->FLOG, "\nr0\t\t\t\t%f", r0);
		fprintf(this->FLOG, "\n(r1r0)^2\t\t\t%f", r1r02);
		fprintf(this->FLOG, "\nr1r0	\t\t\t%f\n", r1r0);

	}
	


	double f, f1, tg2f, f2, sinf2, spr1dr2, sdr1pr2, ch, zn, sinf;

	tg2f = r1r0/Ex1x2;

	f2 = atan2(r1r0, Ex1x2);


	spr1dr2 = sqrt(this->comvar->s2[n]) + this->comvar->r1[n] - this->comvar->r2[n];
	sdr1pr2 = sqrt(this->comvar->s2[n]) - this->comvar->r1[n] + this->comvar->r2[n];

	ch = spr1dr2*sdr1pr2;
	zn = this->comvar->r1[n]*this->comvar->r2[n];

	sinf2 = sqrt(ch/zn);
	sinf = sinf2/2.0;

	f1 = asin(sinf);

	f = f2/2.0;

	if(log)
	{
		fprintf(this->FLOG, "\ntg2f\t\t\t\t%f", tg2f);
		fprintf(this->FLOG, "\n2f\t\t\t\t%f", f2);
		fprintf(this->FLOG, "\nf\t\t\t\t%f", f*180.0/PI);

		fprintf(this->FLOG, "\ns+r1-r2\t\t\t\t%f", spr1dr2);
		fprintf(this->FLOG, "\ns-r1+r2\t\t\t\t%f", sdr1pr2);
		fprintf(this->FLOG, "\nchisl\t\t\t\t%f", ch);
		fprintf(this->FLOG, "\nznam\t\t\t\t%f", zn);
		fprintf(this->FLOG, "\nch/zn\t\t\t\t%f", ch/zn);
		fprintf(this->FLOG, "\n2sinf\t\t\t\t%f", sinf2);
		fprintf(this->FLOG, "\nsinf\t\t\t\t%f", sinf);
		fprintf(this->FLOG, "\nf1\t\t\t\t%f\n", f1*180.0/PI);

	}

/*
	f1 = fatan(r0, sg*this->comvar->r1[n])/2.0;
	f2 = asin(sqrt((sqrt(this->comvar->s2[n]) + this->comvar->r1[n] - this->comvar->r2[n])*(sqrt(this->comvar->s2[n]) - this->comvar->r1[n] + this->comvar->r2[n])/(this->comvar->r1[n]*this->comvar->r2[n]))/2.0);
	if(fabs(f1 - f2)>EPS) return 2;
	f = f1;
*/
	

	double tgv1, r1dr2, cosf;

	r1dr2 = this->comvar->r1[n]/this->comvar->r2[n];

	if(log) fprintf(this->FLOG, "\nr1/r2\t\t\t\t%f", r1dr2);

	r1dr2 = sqrt(r1dr2);

	if(log) fprintf(this->FLOG, "\nsqrt(r1/r2)\t\t\t%f", r1dr2);

	cosf = cos(f);

	sg1 = tgv1 = (cosf - r1dr2)/sin(f);

	double v1, v2, v1d2, v2d2;

	v1 = 2.0*atan2((cosf - r1dr2), sin(f));
	v2 = v1 + 2.0*f;

	v1d2 = v1/2.0;
	v2d2 = v2/2.0;

	sg2 = tan(v2d2);

	double q1, q2, q, cosv1d2, cosv2d2;

	cosv1d2 = cos(v1d2);
	cosv2d2 = cos(v2d2);

	q1 = this->comvar->r1[n]*cosv1d2*cosv1d2;
	q2 = this->comvar->r2[n]*cosv2d2*cosv2d2;
	q = (q1 + q2)/2.0;

	if(log)
	{
		fprintf(this->FLOG, "\ncosf\t\t\t\t%f", cosf);
		fprintf(this->FLOG, "\ncosf-q(r1/r2)\t\t\t%f", cosf - r1dr2);
		fprintf(this->FLOG, "\nsinf\t\t\t\t%f", sin(f));
		fprintf(this->FLOG, "\nsg1\t\t\t\t%f", sg1);
		fprintf(this->FLOG, "\nv1/2\t\t\t\t%f", v1d2*180.0/PI);
		fprintf(this->FLOG, "\nv1\t\t\t\t%f", v1*180.0/PI);
		fprintf(this->FLOG, "\nv2\t\t\t\t%f", v2*180.0/PI);
		fprintf(this->FLOG, "\nv2/2\t\t\t\t%f", v2d2*180.0/PI);
		fprintf(this->FLOG, "\nsg2\t\t\t\t%f", sg2);

	}

	double B1, B2, B;

	B1 = sqrt(2.0)*(sg1 + sg1*sg1*sg1/3.0)/ka;
	B2 = sqrt(2.0)*(sg2 + sg2*sg2*sg2/3.0)/ka;

	B = (B1+B2)/2.0;

	double T1, T2, T, B1q, B2q;

	B1q = - pow(q, 1.5)*B1;
	B2q = - pow(q, 1.5)*B2;

	T1 = this->appcnt->t1 + B1q;
	T2 = this->appcnt->t2 + B2q;

	T = (T1+T2)/2.0;

	if(log)
	{
		fprintf(this->FLOG, "\nB1\t\t\t\t%f", B1);
		fprintf(this->FLOG, "\nB2\t\t\t\t%f", B2);
		fprintf(this->FLOG, "\ncos(v1/2)\t\t\t%f", cosv1d2);
		fprintf(this->FLOG, "\ncos(v2/2)\t\t\t%f", cosv2d2);
		fprintf(this->FLOG, "\ncos(v1/2)^2\t\t\t%f", cosv1d2*cosv1d2);
		fprintf(this->FLOG, "\ncos(v2/2)^2\t\t\t%f", cosv2d2*cosv2d2);
		fprintf(this->FLOG, "\nq1\t\t\t\t%f", q1);
		fprintf(this->FLOG, "\nq2\t\t\t\t%f", q2);
		fprintf(this->FLOG, "\nq\t\t\t\t%f", q);
		fprintf(this->FLOG, "\nB1q\t\t\t\t%f", B1q);
		fprintf(this->FLOG, "\nB2q\t\t\t\t%f", B2q);
		fprintf(this->FLOG, "\nT1\t\t\t\t%f", T1);
		fprintf(this->FLOG, "\nT2\t\t\t\t%f", T2);
		fprintf(this->FLOG, "\nT\t\t\t\t%f", T);

	}


//PQ
	double Px, Py, Pz, Qx, Qy, Qz;
	double a00, a01, a10, a11;

	a00 = cos(v1)/this->comvar->r1[n];
	a01 = sin(v1)/this->comvar->r1[n];
	
	a10 = -sin(v1)/r0;
	a11 = cos(v1)/r0;

	if(log)
	{
		fprintf(this->FLOG, "\n\na00\t\t\t\t%f\na01\t\t\t\t%f", a00, a01);
		fprintf(this->FLOG, "\na10\t\t\t\t%f\na11\t\t\t\t%f\n", a10, a11);
		
	}

	Px = this->comvar->x1*a00 + x0*a10;
	Py = this->comvar->y1*a00 + y0*a10;
	Pz = this->comvar->z1*a00 + z0*a10;

	Qx = this->comvar->x1*a01 + x0*a11;
	Qy = this->comvar->y1*a01 + y0*a11;
	Qz = this->comvar->z1*a01 + z0*a11;

	if(log)
	{
		fprintf(this->FLOG, "\nPx\t\t\t\t%f\nPy\t\t\t\t%f\nPz\t\t\t\t%f", Px, Py, Pz);
		fprintf(this->FLOG, "\nQx\t\t\t\t%f\nQy\t\t\t\t%f\nQz\t\t\t\t%f", Qx, Qy, Qz);
	}

//	double mx, my, mz, nx, ny, nz;

	this->comvar->mx = q*Px;
	this->comvar->my = q*Py;
	this->comvar->mz = q*Pz;

	this->comvar->nx = 2.0*q*Qx;
	this->comvar->ny = 2.0*q*Qy;
	this->comvar->nz = 2.0*q*Qz;

	if((fabs(this->comvar->mx*this->comvar->mx + this->comvar->my*this->comvar->my + this->comvar->mz*this->comvar->mz - q*q)>EPS)||(fabs(this->comvar->nx*this->comvar->nx + this->comvar->ny*this->comvar->ny + this->comvar->nz*this->comvar->nz - 4.0*q*q)>EPS)||(fabs(this->comvar->mx*this->comvar->nx + this->comvar->my*this->comvar->ny + this->comvar->mz*this->comvar->nz)>EPS)) return 3;

//1.11
	double P1, P2, P3, P4, L5, tgw;
	double se, ce, sw, cw;
	double w, Om, inc;

	se = sin(EKV);
	ce = cos(EKV);

	P1 = Pz*ce - Py*se;
	P2 = Qz*ce - Qy*se;

	tgw = P1/P2;

	w = atan2(P1, P2);
	if(w<0.0) w += 2.0*PI;

	sw = sin(w);
	cw = cos(w);

	if(log)
	{
		fprintf(this->FLOG, "\nsine\t\t\t\t%f", se);
		fprintf(this->FLOG, "\ncose\t\t\t\t%f", ce);
		fprintf(this->FLOG, "\nsinisinw\t\t\t%f", P1);
		fprintf(this->FLOG, "\nsinicosw\t\t\t%f", P2);
		fprintf(this->FLOG, "\ntgw\t\t\t\t%f", tgw);
		fprintf(this->FLOG, "\nw\t\t\t\t%f", w*180.0/PI);
		fprintf(this->FLOG, "\ncosw\t\t\t\t%f", cw);
		fprintf(this->FLOG, "\nsinw\t\t\t\t%f", sw);
	}

	P3 = (Py*cw - Qy*sw)/ce;
	P4 = Px*cw - Qx*sw;

	Om = atan2(P3, P4);
	if(Om<0.0) Om += 2.0*PI;

	L5 = -(Px*sw + Qx*cw)/sin(Om);

	P1 /= sw;

	inc = atan2(P1, L5);
	if(inc<0.0) inc += 2.0*PI;

	if(log)
	{
		fprintf(this->FLOG, "\nsinN\t\t\t\t%f", P3);
		fprintf(this->FLOG, "\ncosN\t\t\t\t%f", P4);
		fprintf(this->FLOG, "\nN\t\t\t\t%f", Om*180.0/PI);
		fprintf(this->FLOG, "\ncosi\t\t\t\t%f", L5);
		fprintf(this->FLOG, "\nsini\t\t\t\t%f", P1);
		fprintf(this->FLOG, "\ni\t\t\t\t%f", inc*180.0/PI);
	}

//

	if(log)
	{
		int year, mth;
		double day;
		dat2YMD(T, &year, &mth, &day);
		fprintf(this->FLOG, "\n\nT\t\t\t\t%f\t%d %d %f", T, year, mth, day);
		fprintf(this->FLOG, "\nq\t\t\t\t%f", q);
		fprintf(this->FLOG, "\nw\t\t\t\t%f", w*180.0/PI);
		fprintf(this->FLOG, "\nNode\t\t\t\t%f", Om*180.0/PI);
		fprintf(this->FLOG, "\ni\t\t\t\t%f", inc*180.0/PI);
	}
/*
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	double M0, eJD, ss, TT, sign;
	M0 = 0.0;
	eJD = (double)(int)T;
//	eJD = TT;
	TT = T-eJD;
	B = pow(q, -1.5)*TT;
	pKepler(&ss, B);
	M0 = 2.0*atan(ss);
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	double M0, eJD, ss, dT, sign;
	M0 = 0.0;
	sign = 1.0;
//	eJD = (double)(int)this->appcnt->t;
	T*=2.0;
	T+=chet(T);
	eJD = ((double)(int)T)/2.0;
	T/=2.0;
	dT = eJD - T;
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
*/
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	M0 = 0.0;
	Sign = 1.0;
//	eJD = (double)(int)this->appcnt->t;
	TT=T*2.0;
	TT-=chet(TT);
	eJD = ((double)(int)TT)/2.0;
	dT = eJD - T;
//	dT = TT - eJD;

	nn = ka*pow(q, -1.5)/sqrt(2.0);

	M0 = nn*dT;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	this->objorb = new orbit;

	this->objorb->initElem(eJD, M0, w, Om, inc, 1.0, q);


//
//PN
	double r, d;
	note *s;
	s = new note;
	this->sunpos->getNote(s, 1);

	Sign = 1.0;
	B = pow(q, -1.5)*(this->appcnt->t - T);


	if(B<0.0) 
	{
		B = fabs(B);
		Sign = -1.0;
	}
	pKepler(&sg, B);
	sg = Sign*sg;

	P1 = this->comvar->mx*(1.0 - sg*sg) + this->comvar->nx*sg;
	P2 = this->comvar->my*(1.0 - sg*sg) + this->comvar->ny*sg;
	P3 = this->comvar->mz*(1.0 - sg*sg) + this->comvar->nz*sg;

	P1 += s->x;
	P2 += s->y;
	P3 += s->z;

	double xx1, yy1, zz1, r1, d1;
	this->objorb->detRecEcl(&xx1, &yy1, &zz1, this->appcnt->t);

	xx1 += s->x;
	yy1 += s->y;
	zz1 += s->z;

	rdsys(&r, &d, P1, P2, P3);

	rdsys(&r1, &d1, xx1, yy1, zz1);

	app *vid;
	vid = new app;
	this->apppos->getApp(vid, 1);

	this->comvar->psaccR = fabs(r - vid->r);
	this->comvar->psaccD = fabs(d - vid->d);

	if(log)
	{
		fprintf(this->FLOG, "\n\n!!!!!!Okonchatel'nye oshibki polozgeniya srednego:\ndR''\t\t\t\t%e\ndD''\t\t\t\t%e\ndelta\t\t\t\t%e\n", this->comvar->psaccR*206265.0, this->comvar->psaccD*206265.0, sqrt(this->comvar->psnaccR*206265.0*this->comvar->psnaccR*206265.0 + this->comvar->psnaccD*206265.0*this->comvar->psnaccD*206265.0));
	}

	return 0;
}

int sPorabola::countOrb1(int log)
{
	if(log) fprintf(this->FLOG, "\n////////////////////////////////////////\n\nCOUNT ORBIT\n");

	int n = this->comvar->last;
	double sg1, sg2, sg;

	double q, Node, inc, p, ecc, w, v;

	double r1, r2, r1r2, x12, y12, z12, f;
	double P1, P2, P3;

	x12 = this->comvar->y1*this->comvar->z1 - this->comvar->y2*this->comvar->z1;
	y12 = this->comvar->x1*this->comvar->z2 - this->comvar->x2*this->comvar->z1;
	z12 = this->comvar->x1*this->comvar->y2 - this->comvar->x2*this->comvar->y1;

	P1 = x12;
	P2 = y12*cos(EKV) - z12*sin(EKV);
	P3 = y12*sin(EKV) + z12*cos(EKV);

	r1 = this->comvar->x1*this->comvar->x1 + this->comvar->y1*this->comvar->y1 + this->comvar->z1*this->comvar->z1;
	r2 = this->comvar->x2*this->comvar->x2 + this->comvar->y2*this->comvar->y2 + this->comvar->z2*this->comvar->z2;

//	piNsys(&p, &inc, &Node, x12, y12, z12);

	Node = atan2(P1, P2);
	if(Node<0.0) Node += 2.0*PI;
	inc = atan2(P2, P3*cos(Node));
	if(inc<0.0) inc += 2.0*PI;

	if((fabs(inc)>0.1)&&(fabs(Node)>0.1)) r1r2 = P1/(sin(inc)*sin(Node));
	else if((fabs(inc)>0.1)&&(fabs(Node)<0.1)) r1r2 = P2/(sin(inc)*cos(Node));
	else if(fabs(inc-PI/2.0)>0.1) r1r2 = P3/cos(inc);

	f = asin(r1r2/(r1*r2))/2.0;

//	f = 2.0*PI - f;


	double v1, v2;

	v1 = atan2(1.0/tan(f) - sqrt(r1/r2)/sin(f), 1.0);
	v2 = v1 + 2.0*f;

	if(fabs(v1-PI)>0.1) q = r1*pow(cos(v1/2.0), 2.0);
	else q = (r1*sin(v1/2.0))/(1.0/tan(f) - sqrt(r1/r2)/sin(f));

	if(fabs(inc)<0.1) P1 = (this->comvar->y1*cos(Node) - this->comvar->x1*sin(Node))/cos(inc);
	else P1 = this->comvar->z1/sin(inc);

	P2 = this->comvar->x1*cos(Node) + this->comvar->y1*sin(Node);

	double ui = atan2(P1, P2);

	w = ui - v1;
	if(w<0.0) w += 2.0*PI;

	double T1, T2, T, B1, B2, B;

	sg1 = tan(v1/2.0);
	B1 = sg1 + sg1*sg1*sg1/3.0;
	T1 = this->appcnt->t1 - pow(q, 1.5)*B1;

	sg2 = tan(v2/2.0);
	B2 = sg2 + sg2*sg2*sg2/3.0;
	T2 = this->appcnt->t2 - pow(q, 1.5)*B2;
	















//PQ
/*	double Px, Py, Pz, Qx, Qy, Qz;
	double a00, a01, a10, a11;

	a00 = cos(v1)/this->comvar->r1[n];
	a01 = sin(v1)/this->comvar->r1[n];
	
	a10 = -sin(v1)/r0;
	a11 = cos(v1)/r0;

	if(log)
	{
		fprintf(this->FLOG, "\n\na00\t\t\t\t%f\na01\t\t\t\t%f", a00, a01);
		fprintf(this->FLOG, "\na10\t\t\t\t%f\na11\t\t\t\t%f\n", a10, a11);
		
	}

	Px = this->comvar->x1*a00 + x0*a10;
	Py = this->comvar->y1*a00 + y0*a10;
	Pz = this->comvar->z1*a00 + z0*a10;

	Qx = this->comvar->x1*a01 + x0*a11;
	Qy = this->comvar->y1*a01 + y0*a11;
	Qz = this->comvar->z1*a01 + z0*a11;

	if(log)
	{
		fprintf(this->FLOG, "\nPx\t\t\t\t%f\nPy\t\t\t\t%f\nPz\t\t\t\t%f", Px, Py, Pz);
		fprintf(this->FLOG, "\nQx\t\t\t\t%f\nQy\t\t\t\t%f\nQz\t\t\t\t%f", Qx, Qy, Qz);
	}

//	double mx, my, mz, nx, ny, nz;

	this->comvar->mx = q*Px;
	this->comvar->my = q*Py;
	this->comvar->mz = q*Pz;

	this->comvar->nx = 2.0*q*Qx;
	this->comvar->ny = 2.0*q*Qy;
	this->comvar->nz = 2.0*q*Qz;

	if((fabs(this->comvar->mx*this->comvar->mx + this->comvar->my*this->comvar->my + this->comvar->mz*this->comvar->mz - q*q)>EPS)||(fabs(this->comvar->nx*this->comvar->nx + this->comvar->ny*this->comvar->ny + this->comvar->nz*this->comvar->nz - 4.0*q*q)>EPS)||(fabs(this->comvar->mx*this->comvar->nx + this->comvar->my*this->comvar->ny + this->comvar->mz*this->comvar->nz)>EPS)) return 3;

//1.11
	double P1, P2, P3, P4, L5, tgw;
	double se, ce, sw, cw;
	double w, Om, inc;

	se = sin(EKV);
	ce = cos(EKV);

	P1 = Pz*ce - Py*se;
	P2 = Qz*ce - Qy*se;

	tgw = P1/P2;

	w = atan2(P1, P2);
	if(w<0.0) w += 2.0*PI;

	sw = sin(w);
	cw = cos(w);

	if(log)
	{
		fprintf(this->FLOG, "\nsine\t\t\t\t%f", se);
		fprintf(this->FLOG, "\ncose\t\t\t\t%f", ce);
		fprintf(this->FLOG, "\nsinisinw\t\t\t%f", P1);
		fprintf(this->FLOG, "\nsinicosw\t\t\t%f", P2);
		fprintf(this->FLOG, "\ntgw\t\t\t\t%f", tgw);
		fprintf(this->FLOG, "\nw\t\t\t\t%f", w*180.0/PI);
		fprintf(this->FLOG, "\ncosw\t\t\t\t%f", cw);
		fprintf(this->FLOG, "\nsinw\t\t\t\t%f", sw);
	}

	P3 = (Py*cw - Qy*sw)/ce;
	P4 = Px*cw - Qx*sw;

	Om = atan2(P3, P4);
	if(Om<0.0) Om += 2.0*PI;

	L5 = -(Px*sw + Qx*cw)/sin(Om);

	P1 /= sw;

	inc = atan2(P1, L5);
	if(inc<0.0) inc += 2.0*PI;

	if(log)
	{
		fprintf(this->FLOG, "\nsinN\t\t\t\t%f", P3);
		fprintf(this->FLOG, "\ncosN\t\t\t\t%f", P4);
		fprintf(this->FLOG, "\nN\t\t\t\t%f", Om*180.0/PI);
		fprintf(this->FLOG, "\ncosi\t\t\t\t%f", L5);
		fprintf(this->FLOG, "\nsini\t\t\t\t%f", P1);
		fprintf(this->FLOG, "\ni\t\t\t\t%f", inc*180.0/PI);
	}

//

	if(log)
	{
		int year, mth;
		double day;
		dat2YMD(T, &year, &mth, &day);
		fprintf(this->FLOG, "\n\nT\t\t\t\t%d %d %f", year, mth, day);
		fprintf(this->FLOG, "\nq\t\t\t\t%f", q);
		fprintf(this->FLOG, "\nw\t\t\t\t%f", w*180.0/PI);
		fprintf(this->FLOG, "\nNode\t\t\t\t%f", Om*180.0/PI);
		fprintf(this->FLOG, "\ni\t\t\t\t%f", inc*180.0/PI);
	}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	double M0, eJD, ss;
	M0 = 0.0;
	eJD = (double)(int)T;
	T = T-eJD;
	B = pow(q, -1.5)*T;
	pKepler(&ss, B);
	M0 = 2.0*atan(ss);
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	this->objorb = new orbit;

	this->objorb->initElem(eJD, M0, w, Om, inc, 1.0, q);


//
//PN
	double r, d, sign;
	note *s;
	s = new note;
	this->sunpos->getNote(s, 1);

	sign = 1.0;
	B = pow(q, -1.5)*(this->appcnt->t - T);


	if(B<0.0) 
	{
		B = fabs(B);
		sign = -1.0;
	}
	pKepler(&sg, B);
	sg = sign*sg;

	P1 = this->comvar->mx*(1.0 - sg*sg) + this->comvar->nx*sg + s->x;
	P2 = this->comvar->my*(1.0 - sg*sg) + this->comvar->ny*sg + s->y;
	P3 = this->comvar->mz*(1.0 - sg*sg) + this->comvar->nz*sg + s->z;

	double xx1, yy1, zz1, r1, d1;
	this->objorb->detRecEcl(&xx1, &yy1, &zz1, this->appcnt->t);

	xx1 += s->x;
	yy1 += s->y;
	zz1 += s->z;

	rdsys(&r, &d, P1, P2, P3);

	rdsys(&r1, &d1, xx1, yy1, zz1);

	app *vid;
	vid = new app;
	this->apppos->getApp(vid, 1);

	this->comvar->psnaccR = fabs(r - vid->r);
	this->comvar->psnaccD = fabs(d - vid->d);

	if(log)
	{
		fprintf(this->FLOG, "\n\n!!!!!!Okonchatel'nye oshibki polozgeniya srednego:\ndR''\t\t\t\t%e\ndD''\t\t\t\t%e\n", this->comvar->psnaccR*206265.0, this->comvar->psnaccD*206265.0);
	}
*/
	return 0;
}

/*
int sPorabola::detS(double *s, double B)
{
	double ctgb, tgg, b, g;
	
	ctgb = 3.0*ka*B/(2.0*sqrt(2.0));
	b = atan(1/ctgb);
	tgg = pow(tan(b/2.0), 1.0/3.0);
	g = atan(tgg);

	*s = 2.0/tan(2.0*g);

	return 0;
}
*/
int sPorabola::orbWrite(char *forb)
{
	return(this->objorb->writeCat(forb, 0));
}

int sPorabola::detR(double *r, double r1, double r2)
{
	double rr, rrr;

	rr = (r1+r2)/2.0;

	do
	{
		rrr = rr;
		rr = sqrt(r1*r1*this->comvar->tau1/this->comvar->tau + this->comvar->tau2*r2*r2/this->comvar->tau - this->comvar->tau1*this->comvar->tau2/rrr);
	}while(fabs(rrr-rr)>EPS);

	*r = rr;

	return 0;
}

int sPorabola::fulInfo()
{
	orbCat *norb;
	norb = new orbCat;


	strcpy(norb->eJD, this->objorb->line->eJD);
	strcpy(norb->M0, this->objorb->line->M0);
	strcpy(norb->w, this->objorb->line->w);
	strcpy(norb->Node, this->objorb->line->Node);
	strcpy(norb->inc, this->objorb->line->inc);
	strcpy(norb->ec, this->objorb->line->ec);
	strcpy(norb->q, this->objorb->line->q);

	sprintf(norb->number, "%9d", this->apppos->hat->num);
//	sprintf(norb->epoha, "%4d", (int)this->apppos->hat->Eqx);
	sprintf(norb->G, ".0");
	sprintf(norb->H, ".00");
	sprintf(norb->G, ".0");
	strcpy(norb->name, this->apppos->hat->name);

	strcpy(norb->perturb, this->apppos->hat->pert);

	sprintf(norb->obs, "3");
	sprintf(norb->opp, "0");
	sprintf(norb->obsY, "2004");
	sprintf(norb->regY, "2004");
	sprintf(norb->makeD, "19071983");
	sprintf(norb->un, "0");

	this->objorb->setLCat(norb->number, "", "", "", norb->perturb, "", "", "", "", "", norb->name, "");

	return 0;
}

int sPorabola::int2pert(int *ip, char *pert)
{
	strcpy(pert, "");
	div_t dt;
	do
	{
		dt = div(*ip, 10);
		sprintf(pert, "%1d", dt.rem);
	}while(dt.quot!=1);



	return 0;
}
/*
int sPorabola::rdsys(double *aV, double *dV, double P1, double P2, double P3, int log)
{
/*
	if(P1 == 0.0)
	{
		if(P2 == 0.0)
		{
			*dV = PI/2.0;
			*aV = 0.0;
		}
		else
		{
			*aV = PI/2.0;
			*dV = fatan(P3, P2);
			if(P2<0.0) *dV += PI;
		}
	}
	else if(P2 == 0.0)
	{
		*aV = 0.0;
		*dV = atan2(P3, P1);
		if(P1<0.0) *dV += PI;
	}
	else
	{
		*aV = fatan(P2, P1);
		P3 *= sin(*aV);
		*dV = fatan(P3, P2);
	}

	*dV -= PI;
/

	double tga, cosa, pcosd, tgd, l, m, v, cosd, p;

	tga = P2/P1;
	*aV = atan2(P2, P1);

	if(*aV<0.0) *aV += 2.0*PI;

	cosa = cos(*aV);

	pcosd = P1/cosa;

	tgd = P3/P1;

	*dV = atan2(P3, pcosd);

	cosd = cos(*dV);

	p = P1/(cosa*cosd);

	l = cosd*cosa;
	m = cosd*sin(*aV);
	v = sin(*dV);

	if(log)
	{
		fprintf(this->FLOG, "\np*cos(d)*cos(a)	%f", P1);
		fprintf(this->FLOG, "\np*cos(d)*sin(a)	%f", P2);
		fprintf(this->FLOG, "\np*sin(d)	%f\n", P3);

		fprintf(this->FLOG, "\ntg(a)	%f", tga);
		fprintf(this->FLOG, "\ncos(a)	%f", cosa);
		fprintf(this->FLOG, "\np*cos(d)	%f", pcosd);
		fprintf(this->FLOG, "\ntg(d)	%f", tgd);
		fprintf(this->FLOG, "\ncos(d)	%f\n", cosd);

		fprintf(this->FLOG, "\na	%f", *aV);
		fprintf(this->FLOG, "\nd	%f\n", *dV);

		fprintf(this->FLOG, "\np	%f\n", p);

		fprintf(this->FLOG, "\nl	%f", l);
		fprintf(this->FLOG, "\nm	%f", m);
		fprintf(this->FLOG, "\nv	%f\n", v);



	}

	return 0;
}
*/

int sPorabola::proofSM()
{
	double x, y, z, t, dd;
	double oc;
	double P1, P2, P3, aV, dV;
	int i;
	double day;
	int year, mouth, sgn;
	char *ssgn;
	ssgn = new char[1];


//	printf("\n\nO-C\n");
	fprintf(this->FLOG, "\n\nO-C\n");

	note *s;
	s = new note;
	app *n;
	n = new app;

	this->sunpos->getNote(s, 0);
	this->apppos->getApp(n, 0);

	t = s->eJD;
	this->objorb->detRecEkv(&x, &y, &z, t);

	P1 = x + s->x;
	P2 = y + s->y;
	P3 = z + s->z;

	rdsys(&aV, &dV, P1, P2, P3);

	this->printOC(aV, dV, n->r, n->d, t);
//
	this->sunpos->getNote(s, 1);
	this->apppos->getApp(n, 1);

	t = s->eJD;
	this->objorb->detRecEkv(&x, &y, &z, t);

	P1 = x + s->x;
	P2 = y + s->y;
	P3 = z + s->z;

	rdsys(&aV, &dV, P1, P2, P3);

	this->printOC(aV, dV, n->r, n->d, t);
//
	this->sunpos->getNote(s, 2);
	this->apppos->getApp(n, 2);

	t = s->eJD;
	this->objorb->detRecEkv(&x, &y, &z, t);

	P1 = x + s->x;
	P2 = y + s->y;
	P3 = z + s->z;

	rdsys(&aV, &dV, P1, P2, P3);

	this->printOC(aV, dV, n->r, n->d, t);

	return 0;
}

int sPorabola::printOC(double aV, double dV, double a, double d, double t)
{
	double day, oc;
	int year, mouth, sgn;
	char *ssgn;
	ssgn = new char[1];

	dat2YMD(t, &year, &mouth, &day);
//	printf("\n%d %d %f", year, mouth, day);
	fprintf(this->FLOG,"\n%d %d %f", year, mouth, day);

	rad2hms(aV, &sgn, &year, &mouth, &day);
	if(sgn==0) strcpy(ssgn, "-");
	else strcpy(ssgn, " ");
//	printf(" %s%d %d %9.6f", ssgn, year ,mouth ,day);
	fprintf(this->FLOG, " %s%d %d %9.6f", ssgn, year ,mouth ,day);
	rad2gms(dV, &sgn, &year, &mouth, &day);
	if(sgn==0) strcpy(ssgn, "-");
	else strcpy(ssgn, " ");
//	printf("\t%s%d %d %9.6f", ssgn, year ,mouth ,day);
	fprintf(this->FLOG, "\t%s%d %d %9.6f", ssgn, year ,mouth ,day);

	rad2hms(a, &sgn, &year, &mouth, &day);
	if(sgn==0) strcpy(ssgn, "-");
	else strcpy(ssgn, " ");
//	printf("\n\t\t %s%d %d %9.6f", ssgn, year ,mouth ,day);
	fprintf(this->FLOG, "\n\t\t %s%d %d %9.6f", ssgn, year ,mouth ,day);
	rad2gms(d, &sgn, &year, &mouth, &day);
	if(sgn==0) strcpy(ssgn, "-");
	else strcpy(ssgn, " ");
//	printf("\t%s%d %d %9.6f", ssgn, year ,mouth ,day);
	fprintf(this->FLOG, "\t%s%d %d %9.6f", ssgn, year ,mouth ,day);

	oc = (aV - a)*SECINRAD/15.0;
//	printf("\n\nO-C\"\t\t %e", oc);
	fprintf(this->FLOG, "\n\nO-C\"\t\t %e", oc);
	oc = (dV - d)*SECINRAD;
//	printf("\t\t%e", oc);
	fprintf(this->FLOG, "\t\t%e", oc);
	oc = sqrt((aV - a)*(aV - a) + (dV - d)*(dV - d))*SECINRAD;
//	printf("\t%e\n", oc);
	fprintf(this->FLOG, "\t%e\n", oc);
	oc = (aV - a)*SECINRAD/15.0;
//	printf("\t\t %f", oc);
	fprintf(this->FLOG, "\t\t %f", oc);
	oc = (dV - d)*SECINRAD;
//	printf("\t\t%f", oc);
	fprintf(this->FLOG, "\t\t%f", oc);
	oc = sqrt((aV - a)*(aV - a) + (dV - d)*(dV - d))*SECINRAD;
//	printf("\t%f\n", oc);
	fprintf(this->FLOG, "\t%f\n", oc);


	return 0;
}
//
int sPorabola::soPost()
{
	double r, d, dr, dd;
	double x, y, z;
	this->objorb->detRecEkv(&x, &y, &z, this->appcnt->t);

	note *s;
	s = new note;
	this->sunpos->getNote(s, 1);

	double P1, P2, P3;

	P1 = x + s->x;
	P2 = y + s->y;
	P3 = z + s->z;

	rdsys(&r, &d, P1, P2, P3);

	app *vid;
	vid = new app;
	this->apppos->getApp(vid, 1);

	this->printOC(r, d, vid->r, vid->d, this->appcnt->t);
/*
	this->comvar->extaccR = fabs(r - vid->r);
	this->comvar->extaccD = fabs(d - vid->d);

	if(log)
	{
		fprintf(this->FLOG, "\n\n!!!!!!Vneshnie oshibki polozgeniya srednego:\ndR''\t\t\t\t%e\ndD''\t\t\t\t%e\ndelta\t\t\t\t%e\n", this->comvar->extaccR*206265.0, this->comvar->extaccD*206265.0, sqrt(this->comvar->extaccR*206265.0*this->comvar->extaccR*206265.0 + this->comvar->extaccD*206265.0*this->comvar->extaccD*206265.0));
	}
*/
	return 0;
}
//