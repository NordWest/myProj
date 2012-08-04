//#include <QtGui>
#include <QtCore>
/*
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_inverse.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
 
 #include <boost/numeric/ublas/matrix_proxy.hpp>
 #include <boost/numeric/ublas/io.hpp>
 #include <boost/numeric/ublas/matrix_expression.hpp>
 
 #include <boost/numeric/ublas/triangular.hpp>
 #include <boost/numeric/ublas/lu.hpp>
 */
#include <iostream>
#include <fstream>
#include "math.h"
//#include "astro.h"
//#include "comfunc.h"

//using namespace boost::numeric::ublas;

#ifndef CCDMES_H

typedef struct __PIX__
{
     quint16 X;       //X
     quint16 Y;       //Y
     double I;       //I
}PIX,*PPIX;

typedef struct __STIMG__
{
     double X;       //X
     double Y;       //Y
     double M;       //M
}STIMG,*PSTIMG;

int stDif(STIMG *st0, STIMG *st1);

typedef struct __CCDDATA__
{
     PPIX Buffer;       //pointer to Buffer data
     quint16 Size;      //size of PPIX buffer
	 quint16 boxW;
	 quint16 boxH;
	 quint16 X;
	 quint16 Y;
	 double maxADC;
	 double minADC;
	 double meanADC;
	 double seADC;
	 quint16 Xmax;
	 quint16 Ymax;
}CCDDATA,*PCCDDATA;

class imgAperture
{
public:
     PPIX buffer;       //pointer to Buffer data
     quint16 Size;      //size of PPIX buffer
	 quint16 boxW;
	 quint16 boxH;
	 quint16 X;
	 quint16 Y;
//prop
	 double maxADC;
	 double minADC;
	 double meanADC;
	 double seADC;
	 quint16 Xmax;
	 quint16 Ymax;
	 quint16 Pmax;		//position of maximum point
//	 
	imgAperture();
	imgAperture(qint16 Size);
	~imgAperture();
	
	void copyTo(imgAperture *target);
	void detProp();
	void remPoint(qint16 pos);
//	void remPoint(int x, int y);
//////////////////////////
        void rec2str(char *str);
        void str2rec(char *str);


        imgAperture& operator=(const imgAperture &source);
};


typedef struct __STDATA__
{
     double mean;     
     double st_err;       
}STDATA,*PSTDATA;

//measuring
    double PointData(double *ccdM, quint16 Xc, quint16 Yc, quint16 fWidth);
//	double MEFP(matrix<double> M, quint16 Xc, quint16 Yc);
	CCDDATA ApertureData(double *ccdM, quint16 Xc, quint16 Yc, quint16 ro, quint16 fWidth, quint16 fHeight);
	void ApeData(imgAperture *apeData, double *ccdM, quint16 Xc, quint16 Yc, quint16 ro, quint16 fWidth, quint16 fHeight);
	STIMG CenterOfMas(imgAperture *data);

double StImgWide(imgAperture *data, STIMG cofm, double delta);
void StartConditionsL(imgAperture *data, double delta, double *P);
void StartConditionsG(imgAperture *data, double *P);
void StartConditionsM(imgAperture *data, double delta, double *P);
STDATA StDataFromVector(double *v, int size);

int marksIdentify(double *m0, int mSize0, double *m1, int mSize1, int pNum); 
void sortM2M(double *vect, int size, int dim0, int elem, int direct);
double vectAng(double *v0, double *v1);



//void NLSLevenbergMarquardt(imgAperture *data,  double *P, int pSize, int psf_type, int *act_varint, int N, int NN, int GG, double PSt, double UWESt);
/*
	CCDDATA BoxData(double *ccdM, quint16 Xc, quint16 Yc, quint16 boxWidth, quint16 boxHeight, quint16 fWidth, quint16 fHeight);
	CCDDATA ProfileData(double *ccdM, quint16 Xc, quint16 Yc, quint16 profileWidth, quint16 fWidth);
	STIMG StarCentering(double *ccdM, quint16 Xc, quint16 Yc, quint16 ro, double cm_max_shift, quint16 fWidth, quint16 fHeight);
	STIMG getStarCenter(double *ccdM, quint16 Xc, quint16 Yc, quint16 ro, double cm_max_shift, quint16 fWidth, quint16 fHeight);
	QVector<int> sortAperture(CCDDATA data, int N);
	bool isObject(CCDDATA data, quint16 frpix);
	bool isExist(QVector<int> int_v, int k);
	STIMG CenterOfMas(CCDDATA data);
	double StImgWide(CCDDATA data, STIMG cofm, double delta);
	vector<double> StartConditionsL(CCDDATA data, double delta);
	vector<double> StartConditionsG(CCDDATA data);
	vector<double> StartConditionsM(CCDDATA data, double delta);
	QVector<double> NLSLevenbergMarquardt(CCDDATA data,  vector<double> P, int psf_type, int *act_varint, int N, int NN, int GG, double PSt, double UWESt);
	double Lorentzian(quint16 x, quint16 y, QVector<double> *LorentzianV);
	double Lorentzian_(quint16 x, quint16 y, vector<double> parameters);
	double Gaussian(quint16 x, quint16 y, QVector<double> *GaussianV);
	double Gaussian_(quint16 x, quint16 y, vector<double> parameters);
	double Moffat(quint16 x, quint16 y, QVector<double> *MoffatV);
	double Moffat_(quint16 x, quint16 y, vector<double> parameters);
	double Grad_(quint16 x, quint16 y, vector<double> parameters);
	double DMoffat(double x, double y, QVector<double> *MoffatV);
	double JacobianL(int j, double X, double Y, vector<double> P);
	double JacobianG(int j, double X, double Y, vector<double> P);
	double JacobianM(int j, double X, double Y, vector<double> P);
	double JacobianGrad(int j, double X, double Y, vector<double> P);
	void histFilter(double *ccdM, quint16 fWidth, quint16 fHeight, int size);
	void medianFilter(double *ccdM, quint16 fWidth, quint16 fHeight, int size, QProgressBar *pb, int use_pb);
	////////////////////////////////////////////////////////////
	void subtractPSF(QVector<double> *psfP, int psfType, double *ccdM, quint16 Xc, quint16 Yc, quint16 ro, quint16 fWidth, quint16 fHeight);
	/////////////////////////////////////////////////////////////
	vector<double> LSM(vector<double> L, matrix<double> C, vector<double> weights);
	matrix<double> LSM_err(vector<double> L, matrix<double> C, vector<double> weights);
	matrix<double> LSM_corr(vector<double> L, matrix<double> C, vector<double> weights, double max_res);
	/////////////////////////////////////////////////////////////
	matrix<double> fromDataToMatrix(double *data, int sz);
	STDATA StDataFromVector(QVector<double> v);
	double MaxOfVector(QVector<double> v);
	double MinOfVector(QVector<double> v);
	/////////////////////////////////////////////////////////////
	void MtrxToFile(QString FileName, matrix<double> OUT);
	void VectorToFile(QString FileName, vector<double> OUT);
    ////////////////////////////////////////////
	int getNearest(QVector<QPoint> ol, double x, double y, double rho);
	void test();
	double median(QVector<double> *data);
	matrix<double> FT(QVector<double> v); 
	matrix<double> IFT(matrix<double> ftm);
	void clearBoxNoise(double *ccdM, quint16 Xc, quint16 Yc, quint16 boxWidth, quint16 boxHeight, quint16 fWidth, quint16 fHeight);
	void SubtractFrame(double *ccdM1, double *ccdM2, quint16 fWidth, quint16 fHeight);
	void sub_grad(double *ccdM, quint16 Xc, quint16 Yc, quint16 boxWidth, quint16 boxHeight, quint16 fWidth, quint16 fHeight);
	CCDDATA getRingArray(double *ccdM, int Xc, int Yc, int ro1, int ro2, int fWidth, int fHeight);
	vector<double> StartConditionsGrad(CCDDATA data, double Jo);
	vector<double> planefit(CCDDATA data);
	vector<double> fit2(CCDDATA data);
	double freqpix(double *fdata, quint32 nelements);
	void subgradfit2(double *ccdM, int Xc, int Yc, int ro1, int ro2, int fWidth, int fHeight, vector<double> PG, double Jo);
*/

#define CCDMES_H
#endif
