//#include "./../libs/stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rada.h"
//#include "mymatrix.h"
#include "./../../libs/dele.h"
#include "./../../libs/calc_epm.h"
//#include "DynArr.h"
#include "./../../libs/moody/capsule/capsuleBase/particle/Particle.h"
#include <omp.h>


//  #define k2 6.672590000e-8
//  mymatrix *r;

  extern double a, CC, omega, Ltilde, A;
  extern int nofzbody;
  extern int SK, CENTER, centr_num;
  extern int useEPM;

//  extern myvector *mass; // [telo]
//  extern int error;
  //extern double col, vout;
  extern ever_params *eparam;
  //extern double *mass;
  int iterNum;

  //extern ParticleStruct* sun;
  extern QList <ParticleStruct*> pList;
  //extern QList <ParticleStruct*> jList;

 extern dele *nbody;
 // extern int nofzbody;

double dist(double X0[], double X1[])
{
  return(sqrt(pow(X1[0] - X0[0], 2) + pow(X1[1] - X0[1], 2) + pow(X1[2] - X0[2], 2)));
}
/*
double psR(ParticleStruct* par)
{
  return(sqrt(par->x*par->x + par->y*par->y + par->z*par->z));
}

double psDist(ParticleStruct* par0, ParticleStruct* par1)
{
    return(sqrt(pow(par1->x - par0->x, 2.0) + pow(par1->y - par0->y, 2.0) + pow(par1->z - par0->z, 2.0)));
}
  */

    void force_N(double X[], double V[], double F[]);
    int force_PPN(double X[], double V[], double F[]);
    void force_GN(double X[], double V[], double F[]);

    void Everhardt::force(double X[], double V[], double TS, double F[])
    {
            int i, j, N, komp, teloi, teloj;
            double Rij, Rik, Rjk;
            int iNum = nofzbody;
            int Ni = nofzbody*3;
            int jNum = iNum;//eparam->NV;
            int Nj = Ni;//eparam->NV*3;
            double Ri, Rj, *D, res0, res1;

    //        double beta = 1.0;
    //        double gamma = 1.0;
            iterNum = 0;

            force_GN(X, V, F);
          //force_N(X, V, F);
          //if(force_PPN(X, V, F)) printf("warn iterate overflow\n\n");
          //else printf("iterateNum %d\n\n", iterNum);


    }

    //GELIOCENTR
      void force_GN(double X[], double V[], double F[])
      {
          int i, komp, teloi, teloj;
          //double res1, res0;

          int iNum = pList.size();//nofzbody-1;
          int Ni = iNum*3;
          //int jNum = jList.size();//eparam->NV;
          //int Nj = jNum*3;//eparam->NV*3;

          //teloi=-1;

          //#pragma omp parallel for
          for(teloi=0; teloi<iNum; teloi++)
          {
              int i=teloi*3;
              double Ri = norm(&X[i]);// dist(teloi, 0, X);//sqrt(X[i+0]*X[i+0]+X[i+1]*X[i+1]+X[i+2]*X[i+2]);
              //Ri = psR(iList[teloi]);//norm(&X[i]);

              if(Ri>(eparam->vout))
              {
                  printf("WARN!!!! V OUT!!!!\n");
                  printf("Ri[%d]: %f > %f\n", teloi, Ri, eparam->vout);
                  exit(1);
              }

              double massI = 0.0;
              if(pList[teloi]->identity==Advisor::ordinary) massI=pList[teloi]->mass;
//                  #pragma omp parallel for
                  for(komp=0; komp<3; komp++)
                  {
                      double res0, res1;
                          res0 = res1 = 0.0;
 //                         #pragma omp parallel for reduction(+:res0)
                          for(teloj=0; teloj<iNum; teloj++)
                          {
                             int j=teloj*3;
                             if(teloi!=teloj&&pList[teloj]->identity==Advisor::ordinary)
                             {
                                double Rij = dist(&X[i], &X[j]);
                                //Rj = dist(teloj, 0, X);
                                double Rj = norm(&X[j]);


                                if(Rij<eparam->col)
                                {

                                    printf("teloi= %d\tteloj= %d\n", teloi, teloj);
                                    printf("Rij= %f\n", Rij);
                                    printf("WARN!!!! CRASH!!!!\n");
                                    exit(1);
                                }



                                res0 += pow(pList[teloj]->mass, -1.0)*((X[j+komp] - X[i+komp])/(pow(Rij,3)) - X[j+komp]/(pow(Rj, 3)));

                             }

                          }

                          res1 = -((1.0 + pow(massI, -1.0))*X[i+komp])/(pow(Ri, 3));


                          F[i+komp] = ka*ka*(res0+res1);

                      }
                  //printf("force: %e %e %e\n\n", F[i], F[i+1], F[i+2]);
          }
      }
/*
    void Everhardt::force(double X[], double V[], double TS, double F[])
    {
        int i, j , komp;
        int teloi, teloj;
        double Ri, Dij;
        double res0,  resi, resj;
        double muj, mu0;
        QString name;
        int plaNum, status;

        int iNum = iList.size();
        int Ni = iNum*3;

        int jNum = jList.size();
        int Nj = jNum*3;
        double Xj[jNum], Vj[jNum];

        /*double mass[10] = {
                    1.0, 6.023600000000e+06, 4.085237100000e+05, 328900.5614, 3.098708000000e+06, 1.047348600000e+03, 3.497898000000e+03, 2.290298000000e+04, 1.941224000026e+04, 1.352000000000e+08
        };
        int pla[10] = {
                SUN_NUM, MERCURY_NUM, VENUS_NUM, EARTH_NUM, MARS_NUM, JUPITER_NUM, SATURN_NUM, URANUS_NUM, NEPTUNE_NUM, PLUTO_NUM
        };/

        mu0 = 1.0;


        for(j=0, teloj=0; j<Nj; j+=3, teloj++)
        {
            name = QString(jList[teloj]->name.data());
            if(useEPM) plaNum = epm_planet_num(name);
            else plaNum = planet_num(name.toAscii().data());


            if(plaNum!=-1)
            {

                if(useEPM)
                {
                    status = calc_EPM(plaNum, centr_num, int(TS), TS - int(TS), &Xj[j], &Vj[j]);
                     if(!status)
                     {
                         qDebug() << QString("error EPM\n");
                         //return 1;
                     }
                }
                else
                {
                    nbody->detState(&Xj[j+0], &Xj[j+1], &Xj[j+2], &Vj[j+0], &Vj[j+1], &Vj[j+2], TS, plaNum, CENTER, SK);
                }
                //nbody->detR(&Xj[j], &Xj[j+1], &Xj[j+2], TS, pla[teloj], 0, CENTER_BARY, SK_ECLIPTIC);
                //nbody->detState(&Xj[j], &Xj[j+1], &Xj[j+2], &Vj[j], &Vj[j+1], &Vj[j+2], TS, pla[teloj+1], CENTER, SK);


            }
                //printf("plaNum= %d\tY:\t%f\t%f\t%f\n", pla[teloj], Y[j], Y[j+1], Y[j+2]);

        }

        for(i=0, teloi=0; i<Ni; i+=3, teloi++)
        {
            //mui = pow(jList.at(teloi)->mass, -1.0);
            Ri = sqrt(X[i+0]*X[i+0]+X[i+1]*X[i+1]+X[i+2]*X[i+2]);
            if(Ri>(eparam->vout))
            {
//		    cout << "WARN!!!! V OUT!!!!" << endl;
                printf("WARN!!!! V OUT!!!!\n");
                exit(1);
            }

            for(komp=0; komp<3; komp++)
            {
                resi = 0;

                for(j=0, teloj=0; teloj<iNum; j+=3, teloj++)
                {
                     if(teloi==teloj||iList.at(teloj)->interactionPermission==Advisor::interactNONE) continue;
                     Dij = sqrt(pow(X[j+0] - X[i+0], 2) + pow(X[j+1] - X[i+1], 2) + pow(X[j+2] - X[i+2], 2));

                     if(Dij<eparam->col)
                     {
    //				cout << "WARN!!!! CRASH!!!!" << endl;
                         printf("WARN!!!! CRASH!!!!\n");
                         exit(1);
                     }

                     muj = pow(iList.at(teloj)->mass, -1.0);

                     resi += muj*((X[j+komp] - X[i+komp])/(pow(Dij,3)));

                 }

                resj = 0.0;

                for(j=0, teloj=0; teloj<jNum; j+=3, teloj++)
                {
                     Dij = sqrt(pow(Xj[j+0] - X[i+0], 2) + pow(Xj[j+1] - X[i+1], 2) + pow(Xj[j+2] - X[i+2], 2));

                     if(Dij<eparam->col)
                     {
    //				cout << "WARN!!!! CRASH!!!!" << endl;
                         printf("WARN!!!! CRASH!!!!\n");
                         exit(1);
                     }

                     muj = pow(jList.at(teloj)->mass, -1.0);

                     resj += muj*((Xj[j+komp] - X[i+komp])/(pow(Dij,3)));

                 }

                res0 = -(mu0*X[i+komp])/(pow(Ri, 3));

                F[i+komp] = ka*ka*(res0+resi+resj);
            }
        }

    }




/*
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
/
  //        double beta = 1.0;
  //        double gamma = 1.0;
          iterNum = 0;

        force_N(X, V, F);
        //if(force_PPN(X, V, F)) printf("warn iterate overflow\n\n");
        //else printf("iterateNum %d\n\n", iterNum);


  }



//3 dec
  /*
  void force_N(double X[], double V[], double F[])
  {
      int i, j, N, komp, teloi, teloj;
      double Rij, Ri, Rj, res1, res0;

      int pNum, iNum, Ni;
      double mui, muj;

      iNum = nofzbody;
      Ni = (nofzbody)*3;
      //jNum = iNum;//eparam->NV;
      //int Nj = Ni;//eparam->NV*3;

  //    pNum = pList.size();
  /*    double *Xj, *Vj;
      Xj = new double[3];
      Vj = new double[3];
/


      //i=0;
      for(teloi=0; teloi<nofzbody; teloi++)
      {
          //if(pList[teloi]->interactionPermission==Advisor::interactNONE) continue;
              //Ri = dist(teloi, 0, X);//sqrt(X[i+0]*X[i+0]+X[i+1]*X[i+1]+X[i+2]*X[i+2]);

          i = teloi*3;
          Ri = norm(&X[i]);
            mui = pow(pList[teloi]->mass, -1.0);

              if(Ri>(eparam->vout))
              {
                  printf("WARN!!!! V OUT!!!!\n");
                  printf("Ri[%d]: %f > %f\n", teloi, Ri, eparam->vout);
                  exit(1);
              }

              for(komp=0; komp<3; komp++)
              {
                      res0 = 0.0;
                      for(teloj=0; teloj<nofzbody; teloj++)
                      {
                          j = teloj*3;
                         if(teloi!=teloj&&pList[teloj]->identity==Advisor::ordinary)
                         {
                             muj = ka*ka*pow(pList[teloj]->mass, -1.0);
                             /*
                             Xj[0] = pList.at(teloj)->x;
                             Xj[1] = pList.at(teloj)->y;
                             Xj[2] = pList.at(teloj)->z;

                             Vj[0] = pList.at(teloj)->xd;
                             Vj[1] = pList.at(teloj)->yd;
                             Vj[2] = pList.at(teloj)->zd;/

                             Rij = dist(&X[i], &X[j]);
                            //Rij = sqrt(pow(X[i] - pList.at(teloj)->x, 2.0) + pow(X[i+1] - pList.at(teloj)->y, 2.0) + pow(X[i+2] - pList.at(teloj)->z, 2.0));
                            //Rj = dist(teloj, 0, X);
                            Rj = norm(&X[j]);//sqrt(pList.at(teloj)->x*pList.at(teloj)->x + pList.at(teloj)->y*pList.at(teloj)->y + pList.at(teloj)->z*pList.at(teloj)->z);

                            if(Rij<eparam->col)
                            {

                                printf("teloi= %d\tteloj= %d\n", teloi, teloj);
                                printf("Rij= %f\n", Rij);
                                printf("WARN!!!! CRASH!!!!\n");
                                exit(1);
                            }

                            res0 += muj*((X[j+komp] - X[i+komp])/(pow(Rij,3)));
                            //res0 += pow(mass[teloj+1], -1.0)*((X[j+komp] - X[i+komp])/(pow(Rij,3)) - X[j+komp]/(pow(Rj, 3)));

                         }
                      }
                      //res1 = -((pow(mass[0], -1.0) + pow(mass[teloi], -1.0))*X[i+komp])/(pow(Ri, 3));
                      //res1 = 0;


                      F[i+komp] = res0;

                  }
            //i+=3;
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



      for(i=0, teloi=0; teloi<nofzbody; i+=3, teloi++)
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
                      for(teloj=0; teloj<nofzbody; teloj++)
                      {
                          j = teloj*3;
                             if(teloi!=teloj&&pList[teloj]->identity==Advisor::ordinary)
                             {
                                 Rij = dist(&X[i], &X[j]);
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

                                for(telok=0; telok<nofzbody; telok++)
                                {
                                    k = telok*3;
                                    muk = ka*ka*pow(pList[telok]->mass, -1.0);
                                       if(telok!=teloi&&pList[telok]->identity==Advisor::ordinary)
                                       {
                                          Rik = dist(&X[i], &X[k]);
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

                                       if(telok!=teloj&&pList[telok]->identity==Advisor::ordinary)
                                       {
                                          Rjk = dist(&X[j], &X[k]);
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
/

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
  //3 dec
  */
/*
//GELIOCENTR
  void force_GN(double X[], double V[], double F[])
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
                      res0 = res1 = 0.0;
                      for(j=0, teloj=0; j<Nj; j+=3, teloj++)
                      {
                          if(teloi!=teloj&&teloj!=0&&pList[teloj]->interactionPermission==Advisor::interactALL)
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



                                res0 += pow(pList[teloj]->mass, -1.0)*((X[j+komp] - X[i+komp])/(pow(Rij,3)) - X[j+komp]/(pow(Rj, 3)));

                             }
                      }
                      if(teloi!=0) res1 = -((pow(pList[0]->mass, -1.0) + pow(pList[teloi]->mass, -1.0))*X[i+komp])/(pow(Ri, 3));


                      F[i+komp] = ka*ka*(res0+res1);

                  }
              //printf("force: %e %e %e\n\n", F[i], F[i+1], F[i+2]);
      }
  }
  /*
 void force_GN(double X[], double V[], double TS, double F[])
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
/


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
