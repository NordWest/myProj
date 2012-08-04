#include <QtGui>
#include <QtCore>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_inverse.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
 
 #include <boost/numeric/ublas/matrix_proxy.hpp>
 #include <boost/numeric/ublas/io.hpp>
 #include <boost/numeric/ublas/matrix_expression.hpp>
 
 #include <boost/numeric/ublas/triangular.hpp>
 #include <boost/numeric/ublas/lu.hpp>
  
 #include <iostream>
 #include <fstream>
 #include <vector>

using namespace boost::numeric::ublas;


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

typedef struct __STDATA__
{
     double mean;     
     double st_err;       
}STDATA,*PSTDATA;

//measuring
    double PointData(double *ccdM, quint16 Xc, quint16 Yc, quint16 fWidth);
	double MEFP(matrix<double> M, quint16 Xc, quint16 Yc);
	CCDDATA ApertureData(double *ccdM, quint16 Xc, quint16 Yc, quint16 ro, quint16 fWidth, quint16 fHeight);
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
	double JacobianL(int j, double X, double Y, vector<double> P);
	double JacobianG(int j, double X, double Y, vector<double> P);
	double JacobianM(int j, double X, double Y, vector<double> P);
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