#include <QtCore>
#include <QtGui>
#include <QMainWindow>
#include <QtNetwork>

#include "imagelabel.h"
#include "newNameDlg.h"
#include "getfromheaderdlg.h"

#include "setupmarkscolor.h"

#include "./../libs/mb.h"
#include "./../libs/astro.h"
//#include "./../libs/astro.h"
#include "./../libs/comfunc.h"
#include "./../libs/observatory.h"
//#include "./../libs/multidim.h"

#include "gethttpdlg.h"
#include "findobjectdlg.h"
//#include "gridstatw.h"


struct frameRL
{
	bool is_checked;
	QString name;
	QString path;
};


class MainWindow : public QMainWindow//QMainWindow
{
 Q_OBJECT
 
signals:
//	signalNext();

    
private slots:
	//BEGIN menu slots
	//menu file
        void slotOpenFitsFile();
	void slotOpenEmpty();
        void slotSaveFitsFile();
        void slotSaveFitsFileAs();
        //
	void slotLoadHeader();
        void slotLoadHttpHeader();
        //
        void slotSavePanels();
        void slotLoadPanels();
        //
	void slotOpenScaledFItsFile();
	void slotFrameManagerAct();
	void slotExit();
	//menu view
	void slotIncrease();
	void slotDecrease();
	void slotBackLeft();
	void slotBackRight();
	void slotSignalLeft();
	void slotSignalRight();
	void slotSetDefault();
	void slotSetFullDiapason();
	//menu frame
	void slotNext();
	void slotPreviouse();
	void slotFirst();
	void slotLast();
	void slotDelete();

        void slotSetCurFrame(int i);
	
	//menu tools
	void slotMarks();
        void slotCenterMass(marksGrid *mgr);
	void slotPropAct();
	void slotMarksAct();
        void slotFindHstars();


	//END menu slots
	void slotScrUpdate();
	void slotAngleChanged(double ang);
	void slotTelescopeChanged(int index);
	void slotScxChanged(double scx);
	void slotScyChanged(double scy);
	void slotFovChanged(double fov);
	void slotApeChanged(double ape);
	void slotNewInsButtonClicked();
	void slotNewInsExit();
	void slotSaveButtonClicked();
	void slotRemInsButtonClicked();
	void clearMarks();
	void clearIpixMarks();
	void slotKeyAct();
	void slotConAct();
	void slotPsfAct();
	void slotMoveCheckBoxStateChanged(int state);

	void slotIdentify();
	void slotIdentifyAuto();
	
	void slotFrameListClicked(int r, int c);

	void slotKeyReleased(QKeyEvent *keyEvent);
	
	void slotLevelBinChanged(int lev);
	void slotAperBinChanged(int lev);
        int slotBinFilter();
	void slotFilters();
	void slotFitsHeaderPanel();
	void slotMarksUnify(int apSize);
        void slotMarksUnify1(int apSize);
	
	void slotSaveMarksButtonClicked();
	void slotLoadMarksButtonClicked();
        void slotMoveMarksToCorner();
	
	void slotFlipMarksH();
	void slotFlipMarksV();
	
	void slotCurrDirButtonClicked();
	void slotLogDirButtonClicked();
	void slotGetMJDfromHeader();
	void slotGetRaDefromHeader();
	void slotRaDeChanged(QString str);
	void slotTimeChanged(QString);
	
	void slotFromHeader();
        int slotFindStars();
        void slotGetObjects();

	void slotStatBarUpdate();
	
	void slotUpdateHistButtonClicked();
	void slotShowHist();

        void slotMeasureMarkedButtonClicked();
        void slotMeasureCatMarkedButtonClicked();
        void slotMeasureIPixMarkedButtonClicked();
        void slotMeasureMarksGrid(marksGrid *mgr);

        void slotWcsWidget();
        void slotWCSButtonClicked();
        void slotDropWCS();
        void slotWriteWCS();
        void slotLoadWCS();
        void slotSaveWCS();

        //http
        //void slotProcessingData(bool error);//РћРќРљРЎР’РҐР Р­ РҐ РќРђРџР®РђРќР Р®Р Р­ Р”Р®РњРњРЁР•;
        //void slotStateChanged(int state);//РЇРќРЇР РќРЄРњРҐР• Р“Р®РћРџРќРЇР®;
        //void slotRequestFinished(int id, bool error);//РЎР“РњР®Р Р­, Р’Р Рќ Р‘РЁРћРќРљРњР•РњРҐР• Р“Р®РћРџРќРЇР® Р“Р®Р‘Р•РџР¬Р•РњРќ;
        void slotSetHeaderPanel();
        void slotExpNumChanged(int eNum);

        void slotSaveDefaultPanels();
        void slotLoadDefaultPanels();

        void slotFindObjImg();

        void slotGridStatPanel();
        void slotUpdateGridStat();

        void slotDetRef();

        void clearCatMarks();

        void slotMakeReduction();
        void slotFindCloserStars();
        void slotFindCloserObjects();

        void slotSaveSettings();

        void slotObjectPanel();

        void slotCenterMassButton();
       // void slotDeleteCloserThan(int minRange);

        void slotCheckAllFiles();
        void slotUnCheckAllFiles();

        void slotCatalogChanged(int curIndex);
        void slotUpdateCurDir();

        void slotSetupMarksColor();
        void slotUpdateColors();


public:
	MainWindow();
	void setMenu();
	void setWidgets();
	void setSettings();
	void resizeEvent(QResizeEvent *event);
	bool eventFilter(QObject *obj, QEvent *event);
	void setSettName();
	void wheelEvent(QWheelEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void imageNavigation(QKeyEvent *keyEvent);
	void fillFrameList();
        int getEphemerides();
        int getNumMarks(double magn0, double magn1, int num);
        void setCurDirFile(QString fName);


	
	/////////////////////////
        insSettings *instruments;
	int mainFrame;		//0-fits, 1-marks, 2-workframe
	int rbMouse;		//0-unpressed, 1-presses;
	int lbMouse;		//0-unpressed, 1-presses;
	int ctrlB;		//0-unpressed, 1-presses;
	int xM, yM;
	double mag_1, mag_2;
	newNameDlg *nName;
	QTextStream *out;
        QSettings *settings;
        QSettings *stimproSett;
	QString currDir, logDir;
	QString errStr, posStr;
	int Xo, Yo;
        QString u2FindStr;
        catFinder *starCat;
        QList <catFinder*> starCatList;
        observatory *obsList;
        QString observatoryCat;
        QString obsName;
        QString plateHostName;//, proxyName;
//        int proxyPort, useProxy;
//        QString obsName;
        int isMove2corner;
        int currentCat;
        int identType;
        double sigmaN;
        int isUseEN;            //is useing exposure nums
        double magObj0, magObj1;
        int targNum;

        QColor ipixCol, catCol, refCol, objCol, bgCol;
        QColor ipixBtnCol, catBtnCol, refBtnCol, objBtnCol;
        QString ipixColStr, catColStr, refColStr, objColStr, bgColStr;
        //QIcon ipixBtnIcon, catBtnIcon, refBtnIcon, objBtnIcon;//(ipixIcon);

        double maxObjDisp;
        int isRedRef;
        int useTmu;
        QString tmuFile;
        int headType;

        reductionParams wcsParams;

        QString ast_eph_prog, ast_eph_prog_folder, get_http_prog, get_http_prog_folder;
        QString lspm_prog, lspm_prog_folder, lspm_cat_file;
        QString skybot_prog, skybot_prog_folder;

        QString telescopeFile;
        int instrNum;

        QHttp *http;
        int eventMessages;
        //QList <HeadRecord*> headList;
        QList <ExposureRec*> expList;
//        QList <double*> exposList;
	
	void updateHist();
        void updateWCS(double *wcsV = NULL);

        void initInstr();

        QPixmap *ipixPm, *catPm, *refPm, *objPm;
        QIcon ipixIcon, catIcon, refIcon, objIcon;
        QIcon ipixBtnIcon, catBtnIcon, refBtnIcon, objBtnIcon;//(ipixIcon);
        QPixmap *ipixPmB, *catPmB, *refPmB, *objPmB;
private:
	//BEGIN menu
	//file
	QMenu *fileMenu;
	QAction *openFitsAct;//???????? fits ?????
        QAction *saveFitsAct;
        QAction *saveFitsAsAct;
	QAction *openEmptyAct;//
	QAction *loadHeaderAct;//
        QAction *loadHttpHeaderAct;//
	QAction *openScaledFitsAct;
        QAction *loadPanelsAct;
        QAction *savePanelsAct;
	QAction *viewFMAct;
	QAction *exitAct;//????? ?? ????????? (???????? ??????????)
	//view
	QMenu *viewMenu;
	QAction *incAct;//?????????? ???????????
	QAction *decAct;//?????????? ???????????
	/////////////////// ????????? 6 "????????" (action) ????????? ??????????? "???? ??????????"
	QAction *backLeftAct;//1. ???????? ????? ??????? "???? ??????????" ?????
	QAction *backRightAct;//2. ???????? ????? ??????? "???? ??????????" ??????
	QAction *signalLeftAct;//3. ???????? ?????? ??????? "???? ??????????" ?????
	QAction *signalRightAct;//4. ???????? ?????? ??????? "???? ??????????" ??????
	QAction *setDefaultAct;//5. ??????????? ? ?????????? ??-?????????, ??????????? ??? ???????? fits ?????
	QAction *setFullDiapasonAct;//6. ????????? "???? ??????????" ?????? ????? ????????? ???????? ?? ????????
	QAction    *histAct;
	//frame
	QMenu *frameMenu;
	QAction *nextAct;//??????? ? ?????????? ?????
	QAction *prevAct;//??????? ? ??????????? ?????
	QAction *delAct;//???????? ???????? ?????
	QAction *propAct;
//	QAction *marksAct;
	//tools menu
	QMenu *toolMenu;
	QAction *marksAct;
	QAction *cMassAct;
        QAction *getMJDAct;
	QAction *getRadecAct;
        QAction *findHstarsAct;
        //panels menu
        QMenu *panelsMenu;
        QAction *keyAct;
        QAction *conAct;
        QAction *marksPanelAct;
        QAction *psfAct;
        QAction *filtersAct;
        QAction *fitsHeaderAct;
        QAction *wcsAct;
        QAction *objAct;
        QAction *gStatAct;
        QAction *setupColorAct;

	//END menu 
	//BEGIN widgets
	QScrollBar *hscrBar;//?????????????? ?????? ?????????
	QScrollBar *vscrBar;//???????????? ?????? ?????????
	imageLabel *imgLabel;//?????? ??? ?????????? ??????????? "?? ??????" 
	QWidget *cWidget;//??????????? ??????, ??????? ???????? "???????????" ??? ????? ????????? ? ???????, ???????????????? ??? ?????????? ???????????

	QDockWidget *dockPropWidget;
		    QComboBox *comboTel;
//		    QDoubleSpinBox *flSpinBox;
			QPushButton *newInsButton;
			QPushButton *saveSettButton;
			QPushButton *remInsButton;
			QDoubleSpinBox *xpsSpinBox;
			QDoubleSpinBox *ypsSpinBox;
			QDoubleSpinBox *fovSpinBox;
			QDoubleSpinBox *apeSpinBox;
			QPushButton *currDirButton;
			QLabel *currDirLabel;
			QCheckBox *updateHistCheckBox;
			QLabel *ucacDirLabel;
			QPushButton *ucacDirButton;
			QLabel *usnobDirLabel;
			QPushButton *usnobDirButton;
			QLabel *logDirLabel;
			QPushButton *logDirButton;
			QDoubleSpinBox *rotAngSpinBox;

                        QCheckBox *useProxy;
			QLineEdit *proxyHost;
                        //QLineEdit *wcsLineEdit;
			QSpinBox *proxyPort;
                        QLineEdit *obsCode;
			///////////////////////////
                        //QRadioButton *rbCatUCAC2;
                        //QRadioButton *rbCatUSNOB;
                        QComboBox *catSelect;
			
			
	QDockWidget *dockMarkWidget;
		       QCheckBox *centeringCheckBox;
			   QCheckBox *checkObjCheckBox;
			   QCheckBox *recompCheckBox;
			   QCheckBox *moveCheckBox;
			   QCheckBox *setOcCheckBox;
			   QCheckBox *saveMarksPos;

			   QPushButton *clearIpixButton;
			   
			   QLineEdit *radeLineEdit;
			   QPushButton *getRadeButton;
			   
			   QLineEdit *timeLineEdit;
			   QPushButton *getMjdButton;
			   QLineEdit *expLineEdit;
			   

			   QPushButton *ephButton;
                           QPushButton *mfcButton;
                           QPushButton *clearCatMarksButton;
                           QPushButton *measureCatMarksButton;

                           QComboBox *activMarksGrid;
                           QPushButton *clearButton;
                           QPushButton *measureMarksButton;
			   //
			   QPushButton *saveMarksButton;
			   QPushButton *loadMarksButton;
			   //
			   QPushButton *flipMarksHButton;
			   QPushButton *flipMarksVButton;


                           //QPushButton *flipMarksVButton;

			   QRadioButton *rbMoveAll;
			   QRadioButton *rbMoveSel;
			   QSpinBox *sbAstNum;
			   QListWidget *lwObjList;
			   QPushButton *clearObjButton;
			   QPushButton *toClipboardButton;
			   QPushButton *fromClipboardButton;
			   QLineEdit *mpcObjLineEdit;
			   QPushButton *cMassButton;
			   QPushButton *identifyButton;
			   QPushButton *identifyAutoButton;

                           QPushButton *makeReductionButton;
                           QPushButton *findCloserStarsButton;
                           QPushButton *findCloserObjectsButton;
	
	QDockWidget *dockKeyWidget;	   
		    QWidget *keyWidget;
			QScrollArea *keyScrollArea;
			QLabel *keyLabel;
			
	QDockWidget *dockConWidget;	   //??????? ?????? ??????????
		    QWidget *conWidget;
			QScrollArea *conScrollArea;
//			QLabel *keyLabel;
			QTextEdit *conText;
			
	 QDockWidget *dockScWidget;
        QWidget *magnifyWidget;
          QBoxLayout *boxScLayout;
            QLabel *scaledLabel;
			
	QDockWidget *dockFMgrWidget;
		QTableWidget *frameList;
		QTableWidgetItem *frameItem;
		QPushButton *firstButton;
		QPushButton *prevButton;
		QPushButton *nextButton;
		QPushButton *lastButton;
		QPushButton *openButton;
		QPushButton *closeButton;
                QPushButton *checkAllButton;
                QPushButton *unCheckAllButton;
//		QWidget *fileManagerWidget;
//		QList <frameRL*> frames;

	QDockWidget *dockPsfWidget;
		QRadioButton *lorentzRadioButton;
		QRadioButton *gaussianRadioButton;
		QRadioButton *moffatRadioButton;
		QRadioButton *cMassRadioButton;
		QDoubleSpinBox *orderSpinBox;
		QSpinBox *inSpinBox;
		QSpinBox *regSpinBox;
		QSpinBox *rpsSpinBox;
		QSpinBox *resSpinBox;
		QPushButton *measureMarkedButton;
		QCheckBox *addCheckBox;
		QCheckBox *cropCheckBox;

	QDockWidget *dockFiltersWidget;
                QPushButton *clearIpixButtonF;
		QLabel *levelBinLabel;
		QSlider *levelBinSlider;
		QLabel *aperBinLabel;
		QSlider *aperBinSlider;
		QPushButton *markBinFilterButton;
		
		QSpinBox *starTargetSpinBox;
		QPushButton *findStarsButton;

        QDockWidget *dockWcsWidget;
               QList <QLabel*> wcsList;
                 //
               QLabel *ct1ValLabel;
               QLabel *ct2ValLabel;
               //QLabel *sxValLabel;
               //QLabel *syValLabel;
                   QSpinBox *wcsSpinBox;
                   QPushButton *dropWcsButton;
                   QPushButton *writeWcsButton;
                   QPushButton *wcsButton;
                   QPushButton *wcsLoadButton;
                   QPushButton *wcsSaveButton;
                 //
                   QDoubleSpinBox *smaxSpinBox;

	QDockWidget *dockFitsHeaderWidget;
		QLabel *telLabel;
		QLabel *plateNLabel;
		QLabel *objLabel;
                QLabel *numOfExpLabel;
                QLabel *expNumLabel;
		QLabel *dateObsLabel;
                QLabel *uTimeLabel;
		QLabel *raLabel;
		QLabel *deLabel;
		QLabel *expLabel;
		QLabel *obsLabel;
		
		QLineEdit *telLE;
		QLineEdit *plateNLE;
		QLineEdit *objLE;
                QLineEdit *numOfExpLE;
                QComboBox *expNumSel;
		QLineEdit *dateObsLE;
                QLineEdit *uTimeLE;
		QLineEdit *raLE;
		QLineEdit *deLE;
                QLineEdit *expLE;
		QLineEdit *obsLE;
		
	QDockWidget *dockHist;
		QLabel *histLabel;
		QPushButton *updateHistButton;


         QDockWidget *dockGridStatWidget;
         QWidget *gridStat;

            QPushButton *ipixBtn;
            QPushButton *catBtn;
            QPushButton *refBtn;
            QPushButton *objBtn;
/*
            QLabel *yellLabel;
            QLabel *blueLabel;
            QLabel *redLabel;
            QLabel *greenLabel;
*/
	
	//END widgets
};




