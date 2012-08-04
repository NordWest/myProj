#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rada.h"
//#include "mymatrix.h"
#include "dele.h"
#include "DynArr.h"


//  #define k2 6.672590000e-8
//  mymatrix *r;

  extern double a, CC, omega, Ltilde, A;
  extern int nofzbody;

//  extern myvector *mass; // [telo]
//  extern int error;
  extern double col, vout;

  extern dele *nbody;


void Everhardt::force(double X[], double V[], double TS, double F[])
{
  	int i, j, k, N, komp, teloi, teloj;
	double k2 = 6.672590000e-8;
//	N = (mass->n-1)*3;
	int Ni = nofzbody;
	int Nj = 9*3;
  	double Ri, Rj, *D, res0, res1;

	if(nbody==NULL) return;

	D = new double[Nj];
	Y = new double[Nj];

	double mass[11] = {
		1.0, 5983000.0, 408522.0, 3289000.1, 3098700.0, 1047.3908, 3499.2, 22930.0, 19260.0, 1812000.0, 0
	};

	int pla[11] = {
		10, 0, 1, 13, 3, 4, 5, 6, 7, 8, 0
	};

	for(j=0, teloj=0; j<Nj; j+=3, teloj++)
	{
		nbody->detR(&Y[j], &Y[j+1], &Y[j+2], TS, pla[teloj], 0, CENTER_BARY, SK_ECLIPTIC);
	}

	for(i=0, teloi=1; i<Ni; i+=3, teloi++)
	{
		Ri = sqrt(X[i+0]*X[i+0]+X[i+1]*X[i+1]+X[i+2]*X[i+2]);
//		cout << "Ri= " << Ri << endl;
		if(Ri<col) 
		{
//		    cout << "WARN!!!! CRASH!!!!" << endl;
		    exit(1);
		}
		if(Ri>vout)
		{
//		    cout << "WARN!!!! V OUT!!!!" << endl;
		    exit(1);
		}

		for(j=0; j<N; j+=3)
		{
			if(j!=i)
			{
			    D[j] = sqrt(pow(X[j+0] - X[i+0], 2) + pow(X[j+1] - X[i+1], 2) + pow(X[j+2] - X[i+2], 2));
//			    cout << "D[j]= " << D[j] << endl;
			    if(D[j]<col)
			    {
//				cout << "WARN!!!! CRASH!!!!" << endl;
				exit(1);
			    }
			    if(D[j]>vout)
			    {
//				cout << "WARN!!!! V OUT!!!!" << endl;
				exit(1);
			    }
			}
		}
		


		for(komp=0; komp<3; komp++)
		{
			res0 = res1 = 0.0;
			for(j=0, teloj=1; j<N; j+=3, teloj++)
			{
//				if(i!=j)
//				{
				    Rj = sqrt(X[j]*X[j] + X[j+1]*X[j+1] + X[j+2]*X[j+2]);
//				    cout << "Rj= " << Rj << endl;
				    if(Rj<col)
				    {
	//				cout << "WARN!!!! CRASH!!!!" << endl;
					exit(1);
//					error = 1;
				    }
				    if(Rj>vout)
				    {
	//				cout << "WARN!!!! V OUT!!!!" << endl;
					exit(1);
				    }
				    res0 += mass[teloj]*((X[j+komp] - X[i+komp])/(pow(D[j],3)) - X[j+komp]/(pow(Rj, 3)));
				}
			}
			res1 = -((mass[0] + mass[teloi])*X[i+komp])/(pow(Ri, 3));
			F[i+komp] = k2*(res1 + res0);
		
	}
}

/*==============================================================*/
/* EOF 								*/
/*==============================================================*/
