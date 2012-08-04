#include "./../libs/stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rada.h"
//#include "mymatrix.h"
#include "./../libs/dele.h"
#include "./../libs/DynArr.h"


//  #define k2 6.672590000e-8
//  mymatrix *r;

  extern double a, CC, omega, Ltilde, A;
  extern int nofzbody;

//  extern myvector *mass; // [telo]
//  extern int error;
  extern double col, vout;
  extern ever_params *eparam;

  extern dele *nbody;
  extern int nofzbody;


void Everhardt::force(double X[], double V[], double TS, double F[])
{
  	int i, j, k, N, komp, teloi, teloj;
	double k2 = 6.672590000e-8;
//	N = (mass->n-1)*3;
        int Ni = nofzbody*3;
        int Nj = 10*3;
  	double Ri, Rj, *D, res0, res1;

        printf("TS: %12.2f\n", TS);

	if(nbody==NULL) return;

	D = new double[Nj];
	Y = new double[Nj];

	double mass[11] = {
		1.0, 5983000.0, 408522.0, 3289000.1, 3098700.0, 1047.3908, 3499.2, 22930.0, 19260.0, 1812000.0, 0
	};

	int pla[11] = {
                SUN_NUM, MERCURY_NUM, VENUS_NUM, GEOCENTR_NUM, MARS_NUM, JUPITER_NUM, SATURN_NUM, URANUS_NUM, NEPTUNE_NUM, PLUTO_NUM, 0
	};

	for(j=0, teloj=0; j<Nj; j+=3, teloj++)
	{
                nbody->detR(&Y[j], &Y[j+1], &Y[j+2], TS, pla[teloj], 0, CENTER_BARY, SK_ECLIPTIC);
                printf("plaNum= %d\tY:\t%f\t%f\t%f\n", pla[teloj], Y[j], Y[j+1], Y[j+2]);
	}

	for(i=0, teloi=1; i<Ni; i+=3, teloi++)
	{
		Ri = sqrt(X[i+0]*X[i+0]+X[i+1]*X[i+1]+X[i+2]*X[i+2]);
                printf("Ri= %f\n", Ri);
                if(Ri<eparam->col)
		{
//		    cout << "WARN!!!! CRASH!!!!" << endl;
		    exit(1);
		}
                if(Ri>eparam->vout)
		{
//		    cout << "WARN!!!! V OUT!!!!" << endl;
		    exit(1);
		}

                for(j=0; j<Nj; j+=3)
		{
                        //if(j!=i)
                        //{
                            D[j] = sqrt(pow(Y[j+0] - X[i+0], 2) + pow(Y[j+1] - X[i+1], 2) + pow(Y[j+2] - X[i+2], 2));
                            printf("D[%d]= %f\n", j, D[j]);
//			    cout << "D[j]= " << D[j] << endl;
                            if(D[j]<eparam->col)
			    {
//				cout << "WARN!!!! CRASH!!!!" << endl;
				exit(1);
			    }
                            if(D[j]>eparam->vout)
			    {
//				cout << "WARN!!!! V OUT!!!!" << endl;
				exit(1);
			    }
                        //}
		}
		


		for(komp=0; komp<3; komp++)
		{
			res0 = res1 = 0.0;
                        for(j=0, teloj=1; j<Nj; j+=3, teloj++)
			{
                //                if(i!=j)
                 //               {
                                    Rj = sqrt(Y[j]*Y[j] + Y[j+1]*Y[j+1] + Y[j+2]*Y[j+2]);
                                    printf("Rj= %f\n", Rj);
//				    cout << "Rj= " << Rj << endl;
                                    if(Rj<eparam->col)
				    {
	//				cout << "WARN!!!! CRASH!!!!" << endl;
					exit(1);
//					error = 1;
				    }
                                    if(Rj>eparam->vout)
				    {
	//				cout << "WARN!!!! V OUT!!!!" << endl;
					exit(1);
				    }
                                    res0 += mass[teloj]*((Y[j+komp] - X[i+komp])/(pow(D[j],3)) - Y[j+komp]/(pow(Rj, 3)));
                //		}
			}
                        res1 = -((mass[0])*X[i+komp])/(pow(Ri, 3));
			F[i+komp] = k2*(res1 + res0);

                    }
                printf("force: %f %f %f", F[i], F[i+1], F[i+2]);
	}

}

/*==============================================================*/
/* EOF 								*/
/*==============================================================*/
