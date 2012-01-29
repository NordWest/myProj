#include <QtGui>
#include <QLibrary>
#include "mainwindow.h"
#include "AccuTime.h"
#include "./../libs/fitsio.h"
#include "math.h"
#include "./../astro/astro.h"
//#include <QTranslator>





MainWindow::MainWindow()
{ 
    atInit(NULL);//initialization of accutime driver
        setWindowTitle ("qtccd2: S2C camera and Uniblitz shutter control");

    //


        //

        //app.beep();
        //QApplication::beep();
        //printf("\a\a\a");

    setupMainMenu();
    setupWidgets();
    imageLabel->installEventFilter(this);
	histLabel->installEventFilter(this);
    scaledLabel->installEventFilter(this);
    dockScWidget->installEventFilter(this);   
    scfactor = 1;
    mainX=0;
    mainY=0;
    expStarted = 0;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);
    //////////////////////////////
	firstClick = true;
	histX1 = 5;
	histX2 = 195;
	//updateHist(0);
	///////////////////////////////
	s2c = new S2C;
	connect(s2c, SIGNAL(isExposed()), this, SLOT(slotShowExposedFrame()));
	connect(s2c, SIGNAL(isStarted()), this, SLOT(slotCamStarted()));
	connect(s2c, SIGNAL(isStopped()), this, SLOT(slotCamStopped()));
	connect(s2c, SIGNAL(isFinished()), this, SLOT(slotFinished()));
	s2c->ccd_ignore = 0;
	s2c->adapter_type = 1;
	s2c->adapter_slot = 0;
	s2c->video_size = 2000;
	T_ccd = 0;
	T_hot = 0;
	exp_msek.start();
    //uniblitz dll import
    /*QLibrary UniblitzShutterLib("Shutter");
	UniblitzShutterLib.load();
	if (UniblitzShutterLib.isLoaded())
	{
		SoftControlDll = (SOFTCONTROLDLL) UniblitzShutterLib.resolve("SoftControl");
		HardControlDll = (HARDCONTROLDLL) UniblitzShutterLib.resolve("HardControl");
		SetOnOffDll = (SETONOFFDLL) UniblitzShutterLib.resolve("SetOn_nOff");
		InitPortDll = (INITPORTDLL) UniblitzShutterLib.resolve("Init_port");
		//
		char port[] = "COM1";
 		InitPortDll(port);
 		SoftControlDll();
	}
    *///uniblitz dll import
    QLibrary io32Lib("inpout32");
	io32Lib.load();
	if (io32Lib.isLoaded())
	{
		oup32 = (oupfuncPtr) io32Lib.resolve("Out32");
	}

    numOfFrame=0;
	
    /////////////////////////////////////////////////////////////////////
	settings = new QSettings("conf.ini", QSettings::IniFormat);
	currentDir = settings->value("general/curr_dir").toString();
	currentDarkDir = settings->value("general/dark_dir").toString();
	currentFlatDir = settings->value("general/flat_dir").toString();
	shdelay = settings->value("shutter/shdelay").toInt();
	spbShutterDelay->setValue(shdelay);
	exposureDelay->setValue(settings->value("shutter/exdelay").toInt());
	exptimecorr=settings->value("shutter/exptimecorr").toDouble();
	dHA = settings->value("zero_points/delta_ha").toDouble();
	dDE = settings->value("zero_points/delta_de").toDouble();
	create_cdir = settings->value("general/create_cdir").toInt();
	lang = settings->value("general/lang").toInt();
	//QMessageBox::information(0,"debug",QString( "%1" ).arg(shdelay,5,10,QLatin1Char( ' ' )),QMessageBox::Ok,0,0);
        useWindowsUserNames = settings->value("general/useWindowsUserNames", 0).toInt();
        useEaExp = settings->value("general/useEaExp", 0).toInt();
        mesEndExp = settings->value("general/mesEndExp", 0).toInt();


        settings->beginGroup("observers");
        userNames = settings->childKeys();
        int unSz = userNames.size();
        for(int i=0; i<unSz; i++)obsNames << settings->value(userNames[i], "no name").toString();
        settings->endGroup();

         QString uName;
         if(useWindowsUserNames)
         {
            //QSettings regSet("HKEY_CURRENT_USER\\Volatile Environment", QSettings::NativeFormat);
            QSettings regSet("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer", QSettings::NativeFormat);
            //uName = regSet.value("USERNAME", "yes name").toString();
            uName = regSet.value("Logon User Name", "yes name").toString();
            settings->beginGroup("observers");
            observersLineEdit->setText(settings->value(uName, "no user").toString());
            settings->endGroup();
            //observersLineEdit->setReadOnly(1);
         }



         curFilter = settings->value("filters/curFilter", 0).toInt();
         filterNames = settings->value("filters/filterNames", "SZS21 (100% within 400 to 500 nm)").toString().split("|");
         filterLineEdit->setText(filterNames[curFilter]);


        if(settings->value("general/useBegPropDlg", 0).toInt()) slotBegPropDlg();

        //tracker
        useTracker = settings->value("tracker/useTracker", 0).toInt();
        trackerCommand = settings->value("tracker/command", "").toString();
        comPortNum = settings->value("tracker/comPortNum", 1).toInt();



	/////////////////////////////////////////////////////////////////////////
	if (create_cdir)
	{
		QDate curdate = QDate::currentDate();
		QString dateStr = "ccd"+QString( "%1" ).arg(curdate.year()-2000,2,10,QLatin1Char( '0' ))+
							QString( "%1" ).arg(curdate.month(),2,10,QLatin1Char( '0' ))+
							QString( "%1" ).arg(curdate.day(),2,10,QLatin1Char( '0' ));
		//QMessageBox::information(0,"debug",dateStr,QMessageBox::Ok,0,0);
		QDir curdir(settings->value("general/root_dir").toString());
		curdir.mkdir(dateStr);
		curdir.mkdir(dateStr+"/dark");
		curdir.mkdir(dateStr+"/flat");
		currentDir = settings->value("general/root_dir").toString()+"/"+dateStr;
		currentDarkDir = settings->value("general/root_dir").toString()+"/"+dateStr+"/dark";
		currentFlatDir = settings->value("general/root_dir").toString()+"/"+dateStr+"/flat";
	}
	//////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////
	currentDirLabel->setText("CURRENT FOLDER = "+currentDir);
	currentDarkDirLabel->setText("DARK FOLDER = "+currentDarkDir);
	currentFlatDirLabel->setText("FLAT FOLDER = "+currentFlatDir);
	QString str1 = QString( "%1" ).arg( fabs(dHA),5,'f',1,QLatin1Char( '0' ));
	QString str2 = QString( "%1" ).arg( fabs(dDE),5,'f',1,QLatin1Char( '0' ));
	if (dHA>0) str1 = "+"+str1; else str1 = "-"+str1;
	if (dDE>0) str2 = "+"+str2; else str2 = "-"+str2;
	corrLineEdit->setText(str1+" "+str2);
	/////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////  
	curX =0;
	curY =0;
	////////////////
	ea = new QSettings("LAZA","Eassistant");
	raObj = 0;
	deObj = 0;
	//QMessageBox::information(0,"debug",ea->value("RADEC").toString(),QMessageBox::Ok,0,0);    


        if(useTracker)
        {
            connect(trackerWidget, SIGNAL(stateChanged()), this, SLOT(slotUpdateTrackerState()));
            autoTimer = new autoSetupTimerDlg;
            connect(autoTimer, SIGNAL(timerStoped()), this, SLOT(slotAutoGuidStopped()));
        }

        slotLoadDefaultPanels();

        progStarted = 0;
};

void MainWindow::setupMainMenu()
{
    //BEGIN menu file
    fileMenu = menuBar()->addMenu(tr("file"));
     openAct = new QAction(tr("&open"), this);
        openAct->setShortcut(tr("Ctrl+O"));
        openAct->setStatusTip(tr("open file"));
     fileMenu->addAction(openAct);
     connect(openAct, SIGNAL(triggered()), this, SLOT(open_file()));
     //
	 saveAct = new QAction(tr("&save fits"), this);
        saveAct->setShortcut(tr("Ctrl+S"));
        saveAct->setStatusTip(tr("save fits"));
     fileMenu->addAction(saveAct);
     connect(saveAct, SIGNAL(triggered()), this, SLOT(slotSaveFitsFile()));

    //
    loadPanelsAct = new QAction(tr("&load panels"), this);
    loadPanelsAct->setStatusTip(tr("load panels"));
    fileMenu->addAction(loadPanelsAct);
    connect(loadPanelsAct, SIGNAL(triggered()), this, SLOT(slotLoadPanels()));
    //
    //
    savePanelsAct = new QAction(tr("&save panels"), this);
    savePanelsAct->setStatusTip(tr("save panels"));
    fileMenu->addAction(savePanelsAct);
    connect(savePanelsAct, SIGNAL(triggered()), this, SLOT(slotSavePanels()));
    //
	 //
     exitAct = new QAction(tr("&exit"), this);
        exitAct->setShortcut(tr("Ctrl+X"));
        exitAct->setStatusTip(tr("exit"));
     fileMenu->addAction(exitAct);
     connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
     //END menu file
     
    //BEGIN menu tools
    toolsMenu = menuBar()->addMenu(tr("tools"));
     magnifyAct = new QAction(tr("&view panel"), this);
        magnifyAct->setShortcut(tr("Ctrl+V"));
        magnifyAct->setStatusTip(tr("view panel"));
     toolsMenu->addAction(magnifyAct);
     connect(magnifyAct, SIGNAL(triggered()), this, SLOT(magnify()));
     //
     TimePosAct = new QAction(tr("&pos&time"), this);
        TimePosAct->setShortcut(tr("Ctrl+D"));
        TimePosAct->setStatusTip(tr("pos&time"));
     toolsMenu->addAction(TimePosAct);
     connect(TimePosAct, SIGNAL(triggered()), this, SLOT(placing()));
     //
     propAct = new QAction(tr("&properties"), this);
        propAct->setShortcut(tr("Ctrl+P"));
        propAct->setStatusTip(tr("properties"));
     toolsMenu->addAction(propAct);
     connect(propAct, SIGNAL(triggered()), this, SLOT(slotPropSetup()));
	 //
	 
     //END menu tools
	 //BEGIN menu CAMERA
	 cameraMenu = menuBar()->addMenu(tr("&camera"));
	 //
        cameraAct = new QAction(tr("&camera"), this);
        cameraAct->setShortcut(tr("Ctrl+C"));
        cameraAct->setStatusTip(tr("camera"));
        cameraMenu->addAction(cameraAct);
        connect(cameraAct, SIGNAL(triggered()), this, SLOT(cameraSetup()));
	 //
	 //
     exposureAct = new QAction(tr("&exposure"), this);
        exposureAct->setShortcut(tr("Ctrl+E"));
        exposureAct->setStatusTip(tr("exposure"));
     cameraMenu->addAction(exposureAct);
     connect(exposureAct, SIGNAL(triggered()), this, SLOT(exposureSetup()));
	 //
	 //
     dfAct = new QAction(tr("&dark and flat"), this);
        dfAct->setShortcut(tr("Ctrl+F"));
        dfAct->setStatusTip(tr("dark and flat"));
     cameraMenu->addAction(dfAct);
     connect(dfAct, SIGNAL(triggered()), this, SLOT(slotDarkFlatSetup()));
	 //
	 shutterAct = new QAction(tr("&shutter"), this);
        shutterAct->setShortcut(tr("Ctrl+Z"));
        shutterAct->setStatusTip(tr("shutter"));
     cameraMenu->addAction(shutterAct);
     connect(shutterAct, SIGNAL(triggered()), this, SLOT(slotShutterSetup()));

         trackerAct = new QAction(tr("&tracker"), this);
        trackerAct->setShortcut(tr("Ctrl+T"));
        trackerAct->setStatusTip(tr("tracker"));
     cameraMenu->addAction(trackerAct);
     connect(trackerAct, SIGNAL(triggered()), this, SLOT(slotTrackerSetup()));

         guidAct = new QAction(tr("&guid"), this);
        guidAct->setShortcut(tr("Ctrl+G"));
        guidAct->setStatusTip(tr("guid"));
     cameraMenu->addAction(guidAct);
     connect(guidAct, SIGNAL(triggered()), this, SLOT(slotShowGuid()));

         autoGuidAct = new QAction(tr("&Auto guid"), this);
        autoGuidAct->setShortcut(tr("Ctrl+H"));
        autoGuidAct->setStatusTip(tr("auto guid"));
     cameraMenu->addAction(autoGuidAct);
     connect(autoGuidAct, SIGNAL(triggered()), this, SLOT(slotShowAutoGuid()));
	 //END menu CAMERA
};

void MainWindow::setupWidgets()
{
     imageLabel = new QLabel;
     imageLabel->setBackgroundRole(QPalette::Base);
     imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
     imageLabel->setScaledContents(true);
	 imageLabel->resize(0,0);

     scrollArea = new QScrollArea;
     scrollArea->setMinimumSize(600,400);
     scrollArea->setBackgroundRole(QPalette::Dark);
     scrollArea->setWidget(imageLabel);
     
     setCentralWidget(scrollArea);
     statusBar()->showMessage(tr(" "));

     magnifyWidget = new QWidget;
          
     scaledLabel = new QLabel;
     scaledLabel->setBackgroundRole(QPalette::Base);
     scaledLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
     scaledLabel->setFixedSize(200,200);
	 
	 histLabel = new QLabel;
	 histLabel->setBackgroundRole(QPalette::Base);
     histLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
     histLabel->setFixedSize(200,150);
     
     scspinBox = new QSpinBox;
     scspinBox->setRange(1,10);
     scspinBox->setFixedWidth(200);
     scspinBox->setPrefix(tr("magnify = "));
     connect(scspinBox, SIGNAL(valueChanged(int)), this, SLOT(magnify_state(int)));
     
     scaleCheckBox=new QCheckBox(tr("scaling"));
     scaleCheckBox->setCheckState(Qt::Checked);
     connect(scaleCheckBox, SIGNAL(stateChanged(int)), this, SLOT(scale_state(int)));
     
     magnifyCheckBox=new QCheckBox(tr("smoothing"));
     connect(magnifyCheckBox, SIGNAL(stateChanged(int)), this, SLOT(magnify_state(int)));
     
     gridWidget = new QWidget;
     grid = new QGridLayout(gridWidget);
     grid->addWidget(scaleCheckBox,0,0);
     grid->addWidget(magnifyCheckBox,0,1);
     labelX = new QLabel("X = ");
     labelY = new QLabel("Y = ");
     labelI = new QLabel("I = ");
     contrastSlider = new QSlider(Qt::Horizontal,gridWidget);
     contrastSlider->setMinimum(0);
     contrastSlider->setMaximum(100);
     contrastSlider->setValue(50);
     contrastSlider->setSingleStep(5);
     contrastSlider->setPageStep(10);
     contrastSlider->setTracking(false);
     brightnessSlider = new QSlider(Qt::Horizontal,gridWidget);
     brightnessSlider->setMinimum(0);
     brightnessSlider->setMaximum(100);
     brightnessSlider->setValue(50);
     brightnessSlider->setSingleStep(5);
     brightnessSlider->setPageStep(10);
     brightnessSlider->setTracking(false);
     connect(contrastSlider, SIGNAL(valueChanged(int)), this, SLOT(slotContrast(int)));
     connect(brightnessSlider, SIGNAL(valueChanged(int)), this, SLOT(slotBrightness(int)));
     grid->addWidget(labelX,1,0);
     grid->addWidget(labelY,1,1);
     grid->addWidget(labelI,2,0);
     grid->addWidget(contrastSlider,3,0);
     grid->addWidget(brightnessSlider,3,1);
     labelBackground = new QLabel(tr("background"));
     labelDiapason = new QLabel(tr("diapason"));
     grid->addWidget(labelBackground,4,0);
     grid->addWidget(labelDiapason,4,1);
     
     boxScLayout = new QBoxLayout(QBoxLayout::TopToBottom,magnifyWidget);
     boxScLayout->addWidget(scaledLabel);
     boxScLayout->addWidget(scspinBox);
     boxScLayout->addWidget(gridWidget);
	 boxScLayout->addWidget(histLabel);
          
     dockScWidget = new QDockWidget(tr("view panel"),this,Qt::Widget);
     dockScWidget->setFixedSize(210,550);
     dockScWidget->setWidget(magnifyWidget);
     addDockWidget(Qt::LeftDockWidgetArea,dockScWidget);
     dockScWidget->setFloating(true);
     dockScWidget->hide();
	 
	 ////////////////////////////////////////
	 QPixmap pixmap(200,150);
	 QPainter paint(&pixmap);
	 QPen pen;
     pen.setColor(Qt::red);
	 pen.setWidth(2);
     paint.setPen(pen);
     paint.drawLine(0,0,200,150);
     histLabel->setPixmap(pixmap);
     histLabel->adjustSize();
     ///////////////////////////////////////////
     
     TimePosWidget = new QWidget;
	 QWidget *naWidget = new QWidget;
	 QGridLayout *tpGrid = new QGridLayout(TimePosWidget);
	 QGridLayout *naGrid = new QGridLayout(naWidget);



     labelUTC = new QLabel("UTC = ",TimePosWidget);
         //labelUTC->setFixedWidth(100);
         labelUTC->setFixedSize(100, 30);
	 QLabel *radeLabel = new QLabel("RA DEC MAG:");
	 QLabel *targetLabel = new QLabel(tr("target:"));
	 QLabel *fnameLabel = new QLabel(tr("file name:"));
	 radeLineEdit = new QLineEdit;
	 radeLineEdit -> setInputMask("99 99 99.999 #99 99 99.99 99.99");
	 radeLineEdit -> setText ("00 00 00.000 +00 00 00.00 00.00");
	 corrLineEdit = new QLineEdit;
	 corrLineEdit -> setInputMask("#999.9 #999.9");
	 corrLineEdit -> setText ("-000.0 -000.0");
	 targetLineEdit = new QLineEdit;
	 fnameLineEdit = new QLineEdit;
	 getEaButton = new QPushButton(tr("get from EA"));
     connect(getEaButton, SIGNAL(clicked()), this, SLOT(getEaButtonClicked()));
	 adoptButton = new QPushButton(tr("adopt"));
     connect(adoptButton, SIGNAL(clicked()), this, SLOT(slotAdoptButtonClicked()));


         bFont = QFont::QFont("Helvetica", 60, QFont::Bold);
         sFont = QFont::QFont("Helvetica", 30, QFont::Bold);
         ////////////////////
         sTimeLabel = new QLabel;
         sTimeLabel->setFixedSize(600,50);
         //QPalette naPal(Qt::lightGray,Qt::black);
         sTimeLabel->setBackgroundRole(QPalette::ButtonText);
         sTimeLabel->setFont(sFont);
         sTimeLabel->setText("ok");
         ////////////////////
	 ////////////////////
	 nAstrograph = new QLabel;
         //nAstrograph->setFixedSize(600,300);

         nAstrograph->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

	 //QPalette naPal(Qt::lightGray,Qt::black);
	 nAstrograph->setBackgroundRole(QPalette::ButtonText);
         nAstrograph->setFont(bFont);
	 nAstrograph->setText("ok");
	 ////////////////////
	 tpGrid->addWidget(labelUTC,0,0);
	 tpGrid->addWidget(targetLabel,1,0);
	 tpGrid->addWidget(targetLineEdit,1,1);
	 tpGrid->addWidget(fnameLabel,2,0);
	 tpGrid->addWidget(fnameLineEdit,2,1);
	 tpGrid->addWidget(radeLabel,3,0);
	 tpGrid->addWidget(radeLineEdit,3,1);
	 tpGrid->addWidget(getEaButton,3,2);
	 tpGrid->addWidget(corrLineEdit,4,1);
	 tpGrid->addWidget(adoptButton,4,2);
	 naGrid->addWidget(TimePosWidget,0,0);
         naGrid->addWidget(sTimeLabel,1,0);
         naGrid->addWidget(nAstrograph,2,0);
         naGrid->setRowStretch(3,2);
	 //=================================================================
     dockTimePosWidget = new QDockWidget(tr("position and time"),this,Qt::Widget);
     dockTimePosWidget->setFixedSize(600,400);
     dockTimePosWidget->setWidget(naWidget);
     addDockWidget(Qt::BottomDockWidgetArea,dockTimePosWidget);
     dockTimePosWidget->setFloating(true);
     dockTimePosWidget->hide();
     
     ////////////////////////////////////////////////
     cameraWidget = new QWidget;
     camInitButton = new QPushButton(tr("camera turn on"));
     connect(camInitButton, SIGNAL(clicked()), this, SLOT(cameraButtonClicked()));
     applyButton = new QPushButton(tr("apply"));
     connect(applyButton, SIGNAL(clicked()), this, SLOT(applyButtonClicked()));
	 applyButton->setEnabled(false);
     camTurnOff = new QPushButton(tr("camera turn off"));
     connect(camTurnOff, SIGNAL(clicked()), this, SLOT(camTurnOffClicked()));
     gridCamera = new QGridLayout(cameraWidget);
     camTurnOff->setEnabled(false);
     //////////////////////////////////////////////////////////
     coolingBox = new QGroupBox(tr("cooling"),cameraWidget);
     coolingSpBox = new QSpinBox(coolingBox);
     coolingSpBox->setRange(-50,-10);
     coolingSpBox->setFixedWidth(100);
     coolingSpBox->setValue(-30);
     coolingSpBox->setPrefix("CCD T = ");
     coolCheckBox = new QCheckBox(tr("on/off"));
     coolCheckBox->setCheckState(Qt::Checked);
     fanCheckBox = new QCheckBox(tr("FAN enable"));
     fanCheckBox->setCheckState(Qt::Checked);
     stabCheckBox = new QCheckBox(tr("stabilization"));
     stabCheckBox->setCheckState(Qt::Checked);
	 connect(coolingSpBox, SIGNAL(valueChanged (int)), this, SLOT(camSettingsChanged(int)));
	 connect(coolCheckBox, SIGNAL(stateChanged (int)), this, SLOT(camSettingsChanged(int)));
	 connect(fanCheckBox, SIGNAL(stateChanged (int)), this, SLOT(camSettingsChanged(int)));
	 connect(stabCheckBox, SIGNAL(stateChanged (int)), this, SLOT(camSettingsChanged(int)));
     coolingLayout = new QBoxLayout(QBoxLayout::TopToBottom,coolingBox);
     coolingLayout->addWidget(coolingSpBox);
     coolingLayout->addWidget(coolCheckBox);
     coolingLayout->addWidget(fanCheckBox);
     coolingLayout->addWidget(stabCheckBox);
     ////////////////////////////////////////
     frequencyBox = new QGroupBox(tr("frequency"),cameraWidget);
     frequencyLayout = new QBoxLayout(QBoxLayout::LeftToRight,frequencyBox);
     highRadioButton = new QRadioButton(tr("high"));
     lowRadioButton = new QRadioButton(tr("low"));
     lowRadioButton->setChecked(true);
	 connect(highRadioButton, SIGNAL(toggled(bool)), this, SLOT(camRSettingsChanged(bool)));
	 connect(lowRadioButton, SIGNAL(toggled(bool)), this, SLOT(camRSettingsChanged(bool)));
     frequencyLayout->addWidget(highRadioButton);
     frequencyLayout->addWidget(lowRadioButton); 
     ////////////////////////////////////////////
     cleaningBox = new QGroupBox(tr("cleaning"),cameraWidget);
     cleaningLayout = new QBoxLayout(QBoxLayout::LeftToRight,cleaningBox);
     fastCheckBox = new QCheckBox(tr("fast"));
     slowCheckBox = new QCheckBox(tr("slow"));
     slowCheckBox->setCheckState(Qt::Checked);
	 connect(fastCheckBox, SIGNAL(stateChanged (int)), this, SLOT(camSettingsChanged(int)));
	 connect(slowCheckBox, SIGNAL(stateChanged (int)), this, SLOT(camSettingsChanged(int)));
     cleaningLayout->addWidget(fastCheckBox);
     cleaningLayout->addWidget(slowCheckBox); 
     /////////////////////////////////////////
     QWidget *ClFreqWidget = new QWidget(cameraWidget);
     QBoxLayout *ClFreqLayout = new QBoxLayout(QBoxLayout::TopToBottom,ClFreqWidget);
     ClFreqLayout->addWidget(frequencyBox);
     ClFreqLayout->addWidget(cleaningBox);
     /////////////////////////////////////
     QGroupBox *frameBox = new QGroupBox(tr("frame"),cameraWidget);
     QGridLayout *frameGrid = new QGridLayout(frameBox);
     frameX = new QSpinBox;
     frameX->setRange(1,1160);
     frameX->setFixedWidth(120);
     frameX->setValue(1);
     frameX->setPrefix("X = ");
	 connect(frameX, SIGNAL(valueChanged (int)), this, SLOT(camSettingsChanged(int)));
     frameY = new QSpinBox;
     frameY->setRange(1,1040);
     frameY->setFixedWidth(120);
     frameY->setValue(1);
     frameY->setPrefix("Y = ");
	 connect(frameY, SIGNAL(valueChanged (int)), this, SLOT(camSettingsChanged(int)));
     frameXSize = new QSpinBox;
     frameXSize->setRange(10,1160);
     frameXSize->setFixedWidth(200);
     frameXSize->setValue(1160);
     frameXSize->setPrefix("XSize = ");
	 connect(frameXSize, SIGNAL(valueChanged (int)), this, SLOT(camSettingsChanged(int)));
     frameYSize = new QSpinBox;
     frameYSize->setRange(10,1040);
     frameXSize->setFixedWidth(200);
     frameYSize->setValue(1040);
     frameYSize->setPrefix("YSize = ");
	 connect(frameYSize, SIGNAL(valueChanged (int)), this, SLOT(camSettingsChanged(int)));
     frameBinX = new QSpinBox;
     frameBinX->setRange(1,8);
     frameBinX->setFixedWidth(200);
     frameBinX->setValue(1);
     frameBinX->setPrefix("BinX = ");
	 connect(frameBinX, SIGNAL(valueChanged (int)), this, SLOT(camSettingsChanged(int)));
     frameBinY = new QSpinBox;
     frameBinY->setRange(1,8);
     frameBinY->setFixedWidth(100);
     frameBinY->setValue(1);
     frameBinY->setPrefix("BinY = ");
	 connect(frameBinY, SIGNAL(valueChanged (int)), this, SLOT(camSettingsChanged(int)));
     frameGrid->addWidget(frameX,0,0);
     frameGrid->addWidget(frameY,1,0);
     frameGrid->addWidget(frameXSize,0,1);
     frameGrid->addWidget(frameYSize,1,1);
     frameGrid->addWidget(frameBinX,2,0);
     frameGrid->addWidget(frameBinY,2,1);
	 ////////////////////////////////////////////
	 QGroupBox *gainBox = new QGroupBox(tr("digital gain"),cameraWidget);
     QGridLayout *gainGrid = new QGridLayout(gainBox);
	 chbDigGain = new QCheckBox(tr("digital gain"));
     chbDigGain->setCheckState(Qt::Checked);
	 spbDigGain = new QSpinBox;
     spbDigGain->setRange(1,8);
     spbDigGain->setFixedWidth(100);
     spbDigGain->setValue(8);
     spbDigGain->setPrefix(tr("dig gain = "));
	 //=
	 chbBL = new QCheckBox(tr("BL restoration"));
     chbBL->setCheckState(Qt::Checked);
	 spbBL = new QSpinBox;
     spbBL->setRange(0,100);
     spbBL->setFixedWidth(100);
     spbBL->setValue(50);
     spbBL->setPrefix(tr("BL offset = "));
	 spbOffset = new QSpinBox;
     spbOffset->setRange(0,1000);
     spbOffset->setFixedWidth(140);
     spbOffset->setValue(100);
     spbOffset->setPrefix(tr("offset = "));
	 connect(spbBL, SIGNAL(valueChanged (int)), this, SLOT(camSettingsChanged(int)));
	 connect(spbDigGain, SIGNAL(valueChanged (int)), this, SLOT(camSettingsChanged(int)));
	 connect(chbBL, SIGNAL(stateChanged (int)), this, SLOT(camSettingsChanged(int)));
	 connect(chbDigGain, SIGNAL(stateChanged (int)), this, SLOT(camSettingsChanged(int)));
	 connect(spbOffset, SIGNAL(valueChanged (int)), this, SLOT(camSettingsChanged(int)));
	 gainGrid->addWidget(spbDigGain,0,0);
	 gainGrid->addWidget(chbDigGain,1,0);
	 gainGrid->addWidget(spbBL,2,0);
	 gainGrid->addWidget(chbBL,3,0);
     ////////////////////////////////////////////     
     gridCamera->addWidget(coolingBox,0,0);
     gridCamera->addWidget(ClFreqWidget,0,1);
     gridCamera->addWidget(frameBox,0,2);
	 gridCamera->addWidget(gainBox,0,3);
     gridCamera->addWidget(camInitButton,1,0);
     gridCamera->addWidget(applyButton,1,1);
     gridCamera->addWidget(camTurnOff,1,2);
	 
     //////////////////////////////////////////////////////////
     dockCameraWidget = new QDockWidget(tr("camera"),this,Qt::Widget);
     dockCameraWidget->setFixedSize(700,200);
     dockCameraWidget->setWidget(cameraWidget);
     addDockWidget(Qt::BottomDockWidgetArea,dockCameraWidget);
     dockCameraWidget->setFloating(true);
     dockCameraWidget->hide();
     
     ////////////////////////////////////////////////
	 QWidget *exposureWidget = new QWidget;
	 QGridLayout *expGrid = new QGridLayout(exposureWidget);
	 MakeExposureButton = new QPushButton(tr("make"));
	 MakeExposureButton->setEnabled(false);
	 connect(MakeExposureButton, SIGNAL(clicked()), this, SLOT(MakeExposureButtonClicked()));
	 stopExposureButton = new QPushButton(tr("stop"));
	 connect(stopExposureButton, SIGNAL(clicked()), this, SLOT(stopExposureButtonClicked()));
	 stopExposureButton->setEnabled(false);
	 //
	 exposureDelay = new QSpinBox;
     exposureDelay->setRange(0,10000);
     exposureDelay->setFixedWidth(200);
     //exposureDelay->setValue(0);
     exposureDelay->setPrefix(tr("delay = "));
	 exposureDelay->setSuffix(" ms");
	 spbShutterDelay = new QSpinBox;
     spbShutterDelay->setRange(0,10000);
     spbShutterDelay->setFixedWidth(200);
     //QMessageBox::information(0,"debug",QString( "%1" ).arg(shdelay,5,10,QLatin1Char( ' ' )),QMessageBox::Ok,0,0);
     //shdelay=settings->value("shutter/shdelay").toInt();
     //spbShutterDelay->setValue(shdelay);
     spbShutterDelay->setPrefix(tr("shutter delay = "));
	 spbShutterDelay->setSuffix(" ms ");
	 exposureCount = new QSpinBox;
     exposureCount->setRange(1,1000);
     exposureCount->setFixedWidth(150);
     exposureCount->setValue(1);
     exposureCount->setPrefix(tr("count = "));
	 exposureTime = new QDoubleSpinBox;
	 exposureTime -> setDecimals(3);
	 exposureTime -> setPrefix (tr("exposure time = "));
	 exposureTime -> setSuffix (tr(" sec"));
	 exposureTime -> setRange(0.001,1000.0);
	 exposureTime -> setSingleStep (1);
	 exposureTime -> setValue(1.0);
	 exposureTime -> setFixedWidth(200);
	 chbSaveFits = new QCheckBox(tr("save to fits"));
     chbSaveFits->setCheckState(Qt::Checked); 
	 chbUseShutter = new QCheckBox(tr("use shutter"));
     chbUseShutter->setCheckState(Qt::Checked); 
	 //////////////////////////////////////////
	 expGrid->addWidget(exposureTime,0,0);
	 expGrid->addWidget(exposureDelay,0,1);
	 expGrid->addWidget(exposureCount,0,2);
	 expGrid->addWidget(chbSaveFits,2,0);
	 expGrid->addWidget(chbUseShutter,2,1);
	 expGrid->addWidget(MakeExposureButton,3,0);
	 expGrid->addWidget(stopExposureButton,3,1);
	 expGrid->addWidget(spbOffset,1,0);
	 expGrid->addWidget(spbShutterDelay,1,1);
	 //
	 dockExposure = new QDockWidget("exposure",this,Qt::Widget);
     dockExposure->setFixedSize(600,150);
     dockExposure->setWidget(exposureWidget);
     addDockWidget(Qt::BottomDockWidgetArea,dockExposure);
     dockExposure->setFloating(true);
     dockExposure->hide();
	 //////
	 
	 
	 QWidget *propWidget = new QWidget;
	 QGridLayout *propGrid = new QGridLayout(propWidget);
	 currentDirLabel = new QLabel(tr("set current folder"));
	 currentDirLabel->setFixedWidth(500);
	 currDirButton = new QPushButton("...");
	 connect(currDirButton, SIGNAL(clicked()), this, SLOT(slotCurrDirButtonClicked()));
	 currentDarkDirLabel = new QLabel(tr("set current dark folder"));
	 currentDarkDirLabel->setFixedWidth(500);
	 currDarkDirButton = new QPushButton("...");
	 connect(currDarkDirButton, SIGNAL(clicked()), this, SLOT(slotCurrDarkDirButtonClicked()));
	 currentFlatDirLabel = new QLabel(tr("set current flat folder"));
	 currentFlatDirLabel->setFixedWidth(500);
	 currFlatDirButton = new QPushButton("...");
	 connect(currFlatDirButton, SIGNAL(clicked()), this, SLOT(slotCurrFlatDirButtonClicked()));
	 //
	 sbAirTemperature = new QSpinBox;
     sbAirTemperature->setRange(-50,+50);
     sbAirTemperature->setFixedWidth(160);
     sbAirTemperature->setValue(0);
    sbAirTemperature->setPrefix("air temperature = ");
	//
	dsbFocalLength = new QDoubleSpinBox;
	 dsbFocalLength -> setDecimals(1);
	 dsbFocalLength -> setPrefix (tr("focal length = "));
	 dsbFocalLength -> setSuffix (tr(" mm"));
	 dsbFocalLength -> setRange(0,50);
	 dsbFocalLength -> setSingleStep (0.1);
	 dsbFocalLength -> setValue(33.0);
	 dsbFocalLength ->setFixedWidth(300);
	 //
	 QLabel *observersLabel = new QLabel(tr("observers: "));
	 QLabel *filterLabel = new QLabel(tr("filter: "));
         observersLineEdit = new QLineEdit;
         observersLineEdit->setText("no name");
         observersLineEdit->setFixedWidth(300);
         /*
         observersBox = new QComboBox;
         observersBox->setFixedWidth(300);
        */

	 filterLineEdit = new QLineEdit;
	 filterLineEdit->setText("SZS21 (100% within 400 to 500 nm)");
	 filterLineEdit->setFixedWidth(300);
	 //==========================================================
	 propGrid->addWidget(currentDirLabel,0,0);
	 propGrid->addWidget(currDirButton,0,1);
	 propGrid->addWidget(currentDarkDirLabel,1,0);
	 propGrid->addWidget(currDarkDirButton,1,1);
	 propGrid->addWidget(currentFlatDirLabel,2,0);
	 propGrid->addWidget(currFlatDirButton,2,1);
	 //
	 propGrid->addWidget(sbAirTemperature,3,0);
	 propGrid->addWidget(dsbFocalLength,4,0);
	 propGrid->addWidget(observersLabel,6,0);
         propGrid->addWidget(observersLineEdit,6,1);
         //propGrid->addWidget(observersBox,6,1);
	 propGrid->addWidget(filterLabel,5,0);
	 propGrid->addWidget(filterLineEdit,5,1);
	 //============================================================
	 dockProp = new QDockWidget(tr("properties"),this,Qt::Widget);
     dockProp->setFixedSize(700,250);
     dockProp->setWidget(propWidget);
     addDockWidget(Qt::BottomDockWidgetArea,dockProp);
     dockProp->setFloating(true);
     dockProp->hide();
	 
	 ////////////////////////////////////////////////////////////////
	 QWidget *dfWidget = new QWidget;
	 QGridLayout *dfGrid = new QGridLayout(dfWidget);
	 QGroupBox *gbDark = new QGroupBox(tr("dark frames"),dfWidget);
	 QGridLayout *dGrid = new QGridLayout(gbDark);
	 QGroupBox *gbFlat = new QGroupBox(tr("flat field"),dfWidget);
	 QGridLayout *fGrid = new QGridLayout(gbFlat);
	 //=========================================================
	 darkFNLabel1 = new QLabel(tr("dark-1 file name"));
	 darkFNLabel1->setFixedWidth(300);
	 darkFNLabel2 = new QLabel(tr("dark-2 file name"));
	 darkFNLabel2->setFixedWidth(300);
	 loadDarkFileButton1 = new QPushButton(tr("load dark-1"));
	 connect(loadDarkFileButton1, SIGNAL(clicked()), this, SLOT(slotLoadDarkFileButton1Clicked()));
	 loadDarkFileButton2 = new QPushButton(tr("load dark-2"));
	 connect(loadDarkFileButton2, SIGNAL(clicked()), this, SLOT(slotLoadDarkFileButton2Clicked()));
	 dGrid->addWidget(darkFNLabel1,2,0);
	 dGrid->addWidget(loadDarkFileButton1,2,1);
	 dGrid->addWidget(darkFNLabel2,3,0);
	 dGrid->addWidget(loadDarkFileButton2,3,1);
	 //==========================================================
	 flatFNLabel = new QLabel(tr("flat file name"));
	 flatFNLabel->setFixedWidth(300);
	 loadFlatFileButton = new QPushButton(tr("load flat"));
	 connect(loadFlatFileButton, SIGNAL(clicked()), this, SLOT(slotLoadFlatFileButtonClicked()));
	 //===========================================================
	 fGrid->addWidget(flatFNLabel,2,0);
	 fGrid->addWidget(loadFlatFileButton,2,1);
	 //=================================================
	 QGroupBox *gbApplyDark = new QGroupBox(tr("apply dark..."),dfWidget);
	 QGridLayout *applyDarkGrid = new QGridLayout(gbApplyDark);
	 noDarkRadioButton = new QRadioButton(tr("do not apply"));
     imDarkRadioButton = new QRadioButton(tr("apply in image"));
	 calcDarkRadioButton = new QRadioButton(tr("apply in data"));
	 noDarkRadioButton->setChecked(true);
	 applyDarkGrid->addWidget(noDarkRadioButton,0,0);
	 applyDarkGrid->addWidget(imDarkRadioButton,0,1);
	 applyDarkGrid->addWidget(calcDarkRadioButton,0,2);
	 //=======================================================
	 QGroupBox *gbApplyFlat = new QGroupBox(tr("apply flat..."),dfWidget);
	 QGridLayout *applyFlatGrid = new QGridLayout(gbApplyFlat);
	 noFlatRadioButton = new QRadioButton(tr("do not apply"));
     imFlatRadioButton = new QRadioButton(tr("apply in image"));
	 calcFlatRadioButton = new QRadioButton(tr("apply in data"));
	 noFlatRadioButton->setChecked(true);
	 applyFlatGrid->addWidget(noFlatRadioButton,0,0);
	 applyFlatGrid->addWidget(imFlatRadioButton,0,1);
	 applyFlatGrid->addWidget(calcFlatRadioButton,0,2);
	 //==================================
	 dfOkButton = new QPushButton(tr("ok"));
	 connect(dfOkButton, SIGNAL(clicked()), this, SLOT(slotdfOkButtonClicked()));
	 //==========================================================
	 dfGrid->addWidget(gbDark,0,0);
	 dfGrid->addWidget(gbApplyDark,1,0);
	 dfGrid->addWidget(gbFlat,2,0);
	 dfGrid->addWidget(gbApplyFlat,3,0);
	 dfGrid->addWidget(dfOkButton,4,0);
	 //============================================================
	 dockDarkFlat = new  QDockWidget(tr("dark frames and flat field"),this,Qt::Widget);
     dockDarkFlat->setFixedSize(700,400);
     dockDarkFlat->setWidget(dfWidget);
     addDockWidget(Qt::BottomDockWidgetArea,dockDarkFlat);
     dockDarkFlat->setFloating(true);
     dockDarkFlat->hide();
	 
	 
	 ////////////////////////////////////////////////////////////////
	 shutterOpenButton = new QPushButton(tr("  OPEN  "));
	 connect(shutterOpenButton, SIGNAL(clicked()), this, SLOT(shutterOpenButtonClicked()));
	 shutterCloseButton = new QPushButton(tr("  SHUT  "));
	 connect(shutterCloseButton, SIGNAL(clicked()), this, SLOT(shutterCloseButtonClicked()));
	 //========================================================
	 QWidget *shutterWidget = new QWidget;
	 //=============================================================
	 QGridLayout *shutterGrid = new QGridLayout(shutterWidget);
	 shutterGrid->addWidget(shutterOpenButton,0,0);
	 shutterGrid->addWidget(shutterCloseButton,0,1);
	 //========================================================
	 dockShutter = new  QDockWidget(tr("shutter"),this,Qt::Widget);
     dockShutter->setFixedSize(400,100);
     dockShutter->setWidget(shutterWidget);
     addDockWidget(Qt::BottomDockWidgetArea,dockShutter);
     dockShutter->setFloating(true);
     dockShutter->hide();

     dockTracker = new  QDockWidget(tr("tracker"),this,Qt::Widget);

         trackerWidget = new serialWidget;
         dockTracker->setWidget(trackerWidget);
         addDockWidget(Qt::BottomDockWidgetArea,dockTracker);
         dockTracker->setFloating(true);
         dockTracker->hide();




     dockGuid = new  QDockWidget(tr("guid"),this,Qt::Widget);

         QWidget *guidWidget = new QWidget;
         dockGuid->setWidget(guidWidget);
         addDockWidget(Qt::BottomDockWidgetArea,dockGuid);
         dockGuid->setFloating(true);
         dockGuid->hide();
  /*
         QLabel *stateLabel;
         QPushButton *startButton;
         QPushButton *stopButton;
   */
         QVBoxLayout *guidGrid = new QVBoxLayout(guidWidget);

         stateLabel = new QLabel("no state");
         startButton = new QPushButton("start program");
         stopButton = new QPushButton("stop program");

         stateLog = new QTextEdit();
         stateLog->setReadOnly(1);



         connect(startButton, SIGNAL(clicked()), this, SLOT(slotStartGuidProgram()));
         connect(stopButton, SIGNAL(clicked()), this, SLOT(slotStopGuidProgram()));

         guidGrid->addWidget(stateLabel);
         guidGrid->addWidget(startButton);
         guidGrid->addWidget(stopButton);
         guidGrid->addWidget(stateLog);


    dockAutoGuid = new  QDockWidget(tr("Auto Guid"),this,Qt::Widget);

    QWidget *autoGuidWidget = new QWidget;
    dockAutoGuid->setWidget(autoGuidWidget);
    addDockWidget(Qt::BottomDockWidgetArea,dockAutoGuid);
    dockAutoGuid->setFloating(true);
    dockAutoGuid->hide();

    stateLabel1 = new QLabel("no state");

    timeDelay = new QDoubleSpinBox;
    timeDelay->setPrefix("Time delay: ");
    timeDelay->setSuffix(" minutes");
    timeDelay->setValue(2);
    timeDelay->setRange(1, 100);
    timeDelay->setSingleStep(0.5);
    timeDelay->setDecimals(1);

    autoStopTracker = new QCheckBox("Auto stop tracker after");
    autoStopTracker->setChecked(0);
    startWhenReady = new QCheckBox("Start exposure when ready");
    startWhenReady->setChecked(0);

    startSetupButton = new QPushButton("Start setup");

    connect(startSetupButton, SIGNAL(clicked()), this, SLOT(slotAutoGuidStarted()));

    QVBoxLayout *aGuidGrid = new QVBoxLayout(autoGuidWidget);
   
    aGuidGrid->addWidget(stateLabel1);
    aGuidGrid->addWidget(timeDelay);
    aGuidGrid->addWidget(autoStopTracker);
    //aGuidGrid->addWidget(startWhenReady);
    aGuidGrid->addWidget(startSetupButton);
     
};//end setupWidgets()


void MainWindow::open_file()
{
    QString FName = QFileDialog::getOpenFileName(
                    this,
                    tr("Choose a file for dark1"),
                    currentDir,
                    "CCD (*.fit *.fits *.fts *.img)");
	 if (FName!="")
	 {
	    QFileInfo fileInfo(FName);
	    if (fileInfo.suffix()=="img")
		{
		  if (frame) {
                     delete frame;
                     delete img;
                     } 
           frame = new CCDFrame(FName);
		}
		else
		{
		    QByteArray fn = FName.toAscii();
	        char *fname = fn.data();
	        fitsfile *fptr;
	        int status = 0;
		    long naxes[2];
			long felem = 1;
			fits_open_file(&fptr, fname , READONLY,&status);
			fits_get_img_size(fptr, 2, naxes, &status);
			LONGLONG nelements = naxes[0]*naxes[1];
			int anynul = 0;
			unsigned short *fits_buff = new unsigned short[nelements];
			fits_read_img(fptr, TUSHORT, felem, nelements, NULL, fits_buff, &anynul, &status);
			fits_close_file(fptr, &status);
			////
			if (frame) {//QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
                     delete frame;
                     delete img;
                     }
			frame = new CCDFrame(fits_buff,naxes[0],naxes[1]);
		    frame->getRange(frame->CCDdata);
            //frame->setImgData(frame->CCDdata, frame->maxHist - frame->offsetBC, frame->maxHist + frame->offsetBC,1,0);
			////
			delete fits_buff;
		}
	 
    /////////////////////////        
   img = new QImage(frame->CCDImage,frame->XSize,frame->YSize,QImage::Format_Indexed8);
   if (img->isNull()) QMessageBox::information(0,"debug",tr("image has not been loaded"),QMessageBox::Ok,0,0);
   img->setColorTable(frame->colormap);
   if (scaleCheckBox->checkState() == Qt::Checked)
			  imageLabel->setPixmap(QPixmap::fromImage(img->scaled(scrollArea->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation)));
	else 
			 imageLabel->setPixmap(QPixmap::fromImage(img->copy(0,0,frame->XSize,frame->YSize)));
   imageLabel->adjustSize();
   scfactor = (double)frame->YSize/(double)imageLabel->height();
   if (frame->maxHist<frame->offsetBC) leftB=frame->MinV; else leftB = frame->maxHist-frame->offsetBC;
   if (frame->maxHist+frame->offsetBC>frame->histsize) rightB=frame->MaxV; else rightB = frame->maxHist+frame->offsetBC;
   frame->setImgData(frame->CCDdata, leftB, rightB,1,0);
   contrastSlider->setRange(leftB,rightB);
   contrastSlider->setValue(leftB);
   brightnessSlider->setRange(leftB,rightB);
   brightnessSlider->setValue(rightB);
   date_obs = QDateTime::currentDateTime();
   long hsize = frame->MaxV - frame->MinV + 1;
   histX1 = 5+(int)(190*(double)(leftB - frame->MinV)/hsize);
   histX2 = 5+(int)(190*(double)(rightB - frame->MinV)/hsize);
   updateHist(0);
   QMouseEvent event(QEvent::MouseButtonPress, QPoint::QPoint(curX,curY), Qt::LeftButton, 0, 0);
   QApplication::sendEvent(imageLabel, &event);
   
   //QString mstr;
   //mstr.setNum(histX1);
   //QMessageBox::information(0,"debug",mstr,QMessageBox::Ok,0,0);
   updateHist(0);
   }
};//end open_file()

void MainWindow::magnify()
{
     //QMessageBox::information(0,"debug","bad",QMessageBox::Ok,0,0);
     if (dockScWidget->isHidden()) 
                    {
                      dockScWidget->show();
                      //imageLabel->setCursor(Qt::CrossCursor);
                    }//end if
};

void MainWindow::placing()
{
  if (dockTimePosWidget->isHidden()) 
                    {
                      dockTimePosWidget->show();
                    }//end if
};

void MainWindow::cameraSetup()
{
  if (dockCameraWidget->isHidden()) 
                    {
                      dockCameraWidget->show();
                    }//end if
}//end cameraSetup()

void MainWindow::exposureSetup()
{
  if (dockExposure->isHidden()) 
                    {
                      dockExposure->show();
                    }//end if
}//end cameraSetup()
//slots
void MainWindow::scale_state(int state)
{
   //QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
   if (img){
   if (state == Qt::Checked)
                      imageLabel->setPixmap(QPixmap::fromImage(img->scaled(scrollArea->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation)));
            else 
                     imageLabel->setPixmap(QPixmap::fromImage(img->copy(0,0,frame->XSize,frame->YSize)));
   imageLabel->adjustSize();
   scfactor = (double)(frame->YSize/(double)imageLabel->height());
   }//if img 
};

void MainWindow::magnify_state(int state)
{
   //QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
   if (img){
   int x_pos = mainX;
   int y_pos = mainY;
   int scw = (int) 200/scspinBox->value();
   if ((x_pos>=0)&&(x_pos<frame->XSize)&&(y_pos>=0)&&(y_pos<frame->YSize)){
   QImage sc_image=img->copy(x_pos-scw/2,y_pos-scw/2,scw,scw);
   if (magnifyCheckBox->checkState() == Qt::Checked)
         scaledLabel->setPixmap(QPixmap::fromImage(sc_image.scaled(200,200,Qt::KeepAspectRatio,Qt::SmoothTransformation)));
   else
        scaledLabel->setPixmap(QPixmap::fromImage(sc_image.scaled(200,200,Qt::KeepAspectRatio,Qt::FastTransformation)));
   }//end if
   mainX = x_pos;
   mainY = y_pos;
   }//if img
};//magnify_state

void MainWindow::slotBrightness(int value)
{
   if(frame){
	  if (imDarkRadioButton->isChecked()) 
	      frame->setImgData(frame->CCDdata_im, (quint16)contrastSlider->value(), value,1,0);
	  else
	      frame->setImgData(frame->CCDdata, (quint16)contrastSlider->value(), value,1,0);
      if (img){
         int Len = (int)(frame->XSize*frame->YSize);
         img->loadFromData(frame->CCDImage,Len);
            if (scaleCheckBox->checkState() == Qt::Checked)
                      imageLabel->setPixmap(QPixmap::fromImage(img->scaled(scrollArea->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation)));
            else 
                     imageLabel->setPixmap(QPixmap::fromImage(img->copy(0,0,frame->XSize,frame->YSize)));
           imageLabel->adjustSize();
           scfactor = (double)frame->YSize/(double)imageLabel->height();
		   QMouseEvent event(QEvent::MouseButtonPress, QPoint::QPoint(curX,curY), Qt::LeftButton, 0, 0);
	       QApplication::sendEvent(imageLabel, &event);
      }//if img
   }//frame
}//slotBrightness

void MainWindow::slotContrast(int value)
{
   if(frame){
      //value = (int)(frame->MinV+(frame->MaxV-frame->MinV)*value/100);
	  if (imDarkRadioButton->isChecked()) 
	       frame->setImgData(frame->CCDdata_im, value,(quint16)brightnessSlider->value(),1,0);
	  else
	       frame->setImgData(frame->CCDdata, value, (quint16)brightnessSlider->value(),1,0);
      if (img){
         int Len = (int)(frame->XSize*frame->YSize);
         img->loadFromData(frame->CCDImage,Len);
            if (scaleCheckBox->checkState() == Qt::Checked)
                      imageLabel->setPixmap(QPixmap::fromImage(img->scaled(scrollArea->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation)));
            else 
                     imageLabel->setPixmap(QPixmap::fromImage(img->copy(0,0,frame->XSize,frame->YSize)));
           imageLabel->adjustSize();
           scfactor = (double)frame->YSize/(double)imageLabel->height();
		   QMouseEvent event(QEvent::MouseButtonPress, QPoint::QPoint(curX,curY), Qt::LeftButton, 0, 0);
	       QApplication::sendEvent(imageLabel, &event);
      }//if img
   }//frame
}//slotContrast


void MainWindow::update()
{  
   //utc = QDateTime::currentDateTime();
   //QTime timeUTC = utc.time();
   atGetUnpackedUT(&Year, &Month, &Day, &Hour, &Min, &Sec);
   QTime timeUTC((int)Hour,(int)Min, (int)floor(Sec));
   //timeUTC.
   DATEOBS currT;
   currT.year = Year; currT.month = Month; currT.day = Day;
   currT.hour = Hour; currT.min = Min; currT.sec = Sec;
   double s = getS(currT);
   if (s<0) s = s + 86400;
   if (s>86400) s = s - 86400;
   //////////////////////////////
   HA = s + dHA - raObj/15000;
   if (HA<0) HA = HA + 86400;
   if (HA>86400) HA = HA - 86400;
   double dtHA;
   QStringList tStrL;
   ///////////

   //////////////////////////////////
   DE = 3600*360 - (deObj/1000 + dDE);
   if (DE>=1296000) DE = DE - 1296000;
   /////////////////////////////
   sTimeLabel->setText("s = "+getStrFromS(s,":",0));
   QString deStr = getStrFromDE(DE,":");

   ////
   if(useTracker)
   {
       if(autoTimer->timerStarted)
       {
           dtHA = tHA-HA;
           if(!expStarted&&startWhenReady->isChecked()&&(dtHA<exposureTime->value()-5)&&autoTimer->ui_doneCheck->isChecked()) MakeExposureButtonClicked();
           if(dtHA<1)
           {
               //autoTimer->timerStarted = 0;
               if(autoTimer->ui_doneCheck->isChecked()) slotStartGuidProgram();
               slotAutoGuidStopped();
           }
           else
           {
               tStrL = getStrFromS(dtHA,":",0).split(":");
               if(tStrL.size()==3) autoTimer->ui_timeRemainLabel->setText(QString("%1:%2").arg(tStrL.at(0).toInt()*60+tStrL.at(1).toInt()).arg(tStrL.at(2).toInt()));
           }
           nAstrograph->setText("ha = "+getStrFromS(tHA,":",0)+"\n"+"de = "+deStr.left(deStr.lastIndexOf(":")));
       }
       else
       {
           nAstrograph->setText("ha = "+getStrFromS(HA,":",0)+"\n"+"de = "+deStr.left(deStr.lastIndexOf(":")));
       }

   }
   else nAstrograph->setText("ha = "+getStrFromS(HA,":",0)+"\n"+"de = "+deStr.left(deStr.lastIndexOf(":")));


   //nAstrograph->
  // nAstrograph->setFont(bFont);
  // nAstrograph->setText("ha = "+getStrFromS(HA,":",0)+"\n"+"de = "+deStr.left(deStr.lastIndexOf(":")));
   //QMessageBox::information(0,"debug",timeUTC.toString("hh:mm:ss"),QMessageBox::Ok,0,0);
   //if (dockTimePosWidget->isHidden())
    labelUTC->setText("UTC = "+timeUTC.toString("hh:mm:ss"));
	if (s2c->CamStarted)
	{
		s2c->GetTemperature(&T_ccd,&T_hot);
		QString StatusStr;
		if (exposure_started)
		{
			StatusStr+=  "frame = "+QString( "%1" ).arg(numOfFrame+1,4,10,QLatin1Char( ' ' ))+
			"| time =  "+ time_elepsed+" | ";
		}
		StatusStr+= "cold side =  "+ QString( "%1" ).arg(T_ccd,3,10,QLatin1Char( ' ' ));
		StatusStr = StatusStr +"| hot side =  "+ QString( "%1" ).arg(T_hot,3,10,QLatin1Char( ' ' ));
		statusBar()->showMessage(StatusStr);
	}
}

void MainWindow::cameraButtonClicked()
{
    statusBar()->showMessage(tr("camera is being initialized..."));
    s2c->s2cInitDll();
	if (s2c->CamStarted){ 
     setCamSettings();
     s2c->MakeCam();
     }
    statusBar()->showMessage(" ");

    if(!useWindowsUserNames) dockProp->show();
}//cameraButtonClicked


void MainWindow::MakeExposureButtonClicked()
{
     QTime ctime;
     int isAb = 0;
	 QDate cdate;
	 if (s2c->CamStarted){ 
             expStarted = 1;
	     MakeExposureButton->setEnabled(false);
		 stopExposureButton->setEnabled(true);
	     camTurnOff->setEnabled(false);
	     setCamSettings();
		 s2c->stopGettingImage=false;
		 exp_counter = 0;
		 exposure_started = true;
		 //===============
		 if (!noDarkRadioButton->isChecked())
		 {
		   if(dark_buff!=NULL) delete dark_buff;
		   dark_buff = new unsigned short[dark_buff_size];
		   double delta_t = expt_dark2 - expt_dark1;
		   double t_t1 = (exposureTime->value() + (double)spbShutterDelay->value()*0.002) - expt_dark1;
		   if (delta_t==0)
		   {
		     for (long i=0; i<dark_buff_size; i++)
			   {
			      dark_buff[i] = dark1_buff[i];
			   }
		   }
		   else
		   {
			   for (long i=0; i<dark_buff_size; i++)
			   {
			      dark_buff[i] = (unsigned short)(dark1_buff[i] + (double)(dark2_buff[i] - dark1_buff[i])*t_t1/delta_t);
			   }
		   }
		 }
		 //===============
		 if (chbUseShutter->checkState() == Qt::Checked) shutterCloseButtonClicked();
		 //=============
		s2c->Get_Image();
		frame_counter = 0;
		int frameCount = exposureCount->value();
		//date_obs = QDateTime::currentDateTime();
		atGetUnpackedUT(&Year, &Month, &Day, &Hour, &Min, &Sec);
		ctime = QTime::QTime((int)Hour,(int)Min, (int)floor(Sec), (int)(1000*(Sec-floor(Sec))));
		cdate = QDate::QDate(Year,Month,Day);
		date_obs.setDate(cdate);
		date_obs.setTime(ctime);
		////////////////////////////////
		double msek;
		int m_sek;
		int msekOpen = spbShutterDelay->value() + (int)(exposureTime->value()+ exptimecorr)*1000;
		int msekClose = msekOpen+ exptimecorr + (int)exposureTime->value()*1000;
		QString StatusStr;
		bool Shutter1 = true;
		bool Shutter2 = false;
		exp_msek.restart();
		while(frame_counter < frameCount)
		    {
			 m_sek = exp_msek.elapsed();
			 msek = (double)m_sek/1000;
			 if (chbUseShutter->checkState() == Qt::Checked)
			 {
			    if((m_sek>=msekOpen)&&(Shutter1)) 
				{
				  shutterOpenButtonClicked();
				  //date_obs = QDateTime::currentDateTime();
				    atGetUnpackedUT(&Year, &Month, &Day, &Hour, &Min, &Sec);
					ctime = QTime::QTime((int)Hour,(int)Min, (int)floor(Sec), (int)(1000*(Sec-floor(Sec))));
					cdate = QDate::QDate(Year,Month,Day);
					date_obs.setDate(cdate);
					date_obs.setTime(ctime);
					////////////////////////////////
				  Shutter1 = false;
				  Shutter2 = true;
				}
				if((m_sek>=msekClose)&&(Shutter2)) 
				{
				  //
				  atGetUnpackedUT(&Year, &Month, &Day, &Hour, &Min, &Sec);
				  ctime = QTime::QTime((int)Hour,(int)Min, (int)floor(Sec), (int)(1000*(Sec-floor(Sec))));
				  cdate = QDate::QDate(Year,Month,Day);
				  date_end.setDate(cdate);
				  date_end.setTime(ctime);
				  //
				  shutterCloseButtonClicked();
				  Shutter2 = false;
				}
			 }
			 //s2c->GetTemperature(&T_ccd,&T_hot);
			 //StatusStr = "frame = "+QString( "%1" ).arg(frame_counter+1,4,10,QLatin1Char( ' ' ));
			 //StatusStr = StatusStr +"; time =  "+ QString( "%1" ).arg( msek,7,'f',3,QLatin1Char( '0' ));
			 //StatusStr = StatusStr +"; cold side =  "+ QString( "%1" ).arg(T_ccd,3,10,QLatin1Char( ' ' ));
			 //StatusStr = StatusStr +"; hot side =  "+ QString( "%1" ).arg(T_hot,3,10,QLatin1Char( ' ' ));
			 //
			 numOfFrame = frame_counter;
			 if(frame_counter==0) time_elepsed = QString( "%1" ).arg( msek-(2*spbShutterDelay->value()/1000 + (int)exposureTime->value()),5,'f',1,QLatin1Char( ' ' ));
			 else time_elepsed = QString( "%1" ).arg( msek-msekOpen/1000,5,'f',1,QLatin1Char( ' ' ));
		     //statusBar()->showMessage(StatusStr);
			 //
		     QApplication::processEvents(QEventLoop::AllEvents);
		     //if(m_sek>=msekClose)
		     //{
			 if (s2c->Wait_Ready(0))
				 {
				    s2c->Read_Image(s2c->pReadImg,0);
				    Shutter1 = true;
		            Shutter2 = false;
				    msekOpen = spbShutterDelay->value();
		            msekClose = msekOpen +exptimecorr+ (int)exposureTime->value()*1000;
		            exp_msek.restart();
				    QApplication::processEvents(QEventLoop::AllEvents);
					slotShowExposedFrame();
					QApplication::processEvents(QEventLoop::AllEvents);
					if (chbSaveFits->checkState() == Qt::Checked)
                    {
					    QString FName = currentDir+"/"+fnameLineEdit->text()+QString( "%1" ).arg(frame_counter+1,4,10,QLatin1Char( '0' ))+".fit";
					    QString EqPos = radeLineEdit -> text();
						QStringList sl = EqPos.split(' ');
						frame->sRA =sl[0]+":"+sl[1]+":"+sl[2];
					    frame->sDE =sl[3]+":"+sl[4]+":"+sl[5];
					    frame->exptime = exposureTime -> value();
					    frame->cstemp = T_ccd; 
						frame->hstemp = T_hot;
					   // frame->sUTC = date_obs.toString(Qt::ISODate);
					   // frame->eUTC = date_end.toString(Qt::ISODate);
					    frame->sUTC = date_obs.toString("yyyy-MM-ddThh:mm:ss.zzz");
					    frame->eUTC = date_end.toString("yyyy-MM-ddThh:mm:ss.zzz");
					    frame->sTarget = targetLineEdit->text();
						frame->filterName = filterLineEdit->text();
						frame->observerNames = observersLineEdit->text();
						frame->airTemperature = sbAirTemperature->value();
						frame->focalLength = dsbFocalLength->value(); 
						frame->saveFitsFile(FName);
					}					
					QApplication::processEvents(QEventLoop::AllEvents);
					exp_counter = 0;
					frame_counter++;
					//Shutter1 = true;
		            //Shutter2 = false;
					//msekOpen = spbShutterDelay->value();
		            //msekClose = msekOpen + (int)exposureTime->value()*1000;
		            //exp_msek.restart();
				 }
			  //}
			 if (s2c->stopGettingImage) 
				{
				 QMessageBox::information(0,"debug",tr("exposure has been stopped by user."),QMessageBox::Ok,0,0);
				 //emit isStopped();
                                 isAb = 1;
				 break;

				}
			}
		 ///
                expStarted = 0;
		 s2c->Stop_Get_Image();
                 QApplication::beep();
                 if(autoStopTracker->isChecked()&&!isAb) slotStopGuidProgram();
                 printf("\a");
		 exposure_started = false;
		 MakeExposureButton->setEnabled(true);
	     camTurnOff->setEnabled(true);
		 stopExposureButton->setEnabled(false);
		 if (chbUseShutter->checkState() == Qt::Checked) shutterCloseButtonClicked();

                 if(mesEndExp&&!isAb)
                 {
                     dockTimePosWidget->show();
                     setFocus();
                     //wakeUpDlg wuDlg;
                     //wuDlg.setFocus();
                     //wuDlg.exec();
                 }
		 //QMouseEvent event(QEvent::MouseButtonPress, QPoint::QPoint(1,1), Qt::LeftButton, 0, 0);
		 //QApplication::sendEvent(imageLabel, &event);
     }//enf if s2c->CamStarted 	 
}//end of MakeExposureButtonClicked()


void MainWindow::camTurnOffClicked()
{
 camInitButton->setEnabled(true);
 //s2c->Stop_Get_Image();
 s2c->FreeDrv();
 s2c->CamStarted=false;
 camTurnOff->setEnabled(false);
 MakeExposureButton->setEnabled(false);
}//camTurnOffClicked

void MainWindow::applyButtonClicked()
{
  //QMessageBox::information(0,"debug","apply",QMessageBox::Ok,0,0);
  applyButton->setEnabled(false);
 if (s2c->CamStarted){ 
     setCamSettings();
     s2c->MakeCam();
     }//enf if s2c->CamStarted   
}//applyButtonClicked

void MainWindow::stopExposureButtonClicked()
{
  //QMessageBox::information(0,"debug","stop exposure",QMessageBox::Ok,0,0);
  s2c->stopGettingImage=true;
  exposure_started = false;
}//stopExposureButtonClicked

void MainWindow::closeEvent(QCloseEvent *event)
 {
    if (s2c->CamStarted)
	{
	  QMessageBox::information(0,"debug",tr("camera has not been turned off..."),QMessageBox::Ok,0,0);
	  event->ignore();
	}
    if(useTracker) trackerWidget->closePortButtonClicked();
    slotSaveDefaultPanels();
 } 

bool MainWindow::close()
{
	 //QMessageBox::information(0,"debug","close",QMessageBox::Ok,0,0);
	 atShutdown();//shut down AccuTime
     if (frame) delete frame;
     if (img) delete img;
     if (s2c->CamStarted) s2c->FreeDrv();	 
     //
     if(useTracker) trackerWidget->closePortButtonClicked();

     if (QMainWindow::close())
     return true;
     else
	 return false;
};//close()


void MainWindow::slotShowExposedFrame()
{
//QMessageBox::information(0,"debug","exposed",QMessageBox::Ok,0,0);
if (frame) {
             delete frame;
             delete img;
             } 
frame = new CCDFrame(s2c->pReadImg->pImg,*s2c->pParam->pSizeX,*s2c->pParam->pSizeY, spbOffset->value());
if (frame)
    {
	if (calcDarkRadioButton->isChecked())
    {
	    frame->applyDark(frame->CCDdata, dark_buff);
		if (calcFlatRadioButton->isChecked()) frame->applyFlat(frame->CCDdata, flat_buff);
		frame->getRange(frame->CCDdata);
		if (frame->maxHist<frame->offsetBC) leftB=frame->MinV; else leftB = frame->maxHist-frame->offsetBC;
		if (frame->maxHist+frame->offsetBC>frame->histsize) rightB=frame->MaxV; else rightB = frame->maxHist+frame->offsetBC;
        frame->setImgData(frame->CCDdata, leftB, rightB,1,0);
        QApplication::processEvents(QEventLoop::AllEvents);
	}
    if (imDarkRadioButton->isChecked())
    {
	    frame->setData_im();
	    frame->applyDark(frame->CCDdata_im, dark_buff);
		if (imFlatRadioButton->isChecked()) frame->applyFlat(frame->CCDdata_im, flat_buff);
		frame->getRange(frame->CCDdata_im);
		if (frame->maxHist<frame->offsetBC) leftB=frame->MinV; else leftB = frame->maxHist-frame->offsetBC;
		if (frame->maxHist+frame->offsetBC>frame->histsize) rightB=frame->MaxV; else rightB = frame->maxHist+frame->offsetBC;
        frame->setImgData(frame->CCDdata_im, leftB, rightB,1,0);
        QApplication::processEvents(QEventLoop::AllEvents);
	}
	if (noDarkRadioButton->isChecked())
    {
		frame->getRange(frame->CCDdata);
		if (frame->maxHist<frame->offsetBC) leftB=frame->MinV; else leftB = frame->maxHist-frame->offsetBC;
		if (frame->maxHist+frame->offsetBC>frame->histsize) rightB=frame->MaxV; else rightB = frame->maxHist+frame->offsetBC;
        frame->setImgData(frame->CCDdata, leftB, rightB,1,0);
        QApplication::processEvents(QEventLoop::AllEvents);
	}
	 ///////////////////////////////////////////
     img = new QImage(frame->CCDImage,frame->XSize,frame->YSize,QImage::Format_Indexed8);
     if (img->isNull()) QMessageBox::information(0,"debug",tr("image has not been loaded"),QMessageBox::Ok,0,0);
     img->setColorTable(frame->colormap);
	 QApplication::processEvents(QEventLoop::AllEvents);
      if (scaleCheckBox->checkState() == Qt::Checked)
              imageLabel->setPixmap(QPixmap::fromImage(img->scaled(scrollArea->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation)));
      else 
             imageLabel->setPixmap(QPixmap::fromImage(img->copy(0,0,frame->XSize,frame->YSize)));
      imageLabel->adjustSize();
      scfactor = (double)frame->YSize/(double)imageLabel->height();
	    contrastSlider->setRange(leftB,rightB);
		contrastSlider->setValue(leftB);
		brightnessSlider->setRange(leftB,rightB);
		brightnessSlider->setValue(rightB);
	   long hsize = frame->MaxV - frame->MinV + 1;
	   histX1 = 5+(int)(190*(double)(leftB - frame->MinV)/hsize);
	   histX2 = 5+(int)(190*(double)(rightB - frame->MinV)/hsize);
	   if (histX1<0) histX1 = 0;
	   updateHist(0);
	   QMouseEvent event(QEvent::MouseButtonPress, QPoint::QPoint(curX,curY), Qt::LeftButton, 0, 0);
       QApplication::sendEvent(imageLabel, &event);
	   QApplication::processEvents(QEventLoop::AllEvents);
     }//end if frame 
     
	 //s2cTh->quit();
}//slotShowExposedFrame

void MainWindow::slotCamStarted()
{
   if (s2c->CamStarted) 
	 {
	    //QMessageBox::information(0,"debug","turned on",QMessageBox::Ok,0,0);
		camTurnOff->setEnabled(true);
		camInitButton->setEnabled(false);
		MakeExposureButton->setEnabled(true);
		//s2cTh->quit();
	}//if camstarted
   if(useTracker)
   {
       trackerWidget->portName->setCurrentIndex(comPortNum-1);
       trackerWidget->openPortButtonClicked();
       trackerWidget->loadProgram(trackerCommand);
   }
};

void MainWindow::slotCamStopped()
{
   if (s2c->CamStarted) 
	 {
	    //QMessageBox::information(0,"debug","stop signal",QMessageBox::Ok,0,0);
		MakeExposureButton->setEnabled(true);
        camTurnOff->setEnabled(true);
	    stopExposureButton->setEnabled(false);
		//s2cTh->quit();
	}//if camstarted

   if(useTracker) trackerWidget->closePortButtonClicked();
};

void MainWindow::slotFinished()
{
//QMessageBox::information(0,"debug","finished",QMessageBox::Ok,0,0);
     MakeExposureButton->setEnabled(true);
     camTurnOff->setEnabled(true);
	 stopExposureButton->setEnabled(false);
	 //QMouseEvent event(QEvent::MouseButtonPress, QPoint::QPoint(1,1), Qt::LeftButton, 0, 0);
	 //QApplication::sendEvent(imageLabel, &event);
};

void MainWindow::camSettingsChanged(int state)
{
 if (s2c->CamStarted)
  applyButton->setEnabled(true);
};//camSettingsChanged
void MainWindow::camRSettingsChanged(bool state)
{
 if (s2c->CamStarted)
 applyButton->setEnabled(true);
};//camSettingsChanged

void MainWindow::slotSaveFitsFile()
{
  if (frame) 
   {
     QString FName = QFileDialog::getSaveFileName(
                    this,
                    tr("Enter file name"),
                    currentDir,
                    "fits files (*.fit)");
	QString EqPos = radeLineEdit -> text();
	QStringList sl = EqPos.split(' ');
	frame->sRA =sl[0]+":"+sl[1]+":"+sl[2];
    frame->sDE =sl[3]+":"+sl[4]+":"+sl[5];
    frame->exptime = exposureTime -> value();
    frame->cstemp = T_ccd; 
	frame->hstemp = T_hot;
    frame->sUTC = date_obs.toString(Qt::ISODate);
    frame->sTarget = targetLineEdit->text();
	 frame->saveFitsFile(FName);
   }

};

void MainWindow::slotCurrDirButtonClicked()
{
  currentDir = QFileDialog::getExistingDirectory(this, tr("Set Current Folder"),
                                                 currentDir,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
 currentDirLabel->setText(tr("CURRENT FOLDER = ")+currentDir);
 settings->beginGroup("general");
 settings->setValue("curr_dir", currentDir);	
 settings->endGroup(); 
};

void MainWindow::slotPropSetup()
{
 if (dockProp->isHidden()) 
                    {
                      dockProp->show();
                    }//end if
};

void MainWindow::slotDarkFlatSetup()
{
if (dockDarkFlat->isHidden()) 
                    {
                      dockDarkFlat->show();
                    }//end if
};

void MainWindow::slotLoadDarkFileButton1Clicked()
{
      QString FName = QFileDialog::getOpenFileName(
                    this,
                    tr("Choose a file for dark1"),
                    currentDarkDir,
                    "CCD (*.fit *.fits *.fts)");
	 if (FName!="")
	 {
	    QByteArray fn = FName.toAscii();
        char *fname = fn.data();
        fitsfile *fptr;
        int status = 0;
	    long naxes[2];
		long felem = 1;
		fits_open_file(&fptr, fname , READONLY,&status);
		fits_get_img_size(fptr, 2, naxes, &status);
		fits_read_key(fptr, TDOUBLE, "EXPTIME", &expt_dark1, NULL, &status);
		LONGLONG nelements = naxes[0]*naxes[1];
		int anynul = 0;
		if (dark1_buff!=NULL) delete dark1_buff;
		dark1_buff = new unsigned short[nelements];
		fits_read_img(fptr, TUSHORT, felem, nelements, NULL,dark1_buff ,&anynul, &status);
		fits_close_file(fptr, &status);
		darkFNLabel1->setText(FName);
		dark_buff_size = nelements;
	 }
   
};

void MainWindow::slotLoadDarkFileButton2Clicked()
{
     QString FName = QFileDialog::getOpenFileName(
                    this,
                    tr("Choose a file for dark2"),
                    currentDarkDir,
                    "CCD (*.fit *.fits *.fts)");
	if (FName!="")
	{
	    QByteArray fn = FName.toAscii();
        char *fname = fn.data();
        fitsfile *fptr;
        int status = 0;
	    long naxes[2];
		long felem = 1;
		fits_open_file(&fptr, fname , READONLY,&status);
		fits_get_img_size(fptr, 2, naxes, &status);
		fits_read_key(fptr, TDOUBLE, "EXPTIME", &expt_dark2, NULL, &status);
		LONGLONG nelements = naxes[0]*naxes[1];
		int anynul = 0;
		if (dark2_buff!=NULL) delete dark2_buff;
		dark2_buff = new unsigned short[nelements];
		fits_read_img(fptr, TUSHORT, felem, nelements, NULL,dark2_buff ,&anynul, &status);
		fits_close_file(fptr, &status);
		darkFNLabel2->setText(FName);
		dark_buff_size = nelements;
	}
};


void MainWindow::slotLoadFlatFileButtonClicked()
{
     QString FName = QFileDialog::getOpenFileName(
                    this,
                    tr("Choose a file for flat"),
                    currentFlatDir,
                    "CCD (*.fit *.fits *.fts)");
	 if (FName!="")
	 {
	    QByteArray fn = FName.toAscii();
        char *fname = fn.data();
        fitsfile *fptr;
        int status = 0;
	    long naxes[2];
		long felem = 1;
		fits_open_file(&fptr, fname , READONLY,&status);
		fits_get_img_size(fptr, 2, naxes, &status);
		LONGLONG nelements = naxes[0]*naxes[1];
		int anynul = 0;
		unsigned short *flatb = new unsigned short[nelements];
		fits_read_img(fptr, TUSHORT, felem, nelements, NULL, flatb ,&anynul, &status);
		fits_close_file(fptr, &status);
		//=============
		long i;
		double mflat = 0;
		for (i=0;i<nelements;i++)mflat = mflat+flatb[i];
		mflat = mflat/nelements;
		if (flat_buff!=NULL) delete flat_buff;
		flat_buff = new double[nelements];
		for (i=0;i<nelements;i++) flat_buff[i] = mflat/flatb[i];
		//=============
		delete flatb;
		flatFNLabel->setText(FName);
	 }

};




void MainWindow::slotCurrDarkDirButtonClicked()
{
currentDarkDir = QFileDialog::getExistingDirectory(this, tr("Set Current Dark Folder"),
                                                 currentDir,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
 currentDarkDirLabel->setText(tr("DARK FOLDER = ")+currentDarkDir);
 settings->beginGroup("general");
 settings->setValue("dark_dir", currentDarkDir);	
 settings->endGroup();
};



void MainWindow::slotCurrFlatDirButtonClicked()
{
currentFlatDir = QFileDialog::getExistingDirectory(this, tr("Set Current Flat Folder"),
                                                 currentDir,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
 currentFlatDirLabel->setText("FLAT FOLDER = "+currentFlatDir);
 settings->beginGroup("general");
 settings->setValue("flat_dir", currentFlatDir);	
 settings->endGroup();
};

void MainWindow::getEaButtonClicked()
{
   //QMessageBox::information(0,"debug",getRaDecMag(ea->value("RADEC").toString()),QMessageBox::Ok,0,0);
   radeLineEdit->setText(getRaDecMag(ea->value("RADEC").toString()));
   QString numb = ea->value("Num").toString();
   QString nameb = ea->value("Name").toString();
   nObj = numb.toInt();
   nameObj = nameb;
   typeObj = 1;
   QString comLine = radeLineEdit->text();
   comLine = comLine.trimmed();
   QStringList operands = comLine.split(" ");
   raObj = hms_to_mas(operands[0] +" "+ operands[1] +" "+ operands[2]," ");
   deObj = damas_to_mas(operands[3] +" "+ operands[4] +" "+ operands[5]," ");
   //
   //QMessageBox::information(0,"debug",QString( "%1" ).arg(nObj,4,10,QLatin1Char( ' ' )),QMessageBox::Ok,0,0);
   //targetLineEdit->setText(numb.trimmed()+" "+nameb.trimmed()+" "+operands[6]);
   targetLineEdit->setText(nameb.trimmed().replace(" ", "_"));
   //if((nameb.contains("J",Qt::CaseSensitive))&&(nObj==0))
//	   fnameLineEdit->setText(nameb.trimmed()+"a_");
  // else
       fnameLineEdit->setText(nameb.trimmed()+"_");
       double tmagn;
       if(useEaExp)
       {
           tmagn = operands.at(6).toDouble();
           if(tmagn<=16)
           {
               exposureTime->setValue(120);
               exposureCount->setValue(5);
           }
           else
           {
               exposureTime->setValue(60);
               exposureCount->setValue(10);
           }
       }
   
};

void MainWindow::slotAdoptButtonClicked()
{
   QString comLine = radeLineEdit->text();
   comLine = comLine.trimmed();
   QStringList operands = comLine.split(" ");
   raObj = hms_to_mas(operands[0] +" "+ operands[1] +" "+ operands[2]," ");
   deObj = damas_to_mas(operands[3] +" "+ operands[4] +" "+ operands[5]," ");
   /////////
   comLine = corrLineEdit->text();
   comLine = comLine.trimmed();
   operands.clear();
   operands = comLine.split(" ");
   QString str =operands[0]; 
   dHA = str.toDouble();
   str =operands[1];
   dDE = str.toDouble();
   ///////////////////////////
   settings->beginGroup("zero_points");
   settings->setValue("delta_ha", dHA);	
   settings->setValue("delta_de", dDE);
   settings->endGroup();
};


void MainWindow::setCamSettings()
{
  if (s2c->CamStarted)
  {
    ////////////////////////////////////////////////////////////////////	 
     if (lowRadioButton->isChecked()) *s2c->pParam->pFlFreq = 1; else *s2c->pParam->pFlFreq = 0;    //пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ; 1- High,0 - Low;
     *s2c->pParam->pY = frameX->value();//пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ
     *s2c->pParam->pX = frameY->value() + 23;
     *s2c->pParam->pSizeY = frameXSize->value();  //пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ
     *s2c->pParam->pSizeX = frameYSize->value();
     *s2c->pParam->pBinY = frameBinX->value()-1;  //пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
     *s2c->pParam->pBinX = frameBinY->value()-1;
     if (fastCheckBox->checkState()==Qt::Checked) *s2c->pParam->pFlClrFast = 1; else *s2c->pParam->pFlClrFast = 0;       //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ "пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ" пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ;
     if (slowCheckBox->checkState()==Qt::Checked) *s2c->pParam->pFlClrSlow = 1; else *s2c->pParam->pFlClrSlow = 0;      //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ "пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ" пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ;
     *s2c->pParam->pFrameCount = 1;     //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅ
     *s2c->pParam->pFlPause = 0;   //1- пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ, 0 - пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ.
     *s2c->pParam->pExpos =  exposureTime->value() + exptimecorr + (double)spbShutterDelay->value()*0.002;   //пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅ 
     if (coolCheckBox->checkState()==Qt::Checked) *s2c->pParam->pFlCool = 1; else  *s2c->pParam->pFlCool = 0;     // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ;
     if (fanCheckBox->checkState()==Qt::Checked) *s2c->pParam->pFlFan = 1; else *s2c->pParam->pFlFan = 0;      // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ;
     if (stabCheckBox->checkState()==Qt::Checked) *s2c->pParam->pFlCoolStab = 1; else *s2c->pParam->pFlCoolStab = 0; // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ;
     //*s2c->pParam->pFlLight = 0;
     *s2c->pParam->pFlSyncEn = 0;    // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ;
     *s2c->pParam->pFlSyncFrame = 1;// пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ; пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ = 1
     *s2c->pParam->pFlTest = 0;      // пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ. пїЅпїЅпїЅ == 1, пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ "пїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ"
     *s2c->pParam->pDigGain = spbDigGain->value(); // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ;
     if (chbDigGain->checkState()==Qt::Checked) *s2c->pParam->pDigOff = 1; else *s2c->pParam->pDigOff = 0; // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ;
     *s2c->pParam->pFlMapEn = 0;//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
     *s2c->pParam->pTemperC = coolingSpBox->value();//пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ;
     *s2c->pParam->pExposD = exposureDelay->value();      // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ;
	 //*s2c->pParam->pTEnter = 0;
     ///////////////////////////////////////////////////////////// 
     //s2cTh->s2c->pReadImg = new READIMG;
     s2c->pReadImg->FlSmear = 0;           //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ
     if (chbBL->checkState()==Qt::Checked) s2c->pReadImg->FlStab = 1; else  s2c->pReadImg->FlStab = 0;    //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
     s2c->pReadImg->FlEnterBack = 0;       //пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ
     s2c->pReadImg->FlSubBack = 0;         //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ
     s2c->pReadImg->FlBad = 0;             //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
     s2c->pReadImg->FlAuto = 0;            //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ Autoranging
     s2c->pReadImg->FlAutoMode = 0;        //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ "Auturanging" пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
     s2c->pReadImg->FlMap = 0;             //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
     s2c->pReadImg->CountIntSav = 0;       //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
	 s2c->pReadImg->CountInt = 0;
     s2c->pReadImg->SubOff = spbBL->value();           //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
      ///////////////////////////////////////////////////////////////////////////////
	 if (CCDBuff!=NULL)
	   {
	    //QMessageBox::information(0,"debug","buffers",QMessageBox::Ok,0,0);
	     delete [] CCDBuff;    
         delete [] CCDBuffBack;
         delete [] CCDBuffInt;
	   }//if s2cTh->s2c->pReadImg->pImg
	qint64 CCDBuffSize = (*s2c->pParam->pSizeX)*(*s2c->pParam->pSizeY)*sizeof(quint16);
    CCDBuff = new uchar[CCDBuffSize];    
    CCDBuffBack = new uchar[CCDBuffSize]; 
    CCDBuffInt = new unsigned[(*s2c->pParam->pSizeX)*(*s2c->pParam->pSizeY)]; //	
    s2c->pReadImg->pImg = CCDBuff;    //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ; пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ        
    s2c->pReadImg->pImgBack = CCDBuffBack; //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ; пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
    s2c->pReadImg->pImgInt = CCDBuffInt; //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ; пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
     //////////////////////////////////////////////////// 
  }
};

void MainWindow::updateHist(int pos)
{
     QPicture picture;
	 QPainter paint;
	 paint.begin(&picture);
	 QPen pen;
	 pen.setWidth(1);
	 QBrush brush(Qt::SolidPattern);
	 brush.setColor(Qt::black);
	 paint.setPen(pen);
	 //paint.setBrush(brush);
	 paint.fillRect(0,0,200,150, brush);
	 pen.setColor(Qt::yellow);
     paint.setPen(pen);
	 paint.drawRect(5,5,190,140);
     //paint.drawLine(0,0,200,150);
	 long hsize = frame->MaxV - frame->MinV + 1;
	 int x,y;
	 double maxH = log(frame->hist[frame->maxHist-frame->MinV]);
	 double lnH;
	 pen.setColor(Qt::white);
	 paint.setPen(pen);
	 for(long i = 0; i<hsize; i++)
	 {
        x = 5+(int)(190*(double)i/hsize);
		if (frame->hist[i]!=0) 
		{
		   lnH = log(frame->hist[i]);
           y = 145 - (int)(130*lnH/maxH);
		}
		else y = 145;
        paint.drawLine(x,145,x,y);		
	 }
	 //
	 pen.setColor(Qt::red);
	 paint.setPen(pen);
	 paint.drawLine(histX1,145,histX1,5);
	 paint.drawLine(histX2,145,histX2,5);
	 paint.end();
	 //
     histLabel->setPicture(picture);
     histLabel->adjustSize();
};

////////////////////////////////////////////////////////////////////////
//mainwindow events
//////////////////////////////////////////////////////////////////////////
void MainWindow::resizeEvent(QResizeEvent *event)
{
 if (img){
   if (scaleCheckBox->checkState() == Qt::Checked)
                      imageLabel->setPixmap(QPixmap::fromImage(img->scaled(scrollArea->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation)));
            else 
                     imageLabel->setPixmap(QPixmap::fromImage(img->copy(0,0,frame->XSize,frame->YSize)));
   imageLabel->adjustSize();
   scfactor = (double)(frame->YSize/(double)imageLabel->height());
   }//img
 QMainWindow::resizeEvent(event);
};



//events filter and events holders

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
   if (obj == dockScWidget) {
            if (event->type() == QEvent::Close) {
                //imageLabel->setCursor(Qt::ArrowCursor);
                return true;}
   }//dockScWidget
   if (obj == imageLabel) {
            if ((event->type() == QEvent::MouseMove)||(event->type() == QEvent::MouseButtonPress)) {
                //QMessageBox::information(0,"debug","image has not been loaded",QMessageBox::Ok,0,0);
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                imageLabelMouseMove(mouseEvent);
				//QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
                return true;
            } 
   }//imageLabel
   if (obj == scaledLabel) {
            if ((event->type() == QEvent::MouseMove)||(event->type() == QEvent::MouseButtonPress)) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                scaledLabelMouseMove(mouseEvent);
                return true;
            } 
   }//imageLabel
   if (obj == histLabel) {
            if (event->type() == QEvent::MouseButtonPress) 
			{
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
				if(mouseEvent->button()==Qt::LeftButton)
				{
				  histX1 = mouseEvent->x();
				  firstClick = false;
				}
				else
				{
				  histX2 = mouseEvent->x();
				  firstClick = true;
				}
				if (frame) 
				{
				   if(histX1<5)histX1 = 5;
				   if(histX2<5)histX2 = 5;
				   if(histX1>195)histX1 = 195;
				   if(histX2>195)histX2 = 195;
				   updateHist(0);
				   if (histX1<histX2)
				   {
					 leftB = (long)(frame->MinV + (double)(histX1 - 5)*(frame->MaxV - frame->MinV)/190);
					 rightB = (long)(frame->MinV + (double)(histX2 - 5)*(frame->MaxV - frame->MinV)/190);
				   }
				   else
				   {
					 leftB = (long)(frame->MinV + (double)(histX2 - 5)*(frame->MaxV - frame->MinV)/190);
					 rightB = (long)(frame->MinV + (double)(histX1 - 5)*(frame->MaxV - frame->MinV)/190);
				   }
				   contrastSlider->setRange(leftB,rightB);
				   brightnessSlider->setRange(leftB,rightB);
				   contrastSlider->setValue(leftB);
				   brightnessSlider->setValue(rightB);   
                }				
                return true;
            } 
   }//imageLabel
   return QMainWindow::eventFilter(obj, event);
};


void MainWindow::imageLabelMouseMove(QMouseEvent *mouseEvent)
{
   int x_pos = (int)(scfactor*mouseEvent->x());
   int y_pos = (int)(scfactor*mouseEvent->y());
   int scw = (int) 200/scspinBox->value();
   if ((x_pos>=0)&&(x_pos<frame->XSize)&&(y_pos>=0)&&(y_pos<frame->YSize)){
   QImage sc_image=img->copy(x_pos-scw/2,y_pos-scw/2,scw,scw);
   if (magnifyCheckBox->checkState() == Qt::Checked)
         scaledLabel->setPixmap(QPixmap::fromImage(sc_image.scaled(200,200,Qt::KeepAspectRatio,Qt::SmoothTransformation)));
   else
        scaledLabel->setPixmap(QPixmap::fromImage(sc_image.scaled(200,200,Qt::KeepAspectRatio,Qt::FastTransformation)));
   mainX = x_pos;
   mainY = y_pos;
   curX = mouseEvent->x();
   curY = mouseEvent->y();
   }//end if
};

void MainWindow::scaledLabelMouseMove(QMouseEvent *mouseEvent)
{
    if (frame){
     int scw = (int) 200/scspinBox->value(); 
     int x_pos = (int)(mouseEvent->x()/scspinBox->value())+mainX-scw/2;
     int y_pos = (int)(mouseEvent->y()/scspinBox->value())+mainY-scw/2;
     if ((x_pos>=0)&&(x_pos<frame->XSize)&&(y_pos>=0)&&(y_pos<frame->YSize))
     {
        quint16 valueI;
        if(imDarkRadioButton->isChecked()) 
		    valueI = frame->getValue(frame->CCDdata_im,(quint16)x_pos,(quint16)y_pos);
		else
		    valueI = frame->getValue(frame->CCDdata,(quint16)x_pos,(quint16)y_pos);
        QString sval;
        sval.setNum(x_pos);
        labelX->setText("X = "+sval);
        sval.setNum(y_pos);
        labelY->setText("Y = "+sval);
        sval.setNum(valueI);
        labelI->setText("I = "+sval);
     }
        else {
          labelX->setText("X = NULL");
          labelY->setText("Y = NULL");
          labelI->setText("I = NULL");
        }
     }//end if
};

//shutter

void MainWindow::shutterOpenButtonClicked()
{
 //SetOnOffDll(1);
 oup32(888, 9);
};

void MainWindow::shutterCloseButtonClicked()
{
  //SetOnOffDll(0);
  oup32(888, 6);
};


void MainWindow::slotShutterSetup()
{
if (dockShutter->isHidden()) 
                    {
                      dockShutter->show();
                    }//end if
};

void MainWindow::slotTrackerSetup()
{
if (dockTracker->isHidden())
                    {
                      dockTracker->show();
                    }//end if
};

void MainWindow::slotShowGuid()
{
    if (dockGuid->isHidden()) dockGuid->show();
}
void MainWindow::slotShowAutoGuid()
{
    if (dockAutoGuid->isHidden()) dockAutoGuid->show();
}

void MainWindow::slotdfOkButtonClicked()
{
	dockDarkFlat->hide();
};

void MainWindow::slotBegPropDlg()
{
    begPropDlg bpDlg;

    int i;
    int oSz = obsNames.size();
    bpDlg.ui_obsName->addItem("");
    for(i=0; i<oSz; i++) bpDlg.ui_obsName->addItem(obsNames[i]);
    oSz = filterNames.size();
    for(i=0; i<oSz; i++) bpDlg.ui_filtersBox->addItem(filterNames[i]);
    bpDlg.ui_filtersBox->setCurrentIndex(curFilter);

    if(bpDlg.exec()==QDialog::Rejected) close();
    observersLineEdit->setText(bpDlg.ui_obsName->currentText());
    filterLineEdit->setText(bpDlg.ui_filtersBox->currentText());
    dsbFocalLength->setValue(bpDlg.ui_focalBox->value());
    sbAirTemperature->setValue(bpDlg.ui_tempBox->value());
}

void MainWindow::slotUpdateTrackerState()
{
    if(useTracker)
    {
        //stateLabel->setText(trackerWidget->inputLabel->text());
        QString trStStr;
        switch(trackerWidget->keyState)
        {
        case 0:
            trStStr = QString(tr("Normal command execution"));
            break;
        case 1:
            trStStr = QString(tr("Emergency switch stop"));
            break;
        case 2:
            trStStr = QString(tr("Exec command code error"));
            break;
        case 3:
            trStStr = QString(tr("Exec command stopped"));
            break;
        case 4:
            trStStr = QString(tr("COM-port receiving error"));
            break;
        case 5:
            trStStr = QString(tr("Command error"));
            break;
        case 6:
            trStStr = QString(tr("Command data error"));
            break;
        default:
            trStStr = trackerWidget->inputLabel->text();
            break;
        }



        stateLog->append(trStStr+"\n");
        stateLog->moveCursor(QTextCursor::End);

        trStStr = "Program stopped";
        if(trackerWidget->isProg)trStStr = "Program started";
        stateLabel->setText(trStStr);
        stateLabel1->setText(trStStr);
        //startButton->setEnabled(trackerWidget->keyState!=0);
        //stopButton->setEnabled(trackerWidget->keyState==0);

        //stateLog->append();
    }
}

void MainWindow::slotStartGuidProgram()
{
    if(useTracker) trackerWidget->startProgram();
}

void MainWindow::slotStopGuidProgram()
{
    if(useTracker&&trackerWidget->isProg)
    {
        trackerWidget->stopProgram();
        slotUpdateTrackerState();
    }
}

void MainWindow::slotLoadDefaultPanels()
{
    QString fileName = QString("./default.pan");
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
  /*      QString msg = trUtf8("Ошибка при чтении %1\n%2")
                        .arg(fileName)
                        .arg(file.errorString());
        QMessageBox::warning(this, trUtf8("Ошибка"), msg);*/
        return;
    }

    uchar geo_size;
    QByteArray geo_data;
    QByteArray layout_data;

    bool ok = file.getChar((char*)&geo_size);
    if (ok) {
        geo_data = file.read(geo_size);
        ok = geo_data.size() == geo_size;
    }
    if (ok) {
        layout_data = file.readAll();
        ok = layout_data.size() > 0;
    }

    if (ok)
        ok = restoreGeometry(geo_data);
    if (ok)
        ok = restoreState(layout_data);

    if (!ok) {
  //      QString msg = trUtf8("Ошибка при чтении %1")
       //                 .arg(fileName);
  //      QMessageBox::warning(this, trUtf8("Ошибка"), msg);
        qDebug() << QString("default.pan read error\n");
        return;
    }
}

void MainWindow::slotSaveDefaultPanels()
{
    qDebug() << "save Defauls Panel\n";
    QString DirToSaveAs = QDir::homePath();
    QString fileName = QString("./default.pan");

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QString msg = trUtf8("Ошибка при открытии файла %1\n%2")
                        .arg(fileName)
                        .arg(file.errorString());
        /*QMessageBox::warning(this, trUtf8("Ошибка"), msg);*/
        qDebug() << msg +"\n";
        return;
    }

    QByteArray geo_data = saveGeometry();
    QByteArray layout_data = saveState();

    bool ok = file.putChar((uchar)geo_data.size());
    if (ok)
        ok = file.write(geo_data) == geo_data.size();
    if (ok)
        ok = file.write(layout_data) == layout_data.size();

    if (!ok) {
    /*    QString msg = trUtf8("Ошибка записи в %1\n%2")
                        .arg(fileName)
                        .arg(file.errorString());
        QMessageBox::warning(this, trUtf8("Ошибка"), msg);*/
        qDebug() << QString("default.pan write error\n");
        return;
    }
    qDebug() << "Defauls Panel saved\n";
}

void MainWindow::slotSavePanels()
{
    QString DirToSaveAs = QDir::homePath();
    qDebug() << QString("savePanels to %1\n").arg(DirToSaveAs);
    QString fileName = QFileDialog::getSaveFileName(this, trUtf8("Save panels positions as..."),DirToSaveAs,trUtf8( "Panels param (*.pan)"));
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QString msg = trUtf8("Error while open file %1\n%2")
                        .arg(fileName)
                        .arg(file.errorString());
        QMessageBox::warning(this, trUtf8("Error"), msg);
        return;
    }

    QByteArray geo_data = saveGeometry();
    QByteArray layout_data = saveState();

    bool ok = file.putChar((uchar)geo_data.size());
    if (ok)
        ok = file.write(geo_data) == geo_data.size();
    if (ok)
        ok = file.write(layout_data) == layout_data.size();

    if (!ok) {
        QString msg = trUtf8("Write error in %1\n%2")
                        .arg(fileName)
                        .arg(file.errorString());
        QMessageBox::warning(this, trUtf8("error"), msg);
        return;
    }
}

void MainWindow::slotLoadPanels()
{
    QString fileName
        = QFileDialog::getOpenFileName(this, tr("Load panels positions..."),("/"),"Panels param (*.pan)");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QString msg = QString("Error while reading %1\n%2")
                        .arg(fileName)
                        .arg(file.errorString());
        QMessageBox::warning(this, "Error", msg);
        return;
    }

    uchar geo_size;
    QByteArray geo_data;
    QByteArray layout_data;

    bool ok = file.getChar((char*)&geo_size);
    if (ok) {
        geo_data = file.read(geo_size);
        ok = geo_data.size() == geo_size;
    }
    if (ok) {
        layout_data = file.readAll();
        ok = layout_data.size() > 0;
    }

    if (ok)
        ok = restoreGeometry(geo_data);
    if (ok)
        ok = restoreState(layout_data);

    if (!ok) {
        QString msg = trUtf8("Error while reading %1")
                        .arg(fileName);
        QMessageBox::warning(this, trUtf8("Error"), msg);
        return;
    }
}

void MainWindow::slotAutoGuidStarted()
{
    if(useTracker)
    {
        autoTimer->timerStarted = 1;
        autoTimer->ui_doneCheck->setChecked(0);
        autoTimer->show();
        //targetHA
        //QStringList haStrList;
        tHA = HA+timeDelay->value()*60.0;
        //qDebug() << QString("tHA= ")
        tHA = (floor(tHA/60.0) + 1)*60.0;
        //haStrList = getStrFromS(HA,":",0).split(":");
        //int
    }
}

void MainWindow::slotAutoGuidStopped()
{
    if(useTracker)
    {
        autoTimer->timerStarted = 0;
        autoTimer->ui_timeRemainLabel->setText("STOPED");
        autoTimer->hide();
    }
}
