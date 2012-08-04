//Matrixes and Vectors

#include <fstream>
#include <istream>
//#include <strstrea.h>
#include <stdio.h>
//#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define e 1e-100

struct vect;
struct matr;
class matrix;
class vector;


double det(matrix M);
double Smul(vector *V1, vector *V2);
void OrtR(matrix *M1, matrix *M2, vector *V1, vector *V2);
double kron(int i, int j);
vector& LsysOrt(matrix *A, vector *b);
vector& Lsys(matrix *A, vector *b);
double SNgrade(matrix M);//SelfNumber
int Pruve(matrix M, double l);
vector& SNyakobi(matrix M);//SelfNumber
void MaxNDMod(matrix M, int* i, int* j);
void Vpq(matrix *V, matrix *M);
double Mcos(matrix M, int p, int q);
double Msin(matrix M, int p, int q);
double Sign(double x);
vector& Vmul(vector *V1, vector *V2);
double VectCos(vector *V1, vector *V2);
matrix& Minor(matrix *M, int i, int j);

struct matr
{
	int i, j;
	double value;
	matr *next;
};

class matrix
{
protected:
	matr *M;
public:
	int n;
	int m; //nxm

	matrix();
	matrix(int I, int J);
	matrix(ifstream *file);
	matrix(matrix* M);

	int set(int i, int j, double value);
	double get(int i, int j);
	int add(int i, int j, double value);
	int ded(int i, int j, double value);
	char* print();
	vector& column(int j);
	vector& row(int i);
	vector& diag();
	int is_square();
	int is_symm();

	friend matrix& operator+(matrix M1, matrix M2);
	matrix& operator+=(matrix &M1);
	friend matrix& operator-(matrix M1, matrix M2);
	matrix& operator-=(matrix &M1);
	friend matrix& operator*(matrix M1, matrix M2);
	friend matrix& operator*(matrix M1, double a);
	friend matrix& operator*(double a, matrix M1);
	matrix& operator*=(matrix &M1);
	friend vector& operator*(matrix M1, vector V2);
	friend vector& operator*(vector V1, matrix M2);
	matrix& operator=(matrix &source);

	void inverse();
	void trans();
};

struct vect
{
	int i;
	double value;
	vect *next;
};

class vector
{
public:
	vect *V;

	int n;
	vector();
	vector(int I);
	vector(ifstream *file);
	vector(vector* V);

	int set(int i, double value);
//	int setlen(int i);
	double get(int i);
	int add(int i, double value);
	int ded(int i, double value);
	char* print();

	friend vector& operator+(vector V1, vector V2);
	vector& operator+=(vector &V1);
	friend vector& operator-(vector V1, vector V2);
	vector& operator-=(vector &V1);
	vector& operator*=(double &a);
	friend matrix& operator*(matrix V1, double a);
	friend matrix& operator*(double a, matrix V1);
	vector& operator=(vector &source);
};
