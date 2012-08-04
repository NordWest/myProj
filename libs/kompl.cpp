//#include "stdafx.h"
#include "kompl.h"

//#include <iostream.h>

kompl::kompl(double Re, double Im)
{
	this->Z = new klx;
	this->Z->Re = Re;
	this->Z->Im = Im;
}

kompl::kompl()
{
	this->Z = new klx;
}

kompl::kompl(kompl &source)
{
    this->Z = new klx;
    this->Z->Re = source.Re();
    this->Z->Im = source.Im();
}


double kompl::Re()
{
	return(this->Z->Re);
}

double kompl::Im()
{
	return(this->Z->Im);
}

double kompl::mod()
{
	return(sqrt(this->Z->Re*this->Z->Re + this->Z->Im*this->Z->Im));
}

void kompl::setVal(double valRe, double valIm)
{
    this->Z->Re = valRe;
    this->Z->Im = valIm;
}

void kompl::setRe(double val)
{
	this->Z->Re = val;
}



void kompl::setIm(double val)
{
	this->Z->Im = val;
}

void kompl::addRe(double val)
{
	this->Z->Re += val;
}

void kompl::addIm(double val)
{
	this->Z->Im += val;
}

void kompl::redRe(double val)
{
	this->Z->Re -= val;
}

void kompl::redIm(double val)
{
	this->Z->Im -= val;
}

kompl& kompl::ks()
{
	kompl *k;
	k = new kompl(this->Re(), -this->Im());
	return *k;
}


kompl& operator+(kompl k1, kompl k2)
{
    kompl k(k1);
        //k = new kompl(k1.Re(), k1.Im());
        k.addRe(k2.Re());
        k.addIm(k2.Im());
//	cout << k->Re() << " +i" << k->Im();
        return k;
}

kompl& operator-(kompl k1, kompl k2)
{
    kompl k(k1);
        //k = new kompl(k1.Re(), k1.Im());
        k.redRe(k2.Re());
        k.redIm(k2.Im());
//	cout << k->Re() << " +i" << k->Im();
        return k;
}

void kompl::operator=(kompl source)
{
//	copy( source.Re, source.Im);
        setRe(source.Re());
	setIm(source.Im());
        //return *this;
}
/*
kompl& kompl::operator*(const kompl &k1, const kompl &k2)
{
    kompl k;
    k.setRe(k1.Re()*k2.Re() - k1.Im()*k2.Im());
    k.setIm(k1.Re()*k2.Im() + k1.Im()*k2.Re());
    return &k;
}
*/
/*
kompl& kompl::operator*(kompl &k2)
{
    kompl k;
    k.setRe(this->Re()*k2.Re() - this->Im()*k2.Im());
    k.setIm(this->Re()*k2.Im() + this->Im()*k2.Re());
    return k;
}

kompl& kompl::operator*(kompl *k2)
{
    kompl k;
    k.setRe(this->Re()*k2->Re() - this->Im()*k2->Im());
    k.setIm(this->Re()*k2->Im() + this->Im()*k2->Re());
    return k;
}
*/

/*
kompl kompl::operator*(kompl k2)
{
        kompl k;
        //k = new kompl;
        k.setRe(this->Re()*k2.Re() - this->Im()*k2.Im());
        k.setIm(this->Re()*k2.Im() + this->Im()*k2.Re());
//	cout << k->Re() << " +i" << k->Im();
        return k;
}
*/
kompl& operator*(kompl k1, double a)
{
	kompl *k;
	k = new kompl(k1.Re()*a, k1.Im()*a);
	return *k;
}

kompl& operator*(double a, kompl k1)
{
	kompl *k;
	k = new kompl(k1.Re()*a, k1.Im()*a);
	return *k;
}

kompl& operator/(kompl k1, kompl k2)
{
    kompl k;
    k.setRe((k1.Re()*k2.Re() + k1.Im()*k2.Im())/(k2.Re()*k2.Re() + k2.Im()*k2.Im()));
    k.setIm((k1.Re()*k2.Im() - k1.Im()*k2.Re())/(k2.Re()*k2.Re() + k2.Im()*k2.Im()));
    return k;
}

kompl& operator/(kompl k1, double a)
{
	kompl *k;
	k = new kompl(k1.Re()/a, k1.Im()/a);
	return *k;
}
/*
kompl& operator/(double a, kompl k1)
{
	kompl *k;
	k = new kompl(k1.Re()/a, k1.Im()/a);
	return *k;
}
/*
void operator*=(kompl k1, kompl k2)
{
	double k1R, k1I;
	k1R = k1.Re();
	k1I = k1.Im();
	k1.setRe(k1R*k2.Re() - k1I*k2.Im());
	k1.setIm(k1R*k2.Im() + k1I*k2.Re());
}
*/
kompl kompl::operator+=(kompl k2)
{
    this->setRe(this->Re()+k2.Re());
    this->setIm(this->Im()+k2.Im());
    return *this;
}

kompl kompl::operator-=(kompl k2)
{
    this->setRe(this->Re()-k2.Re());
    this->setIm(this->Im()-k2.Im());
    return *this;
}

kompl kompl::operator*=(kompl k2)
{
    kompl k1(*this);

    //*this = k1*k2;

    this->setRe(k1.Re()*k2.Re() - k1.Im()*k2.Im());
    this->setIm(k1.Re()*k2.Im() + k1.Im()*k2.Re());

    return *this;
}

kompl kompl::operator/=(kompl k2)
{
    kompl k1(*this);

    *this = k1/k2;

    //this->setRe(k1.Re()*k2.Re() - k1.Im()*k2.Im());
    //this->setIm(k1.Re()*k2.Im() + k1.Im()*k2.Re());

    return *this;
}

kompl kompl::operator+=(double a)
{
    this->setRe(this->Re()+a);
    this->setIm(this->Im()+a);
    return *this;
}

kompl kompl::operator-=(double a)
{
    this->setRe(this->Re()-a);
    this->setIm(this->Im()-a);
    return *this;
}

kompl kompl::operator*=(double a)
{
    this->setRe(this->Re()*a);
    this->setIm(this->Im()*a);
    return *this;
}

kompl kompl::operator/=(double a)
{
    this->setRe(this->Re()/a);
    this->setIm(this->Im()/a);
    return *this;
}


/*
void operator+=(kompl k1, kompl *k2)
{
    k1.setRe(k1.Re()+k2->Re());
    k1.setIm(k1.Im()+k2->Im());
}
/
void operator*=(kompl k1, double k2)
{
	k1.setRe(k1.Re()*k2);
	k1.setIm(k1.Im()*k2);
}
*/
kompl::~kompl()
{
	delete this->Z;
}
