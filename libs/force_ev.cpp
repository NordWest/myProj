#include "./../libs/stdafx.h"

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
  extern ever_params *eparam;

  extern dele *nbody;
  extern int nofzbody;


// BARYCENTR
void Everhardt::force(double X[], double V[], double TS, double F[])
{
        int i, j, N, komp, teloi, teloj;
        //double k2 = 6.672590000e-8;
        //double ka = 0.017202098955;//f?
        //double k2 = 1.0325429631115171592476225138836e-4;
        //double k2 = 1;
        double fconst = 6.673e-8;
//	N = (mass->n-1)*3;
        int iNum = nofzbody;
        int Ni = nofzbody*3;
        int jNum = eparam->NV;
        int Nj = eparam->NV*3;
//        int Nj = jNum*3;
        double Ri, Rj, *D, res0, res1;
        //int k;

        //printf("TS: %12.2f\n", TS);

        //printf("X: %f %f %f\n", X[0], X[1])

        if(nbody==NULL) return;

        D = new double[iNum];
  //      Y = new double[Nj];

        double mass[11] = {
                    1.0, 5983000.0, 408522.0, 328900.1, 3098700.0, 1047.3908, 3499.2, 22930.0, 19260.0, 1812000.0, 0
        };

        /*int pla[11] = {
                SUN_NUM, MERCURY_NUM, VENUS_NUM, GEOCENTR_NUM, MARS_NUM, JUPITER_NUM, SATURN_NUM, URANUS_NUM, NEPTUNE_NUM, PLUTO_NUM, 0
        };*/

/*
        for(j=0, teloj=0; j<Nj; j+=3, teloj++)
        {
                nbody->detR(&Y[j], &Y[j+1], &Y[j+2], TS, pla[teloj], 0, CENTER_SUN, SK_ECLIPTIC);
                printf("plaNum= %d\tY:\t%f\t%f\t%f\n", pla[teloj], Y[j], Y[j+1], Y[j+2]);
        }
*/
  /*      for(i=0, teloi=0; j<Ni; i+=3, teloi++)
        {

                printf("plaNum= %d\tY:\t%f\t%f\t%f\n", pla[teloi], X[i], X[i+1], X[i+2]);
        }
*/
        //printf("eparam->col= %f\teparam->vout= %f\n", eparam->col, eparam->vout);

        for(i=0, teloi=0; i<Ni; i+=3, teloi++)
        {
                Ri = sqrt(X[i+0]*X[i+0]+X[i+1]*X[i+1]+X[i+2]*X[i+2]);
                //printf("R[%d]= %f\n", teloi, Ri);
                /*if(Ri<eparam->col)
                {
//		    cout << "WARN!!!! CRASH!!!!" << endl;
                    printf("WARN!!!! CRASH!!!!\n");
                    exit(1);
                }*/
                if(Ri>(eparam->vout))
                {
//		    cout << "WARN!!!! V OUT!!!!" << endl;
                    printf("WARN!!!! V OUT!!!!\n");
                    exit(1);
                }
                //k=0;
                for(j=0, teloj=0; teloj<iNum; j+=3, teloj++)
                {
                        if(teloi!=teloj)
                        {
                            D[teloj] = sqrt(pow(X[j+0] - X[i+0], 2) + pow(X[j+1] - X[i+1], 2) + pow(X[j+2] - X[i+2], 2));
                            //D[teloj] = sqrt(pow(X[j+0] - X[i+0], 2) + pow(X[j+1] - X[i+1], 2) + pow(X[j+2] - X[i+2], 2));
                            //printf("D[%d]= %f\n", teloj, D[teloj]);

//			    cout << "D[j]= " << D[j] << endl;
                            if(D[teloj]<eparam->col)
                            {
//				cout << "WARN!!!! CRASH!!!!" << endl;
                                printf("teloi= %d\tteloj= %d\n", teloi, teloj);
                                printf("D[%d]= %f\n", teloj, D[teloj]);
                                printf("WARN!!!! CRASH!!!!\n");
                                exit(1);
                            }
                            /*if(D[teloj]>eparam->vout)
                            {
//				cout << "WARN!!!! V OUT!!!!" << endl;
                                printf("WARN!!!! V OUT!!!!\n");
                                exit(1);
                            }*/

                        }
                            //k++;
                }



                for(komp=0; komp<3; komp++)
                {
                        res0 = res1 = 0.0;
                        for(j=0, teloj=0; j<Nj; j+=3, teloj++)
                        {
                               if(teloi!=teloj)
                               {
                                    //Rj = sqrt(Y[j]*Y[j] + Y[j+1]*Y[j+1] + Y[j+2]*Y[j+2]);
                                    Rj = sqrt(X[j]*X[j] + X[j+1]*X[j+1] + X[j+2]*X[j+2]);
                                    //printf("Rj= %f\n", Rj);
//				    cout << "Rj= " << Rj << endl;
                                    /*if(Rj<eparam->col)
                                    {
        //				cout << "WARN!!!! CRASH!!!!" << endl;
                                        printf("WARN!!!! CRASH!!!!\n");
                                        exit(1);
//					error = 1;
                                    }*/
                                    if(Rj>eparam->vout)
                                    {
        //				cout << "WARN!!!! V OUT!!!!" << endl;
                                        printf("WARN!!!! V OUT!!!!\n");
                                        exit(1);
                                    }

                                    //res0 += pow(mass[teloj+1], -1.0)*((X[j+komp] - X[i+komp])/(pow(D[teloj],3)) - X[j+komp]/(pow(Rj, 3)));
                                    //res0 += pow(mass[teloj], -1.0)*((Y[j+komp] - X[i+komp])/(pow(D[teloj],3)));
                                    if(mass[teloj]<1.0) continue;
                                    res0 += pow(mass[teloj], -1.0)*((X[j+komp] - X[i+komp])/(pow(D[teloj],3)));
                               }
                        }

                        //res1 = -((pow(mass[0], -1.0)+pow(mass[teloi+1], -1.0))*X[i+komp])/(pow(Ri, 3));
                        res1 = 0.0;

                        //printf("res0= %e\nres1= %e\n", res0, res1);

                        F[i+komp] = ka*ka*(res1 + res0);

                    }
                //printf("force: %e %e %e\n\n", F[i], F[i+1], F[i+2]);
        }

}
/*
 GELIOCENTR
void Everhardt::force(double X[], double V[], double TS, double F[])
{
        int i, j, N, komp, teloi, teloj;
        //double k2 = 6.672590000e-8;
        //double ka = 0.017202098955;//f?
        //double k2 = 1.0325429631115171592476225138836e-4;
        //double k2 = 1;
        double fconst = 6.673e-8;
//	N = (mass->n-1)*3;
        int jNum = eparam->NV/3;
        int Ni = eparam->NV;
//        int Nj = jNum*3;
        double Ri, Rj, *D, res0, res1;
        //int k;

        printf("TS: %12.2f\n", TS);

        //printf("X: %f %f %f\n", X[0], X[1])

        if(nbody==NULL) return;

        D = new double[jNum];
  //      Y = new double[Nj];

        double mass[11] = {
                    1.0, 5983000.0, 408522.0, 328900.1, 3098700.0, 1047.3908, 3499.2, 22930.0, 19260.0, 1812000.0, 0
        };

        int pla[11] = {
                SUN_NUM, MERCURY_NUM, VENUS_NUM, GEOCENTR_NUM, MARS_NUM, JUPITER_NUM, SATURN_NUM, URANUS_NUM, NEPTUNE_NUM, PLUTO_NUM, 0
        };
/*
        for(j=0, teloj=0; j<Nj; j+=3, teloj++)
        {
                nbody->detR(&Y[j], &Y[j+1], &Y[j+2], TS, pla[teloj], 0, CENTER_SUN, SK_ECLIPTIC);
                printf("plaNum= %d\tY:\t%f\t%f\t%f\n", pla[teloj], Y[j], Y[j+1], Y[j+2]);
        }
*/
  /*      for(i=0, teloi=0; j<Ni; i+=3, teloi++)
        {

                printf("plaNum= %d\tY:\t%f\t%f\t%f\n", pla[teloi], X[i], X[i+1], X[i+2]);
        }
/
        printf("eparam->col= %f\teparam->vout= %f\n", eparam->col, eparam->vout);

        for(i=0, teloi=0; i<Ni; i+=3, teloi++)
        {
                Ri = sqrt(X[i+0]*X[i+0]+X[i+1]*X[i+1]+X[i+2]*X[i+2]);
                printf("R[%d]= %f\n", teloi, Ri);
                if(Ri<eparam->col)
                {
//		    cout << "WARN!!!! CRASH!!!!" << endl;
                    printf("WARN!!!! CRASH!!!!\n");
                    exit(1);
                }
                if(Ri>(eparam->vout))
                {
//		    cout << "WARN!!!! V OUT!!!!" << endl;
                    printf("WARN!!!! V OUT!!!!\n");
                    exit(1);
                }
                //k=0;
                for(j=0, teloj=0; teloj<jNum; j+=3, teloj++)
                {
                        if(teloi!=teloj)
                        {
                            D[teloj] = sqrt(pow(X[j+0] - X[i+0], 2) + pow(X[j+1] - X[i+1], 2) + pow(X[j+2] - X[i+2], 2));
                            //D[teloj] = sqrt(pow(X[j+0] - X[i+0], 2) + pow(X[j+1] - X[i+1], 2) + pow(X[j+2] - X[i+2], 2));
                            printf("D[%d]= %f\n", teloj, D[teloj]);

//			    cout << "D[j]= " << D[j] << endl;
                            if(D[teloj]<eparam->col)
                            {
//				cout << "WARN!!!! CRASH!!!!" << endl;
                                printf("WARN!!!! CRASH!!!!\n");
                                exit(1);
                            }
                            if(D[teloj]>eparam->vout)
                            {
//				cout << "WARN!!!! V OUT!!!!" << endl;
                                printf("WARN!!!! V OUT!!!!\n");
                                exit(1);
                            }

                        }
                            //k++;
                }



                for(komp=0; komp<3; komp++)
                {
                        res0 = res1 = 0.0;
                        for(j=0, teloj=0; j<Ni; j+=3, teloj++)
                        {
                               if(teloi!=teloj)
                               {
                                    //Rj = sqrt(Y[j]*Y[j] + Y[j+1]*Y[j+1] + Y[j+2]*Y[j+2]);
                                    Rj = sqrt(X[j]*X[j] + X[j+1]*X[j+1] + X[j+2]*X[j+2]);
                                    printf("Rj= %f\n", Rj);
//				    cout << "Rj= " << Rj << endl;
                                    if(Rj<eparam->col)
                                    {
        //				cout << "WARN!!!! CRASH!!!!" << endl;
                                        printf("WARN!!!! CRASH!!!!\n");
                                        exit(1);
//					error = 1;
                                    }
                                    if(Rj>eparam->vout)
                                    {
        //				cout << "WARN!!!! V OUT!!!!" << endl;
                                        printf("WARN!!!! V OUT!!!!\n");
                                        exit(1);
                                    }

                                    res0 += pow(mass[teloj+1], -1.0)*((X[j+komp] - X[i+komp])/(pow(D[teloj],3)) - X[j+komp]/(pow(Rj, 3)));
                                    //res0 += pow(mass[teloj], -1.0)*((Y[j+komp] - X[i+komp])/(pow(D[teloj],3)));
                                    //res0 += pow(mass[teloj], -1.0)*((X[j+komp] - X[i+komp])/(pow(D[teloj],3)));
                               }
                        }

                        res1 = -((pow(mass[0], -1.0)+pow(mass[teloi+1], -1.0))*X[i+komp])/(pow(Ri, 3));
                        //res1 = 0.0;

                        printf("res0= %e\nres1= %e\n", res0, res1);

                        F[i+komp] = ka*ka*(res1 + res0);

                    }
                printf("force: %e %e %e\n\n", F[i], F[i+1], F[i+2]);
        }

}
*/

/*
  void Everhardt::force(double X[], double V[], double TS, double F[])
  {
          int i, j, N, komp, teloi, teloj;
          //double k2 = 6.672590000e-8;
          //double ka = 0.017202098955;//f?
          //double k2 = 1.0325429631115171592476225138836e-4;
          //double k2 = 1;
          double fconst = 6.673e-8;
  //	N = (mass->n-1)*3;
          printf("nofzbody: %i\n", nofzbody);

          int Ni = eparam->NV;
         int Nj = nofzbody*3;
         int jNum = eparam->NV/3;
          double Ri, Rj, *D, res0, res1;
          //int k;

          printf("TS: %12.2f\n", TS);

          //printf("X: %f %f %f\n", X[0], X[1])

          if(nbody==NULL) return;

          D = new double[nofzbody];
    //      Y = new double[Nj];

          double mass[11] = {
                      1.0, 5983000.0, 408522.0, 328900.1, 3098700.0, 1047.3908, 3499.2, 22930.0, 19260.0, 1812000.0, 0
          };

          int pla[11] = {
                  SUN_NUM, MERCURY_NUM, VENUS_NUM, GEOCENTR_NUM, MARS_NUM, JUPITER_NUM, SATURN_NUM, URANUS_NUM, NEPTUNE_NUM, PLUTO_NUM, 0
          };

          for(j=0, teloj=0; j<Nj; j+=3, teloj++)
          {
                  nbody->detR(&Y[j], &Y[j+1], &Y[j+2], TS, pla[teloj], 0, CENTER_BARY, SK_ECLIPTIC);
                  printf("plaNum= %d\tY:\t%f\t%f\t%f\n", pla[teloj], Y[j], Y[j+1], Y[j+2]);
          }

          for(i=0, teloi=0; i<Ni; i+=3, teloi++)
          {

                  printf("X:\t%f\t%f\t%f\t%f\n", X[i], X[i+1], X[i+2], sqrt(X[i]*X[i] + X[i+1]*X[i+1] + X[i+2]*X[i+2]));
          }

          printf("eparam->col= %f\teparam->vout= %f\n", eparam->col, eparam->vout);

          for(i=0, teloi=0; i<Ni; i+=3, teloi++)
          {
                  Ri = sqrt(X[i+0]*X[i+0]+X[i+1]*X[i+1]+X[i+2]*X[i+2]);
                  printf("R[%d]= %f\n", teloi, Ri);
                  if(Ri<eparam->col)
                  {
  //		    cout << "WARN!!!! CRASH!!!!" << endl;
                      printf("WARN!!!! CRASH!!!!\n");
                      exit(1);
                  }
                  if(Ri>(eparam->vout))
                  {
  //		    cout << "WARN!!!! V OUT!!!!" << endl;
                      printf("WARN!!!! V OUT!!!!\n");
                      exit(1);
                  }
                  //k=0;
                  for(j=0, teloj=0; teloj<nofzbody; j+=3, teloj++)
                  {
                         // if(teloi!=teloj)
                         // {
                              D[teloj] = sqrt(pow(Y[j+0] - X[i+0], 2) + pow(Y[j+1] - X[i+1], 2) + pow(Y[j+2] - X[i+2], 2));
                              //D[teloj] = sqrt(pow(X[j+0] - X[i+0], 2) + pow(X[j+1] - X[i+1], 2) + pow(X[j+2] - X[i+2], 2));
                              printf("D[%d]= %f\n", teloj, D[teloj]);

  //			    cout << "D[j]= " << D[j] << endl;
                              if(D[teloj]<eparam->col)
                              {
  //				cout << "WARN!!!! CRASH!!!!" << endl;
                                  printf("WARN!!!! CRASH!!!!\n");
                                  exit(1);
                              }
                              if(D[teloj]>eparam->vout)
                              {
  //				cout << "WARN!!!! V OUT!!!!" << endl;
                                  printf("WARN!!!! V OUT!!!!\n");
                                  exit(1);
                              }

                          //}
                              //k++;
                  }



                  for(komp=0; komp<3; komp++)
                  {
                          res0 = res1 = 0.0;
                          for(j=0, teloj=0; teloj<nofzbody; j+=3, teloj++)
                          {
                                 //if(teloi!=teloj)
                                 //{
                                      Rj = sqrt(Y[j]*Y[j] + Y[j+1]*Y[j+1] + Y[j+2]*Y[j+2]);
                                      //Rj = sqrt(X[j]*X[j] + X[j+1]*X[j+1] + X[j+2]*X[j+2]);
                                      printf("Rj= %f\n", Rj);
  //				    cout << "Rj= " << Rj << endl;
                                      if(Rj<eparam->col)
                                      {
          //				cout << "WARN!!!! CRASH!!!!" << endl;
                                          printf("WARN!!!! CRASH!!!!\n");
                                          exit(1);
  //					error = 1;
                                      }
                                      if(Rj>eparam->vout)
                                      {
          //				cout << "WARN!!!! V OUT!!!!" << endl;
                                          printf("WARN!!!! V OUT!!!!\n");
                                          exit(1);
                                      }

                                      //res0 += pow(mass[teloj+1], -1.0)*((X[j+komp] - X[i+komp])/(pow(D[teloj],3)) - X[j+komp]/(pow(Rj, 3)));
                                      //res0 += pow(mass[teloj], -1.0)*((Y[j+komp] - X[i+komp])/(pow(D[teloj],3)));
                                      res0 += pow(mass[teloj], -1.0)*((Y[j+komp] - X[i+komp])/(pow(D[teloj],3)));
                                // }
                          }

                          //res1 = -((pow(mass[0], -1.0)+pow(mass[teloi+1], -1.0))*X[i+komp])/(pow(Ri, 3));
                          res1 = 0.0;

                          printf("res0= %e\nres1= %e\n", res0, res1);

                          F[i+komp] = ka*ka*(res1 + res0);

                      }
                  printf("force: %e %e %e\n\n", F[i], F[i+1], F[i+2]);
          }

  }
*/
/*==============================================================*/
/* EOF 								*/
/*==============================================================*/
