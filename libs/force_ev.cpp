#include "./../libs/stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rada.h"
//#include "mymatrix.h"
#include "dele.h"
#include "DynArr.h"
#include "moody/capsule/capsuleBase/particle/Particle.h"


//  #define k2 6.672590000e-8
//  mymatrix *r;

  extern double a, CC, omega, Ltilde, A;
  extern int nofzbody;

//  extern myvector *mass; // [telo]
//  extern int error;
  extern double col, vout;
  extern ever_params *eparam;
  extern double *mass;
  int iterNum;

  extern QList <ParticleStruct*> pList;

 // extern dele *nbody;
 // extern int nofzbody;

  double dist(int i, int j, double X[])
  {
      return(sqrt(pow(X[j*3+0] - X[i*3+0], 2) + pow(X[j*3+1] - X[i*3+1], 2) + pow(X[j*3+2] - X[i*3+2], 2)));
  }

    void force_N(double X[], double V[], double F[]);
    int force_PPN(double X[], double V[], double F[]);
/*
  void force_N(double X[], double V[], double F[])
  {
      int i, j, N, komp, teloi, teloj;
      double Rij, Ri, Rj, res1, res0;

      int iNum = nofzbody-1;
      int Ni = (nofzbody-1)*3;
      int jNum = iNum;//eparam->NV;
      int Nj = Ni;//eparam->NV*3;


      for(i=0, teloi=0; teloi<iNum; i+=3, teloi++)
      {
              //Ri = dist(teloi, 0, X);//sqrt(X[i+0]*X[i+0]+X[i+1]*X[i+1]+X[i+2]*X[i+2]);
          Ri = norm(&X[i]);

              if(Ri>(eparam->vout))
              {
                  printf("WARN!!!! V OUT!!!!\n");
                  printf("Ri[%d]: %f > %f\n", teloi, Ri, eparam->vout);
                  exit(1);
              }

              for(komp=0; komp<3; komp++)
              {
                      res0 = 0.0;
                      for(j=0, teloj=0; j<Nj; j+=3, teloj++)
                      {
                             if(teloi!=teloj)
                             {
                                Rij = dist(teloi, teloj, X);
                                //Rj = dist(teloj, 0, X);
                                Rj = norm(&X[j]);

                                if(Rij<eparam->col)
                                {

                                    printf("teloi= %d\tteloj= %d\n", teloi, teloj);
                                    printf("Rij= %f\n", Rij);
                                    printf("WARN!!!! CRASH!!!!\n");
                                    exit(1);
                                }

                                res0 += pow(mass[teloj], -1.0)*((X[j+komp] - X[i+komp])/(pow(Rij,3)));
                                //res0 += pow(mass[teloj+1], -1.0)*((X[j+komp] - X[i+komp])/(pow(Rij,3)) - X[j+komp]/(pow(Rj, 3)));

                             }
                      }
                      //res1 = -((pow(mass[0], -1.0) + pow(mass[teloi], -1.0))*X[i+komp])/(pow(Ri, 3));
                      res1=0;


                      F[i+komp] = ka*ka*(res0+res1);

                  }
              //printf("force: %e %e %e\n\n", F[i], F[i+1], F[i+2]);
      }
  }

*/


  // BARYCENTR Particles
  void Everhardt::force(double X[], double V[], double TS, double F[])
  {
 /*         int i, j, N, komp, teloi, teloj;
          double Rij, Rik, Rjk;
          int iNum = nofzbody;
          int Ni = nofzbody*3;
          int jNum = iNum;//eparam->NV;
          int Nj = Ni;//eparam->NV*3;
          double Ri, Rj, *D, res0, res1;
*/
          double beta = 1.0;
          double gamma = 1.0;
          iterNum = 0;

        force_N(X, V, F);
        //if(force_PPN(X, V, F)) printf("warn iterate overflow\n\n");
        //else printf("iterateNum %d\n\n", iterNum);


  }

  void force_N(double X[], double V[], double F[])
  {
      int i, j, N, komp, teloi, teloj;
      double Rij, Ri, Rj, res1, res0;

      int iNum = nofzbody;
      int Ni = (nofzbody)*3;
      int jNum = iNum;//eparam->NV;
      int Nj = Ni;//eparam->NV*3;

      for(i=0, teloi=0; teloi<iNum; i+=3, teloi++)
      {
              //Ri = dist(teloi, 0, X);//sqrt(X[i+0]*X[i+0]+X[i+1]*X[i+1]+X[i+2]*X[i+2]);
          Ri = norm(&X[i]);

              if(Ri>(eparam->vout))
              {
                  printf("WARN!!!! V OUT!!!!\n");
                  printf("Ri[%d]: %f > %f\n", teloi, Ri, eparam->vout);
                  exit(1);
              }

              for(komp=0; komp<3; komp++)
              {
                      res0 = 0.0;
                      for(j=0, teloj=0; j<Nj; j+=3, teloj++)
                      {
                          if(teloi!=teloj&&pList[teloj]->interactionPermission==Advisor::interactALL)
                             {
                                Rij = dist(teloi, teloj, X);
                                //Rj = dist(teloj, 0, X);
                                Rj = norm(&X[j]);

                                if(Rij<eparam->col)
                                {

                                    printf("teloi= %d\tteloj= %d\n", teloi, teloj);
                                    printf("Rij= %f\n", Rij);
                                    printf("WARN!!!! CRASH!!!!\n");
                                    exit(1);
                                }

                                res0 += pow(pList[teloj]->mass, -1.0)*((X[j+komp] - X[i+komp])/(pow(Rij,3)));
                                //res0 += pow(mass[teloj+1], -1.0)*((X[j+komp] - X[i+komp])/(pow(Rij,3)) - X[j+komp]/(pow(Rj, 3)));

                             }
                      }
                      //res1 = -((pow(mass[0], -1.0) + pow(mass[teloi], -1.0))*X[i+komp])/(pow(Ri, 3));
                      res1 = 0;


                      F[i+komp] = ka*ka*(res0+res1);

                  }
              //printf("force: %e %e %e\n\n", F[i], F[i+1], F[i+2]);
      }
  }

  int force_PPN(double X[], double V[], double F[])
  {
      int i, j, k, N, komp, teloi, teloj, telok;
      double Rij, Rik, Rk, Rjk, Ri, Rj, res1, res0, res2, res3, res4, Vi, Vj;
      iterNum++;
      if(iterNum>100) return 0;

      int iNum = nofzbody;
      int Ni = (nofzbody)*3;
      int jNum = iNum;//eparam->NV;
      int Nj = Ni;//eparam->NV*3;
      double FN[Ni];
      double summ0, summ1;
      summ0 = summ1 = 0.0;
      for(i=0;i<Ni;i++)
      {
          FN[i] = F[i];
          summ0 += F[i];
      }
      summ0 /= Ni;
      double beta, gamma;
      double muj, muk;
      beta = gamma = 1.0;
      double R0[3], R1[3];
      double V0[3], V1[3];
      double r0, r1, r2, r3, r4, r5, r6, rr;



      for(i=0, teloi=0; teloi<iNum; i+=3, teloi++)
      {
              //Ri = dist(teloi, 0, X);//sqrt(X[i+0]*X[i+0]+X[i+1]*X[i+1]+X[i+2]*X[i+2]);
          Ri = norm(&X[i]);
          Vi = norm(&V[i]);

              if(Ri>(eparam->vout))
              {
                  printf("WARN!!!! V OUT!!!!\n");
                  printf("Ri[%d]: %f > %f\n", teloi, Ri, eparam->vout);
                  exit(1);
              }

              for(komp=0; komp<3; komp++)
              {
                      res0 = 0.0;
                      res3 = 0.0;
                      res4 = 0.0;
                      for(j=0, teloj=0; j<Nj; j+=3, teloj++)
                      {
                             if(teloi!=teloj&&pList[teloj]->interactionPermission==Advisor::interactALL)
                             {
                                Rij = dist(teloi, teloj, X);
                                //Rj = dist(teloj, 0, X);
                                Rj = norm(&X[j]);
                                Vj = norm(&V[j]);

                                if(Rij<eparam->col)
                                {

                                    printf("teloi= %d\tteloj= %d\n", teloi, teloj);
                                    printf("Rij= %f\n", Rij);
                                    printf("WARN!!!! CRASH!!!!\n");
                                    exit(1);
                                }

                                res1 = 0.0;
                                res2 = 0.0;

                                for(k=0, telok=0; k<Nj; k+=3, telok++)
                                {
                                    muk = ka*ka*pow(pList[telok]->mass, -1.0);
                                       if(telok!=teloi)//&&pList[telok]->interactionPermission==Advisor::interactALL)
                                       {
                                          Rik = dist(teloi, telok, X);
                                          //Rj = dist(teloj, 0, X);
                                          //Rk = norm(&X[k]);

                                          if(Rik<eparam->col)
                                          {

                                              printf("teloi= %d\ttelok= %d\n", teloi, telok);
                                              printf("Rij= %f\n", Rik);
                                              printf("WARN!!!! CRASH!!!!\n");
                                              exit(1);
                                          }

                                          res1 += muk/Rik;

                                       }

                                       if(telok!=teloj)//&&pList[telok]->interactionPermission==Advisor::interactALL)
                                       {
                                          Rjk = dist(teloj, telok, X);
                                          //Rj = dist(teloj, 0, X);
                                          //Rk = norm(&X[k]);

                                          if(Rjk<eparam->col)
                                          {

                                              printf("teloj= %d\ttelok= %d\n", teloj, telok);
                                              printf("Rij= %f\n", Rjk);
                                              printf("WARN!!!! CRASH!!!!\n");
                                              exit(1);
                                          }

                                          res2 += muk/Rjk;

                                       }
                                }

                                for(k=0;k<3;k++)
                                {
                                    R0[k] = X[i+k] - X[j+k];
                                    R1[k] = X[j+k] - X[i+k];
                                    V0[k] = (2+2*gamma)*V[i+k] - (1.0+2.0*gamma)*V[j+k];
                                    V1[k] = V[i+k] - V[j+k];
                                }

                                muj = ka*ka*pow(pList[teloj]->mass, -1.0);

                                r0 = -(2.0*(beta+gamma)*res1/(CAU*CAU));
                                r1 = -(2.0*beta-1)*res2/(CAU*CAU);
                                r2 = gamma*pow(Vi/CAU, 2.0);
                                r3 = (1+gamma)*pow(Vj/CAU, 2.0);
                                r4 = -(2.0*(1+gamma)/(CAU*CAU))*Smul3(&V[i], &V[j]);
                                r5 = -(3.0/(2.0*CAU*CAU))*pow(Smul3(&R0[0], &V[j])/Rij, 2.0);
                                r6 = (1.0/(2.0*CAU*CAU))*Smul3(&R1[0], &FN[j]);
                                qDebug() << QString("\nr0: %1\nr1: %2\nr2:%3\nr3:%4\nr4:%5\nr5:%6\nr6:%7\nsumm: %8\n\n").arg(r0).arg(r1).arg(r2).arg(r3).arg(r4).arg(r5).arg(r6).arg(r0 + r1 + r2 + r3 + r4 + r5 + r6);

                                rr = (1.0 + r0 + r1 + r2 + r3 + r4 + r5 + r6);

                                res0 += (muj*(X[j+komp] - X[i+komp])/pow(Rij,3))*rr;

/*
                                res0 += (muj*((X[j+komp] - X[i+komp])/(pow(Rij,3))))
                                        *(1.0
                                          -(2.0*(beta+gamma)*res1/(CAU*CAU))
                                          -(2.0*beta-1)*res2/(CAU*CAU)
                                          +gamma*pow(Vi/CAU, 2.0)
                                          +(1+gamma)*pow(Vj/CAU, 2.0)
                                          -(2.0*(1+gamma)/(CAU*CAU))*Smul3(&V[i], &V[j])
                                          -(3.0/(2.0*CAU*CAU))*pow(Smul3(R0, &V[j])/Rij, 2.0)
                                          +(1.0/(2.0*CAU*CAU))*Smul3(R1, &FN[j]));
*/

                                //res0 += pow(mass[teloj+1], -1.0)*((X[j+komp] - X[i+komp])/(pow(Rij,3)) - X[j+komp]/(pow(Rj, 3)));

                                res3 += ((muj/pow(Rij,3))*Smul3(R0, V0)*V1[komp]);
                                res4 += muj*FN[j+komp]/Rij;


                             }
                      }

                      res3 /= CAU*CAU;
                      res4 = res4*(3+4*gamma)/(2.0*CAU*CAU);

                      printf("res: %e %e %e\n\n", res0, res3, res4);

                      F[i+komp] = res0 + res3 + res4;

                      summ1 += fabs(F[i+komp]-FN[i+komp]);
                  }
              //printf("force: %e %e %e\n\n", F[i], F[i+1], F[i+2]);
              //printf("forceN: %e %e %e\n\n", FN[i], FN[i+1], FN[i+2]);
              //printf("d_force: %e %e %e\n\n", F[i]-FN[i], F[i+1]-FN[i+1], F[i+2]-FN[i+2]);
      }

      summ1/= Ni;
     //printf("summ1: %e\n\n", summ1);

      if(summ1<1e-15) return 0;
      else return(force_PPN(X, V, F));
  }

/*
  // BARYCENTR Particles [SAVE 16 avg]
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
          int jNum = iNum;//eparam->NV;
          int Nj = Ni;//eparam->NV*3;
  //        int Nj = jNum*3;
          double Ri, Rj, *D, res0, res1;

          D = new double[iNum];
    //      Y = new double[Nj];



          for(i=0, teloi=0; teloi<iNum; i+=3, teloi++)
          {
                  Ri = sqrt(X[i+0]*X[i+0]+X[i+1]*X[i+1]+X[i+2]*X[i+2]);
                  //printf("R[%d]= %f\n", teloi, Ri);
                  /*if(Ri<eparam->col)
                  {
  //		    cout << "WARN!!!! CRASH!!!!" << endl;
                      printf("WARN!!!! CRASH!!!!\n");
                      exit(1);
                  }/
                  if(Ri>(eparam->vout))
                  {
  //		    cout << "WARN!!!! V OUT!!!!" << endl;
                      printf("WARN!!!! V OUT!!!!\n");
                      printf("Ri[%d]: %f > %f\n", teloi, Ri, eparam->vout);
                      exit(1);
                  }
                  //k=0;
                  for(j=0, teloj=0; teloj<jNum; j+=3, teloj++)
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
                              }/

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
                                      }/
                                      if(Rj>eparam->vout)
                                      {
          //				cout << "WARN!!!! V OUT!!!!" << endl;
                                          printf("WARN!!!! V OUT!!!!\n");
                                          printf("Rj[%d]: %f > %f\n", teloj, Rj, eparam->vout);
                                          exit(1);
                                      }

                                      //res0 += pow(mass[teloj+1], -1.0)*((X[j+komp] - X[i+komp])/(pow(D[teloj],3)) - X[j+komp]/(pow(Rj, 3)));
                                      //res0 += pow(mass[teloj], -1.0)*((Y[j+komp] - X[i+komp])/(pow(D[teloj],3)));
                                      //if(mass[teloj]<1.0) continue;
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
/
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
                }/
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
                            }/

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
                                    }/
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
