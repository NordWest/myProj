#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QtCore>
#include <QtNetwork>
#include <QMainWindow>
#include "ccdmeasurements.h"
#include "ccdthread.h"
//#include "ccdframe.h"




class MainWindow : public QMainWindow
{
 Q_OBJECT
 
signals:
void signalTest();
void signalLoadFITS(QString);
void signalBC(double, double, double, int);
void signalUpdateImg();
void signalCenterOfMass(int, int, int);
void signalApproxImg(int, int, int, double, int);
void signalSubPsf(int, int, int, int);
void signalMeasureMarkedObjects(int, double, int, int, int);
void signalLeftFrame();
void signalRightFrame();
void signalDeleteFrame();
void signalAddMFrame();
void signalDelMFrame(int);
void signalComboFramesChanged(int);
//void signalStartProc1(int);
void signalExecute(QString);
void signalGetMarks(QString);
void signalUpdateOC(double,double);

public:
    //double tempConstant;
    MainWindow();
    void resizeEvent(QResizeEvent *event);
    void setupWidgets();
    void setupMainMenu();
    bool eventFilter(QObject *obj, QEvent *event);
	void DrawImageInScaledLabel(int x_p, int y_p, int wide);
    //events holders
    void imageLabelMouseMove(QMouseEvent *mouseEvent);
    void scaledLabelMouseMove(QMouseEvent *mouseEvent);
	void imageMeasureClick(QMouseEvent *mouseEvent);
	void imageNavigation(QKeyEvent *keyEvent);
	//emit signals
	void emitTest();
	void emitLoadFITS(QString FileName);
	void emitBC(double b, double c, double g, int inv);
    void emitUpdateImg();
	void emitCenterOfMass(int x, int y, int r);
	void emitApproxImg(int x, int y, int r, double delta, int psft);
	void emitMeasureMarkedObjects(int r, double delta, int psft, int centr, int vstar);
	void updateHist();
    
private slots:
        void open_file();
        void magnify();
		void marking();
		void psfPanel();
		//void dockMeasureWidgetShow();
        void scale_state(int state);
        void magnify_state(int state);
        void slotContrast(int value);
        void slotBrightness(int value);
		void slotSelectState(int state);
		//void slotMeasureButtonClicked();
		void slotThreadDone();
		void slotGamma(int g);
		void slotShowImg(int state);
		void slotUpdateImg();
		void slotUpdateAperture(int value);
		void slotUpdateProfile(int);
		//void slotRestoreButtonClicked();
		void slotCofmButtonClicked();
		void slotBoxButtonClicked();
		void slotCenterButtonClicked();
		void slotPsfButtonClicked();
		void slotMeasured();
		void slotClearButtonClicked();
		void slotAbortButtonClicked();
		void slotProgressAct();
		void slotMeasureMarkedButtonClicked();
		void slotSaveResults();
		void slotPSF();
		void slotDone();
		void slotKeyAct();
		void slotResAct();
		void slotFramesAct();
		void slotAddFrameButtonClicked();
		void slotMFrameAdded();
		void slotDelFrameButtonClicked();
		void slotComboFramesChanged(int index);
		void slotSetFrameButtonClicked();
		void slotStartCalcButtonClicked();
		void slotProcCompleted();
		void slotInvChanged(int inv_v);
		void slotSubButtonClicked();
		void slotTelnetAct();
		void slotPropAct();
		void slotMfcButtonClicked();
		void slotTangPosCalculated();
		void slotMoveCheckBoxStateChanged(int state);
		void slotSaveAsJPG();
		void slotShowHist();
		void slotCurrDirButtonClicked();
		void slotUcacDirButtonClicked();
		void slotUpdateHistButtonClicked();
		void slotLeftButtonClicked();
		void slotRightButtonClicked();
		void slotAngleChanged(double ang);
		void slotInfoButtonClicked();
		void slotTelescopeChanged(int index);
		void slotLogDirButtonClicked();
		void slotSendButtonClicked();
		void slotConnectButtonClicked();
		void slotDisconnectButtonClicked();
		void slotEphButtonClicked();
		void slotConnected();
		void slotDisconnected();
		void progressDataFromHttp();
		void slotClearObjButtonClicked();
		void slotToClipboardButtonClicked();
		void slotFromClipboardButtonClicked();
		void slotProgressDialogShow();
		void slotProgressDialogHide();
        bool close();
        
        
    
private:
       //BEGIN Widgets
       QScrollArea *scrollArea;
            QLabel *imageLabel;
       
       
       QDockWidget *dockScWidget;
        QWidget *magnifyWidget;
          QBoxLayout *boxScLayout;
            QLabel *scaledLabel;
            QSpinBox *scspinBox;
			QSpinBox *aperture_spinBox;
            QWidget *gridWidget;
             QGridLayout *grid;
               QCheckBox *scaleCheckBox;
               QCheckBox *magnifyCheckBox;
			   QCheckBox *apertureCheckBox;
			   //QCheckBox *fullrangeCheckBox;
			   QCheckBox *profileCheckBox;
			   QCheckBox *psfCheckBox;
			   QCheckBox *invCheckBox;
               QSlider *contrastSlider;
               QSlider *brightnessSlider;
               QLabel *labelBackground;
               QLabel *labelDiapason;
			   QSlider *gammaSlider;
			   QStatusBar *statusSW;
			   //QPushButton *restoreButton;
			   QPushButton *cofmButton;
			   QPushButton *boxButton;
			   QPushButton *centerButton;
			   QPushButton *psfButton;
			   QPushButton *subButton;
			   QPushButton *leftButton;
			   QPushButton *rightButton;
			   QPushButton *infoButton;
			   
	    QDockWidget *dockMarkWidget;
		       QCheckBox *centeringCheckBox;
			   QCheckBox *checkObjCheckBox;
			   QCheckBox *recompCheckBox;
			   QCheckBox *moveCheckBox;
			   QCheckBox *setOcCheckBox;
			   QPushButton *clearButton;
			   QLineEdit *radeLineEdit;
			   QPushButton *mfcButton;
			   QPushButton *ephButton;
			   //
			   QRadioButton *rbMoveAll;
			   QRadioButton *rbMoveSel;
			   QSpinBox *sbAstNum;
			   QListWidget *lwObjList;
			   QPushButton *clearObjButton;
			   QPushButton *toClipboardButton;
			   QPushButton *fromClipboardButton;
			   QLineEdit *mpcObjLineEdit;
			   
		QDockWidget *dockPsfWidget;
		       QRadioButton *lorentzRadioButton;
			   QRadioButton *gaussianRadioButton;
			   QRadioButton *moffatRadioButton;
			   QDoubleSpinBox *orderSpinBox;
			   QSpinBox *inSpinBox;
			   QSpinBox *regSpinBox;
			   QSpinBox *rpsSpinBox;
			   QSpinBox *resSpinBox;
			   QPushButton *measureMarkedButton;
			   QCheckBox *addCheckBox;
			   QCheckBox *cropCheckBox;
			   
	    QDockWidget *dockFramesWidget;
		      QComboBox *comboFrames;
			  QPushButton *addFrameButton;
			  QPushButton *delFrameButton;
			  QPushButton *setFrameButton;
			  QPushButton *startCalcButton;
			  QDoubleSpinBox *maxresSpinBox;
			  QLabel *refFrameLabel;
			  QLineEdit *comLineEdit;
			   
		QDialog *progressDialog;
		       QProgressBar *prBar;
			   QPushButton *abortButton;
			   
		QDockWidget *dockKeyWidget;	   
		    QWidget *keyWidget;
			QScrollArea *keyScrollArea;
			QLabel *keyLabel;

        QDockWidget *dockTelnetWidget;
		         QLineEdit *telnetHost;
				 QSpinBox *telnetPort;
				 QLineEdit *telnetRequest;
				 QPushButton *sendButton;
				 QPushButton *connectButton;
				 QPushButton *disconnectButton;
				  QTcpSocket *horizon;
		
		QDockWidget *dockPropWidget;
		    QComboBox *comboTel;
		    QDoubleSpinBox *flSpinBox;
			QDoubleSpinBox *xpsSpinBox;
			QDoubleSpinBox *ypsSpinBox;
			QDoubleSpinBox *fovSpinBox;
			QPushButton *currDirButton;
			QLabel *currDirLabel;
			QCheckBox *updateHistCheckBox;
			QLabel *ucacDirLabel;
			QPushButton *ucacDirButton;
			QLabel *logDirLabel;
			QPushButton *logDirButton;
			QDoubleSpinBox *rotAngSpinBox;
			QCheckBox *useProxy;
			QLineEdit *proxyHost;
			QSpinBox *proxyPort;
			QLineEdit *obsCode;
			
		QDockWidget *dockHist;
			QLabel *histLabel;
			QPushButton *updateHistButton;
           	//QSlider *sldrLeft;
            //QSlider *sldrRight;	
		 
       //END Widgets
       double scfactor;//
       
       
       
       //BEGIN menu construction
       QMenu      *fileMenu;
       
          QAction    *openAct;
		  QAction    *saveResultsAct;
		  QAction    *saveAsJPG;
          QAction    *exitAct;
       
       QMenu      *toolsMenu;
       
          QAction    *magnifyAct;
		  QAction    *markingAct;
		  QAction    *psfAct;
		  QAction    *framesAct;
		  QAction    *telnetAct;
		  QAction    *propAct;
		  
	   QMenu      *viewMenu;
       
          QAction    *progressAct;
		  QAction    *keyAct;
		  QAction    *resAct;
		  QAction    *histAct;
          
       //END menu construction
public:
       int actTel;
       ccdthread *ccdThread;
       QImage *img;
       int mainX, mainY;
	   int currX, currY;
	   int imgWidth, imgHeight;
	   int startX,startY;
	   int deltaX,deltaY;//displacement for marks motion
	   double rot_angle;//angle for marks motion
	   QVector<QRgb> colormap;
       IMGF imgF;
	   QVector<double> vRes;
	   //
	   QSettings *settings;
	   int img_status;
	   QVector<QPoint> objl;//markers of the reference stars
	   QVector<QPoint> m_objl;//markers of the control stars and object for determinations of celestial position
	   QVector<int> refToStar;//references to reference stars matrix
	   int fX, fY, fL, fN;//focused mark, position, list, and number within the list
	   //matrix<double> marks1;
	   //matrix<double> marks2;
	   //double tau1, tau2;
	   QString refFrameName;
	   QString currDir;
	   //
	   int histX1, histX2;// histogramm marks
	   bool firstClick;//what mark has been clicked?
	   
	   QHttp *http;
       QStringList detObjList;      
};

#endif
