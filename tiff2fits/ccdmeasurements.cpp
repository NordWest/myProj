#include <QtCore>
#include <QtGui>
#include "ccdmeasurements.h"
#include "math.h"
#include ".\..\libs\astro.h"

double PointData(double *ccdM, quint16 Xc, quint16 Yc, quint16 fWidth)
{
 return ccdM[Xc + Yc*fWidth];
};

double MEFP(matrix<double> M, quint16 Xc, quint16 Yc)
{
  return M(Xc,Yc);
};

CCDDATA ApertureData(double *ccdM, quint16 Xc, quint16 Yc, quint16 ro, quint16 fWidth, quint16 fHeight)
{
  int i,j;//counters
  double r;//current distance from Xc,Yc
  qint64 ImgBuffSize = ro*ro; //size of aperture buffer
  PPIX PA = new PIX[ImgBuffSize*sizeof(PIX)];//PPIX buffer
  quint16 n = 0;
  //quint32 k;
  double minI = -1;
  double maxI = -1;
  double meanI = 0;
  quint16 X_max = 0;
  quint16 Y_max = 0;
  //
  for (i=Xc-ro;i<Xc+ro;i++)
   {
    for (j=Yc-ro;j<Yc+ro;j++)
       {
	    if ((i>=0)&&(i<fWidth)&&(j>=0)&&(j<fHeight))
		 {
	        r = sqrt(pow(i-Xc,2)+pow(j-Yc,2));
            if (r<=ro)
		    {
			   PA[n].X = i;
			   PA[n].Y = j;
			   PA[n].I = ccdM[i + j*fWidth];
			   ///////////
			   if (minI == -1) minI = PA[n].I;
			   if (maxI == -1) maxI = PA[n].I;
			   if (PA[n].I>maxI) 
			     {
				   maxI = PA[n].I;
				   X_max = i;
                   Y_max = j;
                 }				   
			   if (PA[n].I<minI) minI = PA[n].I;
			   meanI = meanI + PA[n].I;
			   ///////////
			   n++;
            }
         }			
       }//end for j	   
   }//end for i
   meanI = meanI/n;
  ///////////////aperture buffer is filled
   CCDDATA RES;
   RES.Buffer = new PIX[n*sizeof(PIX)];
   double seI = 0;
   for (i=0;i<n;i++)
    {
	  RES.Buffer[i] = PA[i];
	  seI = seI+pow((PA[i].I - meanI),2);
	}
	seI = sqrt(seI/(n-1));
	RES.Size = n;
	RES.boxW = 2*ro;
	RES.boxH = 2*ro;
	RES.X = Xc;
	RES.Y = Yc;
	RES.maxADC = maxI;
	RES.minADC = minI;
	RES.meanADC = meanI;
	RES.seADC = seI;
	RES.Xmax = X_max;
	RES.Ymax = Y_max;
	delete[] PA;
	return RES;
};
////////////////////////////////////////////////////////////////////////
CCDDATA BoxData(double *ccdM, quint16 Xc, quint16 Yc, quint16 boxWidth, quint16 boxHeight, quint16 fWidth, quint16 fHeight)
{
  int i,j;//counters
  qint64 ImgBuffSize = boxWidth*boxHeight; //size of box buffer
  PPIX PA = new PIX[ImgBuffSize*sizeof(PIX)];//PPIX buffer
  quint16 n = 0;
  quint16 boxW = (quint16) boxWidth/2;
  quint16 boxH = (quint16) boxHeight/2;
  double minI = -1;
  double maxI = -1;
  double meanI = 0;
  quint16 X_max = 0;
  quint16 Y_max = 0;
  //
  for (i=Xc-boxW;i<Xc+boxW;i++)
   {
    for (j=Yc-boxH;j<Yc+boxH;j++)
       {
	    if ((i>=0)&&(i<fWidth)&&(j>=0)&&(j<fHeight))
		 {
	        PA[n].X = i;
			PA[n].Y = j;
			PA[n].I = ccdM[i + j*fWidth];
			///////////
			if (minI == -1) minI = PA[n].I;
			if (maxI == -1) maxI = PA[n].I;
			if (PA[n].I>maxI)
                {
				   maxI = PA[n].I;
				   X_max = i;
                   Y_max = j;
                }
			if (PA[n].I<minI) minI = PA[n].I;
			meanI = meanI + PA[n].I;
			   ///////////
			n++; 
         }			
       }//end for j	   
   }//end for i
   meanI = meanI/n;
  ///////////////box buffer is filled
   PPIX RESULTARR = new PIX[n*sizeof(PIX)];
   double seI = 0;
   for (i=0;i<n;i++)
    {
	RESULTARR[i] = PA[i];
	seI = seI+pow((PA[i].I - meanI),2);
	}
	seI = sqrt(seI/(n-1));
	CCDDATA RES;
	RES.Buffer = RESULTARR;
	RES.Size = n;
	RES.boxW = boxWidth;
	RES.boxH = boxHeight;
	RES.X = Xc;
	RES.Y = Yc;
	RES.maxADC = maxI;
	RES.minADC = minI;
	RES.meanADC = meanI;
	RES.seADC = seI;
	RES.Xmax = X_max;
	RES.Ymax = Y_max;
	delete[] PA;
   return RES;
};
////////////////////////////////////////////////////////////////////////

CCDDATA ProfileData(double *ccdM, quint16 Xc, quint16 Yc, quint16 profileWidth, quint16 fWidth)
{
  int i;//counters
  qint64 ImgBuffSize = profileWidth; //size of box buffer
  PPIX PA = new PIX[ImgBuffSize*sizeof(PIX)];//PPIX buffer
  quint16 n = 0;
  quint16 boxW = (quint16) profileWidth/2;
  double minI = -1;
  double maxI = -1;
  double meanI = 0;
  quint16 X_max = 0;
  quint16 Y_max = Yc;
  //
  for (i=Xc-boxW;i<Xc+boxW;i++)
   {
	    if ((i>=0)&&(i<fWidth))
		 {
	        PA[n].X = i;
			PA[n].Y = Yc;
			PA[n].I = ccdM[i + Yc*fWidth];
			///////////
			if (minI == -1) minI = PA[n].I;
			if (maxI == -1) maxI = PA[n].I;
			if (PA[n].I>maxI) 
                {
				   maxI = PA[n].I;
				   X_max = i;
                }				
			if (PA[n].I<minI) minI = PA[n].I;
			meanI = meanI + PA[n].I;
			   ///////////
			n++; 
         }				   
   }//end for i
   meanI = meanI/n;
  ///////////////box buffer is filled
   PPIX RESULTARR = new PIX[n*sizeof(PIX)];
   double seI = 0;
   for (i=0;i<n;i++)
    {
	RESULTARR[i] = PA[i];
	seI = seI+pow((PA[i].I - meanI),2);
	}
	seI = sqrt(seI/(n-1));
	CCDDATA RES;
	RES.Buffer = RESULTARR;
	RES.Size = n;
	RES.boxW = profileWidth;
	RES.boxH = 1;
	RES.X = Xc;
	RES.Y = Yc;
	RES.maxADC = maxI;
	RES.minADC = minI;
	RES.meanADC = meanI;
	RES.seADC = seI;
	RES.Xmax = X_max;
	RES.Ymax = Y_max;
	delete PA;
   return RES;
};

///////////////////////////////////////////////////////////////////////

STIMG StarCentering(double *ccdM, quint16 Xc, quint16 Yc, quint16 ro, double cm_max_shift, quint16 fWidth, quint16 fHeight)
{
 CCDDATA aperture = ApertureData(ccdM,Xc,Yc,2*ro,fWidth, fHeight);
 STIMG cofm;
 cofm.X = aperture.Xmax;
 cofm.Y = aperture.Ymax;
 delete aperture.Buffer;
 return cofm;
};

STIMG getStarCenter(double *ccdM, quint16 Xc, quint16 Yc, quint16 ro, double cm_max_shift, quint16 fWidth, quint16 fHeight)
{
 CCDDATA aperture = ApertureData(ccdM,Xc,Yc,2*ro,fWidth, fHeight);
 CCDDATA curr_app;
 int i,x,y;
 bool isObj = false;
 /////////////////////////
 QVector<int> v = sortAperture(aperture,5);
 for (i=0; i<v.count(); i++)
 {
   x = aperture.Buffer[v[i]].X;
   y = aperture.Buffer[v[i]].Y;
   curr_app = ProfileData(ccdM, x, y, 20, fWidth);
   isObj = isObject(curr_app, 0);
   delete curr_app.Buffer;
   if (isObj) break;
 }
 /////////////////////////
 STIMG cofm;
 cofm.X = x;
 cofm.Y = y;
 delete aperture.Buffer;
 return cofm;
};

QVector<int> sortAperture(CCDDATA data, int N)
{
  QVector<int> max_numb;
  int max_n;
  int i,j;
  for(i=0; i<N; i++)
  {
    max_n = 0;
	for(j=0; j<data.Size; j++)
	{
	   if ((data.Buffer[j].I > data.Buffer[max_n].I)&&(!isExist(max_numb,j)))
	    {
		 max_n = j;
		}
	}
	max_numb << max_n;
  }
  return max_numb;
};

bool isExist(QVector<int> int_v, int k)
{
bool out = false;
for(int i=0; i<int_v.count(); i++)
  {
   if (k = int_v[i]) 
   {
    out = true;
    break;
   }
  }
return out;
};

bool isObject(CCDDATA data, quint16 frpix)
{
  //QString str;
  bool res = false;
  double k = data.seADC/(data.maxADC - data.minADC);
  //str.setNum(k);
  //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
  if (k>0.27) res = true;
  return res;
};

vector<double> StartConditionsL(CCDDATA data, double delta)
{
  vector<double> P(8);
  STIMG CM = CenterOfMas(data);//"center of mass"
  P(0) = data.minADC;//"minimum of I - 1"
  P(1) = data.maxADC - P(0);//"maximum of I"
  P(2) = StImgWide(data,CM,delta);//Image width
  P(3) = P(2);
  P(4) = 0;
  P(5) = CM.X;
  P(6) = CM.Y;
  P(7) = delta;
  return P;
};

vector<double> StartConditionsG(CCDDATA data)
{
 vector<double> P(7);
 STIMG CM = CenterOfMas(data);//"center of mass"
 P(0) = data.minADC-2;
 P(1) = data.maxADC - P(0);
 //int n = 0;
 double r;
 double A = 0;
 for (int i=0;i<data.Size;i++)
 {
   r = pow(data.Buffer[i].X-CM.X,2)+pow(data.Buffer[i].Y-CM.Y,2);
   A = A + log(P(1)/(data.Buffer[i].I-P(0)))/r;
 }
 P(2) = A/data.Size;
 P(3) = P(2);
 P(4) = 0;
 P(5) = CM.X;
 P(6) = CM.Y;
  return P;
};

vector<double> StartConditionsM(CCDDATA data, double delta)
{
  vector<double> P(8);
  double r;
  STIMG CM = CenterOfMas(data);//"center of mass"
  P(0) = data.minADC;//"minimum of I - 1"
  P(1) = data.maxADC - P(0);//"maximum of I"
  P(2) = 0;
  int n = 0;
  for (int i=0;i<data.Size;i++)
  {
    r = pow(data.Buffer[i].X-CM.X,2)+pow(data.Buffer[i].Y-CM.Y,2);
	if ((data.Buffer[i].I-P(0))!=0)
	{
     P(2) = P(2)+pow(P(1)/(data.Buffer[i].I-P(0)),1/delta)/r;
	 n++;
	} 
  }
  P(2) = P(2)/n;
  P(3) = P(2);
  P(4) = 0;
  P(5) = CM.X;
  P(6) = CM.Y;
  P(7) = delta;
  return P;
};


QVector<double> NLSLevenbergMarquardt(CCDDATA data, vector<double> P, int psf_type, int *act_var, int N, int NN, int GG, double PSt, double UWESt)
/*
data is an array of PIX (row pixel data)
P is a vector of initial conditions
psf_type is a type of PSF
act_var is an array, 1 denotes included parameter and 0 denotes excluded parameter
N is number of actual (effective) parameters
*/
{
  QString str;
  //str.setNum(PSt);
  //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
  //str.setNum(UWESt);
  //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
  /////////////
  int i,j,n,it;
  bool singular;
  n = data.Size;
  int q = P.size(); 
  vector<double> dP(N);//vector of needed corrections
  vector<double> deltaP(q);//full vector of corrections
  vector<double> nextP(q);// test vector of corrections
  matrix<double> C (n, N);//plan matrix
  vector<double> R (n);//residuals vector
  vector<double> trR (n);//test residuals vector
  matrix<double> W(N,N);//normal equations matrix
  matrix<double> W_1(N,N);//inverse matrix
  identity_matrix<double> IR (N);//identity matrix to improve conditions
  vector<double> h(N);//normal equations vector
  vector<double> D(N);//vector of the diagonal elements of W
  ///////////////////////////////////
  //matrix<double> iter(1,q);//intermediate corrections of each step
  //vector<double> uwerr(1);//intermadiate unit errors
  /////////////////////////////////////
  double UWE,oldUWE;//unit weight error
  double k; // regularization factor
  int g = 0;
  int m;
  ////////////////////////
  for (i=0;i<n;i++)//residuals calculation, R vector forming
		{
		   switch(psf_type)
				{
				 case 0://Lorentzian
					   R(i) = data.Buffer[i].I - Lorentzian_(data.Buffer[i].X, data.Buffer[i].Y,P);
					   break;
				 case 1://Gaussian
					   R(i) = data.Buffer[i].I - Gaussian_(data.Buffer[i].X, data.Buffer[i].Y,P);
					   break;
				 case 2://Moffat
					   R(i) = data.Buffer[i].I - Moffat_(data.Buffer[i].X, data.Buffer[i].Y,P);
					   break;
				}			
		}//end for i
   UWE = norm_2(R)/sqrt(n-N);
  //////////////////////
  do
   {//begin of iteration
      ///////plan matrix C construction
      for (i=0;i<n;i++)
       {
        it = 0;	
        for (j = 0; j<q; j++)
	      {
		   if (act_var[j])
		    {
			  switch(psf_type)
                 {
			       case 0://Lorentzian
			              C(i,it) = JacobianL(j, data.Buffer[i].X, data.Buffer[i].Y,P);
			              break;
			       case 1://Gaussian
			              C(i,it) = JacobianG(j, data.Buffer[i].X, data.Buffer[i].Y,P);
			              break;
				   case 2://Gaussian
			              C(i,it) = JacobianM(j, data.Buffer[i].X, data.Buffer[i].Y,P);
			              break;
                 }			 
			 it++;
            }//end if			
		   }
        }//end for i
        ////////////////////////////////
		oldUWE = UWE;
		m = 0;
		do{//////////////////////////////////////////////////////////////////////////////////////////////////////
			W = prod(trans(C),C);
			///////////////////////////////
			for (j = 0; j<N; j++) D(j) = W(j,j);
			k = 0.005*m*norm_2(D);
			///////////////////////////////
			W = W + k*IR;
			W_1 = gjinverse(W, singular);
			h = prod(trans(C),R);
			dP = prod(W_1,h);
			////////////////////////////////
			it = 0;
			for (j = 0; j<q; j++)
			{
			  if(act_var[j]) 
				{
				  deltaP(j) = dP(it);
				  it++;
				}
			  else deltaP(j) = 0;
			}
			nextP = P + deltaP;
			////////////////////////////////
			for (i=0;i<n;i++)//residuals calculation, R vector forming
			{
			   switch(psf_type)
					{
					 case 0://Lorentzian
						   trR(i) = data.Buffer[i].I - Lorentzian_(data.Buffer[i].X, data.Buffer[i].Y,nextP);
						   break;
					 case 1://Gaussian
						   trR(i) = data.Buffer[i].I - Gaussian_(data.Buffer[i].X, data.Buffer[i].Y,nextP);
						   break;
					 case 2://Gaussian
						   trR(i) = data.Buffer[i].I - Moffat_(data.Buffer[i].X, data.Buffer[i].Y,nextP);
						   break;
					}			
			}//end for i
			UWE = norm_2(trR)/sqrt(n-N);
			str.setNum(UWE);
			m++;
			if (m == NN) 
			 {
			   oldUWE = UWE +1;
			   str = "ok";
			 }
		}while ((UWE>oldUWE)||(str == "nan"));
		////////////////////////////////
		P = nextP;
		R = trR;
	    g++;
		if (m == NN) g=GG;
   }//end of iteration   
   while((norm_2(deltaP)>PSt*norm_2(P))&&(g<GG)&&((oldUWE-UWE)/oldUWE>UWESt));
   ////////////////////////////////////
   QVector<double> PAR;
   PAR.clear();
   if (m != NN)
   {
	   W_1 = pow(UWE,2)*W_1;
	   /////////////////////////////////////
	  for (i=0;i<q;i++)
	  {
		PAR.append(P(i));
	  }//end for i
	  if (psf_type==1)PAR.append(-1);
	  //error recording
	  it = 0;
	  for (j = 0; j<q; j++)
	  {
	   if(act_var[j]) 
		{
		  PAR.append(sqrt(W_1(it,it)));
		  it++;
		}
	   else PAR.append(0);
	  }
	  if (psf_type==1)PAR.append(-1);
	  /////////////////////
	  PAR.append(UWE);
	  ////////////////////
	  k =0;
	  for (i=0;i<n;i++)//integral calculation
		{
		   switch(psf_type)
				{
				 case 0://Lorentzian
					   k = k + (Lorentzian_(data.Buffer[i].X, data.Buffer[i].Y,P)-P(0));
					   break;
				 case 1://Gaussian
					   k = k + (Gaussian_(data.Buffer[i].X, data.Buffer[i].Y,P)-P(0));
					   break;
				 case 2://Moffat
					   k = k + (Moffat_(data.Buffer[i].X, data.Buffer[i].Y,P)-P(0));
					   break;
				}			
		}//end for i
	  ////////////////////
	  PAR.append(k);
	  PAR.append(g);
	  PAR.append(psf_type);
  }
  else 
  {
    for (i=0;i<20;i++)PAR.append(-1); 
  }
  return PAR;
};


STIMG CenterOfMas(CCDDATA data)
{
 //QMessageBox::information(0,"debug","exellent",QMessageBox::Ok,0,0);
 STIMG CM;
 CM.X = 0;
 CM.Y = 0;
 CM.M = 0;
 for (quint16 i=0;i<data.Size;i++)
   {
     CM.X = CM.X+data.Buffer[i].X*data.Buffer[i].I;
	 CM.Y = CM.Y+data.Buffer[i].Y*data.Buffer[i].I;
	 CM.M = CM.M+data.Buffer[i].I;
   }//end for
 CM.X = CM.X/CM.M;
 CM.Y = CM.Y/CM.M;
 CM.M = CM.M/data.Size; 
 return CM;
};



double StImgWide(CCDDATA data, STIMG cofm, double delta)
{
 double wide = 0;
 double D = data.minADC;
 double Imax = data.maxADC - D;
 double r_2;
 double I_D;
 double r;
 int n = 0;
 for (int i=0;i<data.Size;i++)
   {
     r_2 = pow(data.Buffer[i].X-cofm.X,2)+pow(data.Buffer[i].Y-cofm.Y,2);
	 r = sqrt(r_2);
	 I_D = data.Buffer[i].I-D;
	 if ((I_D>0)&&(r>1))
	  {
       wide = wide+(pow(Imax/I_D,1/delta)-1)/r_2;
	   n++;
	  } 
   }//end for
 return wide/n;
};

double Lorentzian(quint16 x, quint16 y, QVector<double> *LorentzianV)
{
double L = LorentzianV->at(0)+LorentzianV->at(1)/pow((1+LorentzianV->at(2)*pow(x-LorentzianV->at(5),2)
	                             +LorentzianV->at(3)*pow(y-LorentzianV->at(6),2)+
								 LorentzianV->at(4)*(x-LorentzianV->at(5))*(y-LorentzianV->at(6))),LorentzianV->at(7));
return L;		
};

double Lorentzian_(quint16 x, quint16 y, vector<double> parameters)
{
double L = parameters(0)+parameters(1)/pow((1+parameters(2)*pow(x-parameters(5),2)
	                             +parameters(3)*pow(y-parameters(6),2)+
								 parameters(4)*(x-parameters(5))*(y-parameters(6))),parameters(7));
return L;

};

double Gaussian_(quint16 x, quint16 y, vector<double> parameters)
{
  double G = parameters(0)+parameters(1)*exp(-(parameters(2)*pow(x-parameters(5),2)
	                             +parameters(3)*pow(y-parameters(6),2)+
								 parameters(4)*(x-parameters(5))*(y-parameters(6)))); 
  return G;
};


double Gaussian(quint16 x, quint16 y, QVector<double> *GaussianV)
{
  double G = GaussianV->at(0)+GaussianV->at(1)*exp(-(GaussianV->at(2)*pow(x-GaussianV->at(5),2)
	                             +GaussianV->at(3)*pow(y-GaussianV->at(6),2)+
								 GaussianV->at(4)*(x-GaussianV->at(5))*(y-GaussianV->at(6)))); 
  return G;
};

double Moffat(quint16 x, quint16 y, QVector<double> *MoffatV)
{
 double Iof = MoffatV->at(0);
 double Im  = MoffatV->at(1);
 double A   = MoffatV->at(2);
 double B   = MoffatV->at(3);
 double C   = MoffatV->at(4);
 double Xo  = MoffatV->at(5);
 double Yo  = MoffatV->at(6);
 double delta = MoffatV->at(7);
 double ksi = x - Xo;
 double eta = y - Yo;
 double r = A*pow(ksi,2)+B*pow(eta,2)+C*(ksi)*(eta);
 double res = Iof + Im/(1+pow(r,delta));
 return res;
};


double Moffat_(quint16 x, quint16 y, vector<double> parameters)
{
 double Iof = parameters(0);
 double Im  = parameters(1);
 double A   = parameters(2);
 double B   = parameters(3);
 double C   = parameters(4);
 double Xo  = parameters(5);
 double Yo  = parameters(6);
 double delta = parameters(7);
 double ksi = x - Xo;
 double eta = y - Yo;
 double r = A*pow(ksi,2)+B*pow(eta,2)+C*(ksi)*(eta);
 double res = Iof + Im/(1+pow(r,delta));
 return res;
};

double JacobianL(int j, double X, double Y, vector<double> P)
{
 double Iof = P(0);
 double Im  = P(1);
 double A   = P(2);
 double B   = P(3);
 double C   = P(4);
 double Xo  = P(5);
 double Yo  = P(6);
 double delta = P(7);
 ////////////
 double J=0;
 double r = 1+A*pow(X-Xo,2)+B*pow(Y-Yo,2)+C*(X-Xo)*(Y-Yo);
 switch(j)
 {
  case 0:
        J = 1;
        break;
  case 1:
        J = 1/pow(r,delta);
        break;
  case 2:
        J = -Im*delta*pow(X-Xo,2)/pow(r,(delta+1));
        break;
  case 3:
        J = -Im*delta*pow(Y-Yo,2)/pow(r,(delta+1));
        break;
  case 4:
        J = -Im*delta*(X-Xo)*(Y-Yo)/pow(r,(delta+1));
        break;
  case 5:
        J = Im*delta*(2*A*(X-Xo)+C*(Y-Yo))/pow(r,(delta+1));
        break;
  case 6:
        J = Im*delta*(2*B*(Y-Yo)+C*(X-Xo))/pow(r,(delta+1));
        break;
  case 7:
        J = -Im*log(r)/pow(r,delta);
        break;
 }
 return J;
};

double JacobianG(int j, double X, double Y, vector<double> P)
{
double J=0;
double ksi = X-P(5);
 double eta = Y-P(6);
 double r = P(2)*pow(ksi,2)+P(3)*pow(eta,2)+P(4)*ksi*eta;
 switch(j)
 {
  case 0:
        J = 1;
        break;
  case 1:
        J = exp(-r);
        break;
  case 2:
        J = -P(1)*pow(ksi,2)*exp(-r);
        break;
  case 3:
        J = -P(1)*pow(eta,2)*exp(-r);
        break;
  case 4:
        J = -P(1)*ksi*eta*exp(-r);
        break;
  case 5:
        J = P(1)*(P(2)*ksi+P(4)*eta)*exp(-r);
        break;
  case 6:
        J = P(1)*(P(3)*eta+P(4)*ksi)*exp(-r);
        break;
 }
 return J;
};


double JacobianM(int j, double X, double Y, vector<double> P)
{
 double Iof = P(0);
 double Im  = P(1);
 double A   = P(2);
 double B   = P(3);
 double C   = P(4);
 double Xo  = P(5);
 double Yo  = P(6);
 double delta = P(7);
 ////////////
 double J=0;
 double ksi = X - Xo;
 double eta = Y - Yo;
 double r = A*pow(ksi,2)+B*pow(eta,2)+C*(ksi)*(eta);
 double Z = pow((1+pow(r,delta)),2);
 switch(j)
 {
  case 0:
        J = 1;
        break;
  case 1:
        J = 1/(1+pow(r,delta));
        break;
  case 2:
        J = -Im*delta*pow(r,delta-1)*pow(ksi,2)/Z;
        break;
  case 3:
        J = -Im*delta*pow(r,delta-1)*pow(eta,2)/Z;
        break;
  case 4:
        J = -Im*delta*pow(r,delta-1)*ksi*eta/Z;
        break;
  case 5:
        J = Im*delta*pow(r,delta-1)*(2*A*(ksi)+C*(eta))/Z;
        break;
  case 6:
        J = Im*delta*pow(r,delta-1)*(2*B*(eta)+C*(ksi))/Z;
        break;
  case 7:
        J = -Im*pow(r,delta)*log(r)/Z;
        break;
 }
 return J;
};
/////////////////////////////////////////////////////////////////////////////////

void histFilter(double *ccdM, quint16 fWidth, quint16 fHeight, int size)
{
  QString str;
  int ksi,eta,x, y,k, hsize,dsize = (2*size+1)*(2*size+1), prN;
  quint32 fsize = fWidth*fHeight;
  quint32 pix[dsize];
  quint32 minP, maxP, Px, indx;
  double s,minPV,currPix = 0;
  double *rData = new double[fsize];
  for (int i=0;i<fsize;i++)rData[i] = ccdM[i];
  quint64 *hist_data;
  for (eta=0;eta<fHeight;eta++)
   {
    ///////////////////////
     for (ksi=0;ksi<fWidth;ksi++)
	    {
		  ////////////////////
		  ////load row data into array
          minP = (quint32)ccdM[ksi + eta*fWidth];
          maxP = minP;
          k = 0;		  
		  for (y = eta-size; y <= eta+size; y++)
		  {
		     for (x = ksi-size; x <= ksi+size; x++)
			 {
			    if ((x>=0)&&(x<fWidth)&&(y>=0)&&(y<fHeight))
				{
				    //QMessageBox::information(0,"debug",str.setNum(x),QMessageBox::Ok,0,0);
					pix[k] = (quint32)rData[x + y*fWidth];
					//str.setNum(pix[k]);
	                //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
					if (maxP<pix[k])maxP = pix[k];
					if (minP>pix[k])minP = pix[k];
					k++;
				}
			 }
		  }
		  dsize = k;
		  //QMessageBox::information(0,"debug","832",QMessageBox::Ok,0,0);
		  /////////hist
		  hsize = maxP - minP + 1;
		  hist_data = new quint64[hsize];
		  for (k=0;k<hsize;k++) hist_data[k] = 0;
		  for (k=0;k<dsize;k++) hist_data[pix[k]-minP]++;
		  prN = 0;
		  for (k=0;k<hsize;k++) if(hist_data[k]>hist_data[prN]) prN=k;
		  delete hist_data;
		  Px = minP+prN;
		  /////////
		  /*s = 0;
		  for (k=0;k<dsize;k++) 
		   {
		     s = s + pow((double)pix[k]-(double)Px,2);
		   }
		  s = sqrt(s/dsize);
		  */////////
		  indx = ksi + eta*fWidth;
		  currPix = Px;
		  //if(fabs(currPix)>s)currPix = 0;
		  //if(currPix<minPV)minPV = currPix;
          ccdM[indx] = currPix;		  
		  //////
		}
   }
   delete rData;
   //minPV = fabs(minPV);
   //for (int i=0;i<fsize;i++)ccdM[i] = ccdM[i]+minPV;
};
////////////////////////////////////////////////////////////////////////////////////

void medianFilter(double *ccdM, quint16 fWidth, quint16 fHeight, int size, QProgressBar *pb, int use_pb)
{
  QString str;
  int ksi,eta,x, y;
  quint32 fsize = fWidth*fHeight;
  quint32 Px, indx;
  double currPix, minPV = 0;
  double *rData = new double[fsize];
  for (int i=0;i<fsize;i++)rData[i] = ccdM[i];
  QVector<double> bData;
  for (eta=0;eta<fHeight;eta++)
   {
    ///////////////////////
     for (ksi=0;ksi<fWidth;ksi++)
	    {
		  ////load row data into array
          bData.clear();		  
		  for (y = eta-size; y <= eta+size; y++)
		  {
		     for (x = ksi-size; x <= ksi+size; x++)
			 {
			    if ((x>=0)&&(x<fWidth)&&(y>=0)&&(y<fHeight))
				{
					bData << rData[x + y*fWidth];
				}
			 }
		  }
		  currPix = ccdM[ksi + eta*fWidth] - median(&bData);
		  if(currPix<-100) currPix = -100;
		  if(currPix<minPV)minPV=currPix;
          ccdM[ksi + eta*fWidth] = currPix;		  
		  //////
		  if (use_pb==1) pb->setValue(100*(double)(ksi + eta*fWidth)/((double)fsize));
		}
   }
   delete rData;
   for (int i=0;i<fsize;i++)ccdM[i] = ccdM[i] - minPV;
};

void subtractPSF(QVector<double> *psfP, int psfType, double *ccdM, quint16 Xc, quint16 Yc, quint16 ro, quint16 fWidth, quint16 fHeight)
{
   int i,j;
   quint32 k = 0;
   double r;
   for (i=Xc-ro;i<Xc+ro;i++)
   {
    for (j=Yc-ro;j<Yc+ro;j++)
       {
	    if ((i>=0)&&(i<fWidth)&&(j>=0)&&(j<fHeight))
		 {
	        r = sqrt(pow(i-Xc,2)+pow(j-Yc,2));
            if (r<=ro)
		    {
			   k = i + j*fWidth;
			   switch(psfType)
				 {
				   case 0://Lorentzian
						  ccdM[k] = ccdM[k] - Lorentzian(i,j,psfP) + psfP->at(0);
						  break;
				   case 1://Gaussian
						  ccdM[k] = ccdM[k] - Gaussian(i,j,psfP) + psfP->at(0);
						  break;
				   case 2://Moffat
						  ccdM[k] = ccdM[k] - Moffat(i,j,psfP) + psfP->at(0);
						  break;
				 }	
            }
         }			
       }//end for j	   
   }//end for i  
};

///////////////////////////////////////////////////////////////////////////////////

vector<double> LSM(vector<double> L, matrix<double> C, vector<double> weights)
{
  int i;
  bool singular;
  mapped_matrix<double> P (weights.size(), weights.size(), weights.size());
  for (i = 0; i<weights.size();i++) P(i,i) = weights(i);
  matrix<double> PC = prod(P,C);
  vector<double> PL = prod(P,L);
  matrix<double> W = prod(trans(C),PC);
  vector<double> H = prod(trans(C),PL);
  matrix<double> W_1 = gjinverse(W, singular);
  vector<double> Z = prod(W_1,H);
  return Z;
};

matrix<double> LSM_err(vector<double> L, matrix<double> C, vector<double> weights)
{
  int i;
  bool singular;
  mapped_matrix<double> P (weights.size(), weights.size(), weights.size());
  for (i = 0; i<weights.size();i++) P(i,i) = weights(i);
  matrix<double> PC = prod(P,C);
  vector<double> PL = prod(P,L);
  matrix<double> W = prod(trans(C),PC);
  vector<double> H = prod(trans(C),PL);
  matrix<double> W_1 = gjinverse(W, singular);
  vector<double> Z = prod(W_1,H);
  vector<double> R = L - prod(C,Z);
  vector<double> PR = prod(P,R);
  double uerr = inner_prod(R,PR)/(C.size1()-C.size2());
  matrix<double> D = uerr*W_1;
  matrix<double> res(Z.size()+1,2);
  res(0,0)=0; res(0,1) = sqrt(uerr);
  for (i=0;i<Z.size();i++)
  {
    res(i+1,0) = Z(i);res(i+1,1) = sqrt(D(i,i));
  }
  return res;
};

matrix<double> LSM_corr(vector<double> L, matrix<double> C, vector<double> weights, double max_res)
{
  int i,j;
  bool singular, new_loop;
  //////////////////////////
  matrix<double> PC;
  vector<double> PL;
  matrix<double> W;
  vector<double> H;
  matrix<double> W_1;
  vector<double> Z;
  vector<double> R;
  QVector<int> indexes;
  /////////////////////////
  matrix<double> C_;
  vector<double> L_;
  vector<double> weights_;
  //////////////////////////
  int K = C.size1() - C.size2();
  int N = 0;
  do
  {
    N++;
    PC = C;
	for (i=0;i<PC.size1();i++)
	   for (j=0;j<PC.size2();j++)
	        PC(i,j) = weights(i)*PC(i,j);
	/////////////////////////////////////////////
	PL = L;
	for (i=0;i<PL.size();i++) PL(i) = weights(i)*PL(i);
	///////////////////////////////////////////
	W = prod(trans(C),PC);
    H = prod(trans(C),PL);
    W_1 = gjinverse(W, singular);
    Z = prod(W_1,H);
    R = L - prod(C,Z);
	//////////////////////////////////////////
	indexes.resize(0);
	for (i=0;i<R.size();i++)
	{
	   if (fabs(R(i)) < max_res) indexes << i;
	}
	/////////////////////////////////////////
	if ((indexes.count()<R.size())&&(indexes.count()>C.size2()))
	{
	 new_loop = true;
     ///////////////////////////
	 C_.resize(indexes.count(),C.size2());
	 L_.resize(indexes.count());
	 weights_.resize(indexes.count());
	 ////////////////////////////
	 for (i=0;i<indexes.count();i++)
	 {
	   L_(i) = L(indexes[i]);
	   weights_(i) = weights(indexes[i]);
	   for (j=0;j<C.size2();j++)
	       C_(i,j) = C(indexes[i],j);
	 }
	 ////////////////////////////
	 C = C_;
	 L = L_;
	 weights = weights_;
	}
	else new_loop = false;
  }
  while((new_loop)&&(N<K));
  //////////////////////////
  vector<double> PR = R;
  for (i=0;i<PR.size();i++) PR(i) = weights(i)*PR(i);
  //////////////////////////////////
  double uerr = inner_prod(R,PR)/(C.size1()-C.size2());
  matrix<double> D = uerr*W_1;
  matrix<double> res(Z.size()+1,2);
  res(0,0)=C.size1(); res(0,1) = sqrt(uerr);
  for (i=0;i<Z.size();i++)
  {
    res(i+1,0) = Z(i);res(i+1,1) = sqrt(D(i,i));
  }
  return res;
};

matrix<double> fromDataToMatrix(double *data, int sz)
{
   int i,N;
   matrix<double> M(sz,26);
   double *md = M.data().begin();
   N = 26*sz;
   for (i = 0; i<N; i++) md[i] = data[i];
   return M;
};

STDATA StDataFromVector(QVector<double> v)
{
  double m=0, err = 0;
  if (v.count()>2)
  {
	  
	  int i;
	  for (i=0;i<v.count();i++) m = m + v[i];
	  m = m/v.count();
	  for (i=0;i<v.count();i++) err = err + pow(v[i]-m,2);
	  err = sqrt(err/(v.count()-1));
  }
  STDATA StData;
  StData.mean = m;
  StData.st_err = err;
  return StData;  
};

double MaxOfVector(QVector<double> v)
{
  double max_v = v[0];
  for (int i = 0; i<v.size();i++)
  {
    if (v[i]>max_v) max_v = v[i];
  }
  return max_v;
};

double MinOfVector(QVector<double> v)
{
  double min_v = v[0];
  for (int i = 0; i<v.size();i++)
  {
    if (v[i] < min_v) min_v = v[i];
  }
  return min_v;
};


void VectorToFile(QString FileName, vector<double> OUT)
{
   QFile outputfile(FileName);
   outputfile.open(QIODevice::WriteOnly | QIODevice::Text);
   QTextStream out_stream;
   out_stream.setDevice(&outputfile);
   QString S;
   /////////////////////////////////////////
   int m = OUT.size();
   int i;
   for (i=0; i<m;i++)
   {
    S = "";
    S = S+QString( "%1" ).arg( OUT(i),22,'f',15,QLatin1Char( ' ' ));
	S = S+'\n';
	//QMessageBox::information(0,"debug",S,QMessageBox::Ok,0,0);
	out_stream << S;
	}   
   outputfile.close();
};

void MtrxToFile(QString FileName, matrix<double> OUT)
{
   QFile outputfile(FileName);
   outputfile.open(QIODevice::WriteOnly | QIODevice::Text);
   QTextStream out_stream;
   out_stream.setDevice(&outputfile);
   QString S;
   /////////////////////////////////////////
   int m = OUT.size1();
   int n = OUT.size2();
   int i,j;
   for (i=0; i<m;i++)
   {
    S = "";
    for (j=0; j<n;j++)
      {
		S = S+QString( "%1" ).arg( OUT(i,j),22,'f',15,QLatin1Char( ' ' ))+" ";
       }
	S = S+'\n';
	//QMessageBox::information(0,"debug",S,QMessageBox::Ok,0,0);
	out_stream << S;
	}   
   outputfile.close();
};

int getNearest(QVector<QPoint> ol, double x, double y, double rho)
{
   double r, rmin = 10000;
   int N=-1;
   for (int i = 0; i<ol.count();i++)
   {
      QPoint pt = ol[i];
      r = sqrt(pow(pt.x()-x,2)+pow(pt.y()-y,2));
	  if  (r<rho)
	  {
	    if (r < rmin)
		{
		  rmin = r;
		  N = i;
		}
	  }
   }
   return N;
};

double median(QVector<double> *data)
{
  int max_indx;
  QVector<double> copy_data;
  for (int i=0;i<data->count();i++)copy_data << data->at(i);
  data->clear();
  while(copy_data.count()>1)
  {
	  max_indx = 0;
	  for (int j=0;j<copy_data.count();j++)
	  {
	    if(copy_data[j]>copy_data[max_indx])max_indx = j;
	  }
	  data->append(copy_data[max_indx]);
	  copy_data.remove(max_indx);
  }
  data->append(copy_data[0]);
  return data->at(data->count()/2+1);
};

void test()
{
  matrix<double> M(2,2);
  M(0,0) = 0.234;
  M(0,1) = -5.578;
  M(1,0) = 1e-4;
  M(1,1) = 34.12434;
  double *md = M.data().begin();
  QString S;
  for (int i = 0; i<4; i++)
  {
   S.setNum(md[i]);
   QMessageBox::information(0,"debug",S,QMessageBox::Ok,0,0);
  }
};
