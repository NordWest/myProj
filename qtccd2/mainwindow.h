#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QtCore>
#include <QMainWindow>
#include "ccdframe.h"
#include "s2c.h"
#include "serialWidget.h"
#include "begpropdlg.h"
#include "autosetuptimerdlg.h"
#include "wakeupdlg.h"

//BEGIN uniblitz shutter defenitions
//typedef int (*SOFTCONTROLDLL)();
//typedef int (*HARDCONTROLDLL)();
//typedef int (*SETONOFFDLL)(unsigned char On_nOff);
//typedef int (*INITPORTDLL)(char* name);

//END uniblitz shutter defenitions
typedef void (*oupfuncPtr)(short portaddr, short datum);

class MainWindow : public QMainWindow
{
 Q_OBJECT
 
protected:
     void closeEvent(QCloseEvent *event);

public:
    double tempConstant;
    MainWindow();
    void resizeEvent(QResizeEvent *event);
    void setupWidgets();
    void setupMainMenu();
    bool eventFilter(QObject *obj, QEvent *event);
    //events holders
    void imageLabelMouseMove(QMouseEvent *mouseEvent);
    void scaledLabelMouseMove(QMouseEvent *mouseEvent);
	//======================
	//BEGIN uniblitz shutter function
	//SOFTCONTROLDLL SoftControlDll;
	//HARDCONTROLDLL HardControlDll;
	//SETONOFFDLL SetOnOffDll;
	//INITPORTDLL InitPortDll;
	//END uniblitz shutter function
    oupfuncPtr oup32;
    
private slots:
        void open_file();
        void magnify();
        void placing();
        void cameraSetup();
		void exposureSetup();
        void scale_state(int state);
        void magnify_state(int state);
        void slotContrast(int value);
        void slotBrightness(int value);
        void update();
        bool close();
        void cameraButtonClicked();
		void applyButtonClicked();
        void MakeExposureButtonClicked();
		void stopExposureButtonClicked();
        void slotShowExposedFrame();
		void slotCamStarted();
		void slotCamStopped();
		void slotFinished();
        void camTurnOffClicked();
        void camSettingsChanged(int state);
		void camRSettingsChanged(bool state);
		void setCamSettings();
		void slotSaveFitsFile();
		void slotCurrDirButtonClicked();
		void slotPropSetup();
		void slotDarkFlatSetup();
		///
		void slotLoadDarkFileButton1Clicked();
		void slotLoadDarkFileButton2Clicked();
		//
		void slotLoadFlatFileButtonClicked();
		void getEaButtonClicked();
		void slotAdoptButtonClicked();
		//
		void slotCurrDarkDirButtonClicked();
		void slotCurrFlatDirButtonClicked();
		//================================
		void shutterOpenButtonClicked();
        void shutterCloseButtonClicked();
        void slotShutterSetup();
        void slotTrackerSetup();
		void updateHist(int pos);
		//============================
		void slotdfOkButtonClicked();

        void slotBegPropDlg();

        void slotUpdateTrackerState();

        void slotStartGuidProgram();
        void slotStopGuidProgram();
        void slotShowGuid();

        void slotLoadDefaultPanels();
        void slotSaveDefaultPanels();
        void slotSavePanels();
        void slotLoadPanels();

        void slotAutoGuidStarted();
        void slotAutoGuidStopped();
        void slotShowAutoGuid();

    
private:
       //BEGIN Widgets
       QScrollArea *scrollArea;
            QLabel *imageLabel;
       
       
       QDockWidget *dockScWidget;
        QWidget *magnifyWidget;
          QBoxLayout *boxScLayout;
            QLabel *scaledLabel;
			QLabel *histLabel;
            QSpinBox *scspinBox;
            QWidget *gridWidget;
             QGridLayout *grid;
               QCheckBox *scaleCheckBox;
               QCheckBox *magnifyCheckBox;
               QLabel *labelX;
               QLabel *labelY;
               QLabel *labelI;
               QSlider *contrastSlider;
               QSlider *brightnessSlider;
			   QSlider *gammaSlider;
               QLabel *labelBackground;
               QLabel *labelDiapason;
               
        QDockWidget *dockTimePosWidget;
            QWidget *TimePosWidget;
             QLabel *labelUTC;
			 QLineEdit *radeLineEdit;
			 QLineEdit *fnameLineEdit;
			 QLineEdit *corrLineEdit;
			 QLineEdit *targetLineEdit;
             QPushButton *getEaButton;
			 QPushButton *adoptButton;
                         QLabel *sTimeLabel;
			 QLabel *nAstrograph;
			 
        QDockWidget *dockCameraWidget;
            QWidget *cameraWidget;
              QGridLayout *gridCamera;
               QPushButton *camInitButton;
               QPushButton *applyButton;
               QPushButton *camTurnOff;
               /////////////////////////
               QGroupBox *coolingBox;
                QBoxLayout *coolingLayout;
                 QSpinBox *coolingSpBox;
                 QCheckBox *coolCheckBox;
                 QCheckBox *fanCheckBox;
                 QCheckBox *stabCheckBox;
                /////////////////////////  
               QGroupBox *frequencyBox;
                QBoxLayout *frequencyLayout;
                QRadioButton *highRadioButton;
                QRadioButton *lowRadioButton;
                ////////////////////////////
               QGroupBox *cleaningBox;
               QBoxLayout *cleaningLayout;
               QCheckBox *fastCheckBox;
               QCheckBox *slowCheckBox;
               //////////////////////////
                QSpinBox *frameX;
                QSpinBox *frameY;
                QSpinBox *frameXSize;
                QSpinBox *frameYSize;
                QSpinBox *frameBinX;
                QSpinBox *frameBinY;
                //////////////////////////
				QCheckBox *chbDigGain;
				QSpinBox *spbDigGain;
				QCheckBox *chbBL;
				QSpinBox *spbBL;
				
				
				
		QDockWidget *dockExposure;
                 QPushButton *MakeExposureButton;
                 QPushButton *stopExposureButton;
                 //QLineEdit *exposureTime;
				 QDoubleSpinBox *exposureTime;
                 QSpinBox *exposureDelay;
                 QSpinBox *exposureCount;
				 QCheckBox *chbSaveFits;
				 QCheckBox *chbUseShutter;
				 QSpinBox *spbOffset;
				 QSpinBox *spbShutterDelay;

        QDockWidget *dockProp;
                QLabel *currentDirLabel;
                QPushButton *currDirButton;
				QLabel *currentDarkDirLabel;
                QPushButton *currDarkDirButton;
				QLabel *currentFlatDirLabel;
                QPushButton *currFlatDirButton;
				//
				QDoubleSpinBox *dsbFocalLength;
				QSpinBox *sbAirTemperature;
				QLineEdit *observersLineEdit;
                                //QComboBox *observersBox;
				QLineEdit *filterLineEdit;
				
		QDockWidget *dockDarkFlat;
				QLabel *darkFNLabel1;
				QLabel *darkFNLabel2;
				QPushButton *loadDarkFileButton1;
				QPushButton *loadDarkFileButton2;
				QRadioButton *noDarkRadioButton;
                QRadioButton *imDarkRadioButton;
				QRadioButton *calcDarkRadioButton;
                //================			
				QLabel *flatFNLabel;
				QPushButton *loadFlatFileButton;
				QRadioButton *noFlatRadioButton;
                QRadioButton *imFlatRadioButton;
				QRadioButton *calcFlatRadioButton;
				QPushButton *dfOkButton;
				
		QDockWidget *dockShutter;
		        QPushButton *shutterOpenButton;
			    QPushButton *shutterCloseButton;
				
		        
            QDockWidget *dockTracker;
                serialWidget *trackerWidget;

            QDockWidget *dockGuid;
                QLabel *stateLabel;
                QPushButton *startButton;
                QPushButton *stopButton;
                QTextEdit *stateLog;
                //QSpinBox *sbAirTemperature;

            QDockWidget *dockAutoGuid;
                QLabel *stateLabel1;
                QDoubleSpinBox *timeDelay;
                QPushButton *startSetupButton;
                QCheckBox *autoStopTracker;
                QCheckBox *startWhenReady;
                //QLabel *setTimeRemain;
                //QLabel *curRa;
                //QLabel *curDe;


                int progStarted;
                int expStarted;

                int setOk;
				
       //END Widgets
       double scfactor;//
       
       QStringList userNames, obsNames;
       int curFilter;
       QStringList filterNames;
       
       
       //BEGIN menu construction
       QMenu      *fileMenu;
       
          QAction    *openAct;
		  QAction    *saveAct;
          QAction    *exitAct;

          QAction *loadPanelsAct;
          QAction *savePanelsAct;
       
       QMenu      *toolsMenu;
       
          QAction    *magnifyAct;
          QAction    *TimePosAct;
		  QAction    *propAct;
		  
		  
		  
	   QMenu   *cameraMenu;
			QAction    *cameraAct;
			QAction    *exposureAct;
			QAction    *shutterAct;
			QAction    *dfAct;
                        QAction    *trackerAct;
                        QAction    *guidAct;
                        QAction    *autoGuidAct;
       //END menu construction
       
       S2C *s2c;
       QPointer<CCDFrame> frame;
       QImage *img;
       int mainX, mainY;
	   int curX, curY;
       
       QTimer *timer;
       unsigned short Year, Month, Day, Hour, Min;
       double Sec;
       //
       short int T_ccd;
       short int T_hot;

       //
       uchar *CCDBuff;
       uchar *CCDBuffBack;
       unsigned *CCDBuffInt;

       int exp_counter;
       int frame_counter;
       bool exposure_started;
       QDateTime utc;
       QDateTime date_obs;
       QDateTime date_end;
       QTime exp_msek;
       QString currentDir, currentDarkDir, currentFlatDir;
       //
       unsigned short *dark1_buff;//dark1 data buffer
       unsigned short *dark2_buff;//dark2 data buffer
       unsigned short *dark_buff;//dark2 data buffer
       long dark_buff_size;
       double *flat_buff;
       double expt_dark1, expt_dark2;
       unsigned short leftB, rightB;
       int histX1, histX2;
       bool firstClick;
       int useEaExp;
       QSettings *ea;
       QSettings *settings;
       int nObj;
       QString nameObj;
       int mesEndExp;
       int typeObj;
       double tHA;

       int useWindowsUserNames;

       double raObj,deObj, DE, dDE, dHA, HA;
       int shdelay;
   int create_cdir;
       int lang;
       //
       QString time_elepsed;
       int numOfFrame;
       double exptimecorr;

       QString trackerCommand;
       int useTracker;
       int comPortNum;

       autoSetupTimerDlg *autoTimer;

       QFont bFont, sFont;
};

#endif
