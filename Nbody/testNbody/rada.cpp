/*==============================================================*/
/*			RADA.CPP				*/
/*	(c) V.B.Titov		 		28.04.94	*/
/*==============================================================*/
//#include "stdafx.h"


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rada.h"

extern FILE *test;
extern double error;

Everhardt::Everhardt(int nv, int nclass, int nor, int ni, int ll, float xl)
{
 double HH[48] = {
      0.212340538239152E+00, 0.590533135559265E+00, 0.911412040487296E+00,
      0.0985350857988264E+00,0.3045357266463639E+00,0.5620251897526139E+00,
      0.8019865821263918E+00,0.9601901429485313E+00,0.0562625605369221E+00,
      0.1802406917368924E+00,0.3526247171131696E+00,0.5471536263305554E+00,
      0.7342101772154105E+00,0.8853209468390957E+00,0.9775206135612875E+00,
      0.0362578128832095E+00,0.1180789787899987E+00,0.2371769848149604E+00,
      0.3818827653047059E+00,0.5380295989189891E+00,0.6903324200723622E+00,
      0.8238833438370047E+00,0.9256126102908040E+00,0.9855875903511235E+00,
      0.0252736203975203E+00,0.0830416134474051E+00,0.1691751003771814E+00,
      0.2777967151090320E+00,0.4015027202328608E+00,0.5318623869104160E+00,
      0.6599918420853348E+00,0.7771593929561621E+00,0.8753807748555569E+00,
      0.9479645488728194E+00,0.9899817195383196E+00,0.0186103650109879E+00,
      0.0614755408992690E+00,0.1263051786933106E+00,0.2098429717265625E+00,
      0.3078989982803983E+00,0.4155560359786595E+00,0.5274156139958823E+00,
      0.6378686027177611E+00,0.7413764592942375E+00,0.8327489886084423E+00,
      0.9074047753009974E+00,0.9616018612603216E+00,0.9926353489739107E+00
		 };
 int mc[12] =	 {
		  1,13,24,34,43,51,58,64,69,73,76,78
		 };
 int nw[14]  =   {
		  0,0,1,3,6,10,15,21,28,36,45,55,66,78
		 };

 int NXI[78] =	 {
		    2,   3,   4,   5,   6,   7,   8,   9,  10,  11,
		   12,  13,   3,   6,  10,  15,  21,  28,  36,  45,
		   55,  66,  78,   4,  10,  20,  35,  56,  84, 120,
		  165, 220, 286,   5,  15,  35,  70, 126, 210, 330,
		  495, 715,   6,  21,  56, 126, 252, 462, 792,1287,
		    7,  28,  84, 210, 462, 924,1716,   8,  36, 120,
		  330, 792,1716,   9,  45, 165, 495,1287,  10,  55,
		  220, 715,  11,  66, 286,  12,  78,  13
		 };
 int kd2, la, lc;

 int j, l, k, m, n;
 int jd, jdm, lb, ld, le;

 double *mem;
 int off;

 NV = nv; NCLASS = nclass; NOR = nor; LL = ll; XL = xl; NI = ni;

 for(j=0; j<14; j++)
 {
  NW[j] = nw[j];
  H[j] = 0.0L;
 }
 for(j=0; j<13; j++) U[j] = W[j] = 0.0L;

 for(j=0; j<12; j++) MC[j] = mc[j];

 for(j=0; j<78; j++) XI[j] = C[j] = D[j] = R[j] = 0.0L;

 F1 = new double [43*NV];
 FJ = F1+NV; Y = FJ + NV; Z = Y + NV;
 B  = Z + NV;
 BE = B + 13*NV;
 BT = BE + 13*NV;

 KD  = (NOR - 3)/2;
 kd2 = KD/2;
 KE  = KD + 1;
 KF  = KD + 2;
 PW  = 1.0L/((double) (KD + 3));
 NCL = NPQ = NES = 0;
 if (NCLASS == 1) NCL = 1;
 if (NCLASS < 2)  NPQ = 1;

 if (NV == 1) SR = 1.2L; else SR = 1.5L;

 if (LL < 0)	  NES = 1;

 N_CLASS = abs(NCLASS);

 for(n=1, la=kd2*kd2-1; n<KF; n++, la++)
 {
  H[n]	 = HH[la];
  W[n-1] = 1.0L/((double) ((n+1)*(1 + (n+1)*(N_CLASS-1))));
  U[n-1] = n+2;
 }
 W1 = 1.0L/((double) N_CLASS);

 for(j=0; j<KD; j++)
 {
  m = MC[j] - 1;
  for(l=j+1; l<KE; l++, m++)
  {
   XI[m] = (double) (NXI[m]) * W[j]/W[l];
  }
 }

 C[0] = -H[1] * W[0];
 D[0] =  H[1] / W[1];
 R[0] =  1.0L /(H[2] - H[1]);

 for(k=2, la=0, lc=0; k<KE; k++)
 {
  lb = la; la = lc + 1; lc = NW[k+1] - 1;
  jd = lc - la - 1;
  C[la] = -H[k] * C[lb];
  C[lc] = (C[la-1]/W[jd] - H[k]) * W[jd+1];
  D[la] = H[1] * D[lb] * W[k-1]/W[k];
  D[lc] = (D[la-1]*W[k-1] + H[k]) / W[k];
  R[la] = 1.0L/(H[k+1] - H[1]);
  R[lc] = 1.0L/(H[k+1] - H[k]);

  if (k != 2)
  {
   for(l=3; l<=k; l++)
   {
    ld = la + l - 2;
    le = lb + l - 3;
    jdm= ld - la - 1;
    C[ld] = W[jdm+1] * C[le] / W[jdm] - H[k] * C[le+1];
    D[ld] = (D[le] + H[l-1] * D[le+1]) * W[k-1]/W[k];
    R[ld] = 1.0L/(H[k+1] - H[l-1]);
   }
  }
 }
}

Everhardt::~Everhardt(void)
{
 delete F1;
}

void Everhardt::rada27(double *X, double *V, double TI, double TF)
{
 int k, l;
 double *v_ptr, *b_ptr, *b_g_ptr, *bt_ptr, *bt_g_ptr;
 double *be_ptr, *be_g_ptr;

 NSF = NPER = 0;
 TDIF = TF - TI;
 DIR = TDIF/fabs(TDIF);
 SS = pow(10,-(float)LL);
 NL = NI + 30;

/* Set a reasonable estimate to T based on experience */

 if (NES)
   TP = XL = fabs(XL)*DIR;
 else
   TP = (((double) NOR) /11.0L) * pow(0.5L,0.4L* (double) LL) * DIR/2.0L;

if (TP/TDIF > 0.5L) TP = 0.5L*TDIF;

 for(k=0, v_ptr=V, bt_g_ptr=BT, b_g_ptr=B, be_g_ptr=BE; k<NV;
			k++, v_ptr++, bt_g_ptr++, b_g_ptr++, be_g_ptr++)
 {
  if (NCL) *v_ptr = 0.0L;
  for(l=0, bt_ptr=bt_g_ptr, b_ptr=b_g_ptr, be_ptr=be_g_ptr;
			l<KE; l++, b_ptr+=NV, bt_ptr+=NV, be_ptr+=NV)
  {
   *b_ptr = *bt_ptr = 0.0L;
  }
 }

 NF = 0;
 NCOUNT = 0;
 do
  {
   NS = 0;
   TM = TI;
   SM = 10000.0L;
   force(X, V, TM, F1); NF++;
  } while(this->rada_maker(X, V, TF));
}

int Everhardt::rada_maker(double *X, double *V, double TF)
{
 int j, k, l, m, n;
 double *be_g_ptr, *be_ptr, *be_i_ptr, *b_g_ptr, *b_ptr, *b_i_ptr,
	*bt_g_ptr, *bt_ptr;
 double *w_ptr, *u_ptr, *r_ptr, *c_ptr;
 double s, q, res, hsum, temp, val, bdouble;
 double t, tval, t2, eps = 1.0e-10L;

 int j2, la, jdm;

 while(1)
 {
  /* Loop to find the Beta-values from B-values */

  for (k=0, be_g_ptr=BE+NV*(KE-1), b_g_ptr=B+NV*(KE-1);
		k < NV;
				k++, be_g_ptr++, b_g_ptr++)
  {
   *be_g_ptr = *b_g_ptr/W[KE-1];
   for (j=0, be_ptr = BE+k, b_ptr=B+k, w_ptr=W;
		j < KD;
				j++, be_ptr+=NV, b_ptr+=NV, w_ptr++)
   {
    *be_ptr = *b_ptr/(*w_ptr);
    for (l=j+1, b_i_ptr=b_ptr+NV; l<KE; l++, b_i_ptr+=NV)
    {
     n = NW[l] + j;
     *be_ptr += *b_i_ptr * D[n];
    }
   }
  }

  t = TP; tval = fabs(t); t2 = pow(t, (double) N_CLASS);
  /* Iteration loop with NL=NI passes */

  for (m=0; m<NL; m++)
  {
   for (j=1, j2=1; j<KF; j++)
   {
    la = NW[j-1] - 1;
    jdm = j - 1;
    s = H[j];
    q = pow(s, (double) (N_CLASS - 1));
    if (NPQ)
    {
     for (k=0, b_g_ptr=B+(KE-1)*NV; k<NV; k++, b_g_ptr++)
     {
      res = *b_g_ptr;
      for (l=0, b_ptr=b_g_ptr-NV; l<KD; l++, b_ptr-=NV)
      {
       res = *b_ptr + s*res;
      }
      Y[k] = X[k] + q*(t*V[k] + t2*s*(F1[k]*W1 + s*res));
     }
    }
    else
    {
     for (k=0, b_g_ptr=B+(KE-1)*NV; k<NV; k++, b_g_ptr++)
     {
      res = *b_g_ptr;
      temp = res * U[KE-1]; 
      for (l=0, b_ptr=b_g_ptr-NV, u_ptr=U+KE-2; l<KD; l++, b_ptr-=NV, u_ptr--)
      {
       res = *b_ptr + s*res;
       temp = *b_ptr * (*u_ptr) + s*temp;
      }
      Y[k] = X[k] + q*(t*V[k] + t2*s*(F1[k]*W1 + s*res));
      Z[k] = V[k] + s*t*(F1[k] + s*temp);
     }
    }
    force(Y, Z, TM + s*t, FJ); NF++;
    if (j2)
    {
     j2 = 0;
     for (k=0, be_ptr=BE,b_ptr=B; k<NV; k++, be_ptr++, b_ptr++)
     {
      temp = *be_ptr;
      res = (FJ[k] - F1[k])/s;
      *be_ptr = res;
      *b_ptr += (res - temp) * W[0];
     }
    }
    else
    {
     for (k=0, be_ptr=BE+(j-1)*NV, b_ptr=B+(j-1)*NV; k<NV;
				k++, be_ptr++, b_ptr++)
     {
      temp = *be_ptr;
      res = (FJ[k] - F1[k])/s;
      for (l=0, r_ptr=R+la+1, be_i_ptr=BE+k; l<jdm; l++, r_ptr++, be_i_ptr+=NV)
      {
       res = (res - *be_i_ptr)*(*r_ptr);
      }
      *be_ptr = res;
      temp = res - temp;
      *b_ptr += temp * W[j-1];
      for (l=0, c_ptr=C+la+1, b_i_ptr=B+k; l<jdm; l++, c_ptr++, b_i_ptr+=NV)
      {
       *b_i_ptr += *c_ptr * temp;
      }
     }
    }
   }
   if (m < NI-1) continue;

   hsum = 0.0L;
   val = pow(tval, (double) (-KE));
   for (k=0, b_ptr=B+(KE-1)*NV; k<NV; k++, b_ptr++)
   {
    bdouble = *b_ptr;
    hsum += bdouble*bdouble;
   }
   hsum = val*sqrt(hsum);
   if (NSF) continue;
   if (fabs(hsum-SM) <= 0.01L * hsum) break;

   SM = hsum;
  }
  if (NSF == 0)
/* The proper starting value for t */
  {
   if (hsum != 0.0L) TP = pow(SS/hsum, PW) * DIR;

   if (NES)
    TP = XL;
   else
   {
    if (TP/t <= 1.0L)
    {
     TP *= 0.8L;
     NCOUNT++;
     if (NCOUNT > 10) return 0; else return 1;
    }
   }
   NSF = 1;
  }
  /*  Find position and velocity for classes 2 and 2s */
  for (k=0, b_g_ptr=B+(KE-1)*NV; k<NV; k++, b_g_ptr++)
  {
   res = *b_g_ptr;
   for (l=0, b_ptr=B+k; l<KD; l++, b_ptr+=NV)
   {
    res += *b_ptr;
   }
   X[k] += V[k]*t + t2*(F1[k]*W1 + res);
   if (NCL) continue;
   res = *b_g_ptr * U[KE-1];
   for (l=0, b_ptr=B+k; l<KD; l++, b_ptr+=NV)
   {
    res += *b_ptr * U[l];
   }
   V[k] += t*(F1[k] + res);
  }

  TM += t;
  NS++;
  if (NPER) return 0;

/* Control on size of next sequence; return when TF reached */
  force( X, V, TM, F1); NF++;
  if (NES)
   TP = XL;
  else
  {
   if (hsum != 0.0L) TP = pow(SS/hsum, PW) * DIR;

   if (TP/t > SR) TP = SR*t;
  }
  if (DIR*(TM+TP) >= DIR*TF - eps)
  {
   TP = TF - TM;
   NPER = 1;
  }
  q = TP/t;
  for (k=0, b_g_ptr=B, bt_g_ptr=BT; k<NV; k++, b_g_ptr++, bt_g_ptr++)
  {
   res = 1.0L;
   for (j=0, b_ptr=b_g_ptr, bt_ptr=bt_g_ptr; j<KE;
					j++, b_ptr+=NV, bt_ptr+=NV)
   {
    if (NS>1) *bt_ptr = *b_ptr - *bt_ptr;
    if (j<KE-1)
    {
     m = MC[j] - 1;
     for (l=j+1, b_i_ptr=b_ptr+NV; l<KE; l++, b_i_ptr +=NV, m++)
     {
      *b_ptr += XI[m] * (*b_i_ptr);
     }
    }
    res *= q;
    temp = res * (*b_ptr);
    *b_ptr = temp + (*bt_ptr);
    *bt_ptr = temp;
   }
  }
  NL = NI;
 }
}

/*==============================================================*/
/* EOF 								*/
/*==============================================================*/
