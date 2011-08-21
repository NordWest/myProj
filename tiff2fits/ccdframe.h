#include <QtCore>
#include <QtGui>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
//#include "fitsio.h"

using namespace boost::numeric::ublas;

typedef struct __IMGF__
{
  double ADCmax;
  double ADCmin;
  double ADCmean;
  double lHist;
  double rHist;
  double maxHist;
  double fr_pix;
  double leftB;
  double rightB;
  double gamma;
  quint16 xSize;
  quint16 ySize;
  uchar* imgCCD;
  //quint16 *rdata;
  double *fCCD;
  quint32 *histOfImg;
  matrix<double> *rMtrx;
  matrix<double> *pr_ol;
  matrix<double> *pm_ol;
  matrix<double> *p_refcat;//pointer to matrix with reference stars
  matrix<double> *p_cel_pos;
  matrix<double> *p_tangpos;//pointer to tangential coordinates
  matrix<double> *p_resOUT;//pointer to result output
  int max_num_it;  
  int max_num_reg;
  int frIndex;
  double min_par_step;
  double min_uwe_step;
  bool abort;
  int num_of_frames;
  QString file_name;
  QString measured_file_name;
  QString keyStr;
  QString simple_message;
  QString ucac2_path;
  QString log_path;
  QString EphStr;//EPHEMERIS OF SOLAR SYSTEM BODIES 
  QString obs_code;
  QStringList objStrList;
  bool isOldNA;
  int refCCDFrame;
  double max_residual;
  int inv;
  double ra_oc;
  double de_oc;
  double fov;
  double flength;
  double xpsize;
  double ypsize;
  double curr_mjd;
  double mag_1;
  double mag_2;
}IMGF,*PIMGF;
  


class CCDFrame: public QObject
{
Q_OBJECT

signals:
void signalImageProcessed(int);
void signalMeasured();
void signalPSF();
void signalDone();
void signalMFrameAdded();
void signalProcCompleted();
void signalTangPosCalculated();
void signalProgressDialogShow();
void signalProgressDialogHide();

public slots:
        void testing();
		void slotLoadFITS(QString FName);
		void slotLoadFITS_ex(QString FName, QString FNameEx, int rate);
		void slotBC(double b, double c, double g, int inv);
		void slotCenterOfMass(int x, int y, int r);
		void slotApproxImg(int x, int y, int r, double delta, int psft);
		void slotMeasureMarkedObjects(int r, double delta, int psft, int centr, int vstar);
		void slotLeftFrame();
		void slotRightFrame();
		void slotDeleteFrame();
		void slotAddMFrame();
		void slotDelMFrame(int del_index);
		void slotSetMFrame(int index);
		void slotStartProc1(int index, int ang_un, double ang_sc);
		void slotSubtractPSF(int x, int y, int r, int psft);
		void slotExecute(QString comLine);
		void slotGetMarks(QString comLine);
		void slotUpdateOC(double ra, double de);
		
public:
    QString FileName;  //File name
	//fitsfile *fptr; //fitsfile pointer from FITSIO lib
	int status; // fitsfile status
	long *naxes; //array of sizes of each dimension
	int bpix;
	quint16 XSize, YSize; //Pixels sizes
	long nelements; //number of pixels in array
	long nelements_ex;
    uchar* CCDImage;//view buffer
    //quint16 *CCDdata;//ccd image data buffer
	matrix<double> mCCD;
	double *fCCDdata;
    double MaxADC, MinADC, MeanADC; //maximum, minimum and mean ADC
	quint32 *hist;//histogramm
	quint32 histsize;//size of histogramm
	double histL, histR, histMax;//left and right boudaries of hist to view
	double borderL, borderR;
	double *features;
	PIMGF pimgf;
	QVector<double> *mRES;//resulted vector for PSF
	matrix<double> r_ol;
    matrix<double> m_ol;
	matrix<double> refcat;//reference stars
	matrix<double> cel_pos;//celestial positions of measured objects from theory of motion
	matrix<double> tangpos;//tangential coordinates
	matrix<double> resOUT;//output of the results 
    double exp_time;
	double date_obs;
	QString date_obs_str;
	///////////////////////////////////////////////////////////
	QList<long*> pl_naxes;
	QList<uchar*> pl_CCDImage;
	//QList<quint16*> pl_CCDdata;
	QList<double*> pl_fCCDdata;
	QList<quint32*> pl_hist;
	QList<double*> pl_features;
	QStringList FNList;
	QStringList keyList;
	QVector<double> expList;
	QVector<double> mjdList;
	QVector<double> gammaList;
	QVector<int> invList;
	//
	QVector<double> raList;
	QVector<double> deList;
	//
	int pl_index;
	///////////////////////////////////////////////////////////
	QList<double*> roList;
	QList<double*> moList;
	QVector<int> ro_list_sizes;
	QVector<int> mo_list_sizes;
	///////////////////////////////////////////////////////////
	CCDFrame(PIMGF imgf, QVector<double> *ResVect, QProgressBar *pb);
	~CCDFrame();
	///////////////////////////////////////////////////////////
	void LoadFromFITS(QString FileName); //loading image and image format from FITS file
	void LoadFromFITS_ex(QString FName, QString FileNameEx, int rate);
	void DeleteFrame(int N);
	void SetFrame(int N);
	void setImgData(double minV, double maxV, double Gamma, int inverse);//setting 
	double getValue(quint16 x_pos, quint16 y_pos);
    void getRange();
	void getHist();
	void getHistPr();
	void getRangeIndx(int indexF);
	void getHistIndx(int indexF);
	void getHistPrIndx(int indexF);
	void setImgDataIndx(int indexF);
	//////////////////////
	void astrometric_reduction(int indexM, int indexF, double max_res, int rcat, int uw, int saveLog, QString mpcFName, int diff_sat, int signal_out);
	void hFilter(int indexF, int size);
	void mFilter(int indexF, int size, int use_pb);
	void saveDates();
	void setImgDataAsIndx(int index);
	void getEph();
	//emit signals
	void emitImageProcessed(int state);
	void emitMeasured();
	void emitSignalPSF();
	void emitSignalDone();
	
	//
	QProgressBar *progressBar;
    
};



