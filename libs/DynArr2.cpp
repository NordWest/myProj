#include "stdafx.h"
#include "DynArr2.h"

List4::List4()
{
	this->record = new record4L;
	this->record->pos = -1;
	this->record->x = new double[4];
	this->root = new record4L;
	this->root->pos = -1;
	this->root->next = NULL;

	this->dim = 4;
	this->num = 0;
}

List4::List4(int dim)
{
	this->record = new record4L;
	this->record->pos = -1;
	this->record->x = new double[dim];
	this->root = new record4L;
	this->root->pos = -1;
	this->root->next = NULL;

	this->dim = dim;
	this->num = 0;
}

List4::~List4()
{
	this->Free();

}

int List4::Add(double *x, int pos)
{
	record4L *next, *temp;
	next = this->root->next;
	temp = this->root;

	while(next!=NULL)
	{
		temp = next;
		next = next->next;

		if((pos>temp->pos)&&(pos<temp->pos)) break;
	}

	temp->next = new record4L;
	temp->next->pos = pos;
	temp->next->next = next;
	temp->next->x = new double[this->dim];
	for(int i=0; i<this->dim; i++)
	{
		temp->next->x[i] = x[i];
	}

	this->num++;
	
	return 0;
}

int List4::Get(double *x, int pos)
{
	record4L *next, *temp;
	next = this->root->next;
	temp = this->root;

	while((pos!=temp->pos)&&(next!=NULL))
	{
		temp = next;
		next = next->next;
	}

	if(pos!=temp->pos) return 1;

	for(int i=0; i<this->dim; i++)
	{
		x[i] = temp->x[i];
	}

	return 0;
}

int List4::Del(int pos)
{
	if(pos>this->num-1) return 1;

	record4L *next, *temp;
	next = this->root->next;
	temp = this->root;



	while(next!=NULL)
	{
		if(pos==next->pos) break;
		temp = next;
		next = next->next;
		
	}

	if(pos!=next->pos) return 1;

	delete [] temp->next->x;

	temp->next = next->next;

	this->num--;
	return 0;
}

int List4::Push(double *x)
{
	return(this->Add(x, this->num));
/*
	record4L *next, *temp;
	next = this->root->next;
	temp = this->root;

	while(next!=NULL)
	{
		temp = next;
		next = next->next;

		if((pos>temp->pos)&&(pos<next->pos)) break;
	}

	temp->next = new record4L;
	temp->next->pos = pos;
	temp->next->next = next;
	temp->next->x = new double[this->dim];
	for(int i=0; i<this->dim; i++)
	{
		temp->next->x[i] = x[i];
	}

	this->num++;
	
	return 0;*/
}

int List4::GetNum()
{
	return this->num;
}

int List4::Free()
{
	int i;
	for(i=num-1; i>=0; i--)
	{
		if(this->Del(i)) return i+1;
	}

	return 0;
}
/*
#include "stdafx.h"
//#include "..\DAster\StdAfx.h"
//#include "..\DAster\DAster.h"

//#include "MainFrm.h"

/////////////////////////////////////

#include "DynArr2.h"

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <malloc.h>

#include <string.h>

#ifndef DYNARR_CPP

#define DYNARR_CPP

#define MSIZE 1000

DynArr::DynArr()
{
	this->initMEM(0);
//		this->array = (double*)calloc(1000, sizeof(double));
//		this->setlength(0);
}

DynArr::DynArr(int size)
{
	this->initMEM(size);

	if(size<=MSIZE)
	{
		this->setlength(size);
	}
}

DynArr::DynArr(int size, double *arr)
{
	this->initMEM(size);

	if(size<=MSIZE)
	{
		this->initArr(size, arr);
	}
	else this->initArr(0, NULL);
}

DynArr::~DynArr()
{
	delete this->arr;
	this->arr = NULL;
}

int DynArr::initMEM(int Rsize)
{
	this->arr = new List4(1);
//	this->array = new double[Rsize];
	this->setlength(Rsize);
	return 0;
}

int DynArr::initArr(int size, double *arr)
{
	if((size==0)||(arr==NULL)) return 1;
//	this->initMEM(size);
//	this->setlength(size);
	for(int i=0; i<size; i++)
	{
		this->set(i, arr[i]);
	}

	return 0;
}

int DynArr::getlength()
{
	return this->n;
}
int DynArr::setlength(int size)
{
	this->n = size;
	return 0;
}
double DynArr::get(int i)
{
	return(this->array[i]);
}
int DynArr::set(int i, double value)
{
	if(i<this->getlength())
	{
		this->array[i] = value;
	}
	else return 1;
	return 0;
}

int DynArr::print(char *str)
{
	double val;
	if(str==NULL) return 1;

	char *field;
	field = new char[15];

	strcpy(str, "");
	strcpy(field, "");
	for(int i=0; i<this->getlength(); i++)
	{
		val = this->get(i);
		if(fabs(val)<FTOE) sprintf(field, "%11.8e\t", this->get(i));
		else sprintf(field, "%11.8f\t", this->get(i));
		strcat(str, field);
	}
	return 0;
}

////////////myvector//////////////////////

myvector::myvector() : DynArr()
{
//	this->initMEM(0);
//	this->setlength(0);
}

myvector::myvector(int size) : DynArr(size)
{
//	this->initMEM(size);
//	this->setlength(size);
}

myvector::myvector(int size, double *arr) : DynArr(size, arr)
{
//	this->initArr(size, arr);
}

myvector::myvector(myvector &vt)
{
	int i, n;
	n = vt.getlength();

	double *arr;
	arr = (double*)calloc(n, sizeof(double));
	
	for(i=0; i<n; i++)
	{
		arr[i] = vt.get(i);
	}

	this->initMEM(n);
	this->initArr(n, arr);
}

myvector::myvector(myvector *vt)
{
	int i, n;
	n = vt->getlength();

	double *arr;
	arr = (double*)calloc(n, sizeof(double));

	
	
	for(i=0; i<n; i++)
	{
		arr[i] = vt->get(i);
	}

	this->initMEM(n);
	this->initArr(n, arr);
}

myvector::~myvector()
{
//	free(this->array);
//	this->array = NULL;
}


int myvector::initStr(char *str)
{
	int n, i, pos, lth;
	double num;
	char *slo;
	slo = new char[10];

	i=0;
	n = strlen(str);
	pos = 0;
	do
	{
		pos = this->slovoSP(str, slo, pos);
		num = atof(slo);
		lth = this->getlength();
		lth++;
		this->setlength(lth);
		this->set(i, num);
		i++;
	}while(pos<n);

	return 0;
}

int myvector::initFile(char *fname)
{
	return 0;
}

int myvector::push(double value)
{
	int n;
	n = this->getlength();
	n++;
	if(n>MSIZE) return 1;
	this->setlength(n);
	this->set(n-1, value);
	return 0;
}
int myvector::pop()
{
	int n = this->getlength();
	this->setlength(--n);
	return 0;
}


int myvector::slovoSP(char *str, char *slovo, int fpos)
{
	int len1, len2, len;
	char *slo;

	len = strlen(str);

	if(fpos==len)
	{
		strcpy(slovo, "");
		return fpos;
	}

	slo = new char[strlen(&str[fpos])];

	if(!(str!=NULL)||!(slovo!=NULL)) return 1;

	do
	{
		len1 = strlen(&str[fpos]);
		slo = strstr(&str[fpos], " ");
		if(slo==NULL) break;
		len2 = strlen(slo);
		if(len1==len2)
		{
			fpos++;
		}
		else break;

	}while(fpos<len);

	strcpy(slovo, "");
	strncat(slovo, &str[fpos], len1-len2);

	len1 = strlen(slovo);
	return(fpos + len1);

}

myvector& operator+(myvector &V1, myvector &V2)
{
	myvector *V;

	if(V1.getlength()==V2.getlength())
	{
		V = new myvector(V1.getlength());
	}
	else 
	{
		V = new myvector(0);
		return(*V);
	}

	for(int i=0; i<V1.getlength(); i++)
	{
		V->set(i, V1.get(i)+V2.get(i));
	}

	return *V;
}

myvector& operator-(myvector &V1, myvector &V2)
{
	myvector *V;

	if(V1.getlength()==V2.getlength())
	{
		V = new myvector(V1.getlength());
	}
	else 
	{
		V = new myvector(0);
		return(*V);
	}

	for(int i=0; i<V1.getlength(); i++)
	{
		V->set(i, V1.get(i)-V2.get(i));
	}

	return *V;
}

myvector& operator*(myvector &V, double a)
{
	myvector *v;

	v = new myvector(V.getlength());

	for(int i=0; i<V.getlength(); i++)
	{
		v->set(i, V.get(i)*a);
	}

	return *v;
}

myvector& operator*(double a, myvector &V)
{
	myvector *v;

	v = new myvector(V.getlength());

	for(int i=0; i<V.getlength(); i++)
	{
		v->set(i, V.get(i)*a);
	}

	return *v;
}

myvector& myvector::operator=(myvector &source)
{
	int i;
	setlength(source.getlength());

	for(i=0; i<source.getlength(); i++)
	{
		this->set(i, source.get(i));
	}
	return *this;
}
/*
myvector& myvector::operator-(myvector &source)
{
	int i;
	setlength(source.getlength());

	for(i=0; i<source.getlength(); i++)
	{
		this->set(i, -source.get(i));
	}
	return *this;
}
/

double myvector::norm()
{
	double nm = 0.0;
	int len;
	len = this->getlength();

	for(int i = 0; i<len; i++)
	{
		nm += this->get(i)*this->get(i);
	}

	return(sqrt(nm));
}

int myvector::RotZ(double angr)
{
	myvector *xt;
	xt = new myvector(3);

	xt->set(0, this->get(0));
	xt->set(1, this->get(1));
	xt->set(2, this->get(2));

	this->set(0, cos(angr)*xt->get(0) - sin(angr)*xt->get(1));
	this->set(1, sin(angr)*xt->get(0) + cos(angr)*xt->get(1));
	this->set(2, xt->get(2));

	return 0;
}

int myvector::RotX(double angr)
{
	myvector *xt;
	xt = new myvector(3);

	xt->set(0, this->get(0));
	xt->set(1, this->get(1));
	xt->set(2, this->get(2));

	this->set(0, xt->get(0));
	this->set(1, cos(angr)*xt->get(1) - sin(angr)*xt->get(2));
	this->set(2, sin(angr)*xt->get(1) + cos(angr)*xt->get(2));

	return 0;
}

///////////////////////////////////////Dyn2DArr

Dyn2DArr::Dyn2DArr()
{
		this->initMEM(0, 0);
//		this->setRS(0);
//		this->setCS(0);
}

Dyn2DArr::Dyn2DArr(int Rsize, int Csize)
{
	this->initMEM(Rsize, Csize);
}

Dyn2DArr::Dyn2DArr(int Rsize, int Csize, double **arr)
{
	this->initMEM(Rsize, Csize);

	if((Rsize<=MRSIZE)&&(Csize<=MCSIZE))
	{
		this->initArr(Rsize, Csize, arr);
	}
	else this->initArr(0, 0, NULL);
}

Dyn2DArr::~Dyn2DArr()
{
	for(int i=0; i<this->getRS(); i++)
	{
		delete [] this->array[i];
	}
	delete [] this->array;
	this->array = NULL;
}

int Dyn2DArr::initMEM(int Rsize, int Csize)
{
	this->array = (double**)calloc(Rsize, sizeof(double*));
//	this->array = new double*;

	this->setDIM(Rsize, Csize);
	for(int i=0; i<Rsize; i++)
	{
		this->array[i] = (double*)calloc(Csize, sizeof(double));
//		this->array[i] = new double[Csize];
	}
	return 0;
}

int Dyn2DArr::initArr(int Rsize, int Csize, double **arr)
{

	if((Rsize==0)||(Csize==0)||(arr==NULL)) return 1;
//	this->initMEM(Rsize, Csize);
//	this->setRS(Rsize);
//	this->setCS(Csize);
	for(int i=0; i<Rsize; i++)
	{
		for(int j=0; j<Csize; j++)
		{
			this->set(i, j, arr[i][j]);
		}
	}

	return 0;
}

int Dyn2DArr::getRS()
{
	return this->RS;
}

int Dyn2DArr::getCS()
{
	return this->CS;
}

int Dyn2DArr::setRS(int Rsize)
{
	this->RS = Rsize;
	return 0;
}

int Dyn2DArr::setCS(int Csize)
{
	this->CS = Csize;
	return 0;
}

int Dyn2DArr::setDIM(int Rsize, int Csize)
{
	this->setRS(Rsize);
	this->setCS(Csize);

	return 0;
}

double Dyn2DArr::get(int i, int j)
{
	return(this->array[i][j]);
}
int Dyn2DArr::set(int i, int j, double value)
{
	if((i<this->RS)&&(j<this->CS))
	{
		this->array[i][j] = value;
	}
	else return 1;

	return 0;
}

int Dyn2DArr::print(char *str)
{
	if(str==NULL) return 1;

	char *field;
	field = new char[15];

	strcpy(str, "");

	int ndim, mdim;
	ndim = this->getRS();
	mdim = this->getCS();
	
	for(int i=0; i<ndim; i++)
	{
		for(int j=0; j<mdim; j++)
		{
			strcpy(field, "");
			sprintf(field, "%e\t", this->get(i, j));
			strcat(str, field);
		}
		strcat(str, "\n");
	}

	return 0;
}

///////////////////////////mymatrix/////////////////////////////////

mymatrix::mymatrix() : Dyn2DArr()
{
//	this->initMEM(0, 0);
//	this->setRS(0);
//	this->setCS(0);
}

mymatrix::mymatrix(int Rsize, int Csize) : Dyn2DArr(Rsize, Csize)
{
//	this->initMEM(Rsize, Csize);

//	this->setRS(Rsize);
//	this->setCS(Csize);
}

mymatrix::mymatrix(int Rsize, int Csize, double **arr) : Dyn2DArr(Rsize, Csize, arr)
{
//	this->initMEM(Rsize, Csize);
//	this->initArr(Rsize, Csize, arr);
}

mymatrix::mymatrix(mymatrix &matr)
{
	int i, j, n, m;
	n = matr.getRS();
	m = matr.getCS();

	double **arr;
	arr = (double**)calloc(n, sizeof(double*));

	
	
	for(i=0; i<n; i++)
	{
		arr[i] = (double*)calloc(m, sizeof(double));
		for(j = 0; j<m; j++)
		{
			arr[i][j] = matr.get(i, j);
		}
	}

	this->initMEM(n, m);
	this->initArr(n, m, arr);
}

mymatrix::mymatrix(mymatrix *matr)
{
	int i, j, n, m;
	n = matr->getRS();
	m = matr->getCS();

	double **arr;
	arr = (double**)calloc(n, sizeof(double*));
	
	for(i=0; i<n; i++)
	{
		arr[i] = (double*)calloc(m, sizeof(double));
		for(j = 0; j<m; j++)
		{
			arr[i][j] = matr->get(i, j);
		}
	}

	this->initMEM(n, m);
	this->initArr(n, m, arr);
}

mymatrix::~mymatrix()
{
/*	for(int i=0; i<this->getRS(); i++)
	{
		free(this->array[i]);
	}

	free(this->array);
	this->array = NULL;/
}

int mymatrix::initStr(char *str)
{
	int n, m, i, j, jj, Rpos, Cpos, sl;
	double num;
	char *slo, *slostr;
	slo = new char[10];
	slostr = new char[1000];

	n = strlen(str);
	
	i = Rpos = 0;
	do
	{
//		if(Rpos==n) return 0;
		Rpos = this->slovoNL(str, slostr, Rpos);
		sl = strlen(slostr);
		if(sl==0) continue;
		m = strlen(slostr);
		
		j = Cpos = 0;
		while(Cpos<m)
		{
			Cpos = this->slovoSP(slostr, slo, Cpos);
			sl = strlen(slo);
			if(sl==0) continue;
			num = atof(slo);
//			lth = this->getClength();
//			lth++;
			
			this->set(i, j, num);
			j++;
		}
		
		if(i==0)
		{
			jj = j;
			this->setCS(jj);
		}
		else if(jj!=j)
		{
			this->setCS(0);
			this->setRS(0);
			return 1;
		}

		

		i++;
		
	}while(Rpos<n);

	this->setRS(i);

	return 0;
}

int mymatrix::initFile(char *fname)
{
	return 0;
}

int mymatrix::slovoSP(char *str, char *slovo, int fpos)
{
	int len1, len2, len;
	char *slo;

	len = strlen(str);

	if(fpos==len)
	{
		strcpy(slovo, "");
		return fpos;
	}

	slo = new char[strlen(&str[fpos])];

	if(!(str!=NULL)||!(slovo!=NULL)) return 1;

	do
	{
		len1 = strlen(&str[fpos]);
		slo = strstr(&str[fpos], " ");
		if(slo==NULL) break;
		len2 = strlen(slo);
		if(len1==len2)
		{
			fpos++;
		}
		else break;

	}while(fpos<len);

	strcpy(slovo, "");
	strncat(slovo, &str[fpos], len1-len2);

	len1 = strlen(slovo);
	return(fpos + len1);

//	return 0;
}

int mymatrix::slovoNL(char *str, char *slovo, int fpos)
{
	int len1, len2, len;
	char *slo;

	len = strlen(str);

	if(fpos==len)
	{
		strcpy(slovo, "");
		return fpos;
	}

	slo = new char[strlen(&str[fpos])];

	if(!(str!=NULL)||!(slovo!=NULL)) return 1;

	do
	{
		len1 = strlen(&str[fpos]);
		slo = strstr(&str[fpos], "\n");
		if(slo==NULL) break;
		len2 = strlen(slo);
		if(len1==len2)
		{
			fpos++;
		}
		else break;

	}while(fpos<len);

	strcpy(slovo, "");
	strncat(slovo, &str[fpos], len1-len2);

	len1 = strlen(slovo);
	return(fpos + len1);

//	return 0;
}

int mymatrix::isSqr()
{
	return(this->getCS()==this->getRS());
}

int mymatrix::isSym()
{
	int i, j, n;
	if(!isSqr()) return 0;

	n = this->getRS();
	for(i=0; i<n; i++)
	{
		for(j=0; j<n; j++)
		{
			if(this->get(i, j)!=this->get(j, i)) return 0;
		}
	}

	return 1;
}

mymatrix& operator+(mymatrix &M1, mymatrix &M2)
{

	mymatrix *M;

	if((M1.getRS()==M2.getCS())&&(M1.getRS()==M2.getRS()))
	{
		M = new mymatrix(M1.getRS(), M1.getCS());
	}
	else 
	{
		M = new mymatrix(0, 0);
		return(*M);
	}

	for(int i=0; i<M1.getRS(); i++)
	{
		for(int j=0; j<M1.getCS(); j++)
		{
			M->set(i, j, M1.get(i, j)+M2.get(i, j));
		}
	}

	return *M;
}

mymatrix& operator-(mymatrix &M1, mymatrix &M2)
{
	mymatrix *M;

	if((M1.getRS()==M2.getRS())&&(M1.getRS()==M2.getRS()))
	{
		M = new mymatrix(M1.getRS(), M1.getCS());
	}
	else 
	{
		M = new mymatrix(0, 0);
		return(*M);
	}

	for(int i=0; i<M1.getRS(); i++)
	{
		for(int j=0; j<M1.getCS(); j++)
		{
			M->set(i, j, M1.get(i, j)-M2.get(i, j));
		}
	}

	return *M;
}

mymatrix& operator*(mymatrix &M, double a)
{
	mymatrix *m;

	m = new mymatrix(M.getRS(), M.getCS());

	for(int i=0; i<M.getRS(); i++)
	{
		for(int j=0; j<M.getCS(); j++)
		{
			m->set(i, j, M.get(i, j)*a);
		}
	}

	return *m;
}

mymatrix& operator*(double a, mymatrix &M)
{
	mymatrix *m;

	m = new mymatrix(M.getRS(), M.getCS());

	for(int i=0; i<M.getRS(); i++)
	{
		for(int j=0; j<M.getCS(); j++)
		{
			m->set(i, j, M.get(i, j)*a);
		}
	}

	return *m;
}

mymatrix& mymatrix::operator=(mymatrix &source)
{

	int i, j;
	initMEM(source.getRS(), source.getCS());
//	setRS(source.getRS());
//	setCS(source.getCS());

	for(i=0; i<source.getRS(); i++)
	{
		for(j=0; j<source.getCS(); j++)
		{
			this->set(i, j, source.get(i, j));
		}
	}

	return *this;
}

myvector& mymatrix::column(int j)
{
	myvector *v;
	int n = this->getRS();
	int i;
	v = new myvector(n);

	for(i=0; i<n; i++) v->set(i, this->get(i, j));

	return *v;
}

myvector& mymatrix::row(int i)
{
	myvector *v;
	int n = this->getCS();
	int j;
	v = new myvector(n);

	for(j=0; j<n; j++) v->set(j, this->get(i, j));

	return *v;
}

myvector& mymatrix::diag()
{
	int j;
	myvector *V;

	if(!this->isSqr())
	{
		V = new myvector(0);
		return *V;
	}


	V = new myvector(this->getCS());

	for(j=0; j<this->getCS(); j++)
	{
		V->set(j, this->get(j, j));
	}

	return *V;
}

mymatrix& operator*(mymatrix &M1, mymatrix &M2)
{
	int i, j;
	int M2m, M2n, M1m, M1n, Mn, Mm;
	double val;
	mymatrix *M;

	M2n = M2.getRS();
	M1m = M1.getCS();
	M2m = M2.getCS();
	M1n = M1.getRS();

	if(M1m==M2n)
	{
		M = new mymatrix(M1n, M2m);
	}
	else 
	{
		M = new mymatrix(0, 0);
		return *M;
	}

	Mn = M1n;
	Mm = M2m;

	for(i=0; i<Mn; i++)
	{
		for(j=0; j<Mm; j++)
		{
			val = Smul(&M1.row(i),&M2.column(j));
			M->set(i, j, val);
		}
	}

	return *M;
}


myvector& operator*(mymatrix &M1, myvector &V2)
{
	int i, j;
	double val;
	myvector *V;

	if(M1.getCS()==V2.getlength())
	{
		V = new myvector(M1.getCS());
	}
	else 
	{
		V = new myvector(0);
		return *V;
	}

	for(i=0; i<M1.getRS(); i++)
	{
		val = 0.0;
		for(j=0; j<M1.getCS(); j++)
		{
				val += M1.get(i, j)*V2.get(j);
		}
		V->set(i, val);
	}

	
	return *V;
}

myvector& operator*(myvector &V1, mymatrix &M2)
{
	int i, j;
	double val;
	myvector *V;

	if(V1.getlength()==M2.getRS())
	{
		V = new myvector(M2.getCS());
	}
	else 
	{
		V = new myvector(M2.getCS());
		return *V;
	}

	for(i=0; i<M2.getCS(); i++)
	{
		val = 0.0;
		for(j=0; j<M2.getRS(); j++)
		{
				val += V1.get(j)*M2.get(j, i);
		}
		V->set(i, val);
	}

	return *V;
}

int mymatrix::inverse()
{
	if(!this->isSqr())
	{
		return 1;
	}
/*
	char *pm;
	pm = new char[1000];
	FILE *FNM;
	FNM = fopen("inv.log", "w");
/
	int i, j, k, n, m;
	double g, val;
	n = this->getRS();
	m = n*2.0;
	mymatrix V(n, m), E(n, n);

	for(i=0; i<n; i++)
	{
		for(j=0; j<m; j++)
		{
			if(j<n) V.set(i, j, this->get(i, j));
			else V.set(i, j, kron(i, j-n));
		}
	}

/*
	V.print(pm);
	fprintf(FNM, "\nV:\n%s\n", pm);
/

	for(k=0; k<n-1; k++)
	{
		g = 0.0;
		for(i=k+1; i<n; i++)
		{
			g = V.get(i, k)/V.get(k, k);
			val = 0.0;
			for(j=k; j<m; j++)
			{
				val = V.get(i, j) - V.get(k, j)*g;
				V.set(i, j, val);
			}
		}
	}
/*
	V.print(pm);
	fprintf(FNM, "\nV:\n%s\n", pm);
/
	for(k=n-1; k>0; k--)
	{
		g = 0.0;
		for(i=k-1; i>=0; i--)
		{
			g = V.get(i, k)/V.get(k, k);
			val = 0.0;
			for(j=k; j<m; j++)
			{
				val = V.get(i, j) - V.get(k, j)*g;
				V.set(i, j, val);
			}
		}
	}
/*
	V.print(pm);
	fprintf(FNM, "\nV:\n%s\n", pm);
/
	for(i=0; i<n; i++)
	{
		g = 1.0/V.get(i, i);
		val = 0.0;
		for(j=0; j<m; j++)
		{
			val = V.get(i, j)*g;
			V.set(i, j, val);
			if(j>=n) this->set(i, j-n, val);
		}
	}

/*
	V.print(pm);
	fprintf(FNM, "\nV:\n%s\n", pm);

	this->print(pm);
	fprintf(FNM, "\nthis:\n%s\n", pm);
/
	return 0;
}


//////////////////////////////////////////////////////////////////////////

double Smul(myvector *V1, myvector *V2)
{
	if(V2->getlength()!=V1->getlength())
	{
		return 1;
	}

	int i;
	double val;

	val = 0.0;
	for(i=0; i<V1->getlength(); i++)
	{
		val += V1->get(i)*V2->get(i);
	}
	return(val);
}


double kron(int i, int j)
{
	if(i==j) return(1.0);
	return(0.0);
}

int Lsys(mymatrix *A, myvector *b, myvector *x)
{
/*
	char *str;
	str = new char[1000];
/

	if(!A->isSqr())
	{
		return 1;
	}

	int n;
	mymatrix Al;
	myvector *xx;

	n = A->getRS();
	xx = new myvector(n);

	Al = *A;

/*
	Al.print(str);
	printf("\nAl:\n%s\n", str);
	A->print(str);
	printf("\nA:\n%s\n", str);
/

	Al.inverse();

/*
	Al.print(str);
	printf("\nAl inverse:\n%s\n", str);
/

	*x = Al*(*b);

	return 0;
}


int MNK(mymatrix *a, myvector *y, myvector *x, myvector *sg)
{
	char *pm;
	pm = new char[1000];
	int m, n;
	m = a->getCS();
	n = a->getRS();

	if(a->getRS()!=y->getlength()) return 1;
//	n = a->rowlength();
	
	mymatrix *c;
	myvector *b;
	double num;

	c = new mymatrix(m, m);
	b = new myvector(m);

	int k, j, i;

	for(k=0; k<m; k++)
	{
		for(j=0; j<m; j++)
		{
			num = 0.0;
			for(i=0; i<n; i++)
			{
				num += a->get(i, k)*a->get(i, j);
			}
			c->set(k, j, num);
		}
		num = 0.0;
		for(i=0; i<n; i++)
		{
			num += a->get(i, k)*y->get(i);
		}
		b->set(k, num);
	}

	Lsys(c, b, x);

	if(sg!=NULL)
	{
//		sg = new myvector(m);
		double sgma, Eeps, Ey, Eby, Eay, E2, Eb;

		Eeps = 0.0;
		Ey = 0.0;
		for(i=0; i<n; i++)
		{
			Ey += y->get(i)*y->get(i);
		}

		E2 = 0.0;
		E2 = Smul(x, b);
/*		for(j=0; j<m; j++)
		{

			Eby = Eay = 0.0
			for(i=0; i<n; i++)
			{
				Eay += a->get(i, j)*y->get(i);
				Eby += b->get(i)
			}
			Ex += x->get(j)*Eay;
		}/

		Eeps = fabs(Ey - E2);
	
		sgma = sqrt(Eeps/(n-m));

		for(j=0; j<m; j++)
		{
			Eb = det(Minor(c, j, j))/det(c);
			sg->set(j, sgma*sqrt(Eb));
		}

	}

	return 0;
}

myvector& Vmul(myvector *V1, myvector *V2)
{
	if((V1->getlength()!=V2->getlength())||(V1->getlength()<3)||(V2->getlength()<3))
	{
//		cout << endl << "error: metod yakobi ne goditsya" << endl;
		printf("error: nel'zya peremnogit' vectora");
		exit(1);
	}
	int i, j, N;
	N = V1->getlength();
	myvector *V;
	mymatrix *M;
	M = new mymatrix(3, N);

	double val;
	V = new myvector(N);
	for(i=0; i<3; i++)
	{
		for(j=0; j<N; j++)
		{
			if(i==0) val = 1.0;
			if(i==1) val = V1->get(j);
			if(i==2) val = V2->get(j);
			M->set(i, j, val);
		}
	}
/*
	char *pm;
	pm = new char[1000];
	M->print(pm);
	printf("M:\n%s\n", pm);
/
	for(i=0; i<N; i++)
	{
		V->set(i, pow(-1.0, i+2)*det(Minor(M, 0, i)));
	}
//
/*	V->set(0, V1->get(1)*V2->get(2) - V1->get(2)*V2->get(1));
	V->set(1, V1->get(2)*V2->get(0) - V1->get(0)*V2->get(2));
	V->set(2, V1->get(0)*V2->get(1) - V1->get(1)*V2->get(0));
/
	delete M;

	return *V;

}


mymatrix& Minor(mymatrix *M, int i, int j)
{
	if((M->getRS()==1)&&(M->getCS()==1))
	{
		return *M;
	}
	mymatrix *MN;
	MN = new mymatrix(M->getRS()-1, M->getCS()-1);
	int p, q, k, l;

	for(p=0, k=0; p<M->getRS(); p++)
	{
		if(p==i) continue;
		for(q=0, l=0; q<M->getCS(); q++)
		{
			if(q==j) continue;
			MN->set(k, l, M->get(p, q));
			l++;
		}
		k++;
	}

	return *MN;
}

double det(mymatrix M)
{
	if((M.getRS()==1)&&(M.getCS()==1)) return M.get(0, 0);

	int i, j;
	double val;
/*
	char *pm;
	pm = new char[1000];
	M.print(pm);
	printf("M:\n%s\n", pm);
/


	j = 0;

	val = 0.0;
	for(i=0; i< M.getRS(); i++)
	{
//		if(fabs(M.get(i, j))>e) val += M.get(i, j)*pow(-1.0, i+j+2)*det((M, i, j));
		val += M.get(i, j)*pow(-1.0, i+j+2)*det(Minor(&M, i, j));
	}

	return val;
}

#endif
*/