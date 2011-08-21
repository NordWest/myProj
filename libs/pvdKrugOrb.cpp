#include "stdafx.h"
#include "pvdKrugOrb.h"

PvdVars::PvdVars()
{
	r = new myvector(3);
	g = new myvector(3);
	d = new myvector(3);

	rt = new myvector(3);
	gt = new myvector(3);
	dt = new myvector(3);

	rtt = new myvector(3);
	gtt = new myvector(3);
	dtt = new myvector(3);

	R = new myvector(3);
	G = new myvector(3);
	D = new myvector(3);

	Dt = new myvector(3);
	Dtt = new myvector(3);
	
	I = new myvector(3);
	J = new myvector(3);
	K = new myvector(3);

	Q = new myvector(3);
	V = new myvector(3);

	M = new myvector(3);
	T = new myvector(3);

	this->Ckey = -1;

}

PvdVars::~PvdVars()
{
	delete r;
	delete g;
	delete d;

	delete rt;
	delete gt;
	delete dt;

	delete rtt;
	delete gtt;
	delete dtt;

	delete R;
	delete G;
	delete D;

	delete Dt;
	delete Dtt;
	
	delete I;
	delete J;
	delete K;

	delete Q;
	delete V;

	delete M;
	delete T;
}

pvdKrugOrb::pvdKrugOrb()
{
	this->obs = new observ("header.405");
	this->vars = new PvdVars;
	this->vpos = new pvdK;

	inipref = new char[255];

	fheader = new char[255];
	fbin = new char[255];
	fobs = new char[255];
	fhat = new char[255];
	fres = new char[255];

	this->initPrefix("..\\res\\ea\\");

	fname = new char[255];

	flog = new char[255];
	strcpy(flog, "pvd.log");
//	FLOG = fopen(flog, "w+");

	foc = new char[255];
	strcpy(foc, "oc.dat");

//	this->res = new OrbCat("res.elm", EPOS5_CAT);
	this->orb = new orbit;

	rmin = 1.0;
	rmax = 30.0;
	rstep = 0.5;
	progr = 0.0;
}

pvdKrugOrb::~pvdKrugOrb()
{
	delete this->obs;
	delete this->vars;
	delete this->vpos;
	
	delete [] fname;
	delete [] flog;
	delete [] foc;
	delete [] inipref;
	delete [] fheader;
	delete [] fbin;
	delete [] fobs;
	delete [] fhat;
	delete [] fres;

	fclose(FLOG);
}

int pvdKrugOrb::initPrefix(char *prefix)
{
	if(prefix==NULL) return 1;

	strcpy(inipref, prefix);

	sprintf(fheader, "%sheader.405", inipref);
	sprintf(fbin, "%sascp2000.405", inipref);
	sprintf(fobs, "%sObs.txt", inipref);

	return 0;
}

int pvdKrugOrb::initEnv(char *fn_dele_h, char *fn_dele_bin, char *fn_obs)
{
	if(this->obs->init(fn_obs, fn_dele_h, fn_dele_bin)) return 1;
	if(this->obs->set_obs_parpam(13, CENTER_SUN, SK_EKVATOR, "084")) return 1;
	
	return 0;
}

int pvdKrugOrb::initObs(char *fn_pvd, int file_mod)
{
	strcpy(this->fname, fn_pvd);
	initFnames();
	
	switch(file_mod)
	{
	case 0:
		ast *oast;
		oast = new ast;
		
		oast->initFile(fn_pvd);
		oast->GetCoordList(this->vpos->coords);
		
//		delete oast;
		break;
	case 1:
		mpcs *mpc_obs;
		mpc_obs = new mpcs;
		mpc_obs->init(fn_pvd);
		mpc_obs->GetCoordList(this->vpos->coords);

//		delete mpc_obs;
		break;
	default:
		return 1;
		break;
	}

	return 0;
}

int pvdKrugOrb::initFnames()
{
	strcpy(this->fhat, this->fname);
	int len = strlen(fhat);
	strcpy(&fhat[len-4], "\0");

	sprintf(this->flog, "%s.log", fhat);
	sprintf(this->foc, "%s.oc", fhat);
	sprintf(this->fres, "%s.elm", fhat);


	if(this->klog) FLOG = fopen(flog, "w+");

	return 0;
}

int pvdKrugOrb::initVar()
{
	if(this->vpos->findPVD()) return 1;

	double *x;
	x = new double[4];

	char *str;
	str = new char[255];

	myvector *temp;
	temp = new myvector(3);
/*
	if(klog)
	{

		dat2YMD_str(this->vpos->t, str);
		fprintf(FLOG, "%s\n\n", str);

		rad2hms_str(vpos->r->val, str);
		fprintf(FLOG, "RA\t%s\n", str);
		rad2gms_str(vpos->d->val, str);
		fprintf(FLOG, "DEC\t%s\n", str);
		fprintf(FLOG, "m\t%f\n", vpos->m->val*SECINRAD);
		fprintf(FLOG, "psi\t%f\n", rad2grad(vpos->psi->val));
		fprintf(FLOG, "mt\t%f\n", vpos->mt->val*SECINRAD);
		fprintf(FLOG, "c\t%f\n\n", vpos->c->val);
	}
*/

	this->vars->t = this->vpos->t;
	this->obs->det_observ(this->vars->t);

	this->vars->g->set(0, this->obs->ox);
	this->vars->g->set(1, this->obs->oy);
	this->vars->g->set(2, this->obs->oz);

	double norm = this->vars->g->norm(); 
	this->vars->G->set(0, this->vars->g->get(0)/norm);
	this->vars->G->set(1, this->vars->g->get(1)/norm);
	this->vars->G->set(2, this->vars->g->get(2)/norm);

	this->vars->gt->set(0, this->obs->ovx);
	this->vars->gt->set(1, this->obs->ovy);
	this->vars->gt->set(2, this->obs->ovz);

	this->vars->gtt->set(0, this->obs->ovxt);
	this->vars->gtt->set(1, this->obs->ovyt);
	this->vars->gtt->set(2, this->obs->ovzt);

	this->vars->g2 = this->vars->g->get(0)*this->vars->g->get(0) + this->vars->g->get(1)*this->vars->g->get(1) + this->vars->g->get(2)*this->vars->g->get(2);
	this->vars->gt2 = this->vars->gt->get(0)*this->vars->gt->get(0) + this->vars->gt->get(1)*this->vars->gt->get(1) + this->vars->gt->get(2)*this->vars->gt->get(2);
	this->vars->gtt2 = this->vars->gtt->get(0)*this->vars->gtt->get(0) + this->vars->gtt->get(1)*this->vars->gtt->get(1) + this->vars->gtt->get(2)*this->vars->gtt->get(2);
/*
	if(klog)
	{
		fprintf(FLOG, "g:\t\t%15.13f %15.13f %15.13f\tg2 = %15.13f\n", this->vars->g->get(0), this->vars->g->get(1), this->vars->g->get(2), this->vars->g2);
		fprintf(FLOG, "gt:\t\t%15.13f %15.13f %15.13f\tgt2 = %15.13f\n", this->vars->gt->get(0), this->vars->gt->get(1), this->vars->gt->get(2), this->vars->gt2);
		fprintf(FLOG, "gtt:\t%15.13f %15.13f %15.13f\tgt2 = %15.13f\n\n", this->vars->gtt->get(0), this->vars->gtt->get(1), this->vars->gtt->get(2), this->vars->gtt2);
		fprintf(FLOG, "G:\t%15.13f %15.13f %15.13f\tn = %15.13f\n\n", this->vars->G->get(0), this->vars->G->get(1), this->vars->G->get(2), this->vars->G->norm());
		*temp = Vmul(this->vars->g, this->vars->G);
		fprintf(FLOG, "gxG:\t%15.13f %15.13f %15.13f\tn = %15.13f\n\n", temp->get(0), temp->get(1), temp->get(2), temp->norm());
	}
*/
/*
	this->vpos->coords->Get(x, this->vpos->coords->GetNum());
	this->vars->t1 = x[0];
	this->obs->det_observ(this->vars->t1);

	this->vars->g1 = new myvector(3);
	this->vars->g1->set(0, this->obs->ox);
	this->vars->g1->set(1, this->obs->oy);
	this->vars->g1->set(2, this->obs->oz);
*/
	this->vars->D->set(0, cos(this->vpos->d->val)*cos(this->vpos->r->val));
	this->vars->D->set(1, cos(this->vpos->d->val)*sin(this->vpos->r->val));
	this->vars->D->set(2, sin(this->vpos->d->val));

	this->vars->T->set(0, -sin(this->vpos->psi->val)*sin(this->vpos->r->val) - cos(this->vpos->psi->val)*sin(this->vpos->d->val)*cos(this->vpos->r->val));
	this->vars->T->set(1, sin(this->vpos->psi->val)*cos(this->vpos->r->val) - cos(this->vpos->psi->val)*sin(this->vpos->d->val)*sin(this->vpos->r->val));
	this->vars->T->set(2, cos(this->vpos->psi->val)*cos(this->vpos->d->val));

	this->vars->M->set(0, cos(this->vpos->psi->val)*sin(this->vpos->r->val) - sin(this->vpos->psi->val)*sin(this->vpos->d->val)*cos(this->vpos->r->val));
	this->vars->M->set(1, -cos(this->vpos->psi->val)*cos(this->vpos->r->val) - sin(this->vpos->psi->val)*sin(this->vpos->d->val)*sin(this->vpos->r->val));
	this->vars->M->set(2, sin(this->vpos->psi->val)*cos(this->vpos->d->val));
/*
	if(klog)
	{
		fprintf(FLOG, "D:\t\t%15.13f %15.13f %15.13f\n", this->vars->D->get(0), this->vars->D->get(1), this->vars->D->get(2));
		fprintf(FLOG, "T:\t\t%15.13f %15.13f %15.13f\n", this->vars->T->get(0), this->vars->T->get(1), this->vars->T->get(2));
		fprintf(FLOG, "M:\t\t%15.13f %15.13f %15.13f\n", this->vars->M->get(0), this->vars->M->get(1), this->vars->M->get(2));
		*temp = Vmul(this->vars->D, this->vars->T);
		fprintf(FLOG, "(M-DxT)/M [0]:\t%e %e %e\n\n", (this->vars->M->get(0) - temp->get(0))/this->vars->M->get(0), (this->vars->M->get(1) - temp->get(1))/this->vars->M->get(1), (this->vars->M->get(2) - temp->get(2))/this->vars->M->get(2));

	}
*/


//	this->vars->chv[this->vars->Ckey].R = new myvector(3);
/*
	this->vars->GsD = Smul(this->vars->G, this->vpos->D);
	this->vars->gsD = Smul(this->vars->g, this->vpos->D);
	this->vars->gtsDt = Smul(this->vars->gt, this->vpos->Dt);
	this->vars->gtsD = Smul(this->vars->gt, this->vpos->D);
	this->vars->gtsT = Smul(this->vars->gt, this->vars->T);
	this->vars->gttsT = Smul(this->vars->gtt, this->vars->T);

	this->vars->gsT = Smul(this->vars->g, this->vars->T);
	this->vars->gttsM = Smul(this->vars->gtt, this->vars->M);
	this->vars->gsM = Smul(this->vars->g, this->vars->M);
	this->vars->GsM = Smul(this->vars->G, this->vars->M);
	this->vars->signGM = sign(this->vars->GsM);
*/

	delete temp;
	delete [] str;
	delete [] x;
	return 0;
}

int pvdKrugOrb::compute()
{
	printf("\n\nCircle orbit computation\n\n");
	printf("initional file: %s\n", this->fname);
	printf("lof file: %s\n", this->flog);
	printf("O-C file: %s\n\n", this->foc);

	printf("Initial geliocentric distance: %f\n", this->rmin);
	printf("Last geliocentric distance: %f\n", this->rmax);
	printf("Step with calc: %f\n\n", this->rstep);

	if(this->initEnv(fheader, fbin, fobs))
	{
		printf("ERR InitEnv\n\nAny Key...\n");
		getch();
		return 0;
	}
	this->initVar();

	double r0, r1, r2, sg0, sg2, sg1, si0, rm, sgmin;

	rm = 500.0;
	sgmin = 1e+14;

	r0 = rmin;
	r2 = rmax;

	double dr = rmax - rmin;

	progr = 0.0;

	printf("r\t\tsg\t\tniter\n");

	for(r0=rmin; r0<=rmax; r0+=rstep)
	{
		printf("%f\t", r0);
		
		if(this->compOnce(r0))
		{
			printf("err\t\terr\n");
			continue;
		}
		if(sgmin>this->sg)
		{
			sgmin = sg;
			rm = r0;
		}

		printf("%e\t%d\n", this->sg, this->vars->iter);
	
		if(klog)
		{
			fprintf(FLOG, "r0:%f\trm:%15.13f\tsg:%e\tniter: %d\n\n", r0, rm, sg, this->vars->iter);

			fprintf(FLOG, "Q:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n", this->vars->Q->get(0), this->vars->Q->get(1), this->vars->Q->get(2), this->vars->Q->norm());
			fprintf(FLOG, "V:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n", this->vars->V->get(0), this->vars->V->get(1), this->vars->V->get(2), this->vars->V->norm());
			fprintf(FLOG, "R:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n\n", this->vars->R->get(0), this->vars->R->get(1), this->vars->R->get(2), this->vars->R->norm());
			fprintf(FLOG, "V*R = %15.13f = 0.0\n", Smul(this->vars->R, this->vars->V));
	//		fprintf(FLOG, "V*M= %15.13f = %15.13f\n\n", Smul(this->vars->V, this->vars->M), VM);

			fprintf(FLOG, "OrbElem:\n\n");
			fprintf(FLOG, "eJD\t%f\n", this->orb->elem->eJD);
			fprintf(FLOG, "M0\t%f\n", rad2grad(this->orb->elem->M0));
			fprintf(FLOG, "w\t%f\n", rad2grad(this->orb->elem->w));
			fprintf(FLOG, "Node\t%f\n", rad2grad(this->orb->elem->Node));
			fprintf(FLOG, "inc\t%f\n", rad2grad(this->orb->elem->inc));
			fprintf(FLOG, "ecc\t%f\n", this->orb->elem->ec);
			fprintf(FLOG, "q\t%f\n\n", this->orb->elem->q);
		}

		progr = (r0-rmin)/dr;

	}

	char *str;
	str = new char[255];

	myvector *temp;
	temp = new myvector(3);

	if(this->compOnce(rm)) return 1;
	if(klog)
	{
		fprintf(FLOG, "RESULT\n\n");

		dat2YMD_str(this->vpos->t, str);
		fprintf(FLOG, "%s\n\n", str);

		rad2hms_str(vpos->r->val, str);
		fprintf(FLOG, "RA\t%s\n", str);
		rad2gms_str(vpos->d->val, str);
		fprintf(FLOG, "DEC\t%s\n", str);
		fprintf(FLOG, "m\t%f\n", vpos->m->val*SECINRAD);
		fprintf(FLOG, "psi\t%f\n", rad2grad(vpos->psi->val));
		fprintf(FLOG, "mt\t%f\n", vpos->mt->val*SECINRAD);
		fprintf(FLOG, "c\t%f\n\n", vpos->c->val);

		fprintf(FLOG, "r/rt/rtt\t%15.13f/%15.13f/%15.13f\n\n", vpos->r->val, vpos->rt->val, vpos->rtt->val);

		fprintf(FLOG, "g:\t\t%15.13f %15.13f %15.13f\tg2 = %15.13f\n", this->vars->g->get(0), this->vars->g->get(1), this->vars->g->get(2), this->vars->g2);
		fprintf(FLOG, "gt:\t\t%15.13f %15.13f %15.13f\tgt2 = %15.13f\n", this->vars->gt->get(0), this->vars->gt->get(1), this->vars->gt->get(2), this->vars->gt2);
		fprintf(FLOG, "gtt:\t%15.13f %15.13f %15.13f\tgt2 = %15.13f\n\n", this->vars->gtt->get(0), this->vars->gtt->get(1), this->vars->gtt->get(2), this->vars->gtt2);
		fprintf(FLOG, "G:\t%15.13f %15.13f %15.13f\tn = %15.13f\n\n", this->vars->G->get(0), this->vars->G->get(1), this->vars->G->get(2), this->vars->G->norm());
		*temp = Vmul(this->vars->g, this->vars->G);
		fprintf(FLOG, "gxG:\t%15.13f %15.13f %15.13f\tn = %15.13f\n\n", temp->get(0), temp->get(1), temp->get(2), temp->norm());

		fprintf(FLOG, "D:\t\t%15.13f %15.13f %15.13f\n", this->vars->D->get(0), this->vars->D->get(1), this->vars->D->get(2));
		fprintf(FLOG, "T:\t\t%15.13f %15.13f %15.13f\n", this->vars->T->get(0), this->vars->T->get(1), this->vars->T->get(2));
		fprintf(FLOG, "M:\t\t%15.13f %15.13f %15.13f\n", this->vars->M->get(0), this->vars->M->get(1), this->vars->M->get(2));
		*temp = Vmul(this->vars->D, this->vars->T);
		fprintf(FLOG, "(M-DxT)/M [0]:\t%e %e %e\n\n", (this->vars->M->get(0) - temp->get(0))/this->vars->M->get(0), (this->vars->M->get(1) - temp->get(1))/this->vars->M->get(1), (this->vars->M->get(2) - temp->get(2))/this->vars->M->get(2));

		fprintf(FLOG, "rm:%15.13f\tsgmin:%e\tniter: %d\n\n", rm, sgmin, this->vars->iter);

		fprintf(FLOG, "Q:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n", this->vars->Q->get(0), this->vars->Q->get(1), this->vars->Q->get(2), this->vars->Q->norm());
		fprintf(FLOG, "V:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n", this->vars->V->get(0), this->vars->V->get(1), this->vars->V->get(2), this->vars->V->norm());
		fprintf(FLOG, "R:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n\n", this->vars->R->get(0), this->vars->R->get(1), this->vars->R->get(2), this->vars->R->norm());
		fprintf(FLOG, "V*R = %15.13f = 0.0\n\n", Smul(this->vars->V, this->vars->R));
//		fprintf(FLOG, "V*M= %15.13f = %15.13f\n\n", Smul(this->vars->V, this->vars->M), VM);

		fprintf(FLOG, "OrbElem:\n");
		fprintf(FLOG, "eJD\t%f\n", this->orb->elem->eJD);
		fprintf(FLOG, "M0\t%f\n", rad2grad(this->orb->elem->M0));
		fprintf(FLOG, "w\t%f\n", rad2grad(this->orb->elem->w));
		fprintf(FLOG, "Node\t%f\n", rad2grad(this->orb->elem->Node));
		fprintf(FLOG, "inc\t%f\n", rad2grad(this->orb->elem->inc));
		fprintf(FLOG, "ecc\t%f\n", this->orb->elem->ec);
		fprintf(FLOG, "q\t%f\n\n", this->orb->elem->q);


	}
/*
	if(this->appFirst(rmin)) return 1;
	if(this->computeOrb()) return 1;
	if(klog) this->computeOC();
*/
	OrbCat *oc;
	oc = new OrbCat;
	oc->init("res.elm");
	oc->Free();

	this->orb->set(oc);
	oc->AddRec();

	delete [] str;
	delete temp;

	return 0;

}

int pvdKrugOrb::computeIter()
{
	printf("\n\nCircle orbit computation\n\n");
	printf("initional file: %s\n", this->fname);
	printf("lof file: %s\n", this->flog);
	printf("O-C file: %s\n\n", this->foc);

	printf("Initial geliocentric distance: %f\n", this->rmin);
//	printf("Last geliocentric distance: %f\n", this->rmax);
//	printf("Step with calc: %f\n\n", this->rstep);

	if(this->initEnv(fheader, fbin, fobs))
	{
		printf("ERR InitEnv\n\nAny Key...\n");
		getch();
		return 0;
	}
	if(this->initVar()) return 1;

	double r0, r1, r2, sg0, sg2, sg1, si0, rm, sgmin;

//	rm = 500.0;
//	sgmin = 1e+14;


	this->klog = 1;

	r0 = rmin;
	r2 = rmax;

	double dr = rmax - rmin;

	progr = 0.0;

	printf("Iter\tk\t\tm\t\tr\n");

//	printf("%f\t", r0);
	
	if(this->compOnceIter(r0)) return 1;


//		printf("%e\t%d\n", this->sg, this->vars->iter);
/*	
		if(klog)
		{
			fprintf(FLOG, "r0:%f\trm:%15.13f\tsg:%e\tniter: %d\n\n", r0, rm, sg, this->vars->iter);

			fprintf(FLOG, "Q:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n", this->vars->Q->get(0), this->vars->Q->get(1), this->vars->Q->get(2), this->vars->Q->norm());
			fprintf(FLOG, "V:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n", this->vars->V->get(0), this->vars->V->get(1), this->vars->V->get(2), this->vars->V->norm());
			fprintf(FLOG, "R:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n\n", this->vars->R->get(0), this->vars->R->get(1), this->vars->R->get(2), this->vars->R->norm());
			fprintf(FLOG, "V*R = %15.13f = 0.0\n", Smul(this->vars->V, this->vars->R));
	//		fprintf(FLOG, "V*M= %15.13f = %15.13f\n\n", Smul(this->vars->V, this->vars->M), VM);

			fprintf(FLOG, "OrbElem:\n\n");
			fprintf(FLOG, "eJD\t%f\n", this->orb->elem->eJD);
			fprintf(FLOG, "M0\t%f\n", rad2grad(this->orb->elem->M0));
			fprintf(FLOG, "w\t%f\n", rad2grad(this->orb->elem->w));
			fprintf(FLOG, "Node\t%f\n", rad2grad(this->orb->elem->Node));
			fprintf(FLOG, "inc\t%f\n", rad2grad(this->orb->elem->inc));
			fprintf(FLOG, "ecc\t%f\n", this->orb->elem->ec);
			fprintf(FLOG, "q\t%f\n\n", this->orb->elem->q);
		}
*/
	char *str;
	str = new char[255];

	myvector *temp;
	temp = new myvector(3);

//	if(this->compOnce(rm)) return 1;
	if(klog)
	{
		fprintf(FLOG, "RESULT\n\n");

		dat2YMD_str(this->vpos->t, str);
		fprintf(FLOG, "%s\n\n", str);

		rad2hms_str(vpos->r->val, str);
		fprintf(FLOG, "RA\t%s\n", str);
		rad2gms_str(vpos->d->val, str);
		fprintf(FLOG, "DEC\t%s\n", str);
		fprintf(FLOG, "m\t%f\n", vpos->m->val*SECINRAD);
		fprintf(FLOG, "psi\t%f\n", rad2grad(vpos->psi->val));
		fprintf(FLOG, "mt\t%f\n", vpos->mt->val*SECINRAD);
		fprintf(FLOG, "c\t%f\n\n", vpos->c->val);

		fprintf(FLOG, "r/rt/rtt\t%15.13f/%15.13f/%15.13f\n\n", vpos->r->val, vpos->rt->val, vpos->rtt->val);

		fprintf(FLOG, "g:\t\t%15.13f %15.13f %15.13f\tg2 = %15.13f\n", this->vars->g->get(0), this->vars->g->get(1), this->vars->g->get(2), this->vars->g2);
		fprintf(FLOG, "gt:\t\t%15.13f %15.13f %15.13f\tgt2 = %15.13f\n", this->vars->gt->get(0), this->vars->gt->get(1), this->vars->gt->get(2), this->vars->gt2);
		fprintf(FLOG, "gtt:\t%15.13f %15.13f %15.13f\tgt2 = %15.13f\n\n", this->vars->gtt->get(0), this->vars->gtt->get(1), this->vars->gtt->get(2), this->vars->gtt2);
		fprintf(FLOG, "G:\t%15.13f %15.13f %15.13f\tn = %15.13f\n\n", this->vars->G->get(0), this->vars->G->get(1), this->vars->G->get(2), this->vars->G->norm());
		*temp = Vmul(this->vars->g, this->vars->G);
		fprintf(FLOG, "gxG:\t%15.13f %15.13f %15.13f\tn = %15.13f\n\n", temp->get(0), temp->get(1), temp->get(2), temp->norm());

		fprintf(FLOG, "D:\t\t%15.13f %15.13f %15.13f\n", this->vars->D->get(0), this->vars->D->get(1), this->vars->D->get(2));
		fprintf(FLOG, "T:\t\t%15.13f %15.13f %15.13f\n", this->vars->T->get(0), this->vars->T->get(1), this->vars->T->get(2));
		fprintf(FLOG, "M:\t\t%15.13f %15.13f %15.13f\n", this->vars->M->get(0), this->vars->M->get(1), this->vars->M->get(2));
		*temp = Vmul(this->vars->D, this->vars->T);
		fprintf(FLOG, "(M-DxT)/M [0]:\t%e %e %e\n\n", (this->vars->M->get(0) - temp->get(0))/this->vars->M->get(0), (this->vars->M->get(1) - temp->get(1))/this->vars->M->get(1), (this->vars->M->get(2) - temp->get(2))/this->vars->M->get(2));

	//	fprintf(FLOG, "rm:%15.13f\tsgmin:%e\tniter: %d\n\n", rm, sgmin, this->vars->iter);

		fprintf(FLOG, "Q:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n", this->vars->Q->get(0), this->vars->Q->get(1), this->vars->Q->get(2), this->vars->Q->norm());
		fprintf(FLOG, "V:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n", this->vars->V->get(0), this->vars->V->get(1), this->vars->V->get(2), this->vars->V->norm());
		fprintf(FLOG, "R:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n\n", this->vars->R->get(0), this->vars->R->get(1), this->vars->R->get(2), this->vars->R->norm());
		fprintf(FLOG, "V*R = %15.13f = 0.0\n\n", Smul(this->vars->V, this->vars->R));
//		fprintf(FLOG, "V*M= %15.13f = %15.13f\n\n", Smul(this->vars->V, this->vars->M), VM);

		fprintf(FLOG, "OrbElem:\n");
		fprintf(FLOG, "eJD\t%f\n", this->orb->elem->eJD);
		fprintf(FLOG, "M0\t%f\n", rad2grad(this->orb->elem->M0));
		fprintf(FLOG, "w\t%f\n", rad2grad(this->orb->elem->w));
		fprintf(FLOG, "Node\t%f\n", rad2grad(this->orb->elem->Node));
		fprintf(FLOG, "inc\t%f\n", rad2grad(this->orb->elem->inc));
		fprintf(FLOG, "ecc\t%f\n", this->orb->elem->ec);
		fprintf(FLOG, "q\t%f\n\n", this->orb->elem->q);


	}
/*
	if(this->appFirst(rmin)) return 1;
	if(this->computeOrb()) return 1;
	if(klog) this->computeOC();
*/
	OrbCat *oc;
	oc = new OrbCat;
	oc->init(this->fres);
	oc->Free();

	this->orb->set(oc);
	oc->AddRec();

	delete [] str;
	delete temp;

	return 0;

}

int pvdKrugOrb::compOnce(double r)
{
	if(this->appFirst(r)) return 1;
	if(this->computeOrb()) return 1;
	this->computeOC();
	return 0;
}

int pvdKrugOrb::compOnceIter(double r)
{
	int iterR = 0;
	if(this->appFirstIter(&r, &iterR)) return 1;
	if(this->computeOrb()) return 1;
	this->computeOC();
	return 0;
}

int pvdKrugOrb::appFirst(double r0)
{
	double k, k0, k1, m, VM, VT, d1, r;

	char *str;
	str = new char[255];

	fseek(FLOG, 0, SEEK_END);

	myvector *temp;
	temp = new myvector(3);


	this->vars->iter = 0;

//	if(klog) fprintf(FLOG, "r0 = %f\t", r0);
//	klog = !klog;

	k = this->det_k(r0);

	do
	{
		k0 = k;
		
		m = this->det_m(k0);
		this->det_R(k, m);
		VM = this->det_VM(r0);
		VT = this->det_VT(r0);

		IntAlongCurv(this->vars->V, this->vars->M, this->vars->R, VM, 0.0, 0);

		k = this->det_k1(r0);	
		this->vars->iter++;
//		printf("R*V\t%e\n", Smul(this->vars->R, this->vars->V));
	//	printf("M*V\t%e\n", Smul(this->vars->M, this->vars->V)-VM);

//		printf("%d\t%f\t%f\n", this->vars->iter, k, m);
	}while((fabs(k-k0)>1e-16)&&(this->vars->iter<=100));

	

	if(this->vars->iter>100) return 1;


	*this->vars->Q = Vmul(this->vars->R, this->vars->V);

/*	if(klog)
	{
		fprintf(FLOG, "Q:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n", this->vars->Q->get(0), this->vars->Q->get(1), this->vars->Q->get(2), this->vars->Q->norm());
		fprintf(FLOG, "V:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n", this->vars->V->get(0), this->vars->V->get(1), this->vars->V->get(2), this->vars->V->norm());
		fprintf(FLOG, "R:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n\n", this->vars->R->get(0), this->vars->R->get(1), this->vars->R->get(2), this->vars->R->norm());
		fprintf(FLOG, "V*R = %15.13f = 0.0\n", Smul(this->vars->V, this->vars->R));
		fprintf(FLOG, "V*M= %15.13f = %15.13f\n\n", Smul(this->vars->V, this->vars->M), VM);
	}
*/
	d1 = k*this->vars->g->norm();
	r0 = sqrt(d1*d1 + this->vars->g->norm()*this->vars->g->norm() + 2.0*d1*Smul(this->vars->D, this->vars->g));

//	printf("%d\t%f\t", this->vars->iter, r0);

	this->vars->r->set(0, r0*this->vars->R->get(0));
	this->vars->r->set(1, r0*this->vars->R->get(1));
	this->vars->r->set(2, r0*this->vars->R->get(2));
/*
	if(klog)
	{
		fprintf(FLOG, "r0:\t %15.13f\n", r0);
		fprintf(FLOG, "r:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n\n", this->vars->r->get(0), this->vars->r->get(1), this->vars->r->get(2), this->vars->r->norm());
	}
*/
//	klog = !klog;

//	fclose(FLOG);
	delete temp;

	

	return 0;
}

int pvdKrugOrb::appFirstIter(double *r0, int *iterR)
{
	if(*r0>100.0) return 1;
	if(*iterR>100) return 2;

	double k, k0, k1, m, VM, VT, d1, r, RM;

	myvector *tmp;
	tmp = new myvector(3);

	*iterR+=1;

	double rt = *r0;

//	printf("iterR %d\tr0 %f\trt%f\n", *iterR, *r0, rt);
	if(klog) fprintf(FLOG, "#######################################################\nIter: %d\tr: %15.13f\trt %15.13f\n", *iterR, *r0, rt);
		

//	char *str;
//	str = new char[255];

//	if(*r0>50.0) return 1;

	fseek(FLOG, 0, SEEK_END);

//	myvector *temp;
//	temp = new myvector(3);


	this->vars->iter = 0;

//	if(klog) fprintf(FLOG, "r0 = %f\t", r0);
//	klog = !klog;
//	r = *r0;
	
	k0 = this->det_k(rt);

//	if(*iterR==1) k = this->det_k(rt);
//	else k = this->det_k1(rt);
		
	VM = this->det_VM(rt);
	double RV;

	if(klog) fprintf(FLOG, "\nkappa = %15.13f\tVM = %15.13f\n", k0, VM);
	
//	VM = sqrt(rt)*Smul(this->vars->gt, this->vars->M)/ka;

	int direct = 0;

	do
	{
		k = k0;
		r = *r0;

//		if((*iterR>0)&&this->vars->iter==0) k = this->det_k1(r);
 
		if(klog) fprintf(FLOG, "iter = %d\tr= %15.13f\n", this->vars->iter, r);

//		if(klog) fprintf(FLOG, "gtM %f\n", Smul(this->vars->gt, this->vars->M));
		
		m = this->det_m(k0);
		this->det_R(k0, m);

		printf("%d\t%f\t%f\t%15.13f\n", this->vars->iter, k0, m, r);
		if(klog) fprintf(FLOG, "k= %15.13f\tm= %15.13f\n\n", k0, m);
		
		if(this->vars->iter>0) VM = Smul(this->vars->V, this->vars->M);
 		RM = Smul(this->vars->R, this->vars->M);
		if(klog) fprintf(FLOG, "R:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n", this->vars->R->get(0), this->vars->R->get(1), this->vars->R->get(2), this->vars->R->norm());
		if(klog) fprintf(FLOG, "M:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n", this->vars->M->get(0), this->vars->M->get(1), this->vars->M->get(2), this->vars->M->norm());
		if(klog) fprintf(FLOG, "VM = %15.13f\nRM= %15.13f\n\n", VM, RM);
		
//		if(IntAlongCurv(this->vars->V, this->vars->R, this->vars->M, 0.0, VM))IntAlongCurv(this->vars->V, this->vars->M, this->vars->R, VM, 0.0);
//		IntAlongCurv(this->vars->V, this->vars->M, this->vars->R, VM, 0.0);
/*		if(VM<0.0)
		{
			tmp->set(0, -this->vars->M->get(0));
			tmp->set(1, -this->vars->M->get(1));
			tmp->set(2, -this->vars->M->get(2));
			IntAlongCurv(this->vars->V, this->vars->R, tmp, 0.0, -VM);
		}*/
		IntAlongCurv(this->vars->V, this->vars->R, this->vars->M, 0.0, VM, direct);
//		IntAlongCurv1(this->vars->V, this->vars->M, this->vars->R, Smul(this->vars->R, this->vars->M), VM);

		if(klog) fprintf(FLOG, "V:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n", this->vars->V->get(0), this->vars->V->get(1), this->vars->V->get(2), this->vars->V->norm());

		k0 = this->det_k1(r);

		if(klog) fprintf(FLOG, "k1= %15.13f\n", k0);
//		IntAlongCurv(this->vars->V, this->vars->R, this->vars->M, 0.0, VM);
//		if(k0<0.0) IntAlongCurv(this->vars->V, this->vars->R, this->vars->M, 0.0, VM, 1);
		if(k0<0.0)
		{
			direct = !direct;
			IntAlongCurv(this->vars->V, this->vars->R, this->vars->M, 0.0, VM, direct);
			if(klog) fprintf(FLOG, "V:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n", this->vars->V->get(0), this->vars->V->get(1), this->vars->V->get(2), this->vars->V->norm());
			k0 = this->det_k1(r);
			if(k0<0.0)
			{
				*r0 += 0.1;
				if(*r0>100.0) return 1;
				continue;
			}
//			continue;
/*
			IntAlongCurv(this->vars->V, this->vars->R, this->vars->M, 0.0, VM, 1);
			k = this->det_k1(r);
			if(klog) fprintf(FLOG, "k1= %15.13f\n", k);
/*
			IntAlongCurv(this->vars->V, this->vars->M, this->vars->R, VM, 0.0, 0);
			k = this->det_k1(r);
			if(klog) fprintf(FLOG, "k1= %15.13f\n", k);

			IntAlongCurv(this->vars->V, this->vars->M, this->vars->R, VM, 0.0, 1);
			k = this->det_k1(r);
			if(klog) fprintf(FLOG, "k1= %15.13f\n", k);
*/
		}
//
//		if(klog) fprintf(FLOG, "k1= %15.13f\n", k);

/*		if(k<0.0)
		{
			IntAlongCurv(this->vars->V, this->vars->M, this->vars->R, VM, 0.0);
			k = this->det_k1(r);
		}
*/
		if(klog)
		{
			fprintf(FLOG, "V:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n", this->vars->V->get(0), this->vars->V->get(1), this->vars->V->get(2), this->vars->V->norm());
			fprintf(FLOG, "V*M - VM = %15.13f\n", Smul(this->vars->V, this->vars->M) - VM);
			fprintf(FLOG, "VR = %15.13f\n\n", Smul(this->vars->V, this->vars->R));
		}

		d1 = k0*this->vars->g->norm();
		*r0 = sqrt(d1*d1 + this->vars->g->norm()*this->vars->g->norm() + 2.0*d1*Smul(this->vars->g, this->vars->D));

//		VT = this->det_VT(r);
//		if(klog) fprintf(FLOG, "VT - VT = %15.13f\n", VT - Smul(this->vars->V, this->vars->T));
		if(klog) fprintf(FLOG, "---------------------------------------\n");
	
		this->vars->iter++;

		
	}while((fabs(r-*r0)>EPS)&&(this->vars->iter<=100));



	if(this->vars->iter>100)
	{
/*
		*r0+=0.5;
		*iterR-=1;
		return(this->appFirstIter(r0, iterR));
		*/
		return(2);
	}

	*this->vars->Q = Vmul(this->vars->R, this->vars->V);

	if(klog)
	{
		fprintf(FLOG, "\nQ:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n", this->vars->Q->get(0), this->vars->Q->get(1), this->vars->Q->get(2), this->vars->Q->norm());
		fprintf(FLOG, "V:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n", this->vars->V->get(0), this->vars->V->get(1), this->vars->V->get(2), this->vars->V->norm());
		fprintf(FLOG, "R:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n\n", this->vars->R->get(0), this->vars->R->get(1), this->vars->R->get(2), this->vars->R->norm());
		fprintf(FLOG, "V*R = %15.13f = 0.0\n", Smul(this->vars->V, this->vars->R));
		fprintf(FLOG, "Q*V = %15.13f = 0.0\n", Smul(this->vars->Q, this->vars->V));
		fprintf(FLOG, "Q*R = %15.13f = 0.0\n", Smul(this->vars->Q, this->vars->R));
		fprintf(FLOG, "V*M - VM = %15.13f\n\n", Smul(this->vars->V, this->vars->M) - VM);
	}

//	d1 = k*this->vars->g->norm();
//	printf("%f\n", d1);
//	if(klog) fprintf(FLOG, "d: %15.13f\n", d1);
//	*r0 = sqrt(d1*d1 + this->vars->g->norm()*this->vars->g->norm() + 2.0*d1*Smul(this->vars->D, this->vars->g));
//	*r0 = this->vars->g->norm()*sqrt(1.0 + k*k + 2.0*k*Smul(this->vars->G, this->vars->D));

	if(klog) fprintf(FLOG, "\nr0 = \t%15.13f\n", *r0);

	this->vars->r->set(0, *r0*this->vars->R->get(0));
	this->vars->r->set(1, *r0*this->vars->R->get(1));
	this->vars->r->set(2, *r0*this->vars->R->get(2));

	if(klog) fprintf(FLOG, "r:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n", this->vars->r->get(0), this->vars->r->get(1), this->vars->r->get(2), this->vars->r->norm());

	int resI = 0;

	if(fabs(rt-(*r0))>1e-5) resI = this->appFirstIter(r0, iterR);

//	printf("\n\nr0 %f\n\n", *r0);

//	printf("%d\t%f\t", this->vars->iter, r0);

	
/*
	if(klog)
	{
		fprintf(FLOG, "r0:\t %15.13f\n", r0);
		fprintf(FLOG, "r:\t\t%15.13f %15.13f %15.13f\tn = %15.13f\n\n", this->vars->r->get(0), this->vars->r->get(1), this->vars->r->get(2), this->vars->r->norm());
	}
*/
//	klog = !klog;

//	fclose(FLOG);
//	delete temp;

	return resI;
}

double pvdKrugOrb::det_VM(double r)
{
	return(sqrt(r)*Smul(this->vars->gt, this->vars->M)/ka);
}

double pvdKrugOrb::det_VT(double r)
{
	return(sqrt(r)*Smul(this->vars->gt, this->vars->T)/ka);
}

void pvdKrugOrb::det_R(double k, double m)
{
	this->vars->R->set(0, this->vars->G->get(0)/m + k*this->vars->D->get(0)/m);
	this->vars->R->set(1, this->vars->G->get(1)/m + k*this->vars->D->get(1)/m);
	this->vars->R->set(2, this->vars->G->get(2)/m + k*this->vars->D->get(2)/m);
}

double pvdKrugOrb::det_k(double r)
{
	double sl1, sl2, sl3;


	sl1 = -Smul(this->vars->G, this->vars->D);
	sl2 = pow(r/this->vars->g->norm(), 2.0);
	sl3 = pow(Smul(this->vars->G, this->vars->D), 2.0);

	double ret = sl1 + sqrt(sl2 + sl3 - 1.0);


	return(ret);
}

double pvdKrugOrb::det_k1(double r)
{
	return(((ka*Smul(this->vars->V, this->vars->T))/(sqrt(r)) - Smul(this->vars->gt, this->vars->T))/(this->vpos->m->val*this->vars->g->norm()));
//	return((ka*VT/(this->vars->g->norm()*sqrt(r)) - Smul(this->vars->gt, this->vars->T))/this->vpos->m->val);
}

double pvdKrugOrb::det_m(double k)
{
	return(sqrt(1.0 + k*k + 2.0*k*Smul(this->vars->G, this->vars->D)));
}

int pvdKrugOrb::computeOrb()
{
	this->orb->elem->M0 = 0.0;
	this->orb->elem->eJD = this->vars->t;
	this->orb->elem->ec = 0.0;

//	this->orb->elem->Node = atan2(this->vars->R->get(2), this->vars->V->get(2));
//	if(this->orb->elem->Node<0.0) this->orb->elem->Node += 2.0*PI;
	myvector *Qr, *Rr, *Vr;
	Qr = new myvector(3);
	Rr = new myvector(3);
	Vr = new myvector(3);

	RotX(Qr, this->vars->Q, -EKV);
	RotX(Vr, this->vars->V, -EKV);
	RotX(Rr, this->vars->R, -EKV);
/*
	RotX(Qr, this->vars->Q, 0.0);
	RotX(Vr, this->vars->V, 0.0);
	RotX(Rr, this->vars->R, 0.0);
*/
//	printf("\n\nOrb R*V\t%e\n", Smul(this->vars->R, this->vars->V));

/*
	this->orb->elem->Node = atan2(this->vars->Q->get(1), this->vars->Q->get(0)) + PI/2.0;
	if(this->orb->elem->Node<0.0) this->orb->elem->Node += 2.0*PI;

	this->orb->elem->inc = atan2(sqrt(this->vars->Q->get(0)*this->vars->Q->get(0) + this->vars->Q->get(1)*this->vars->Q->get(1)), this->vars->Q->get(2));
	if(this->orb->elem->inc<0.0) this->orb->elem->inc += 2.0*PI;

	this->orb->elem->w = atan2(this->vars->R->get(2), this->vars->V->get(2));
*/
	this->orb->elem->Node = atan2(Qr->get(1), Qr->get(0)) + PI/2.0;
	if(this->orb->elem->Node<0.0) this->orb->elem->Node += 2.0*PI;
/*
	if(sin(this->orb->elem->Node)>0.1)
	{
		this->orb->elem->inc = atan2(Qr->get(0), Qr->get(2)*sin(this->orb->elem->Node));
	}
	else
	{
		this->orb->elem->inc = atan2(-Qr->get(1), Qr->get(2)*cos(this->orb->elem->Node));
	}
*/
	this->orb->elem->inc = atan2(sqrt(Qr->get(0)*Qr->get(0) + Qr->get(1)*Qr->get(1)), Qr->get(2));
	if(this->orb->elem->inc<0.0) this->orb->elem->inc += 2.0*PI;

	this->orb->elem->M0 = atan2(Rr->get(2), Vr->get(2));
	if(this->orb->elem->M0<0.0) this->orb->elem->M0 += 2.0*PI;
	this->orb->elem->w = 0.0;


	this->orb->elem->q = this->vars->r->norm();
/*
	if(klog)
	{
		fprintf(FLOG, "OrbElem:\n\n");
		fprintf(FLOG, "eJD\t%f\n", this->orb->elem->eJD);
		fprintf(FLOG, "M0\t%f\n", rad2grad(this->orb->elem->M0));
		fprintf(FLOG, "w\t%f\n", rad2grad(this->orb->elem->w));
		fprintf(FLOG, "Node\t%f\n", rad2grad(this->orb->elem->Node));
		fprintf(FLOG, "inc\t%f\n", rad2grad(this->orb->elem->inc));
		fprintf(FLOG, "ecc\t%f\n", this->orb->elem->ec);
		fprintf(FLOG, "q\t%f\n\n", this->orb->elem->q);
	}
*/

	return 0;
}
/*
int pvdKrugOrb::computeOrb1()
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
	n = this->vars->Ckey;
//	n = 0;
	r->set(0, this->vars->chv[n].r[0]);
	r->set(1, this->vars->chv[n].r[1]);
	r->set(2, this->vars->chv[n].r[2]);
	rt->set(0, this->vars->chv[n].rt[0]);
	rt->set(1, this->vars->chv[n].rt[1]);
	rt->set(2, this->vars->chv[n].rt[2]);

//
	char *pv;
	pv = new char[100];
	r->print(pv);
	printf("\n\nr ecl:	%s	%f", pv, r->norm());
	rt->print(pv);
	printf("\nrt ecl:	%s	%f", pv, rt->norm());
//

//	r->RotX(EKV);
//	rt->RotX(EKV);

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
	dT = eJD - TT + 1;
//	dT = TT - eJD;

	nn = ka*pow(q, -1.5)/sqrt(2.0);

	M0 = nn*dT;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	printf("\n\nOrb Elsements\n\nT\t%f\nPer.\t%f\nNode\t%f\ninc\t%f\nq\t%f\n", TT, w*180.0/PI, Node*180.0/PI, inc*180.0/PI, q);
/*	if(this->klog)
	{
		fprintf(this->slog->FLOG, "\n\nOrb Elsements\n\nT\t%f\nPer.\t%f\nNode\t%f\ninc\t%f\nq\t%f\n", TT, w*180.0/PI, Node*180.0/PI, inc*180.0/PI, q);
	}
/
//	this->res = new OrbCat(;

	this->orb->elem->eJD = eJD;
	this->orb->elem->M0 = M0;
	this->orb->elem->w = w;
	this->orb->elem->Node = Node;
	this->orb->elem->inc = inc;
	this->orb->elem->ec = ecc;


//	this->orb->set(res);

//	this->res->initElem(eJD, M0, w, Node, inc, ecc, q);
//
//
	return 0;
}
*/

int pvdKrugOrb::computeOC()
{
	OC *ocf;
	ocf = new OC;

	ocf->f_out->init(foc, OC_FILE_REC_SIZE);
	ocf->f_out->Free();
	
	ocf->obs->init(fobs, fheader, fbin);
	ocf->obs->set_obs_parpam(GEOCENTR_NUM, CENTER_SUN, SK_EKVATOR, "084");

	ocf->CountOC(orb, vpos->coords);

	this->sg = ocf->sg;

//	if(klog) fprintf(FLOG, "%e\n", sg);
//	delete ocf;

	return 0;
}