#include <QtCore>
#include <QCoreApplication>
//#include "ang2pix_ring.c"
#include "pix2ang_ring.c"
#include "./../libs/astro.h"

long double fact(int k)//factorial
{
  long double f=1;
  for(int i=1;i<=k;i++) f=f*i;
  return f;
}

long double fact2(int k1, int k2)//product from k1 to k2
{
  long double f=1;
  for(int i=k1;i<=k2;i++) f=f*i;
  return f;
}

long double PL(int N, int K, double B) //Legendre polinom P_nk, B - latitude in radians
{
    long double n = (long double)N;
    long double k = (long double)K;
    if(N<K) return 0;
    if(N==K) return fact2(K+1,2*K)/pow(2,K)*pow(cos(B),K);
    if(N==K+1) return fact2(K+2,2*K+2)/pow(2,K+1)*pow(cos(B),K)*sin(B);
    return sin(B)*(2*n-1)/(n-k)*PL(N-1,K,B)-(n+k-1)/(n-k)*PL(N-2,K,B);
}

int indexJ(int N, int K, int P)//index J from N,K,P
{return N*N+2*K+P-1;}

void indexes(int J, int& N, int& K, int& P) //indexes N,K,P from J
{
    N = (int)sqrt((double)J);
    K = J-N*N;
    if(K % 2 == 0) P=1; else P=0;
    K=(K-P+1)/2;
}

QString indexesStr(int J)
{
    int N,K,P;
    indexes(J,N,K,P);
    return QString("%1").arg(J,3,'f',0,QLatin1Char( ' ' ))+
          QString("%1").arg(N,3,'f',0,QLatin1Char( ' ' ))+
          QString("%1").arg(K,3,'f',0,QLatin1Char( ' ' ))+
          QString("%1").arg(P,3,'f',0,QLatin1Char( ' ' ));
}


long double usf(int N, int K, int P, double L, double B)//unnormalized spherical function
{
    if(K==0)return PL(N,0,B);
    if(P==0) return PL(N,K,B)*sin(K*L); else return PL(N,K,B)*cos(K*L);
}

long double FR(int N,int K)//norm of spherical function
{
    long double n = (long double)N;
    long double fr = sqrt((2*n+1)/(4*M_PI));
    if(K>0) return fr*sqrt(2/fact2(N-K+1,N+K)); else return fr;
}

long double sf(int N, int K, int P, double L, double B)//normalized spherical function
{
    return FR(N,K)*usf(N,K,P,L,B);
}

long double sfj(int J, double L, double B)// normalized spherical function via index J
{
    int N,K,P;
    indexes(J,N,K,P);
    return sf(N,K,P,L,B);
}

///////////////////////////////////////////////////////////////////


long double TL(int N, int K, int P, double L, double B) //longtitudial component of toroidal function
{
    long double n = (long double)N;
    if(K==0) return FR(N,K)/sqrt(n*(n+1))*PL(N,1,B);
    if(P==0) return FR(N,K)/sqrt(n*(n+1))*(-K*tan(B)*PL(N,K,B)+PL(N,K+1,B))*sin(K*L);
    else return FR(N,K)/sqrt(n*(n+1))*(-K*tan(B)*PL(N,K,B)+PL(N,K+1,B))*cos(K*L);
}

long double TLJ(int J, double L, double B)
{
    int N,K,P;
    indexes(J,N,K,P);
    return TL(N,K,P,L,B);
}

long double TB(int N, int K, int P, double L, double B) //latitudial component of toroidal function
{
    long double n = (long double)N;
    if(K==0) return 0;
    if(P==0) return FR(N,K)/sqrt(n*(n+1))*(-K/cos(B)*PL(N,K,B)*cos(K*L));
    else return FR(N,K)/sqrt(n*(n+1))*(K/cos(B)*PL(N,K,B)*sin(K*L));
}

long double TBJ(int J, double L, double B)
{
    int N,K,P;
    indexes(J,N,K,P);
    return TB(N,K,P,L,B);
}

long double SL(int N, int K, int P, double L, double B) //longtitudial component of spheroidal function
{
    long double n = (long double)N;
    if(K==0) return 0;
    if(P==0) return FR(N,K)/sqrt(n*(n+1))*(K/cos(B)*PL(N,K,B)*cos(K*L));
    else return FR(N,K)/sqrt(n*(n+1))*(-K/cos(B)*PL(N,K,B)*sin(K*L));
}

long double SLJ(int J, double L, double B)
{
    int N,K,P;
    indexes(J,N,K,P);
    return SL(N,K,P,L,B);
}

long double SB(int N, int K, int P, double L, double B) //latitudial component of spheroidal function
{
    long double n = (long double)N;
    if(K==0) return FR(N,K)/sqrt(n*(n+1))*PL(N,1,B);
    if(P==0) return FR(N,K)/sqrt(n*(n+1))*(-K*tan(B)*PL(N,K,B)+PL(N,K+1,B))*sin(K*L);
    else return FR(N,K)/sqrt(n*(n+1))*(-K*tan(B)*PL(N,K,B)+PL(N,K+1,B))*cos(K*L);
}

long double SBJ(int J, double L, double B)
{
    int N,K,P;
    indexes(J,N,K,P);
    return SB(N,K,P,L,B);
}



int main(int argc, char *argv[])
{
    int N = QString(argv[1]).toInt();
    int M = QString(argv[2]).toInt();
    ///
    QTextStream stream(stdout);

    double deMin = grad2rad(-30);
    double deMax = grad2rad(30);

    double s1 = sin(deMin);
    double s2 = sin(deMax);
    ///
    double *phi = new double; // phi and theta are spherical coordinates of the center of appropriate HealPix area
    double *theta = new double;
    double L,B;
    double I[4];
    int kNum = 0;
    for(int k=0;k<4;k++)I[k]=0;
    for(int pn=0;pn<12288;pn++)
    {
        pix2ang_ring(32,pn,theta,phi);//get phi and theta from current index of the area
        L = phi[0];
        B = M_PI/2-theta[0];

        if(B<deMin||B>deMax) continue;

        B = asin((2.0*sin(B)/(s2-s1))-(s2+s1)/(s2-s1));

        kNum++;


        //I[0]+=sfj(N,L,B)*sfj(M,L,B)*4*M_PI/768;
        I[0]+=(TLJ(N,L,B)*TLJ(M,L,B)+TBJ(N,L,B)*TBJ(M,L,B))*4*M_PI;///12288;
        I[1]+=(SLJ(N,L,B)*SLJ(M,L,B)+SBJ(N,L,B)*SBJ(M,L,B))*4*M_PI;//12288;
        I[2]+=(TLJ(N,L,B)*SLJ(M,L,B)+TBJ(N,L,B)*SBJ(M,L,B))*4*M_PI;//12288;
        I[3]+=(SLJ(N,L,B)*TLJ(M,L,B)+SBJ(N,L,B)*TBJ(M,L,B))*4*M_PI;//12288;
        //stream << QString("%1").arg(sfj(N,L,B),10,'f',5,QLatin1Char( '0' )) <<endl;
    }

    qDebug() << QString("kNum: %1\n").arg(kNum);

    for(int k=0;k<4;k++)I[k]/=kNum;
    //
    //I[1]=I[0];I[2]=I[0];I[3]=I[0];
    //for(int k=0;k<4;k++) stream << QString("%1").arg(I[k],10,'f',5,QLatin1Char( '0' )) <<endl;
    stream <<"Tl_n x Tl_m +Tb_n x Tb_m = "<<QString("%1").arg(I[0],10,'f',6,QLatin1Char( '0' )) <<endl;
    stream <<"Sl_n x Sl_m +Sb_n x Sb_m = "<<QString("%1").arg(I[1],10,'f',6,QLatin1Char( '0' )) <<endl;
    stream <<"Tl_n x Sl_m +Tb_n x Sb_m = "<<QString("%1").arg(I[2],10,'f',6,QLatin1Char( '0' )) <<endl;
    stream <<"Sl_n x Tl_m +Sb_n x Tb_m = "<<QString("%1").arg(I[3],10,'f',6,QLatin1Char( '0' )) <<endl;
    stream <<"n => N,K,P " << indexesStr(N)<<endl<<"m => N,K,P " << indexesStr(M)<<endl;
    //
    delete phi;delete theta;
    return 0;

}
