#include <QtCore>
//#include <QtGui>
#include "ccdmeasurements.h"


imgAperture::imgAperture()
{
    //delete [] buffer;
	buffer = NULL;
	Size = 0;
	boxW = 0;
	boxH = 0;
	X = 0;
	Y = 0;
	maxADC = 0;
	minADC = 0;
	meanADC = 0;
	seADC = 0;
	Xmax = 0;
	Ymax = 0;
}

imgAperture::imgAperture(qint16 iSize)
{
	imgAperture();
        //buffer = (PPIX) malloc(Size*sizeof(PPIX));
        buffer = new PIX[iSize*sizeof(PIX)];
        this->Size = iSize;
}

void imgAperture::copyTo(imgAperture *target)
{
    //qDebug() << QString("imgAperture::copyTo\n");
        //target->buffer = (PPIX) malloc(Size*sizeof(PPIX));
        for(qint16 i=0; i<Size; i++)
	{
//		QMessageBox::information(0,"debug1",QString("%1/%2").arg(i).arg(Size),QMessageBox::Ok,0,0);
//		QMessageBox::information(0,"debug1",QString("%1 %2 %3").arg(i).arg(target->buffer[i].X).arg(target->buffer[i].Y).arg(target->buffer[i].I),QMessageBox::Ok,0,0);
		target->buffer[i].X = buffer[i].X;
		target->buffer[i].Y = buffer[i].Y;
		target->buffer[i].I = buffer[i].I;
        }
	target->Size = Size;
	target->X = X;
	target->Xmax = Xmax;
	target->Y = Y;
	target->Ymax = Ymax;
	target->boxH = boxH;
	target->boxW = boxW;
	target->maxADC = maxADC;
	target->meanADC = meanADC;
	target->seADC = seADC;
}

imgAperture::~imgAperture()
{
	delete [] buffer;
}

void imgAperture::detProp()
{
	qint16 i;
	minADC = -1;
	maxADC = -1;
	meanADC = 0;
	Xmax = 0;
	Ymax = 0;
	Pmax = 0;
	for(i=0; i<Size; i++)
	{
		if (minADC == -1) minADC = buffer[i].I;
		if (maxADC == -1) maxADC = buffer[i].I;
		if (buffer[i].I>maxADC) 
		{
			maxADC = buffer[i].I;
			Xmax = buffer[i].X;
			Ymax = buffer[i].Y;
			Pmax = i;
		}
		if (buffer[i].I<minADC) minADC = buffer[i].I;
		meanADC = meanADC + buffer[i].I;
		
	}
	meanADC = meanADC/Size;
	
	for (i=0;i<Size;i++) seADC = seADC+pow(buffer[i].I - meanADC,2);
	seADC = sqrt(seADC/(Size-1));
}

void imgAperture::remPoint(qint16 pos)
{
	if(pos>Size||pos<0) return;
	
	for(qint16 i=pos; i<Size-1; i++)
	{
		buffer[i].X = buffer[i+1].X;
		buffer[i].Y = buffer[i+1].Y;
		buffer[i].I = buffer[i+1].I;
	}
	Size--;
}

imgAperture& imgAperture::operator=(const imgAperture &source)
{
    if(buffer!=NULL) delete [] buffer;
    Size = source.Size;
    buffer = (PPIX) malloc(Size*sizeof(PPIX));

     int i;
     for(i=0; i<Size; i++)
     {
        buffer[i].X = source.buffer[i].X;
        buffer[i].Y = source.buffer[i].Y;
        buffer[i].I = source.buffer[i].I;
     }
    Size = source.Size;
    X = source.X;
    Xmax = source.Xmax;
    Y = source.Y;
    Ymax = source.Ymax;
    boxH = source.boxH;
    boxW = source.boxW;
    maxADC = source.maxADC;
    meanADC = source.meanADC;
    seADC = source.seADC;

    return *this;
}
/*
void imgAperture::remPoint(int x, int y)
{
//	int i;
}
*/

double PointData(double *ccdM, quint16 Xc, quint16 Yc, quint16 fWidth)
{
 return ccdM[Xc + Yc*fWidth];
};
/*
double MEFP(matrix<double> M, quint16 Xc, quint16 Yc)
{
  return M(Xc,Yc);
};
*/

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

void ApeData(imgAperture *apeData, double *ccdM, quint16 Xc, quint16 Yc, quint16 ro, quint16 fWidth, quint16 fHeight)
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
  quint16 P_max = 0;
//  QMessageBox::information(0,"Xc diap", QString("%1 %2").arg(Xc-ro).arg(Xc+ro),QMessageBox::Ok,0,0);
//  QMessageBox::information(0,"Yc diap", QString("%1 %2").arg(Yc-ro).arg(Yc+ro),QMessageBox::Ok,0,0);
  //qDebug() << QString("Xc diap: %1 - %2\n").arg(Xc-ro).arg(Xc+ro);
  //qDebug() << QString("Yc diap: %1 - %2\n").arg(Yc-ro).arg(Yc+ro);
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
                            //qDebug() << QString("ccdM[%1]= %2\n").arg(i + j*fWidth).arg(ccdM[i + j*fWidth]);
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
                   P_max = n;
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
   //qDebug() << QString("meanI= %1\n").arg(meanI);
 //  QMessageBox::information(0,"poinT", QString("%1").arg(meanI),QMessageBox::Ok,0,0);
  ///////////////aperture buffer is filled
//   CCDDATA RES;
//   apeData.Buffer = new PIX[n*sizeof(PIX)];
	apeData->buffer = new PIX[n*sizeof(PIX)];
        //qDebug() << "apeData->buffer " << apeData->buffer << "\n";
   double seI = 0;
   for (i=0;i<n;i++)
    {
	  apeData->buffer[i] = PA[i];
	  seI = seI+pow((PA[i].I - meanI),2);
	}
//	QMessageBox::information(0,"poinT", "",QMessageBox::Ok,0,0);
	seI = sqrt(seI/(n-1));
	apeData->Size = n;
	apeData->boxW = 2*ro;
	apeData->boxH = 2*ro;
	apeData->X = Xc;
	apeData->Y = Yc;
	apeData->maxADC = maxI;
	apeData->minADC = minI;
	apeData->meanADC = meanI;
	apeData->seADC = seI;
	apeData->Xmax = X_max;
	apeData->Ymax = Y_max;
	apeData->Pmax = P_max;
        //qDebug() << "end0\n";
	delete[] PA;
        //qDebug() << "end1\n";
	return;
};

int stDif(STIMG *st0, STIMG *st1)
{
	return((int)sqrt((st0->X-st1->X)*(st0->X-st1->X) + (st0->Y-st1->Y)*(st0->Y-st1->Y)));
}

STIMG CenterOfMas(imgAperture *data)
{
    //qDebug() << "\ncenterofmas\n";
	quint16 i;
	STIMG CM, CM0;
	CM0.X = CM.X = 0;
	CM0.Y = CM.Y = 0;
	CM0.M = CM.M = 0;
        imgAperture *dLoc;
        dLoc = new imgAperture(data->Size);
        //dLoc->buffer = new PIX[data->Size*sizeof(PIX)];
        //dLoc->buffer = (PPIX) malloc(data->Size*sizeof(PPIX));
        //qDebug() << "\ncoping\n";
        data->copyTo(dLoc);
        //qDebug() << "\ncopied\n";
        //dLoc = data;
	int r0;
//	double d = dLoc->maxADC - dLoc->minADC;
	double d0 = dLoc->meanADC - (dLoc->meanADC-dLoc->minADC)/4.0;
	double d1 = dLoc->meanADC + (dLoc->maxADC-dLoc->meanADC)/2.0;
	int dS;
	do
	{
                dLoc->detProp();
	 	CM0.X = CM.X;
		CM0.Y = CM.Y;
		CM0.M = CM.M;
		
		CM.X = 0;
		CM.Y = 0;
	 	CM.M = 0;
		dS = 0;
                for (i=0;i<dLoc->Size;i++)
		{
//			if((dLoc->buffer[i].I<d1)&&(dLoc->buffer[i].I>d0))
                        if(dLoc->buffer[i].I>dLoc->meanADC)
			{
                                CM.X = CM.X+dLoc->buffer[i].X*dLoc->buffer[i].I;
				CM.Y = CM.Y+dLoc->buffer[i].Y*dLoc->buffer[i].I;
				CM.M = CM.M+dLoc->buffer[i].I;
				dS++;
			}
	   	}//end for
		CM.X = CM.X/CM.M;
		CM.Y = CM.Y/CM.M;
		CM.M = CM.M/dS;
	
		dLoc->remPoint(dLoc->Pmax);
                dLoc->detProp();
	 
		r0 = stDif(&CM0, &CM);
               // qDebug() << QString("r0= %1\tdLoc->Size= %2\n").arg(r0).arg(dLoc->Size);
        }while(r0>1&&dLoc->Size>2);
        delete dLoc;
        //qDebug() << QString("cm.X= %1\tcm.Y= %2\n").arg(CM.X).arg(CM.Y);
	return CM;
};


void StartConditionsL(imgAperture *data, double delta, double *P)
{
	if(P==NULL) return;
//  P = new double[8];
  STIMG CM = CenterOfMas(data);//"center of mass"
  P[0] = data->minADC;//"minimum of I - 1"
  P[1] = data->maxADC - P[0];//"maximum of I"
  P[2] = StImgWide(data,CM,delta);//Image width
  P[3] = P[2];
  P[4] = 0;
  P[5] = CM.X;
  P[6] = CM.Y;
  P[7] = delta;
//  return P;
};

void StartConditionsG(imgAperture *data, double *P)
{
	if(P==NULL) return;
// vector<double> P(7);
 STIMG CM = CenterOfMas(data);//"center of mass"
 P[0] = data->minADC-2;
 P[1] = data->maxADC - P[0];
 //int n = 0;
 double r;
 double A = 0;
 for (int i=0;i<data->Size;i++)
 {
   r = pow(data->buffer[i].X-CM.X,2)+pow(data->buffer[i].Y-CM.Y,2);
   A = A + log(P[1]/(data->buffer[i].I-P[0]))/r;
 }
 P[2] = A/data->Size;
 P[3] = P[2];
 P[4] = 0;
 P[5] = CM.X;
 P[6] = CM.Y;
//  return P;
}

void StartConditionsM(imgAperture *data, double delta, double *P)
{
	if(P==NULL) return;
//  vector<double> P(8);
  double r;
  STIMG CM = CenterOfMas(data);//"center of mass"
  P[0] = data->minADC;//"minimum of I - 1"
  P[1] = data->maxADC - P[0];//"maximum of I"
  P[2] = 0;
  int n = 0;
  for (int i=0;i<data->Size;i++)
  {
    r = pow(data->buffer[i].X-CM.X,2)+pow(data->buffer[i].Y-CM.Y,2);
	if ((data->buffer[i].I-P[0])!=0)
	{
     P[2] = P[2]+pow(P[1]/(data->buffer[i].I-P[0]),1/delta)/r;
	 n++;
	} 
  }
  P[2] = P[2]/n;
  P[3] = P[2];
  P[4] = 0;
  P[5] = CM.X;
  P[6] = CM.Y;
  P[7] = delta;
//  return P;
}

double StImgWide(imgAperture *data, STIMG cofm, double delta)
{
 double wide = 0;
 double D = data->minADC;
 double Imax = data->maxADC - D;
 double r_2;
 double I_D;
 double r;
 int n = 0;
 for (int i=0;i<data->Size;i++)
   {
     r_2 = pow(data->buffer[i].X-cofm.X,2)+pow(data->buffer[i].Y-cofm.Y,2);
	 r = sqrt(r_2);
	 I_D = data->buffer[i].I-D;
	 if ((I_D>0)&&(r>1))
	  {
       wide = wide+(pow(Imax/I_D,1/delta)-1)/r_2;
	   n++;
	  } 
   }//end for
 return wide/n;
};

STDATA StDataFromVector(double *v, int size)
{
	  double m=0, err = 0;
	  if (size>2)
	  {
		  
		  int i;
		  for (i=0;i<size;i++) m = m + v[i];
		  m = m/size;
		  for (i=0;i<size;i++) err = err + pow(v[i]-m,2);
		  err = sqrt(err/(size-1));
	  }
	  STDATA StData;
	  StData.mean = m;
	  StData.st_err = err;
	  return StData;  
}

void sortM2M(double *vect, int size, int dim, int elem, int direct)
{
	double *x = new double[dim];
	int i, j, k, num, vPos;
	double val, temp;
	num = size/dim;
	val = vect[elem];
	for(i=0; i<num; i++)
	{
		val = vect[i*dim+elem];
		vPos= i;
		for(j=i+1; j<num; j++)
		{
			qDebug() << i << ":" << j << "|" << val << ":" << vect[j*dim+elem] << "\n";
			if((val-vect[j*dim+elem])*direct>0.0)
			{
//				qDebug() << val << ":" << vect[j*dim+elem] << "\n";
				val = vect[j*dim+elem];
				vPos = j;
			}
		}
		if(vPos==i) continue;
		for(k = 0; k<dim; k++)
		{
			
			temp = vect[i*dim+k];
			vect[i*dim+k] = vect[vPos*dim+k];
			vect[vPos*dim+k] = temp;
		}
		qDebug() << i << ":" << vect[i*dim+elem] << "\n";
		
	}
}

double vectAng(double *v0, double *v1)
{
	return acos((v0[0]*v1[0] + v0[1]*v1[1])/(sqrt(v0[0]*v0[0] +v0[1]*v0[1])*sqrt(v1[0]*v1[0] +v1[1]*v1[1])));
}

int marksIdentify(double *m0, int mSize0, double *m1, int mSize1, int pNum)	//m0 - метки с отбросами (xi, yi, mi) [3*mSize0]; m1 - тангенсальные координаты (xi, yi, Ii) [3*mSize1]/
{
	if((mSize1<pNum)||(mSize0<pNum)) return 2;
	int iNum;
	int i, j, k;
	int begPos, endPos;
/*	
	qDebug() << "\nm0\n";
	for(i=0; i<mSize0; i++) qDebug() << "\n" << m0[i*3] << "|" << m0[i*3+1] << "|" << m0[i*3+2] << "\n";
	qDebug() << "\nm1\n";
	for(i=0; i<mSize1; i++) qDebug() << "\n" << m1[i*3] << "|" << m1[i*3+1] << "|" << m1[i*3+2] << "\n";
*/
	sortM2M(m0, mSize0*3, 3, 2, -1);
	sortM2M(m1, mSize1*3, 3, 2, 1);
	
	/*
	qDebug() << "\nm0\n";
	for(i=0; i<mSize0; i++) qDebug() << "\n" << m0[i*3] << "|" << m0[i*3+1] << "|" << m0[i*3+2] << "\n";
	
	qDebug() << "\nm1\n";
	for(i=0; i<mSize1; i++) qDebug() << "\n" << m1[i*3] << "|" << m1[i*3+1] << "|" << m1[i*3+2] << "\n";
*/
	
//	iNum = fakt(pNum);
	iNum = pNum - 1;
	
	double *ang0 = new double[iNum];
	double *ang1 = new double[iNum];
	double *difs = new double[iNum];
	
	double *m00 = new double[2];
	double *m01 = new double[2];
	double *m10 = new double[2];
	double *m11 = new double[2];
	
	double *mks = new double[4*iNum-2];
	double *mksZ = new double[4*iNum-2];	
	
	begPos = 0; endPos = begPos+pNum;
	k = 0;
	int p;
//	for(k=0; k<5; k++)
//	{

		for(i=0;i<5; i++)
		{
			p=0;
			m00[0] = m0[3] - m0[0];
			m00[1] = m0[4] - m0[1];
			
			m10[0] = m1[(i+1)*3] - m1[0];
			m10[1] = m1[(i+1)*3+1] - m1[1];
			
			qDebug() << "\nm00" << m00[0] << "|" << m00[1] << "\n";
			qDebug() << "\nm10" << m10[0] << "|" << m10[1] << "\n";
			
			for(j=2; j<endPos; j++)
			{
				m01[0] = m0[j*3] - m0[0];
				m01[1] = m0[j*3+1] - m0[1];
				m11[0] = m1[j*3] - m1[0];
				m11[1] = m1[j*3+1] - m1[1];
				ang0[p] = vectAng(m00, m01);
				ang1[p] = vectAng(m10, m11);
				difs[p] = fabs(ang0[p] - ang1[p]);
				
				qDebug() << "\nm01" << m01[0] << "|" << m01[1] << "\n";
				qDebug() << "\nm11" << m11[0] << "|" << m11[1] << "\n";
				qDebug() << p << ": " << ang0[p] << " - " << ang1[p] << " = " << difs[p] << "\n";
				
				p+=1;
			}
		}
//	}
}



/*
void NLSLevenbergMarquardt(imgAperture *data, double *P, int pSize, int psf_type, int *act_var, int N, int NN, int GG, double PSt, double UWESt)
/*
data is an array of PIX (row pixel data)
P is a vector of initial conditions
psf_type is a type of PSF
act_var is an array, 1 denotes included parameter and 0 denotes excluded parameter
N is number of actual (effective) parameters
/
{
  QString str;
  //str.setNum(PSt);
  //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
  //str.setNum(UWESt);
  //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
  /////////////
  int i,j,n,it;
  bool singular;
  n = data->Size;
  int q = pSize; 
  double *dP = new double[N];//vector of needed corrections
  double *deltaP = new double[q];//full vector of corrections
  double *nextP = new double[q];// test vector of corrections
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
				 case 3://Gradient 1/(1+r)
					   R(i) = data.Buffer[i].I - Grad_(data.Buffer[i].X, data.Buffer[i].Y,P);
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
				   case 3://Gradient 1/(1+r)
			              C(i,it) = JacobianGrad(j, data.Buffer[i].X, data.Buffer[i].Y,P);
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
					 case 3://Gradient 1/(1+r)
						   R(i) = data.Buffer[i].I - Grad_(data.Buffer[i].X, data.Buffer[i].Y,P);
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
				 case 3://Moffat
					   k = k + (Grad_(data.Buffer[i].X, data.Buffer[i].Y,P)-P(0));
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
*/
////////////////////////////////////////////////////////////////////////
/*
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
  for (j=Yc-boxH;j<Yc+boxH;j++)
   {
    for (i=Xc-boxW;i<Xc+boxW;i++)
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
/
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
				 case 3://Gradient 1/(1+r)
					   R(i) = data.Buffer[i].I - Grad_(data.Buffer[i].X, data.Buffer[i].Y,P);
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
				   case 3://Gradient 1/(1+r)
			              C(i,it) = JacobianGrad(j, data.Buffer[i].X, data.Buffer[i].Y,P);
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
					 case 3://Gradient 1/(1+r)
						   R(i) = data.Buffer[i].I - Grad_(data.Buffer[i].X, data.Buffer[i].Y,P);
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
				 case 3://Moffat
					   k = k + (Grad_(data.Buffer[i].X, data.Buffer[i].Y,P)-P(0));
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

double DMoffat(double x, double y, QVector<double> *MoffatV)
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

double Grad_(quint16 x, quint16 y, vector<double> parameters)
{
	double grad =parameters(0)+parameters(1)/(1+sqrt(pow(x-parameters(2),2)+pow(y-parameters(3),2)));
	return grad;
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
double JacobianGrad(int j, double X, double Y, vector<double> P)
{
 double Jo = P(0);
 double Jmax  = P(1);
 double Xo  = P(2);
 double Yo  = P(3);
 ////////////
 double J=0;
 double ksi = X - Xo;
 double eta = Y - Yo;
 double r = sqrt(pow(ksi,2)+pow(eta,2));
 switch(j)
 {
  case 0:
        J = 1;
        break;
  case 1:
        J = 1/(1+r);
        break;
  case 2:
        J = Jmax*ksi/(r*pow(1+r,2));
        break;
  case 3:
        J = Jmax*eta/(r*pow(1+r,2));
        break;
 }
 return J;
};



/////////////////////////////////////////////////////////////////////////////////////

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
		  /
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

matrix<double> FT(QVector<double> v)
{
  matrix<double> res(v.count(),2);
  double uR,uIm;
  double PI = atan(1)*4;
  int N = v.count();
  for(int i=0;i<N;i++)
  {
    uR = 0; uIm = 0;
	for(int j=0;j<N;j++)
	{
	 uR = uR + v[j]*cos(2*PI*i*j/N);
	 uIm = uIm - v[j]*sin(2*PI*i*j/N);
	}
	res(i,0) = uR;
	res(i,1) = uIm;
  }
  return res;
};

matrix<double> IFT(matrix<double> ftm)
{
   int N = ftm.size1();
   matrix<double> res(N,2);
   double uR,uIm,r;
   double PI = atan(1)*4;
   for(int i=0;i<N;i++)
   {
      uR = 0; uIm = 0; 
	  for(int j=0;j<N;j++)
	   {  
	     uR = uR + ftm(j,0)*cos(2*PI*i*j/N)-ftm(j,1)*sin(2*PI*i*j/N);
	     uIm = uIm + ftm(j,0)*sin(2*PI*i*j/N)+ftm(j,1)*cos(2*PI*i*j/N);
	   } 
	  res(i,0) = uR/N;
	  res(i,1) = uIm/N;
   }
   return res;
};

void clearBoxNoise(double *ccdM, quint16 Xc, quint16 Yc, quint16 boxWidth, quint16 boxHeight, quint16 fWidth, quint16 fHeight)
{
   QVector<double> v;
   QVector<int> idx;
   int x,y;
   int bW = boxWidth/2;
   int bH = boxHeight/2;
   for (x = Xc-bW;x<Xc+bW;x++)
   {
     for (y = Yc-bH;y<Yc+bH;y++)
	 {
	   if((x>=0)&&(y>=0)&&(x<fWidth)&&(y<fHeight))
         {
           v << ccdM[x+y*fWidth];
		   idx << x+y*fWidth;
		 }
	 }
   }
   int N = v.count();
   matrix<double> ftm = FT(v);
   for(int i=4*boxWidth;i<N;i++) 
   {
     ftm(i,0) = 0;
	 ftm(i,1) = 0;
   }
   matrix<double> iftm = IFT(ftm);
   for(int i=0;i<N;i++) 
   {
     ccdM[idx[i]] = iftm(i,0);
   }
};

void SubtractFrame(double *ccdM1, double *ccdM2, quint16 fWidth, quint16 fHeight)
{
 quint32 fsize = fWidth*fHeight;
 double minP = 0;
 for(int i=0;i<fsize;i++) 
   {
     ccdM1[i] = ccdM1[i]-ccdM2[i];
	 if(ccdM1[i]<minP)minP = ccdM1[i];
   }
 for(int i=0;i<fsize;i++) ccdM1[i] = ccdM1[i]-minP; 
};

void sub_grad(double *ccdM, quint16 Xc, quint16 Yc, quint16 boxWidth, quint16 boxHeight, quint16 fWidth, quint16 fHeight)
{
   int x,y,cx;
   int bW = boxWidth/2;
   int bH = boxHeight/2;
   double v,v1,v2;
   if ((Yc-bH>0)&&(Xc-bW>0)&&(Yc+bH<=fHeight)&&(Xc-bW<=fWidth))
   {
	   for (y = Yc-bH;y<Yc+bH;y++)
	   {
		 v1 = ccdM[y*fWidth+Xc-bW];
		 v2 = ccdM[y*fWidth+Xc+bW-1];
	     for (x = Xc-bW;x<Xc+bW;x++)
		 {
		   v = v1+(v2-v1)*((double)(x-(Xc-bW)))/boxWidth;
		   ccdM[y*fWidth+x] = ccdM[y*fWidth+x] - v;
		   if (ccdM[y*fWidth+x]<0)ccdM[y*fWidth+x]=0;
		 }
	   }
   }
};

CCDDATA getRingArray(double *ccdM, int Xc, int Yc, int ro1, int ro2, int fWidth, int fHeight)
{
	double* pixdata = new double[4*ro2*ro2];
	int *xdata = new int[4*ro2*ro2];
	int *ydata = new int[4*ro2*ro2];
	double dist;
	int k = 0;
	for(int y = Yc-ro2; y<=Yc+ro2;y++)
		for(int x = Xc-ro2; x<=Xc+ro2;x++)
			if((x>=0)&&(y>=0)&&(x<fWidth)&&(y<fHeight))
			{
				dist = sqrt(pow(x-Xc,2)+pow(y-Yc,2));
				if((dist>ro1)&&(dist<ro2)) {pixdata[k] = ccdM[x+y*fWidth];xdata[k]=x;ydata[k]=y;k++;}
			}
	CCDDATA RES;
	RES.Buffer = new PIX[k*sizeof(PIX)];
	RES.maxADC = pixdata[0];
	RES.minADC = pixdata[0];
	RES.meanADC = 0;
	RES.Xmax = xdata[0];
	RES.Ymax = ydata[0];
	RES.X = Xc;
	RES.Y = Yc;
	for(int i=0;i<k;i++)
	{
		RES.Buffer[i].X = xdata[i];
		RES.Buffer[i].Y = ydata[i];
		RES.Buffer[i].I = pixdata[i];
		if(pixdata[i]>RES.maxADC){RES.maxADC = pixdata[i];RES.Xmax = xdata[i];RES.Ymax = ydata[i];}
		if(pixdata[i]<RES.minADC)RES.minADC = pixdata[i];
		RES.meanADC = RES.meanADC + pixdata[i];
	}
	RES.meanADC = RES.meanADC/k;
	RES.seADC = 0; 
	for(int i=0;i<k;i++) RES.seADC = RES.seADC + pow(pixdata[i]-RES.meanADC,2);
	RES.seADC = sqrt(RES.seADC/(k-1));
	RES.Size = k;
	RES.boxW = 2*ro2;
	RES.boxH = 2*ro2;
	delete pixdata;
	delete xdata;
	delete ydata;
	return RES;
};

vector<double> StartConditionsGrad(CCDDATA data, double Jo)
{
	vector<double> RES(4);
	int Xmin, Ymin;
	double minadc = data.Buffer[0].I;
	for(int i=0;i<data.Size;i++) if(data.Buffer[i].I<minadc)
									{minadc = data.Buffer[i].I;
									Xmin = data.Buffer[i].X;
									Ymin = data.Buffer[i].Y;}
	double maxmind = sqrt(pow(data.Xmax - Xmin,2)+pow(data.Ymax - Ymin,2));
	double Jmax = maxmind/(1/(minadc-Jo)-1/(data.maxADC-Jo));
	//
	double r = 0;
	for(int i=0;i<data.Size;i++) r = r+1+Jmax/(data.Buffer[i].I-Jo);
	r = r/data.Size;
	double Xo = data.X + r*(data.Xmax - Xmin)/maxmind;
	double Yo = data.Y + r*(data.Ymax - Ymin)/maxmind;
	RES(0) = Jo;
	RES(1) = Jmax;
	RES(2) = Xo;
	RES(3) = Yo;
	return RES;
};

vector<double> planefit(CCDDATA data)
{
	vector<double> J(data.Size);
	vector<double> Weights(data.Size);
	matrix<double> C(data.Size,3);
	for(int i=0;i<data.Size;i++)
	{
		J(i) = data.Buffer[i].I;
		Weights(i)=1;
		C(i,0) = data.Buffer[i].X;
		C(i,1) = data.Buffer[i].Y;
		C(i,2) = 1;
	}
	return LSM(J, C, Weights);
};

vector<double> fit2(CCDDATA data)
{
	vector<double> J(data.Size);
	vector<double> Weights(data.Size);
	matrix<double> C(data.Size,6);
	for(int i=0;i<data.Size;i++)
	{
		J(i) = data.Buffer[i].I;
		Weights(i)=1;
		C(i,0) = pow(data.Buffer[i].X,2);
		C(i,1) = pow(data.Buffer[i].Y,2);
		C(i,2) = data.Buffer[i].X*data.Buffer[i].Y;
		C(i,3) = data.Buffer[i].X;
		C(i,4) = data.Buffer[i].Y;
		C(i,5) = 1;
	}
	return LSM(J, C, Weights);
};

double freqpix(double *fdata, quint32 nelements)
{
	double maxv = fdata[0];
	double minv = fdata[0];
	for(quint32 k=0;k<nelements;k++)
	{
		if(fdata[k]>maxv)maxv=fdata[k];
		if(fdata[k]<minv)minv=fdata[k];
	}
	int hsize = (int)maxv-(int)minv+1;
	quint32 *pf = new quint32[hsize];
	for(int i=0;i<hsize;i++) pf[i]=0;
	for(quint32 k=0;k<nelements;k++) pf[(int)fdata[k]-(int)minv]++;
	int fpn=0;
	for(int i=0;i<hsize;i++) if(pf[i]>pf[fpn])fpn=i;
	fpn = fpn + minv;
	return (double) fpn;
};


void subgradfit2(double *ccdM, int Xc, int Yc, int ro1, int ro2, int fWidth, int fHeight, vector<double> PG, double Jo)
{
	int Xmin = Xc - ro2;
	int Xmax = Xc + ro2;
	int Ymin = Yc - ro2;
	int Ymax = Yc + ro2;
	for(int Y = Ymin;Y<=Ymax;Y++)
		for(int X = Xmin;X<=Xmax;X++)
			if((X>=0)&&(Y>=0)&&(X<fWidth)&&(Y<fHeight))
			{
				if(sqrt(pow(X-Xc,2)+pow(Y-Yc,2))<ro2) 
					ccdM[X+Y*fWidth] = Jo+ccdM[X+Y*fWidth] - 
							(PG(0)*pow(X,2)+PG(1)*pow(Y,2)+PG(2)*X*Y+PG(3)*X+PG(4)*Y+PG(5));
			}
};
*/
