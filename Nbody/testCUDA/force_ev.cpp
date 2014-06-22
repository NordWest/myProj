#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rada.h"
#include <dele.h>
#include <omp.h>
#include <cuda_runtime.h>

//#define OMPLIB

#define ka 0.017202098955
#define CAU 173.144632685

extern "C" __global__ void VecAdd_kernel(const float *A, const float *B, float *C, int N);

  //extern double a, CC, omega, Ltilde, A;
  //extern int nofzbody;
 // extern int SK, CENTER, centr_num;
  //extern int useEPM;

  int iterNum;

  extern ever_params *eparam;
  extern int nofzbody;
  extern double *mass;
  //extern dele eph;
  //extern QList <nbobjStruct*> pls;

extern "C" double dist3(double X0[], double X1[])
{
    return(sqrt(pow(X1[0] - X0[0], 2) + pow(X1[1] - X0[1], 2) + pow(X1[2] - X0[2], 2)));
}

extern "C" double norm3(double *v)
{
    double nm = 0.0;
    int len;
    len = 3;

    for(int i = 0; i<len; i++)
    {
            nm += v[i]*v[i];
    }

    return(sqrt(nm));
}

double Smul3(double *V1, double *V2)
{
    int i;
    double val;

    val = 0.0;
    for(i=0; i<3; i++)
    {
            val += V1[i]*V2[i];
    }
    return(val);
}

void force_N(double X[], double V[], double F[]);
int force_PPN(double X[], double V[], double F[]);
void force_GN(double X[], double V[], double F[]);

extern "C" void force_GN_CU(double X[], double V[], double F[]);
//void force_GN_dele(double X[], double V[], double TS, double F[]);

void Everhardt::force(double X[], double V[], double TS, double F[])
{
    iterNum = 0;

    //force_GN(X, V, F);
    force_GN_CU(X, V, F);
    //if(eparam->ppn)force_PPN(X, V, F);
    //force_GN_dele(X, V, TS, F);
}
/*
//GELIOCENTR
void force_GN(double X[], double V[], double F[])
{
  int iNum = nofzbody;
  int Ni = iNum*3;

  //printf("nofzbody: %d\n", nofzbody);

#pragma omp parallel for
  for(int teloi=0; teloi<iNum; teloi++)
  {
      int i=teloi*3;
      double Ri = norm3(&X[i]);

      if(Ri>(eparam->vout))
      {
          printf("WARN!!!! V OUT!!!!\n");
          printf("Ri[%d]: %f > %f\n", teloi, Ri, eparam->vout);
          exit(1);
      }

      double massI = mass[teloi];//0.0;
      //printf("massI: %e\n", massI);
      if(massI<0)massI=0;
      for(int komp=0; komp<3; komp++)
      {
          double res0, res1;
              res0 = res1 = 0.0;
//                         #pragma omp parallel for reduction(+:res0)
              for(int teloj=0; teloj<iNum; teloj++)
              {
                 int j=teloj*3;
                 double massJ = mass[teloj];
                 if(teloi!=teloj&&massJ>0)
                 {
                    double Rij = dist3(&X[i], &X[j]);
                    double Rj = norm3(&X[j]);

                    if(Rij<eparam->col)
                    {

                        printf("teloi= %d\tteloj= %d\n", teloi, teloj);
                        printf("Xi: %f\t%f\t%f\n", X[i], X[i+1], X[i+2]);
                        printf("Xj: %f\t%f\t%f\n", X[j], X[j+1], X[j+2]);
                        printf("Rij= %f\n", Rij);
                        printf("WARN!!!! CRASH!!!!\n");
                        exit(1);
                    }



                    res0 += massJ*((X[j+komp] - X[i+komp])/(pow(Rij,3)) - X[j+komp]/(pow(Rj, 3)));

                 }

              }

              res1 = -((1.0 + massI)*X[i+komp])/(pow(Ri, 3));


              F[i+komp] = ka*ka*(res0+res1);

          }
  }
}
/*
void force_GN_dele(double X[], double V[], double TS, double F[])
{
  int iNum = nofzbody;
  int Ni = iNum*3;
  int fNum = pls.size();
  int Nf = fNum*3;
  double *XF = new double[Nf];

  for(int i=0; i<fNum; i++)
  {
      eph.detR(&XF[i*3], &XF[i*3+1], &XF[i*3+2], TS, pls[i]->planet_num, 0, CENTER, SK);
  }

  #pragma omp parallel for
  for(int teloi=0; teloi<iNum; teloi++)
  {
      int i=teloi*3;
      double Ri = norm3(&X[i]);

      if(Ri>(eparam->vout))
      {
          printf("WARN!!!! V OUT!!!!\n");
          printf("Ri[%d]: %f > %f\n", teloi, Ri, eparam->vout);
          exit(1);
      }

      double massI = mass[teloi];//0.0;
      if(massI<0)massI=0;
      for(int komp=0; komp<3; komp++)
      {
          double res0, res1;
              res0 = res1 = 0.0;
//                         #pragma omp parallel for reduction(+:res0)
              for(int teloj=0; teloj<iNum; teloj++)
              {
                 int j=teloj*3;
                 if(teloi!=teloj&&mass[teloj]>0)
                 {
                    double Rij = dist3(&X[i], &X[j]);
                    double Rj = norm3(&X[j]);

                    if(Rij<eparam->col)
                    {

                        printf("teloi= %d\tteloj= %d\n", teloi, teloj);
                        printf("Rij= %f\n", Rij);
                        printf("WARN!!!! CRASH!!!!\n");
                        exit(1);
                    }



                    res0 += mass[teloj]*((X[j+komp] - X[i+komp])/(pow(Rij,3)) - X[j+komp]/(pow(Rj, 3)));

                 }

              }

              for(int telof=0; telof<fNum; telof++)
              {
                 int fn=telof*3;
                 if(pls[telof]->mass>0)
                 {
                    double Rij = dist3(&X[i], &XF[fn]);
                    double Rj = norm3(&XF[fn]);

                    if(Rij<eparam->col)
                    {

                        printf("teloi= %d\ttelof= %d\n", teloi, telof);
                        printf("Rij= %f\n", Rij);
                        printf("WARN!!!! CRASH!!!!\n");
                        exit(1);
                    }



                    res0 += pls[telof]->mass*((XF[fn+komp] - X[i+komp])/(pow(Rij,3)) - XF[fn+komp]/(pow(Rj, 3)));

                 }

              }

              res1 = -((1.0 + massI)*X[i+komp])/(pow(Ri, 3));


              F[i+komp] = ka*ka*(res0+res1);

          }
  }
}
*/

int force_PPN(double X[], double V[], double F[])
{
     int i, j, k, N, komp, telok;
     double Rij, Rik, Rk, Rjk, Ri, Rj, res1, res0, res2, res3, res4, Vi, Vj;
     iterNum++;
     if(iterNum>100) return 0;

     int iNum = nofzbody;
     int Ni = (nofzbody)*3;

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


#pragma omp parallel for
     for(int teloi=0; teloi<nofzbody; teloi++)
     {
         int i=teloi*3;
         double Ri = norm3(&X[i]);
         double Vi = norm3(&V[i]);

         double massI = mass[teloi];//0.0;
         if(massI<0)massI=0;

             if(Ri>(eparam->vout))
             {
                 printf("WARN!!!! V OUT!!!!\n");
                 printf("Ri[%d]: %f > %f\n", teloi, Ri, eparam->vout);
                 exit(1);
             }

             for(int komp=0; komp<3; komp++)
             {
                 double res0, res3, res4;
                     res0 = 0.0;
                     res3 = 0.0;
                     res4 = 0.0;
                     for(int teloj=0; teloj<nofzbody; teloj++)
                     {
                         double res1, res2;
                         j = teloj*3;
                            if(teloi!=teloj&&mass[teloj]>0)
                            {
                                Rij = dist3(&X[i], &X[j]);
                               //Rj = dist(teloj, 0, X);
                               Rj = norm3(&X[j]);
                               Vj = norm3(&V[j]);

                               if(Rij<eparam->col)
                               {

                                   printf("teloi= %d\tteloj= %d\n", teloi, teloj);
                                   printf("Rij= %f\n", Rij);
                                   printf("WARN!!!! CRASH!!!!\n");
                                   exit(1);
                               }

                               res1 = 0.0;
                               res2 = 0.0;

                               for(int telok=0; telok<nofzbody; telok++)
                               {
                                   k = telok*3;
                                   muk = ka*ka*pow(mass[telok], -1.0);
                                      if(telok!=teloi&&mass[telok]>0)
                                      {
                                         Rik = dist3(&X[i], &X[k]);
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

                                      if(telok!=teloj&&mass[telok]>0)
                                      {
                                         Rjk = dist3(&X[j], &X[k]);
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

                               muj = ka*ka*pow(mass[teloj], -1.0);

                               r0 = -(2.0*(beta+gamma)*res1/(CAU*CAU));
                               r1 = -(2.0*beta-1)*res2/(CAU*CAU);
                               r2 = gamma*pow(Vi/CAU, 2.0);
                               r3 = (1+gamma)*pow(Vj/CAU, 2.0);
                               r4 = -(2.0*(1+gamma)/(CAU*CAU))*Smul3(&V[i], &V[j]);
                               r5 = -(3.0/(2.0*CAU*CAU))*pow(Smul3(&R0[0], &V[j])/Rij, 2.0);
                               r6 = (1.0/(2.0*CAU*CAU))*Smul3(&R1[0], &FN[j]);
                               //qDebug() << QString("\nr0: %1\nr1: %2\nr2:%3\nr3:%4\nr4:%5\nr5:%6\nr6:%7\nsumm: %8\n\n").arg(r0).arg(r1).arg(r2).arg(r3).arg(r4).arg(r5).arg(r6).arg(r0 + r1 + r2 + r3 + r4 + r5 + r6);

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

                     //printf("res: %e %e %e\n\n", res0, res3, res4);

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

/*==============================================================*/
/* EOF 								*/
/*==============================================================*/
