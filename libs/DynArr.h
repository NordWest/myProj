//#pragma once
#ifndef DA_H
#define DA_H

#define MSIZE 1000
#define MRSIZE 1000
#define MCSIZE 1000
#define FTOE 1.0e-3

//#define STRPRINT MSIZE*MRSIZE*MCSIZE

class DynArr;
class myvector;
class Dyn2DArr;
class mymatrix;

double Smul(myvector *V1, myvector *V2);
double det(mymatrix *M);
//void OrtR(mymatrix *M1, mymatrix *M2, myvector *V1, myvector *V2);
double kron(int i, int j);
//myvector& LsysOrt(mymatrix *A, myvector *b);
int Lsys(mymatrix *A, myvector *b, myvector *x);
//double SNgrade(mymatrix *M);//SelfNumber
//int Pruve(mymatrix M, double l);
//myvector& SNyakobi(mymatrix M);//SelfNumber
//void MaxNDMod(mymatrix M, int* i, int* j);
//void Vpq(mymatrix *V, mymatrix *M);
//double Mcos(mymatrix M, int p, int q);
//double Msin(mymatrix M, int p, int q);
//double Sign(double x);
myvector& Vmul(myvector *V1, myvector *V2);
//double VectCos(myvector *V1, myvector *V2);
mymatrix* Minor(mymatrix *M, int i, int j);
//double Smul(myvector *V1, myvector *V2);
int MNK(mymatrix *a, myvector *y, myvector *x, myvector *sg);
double det_average(myvector *x);
int IntAlongCurv(myvector *C, myvector *A, myvector *B, double ca, double cb, int direct);
int IntAlongCurv1(myvector *C, myvector *A, myvector *B, double ca, double cb);
int RotZ(myvector *x, myvector *x0, double angr);
int RotX(myvector *x, myvector *x0, double angr);
double VectAng(myvector *V1, myvector *V2);


//#ifndef DA_H
class DynArr
{
protected:
	double *array;
	int n;

	int initMEM(int Rsize);
	int initArr(int size, double *arr);

public:
	int getlength();
	double get(int i);

	int setlength(int size);
	int set(int i, double value);

	DynArr();
	DynArr(int size);
	DynArr(int size, double *arr);
	~DynArr();

	int print(char *str);
};

class myvector: public DynArr
{
private:
	int slovoSP(char *str, char *slovo, int fpos);
	int initStr(char *str);
	int initFile(char *fname);

public:
	myvector();
	~myvector();
	myvector(int size);
	myvector(int size, double *arr);
	myvector(myvector &vect);
	myvector(myvector *vt);

	int push(double value);
	int pop();

	myvector& operator=(myvector &source);
//	myvector& operator-(myvector &source);


	friend myvector& operator+(myvector &V1, myvector &V2);	//WARN use only like this: *y = *x1 + *x2;
	friend myvector& operator-(myvector &V1, myvector &V2);

	friend myvector& operator*(double a, myvector &V);
	friend myvector& operator*(myvector &V, double a);

	int RotZ(double angr);
	int RotX(double angr);

	double norm();
};
//////////////////////////////////////

class Dyn2DArr
{
protected:
	double **array;
	int RS;
	int CS;
	int initArr(int Rsize, int Csize, double **arr);
	int initMEM(int Rsize, int Csize);

	int setRS(int size);
	int setCS(int size);
	int setDIM(int Rsize, int Csize);


public:
	int getRS();
	int getCS();

	double get(int i, int j);
	int set(int i, int j, double value);

//public:	
	Dyn2DArr();
	Dyn2DArr(int Rsize, int Csize);
	Dyn2DArr(int Rsize, int Csize, double **arr);
	~Dyn2DArr();
	
	int print(char *str);
};

class mymatrix: public Dyn2DArr
{
private:
	int slovoSP(char *str, char *slovo, int fpos);
	int slovoNL(char *str, char *slovo, int fpos);

	int initStr(char *str);
	int initFile(char *fname);

public:
	int isSqr();
	int isSym();
	void upR(){this->RS++;};
	void upC(){this->CS++;};


	
	mymatrix();
	~mymatrix();
	mymatrix(int Rsize, int Csize);
	mymatrix(int Rsize, int Csize, double **arr);
//	mymatrix(mymatrix matr);
	mymatrix(mymatrix &matr);
	mymatrix(mymatrix *matr);

	myvector& column(int j);
	myvector& row(int i);
	myvector& diag();

	mymatrix& operator=(mymatrix &source);
//	mymatrix& operator-(mymatrix &source);

	friend mymatrix& operator+(mymatrix &V1, mymatrix &V2);	//WARN use only like this: *y = *x1 + *x2;
	friend mymatrix& operator-(mymatrix &V1, mymatrix &V2);

	friend mymatrix& operator*(double a, mymatrix &V);
	friend mymatrix& operator*(mymatrix &V, double a);

	friend mymatrix& operator*(mymatrix &M1, mymatrix &M2);
	friend myvector& operator*(mymatrix &M1, myvector &V2);
	friend myvector& operator*(myvector &V1, mymatrix &M2);



	int inverse();
};
/*
double Smul(myvector *V1, myvector *V2);
double det(mymatrix M);
//void OrtR(mymatrix *M1, mymatrix *M2, myvector *V1, myvector *V2);
double kron(int i, int j);
//myvector& LsysOrt(mymatrix *A, myvector *b);
int Lsys(mymatrix *A, myvector *b, myvector *x);
//double SNgrade(mymatrix *M);//SelfNumber
//int Pruve(mymatrix M, double l);
//myvector& SNyakobi(mymatrix M);//SelfNumber
//void MaxNDMod(mymatrix M, int* i, int* j);
//void Vpq(mymatrix *V, mymatrix *M);
//double Mcos(mymatrix M, int p, int q);
//double Msin(mymatrix M, int p, int q);
//double Sign(double x);
myvector& Vmul(myvector *V1, myvector *V2);
//double VectCos(myvector *V1, myvector *V2);
mymatrix& Minor(mymatrix *M, int i, int j);
//double Smul(myvector *V1, myvector *V2);
int MNK(mymatrix *a, myvector *y, myvector *x, myvector *sg);
double det_average(myvector *x);
int IntAlongCurv(myvector *C, myvector *A, myvector *B, double ca, double cb, int direct);
int IntAlongCurv1(myvector *C, myvector *A, myvector *B, double ca, double cb);
int RotZ(myvector *x, myvector *x0, double angr);
int RotX(myvector *x, myvector *x0, double angr);
double VectAng(myvector *V1, myvector *V2);
*/

//////////////////////////////////////////////

#endif

