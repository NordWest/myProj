//#pragma once
//#include <stdio.h>
//#include <fstream.h>
//#include "mymatrix.h"

//#include <fstream>
//#include <istream>
//#include <strstrea.h>
#include <stdio.h>
//#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>



//#define NULL 0xcdcdcdcd

struct klx
{
	double Re;
	double Im;
};


class kompl
{
protected:
	klx *Z;
public:
	kompl();
        kompl(kompl &source);
        //kompl(kompl source);
	kompl(double Re, double Im);
	double Im();
	double Re();
        void setVal(double valRe, double valIm);
	void setRe(double val);
	void setIm(double val);
	void addRe(double val);
	void addIm(double val);
        void redRe(double val);
        void redIm(double val);
	kompl& ks();
	double mod();
        friend kompl& operator+(kompl k1, kompl k2);
	friend kompl& operator-(kompl k1, kompl k2);
        //friend kompl& operator*(const kompl &k1, const kompl &k2);
        //friend kompl& operator*(kompl &k2);
        //kompl& operator*(kompl *k2);
        //friend kompl& operator*(const kompl k2);

        //kompl operator*(kompl k1);
	friend kompl& operator*(kompl k1, double a);
	friend kompl& operator*(double a, kompl k1);
        friend kompl& operator/(kompl k1, kompl k2);
	friend kompl& operator/(kompl k1, double a);
        //friend kompl& operator/(double a, kompl k1);
        //friend void operator+=(kompl k1, kompl k2);
        kompl operator+=(kompl k2);
        kompl operator-=(kompl k2);
        kompl operator*=(kompl k2);
        kompl operator/=(kompl k2);

        kompl operator+=(double a);
        kompl operator-=(double a);
        kompl operator*=(double a);
        kompl operator/=(double a);
        //friend void operator*=(kompl k1, kompl k2);
        //friend void operator*=(kompl k1, double k2);
        void operator=(kompl source);
	~kompl();
};

//kompl& operator*(const kompl &k1, const kompl &k2);
