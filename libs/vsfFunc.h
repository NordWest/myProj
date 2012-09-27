#ifndef VSFFUNC_H
#define VSFFUNC_H

#include <QString>

long double fact(int k);                                   //factorial
long double fact2(int k1, int k2);                          //product from k1 to k2
long double PL(int N, int K, double B);                     //Legendre polinom P_nk, B - latitude in radians
int indexJ(int N, int K, int P);                            //index J from N,K,P
void indexes(int J, int& N, int& K, int& P);                //indexes N,K,P from J
QString indexesStr(int J);
long double usf(int N, int K, int P, double L, double B);   //unnormalized spherical function
long double FR(int N,int K);                                //norm of spherical function
long double sf(int N, int K, int P, double L, double B);    //normalized spherical function
long double sfj(int J, double L, double B);                 // normalized spherical function via index J
long double TL(int N, int K, int P, double L, double B);    //longtitudial component of toroidal function
long double TLJ(int J, double L, double B);
long double TB(int N, int K, int P, double L, double B);    //latitudial component of toroidal function
long double TBJ(int J, double L, double B);
long double SL(int N, int K, int P, double L, double B);    //longtitudial component of spheroidal function
long double SLJ(int J, double L, double B);
long double SB(int N, int K, int P, double L, double B);    //latitudial component of spheroidal function
long double SBJ(int J, double L, double B);



#endif // VSFFUNC_H
