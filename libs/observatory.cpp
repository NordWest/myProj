//#include "stdafx.h"
//#include "..\DAster\StdAfx.h"
//#include "..\DAster\DAster.h"

//#include "stdafx.h"
//#include "WinLofo.h"

//#include "MainFrm.h"

/////////////////////////////////////

#include "observatory.h"


///////////////////////////////////////////////////

obsy::obsy()
{
	this->num = new char[16];
	this->name = new char[64];
	this->tail = new char[16];
	strcpy(this->tail, "");
}

obsy::~obsy()
{
	delete [] this->num;
	delete [] this->name;
	delete [] this->tail;
}

int obsy::copyTo(obsy *dest)
{
    if(dest==NULL) return 1;

    dest->Cos = Cos;
    dest->Long = Long;
    sprintf(dest->name, name, 64);
    sprintf(dest->num, num, 16);
    sprintf(dest->tail, tail, 16);
    dest->Sin = Sin;

    return 0;
}

double obsy::getFi()
{
	return atan2(Sin, Cos);
}

///////////////////////////////////////////////////

observatory::observatory() : FileDynStr(OBS_SIZE)
{
	this->record = new obsy;
	this->h = 6378.140/AUKM;
//	this->nofobsys = 0;
}

observatory::~observatory()
{
	delete(this->record);
//	this->nofobsys = 0;
}

/*
int observatory::init(char *fn)
{
    return(init(fn, OBS_SIZE));
}
*/
int observatory::rec2s(char *arec)
{
	if(arec==NULL) return 1;

	char *slo, *slo1;
	slo = new char[32];
	slo1 = new char[32];

//	int2str(slo1, this->record->num);
//	index(slo, slo1, "0", 3);
	sprintf(arec, "%s%10.5f%8.6f %7.5f %s%s", this->record->num, this->record->Long, this->record->Cos, this->record->Sin, this->record->name, this->record->tail);

	delete [] slo;
	return 0;
}

int observatory::s2rec(char *arec)
{
	if(arec==NULL) return 1;

	char *slo;

	slo = new char[82];

	slovoG(str, slo, 0, 3);
	strcpy(this->record->num, slo);
	strcpy(&this->record->num[3], "\0");

	slovoG(str, slo, 4, 13);
	this->record->Long = atof(slo)*PI/180.0;
	slovoG(str, slo, 13, 20);
	this->record->Cos = atof(slo);
	slovoG(str, slo, 21, 29);
	this->record->Sin = atof(slo);
	slovoG(str, slo, 30, 81);
	strcpy(this->record->name, slo);
	strcpy(&this->record->name[strlen(slo)-1], "\0");

	delete [] slo;

	return 0;
}

int observatory::addobsy(char *num, double Long, double Cos, double Sin, char *name, int pos)
{
	if(name==NULL)return 1;
	strncpy(this->record->num, num, 3);
	this->record->Long = Long;
	this->record->Cos = Cos;
	this->record->Sin = Sin;
	index(this->record->name, name, " ", 50);

	this->rec2s(this->str);
	this->AddStr(this->str, pos);
	return 0;
}

int observatory::getobsynum(int nofobsy)
{
	this->ReadStr(nofobsy);
	this->s2rec(this->str);

	return 0;
}

int observatory::getobsynumO(char* nofobsy)
{
	int i = 0;
//printf("i= %d\n", i);
	while(!this->ReadStr(i))
	{
                i++;
             //   printf("%d: %s\n", i, str);
		this->s2rec(this->str);
		if(streqv(this->record->num, nofobsy)) return 0;
	}


	return 1;
}

int observatory::getobsynumO_alt(char* nofobsy)
{
        int i = 0;

        while(!this->ReadStr(i))
        {
                i++;
                this->s2rec(this->str);
                if(streqv(this->record->num, nofobsy)) return i-1;
        }

        return -1;
}


int observatory::getobsyname(char *name)
{
	int i = 0;

	char *oname_t = new char[16];

	while(!this->ReadStr(i))
	{
		i++;

		this->s2rec(this->str);

		if(streqv(this->record->name, name)) return 0;
	}

	return 1;
}

int observatory::getobsyname_alt(char *name)
{
        int i = 0;

        char *oname_t = new char[16];

        while(!this->ReadStr(i))
        {
                i++;

                this->s2rec(this->str);

                if(streqv(this->record->name, name)) return i-1;
        }

        return -1;
}

int observatory::det_state(double tUTC)
{
    //UTC2s_alt(tUTC, this->record->Long, &this->stime);

    double gm1;
    jdUT1_to_GMST1(&gm1, tUTC);//pady
    //UTC2s(mjd2jd(refParam.utc), refParam.Long, &s);
    stime = 2.0*PI*gm1 + this->record->Long;

    this->x = this->h*this->record->Cos*cos(this->stime);
    this->y = this->h*this->record->Cos*sin(this->stime);
    this->z = this->h*this->record->Sin;

    double R = sqrt(x*x + y*y);
    vx = 2.0*PI*x;
    vy = 2.0*PI*y;
    vz = 0.0;

    state[0] = x;
    state[1] = y;
    state[2] = z;
    state[3] = vx;
    state[4] = vy;
    state[5] = vz;

	return 0;
}


/*
int observatory::initFile(char *fname)
{
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

	int num;
	double Long, Sin, Cos;
	char *slo, *s;

	char *name;//, *tail;
	name = new char[30];

	slo = new char[20];
	s = new char[1];

	strcpy(nstr, str);
	fgets(str, STRLEN, FN);
	strcpy(nstr, str);
	fgets(str, STRLEN, FN);


	while(strcmp(str, nstr)!=0)
	{
			//
		slovoG(str, slo, 0, 3);
		num = str2int(slo);

		slovoG(str, slo, 4, 12);
		Long = atof(slo)*PI/180.0;
		slovoG(str, slo, 13, 20);
		Cos = atof(slo);
		slovoG(str, slo, 21, 29);
		Sin = atof(slo);
		slovoG(str, slo, 30, 60);
		strcpy(name, slo);

		strcpy(nstr, str);
		fgets(str, STRLEN, FN);

		if(strcmp(str, nstr)==0) return 0;
		this->addobsy(num, Long, Cos, Sin, name);
	}

	return 0;
}
*/
