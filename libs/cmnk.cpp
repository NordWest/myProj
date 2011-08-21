// cmnk.cpp: implementation of the cmnk class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"

#include "cmnk.h"
#include "math.h"

cmnk::cmnk()
{
	a=0;
	mkm=0;
	mnvar=0;
	xf=0;
	sgm=0;
	c=0;


}

cmnk::~cmnk()
{

	if (c!=0)
	{
		for(int i=0; i<mnvar+3;i++)
		{
			delete c[i];
		}
		delete c;
		c=0;
	}

	if (a!=0)
	{
		for(int i=0; i<mkm;i++)
		{
			delete a[i];
		}
		delete a;
		a=0;
	}

	if (xf!=0)
	{
		delete xf;
		xf=0;
	}
	
	if (sgm!=0)
	{
		delete sgm;
		sgm=0;
	}
}
void cmnk::setel(int km, int nvar,long double el)
{
	a[km][nvar]=el;

}
void cmnk::initmnk(int km, int nvar)
{
	if (c!=0)
	{
		for(int i=0; i<mnvar+3;i++)
		{
			delete c[i];
		}
		delete c;
		c=0;
	}

	if (a!=0)
	{
		for(int i=0; i<mkm;i++)
		{
			delete a[i];
		}
		delete a;
		a=0;
	}

	if (xf!=0)
	{
		delete xf;
		xf=0;
	}
	
	if (sgm!=0)
	{
		delete sgm;
		sgm=0;
	}
	a=new long double * [km];
	int i;
	for(i=0; i<km; i++)
	{
		a[i]=new long double [nvar+3];
	}

	c=new long double * [nvar+3];
	for(i=0; i<nvar+3; i++)
	{
		c[i]=new long double [nvar+3];
	}

	xf=new long double [nvar+3];
	sgm= new long double [nvar+3];
	mnvar=nvar;
	mkm=km;
}

long double* cmnk::ptlst(int km, int sn)
{
	
   long double sum;
   int i,j,k;

     for (k=0;k<sn; k++)
     {
           for(j=0 ; j<sn+1; j++)
           {
                sum=0;
                for(i=0;i<km; i++) sum=sum+a[i][k]*a[i][j];
                c[k][j]=sum;
		   }
     }
     slvls2(sn);
	 return xf;
}

void  cmnk::slvls2(int s)
{
	
   int i,k,m;
   //a:matrls;

      for(m=0;m<s-1; m++)
      {
            for(i=m+1;i<s;i++) 
            {
                 for (k=m+1; k<s+1; k++)
                 {
                       c[i][k]=c[i][k]-c[i][m]*c[m][k]/c[m][m];
                 }
            }
      }
      for (m=s-1;m>-1; m--)
      {
           xf[m]=c[m][s];
           for (k=m+1;k<s; k++){ xf[m]=xf[m]-xf[k]*c[m][k];}
           xf[m]=xf[m]/c[m][m];

      }


}
long double *  cmnk::sgmnk(int km,int sn)
{
	

	long double sum,sum2;
    int i,j;
     sum=0;
     for(i=0;i<km;i++)
     {
          sum2=0;
          for (j=0;j<sn; j++)
		  {
               sum2=sum2+a[i][j]*xf[j];
          };
          sum=sum+(a[i][sn]-sum2)*(a[i][sn]-sum2);
	 };
     //{d:=det(c,sn);}
     for (j=0;j<sn; j++)
     {
          sgm[j]=sqrt(sum/((km-sn)*c[j][j]));
      
     };
     sgm[sn]=sqrt(sum/((km-sn)));
	 return sgm;
}
