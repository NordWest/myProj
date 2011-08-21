#include "stdafx.h"
#include "oc.h"

void oc_file_rec::copyTo(oc_file_rec *rec)
{
	rec->d0 = this->d0;
	rec->d1 = this->d1;
	rec->r0 = this->r0;
	rec->r1 = this->r1;
	rec->d_dif = this->d_dif;
	rec->r_dif = this->r_dif;
}

void oc_file_rec::set(double JD, double r0, double  d0, double r1, double d1)
{
	this->JD = JD;
	this->r0 = r0;
	this->d0 = d0;
	this->r1 = r1;
	this->d1 = d1;

	this->r_dif = r0 - r1;
	this->d_dif = d0 - d1;
	this->tot_dif = sqrt(r_dif*r_dif + d_dif*d_dif);
}

oc_file::oc_file()
{
	this->record = new oc_file_rec;
}

oc_file::~oc_file()
{
	delete this->record;
}

void oc_file::rec2s()
{
	int y, m, s;
	double d;

	char *slo;
	slo = new char[128];

	strcpy(this->str, "");

	dat2YMD_str(this->record->JD, slo);
	strcat(str, slo);
	strcat(str, "\t");

	rad2hms_str(this->record->r0, slo);
	strcat(str, slo);
	strcat(str, "\t");

	rad2gms_str(this->record->d0, slo);
	strcat(str, slo);
	strcat(str, "\n");

	strcat(str, "\t\t\t\t\t");

	rad2hms_str(this->record->r1, slo);
	strcat(str, slo);
	strcat(str, "\t");

	rad2gms_str(this->record->d1, slo);
	strcat(str, slo);
	strcat(str, "\n");

	strcat(str, "\t\t\t\t\t");

	rad2hms_str(this->record->r_dif, slo);
	strcat(str, slo);
	strcat(str, "\t");

	rad2gms_str(this->record->d_dif, slo);
	strcat(str, slo);
	strcat(str, "\n");

	strcat(str, "\t\t\t\t\t\t\t\t\t");

	rad2gms_str1(this->record->tot_dif, slo);
	strcat(str, slo);

	delete [] slo;
}

void oc_file::s2rec(char *arec)
{
	char *tstr;
	tstr = new char[30];
	char *s;
	s = new char[30];

	int sign;
	double numb;

	int year, mth;

	slovoG(arec, tstr, 0, 4);
	year = atoi(tstr);
	slovoG(arec, tstr, 5, 7);
	mth = atoi(tstr);

	slovoG(arec, tstr, 8, 18);
	numb = atof(tstr);

	dat2JD(&this->record->JD, year, mth, numb);

//	slovoG(arec, s, 20, 21);
	slovoG(arec, tstr, 20, 22);
	year = atoi(tstr);
	slovoG(arec, tstr, 23, 25);
	mth = atoi(tstr);
	slovoG(arec, tstr, 26, 33);
	numb = atof(tstr);

	sign = 1;//(!strcmp(s, " "))||(!strcmp(s, "+"));
	hms2rad(&this->record->r0, sign, year, mth, numb);		

	slovoG(arec, s, 36, 37);
	slovoG(arec, tstr, 37, 39);
	year = atoi(tstr);
	slovoG(arec, tstr, 40, 42);
	mth = atoi(tstr);
	slovoG(arec, tstr, 43, 49);
	numb = atof(tstr);

	sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
	gms2rad(&this->record->d0, sign, year, mth, numb);

	
/////////////////////////////////
	slovoG(arec, tstr, 52, 54);
	year = atoi(tstr);
	slovoG(arec, tstr, 55, 57);
	mth = atoi(tstr);
	slovoG(arec, tstr, 58, 64);
	numb = atof(tstr);

	sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
	hms2rad(&this->record->r1, sign, year, mth, numb);		

	slovoG(arec, s, 66, 67);
	slovoG(arec, tstr, 67, 69);
	year = atoi(tstr);
	slovoG(arec, tstr, 70, 72);
	mth = atoi(tstr);
	slovoG(arec, tstr, 73, 79);
	numb = atof(tstr);

	sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
	gms2rad(&this->record->d1, sign, year, mth, numb);

/////////////////////////////////
	slovoG(arec, tstr, 85, 87);
	year = atoi(tstr);
	slovoG(arec, tstr, 88, 90);
	mth = atoi(tstr);
	slovoG(arec, tstr, 91, 98);
	numb = atof(tstr);

	sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
	hms2rad(&this->record->r_dif, sign, year, mth, numb);		

	slovoG(arec, s, 99, 100);
	slovoG(arec, tstr, 100, 102);
	year = atoi(tstr);
	slovoG(arec, tstr, 103, 105);
	mth = atoi(tstr);
	slovoG(arec, tstr, 106, 112);
	numb = atof(tstr);

	sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
	gms2rad(&this->record->d_dif, sign, year, mth, numb);

/////////////////////////////////
//	slovoG(arec, s, 122, 123);
	slovoG(arec, tstr, 122, 125);
	year = atoi(tstr);
	slovoG(arec, tstr, 126, 128);
	mth = atoi(tstr);
	slovoG(arec, tstr, 129, 135);
	numb = atof(tstr);

	sign = (!strcmp(s, " "))||(!strcmp(s, "+"));
	gms2rad(&this->record->tot_dif, sign, year, mth, numb);



	delete [] tstr;
	delete [] s;


}

int oc_file::GetRec(int pos)
{
	char *str1;
	int sz = OC_FILE_REC_SIZE;
	str1 = new char[1024];
	strcpy(str1, "");

	this->ReadStr(pos*3);
	strcat(str1, this->str);
	this->ReadStr(pos*3+1);
	strcat(str1, this->str);
	this->ReadStr(pos*3+2);
	strcat(str1, this->str);
	this->ReadStr(pos*3+3);
	strcat(str1, this->str);

//	strcpy(str, str1);

	this->s2rec(str1);

	return 0;
}

int oc_file::AddRec(double JD, double r0, double d0, double r1, double d1)
{
	this->record->set(JD, r0, d0, r1, d1);
	this->rec2s();
	this->AddStr(this->str, this->nstr);
	return 0;
}

int oc_file::AddRec()
{
//	this->record->set(JD, r0, d0, r1, d1);
	this->rec2s();
	this->AddStr(this->str, this->nstr);
	return 0;
}

OC::OC()
{
//	this->orb0 = new orbit;
//	this->orb1 = new orbit;

	this->f_out = new oc_file;
	this->f_out->init("oc.dat", OC_FILE_REC_SIZE);
	this->f_out->Free();

	this->obs = new observ("header.405");
}

OC::~OC()
{
//	delete orb0;
//	delete orb1;
	delete this->f_out;
	delete this->obs;
}
/*
int OC::CountOC(double t0, double t1, double step)
{
	double t;

	for(t=t0; t<=t1; t+=step)
	{
		this->DetOC(t);
		this->f_out->AddRec();
	/*
		if(this->obs->det_observ(t)) return 1;
		this->orb0->detRecEkv(&x0, &y0, &z0, t);
		this->orb1->detRecEkv(&x1, &y1, &z1, t);

		detRDnumGC(&ra0, &de0, x0, y0, z0, this->obs->ox, this->obs->oy, this->obs->oz, this->obs->obs->dcx, this->obs->obs->dcy, this->obs->obs->dcz);

		detRDnumGC(&ra1, &de1, x1, y1, z1, this->obs->ox, this->obs->oy, this->obs->oz, this->obs->obs->dcx, this->obs->obs->dcy, this->obs->obs->dcz);


		this->f_out->AddRec(t, ra0, de0, ra1, de1);
		/
	}

	return 0;
}
*/
int OC::CountOC(orbit *orb, List4 *odata)
{
	double t;
	double x0, y0, z0, x1, y1, z1, ra0, ra1, de0, de1;

	if(odata==NULL) return 1;

	int num = odata->GetNum();

	double *x;
	x = new double[odata->GetDim()];

	double diff = 0.0;

	for(int i=0; i<num; i++)
	{
		odata->Get(x, i);

		this->obs->det_observ(x[0]);

		orb->detRecEkv(&x0, &y0, &z0, x[0]);

		detRDnumGC(&ra0, &de0, x0, y0, z0, this->obs->ox, this->obs->oy, this->obs->oz, this->obs->obs->dcx, this->obs->obs->dcy, this->obs->obs->dcz);

		this->f_out->record->set(x[0], x[1], x[2], ra0, de0);

		diff += this->f_out->record->tot_dif*this->f_out->record->tot_dif;

		this->f_out->AddRec();
	}

	this->sg = sqrt(diff/(num*(num-1)));


	return 0;
}
/*
int OC::DetOC(double t)
{
	double x0, y0, z0, x1, y1, z1, ra0, ra1, de0, de1;

	if(this->obs->det_observ(t)) return 1;
	this->orb0->detRecEkv(&x0, &y0, &z0, t);
	this->orb1->detRecEkv(&x1, &y1, &z1, t);

	detRDnumGC(&ra0, &de0, x0, y0, z0, this->obs->ox, this->obs->oy, this->obs->oz, this->obs->obs->dcx, this->obs->obs->dcy, this->obs->obs->dcz);
	detRDnumGC(&ra1, &de1, x1, y1, z1, this->obs->ox, this->obs->oy, this->obs->oz, this->obs->obs->dcx, this->obs->obs->dcy, this->obs->obs->dcz);

	this->f_out->record->set(t, ra0, de0, ra1, de1);

}
*/