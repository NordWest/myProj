#include <QtGui>
#include "mainwindow.h"
#include "math.h"
#include ".\..\libs\astro.h"



MainWindow::MainWindow()
{
	QApplication::setStyle(new QPlastiqueStyle);
    setWindowTitle ("astrored2");
    setupMainMenu();
    setupWidgets();
    imageLabel->installEventFilter(this);
    scaledLabel->installEventFilter(this);
    dockScWidget->installEventFilter(this);
    dockKeyWidget->installEventFilter(this);
	histLabel->installEventFilter(this);	
    this -> installEventFilter(this);	
    scfactor = 1;
	img_status = 0;
    mainX=0;
    mainY=0;
	currX = 0;
	currY = 0;
	deltaX =0;
	deltaY =0;
	//======================================
	firstClick = true;
	histX1 = 5;
	histX2 = 195;
	fX = 0;
	fY = 0;
	fL = 0;
	fN = 0;
	currDir = "";
	//========================================
	connect(this, SIGNAL(signalUpdateImg()), this, SLOT(slotUpdateImg()),Qt::QueuedConnection);
	ccdThread = new ccdthread(&imgF, prBar, this);
	ccdThread->initCCDThread(&vRes);
	//QMessageBox::information(0,"astrored2","Probably, Charon have not been found!",QMessageBox::Ok,0,0);
	//color map
	colormap.resize(256);
    for (quint16 j=0; j<256;j++) colormap[j]=qRgb(j,j,j);
    /*
	settings = new QSettings(QSettings::IniFormat,QSettings::SystemScope,"CAO RAS","qtccd");
	 s2cTh->s2c->ccd_ignore = settings->value("ccd/ccd_ignore").toInt();
	 s2cTh->s2c->adapter_type = settings->value("ccd/adapter_type").toInt();
	 s2cTh->s2c->adapter_slot = (unsigned) settings->value("ccd/adapter_slot").toInt();
	 s2cTh->s2c->video_size = (unsigned) settings->value("ccd/video_size").toInt();
	//settings->setPath(QSettings::IniFormat,QSettings::SystemScope,"C:/QT/myQTprojects/qtccd/release");
	
	settings->beginGroup("general");
	settings->setValue("qtccd_path", "C:/QT/myQTprojects/qtccd/release");
	settings->endGroup();
	settings->beginGroup("ccd");
	settings->setValue("ccd_ignore", 0);
	settings->setValue("adapter_type", 1);
	settings->setValue("adapter_slot", 0);
	settings->setValue("video_size", 2000);
	settings->endGroup();
           */
	settings = new QSettings("conf/conf.ini",
                    QSettings::IniFormat);
	currDir = settings->value("general/curr_dir").toString();
	currDirLabel->setText("Current folder = "+currDir);
	imgF.ucac2_path = settings->value("catalogs/ucac2_path").toString();
	ucacDirLabel->setText("ucac2 folder = "+imgF.ucac2_path);
	imgF.log_path = settings->value("general/log_path").toString();
	logDirLabel->setText("log folder = "+imgF.log_path);
	//
	QString telescopes = settings->value("telescopes/telescope_list").toString();
	QStringList telList = telescopes.split("|");
	comboTel->addItems(telList);
	actTel = settings->value("telescopes/activ_telescope").toInt();
	comboTel->setCurrentIndex(actTel);
	//QString str;
	//str.setNum(actTel);
	//QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
	switch(actTel)
		 {
		   case 0://NA
				  {
				    flSpinBox->setValue(3467);
					xpsSpinBox->setValue(16);
					ypsSpinBox->setValue(16);
					fovSpinBox->setValue(25);
					rotAngSpinBox->setValue(0);
				    obsCode->setText("084");
				  break;
				  }
		   case 1://FTN
		          {
				    flSpinBox->setValue(20000);
					xpsSpinBox->setValue(27);
					ypsSpinBox->setValue(27);
					fovSpinBox->setValue(10);
					rotAngSpinBox->setValue(180);
					obsCode->setText("F65");
				  break;
		          }
		 }
	imgF.flength = flSpinBox->value();
	imgF.xpsize  = xpsSpinBox->value();
	imgF.ypsize  = ypsSpinBox->value();
	imgF.fov  = fovSpinBox->value()*60000;
    rot_angle = rotAngSpinBox->value()*atan(1)*4/180;		 
    //settings->beginGroup("general");
    //settings->setValue("curr_dir", "C:/QT/myQTprojects/astrored2/release");	
    //settings->endGroup();
	///////////////////////////////////////////////////////////////
	http = new QHttp(this);
    connect(http, SIGNAL(done(bool)), this, SLOT(progressDataFromHttp()));
	imgF.obs_code = obsCode->text();
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
	 saveResultsAct = new QAction(tr("&save results"), this);
        saveResultsAct->setShortcut(tr("Ctrl+Q"));
        saveResultsAct->setStatusTip(tr("save results"));
     fileMenu->addAction(saveResultsAct);
     connect(saveResultsAct, SIGNAL(triggered()), this, SLOT(slotSaveResults()));
	 //
	 saveAsJPG = new QAction(tr("&save as *.jpg"), this);
        saveAsJPG->setShortcut(tr("Ctrl+J"));
        saveAsJPG->setStatusTip(tr("save as *.jpg"));
     fileMenu->addAction(saveAsJPG);
     connect(saveAsJPG, SIGNAL(triggered()), this, SLOT(slotSaveAsJPG()));
	 //
     exitAct = new QAction(tr("&exit"), this);
        exitAct->setShortcut(tr("Ctrl+X"));
        exitAct->setStatusTip(tr("exit"));
     fileMenu->addAction(exitAct);
     connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
     //END menu file
     
    //BEGIN menu tools
    toolsMenu = menuBar()->addMenu(tr("tools"));
	 markingAct = new QAction(tr("&marking panel"), this);
        markingAct->setShortcut(tr("Ctrl+M"));
        markingAct->setStatusTip(tr("marking panel"));
     toolsMenu->addAction(markingAct);
     connect(markingAct, SIGNAL(triggered()), this, SLOT(marking()));
	 //
	 psfAct = new QAction(tr("&psf panel"), this);
        psfAct->setShortcut(tr("Ctrl+T"));
        psfAct->setStatusTip(tr("psf panel"));
     toolsMenu->addAction(psfAct);
     connect(psfAct, SIGNAL(triggered()), this, SLOT(psfPanel()));
	 //
	 framesAct = new QAction(tr("&frames panel"), this);
        framesAct->setShortcut(tr("Ctrl+F"));
        framesAct->setStatusTip(tr("frames panel"));
     toolsMenu->addAction(framesAct);
     connect(framesAct, SIGNAL(triggered()), this, SLOT(slotFramesAct()));
	 //
	 telnetAct = new QAction(tr("&telnet panel"), this);
        telnetAct->setShortcut(tr("Ctrl+I"));
        telnetAct->setStatusTip(tr("telnet panel"));
     toolsMenu->addAction(telnetAct);
     connect(telnetAct, SIGNAL(triggered()), this, SLOT(slotTelnetAct()));
	 //
	 propAct = new QAction(tr("&properties panel"), this);
        propAct->setShortcut(tr("Ctrl+P"));
        propAct->setStatusTip(tr("properties panel"));
     toolsMenu->addAction(propAct);
     connect(propAct, SIGNAL(triggered()), this, SLOT(slotPropAct()));
	 //
	 //BEGIN menu view
    viewMenu = menuBar()->addMenu(tr("view"));
	magnifyAct = new QAction(tr("&view panel"), this);
        magnifyAct->setShortcut(tr("Ctrl+V"));
        magnifyAct->setStatusTip(tr("view panel"));
     viewMenu->addAction(magnifyAct);
     connect(magnifyAct, SIGNAL(triggered()), this, SLOT(magnify()));
	 //
	histAct = new QAction(tr("&histogram"), this);
        histAct->setShortcut(tr("Ctrl+G"));
        histAct->setStatusTip(tr("histogram"));
     viewMenu->addAction(histAct);
     connect(histAct, SIGNAL(triggered()), this, SLOT(slotShowHist()));
	 //
     progressAct = new QAction(tr("&progress window"), this);
        progressAct->setShortcut(tr("Ctrl+W"));
        progressAct->setStatusTip(tr("progress window"));
     viewMenu->addAction(progressAct);
     connect(progressAct, SIGNAL(triggered()), this, SLOT(slotProgressAct()));
	 //
	 keyAct = new QAction(tr("&FITS header"), this);
        keyAct->setShortcut(tr("Ctrl+H"));
        keyAct->setStatusTip(tr("FITS header"));
     viewMenu->addAction(keyAct);
     connect(keyAct, SIGNAL(triggered()), this, SLOT(slotKeyAct()));
     //
	 resAct = new QAction(tr("&results"), this);
        resAct->setShortcut(tr("Ctrl+R"));
        resAct->setStatusTip(tr("results"));
     viewMenu->addAction(resAct);
     connect(resAct, SIGNAL(triggered()), this, SLOT(slotResAct()));
	 /*measureAct = new QAction(tr("&measure panel"), this);
        measureAct->setShortcut(tr("Ctrl+M"));
        measureAct->setStatusTip(tr("measure panel"));
     toolsMenu->addAction(measureAct);
     connect(measureAct, SIGNAL(triggered()), this, SLOT(dockMeasureWidgetShow()));*/
     //END menu tools
};

void MainWindow::setupWidgets()
{
     imageLabel = new QLabel;
     imageLabel->setBackgroundRole(QPalette::Base);
     imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
     imageLabel->setScaledContents(true);
	 imageLabel->setCursor(Qt::CrossCursor);
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
	 
     
     scspinBox = new QSpinBox;
     scspinBox->setRange(1,10);
     //scspinBox->setFixedWidth(70);
     scspinBox->setPrefix(tr("magnify = "));
     connect(scspinBox, SIGNAL(valueChanged(int)), this, SLOT(magnify_state(int)));
	 
	 aperture_spinBox = new QSpinBox;
     aperture_spinBox->setRange(3,40);
	 aperture_spinBox->setValue(10);
     //aperture_spinBox->setFixedWidth(130);
     aperture_spinBox->setPrefix(tr("aperture radius = "));
	 connect(aperture_spinBox, SIGNAL(valueChanged(int)), this, SLOT(slotUpdateAperture(int)));
     
     scaleCheckBox=new QCheckBox(tr("scaling"));
     scaleCheckBox->setCheckState(Qt::Checked);
     connect(scaleCheckBox, SIGNAL(stateChanged(int)), this, SLOT(scale_state(int)));
     
     magnifyCheckBox=new QCheckBox(tr("smoothing"));
     connect(magnifyCheckBox, SIGNAL(stateChanged(int)), this, SLOT(magnify_state(int)));
	 
	 apertureCheckBox=new QCheckBox(tr("aperture"));
	 connect(apertureCheckBox, SIGNAL(stateChanged(int)), this, SLOT(slotUpdateAperture(int)));
	 
	 profileCheckBox=new QCheckBox(tr("profile"));
	 connect(profileCheckBox, SIGNAL(stateChanged(int)), this, SLOT(slotUpdateProfile(int)));
	 
	 psfCheckBox=new QCheckBox(tr("psf"));
	 connect(psfCheckBox, SIGNAL(stateChanged(int)), this, SLOT(slotUpdateProfile(int)));
	 
	 invCheckBox=new QCheckBox(tr("invert image"));
	 connect(invCheckBox, SIGNAL(stateChanged(int)), this, SLOT(slotInvChanged(int)));
	 
	 leftButton = new QPushButton("<<");
	 connect(leftButton, SIGNAL(clicked()), this, SLOT(slotLeftButtonClicked()));
	 
	 rightButton = new QPushButton(">>");
	 connect(rightButton, SIGNAL(clicked()), this, SLOT(slotRightButtonClicked()));
	 //fullrangeCheckBox=new QCheckBox(tr("full range"));
     
     gridWidget = new QWidget;
     grid = new QGridLayout(gridWidget);
	 grid->addWidget(apertureCheckBox,0,0);
     grid->addWidget(profileCheckBox,0,1);
	 grid->addWidget(psfCheckBox,0,2);
     grid->addWidget(scaleCheckBox,1,0);
     grid->addWidget(magnifyCheckBox,1,1);
	 grid->addWidget(invCheckBox,1,2);
     contrastSlider = new QSlider(Qt::Horizontal,gridWidget);
     contrastSlider->setMinimum(0);
     contrastSlider->setMaximum(100);
     contrastSlider->setValue(50);
     contrastSlider->setSingleStep(1);
     contrastSlider->setPageStep(10);
     contrastSlider->setTracking(false);
     brightnessSlider = new QSlider(Qt::Horizontal,gridWidget);
     brightnessSlider->setMinimum(0);
     brightnessSlider->setMaximum(100);
     brightnessSlider->setValue(50);
     brightnessSlider->setSingleStep(1);
     brightnessSlider->setPageStep(10);
     brightnessSlider->setTracking(false);
     connect(contrastSlider, SIGNAL(valueChanged(int)), this, SLOT(slotContrast(int)));
     connect(brightnessSlider, SIGNAL(valueChanged(int)), this, SLOT(slotBrightness(int)));
     grid->addWidget(contrastSlider,2,0);
     grid->addWidget(brightnessSlider,2,1);
     labelBackground = new QLabel(tr("background"));
     labelDiapason = new QLabel(tr("diapason"));
     grid->addWidget(labelBackground,3,0);
     grid->addWidget(labelDiapason,3,1);
	 //////////////////////////////////////////////
	 gammaSlider = new QSlider(Qt::Horizontal,gridWidget);
     gammaSlider->setMinimum(0);
     gammaSlider->setMaximum(100);
     gammaSlider->setValue(0);
     gammaSlider->setSingleStep(5);
     gammaSlider->setPageStep(10);
     gammaSlider->setTracking(false);
     connect(gammaSlider, SIGNAL(valueChanged(int)), this, SLOT(slotGamma(int)));
	 ////////////////////////////////////////////////
     grid->addWidget(gammaSlider,2,2);
	 QLabel *labelGamma = new QLabel (tr("gamma"));
	 grid->addWidget(labelGamma,3,2);
	 //////////////////////////////////
	 //restoreButton = new QPushButton(tr("adopt full range"));
	 //connect(restoreButton, SIGNAL(clicked()), this, SLOT(slotRestoreButtonClicked()));
	 //grid->addWidget(restoreButton,3,0);
	 ////////////////////////////////////
	 cofmButton = new QPushButton(tr("center of mass"));
	 connect(cofmButton, SIGNAL(clicked()), this, SLOT(slotCofmButtonClicked()));
	 grid->addWidget(cofmButton,4,1);
	 /////////////////////////////////////////////////
	 boxButton = new QPushButton(tr("box data"));
	 connect(boxButton, SIGNAL(clicked()), this, SLOT(slotBoxButtonClicked()));
	 grid->addWidget(boxButton,4,2);
	 ///////////////////////////////////
	 infoButton = new QPushButton(tr("info"));
	 connect(infoButton, SIGNAL(clicked()), this, SLOT(slotInfoButtonClicked()));
	 grid->addWidget(infoButton,4,0);
	 /////////////////////////////////////////////////
	 centerButton = new QPushButton(tr("centering"));
	 connect(centerButton, SIGNAL(clicked()), this, SLOT(slotCenterButtonClicked()));
	 grid->addWidget(centerButton,5,0);
	 ///////////////////////////////////
	 /////////////////////////////////////////////////
	 psfButton = new QPushButton(tr("PSF"));
	 connect(psfButton, SIGNAL(clicked()), this, SLOT(slotPsfButtonClicked()));
	 grid->addWidget(psfButton,5,1);
	 ///////////////////////////////////
	 subButton = new QPushButton(tr("subtract PSF"));
	 connect(subButton, SIGNAL(clicked()), this, SLOT(slotSubButtonClicked()));
	 grid->addWidget(subButton,5,2);
	 ///////////////////////////////////
	 statusSW = new QStatusBar();
	 //////////////////////////////////////
	 QWidget *apertWidget = new QWidget;
     QGridLayout *apertGrid = new QGridLayout(apertWidget);
	 apertGrid->addWidget(scspinBox,0,0);
	 apertGrid->addWidget(aperture_spinBox,0,1);
	 apertGrid->addWidget(leftButton,1,0);
	 apertGrid->addWidget(rightButton,1,1);
	 ////////////////////////////////////
     boxScLayout = new QBoxLayout(QBoxLayout::TopToBottom,magnifyWidget);
     boxScLayout->addWidget(scaledLabel);
     boxScLayout->addWidget(apertWidget);
     boxScLayout->addWidget(gridWidget);
	 boxScLayout->addWidget(statusSW);
	 //boxScLayout->addWidget(histLabel);
	 //boxScLayout->addWidget(gammaSpinBox);fullrangeCheckBox
          
     dockScWidget = new QDockWidget(tr("view panel"),this,Qt::Widget);
     dockScWidget->setMinimumSize(250,450);
     dockScWidget->setWidget(magnifyWidget);
     addDockWidget(Qt::LeftDockWidgetArea,dockScWidget);
     //dockScWidget->setFloating(true);
     //dockScWidget->hide();
     
     /////////////////////////////////////checkStarCheckBox;
	  QWidget *markWidget = new QWidget;
	   centeringCheckBox=new QCheckBox(tr("centering"));
	   checkObjCheckBox=new QCheckBox(tr("check object"));
	   recompCheckBox=new QCheckBox(tr("recalculation of marked objects"));
	   clearButton = new QPushButton(tr("clear marks"));
	   connect(clearButton, SIGNAL(clicked()), this, SLOT(slotClearButtonClicked()));
	   mfcButton = new QPushButton(tr("marks from catalog"));
	   connect(mfcButton, SIGNAL(clicked()), this, SLOT(slotMfcButtonClicked()));
	   ephButton = new QPushButton(tr("get EPH"));
	   connect(ephButton, SIGNAL(clicked()), this, SLOT(slotEphButtonClicked()));
	   //+++
	   QLabel *radeLabel = new QLabel("RA and DEC:");
	   radeLineEdit = new QLineEdit;
	   radeLineEdit -> setInputMask("99 99 99.999 #99 99 99.99 99.9 99.9");
	   radeLineEdit -> setText ("00 00 00.000 +00 00 00.00 09.0 15.0");
	   radeLineEdit -> setFixedWidth(200);
	   ///////////////////////////////////////////
	   sbAstNum = new QSpinBox;
       sbAstNum->setRange(1,200000);
       sbAstNum->setFixedWidth(150);
	   sbAstNum->setValue(1);
       sbAstNum->setPrefix(tr("AstNum = "));
	   ///////////////////////////
	   
	   mpcObjLineEdit = new QLineEdit;
	   mpcObjLineEdit -> setFixedWidth(100);
	   ///////////////////////////
	   lwObjList = new QListWidget;
	   lwObjList->setFixedSize(200,50);
	   clearObjButton = new QPushButton(tr("clear"));
	   connect(clearObjButton, SIGNAL(clicked()), this, SLOT(slotClearObjButtonClicked()));
	   toClipboardButton = new QPushButton(tr("to clipboard"));
	   connect(toClipboardButton, SIGNAL(clicked()), this, SLOT(slotToClipboardButtonClicked()));
	   fromClipboardButton = new QPushButton(tr("from clipboard"));
	   connect(fromClipboardButton, SIGNAL(clicked()), this, SLOT(slotFromClipboardButtonClicked()));
       /////////////////////////////////////////////////////////////moveCheckBox;
	   moveCheckBox=new QCheckBox(tr("move marks"));
	   setOcCheckBox=new QCheckBox(tr("set optical center"));
	   connect(moveCheckBox, SIGNAL(stateChanged ( int )), this, SLOT(slotMoveCheckBoxStateChanged(int)));
	   rbMoveAll = new QRadioButton(tr("move all"));
	   rbMoveSel = new QRadioButton(tr("move selected"));
	   rbMoveAll -> setChecked(true);
	   QWidget *mvWidget = new QWidget;
	   QGroupBox *mvBox = new QGroupBox(tr("moving type"),mvWidget);
       QBoxLayout *mvLayout = new QBoxLayout(QBoxLayout::LeftToRight,mvBox);
	   mvLayout->addWidget(moveCheckBox);
       mvLayout->addWidget(rbMoveAll);
       mvLayout->addWidget(rbMoveSel);
	   mvLayout->addWidget(setOcCheckBox);
	   mvWidget->setFixedSize(5*mvBox->width(),2*mvBox->height());
       //
	   QWidget *posmWidget = new QWidget;
	   QGroupBox *posmBox = new QGroupBox(tr("set position"),posmWidget);
       QBoxLayout *posmLayout = new QBoxLayout(QBoxLayout::LeftToRight,posmBox);
	   posmLayout->addWidget(radeLabel);
       posmLayout->addWidget(radeLineEdit);
       posmLayout->addWidget(mfcButton);
	   posmWidget->setFixedSize(6*posmBox->width(),2*posmBox->height());
       //
	   QWidget *targWidget = new QWidget;
	   QGroupBox *targBox = new QGroupBox(tr("set target"),targWidget);
       QGridLayout *targLayout = new QGridLayout(targBox);
	   targLayout->addWidget(lwObjList,0,0);
	   targLayout->addWidget(sbAstNum,0,1);
	   targLayout->addWidget(mpcObjLineEdit,0,2);
	   targLayout->addWidget(ephButton,1,0);
	   targLayout->addWidget(clearObjButton,1,1);
	   targLayout->addWidget(toClipboardButton,1,2);
	   targLayout->addWidget(fromClipboardButton,1,3);
	   targWidget->setFixedSize(6*targBox->width(),4*targBox->height());
	   //
	   QWidget *mcontrolWidget = new QWidget;
	   QGroupBox *mcontrolBox = new QGroupBox(tr("marks"),mcontrolWidget);
       QBoxLayout *mcontrolLayout = new QBoxLayout(QBoxLayout::LeftToRight,mcontrolBox);
	   mcontrolLayout->addWidget(centeringCheckBox);
       mcontrolLayout->addWidget(checkObjCheckBox);
       mcontrolLayout->addWidget(recompCheckBox);
	   mcontrolLayout->addWidget(clearButton);
	   mcontrolWidget->setFixedSize(5*mcontrolBox->width(),2*mcontrolBox->height());
       //
	   //
       QGridLayout *markGrid = new QGridLayout(markWidget);
       markGrid->addWidget(mcontrolWidget,0,0);
	   markGrid->addWidget(posmWidget,1,0);
	   markGrid->addWidget(targWidget,2,0);
	   markGrid->addWidget(mvWidget,3,0);
	 dockMarkWidget = new QDockWidget(tr("marking panel"),this,Qt::Widget);
     dockMarkWidget->setWidget(markWidget);
	 dockMarkWidget->setFloating(true);
     addDockWidget(Qt::BottomDockWidgetArea,dockMarkWidget);
	 dockMarkWidget->hide();
     ///////////////////////////////////////////
	  QWidget *psfWidget = new QWidget;
	   lorentzRadioButton = new QRadioButton(tr("lorentzian"));
	   gaussianRadioButton = new QRadioButton(tr("gaussian"));
	   moffatRadioButton = new QRadioButton(tr("moffat"));
	   lorentzRadioButton -> setChecked(true);
	   orderSpinBox = new QDoubleSpinBox;
	   orderSpinBox -> setDecimals(1);
	   orderSpinBox -> setPrefix (tr("degree of the model = "));
	   orderSpinBox -> setRange(1.0,4.0);
	   orderSpinBox -> setSingleStep (0.1);
	   orderSpinBox -> setValue(1.2);
	   //
	   QWidget *gbWidget = new QWidget;
	   QGroupBox *mBox = new QGroupBox(tr("psf model"),gbWidget);
       QBoxLayout *mLayout = new QBoxLayout(QBoxLayout::LeftToRight,mBox);
       mLayout->addWidget(lorentzRadioButton);
       mLayout->addWidget(gaussianRadioButton);
	   mLayout->addWidget(moffatRadioButton);
	   gbWidget->setFixedSize ( 3*mBox->width(),2*mBox->height());
       //
	   inSpinBox = new QSpinBox;
	   inSpinBox -> setRange(10,50);
	   inSpinBox -> setValue(10);
	   inSpinBox -> setPrefix(tr("max number of iteration = "));
	   regSpinBox = new QSpinBox;
	   regSpinBox -> setRange(10,50);
	   regSpinBox -> setValue(10);
	   regSpinBox -> setPrefix(tr("max number of regularization = "));
	   rpsSpinBox = new QSpinBox;
	   rpsSpinBox -> setRange(-8,-3);
	   rpsSpinBox -> setValue(-6);
	   rpsSpinBox -> setPrefix(tr("minimum relative step = 1e"));
	   resSpinBox = new QSpinBox;
	   resSpinBox -> setRange(-8,-3);
	   resSpinBox -> setValue(-5);
	   resSpinBox -> setPrefix(tr("minimum relative UWE step = 1e"));
	   QWidget *appWidget = new QWidget;
	   QGroupBox *appBox = new QGroupBox(tr("NLS properties"),appWidget);
       QBoxLayout *appLayout = new QBoxLayout(QBoxLayout::TopToBottom,appBox);
       appLayout->addWidget(inSpinBox);
       appLayout->addWidget(regSpinBox);
	   appLayout->addWidget(rpsSpinBox);
       appLayout->addWidget(resSpinBox);
	   appBox->setFixedSize ( 3*mBox->width(),5*mBox->height());
	   appWidget->setFixedSize ( 3*mBox->width(),5*mBox->height());
	   measureMarkedButton = new QPushButton(tr("measure marked objects"));
	   connect(measureMarkedButton, SIGNAL(clicked()), this, SLOT(slotMeasureMarkedButtonClicked())); 
	   addCheckBox=new QCheckBox(tr("add to frame list"));
	   cropCheckBox=new QCheckBox(tr("crop"));
       //
       QGridLayout *psfGrid = new QGridLayout(psfWidget);
       psfGrid->addWidget(gbWidget,0,0);	   
	   psfGrid->addWidget(orderSpinBox,1,0);
	   psfGrid->addWidget(appWidget,2,0);
	   psfGrid->addWidget(measureMarkedButton,3,0);
	   psfGrid->addWidget(addCheckBox,4,0);
	   psfGrid->addWidget(cropCheckBox,5,0);
       //
	 dockPsfWidget = new QDockWidget(tr("psf panel"),this,Qt::Widget);
     dockPsfWidget->setWidget(psfWidget);
	 dockPsfWidget->setFloating(true);
     addDockWidget(Qt::RightDockWidgetArea,dockPsfWidget);
	 
	 dockPsfWidget->hide();
     ///////////////////////////////////////////
	 QWidget *framesWidget = new QWidget;
	 comboFrames = new QComboBox(framesWidget);
	 connect(comboFrames, SIGNAL(currentIndexChanged(int)), this, SLOT(slotComboFramesChanged(int)));
	 QGridLayout *framesGrid = new QGridLayout(framesWidget);
	 addFrameButton = new QPushButton(tr("add frame"));
	 connect(addFrameButton, SIGNAL(clicked()), this, SLOT(slotAddFrameButtonClicked()));
	 delFrameButton = new QPushButton(tr("delete frame"));
	 connect(delFrameButton, SIGNAL(clicked()), this, SLOT(slotDelFrameButtonClicked()));
	 setFrameButton = new QPushButton(tr("set frame"));
	 connect(setFrameButton, SIGNAL(clicked()), this, SLOT(slotSetFrameButtonClicked()));
	 refFrameLabel = new QLabel(tr("reference CCD frame = "));
	 startCalcButton = new QPushButton(tr("execute"));
	 connect(startCalcButton, SIGNAL(clicked()), this, SLOT(slotStartCalcButtonClicked()));
	 comLineEdit = new QLineEdit("COMMAND");
	 ///
	 maxresSpinBox = new QDoubleSpinBox;
	 maxresSpinBox -> setDecimals(1);
	 maxresSpinBox -> setPrefix (tr("max residual = "));
	 maxresSpinBox -> setRange(0.0,10.0);
	 maxresSpinBox -> setSingleStep (0.1);
	 maxresSpinBox -> setValue(0.3);
	 ///
	 framesGrid -> addWidget(comboFrames,0,0);
	 framesGrid -> addWidget(addFrameButton,1,0);
	 framesGrid -> addWidget(delFrameButton,2,0);
	 framesGrid -> addWidget(setFrameButton,3,0);
	 framesGrid -> addWidget(maxresSpinBox,4,0);
	 framesGrid -> addWidget(startCalcButton,7,0);
	 framesGrid -> addWidget(refFrameLabel,5,0);
	 framesGrid -> addWidget(comLineEdit,6,0);
	 //======================================
	 dockFramesWidget = new QDockWidget(tr("frames panel"),this,Qt::Widget);
     dockFramesWidget->setWidget(framesWidget);
	 dockFramesWidget->setFloating(true);
     addDockWidget(Qt::TopDockWidgetArea,dockFramesWidget);
	 
	 dockFramesWidget->hide();
	 ///////////////////////////
	 
	 progressDialog = new QDialog(this);
	 prBar = new QProgressBar(progressDialog);
	 prBar -> setRange(0,100);
	 prBar -> setTextVisible (true);
	 abortButton = new QPushButton(tr("abort"));
	 connect(abortButton, SIGNAL(clicked()), this, SLOT(slotAbortButtonClicked())); 
	 QGridLayout *prGrid = new QGridLayout(progressDialog);
	 prGrid->addWidget(prBar,0,0);
	 prGrid->addWidget(abortButton,1,0);
	 //progressDialog ->show();
	 //
	 keyWidget = new QWidget;
	 keyLabel = new QLabel(keyWidget);
	 QPalette palette;
	 QBrush brush;
	 brush.setColor(Qt::white);
	 palette.setBrush(QPalette::Foreground,brush);
	 brush.setColor(Qt::black);
	 palette.setBrush(QPalette::Background,brush);
	 keyLabel->setPalette(palette);
	 keyScrollArea = new QScrollArea(keyWidget);
	 keyScrollArea->setMinimumSize(600,150);
     keyScrollArea->setBackgroundRole(QPalette::Shadow);
     keyScrollArea->setWidget(keyLabel);
	 keyWidget->setMinimumSize(625,175);
	 dockKeyWidget = new QDockWidget(tr("info panel"),this,Qt::Widget);
     dockKeyWidget->setWidget(keyWidget);
	 dockKeyWidget->setMinimumSize(625,175);
	 dockKeyWidget->setFloating(true);
	 //dockKeyWidget->setWindowFlags(Qt::Dialog); 
	 //dockKeyWidget->setFeatures(QDockWidget::AllDockWidgetFeatures);
     addDockWidget(Qt::BottomDockWidgetArea,dockKeyWidget);
	 dockKeyWidget->hide();
	 
	 //======================================
	 QWidget *telnetWidget = new QWidget;
	 QGridLayout *telnetGrid = new QGridLayout(telnetWidget);
	 //////////////////////////////////////////////
	 telnetPort = new QSpinBox;
	 telnetPort -> setRange(0,10000);
	 telnetPort -> setValue(6775);
	 telnetPort -> setPrefix(tr("port = "));
	 telnetHost = new QLineEdit;
	 telnetHost -> setFixedWidth(200);
	 telnetHost ->setText("horizons.jpl.nasa.gov");
	 telnetRequest = new QLineEdit;
	 telnetRequest -> setFixedWidth(200);
	 /////////////////////////////////////////////
	 sendButton = new QPushButton(tr("send"));
	 connect(sendButton, SIGNAL(clicked()), this, SLOT(slotSendButtonClicked()));
	 connectButton = new QPushButton(tr("connect"));
	 connect(connectButton, SIGNAL(clicked()), this, SLOT(slotConnectButtonClicked()));
	 disconnectButton = new QPushButton(tr("disconnect"));
	 connect(disconnectButton, SIGNAL(clicked()), this, SLOT(slotDisconnectButtonClicked()));
	 /////////////////
	 telnetGrid -> addWidget(telnetHost,0,0);
	 telnetGrid -> addWidget(telnetPort,0,1);
	 telnetGrid -> addWidget(connectButton,1,0);
	 telnetGrid -> addWidget(disconnectButton,1,1);
	 telnetGrid -> addWidget(telnetRequest,2,0);
	 telnetGrid -> addWidget(sendButton,2,1);
	 //+++
	 dockTelnetWidget = new QDockWidget(tr("telnet"),this,Qt::Widget);
     dockTelnetWidget->setWidget(telnetWidget);
	 dockTelnetWidget->setFloating(true);
     addDockWidget(Qt::TopDockWidgetArea,dockTelnetWidget);
	 
	 dockTelnetWidget->hide();
	 //========================================
	 horizon = new QTcpSocket;
	 connect(horizon, SIGNAL(connected()), this, SLOT(slotConnected()));
	 connect(horizon, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
	 //======================================
	 QWidget *propWidget = new QWidget;
	 //
	 comboTel = new QComboBox();
	 connect(comboTel, SIGNAL(highlighted(int)), this, SLOT(slotTelescopeChanged(int))); 
	 //
	 flSpinBox = new QDoubleSpinBox;
	 flSpinBox -> setDecimals(3);
	 flSpinBox -> setPrefix (tr("focal length = "));
	 flSpinBox -> setSuffix (tr(" mm"));
	 flSpinBox -> setRange(0.0,40000.0);
	 flSpinBox -> setSingleStep (10);
	 flSpinBox -> setValue(3467.000);
	 //
	 xpsSpinBox = new QDoubleSpinBox;
	 xpsSpinBox -> setDecimals(2);
	 xpsSpinBox -> setPrefix (tr("pixel x-size = "));
	 xpsSpinBox -> setSuffix (tr(" mkm"));
	 xpsSpinBox -> setRange(1.0,100.0);
	 xpsSpinBox -> setSingleStep (1);
	 xpsSpinBox -> setValue(16.0);
	 //
	 ypsSpinBox = new QDoubleSpinBox;
	 ypsSpinBox -> setDecimals(2);
	 ypsSpinBox -> setPrefix (tr("pixel y-size = "));
	 ypsSpinBox -> setSuffix (tr(" mkm"));
	 ypsSpinBox -> setRange(1.0,100.0);
	 ypsSpinBox -> setSingleStep (1);
	 ypsSpinBox -> setValue(16.0);
	 //
	 fovSpinBox = new QDoubleSpinBox;
	 fovSpinBox -> setDecimals(1);
	 fovSpinBox -> setPrefix (tr("fov = "));
	 fovSpinBox -> setSuffix (tr(" arcmin"));
	 fovSpinBox -> setRange(0.0,120.0);
	 fovSpinBox -> setSingleStep (1);
	 fovSpinBox -> setValue(15.0);
	 //
	 rotAngSpinBox = new QDoubleSpinBox;
	 rotAngSpinBox -> setDecimals(2);
	 rotAngSpinBox -> setPrefix (tr("pos angle = "));
	 rotAngSpinBox -> setSuffix (tr(" deg"));
	 rotAngSpinBox -> setRange(0,360);
	 rotAngSpinBox -> setSingleStep (1);
	 rotAngSpinBox -> setValue(0);
	 connect(rotAngSpinBox, SIGNAL(valueChanged(double)), this, SLOT(slotAngleChanged(double)));
	 //
	 currDirLabel = new QLabel(tr("current folder = "));
	 currDirButton = new QPushButton(tr("..."));
	 connect(currDirButton, SIGNAL(clicked()), this, SLOT(slotCurrDirButtonClicked()));
	 updateHistCheckBox=new QCheckBox(tr("update histogram"));
	 ucacDirLabel = new QLabel(tr("ucac2 folder = "));
	 ucacDirButton = new QPushButton(tr("..."));
	 connect(ucacDirButton, SIGNAL(clicked()), this, SLOT(slotUcacDirButtonClicked()));
	 logDirLabel = new QLabel(tr("log folder = "));
	 logDirButton = new QPushButton(tr("..."));
	 connect(logDirButton, SIGNAL(clicked()), this, SLOT(slotLogDirButtonClicked()));
	 //
	 proxyPort = new QSpinBox;
	 proxyPort -> setRange(0,10000);
	 proxyPort -> setValue(3128);
	 proxyPort -> setPrefix(tr("proxy port = "));
	 proxyPort -> setFixedWidth(200);
	 proxyHost = new QLineEdit;
	 proxyHost -> setFixedWidth(200);
	 proxyHost -> setText("proxy.gao.spb.ru");
	 useProxy=new QCheckBox(tr("use proxy"));
	 useProxy->setCheckState(Qt::Checked);
	 obsCode= new QLineEdit;
	 obsCode -> setFixedWidth(200);
	 obsCode -> setText("084");
	 //
	 QGridLayout *propGrid = new QGridLayout(propWidget);
	 propGrid -> addWidget(comboTel,0,0);
	 propGrid -> addWidget(flSpinBox,1,0);
	 propGrid -> addWidget(xpsSpinBox,2,0);
	 propGrid -> addWidget(ypsSpinBox,3,0);
	 propGrid -> addWidget(fovSpinBox,4,0);
	 propGrid -> addWidget(rotAngSpinBox,5,0);
	 propGrid -> addWidget(currDirLabel,6,0);
	 propGrid -> addWidget(currDirButton,6,1);
	 propGrid -> addWidget(ucacDirLabel,7,0);
	 propGrid -> addWidget(ucacDirButton,7,1);
	 propGrid -> addWidget(logDirLabel,8,0);
	 propGrid -> addWidget(logDirButton,8,1);
	 propGrid -> addWidget(updateHistCheckBox,9,0);
	 propGrid -> addWidget(proxyHost,10,0);
	 propGrid -> addWidget(proxyPort,11,0);
	 propGrid -> addWidget(useProxy,12,0);
	 propGrid -> addWidget(obsCode,13,0);
	 //propGrid -> addWidget(radeLineEdit,0,1);
	 //+++
	 dockPropWidget = new QDockWidget(tr("properties"),this,Qt::Widget);
     dockPropWidget->setWidget(propWidget);
	 dockPropWidget->setFloating(true);
     addDockWidget(Qt::TopDockWidgetArea,dockPropWidget);
	 
	 dockPropWidget->hide();
	 //=================================
	 histLabel = new QLabel;
     histLabel->setBackgroundRole(QPalette::Dark);
     histLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
     histLabel->setFixedSize(650,350);
	 updateHistButton = new QPushButton(tr("update"));
	 connect(updateHistButton, SIGNAL(clicked()), this, SLOT(slotUpdateHistButtonClicked())); 
	 //===========
	 QWidget *histWidget = new QWidget;
	 QGridLayout *histGrid = new QGridLayout(histWidget);
	 histGrid->addWidget(histLabel,0,0);
	 histGrid->addWidget(updateHistButton,1,0);
	 dockHist = new QDockWidget(tr("histogram"),this,Qt::Widget);
     dockHist->setWidget(histWidget);
	 dockHist->setFloating(true);
	 //dockHist->setAllowedAreas (Qt::NoDockWidgetArea ); 
     addDockWidget(Qt::TopDockWidgetArea,dockHist);
	 dockHist->hide();
	 //===================================
    
};//end setupWidgets()


void MainWindow::open_file()
{
    
	QStringList FName = QFileDialog::getOpenFileNames(
                    this,
                    tr("Choose files"),
                    currDir,
                    "CCD (*.img *.fit *.fits *.fts)");
    
    if (FName.count()!=0)
    {
	   statusBar()->showMessage(tr("loading"));
	   for (int i=1;i<FName.count();i++) emitLoadFITS(FName.at(i));
	   emitLoadFITS(FName.at(0));
	   statusBar()->showMessage(tr(""));
    }
    keyLabel->setText(imgF.keyStr);
    keyLabel->adjustSize();	
	//if (FName!="") emitLoadFITS(FName); else statusBar()->showMessage("");
	//setWindowTitle ("astrored2: "+FName);
};//end open_file()

void MainWindow::magnify()
{
     //QMessageBox::information(0,"debug","bad",QMessageBox::Ok,0,0);
     if (dockScWidget->isHidden()) 
                    {
                      dockScWidget->show();
					  imageLabel->setCursor(Qt::CrossCursor);
                    }//end if
};

void MainWindow::marking()
{
//QMessageBox::information(0,"debug","marking",QMessageBox::Ok,0,0);
if (dockMarkWidget->isHidden()) 
                    {
                      dockMarkWidget->show();
                    }//end if
};

void MainWindow::psfPanel()
{
if (dockPsfWidget->isHidden()) 
                    {
                      dockPsfWidget->show();
                    }//end if
};

void MainWindow::slotShowHist()
{
if (dockHist->isHidden()) 
                    {
                      dockHist->show();
                    }//end if

};

/*void MainWindow::dockMeasureWidgetShow()
{
//QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
if (dockMeasureWidget->isHidden()) 
                    {
                      dockMeasureWidget->show();
                    }//end if
}*/

void MainWindow::scale_state(int state)
{
emitUpdateImg();    
};

void MainWindow::slotSelectState(int state)
{
 emitUpdateImg(); 
};

void MainWindow::magnify_state(int state)
{
   //QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
   if (img){
   int x_pos = mainX;
   int y_pos = mainY;
   int scw = (int) 200/scspinBox->value();
  if ((x_pos>=0)&&(x_pos<imgWidth)&&(y_pos>=0)&&(y_pos<imgHeight)){
   DrawImageInScaledLabel(x_pos, y_pos, scw);
   }//end if
   mainX = x_pos;
   mainY = y_pos;
   }//if img
};//magnify_state

void MainWindow::updateHist()
{
     int wh = histLabel->width();
	 int hh = histLabel->height();
	 QPicture picture;
	 QPainter paint;
	 paint.begin(&picture);
	 QPen pen;
	 pen.setWidth(1);
	 QBrush brush(Qt::SolidPattern);
	 brush.setColor(Qt::black);
	 paint.setPen(pen);
	 //paint.setBrush(brush);
	 paint.drawRect(0,0,wh,hh);
	 paint.fillRect(0,0,wh,hh,brush);
	 pen.setColor(Qt::yellow);
     paint.setPen(pen);
	 paint.drawRect(5,5,wh-10,hh-10);
	 long hsize = imgF.ADCmax - imgF.ADCmin + 1;
	 int x,y;
	 double maxH = log(imgF.maxHist);
	 double lnH;
	 pen.setColor(Qt::white);
	 paint.setPen(pen);
	 for(long i = 0; i<hsize; i++)
	 {
        x = 5+(int)((wh-10)*(double)i/hsize);
		if (imgF.histOfImg[i]!=0) 
		{
		   lnH = log(imgF.histOfImg[i]);
           y = (hh-5) - (int)((hh-10)*lnH/maxH);
		}
		else y = hh-5;
        paint.drawLine(x,hh-5,x,y);		
	 }
	 //
	 pen.setColor(Qt::red);
	 paint.setPen(pen);
	 paint.drawLine(histX1,hh-5,histX1,5);
	 paint.drawLine(histX2,hh-5,histX2,5);
	 paint.end();
	 //
	 histLabel->clear();
     histLabel->setPicture(picture);
     //histLabel->adjustSize();
};


void MainWindow::slotBrightness(int value)
{
 if (img_status!=1)
  {
  double gfactor = 1+4*gammaSlider->value()/100;
  if (img) emitBC((double)contrastSlider->value(),(double)value,gfactor,invCheckBox->checkState());
  }
};//slotBrightness

void MainWindow::slotContrast(int value)
{
if (img_status!=1)
  {
   double gfactor = 1+4*gammaSlider->value()/100;
   if (img) emitBC((double)value,(double)brightnessSlider->value(),gfactor,invCheckBox->checkState());
   }
}//slotContrast

void MainWindow::slotGamma(int g)
{
if (img_status!=1)
  {
  double gfactor = 1+4*g/100;
  if (img) emitBC((double)contrastSlider->value(),(double)brightnessSlider->value(),gfactor,invCheckBox->checkState());
  }
};//slotGamma




void MainWindow::slotShowImg(int state)
{
//QMessageBox::information(0,"debug","image",QMessageBox::Ok,0,0);
//QString s;
//s.setNum(imgF.num_of_frames);
//QMessageBox::information(0,"debug",s,QMessageBox::Ok,0,0);
if (imgF.num_of_frames==0)
{//QMessageBox::information(0,"debug","image",QMessageBox::Ok,0,0);
 delete img;
 img=NULL;
 imageLabel->resize (0,0);
 QPixmap pix;
 QPainter paint(&pix);
 scaledLabel->setPixmap(pix);
}
else
{
if (state)//new img
  {
	  if (img) delete img;
	  imgWidth = imgF.xSize;
	  imgHeight = imgF.ySize;
	  setWindowTitle ("astrored2: "+imgF.file_name);
      img = new QImage(imgF.imgCCD,imgWidth,
                       imgHeight,QImage::Format_Indexed8);
      img->setColorTable(colormap);
	  /////////////////////////////////////////////////////////////
	  keyWidget->setWindowTitle(tr("fits header"));
	  QFont f("Courier", 8,QFont::Bold);
      keyLabel->setFont(f);
	  keyLabel->setTextFormat(Qt::PlainText);
	  keyLabel->setText(imgF.keyStr);
      keyLabel->adjustSize();
	  ////////////////////////////////////////////////////////////
      //BEGIN sliders
	  img_status = 1;
	  
	 contrastSlider->setRange((int)imgF.leftB,(int)imgF.rightB);
	 //imgF.lHist = imgF.leftB + (imgF.rightB-imgF.leftB)/2;
	 contrastSlider->setValue((int)imgF.leftB);
	 brightnessSlider->setRange((int)imgF.leftB,(int)imgF.rightB);
	 brightnessSlider->setValue((int)imgF.rightB);
	 gammaSlider->setRange(0,100);
	 gammaSlider->setValue(0);
	
	  img_status = 0;
      //END sliders
	  if ((mainX>imgWidth)||(mainY>imgHeight))
	  {
	    mainX = 0;
	    mainY = 0;
	  }
      //QMessageBox::information(0,"debug","not empty",QMessageBox::Ok,0,0);
 }
else//existing img
   {
    int Len = (int)(imgWidth*imgHeight);
    img->loadFromData(imgF.imgCCD,Len);
    img_status = 0;	
   }//end of existing img
//////////////////////////////////////////////////
invCheckBox->setCheckState(Qt::CheckState (imgF.inv));
long hsize = imgF.ADCmax - imgF.ADCmin + 1;
histX1 = 5+(int)((histLabel->width()-10)*(double)(imgF.leftB - imgF.ADCmin)/hsize);
histX2 = 5+(int)((histLabel->width()-10)*(double)(imgF.rightB - imgF.ADCmin)/hsize);
if (updateHistCheckBox->checkState() == Qt::Checked) updateHist();
//QString str;
//str.setNum(imgF.leftB);
//QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
//str.setNum(imgF.ADCmin);
//QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
slotUpdateImg();
slotUpdateAperture(0);
///////////////////
radeLineEdit->setText(mas_to_hms(imgF.ra_oc, " ",3)+" "+mas_to_damas(imgF.de_oc, " ",2)+" 08.0 17.0");

///////////////////
}
statusBar()->showMessage("");
};

void MainWindow::slotUpdateImg()
{
if (img){
   if (scaleCheckBox->checkState() == Qt::Checked)
   {
      double scf = (double)(imgHeight/(double)scrollArea->height());
      currX = (int) (mainX/scf);
      currY = (int) (mainY/scf);
      QPixmap pixmap(QPixmap::fromImage(img->scaledToHeight(scrollArea->height(),Qt::FastTransformation)));
	  QPainter paint(&pixmap);
		   QPen pen;
           pen.setColor(Qt::red);
		   pen.setWidth(2);
           paint.setPen(pen);
           paint.drawLine(currX,currY,currX+10,currY);
		   paint.drawLine(currX,currY,currX,currY+10);
		   ////////////////////////////////
		   int x,y,r;
		   if (rbMoveAll->isChecked())
		   {
			   pen.setColor(Qt::green);
			   paint.setPen(pen);
			   for (int i = 0; i<objl.count();i++)
			   {
			      QPoint pt = objl[i];
				  x = (int)((pt.x()*cos(rot_angle)+pt.y()*sin(rot_angle)+deltaX)/scf);
				  y = (int)((-pt.x()*sin(rot_angle)+pt.y()*cos(rot_angle)+deltaY)/scf);
				  r = (int)(aperture_spinBox->value()/scf);
				  if (pt.x()!=-1)paint.drawEllipse(x-r,y-r,2*r,2*r);
			   }
			   ////////////////////////////////
			   pen.setColor(Qt::yellow);
			   paint.setPen(pen);
			   for (int i = 0; i<m_objl.count();i++)
			   {
			      QPoint mpt = m_objl[i];
				  x = (int)((mpt.x()*cos(rot_angle)+mpt.y()*sin(rot_angle)+deltaX)/scf);
				  y = (int)((-mpt.x()*sin(rot_angle)+mpt.y()*cos(rot_angle)+deltaY)/scf);
				  r = (int)(aperture_spinBox->value()/scf);
				  if (mpt.x()!=-1)paint.drawEllipse(x-r,y-r,2*r,2*r);
			   }
			   if ((fX!=0)&&(fY!=0))
			   {
			      pen.setColor(Qt::red);
			      paint.setPen(pen);
			      x = (int)((fX*cos(rot_angle)+fY*sin(rot_angle)+deltaX)/scf);
				  y = (int)((-fX*sin(rot_angle)+fY*cos(rot_angle)+deltaY)/scf);
				  r = (int)(aperture_spinBox->value()/scf);
				  paint.drawEllipse(x-r,y-r,2*r,2*r);
			   }
		   }
		   else
		   {
		       pen.setColor(Qt::green);
			   paint.setPen(pen);
			   for (int i = 0; i<objl.count();i++)
			   {
			      QPoint pt = objl[i];
				  x = pt.x()/scf;
				  y = pt.y()/scf;
				  r = (int)(aperture_spinBox->value()/scf);
				  if (pt.x()!=-1)paint.drawEllipse(x-r,y-r,2*r,2*r);
			   }
			   ////////////////////////////////
			   pen.setColor(Qt::yellow);
			   paint.setPen(pen);
			   for (int i = 0; i<m_objl.count();i++)
			   {
			      QPoint mpt = m_objl[i];
				  x = mpt.x()/scf;
				  y = mpt.y()/scf;
				  r = (int)(aperture_spinBox->value()/scf);
				  if (mpt.x()!=-1)paint.drawEllipse(x-r,y-r,2*r,2*r);
			   }
		      if ((fX!=0)&&(fY!=0))
			   {
			      pen.setColor(Qt::red);
			      paint.setPen(pen);
			      x = (int)((fX*cos(rot_angle)+fY*sin(rot_angle)+deltaX)/scf);
				  y = (int)((-fX*sin(rot_angle)+fY*cos(rot_angle)+deltaY)/scf);
				  r = (int)(aperture_spinBox->value()/scf);
				  paint.drawEllipse(x-r,y-r,2*r,2*r);
			   }
		   }
		   ////////////////////////////////////////////////
	  imageLabel->setPixmap(pixmap);
   }  
   else 
   {
      currX = mainX;
	  currY = mainY;
	  QPixmap pixmap(QPixmap::fromImage(img->copy(0,0,imgWidth,imgHeight)));
	  QPainter paint(&pixmap);
	       QPen pen;
           pen.setColor(Qt::red);
		   pen.setWidth(2);
           paint.setPen(pen);
           paint.drawLine(currX,currY,currX+10,currY);
		   paint.drawLine(currX,currY,currX,currY+10);
		   ///////////////////////////////////
		   int x,y,r;
		   if (rbMoveAll->isChecked())
		   {
			   pen.setColor(Qt::green);
			   paint.setPen(pen);
			   for (int i = 0; i<objl.count();i++)
			   {
			      QPoint pt = objl[i];
				  x = (int)(pt.x()*cos(rot_angle)+pt.y()*sin(rot_angle)+deltaX);
				  y = (int)(-pt.x()*sin(rot_angle)+pt.y()*cos(rot_angle)+deltaY);
				  r = (int)(aperture_spinBox->value());
				  if (x!=-1)paint.drawEllipse(x-r,y-r,2*r,2*r);
			   }
			   //////////////////////////////////
			   pen.setColor(Qt::yellow);
			   paint.setPen(pen);
			   for (int i = 0; i<m_objl.count();i++)
			   {
			      QPoint mpt = m_objl[i];
				  x = (int)(mpt.x()*cos(rot_angle)+mpt.y()*sin(rot_angle)+deltaX);
				  y = (int)(-mpt.x()*sin(rot_angle)+mpt.y()*cos(rot_angle)+deltaY);
				  r = (int)(aperture_spinBox->value());
				  if (x!=-1)paint.drawEllipse(x-r,y-r,2*r,2*r);
			   }
			   //////////////////////////////////
			   if ((fX!=0)&&(fY!=0))
			   {
			      pen.setColor(Qt::red);
			      paint.setPen(pen);
			      x = (int)(fX*cos(rot_angle)+fY*sin(rot_angle)+deltaX);
				  y = (int)(-fX*sin(rot_angle)+fY*cos(rot_angle)+deltaY);
				  r = (int)(aperture_spinBox->value());
				  paint.drawEllipse(x-r,y-r,2*r,2*r);
			   }
		   }
		   else
		   {
		       pen.setColor(Qt::green);
			   paint.setPen(pen);
			   for (int i = 0; i<objl.count();i++)
			   {
			      QPoint pt = objl[i];
				  x = pt.x();
				  y = pt.y();
				  r = (int)(aperture_spinBox->value());
				  if (x!=-1)paint.drawEllipse(x-r,y-r,2*r,2*r);
			   }
			   //////////////////////////////////
			   pen.setColor(Qt::yellow);
			   paint.setPen(pen);
			   for (int i = 0; i<m_objl.count();i++)
			   {
			      QPoint mpt = m_objl[i];
				  x = mpt.x();
				  y = mpt.y();
				  r = (int)(aperture_spinBox->value());
				  if (x!=-1)paint.drawEllipse(x-r,y-r,2*r,2*r);
			   }
		       if ((fX!=0)&&(fY!=0))
			   {
			      pen.setColor(Qt::red);
			      paint.setPen(pen);
			      x = (int)(fX*cos(rot_angle)+fY*sin(rot_angle)+deltaX);
				  y = (int)(-fX*sin(rot_angle)+fY*cos(rot_angle)+deltaY);
				  r = (int)(aperture_spinBox->value());
				  paint.drawEllipse(x-r,y-r,2*r,2*r);
			   }
		   }
	  imageLabel->setPixmap(pixmap);
	}  
   imageLabel->adjustSize();
   scfactor = (double)(imgHeight/(double)imageLabel->height());
   //slotUpdateAperture(0);
   }//img
};

void MainWindow::slotUpdateAperture(int value)
{  
   int scw = (int) 200/scspinBox->value();
   DrawImageInScaledLabel(mainX, mainY, scw);
   slotUpdateImg();
};

void MainWindow::slotUpdateProfile(int value)
{
   int scw = (int) 200/scspinBox->value();
   DrawImageInScaledLabel(mainX, mainY, scw); 
};
/*
void MainWindow::slotRestoreButtonClicked()
{
  if (img)
    {  
     if (fullrangeCheckBox->checkState() == Qt::Checked)
	  {
	    contrastSlider->setRange((int)imgF.ADCmin,(int)imgF.ADCmax);
	     contrastSlider->setValue((int)imgF.lHist);
	     brightnessSlider->setRange((int)imgF.ADCmin,(int)imgF.ADCmax);
         brightnessSlider->setValue((int)imgF.rHist);
	     gammaSlider->setRange(0,100);
         gammaSlider->setValue(0);
	  }
	  else
	  {
	     contrastSlider->setRange((int)imgF.leftB,(int)imgF.rightB);
	     contrastSlider->setValue((int)imgF.lHist);
	     brightnessSlider->setRange((int)imgF.leftB,(int)imgF.rightB);
         brightnessSlider->setValue((int)imgF.rHist);
	     gammaSlider->setRange(0,100);
         gammaSlider->setValue(0);
	 }
	} 
};
*/
void MainWindow::slotCofmButtonClicked()
{
if (img) 
   {
      emitCenterOfMass(mainX, mainY, aperture_spinBox->value());	  
   }
};

void MainWindow::slotBoxButtonClicked()
{
if (img) 
   {
    int scw = (int) 200/scspinBox->value();
	CCDDATA box_data = BoxData(imgF.fCCD, mainX, mainY, scw, scw, imgF.xSize, imgF.ySize);
    /////////////////////////
    QString StatusStr(tr("box data: X = "));
    StatusStr = StatusStr+QString( "%1" ).arg( mainX,4,10,QLatin1Char( ' ' ))+"; Y = ";
    StatusStr = StatusStr+QString( "%1" ).arg( mainY,4,10,QLatin1Char( ' ' ))+"; size = ";
	StatusStr = StatusStr+QString( "%1" ).arg( scw,4,10,QLatin1Char( ' ' ))+"; mean = ";
    StatusStr = StatusStr+QString( "%1" ).arg( box_data.meanADC,6,'f',3,QLatin1Char( ' ' ))+"; se = ";
    StatusStr = StatusStr+QString( "%1" ).arg( box_data.seADC,6,'f',3,QLatin1Char( ' ' ))+"; min = ";
	StatusStr = StatusStr+QString( "%1" ).arg( box_data.minADC,6,'f',1,QLatin1Char( ' ' ))+"; max = ";
	StatusStr = StatusStr+QString( "%1" ).arg( box_data.maxADC,6,'f',1,QLatin1Char( ' ' ));
	statusBar()->showMessage(StatusStr);
    delete[]	box_data.Buffer;
   }
};

void MainWindow::slotCenterButtonClicked()
{
 if (img)
 {
   int x_pos = mainX;
   int y_pos = mainY;
   int scw = (int) 200/scspinBox->value();
   //STIMG st_center = StarCentering(imgF.fCCD, mainX, mainY, aperture_spinBox->value(), 2, imgF.xSize, imgF.ySize);
   STIMG st_center = getStarCenter(imgF.fCCD, mainX, mainY, aperture_spinBox->value(), 2, imgF.xSize, imgF.ySize);
   x_pos = (int)st_center.X;
   y_pos = (int)st_center.Y;
   if ((x_pos>=0)&&(x_pos<imgWidth)&&(y_pos>=0)&&(y_pos<imgHeight))
	{
     DrawImageInScaledLabel(x_pos, y_pos, scw);
    }//end if
   mainX = x_pos;
   mainY = y_pos;
  }//if img
};

void MainWindow::slotPsfButtonClicked()
{
 if (img) 
   {
   int psft;
   if (lorentzRadioButton->isChecked()) psft = 0;
   if (gaussianRadioButton->isChecked()) psft = 1;
   if (moffatRadioButton->isChecked()) psft = 2;
   imgF.max_num_it = inSpinBox ->value();
   imgF.max_num_reg = regSpinBox ->value();
   imgF.min_par_step = pow(10,rpsSpinBox ->value());
   imgF.min_uwe_step = pow(10,resSpinBox ->value());
   emitApproxImg(mainX, mainY, aperture_spinBox->value(),orderSpinBox->value(),psft);
   }
};

void MainWindow::slotSubButtonClicked()
{
if (img) 
   {
   int psft;
   if (lorentzRadioButton->isChecked()) psft = 0;
   if (gaussianRadioButton->isChecked()) psft = 1;
   if (moffatRadioButton->isChecked()) psft = 2;
   emit signalSubPsf(mainX, mainY, aperture_spinBox->value(),psft);
   }
};

void MainWindow::slotClearButtonClicked()
{
 objl.clear();
 m_objl.clear();
 refToStar.clear();
 slotUpdateImg();
};

void MainWindow::slotAbortButtonClicked()
{
  imgF.abort = true;
  progressDialog->hide();
};

void MainWindow::slotMeasureMarkedButtonClicked()
{
if (img) 
   {
   imgF.pr_ol->resize(1,1);
   imgF.pm_ol->resize(1,1);
   int psft;
   if (lorentzRadioButton->isChecked()) psft = 0;
   if (gaussianRadioButton->isChecked()) psft = 1;
   if (moffatRadioButton->isChecked()) psft = 2;
   imgF.max_num_it = inSpinBox ->value();
   imgF.max_num_reg = regSpinBox ->value();
   imgF.min_par_step = pow(10,rpsSpinBox ->value());
   imgF.min_uwe_step = pow(10,resSpinBox ->value());
   //
   int i;
   QPoint pt;
   if (cropCheckBox->checkState() == Qt::Checked)
   {
     i = 0;
	 do
	 {
	   pt = objl[i];
       if ((pt.x()>=0)&&(pt.x()<imgF.xSize)&&(pt.y()>=0)&&(pt.y()<imgF.ySize))
	        i++;
		else
		   {
       	    objl.remove(i);
			refToStar.remove(i);
		   }
	 }
	 while(i<objl.count());
   }
   //
   imgF.pr_ol->resize(objl.count(),26);
   for (i = 0;i<objl.count();i++)
   {
     pt = objl[i];
	 imgF.pr_ol->insert_element(i,0,pt.x());
     imgF.pr_ol->insert_element(i,1,pt.y());	 
	 imgF.pr_ol->insert_element(i,25,refToStar[i]);
   }
   imgF.pm_ol->resize(m_objl.count(),26);
   for (i = 0;i<m_objl.count();i++)
   {
     pt = m_objl[i];
	 imgF.pm_ol->insert_element(i,0,pt.x());
     imgF.pm_ol->insert_element(i,1,pt.y());	 
   }
   //
  imgF.abort = false;
  progressDialog->show();
  int cent = 0;
  if(centeringCheckBox->checkState() == Qt::Checked) cent = 1;
  int isobj = 0;
  if(checkObjCheckBox->checkState() == Qt::Checked) isobj = 1;
  emitMeasureMarkedObjects(aperture_spinBox->value(),orderSpinBox->value(), psft, cent, isobj);
  }
};

void MainWindow::slotProgressAct()
{
  progressDialog->show();
};

void MainWindow::slotMeasured()
{
      
        QString StatusStr(tr("center of mass: X = "));
		StatusStr = StatusStr+QString( "%1" ).arg( vRes[0],6,'f',3,QLatin1Char( ' ' ))+"; Y = ";
		StatusStr = StatusStr+QString( "%1" ).arg( vRes[1],6,'f',3,QLatin1Char( ' ' ))+"; mean = ";
		StatusStr = StatusStr+QString( "%1" ).arg( vRes[2],6,'f',3,QLatin1Char( ' ' ));
		statusBar()->showMessage(StatusStr);
};

void MainWindow::slotPSF()
{
    QString StatusStr("P = ");
	int i;
	for (i = 0;i<8;i++)
	StatusStr = StatusStr+QString( "%1" ).arg( vRes[i],5,'f',2,QLatin1Char( ' ' ))+" ";
	StatusStr = StatusStr+" ERR = ";
	for (i = 8;i<16;i++)
	StatusStr = StatusStr+QString( "%1" ).arg( vRes[i],5,'f',4,QLatin1Char( ' ' ))+" ";
	//
	StatusStr = StatusStr+" UWE = ";
	StatusStr = StatusStr+QString( "%1" ).arg( vRes[16],5,'f',2,QLatin1Char( ' ' ))+" ";
	StatusStr = StatusStr+" S = ";
	StatusStr = StatusStr+QString( "%1" ).arg( vRes[17],5,'f',2,QLatin1Char( ' ' ))+" ";
	StatusStr = StatusStr+" N = ";
	StatusStr = StatusStr+QString( "%1" ).arg( vRes[18],3,'f',0,QLatin1Char( ' ' ))+" ";
	StatusStr = StatusStr+" psf = ";
	StatusStr = StatusStr+QString( "%1" ).arg( vRes[19],1,'f',0,QLatin1Char( ' ' ))+" ";
	statusBar()->showMessage(StatusStr);
};

void MainWindow::slotDone()
{
 progressDialog->hide();
 if(recompCheckBox->checkState() == Qt::Checked)
 {
   int i;
   objl.clear();
   QPoint pnt;
   for(i =0;i<imgF.pr_ol->size1();i++)
   {
      pnt.setX((int) MEFP(*imgF.pr_ol, i, 5));
	  pnt.setY((int) MEFP(*imgF.pr_ol, i, 6));
	  //if(pnt.x()!=-1) 
	  objl<<pnt;
   };
   m_objl.clear();
   for(i =0;i<imgF.pm_ol->size1();i++)
   {
      pnt.setX((int) MEFP(*imgF.pm_ol, i, 5));
	  pnt.setY((int) MEFP(*imgF.pm_ol, i, 6));
	  //if(pnt.x()!=-1) 
	  m_objl<<pnt;
   };
  slotUpdateImg();
 }
if (addCheckBox->checkState() == Qt::Checked)
{
  emit signalAddMFrame();
} 
};

void MainWindow::slotSaveResults()
{
if (img) 
   {
QString FName = QFileDialog::getSaveFileName(
                    this,
                    tr("input file name"),
                    "",
                    "text files (*.txt)");
 if(FName!="")
 {
   int i,j;
   double mv;
   QFile outputfile(FName);
   outputfile.open(QIODevice::WriteOnly | QIODevice::Text);
   QTextStream out_stream;
   out_stream.setDevice(&outputfile);
   QString S;
   /////////////////////////////////////////
   S = "The results of measurements of marked objects.\n\n";
   out_stream << S;
   S = "reference objects:\n\n";
   out_stream << S;
   for (j = 0; j<imgF.pr_ol->size1(); j++)
   {
     S = "";
     for (i = 0;i<8;i++)
	  {
	    mv = MEFP(*imgF.pr_ol,j,i);
	    S = S+QString( "%1" ).arg( mv,10,'f',2,QLatin1Char( ' ' ))+" ";
	  }
	 S = S + " | ";
	 for (i = 8;i<16;i++)
	  {
	   mv = MEFP(*imgF.pr_ol,j,i);
	   S = S+QString( "%1" ).arg( mv,10,'f',4,QLatin1Char( ' ' ))+" ";
	  }
     S = S + " | ";
	 mv = MEFP(*imgF.pr_ol,j,16);
     S = S+QString( "%1" ).arg( mv,8,'f',2,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pr_ol,j,17);
	 S = S+QString( "%1" ).arg( mv,12,'f',2,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pr_ol,j,18);
	 S = S+QString( "%1" ).arg( mv,3,'f',0,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pr_ol,j,19);
	 S = S+QString( "%1" ).arg( mv,1,'f',0,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pr_ol,j,20);
	 S = S+QString( "%1" ).arg( mv,14,'f',6,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pr_ol,j,21);
	 S = S+QString( "%1" ).arg( mv,6,'f',2,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pr_ol,j,22);
	 S = S+QString( "%1" ).arg( mv,8,'f',2,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pr_ol,j,23);
	 S = S+QString( "%1" ).arg( mv,8,'f',2,QLatin1Char( ' ' ))+" ";
     S = S + "\n";	 
	 out_stream << S;
   }
   S = "\n\n measuring objects:\n\n";
   out_stream << S;
   for (j = 0; j<imgF.pm_ol->size1(); j++)
   {
     S = "";
     for (i = 0;i<8;i++)
	  {
	    mv = MEFP(*imgF.pm_ol,j,i);
	    S = S+QString( "%1" ).arg( mv,10,'f',2,QLatin1Char( ' ' ))+" ";
	  }
	 S = S + " | ";
	 for (i = 8;i<16;i++)
	  {
	   mv = MEFP(*imgF.pm_ol,j,i);
	   S = S+QString( "%1" ).arg( mv,10,'f',4,QLatin1Char( ' ' ))+" ";
	  }
     S = S + " | ";
	 mv = MEFP(*imgF.pm_ol,j,16);
     S = S+QString( "%1" ).arg( mv,8,'f',2,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pm_ol,j,17);
	 S = S+QString( "%1" ).arg( mv,12,'f',2,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pm_ol,j,18);
	 S = S+QString( "%1" ).arg( mv,3,'f',0,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pm_ol,j,19);
	 S = S+QString( "%1" ).arg( mv,1,'f',0,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pm_ol,j,20);
	 S = S+QString( "%1" ).arg( mv,14,'f',6,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pm_ol,j,21);
	 S = S+QString( "%1" ).arg( mv,6,'f',2,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pr_ol,j,22);
	 S = S+QString( "%1" ).arg( mv,8,'f',2,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pr_ol,j,23);
	 S = S+QString( "%1" ).arg( mv,8,'f',2,QLatin1Char( ' ' ))+" ";
     S = S + "\n";	 
	 out_stream << S;
   }
   //out_stream << S;  
   outputfile.close();
 }
}
};

void MainWindow::slotKeyAct()
{
 /////////////////////////////////////////////////////////////
  dockKeyWidget->setWindowTitle(tr("fits header"));
  QFont f("Courier", 8, QFont::Bold);
  keyLabel->setFont(f);
  keyLabel->setTextFormat(Qt::PlainText);
  keyLabel->setText(imgF.keyStr);
  keyLabel->adjustSize();
  ////////////////////////////////////////////////////////////
  dockKeyWidget->show();
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(imgF.keyStr);
};

void MainWindow::slotResAct()
{
 if(img){
 QString S;
 int i,j;
 double mv;
 //
   S = "The results of measurements of marked objects.\n\n";
   S = S+"reference objects:\n\n";
   for (j = 0; j<imgF.pr_ol->size1(); j++)
   {
     for (i = 0;i<8;i++)
	  {
	    mv = MEFP(*imgF.pr_ol,j,i);
	    S = S+QString( "%1" ).arg( mv,10,'f',2,QLatin1Char( ' ' ))+" ";
	  }
	 S = S + " | ";
	 for (i = 8;i<16;i++)
	  {
	   mv = MEFP(*imgF.pr_ol,j,i);
	   S = S+QString( "%1" ).arg( mv,10,'f',4,QLatin1Char( ' ' ))+" ";
	  }
     S = S + " | ";
	 mv = MEFP(*imgF.pr_ol,j,16);
     S = S+QString( "%1" ).arg( mv,8,'f',2,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pr_ol,j,17);
	 S = S+QString( "%1" ).arg( mv,12,'f',2,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pr_ol,j,18);
	 S = S+QString( "%1" ).arg( mv,3,'f',0,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pr_ol,j,19);
	 S = S+QString( "%1" ).arg( mv,1,'f',0,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pr_ol,j,20);
	 S = S+QString( "%1" ).arg( mv,14,'f',6,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pr_ol,j,21);
	 S = S+QString( "%1" ).arg( mv,6,'f',2,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pr_ol,j,22);
	 S = S+QString( "%1" ).arg( mv,8,'f',2,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pr_ol,j,23);
	 S = S+QString( "%1" ).arg( mv,8,'f',2,QLatin1Char( ' ' ))+" ";
     S = S + "\n";	 
   }
   S = S+ "\n\n measuring objects:\n\n";
   for (j = 0; j<imgF.pm_ol->size1(); j++)
   {
     for (i = 0;i<8;i++)
	  {
	    mv = MEFP(*imgF.pm_ol,j,i);
	    S = S+QString( "%1" ).arg( mv,10,'f',2,QLatin1Char( ' ' ))+" ";
	  }
	 S = S + " | ";
	 for (i = 8;i<16;i++)
	  {
	   mv = MEFP(*imgF.pm_ol,j,i);
	   S = S+QString( "%1" ).arg( mv,10,'f',4,QLatin1Char( ' ' ))+" ";
	  }
     S = S + " | ";
	 mv = MEFP(*imgF.pm_ol,j,16);
     S = S+QString( "%1" ).arg( mv,8,'f',2,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pm_ol,j,17);
	 S = S+QString( "%1" ).arg( mv,12,'f',2,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pm_ol,j,18);
	 S = S+QString( "%1" ).arg( mv,3,'f',0,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pm_ol,j,19);
	 S = S+QString( "%1" ).arg( mv,1,'f',0,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pm_ol,j,20);
	 S = S+QString( "%1" ).arg( mv,14,'f',6,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pm_ol,j,21);
	 S = S+QString( "%1" ).arg( mv,6,'f',2,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pm_ol,j,22);
	 S = S+QString( "%1" ).arg( mv,8,'f',2,QLatin1Char( ' ' ))+" ";
	 mv = MEFP(*imgF.pm_ol,j,23);
	 S = S+QString( "%1" ).arg( mv,8,'f',2,QLatin1Char( ' ' ))+" ";
     S = S + "\n";	 
   }
 //
 dockKeyWidget->setWindowTitle(tr("results of measurements"));
 QFont f("Courier", 8, QFont::Bold);
 keyLabel->setFont(f);
 keyLabel->setTextFormat(Qt::PlainText);
 keyLabel->setText(S);
 keyLabel->adjustSize();
 dockKeyWidget->show();
 QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(S);
} 
};


void MainWindow::slotInvChanged(int inv_v)
{
if (img_status!=1)
  {
   if (imgF.inv != invCheckBox->checkState())
   {
	   double gfactor = 1+4*gammaSlider->value()/100;
	   imgF.inv = invCheckBox->checkState();
	   if (img) emitBC((double)contrastSlider->value(),(double)brightnessSlider->value(),gfactor,invCheckBox->checkState());
   }
  }
};


bool MainWindow::close()
{
     if (img) delete img; 
     //
	 ccdThread->exit();
     if (QMainWindow::close())
     return true;
     else return false;
};//close()


void MainWindow::slotFramesAct()
{
dockFramesWidget->show();
};


void MainWindow::slotAddFrameButtonClicked()
{
emit signalAddMFrame();
};



void MainWindow::slotMFrameAdded()
{
   comboFrames -> addItem(imgF.measured_file_name+"_"+QString( "%1" ).arg(comboFrames->count()+1,2,'f',0,QLatin1Char( '0' )));
};

void MainWindow::slotDelFrameButtonClicked()
{
   if (comboFrames->count()!=0)
    {
	    int fr = comboFrames->findText(refFrameName);
	    if ((fr!=-1)&&(fr==comboFrames->currentIndex()))
		{
		  refFrameLabel->setText(tr("reference CCD frame = "));
		  refFrameName = "";
		}
        emit signalDelMFrame(comboFrames -> currentIndex());
        comboFrames ->removeItem(comboFrames -> currentIndex());
	}
};

void MainWindow::slotComboFramesChanged(int index)
{
   emit signalComboFramesChanged(index);
};


void MainWindow::slotSetFrameButtonClicked()
{
  if (comboFrames->count()!=0)
    {
       refFrameLabel->setText(tr("reference CCD frame = ")+comboFrames->currentText());
       refFrameName	= comboFrames->currentText();
	   //imgF.refCCDFrame = comboFrames -> currentIndex();
	}  
};


void MainWindow::slotStartCalcButtonClicked()
{
   /*int fr = comboFrames->findText(refFrameName);
   if (fr!=-1)
   {
     imgF.max_residual = maxresSpinBox->value();
     emit signalStartProc1(fr);
   }*/
   emit signalExecute(comLineEdit->text());
};

void MainWindow::slotProcCompleted()
{
 dockKeyWidget->setWindowTitle(tr("results of calculations"));
 QFont f("Courier", 8, QFont::Bold);
 keyLabel->setFont(f);
 keyLabel->setTextFormat(Qt::PlainText);
 keyLabel->setText(imgF.simple_message);
 keyLabel->adjustSize();
 dockKeyWidget->show();
 QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(imgF.simple_message);
};

void MainWindow::slotTelnetAct()
{
   dockTelnetWidget->show();
};


void MainWindow::slotPropAct()
{
   dockPropWidget->show();
};

void MainWindow::slotMfcButtonClicked()
{
	//QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
	//imgF.fov = 1200000;
    imgF.flength = flSpinBox->value();
    imgF.xpsize  = xpsSpinBox->value();
    imgF.ypsize  = ypsSpinBox->value();
	imgF.fov  = fovSpinBox->value()*60000;
	rot_angle = rotAngSpinBox->value()*atan(1)*4/180;
	emit signalGetMarks(radeLineEdit->text());
};

void MainWindow::slotTangPosCalculated()
{
    //QString str;
	//QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
    objl.clear();
	refToStar.clear();
	QPoint pnt;
	double ksi,eta;
	for(int i = 0; i<imgF.p_tangpos->size1(); i++)
	{
	   ksi = MEFP(*imgF.p_tangpos,i,0)*cos(rot_angle)+MEFP(*imgF.p_tangpos,i,1)*sin(rot_angle);
	   eta = -MEFP(*imgF.p_tangpos,i,0)*sin(rot_angle)+MEFP(*imgF.p_tangpos,i,1)*cos(rot_angle);
	   //str.setNum(ksi);
	   //QMessageBox::information(0,"debug",str,QMessageBox::Ok,0,0);
	   pnt.setX(imgF.xSize/2 + (int)(ksi/(206.265*imgF.xpsize/imgF.flength)));
	   pnt.setY(imgF.ySize/2 - (int)(eta/(206.265*imgF.ypsize/imgF.flength)));
	   objl << pnt;
	   refToStar << i;
	}
	////
	if(imgF.p_cel_pos->size1()!=0)
	{
		m_objl.clear();
		vector<double> EP(2);
		for(int i = 0; i<imgF.p_cel_pos->size1(); i++)
		{
			EP = getRaDeToTang(MEFP(*imgF.p_cel_pos,i,0),
		                                    	MEFP(*imgF.p_cel_pos,i,1), 
												imgF.ra_oc, imgF.de_oc);
			ksi = EP(0)*cos(rot_angle)+EP(1)*sin(rot_angle);
			eta = -EP(0)*sin(rot_angle)+EP(1)*cos(rot_angle);
			pnt.setX(imgF.xSize/2 + (int)(ksi/(206.265*imgF.xpsize/imgF.flength)));
			pnt.setY(imgF.ySize/2 - (int)(eta/(206.265*imgF.ypsize/imgF.flength)));
			m_objl << pnt;
		}
	}
	////
	deltaX=0;
	deltaY=0;
	rot_angle = 0;
	slotUpdateImg();
};

void MainWindow::slotMoveCheckBoxStateChanged(int state)
{
   if (moveCheckBox->checkState() == Qt::Checked)
   {
     imageLabel->setCursor(Qt::ClosedHandCursor);   
   }
   else
   {
     imageLabel->setCursor(Qt::CrossCursor);
   }
};


void MainWindow::slotSaveAsJPG()
{
  if (img) 
   {
     QString FName = QFileDialog::getSaveFileName(
                    this,
                    tr("Enter file name"),
                    "",
                    "jpeg image (*.jpg)");
	 img->save(FName,"JPG",100);
   }
};


void MainWindow::slotCurrDirButtonClicked()
{
currDir = QFileDialog::getExistingDirectory(this, tr("Set Current Folder"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
 currDirLabel->setText("Current folder = "+currDir);
 settings->beginGroup("general");
 settings->setValue("curr_dir", currDir);	
 settings->endGroup();

}

void MainWindow::slotUcacDirButtonClicked()
{
imgF.ucac2_path = QFileDialog::getExistingDirectory(this, tr("Set UCAC2 Folder"),
                                                 currDir,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
 ucacDirLabel->setText("ucac2 folder = "+imgF.ucac2_path);
 settings->beginGroup("catalogs");
 settings->setValue("ucac2_path", imgF.ucac2_path);	
 settings->endGroup();
}
void MainWindow::slotLogDirButtonClicked()
{
imgF.log_path = QFileDialog::getExistingDirectory(this, tr("Set LOG Folder"),
                                                 currDir,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
 logDirLabel->setText("log folder = "+imgF.log_path);
 settings->beginGroup("general");
 settings->setValue("log_path", imgF.log_path);	
 settings->endGroup();
}

void MainWindow::slotUpdateHistButtonClicked()
{
 updateHist();
}

void MainWindow::slotLeftButtonClicked()
{
  emit signalLeftFrame();
}
void MainWindow::slotRightButtonClicked()
{
  emit signalRightFrame();
}

void MainWindow::slotAngleChanged(double ang)
{
rot_angle = ang*atan(1)*4/180;

};

void MainWindow::slotInfoButtonClicked()
{
//QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
 int N = imgF.p_refcat->size2();
 if ((fL == 0)&&(fX!=0)&&(fY!=0)&&(N!=0))
 {
	  QString str = "";
	  QString strel;
	  int N = imgF.p_refcat->size2();
	  int K = refToStar[fN];
	  double rfcv;
	  for (int i = 0; i<N;i++)
	  {
	    rfcv = MEFP(*imgF.p_refcat,K,i);
		if (i==0) str = str + mas_to_hms(rfcv, ":", 2)+ " | ";
		if (i==1) str = str + mas_to_damas(rfcv, ":", 3)+ " | ";
		if (i>1)
		{
		  strel.setNum(rfcv);
		  str = str + strel + " | ";
		}
	  }
	  statusBar()->showMessage(str);
  }
};

void MainWindow::slotTelescopeChanged(int index)
{
  actTel = index;
  //
  switch(index)
		 {
		   case 0://NA
				  {
				    flSpinBox->setValue(3467);
					xpsSpinBox->setValue(16);
					ypsSpinBox->setValue(16);
					fovSpinBox->setValue(25);
					rotAngSpinBox->setValue(0);
				    obsCode->setText("084");
				  break;
				  }
		   case 1://FTN
		          {
				    flSpinBox->setValue(20000);
					xpsSpinBox->setValue(27);
					ypsSpinBox->setValue(27);
					fovSpinBox->setValue(10);
					rotAngSpinBox->setValue(180);
					obsCode->setText("F65");
				  break;
		          }
		 }
	imgF.flength = flSpinBox->value();
	imgF.xpsize  = xpsSpinBox->value();
	imgF.ypsize  = ypsSpinBox->value();
	imgF.fov  = fovSpinBox->value()*60000;
    rot_angle = rotAngSpinBox->value()*atan(1)*4/180;
  settings->setValue("telescopes/activ_telescope", index);
   imgF.obs_code = obsCode->text();  
};

void MainWindow::slotSendButtonClicked()
{
    //QString request = telnetRequest->text()+"\n";
    //QByteArray baRequest = request.toAscii();
    //char *buffer = baRequest.data();
	
	//imgF.keyStr = rep;
    // slotKeyAct(); 
	//QTextStream in(horizon);
};

void MainWindow::slotConnectButtonClicked()
{
   horizon->connectToHost(telnetHost->text(),(quint16)telnetPort->value(),QIODevice::ReadWrite);
};

void MainWindow::slotDisconnectButtonClicked()
{
  horizon->disconnectFromHost();
};

void MainWindow::slotConnected()
{
  imgF.keyStr = "connected";
  slotKeyAct();
  qint64 bsize = horizon->bytesAvailable();
  char buffer[bsize];
  horizon->read(buffer, bsize);
  imgF.keyStr = QString(buffer);
  slotKeyAct();
  /*
  char buffer[80];
  forever {
         horizon->read(buffer, 80);
         if (!horizon->waitForReadyRead())
             break;
  }
  imgF.keyStr = QString(buffer);
  slotKeyAct();  
  */
};



void MainWindow::slotDisconnected()
{
  imgF.keyStr = "disconnected";
  slotKeyAct();
};

void MainWindow::slotEphButtonClicked()
{
  DATEOBS dObs = getDATEOBSfromMJD(imgF.curr_mjd);
  QString str = "?an="+ QString( "%1" ).arg( dObs.year,4,10,QLatin1Char( '0' ))+
  "&mois="+QString( "%1" ).arg( dObs.month,4,10,QLatin1Char( '0' ))+
  "&jour="+QString( "%1" ).arg( dObs.day,4,10,QLatin1Char( '0' ))+
  "&heure="+QString( "%1" ).arg( dObs.hour,4,10,QLatin1Char( '0' ))+
  "&minute="+QString( "%1" ).arg( dObs.min,4,10,QLatin1Char( '0' ))+
  "&seconde="+QString( "%1" ).arg( dObs.sec,6,'f',3,QLatin1Char( '0' ))+
  "&numaster="+QString( "%1" ).arg( sbAstNum->value(),6,10,QLatin1Char( '0' ))+
  "&UAI_code="+obsCode->text()+
  "&planete=Aster&nbdates=1&centre=4";
  QByteArray requestData = str.toAscii();
  if (useProxy->checkState() == Qt::Checked) http->setProxy(proxyHost->text(), proxyPort->value());
   http->setHost("www.imcce.fr");    
   //http->post("/cgi-bin/ephepos.cgi/calcul", requestData);
   http->get("/cgi-bin/ephepos.cgi/calcul"+str);
   statusBar()->showMessage("connecting to www.imcce.fr...");
};

void MainWindow::progressDataFromHttp()
{ //QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
    QByteArray httpData = http->readAll();
	QString str(httpData);
	QString sd;
	double ra,de;
	/////////////////////
	int k1 = str.lastIndexOf("km/s</font></span> <br /></strong><strong><font face='Courier New,Courier'>");
	int k2 = str.lastIndexOf("</font> <br /></strong><strong><span class='texte3'>");
	k1 = k1 + 76;
	str = str.mid(k1, k2-k1);
	str = str.trimmed();
	str = str.replace("  "," ");
	str = str.replace("  "," ");
	///////////////////////
	QStringList sl;
	detObjList << QString( "%1" ).arg( sbAstNum->value(),6,10,QLatin1Char( '0' ))+" "+str;
	mpcObjLineEdit -> setText(QString( "%1" ).arg( sbAstNum->value(),5,10,QLatin1Char( '0' )));
	imgF.objStrList << mpcObjLineEdit->text();
	imgF.p_cel_pos->resize(detObjList.count(),12);
	lwObjList->clear();
	for (int i=0;i<detObjList.count();i++)
	{
	   str = detObjList[i];
	   lwObjList->addItem(str);
	   sl = str.split(" ");
	   sd = sl[sl.count()-1]; imgF.p_cel_pos->insert_element(i,10,sd.toDouble());
	   sd = sl[sl.count()-2]; imgF.p_cel_pos->insert_element(i,9,sd.toDouble());
	   sd = sl[sl.count()-3]; imgF.p_cel_pos->insert_element(i,8,sd.toDouble());
	   sd = sl[sl.count()-4]; imgF.p_cel_pos->insert_element(i,7,sd.toDouble());
	   sd = sl[sl.count()-5]; imgF.p_cel_pos->insert_element(i,6,sd.toDouble());
	   sd = sl[sl.count()-6]; imgF.p_cel_pos->insert_element(i,5,sd.toDouble());
	   sd = sl[sl.count()-7]; imgF.p_cel_pos->insert_element(i,4,sd.toDouble());
	   imgF.p_cel_pos->insert_element(i,3,imgF.curr_mjd);
	   imgF.p_cel_pos->insert_element(i,2,(double)sbAstNum->value());
	   sd = sl[sl.count()-10]+":"+sl[sl.count()-9]+":"+sl[sl.count()-8];
	   //QMessageBox::information(0,"debug",sd,QMessageBox::Ok,0,0);
	   de = damas_to_mas(sd, ":");
	   sd = sl[sl.count()-13]+":"+sl[sl.count()-12]+":"+sl[sl.count()-11];
	   //QMessageBox::information(0,"debug",sd,QMessageBox::Ok,0,0);
	   ra = hms_to_mas(sd, ":");
	   imgF.p_cel_pos->insert_element(i,1,de);
	   imgF.p_cel_pos->insert_element(i,0,ra);
	}
	////////////////////
	//imgF.keyStr = str;
    //slotKeyAct();
	QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(str);
	statusBar()->showMessage("");
};

void MainWindow::slotClearObjButtonClicked()
{
  lwObjList->clear();
  detObjList.clear();
  imgF.objStrList.clear();
  imgF.p_cel_pos->resize(1,12);
}

void MainWindow::slotToClipboardButtonClicked()
{
    QString str = QString( "%1" ).arg(imgF.curr_mjd,15,'f',7,QLatin1Char( ' ' ));
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(str.trimmed());
};

void MainWindow::slotFromClipboardButtonClicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    QString str = clipboard->text();
	str = str.trimmed();
	str = str.replace("  "," ");
	str = str.replace("  "," ");
	//
	QString sd;
	double ra,de;
	QStringList sl;
	detObjList << str;
	imgF.objStrList << mpcObjLineEdit->text();
	imgF.p_cel_pos->resize(detObjList.count(),12);
	lwObjList->clear();
	for (int i=0;i<detObjList.count();i++)
	{
	   str = detObjList[i];
	   lwObjList->addItem(imgF.objStrList[i]+" "+str);
	   sl = str.split(" ");
	   imgF.p_cel_pos->insert_element(i,3,imgF.curr_mjd);
	   sd = sl[0]+":"+sl[1]+":"+sl[2];
	   ra = hms_to_mas(sd, ":");
	   sd = sl[3]+":"+sl[4]+":"+sl[5];
	   de = damas_to_mas(sd, ":");
	   imgF.p_cel_pos->insert_element(i,1,de);
	   imgF.p_cel_pos->insert_element(i,0,ra);
	}
	//
	if (setOcCheckBox->checkState() == Qt::Checked)
	    {
	      radeLineEdit->setText(mas_to_hms(ra, " ",3)+" "+mas_to_damas(de, " ",2)+" 09.0 15.0");
		  emit signalUpdateOC(ra,de);
		}
	statusBar()->showMessage(mas_to_hms(ra,":",3)+" "+mas_to_damas(de,":",2));
};

void MainWindow::slotProgressDialogShow()
{
   progressDialog->show();
};

void MainWindow::slotProgressDialogHide()
{
   progressDialog->hide();
};

////////////////////////////////////////////////////////////////////////
//mainwindow events
//////////////////////////////////////////////////////////////////////////
void MainWindow::resizeEvent(QResizeEvent *event)
{
 emitUpdateImg();  
 QMainWindow::resizeEvent(event);
};


//events filter and events holders

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
   if (obj == dockScWidget) {
            if (event->type() == QEvent::Close) {
                imageLabel->setCursor(Qt::ArrowCursor);
                return true;}
   }//dockScWidget
   if (obj == imageLabel) {
            if (event->type() == QEvent::MouseMove) {
                //QMessageBox::information(0,"debug","image has not been loaded",QMessageBox::Ok,0,0);
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                imageLabelMouseMove(mouseEvent);
                return true;
            }
            if (event->type() == QEvent::MouseButtonPress)
            {
			  QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
			  //QMessageBox::information(0,"debug","start",QMessageBox::Ok,0,0);
			  startX = mouseEvent->x();
			  startY = mouseEvent->y();
			  if (moveCheckBox->checkState() != Qt::Checked) imageMeasureClick(mouseEvent);
			}
            if ((event->type() == QEvent::MouseButtonRelease)&&(moveCheckBox->checkState() == Qt::Checked))
            {
			   int i,x,y;
			   QPoint pnt;
			   if (rbMoveAll->isChecked())
			   {
				   
				   for (i=0;i<objl.count();i++)
				   {
					 pnt = objl[i];
					 x = pnt.x();
					 y = pnt.y();
					 pnt.setX((int)(x*cos(rot_angle)+y*sin(rot_angle)+deltaX));
					 pnt.setY((int)(-x*sin(rot_angle)+y*cos(rot_angle)+deltaY));
					 objl[i] = pnt;
				   }
				   for (i=0;i<m_objl.count();i++)
				   {
					 pnt = m_objl[i];
					 x = pnt.x();
					 y = pnt.y();
					 pnt.setX((int)(x*cos(rot_angle)+y*sin(rot_angle)+deltaX));
					 pnt.setY((int)(-x*sin(rot_angle)+y*cos(rot_angle)+deltaY));
					 m_objl[i] = pnt;
				   }
				   if ((fX!=0)&&(fY!=0))
				   {
				     if (fL==0)
					   {
					      pnt = objl[fN];
						  fX = pnt.x(); fY = pnt.y();
					   }
					   else
					   {
					      pnt = m_objl[fN];
						  fX = pnt.x(); fY = pnt.y();
					   }
				   }
			   }
			   else
			   {
				   if ((fX!=0)&&(fY!=0))
				   {
					   if (fL==0)
					   {
					      pnt = objl[fN];
						  x = pnt.x();
						  y = pnt.y();
					      pnt.setX((int)(x*cos(rot_angle)+y*sin(rot_angle)+deltaX));
						  pnt.setY((int)(-x*sin(rot_angle)+y*cos(rot_angle)+deltaY));
					      objl[fN] = pnt;
						  fX = pnt.x(); fY = pnt.y();
					   }
					   else
					   {
					      pnt = m_objl[fN];
						  x = pnt.x();
						  y = pnt.y();
					      pnt.setX((int)(x*cos(rot_angle)+y*sin(rot_angle)+deltaX));
						  pnt.setY((int)(-x*sin(rot_angle)+y*cos(rot_angle)+deltaY));
					      m_objl[fN] = pnt;
						  fX = pnt.x(); fY = pnt.y();
					   }
				   }
			   }
			   deltaX=0;
			   deltaY=0;
			   rot_angle = 0;
			   slotUpdateImg();
			}			
   }//imageLabel
   if (obj == scaledLabel) {
            if (event->type() == QEvent::MouseMove) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                scaledLabelMouseMove(mouseEvent);
                return true;
            } 
   }//imageLabel
   if ((obj == this)||(obj == dockKeyWidget)) {
            if (event->type() == QEvent::KeyPress) {
                QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
                imageNavigation(keyEvent);
                return true;
            }
   }//imageLabel
   if (obj == dockKeyWidget)
   {
      if (event->type() == QEvent::Resize)
	  {
	    QSize saSize(dockKeyWidget->width()-25,dockKeyWidget->height()-25);
		QSize kwSize(dockKeyWidget->width()-20,dockKeyWidget->height()-20);
		keyScrollArea ->resize(saSize);
		keyWidget ->resize(kwSize);
		return true;
	  }
   }
//==============================================================================
   if (obj == histLabel) {
            if (event->type() == QEvent::MouseButtonPress) 
			{
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
				int wh = histLabel->width();
				int hh = histLabel->height();
				if(firstClick)
				{
				  histX1 = mouseEvent->x();
				  firstClick = false;
				}
				else
				{
				  histX2 = mouseEvent->x();
				  firstClick = true;
				}
				if (img) 
				{
				   if(histX1<5)histX1 = 5;
				   if(histX2<5)histX2 = 5;
				   if(histX1>(wh-5))histX1 = wh-5;
				   if(histX2>(wh-5))histX2 = wh-5;
				   updateHist();
				   if (histX1<histX2)
				   {
					 imgF.leftB = imgF.ADCmin + (double)(histX1 - 5)*(imgF.ADCmax - imgF.ADCmin)/(wh-10);
					 imgF.rightB = imgF.ADCmin + (double)(histX2 - 5)*(imgF.ADCmax - imgF.ADCmin)/(wh-10);
				   }
				   else
				   {
					 imgF.leftB = imgF.ADCmin + (double)(histX2 - 5)*(imgF.ADCmax - imgF.ADCmin)/(wh-10);
					 imgF.rightB = imgF.ADCmin + (double)(histX1 - 5)*(imgF.ADCmax - imgF.ADCmin)/(wh-10);
				   }
					 contrastSlider->setRange((int)imgF.leftB,(int)imgF.rightB);
					 //imgF.lHist = imgF.leftB + (imgF.rightB-imgF.leftB)/2;
					 contrastSlider->setValue((int)imgF.leftB);
					 brightnessSlider->setRange((int)imgF.leftB,(int)imgF.rightB);
					 brightnessSlider->setValue((int)imgF.rightB);				   
                }				
                return true;
            } 
   }//imageLabel

//===============================================================================   
   return QMainWindow::eventFilter(obj, event);
};


void MainWindow::imageLabelMouseMove(QMouseEvent *mouseEvent)
{
   int x_pos = (int)(scfactor*mouseEvent->x());
   int y_pos = (int)(scfactor*mouseEvent->y());
   int scw = (int) 200/scspinBox->value();
   if ((x_pos>0)&&(x_pos<imgWidth)&&(y_pos>0)&&(y_pos<imgHeight)){
	   DrawImageInScaledLabel(x_pos, y_pos, scw);
	   mainX = x_pos;
	   mainY = y_pos;
	   currX = mouseEvent->x();
	   currY = mouseEvent->y();
	   if (moveCheckBox->checkState() == Qt::Checked)
	   {
	      if (mouseEvent->modifiers () == Qt::ControlModifier)
		  {
		    rot_angle = -atan((currY - startY)/fabs(currX));
			deltaX = 0;
	        deltaY = 0;
		  }
		  else
		  {
	        deltaX = (int)(scfactor*(currX - startX));
	        deltaY = (int)(scfactor*(currY - startY));
			rot_angle = 0;
		  }
	   }
	   slotUpdateImg();
   }//end if
};

void MainWindow::scaledLabelMouseMove(QMouseEvent *mouseEvent)
{
   if (img!=NULL){
     int scw = (int) 200/scspinBox->value(); 
     int x_pos = (int)(mouseEvent->x()/scspinBox->value())+mainX-scw/2;
     int y_pos = (int)(mouseEvent->y()/scspinBox->value())+mainY-scw/2;
     if ((x_pos>=0)&&(x_pos<imgWidth)&&(y_pos>=0)&&(y_pos<imgHeight))
     {
        double valueI = PointData(imgF.fCCD,x_pos,y_pos, imgF.xSize);
		//////////////////////////////////
		QString StatusStr("X = ");
		StatusStr = StatusStr+QString( "%1" ).arg( x_pos,4,10,QLatin1Char( ' ' ))+"; Y = ";
		StatusStr = StatusStr+QString( "%1" ).arg( y_pos,4,10,QLatin1Char( ' ' ))+"; ADC = ";
		StatusStr = StatusStr+QString( "%1" ).arg( valueI,8,'f',2,QLatin1Char( ' ' ));
		statusSW->showMessage(StatusStr);
     }
        else {
		  statusSW->showMessage("NULL");
        }
     }//end if
};

void MainWindow::imageMeasureClick(QMouseEvent *mouseEvent)
{
   //if (mouseEvent->button()==Qt::LeftButton)
   //{
    int x_pos = (int)(scfactor*mouseEvent->x());
    int y_pos = (int)(scfactor*mouseEvent->y());
    int scw = (int) 200/scspinBox->value();
   if ((x_pos>0)&&(x_pos<imgWidth)&&(y_pos>0)&&(y_pos<imgHeight)){
	   DrawImageInScaledLabel(x_pos, y_pos, scw);
	   mainX = x_pos;
	   mainY = y_pos;
	   currX = mouseEvent->x();
	   currY = mouseEvent->y();
	   QString StatusStr;
	   int x,y;
	   if(centeringCheckBox->checkState() == Qt::Checked)
		   {
		     //STIMG st_center = StarCentering(imgF.fCCD, mainX, mainY, aperture_spinBox->value(), 2, imgF.xSize, imgF.ySize);
			 STIMG st_center = getStarCenter(imgF.fCCD, mainX, mainY, aperture_spinBox->value(), 2, imgF.xSize, imgF.ySize);
             x = (int)st_center.X;
             y = (int)st_center.Y;
		   }
		else
		   {
		     x = mainX;
			 y = mainY;
		   }
	   //Qt::AltModifier
	   CCDDATA data = ProfileData(imgF.fCCD, x, y, 20, imgF.xSize);
	   bool isObj = true;
	   if(checkObjCheckBox->checkState() == Qt::Checked)
                                      isObj = isObject(data, imgF.fr_pix);
	   delete[] data.Buffer;
	   //
	   if ((mouseEvent->modifiers () == Qt::ControlModifier)&&(mouseEvent->button()==Qt::LeftButton))
	    {
		   if (isObj)
            {		   
			  QPoint pt(x,y);
		      objl.append(pt);
              refToStar << objl.count()-1;			  
			}
		}
		if ((mouseEvent->modifiers () == Qt::AltModifier)&&(mouseEvent->button()==Qt::LeftButton))
	    {
		  if (isObj)
            {
		      QPoint mpt(x,y);
		      m_objl.append(mpt);
			}
		}
	   if ((mouseEvent->modifiers () == Qt::ShiftModifier)&&(mouseEvent->button()==Qt::LeftButton))
        {
		   int N = getNearest(objl, mainX, mainY, 10);
		   if (N!=-1) 
		     {
		        objl.remove(N);
				refToStar.remove(N);
			 }
		   N = getNearest(m_objl, mainX, mainY, 10);
		   if (N!=-1) m_objl.remove(N);
		   //
		   N = getNearest(objl, fX, fY, 10);
		   if (N!=-1)
			  {
				 fX = objl[N].x();
				 fY = objl[N].y();
				 fL = 0;
				 fN = N;
			  }		  
		   else
			  {
			  N = getNearest(m_objl, fX, fY, 10);
			  if (N!=-1)
				{
				 fX = m_objl[N].x();
				 fY = m_objl[N].y();
				 fL = 1;
				 fN = N;
				 }
              else
               {
			     fX = 0;
				 fY = 0;
				 fN = 0;
				 fL = 0;
			   }			  
			  }
		   //
        }
		if (mouseEvent->button()==Qt::RightButton)
        {
		   if (mouseEvent->modifiers () == Qt::ControlModifier)
		   {
		     fX = 0;
			 fY = 0;
			 fN = 0;
			 fL = 0;
		   }
		   else
		   {
			   int N = getNearest(objl, mainX, mainY, 10);
			   if (N!=-1)
	              {
				     fX = objl[N].x();
					 fY = objl[N].y();
					 fL = 0;
					 fN = N;
				  }		  
			   else
			      {
			      N = getNearest(m_objl, mainX, mainY, 10);
			      if (N!=-1)
	                {
				     fX = m_objl[N].x();
					 fY = m_objl[N].y();
					 fL = 1;
					 fN = N;
				     }				  
				  }
			}
        }
	   StatusStr.setNum(objl.count());
       statusBar()->showMessage(StatusStr);
	   slotUpdateImg();	   
	//}	 
   }//end if
}//end imageMeasureClick

void MainWindow::DrawImageInScaledLabel(int x_p, int y_p, int wide)
{
 if(img)
 {
    QImage image=img->copy(x_p-wide/2,y_p-wide/2,wide,wide);
	QImage sc_image;
	if (magnifyCheckBox->checkState() == Qt::Checked)
        sc_image = image.scaled(200,200,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    else
        sc_image = image.scaled(200,200,Qt::KeepAspectRatio,Qt::FastTransformation);
	QPixmap pix(QPixmap::fromImage(sc_image));
	if (apertureCheckBox->checkState() == Qt::Checked)
		  {
		   QPainter paint(&pix);
		   QPen pen;
		   pen.setColor(Qt::yellow);
		   pen.setWidth(2);
           paint.setPen(pen);
           int a_radius = aperture_spinBox->value();
		   int m_factor = scspinBox->value();
           paint.drawEllipse(100-m_factor*a_radius,100-m_factor*a_radius,2*m_factor*a_radius,2*m_factor*a_radius);
		  }
    if (profileCheckBox->checkState() == Qt::Checked)
		  {
		   CCDDATA profile = ProfileData(imgF.fCCD, x_p, y_p, wide,imgF.xSize);
		   /////////////////////////
		   QString StatusStr(tr("profile: Y = "));
		   StatusStr = StatusStr+QString( "%1" ).arg( y_p,4,10,QLatin1Char( ' ' ))+"; width = ";
		   StatusStr = StatusStr+QString( "%1" ).arg( wide,4,10,QLatin1Char( ' ' ))+"; mean = ";
		   StatusStr = StatusStr+QString( "%1" ).arg( profile.meanADC,6,'f',3,QLatin1Char( ' ' ))+"; se = ";
		   StatusStr = StatusStr+QString( "%1" ).arg( profile.seADC,6,'f',3,QLatin1Char( ' ' ))+"; min = ";
		   StatusStr = StatusStr+QString( "%1" ).arg( profile.minADC,6,'f',1,QLatin1Char( ' ' ))+"; max = ";
		   StatusStr = StatusStr+QString( "%1" ).arg( profile.maxADC,6,'f',1,QLatin1Char( ' ' ));
		   statusBar()->showMessage(StatusStr);
		   /////////////////////////
		   QPainter paint_p(&pix);
		   QPen pen_p;
		   pen_p.setWidth(2);
		   pen_p.setColor(Qt::yellow);
           paint_p.setPen(pen_p);
		   paint_p.drawLine ( 0, 100, 200, 100 );
		   pen_p.setColor(Qt::red);
           paint_p.setPen(pen_p);
		   double diap = (profile.maxADC - profile.minADC);
		   int peh;
		   int xx = 0;
		   if ((x_p-wide/2)<0) xx = ((int)wide/2 - x_p)*scspinBox->value();
		   int yy = 20;
		   int old_peh = yy;
		   for (int i = 0;  i<profile.Size; i++)
		     {
			  if (profile.Buffer[i].X<imgWidth)
			    {
			     peh = (int)(150*(profile.Buffer[i].I-profile.minADC)/diap);
			     paint_p.drawLine ( xx, 200-(yy+old_peh), xx, 200-(yy+peh) );
			     paint_p.drawLine ( xx, 200-(yy+peh), xx + scspinBox->value(),200-(yy+peh) );
			     xx = xx + scspinBox->value();
			     old_peh = peh;
			    }//end if
			 }//end for i
           delete [] profile.Buffer;
		  }//end profile
	if ((psfCheckBox->checkState() == Qt::Checked)&&(vRes.size()>=4))
	   {
	       CCDDATA prof = ProfileData(imgF.fCCD, x_p, y_p, 2*aperture_spinBox->value(), imgF.xSize);
		   QPainter paint_psf(&pix);
		   QPen pen_psf;
		   pen_psf.setWidth(2);
		   //////////
		   pen_psf.setColor(Qt::yellow);
		   paint_psf.setPen(pen_psf);
		   paint_psf.drawLine (0,100,200,100 );
		   pen_psf.setColor(Qt::green);
		   ///////////
		   QPen pen_d;
		   pen_d.setColor(Qt::blue);
		   pen_d.setWidth(2);
		   double diap_psf = (prof.maxADC - prof.minADC);
		   int peh_psf, psfv;
		   int xx_psf = 100-aperture_spinBox->value()*scspinBox->value();
		   if ((x_p-aperture_spinBox->value())<0) xx_psf = (aperture_spinBox->value() - x_p)*scspinBox->value();
		   int yy_psf = 20;
		   double r;
		   for (int i = 0;  i<prof.Size; i++)
		     {
			  if (prof.Buffer[i].X<imgWidth)
			    {
				  /////////////////////////
		          //QString StStr =
		          //    QString( "%1" ).arg( vRes[5],6,'f',3,QLatin1Char( ' ' ))+" ";
		          //    StStr = StStr+QString( "%1" ).arg( vRes[6],6,'f',3,QLatin1Char( ' ' ));
		          //    statusBar()->showMessage(StStr);
		          /////////////////////////
				 r = sqrt(pow(prof.Buffer[i].X-vRes[5],2)+pow(prof.Buffer[i].Y-vRes[6],2));
				 if (r<aperture_spinBox->value())
				  {
				    if(lorentzRadioButton->isChecked())
				       psfv = (int)(150*(Lorentzian(prof.Buffer[i].X,prof.Buffer[i].Y,&vRes)-prof.minADC)/diap_psf);
					if(gaussianRadioButton->isChecked())
				       psfv = (int)(150*(Gaussian(prof.Buffer[i].X,prof.Buffer[i].Y,&vRes)-prof.minADC)/diap_psf);
					if(moffatRadioButton->isChecked())
				       psfv = (int)(150*(Moffat(prof.Buffer[i].X,prof.Buffer[i].Y,&vRes)-prof.minADC)/diap_psf);
				   ///////////////////////////////////////////////////////////////////	   
			       peh_psf = (int)(150*(prof.Buffer[i].I-prof.minADC)/diap_psf);
				   paint_psf.setPen(pen_d);
			       paint_psf.drawLine ( xx_psf, 200-(yy_psf+peh_psf), xx_psf + scspinBox->value(),200-(yy_psf+peh_psf) );
				   paint_psf.setPen(pen_psf);
				   paint_psf.drawLine ( xx_psf, 200-(yy_psf+psfv), xx_psf + scspinBox->value(),200-(yy_psf+psfv) );
				   }
			       xx_psf = xx_psf + scspinBox->value();
				  
			    }//end if
			 }//end for i
	   }
	scaledLabel->setPixmap(pix);
	/////////////////////////////////////////////////////
	}//end img
}


void MainWindow::imageNavigation(QKeyEvent *keyEvent)
{
   //QMessageBox::information(0,"debug","ok",QMessageBox::Ok,0,0);
   //if (keyEvent-> modifiers () == Qt::ControlModifier)
   //{
     if (keyEvent->key() == Qt::Key_Less) emit signalLeftFrame();
	 if (keyEvent->key() == Qt::Key_Greater) emit signalRightFrame();
	 if (keyEvent->key() == Qt::Key_Delete) emit signalDeleteFrame();
   //}
};

//emit signals

void MainWindow::emitTest()
{
 emit signalTest();
};

void MainWindow::emitLoadFITS(QString FileName)
{
 emit signalLoadFITS(FileName);
};

void MainWindow::emitBC(double b, double c, double g, int inv)
{
 emit signalBC(b, c, g, inv);
};

void MainWindow::emitUpdateImg()
{
 emit signalUpdateImg();
};

void MainWindow::emitCenterOfMass(int x, int y, int r)
{
 emit signalCenterOfMass(x,y,r);
};

void MainWindow::emitApproxImg(int x, int y, int r, double delta, int psft)
{
 emit signalApproxImg(x,y,r,delta,psft);
};

void MainWindow::emitMeasureMarkedObjects(int r, double delta, int psft, int centr, int vstar)
{
 emit signalMeasureMarkedObjects(r,delta,psft,centr,vstar);
};

void MainWindow::slotThreadDone()
{QMessageBox::information(0,"debug","shutted",QMessageBox::Ok,0,0);
};
