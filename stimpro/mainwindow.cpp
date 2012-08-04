#include "mainwindow.h"
//#include "fitsdata.h"
//#include "mb.hpp"
//#include "..\libs\astro.h"
//#include "./../libs/astro.h"
//#include "./../libs/comfunc.h"
using namespace std;
//#include "./../libs/mb.h"
//QList <double*> tL;




MainWindow::MainWindow()
{
    setWindowTitle("stimpro");
    setMenu();
    setWidgets();
    setSettings();		//settings0

    slotLoadDefaultPanels();
    
    mainFrame = 0;
    rbMouse = 0;
    lbMouse = 0;
    xM = yM = 0;
    //mag_1 = 6.0;
    //mag_2 = 15;
    ctrlB = 0;
    rbMouse = 0;
    lbMouse = 0;

    obsList = new observatory;
    obsList->init(observatoryCat.toAscii().data(), OBS_SIZE);



   imgLabel->installEventFilter(this);
   this->installEventFilter(this);
   histLabel->installEventFilter(this);
    
    setMouseTracking(true);
    centralWidget()->setMouseTracking(true);

    slotUpdateColors();
};

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if ((obj == this)||(obj == cWidget))
    {
        if(event->type()==QCloseEvent::Close) slotExit();
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (event->type() == QEvent::KeyPress)
        {
            imageNavigation(keyEvent);
            return true;
        }
        else if (event->type() == QEvent::KeyRelease) slotKeyReleased(keyEvent);
    }//imageLabel
     if (obj == histLabel) {
            if (event->type() == QEvent::MouseButtonPress) 
			{
				fitsdata *fitsd = imgLabel->getCurFits();
				if(fitsd!=NULL)
				{
					
	                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
					int wh = histLabel->width();
					int hh = histLabel->height();
					if(mouseEvent->button()==Qt::LeftButton)
					{
					  fitsd->minp = mouseEvent->x();
					}
					else if(mouseEvent->button()==Qt::RightButton)
					{
					  fitsd->maxp = mouseEvent->x();
					}
                                        levelBinSlider->setRange(fitsd->minp, 3.0*fitsd->maxp);
                                        levelBinSlider->setValue(fitsd->maxp);
				}
/*				fitsdata *fitsd = imgLabel->getCurFits(); 
				if (fitsd!=NULL) 
				{
				   if(fitsd->minp<5)fitsd->minp = 5;
				   if(fitsd->maxp<5)fitsd->maxp = 5;
				   if(fitsd->minp>(wh-5))fitsd->minp = wh-5;
				   if(fitsd->maxp>(wh-5))fitsd->maxp = wh-5;
				   updateHist();
				   if (fitsd->minp<fitsd->maxp)
				   {
					 imgF.lHist = imgF.ADCmin + (double)(fitsd->minp - 5)*(imgF.ADCmax - imgF.ADCmin)/(wh-10);
					 imgF.rHist = imgF.ADCmin + (double)(histX2 - 5)*(imgF.ADCmax - imgF.ADCmin)/(wh-10);
				   }
				   else
				   {
					 imgF.lHist = imgF.ADCmin + (double)(histX2 - 5)*(imgF.ADCmax - imgF.ADCmin)/(wh-10);
					 imgF.rHist = imgF.ADCmin + (double)(histX1 - 5)*(imgF.ADCmax - imgF.ADCmin)/(wh-10);
				   }
				     imgF.leftB = imgF.lHist;
					 imgF.rightB = imgF.rHist;
					 contrastSlider->setRange((int)imgF.leftB,(int)imgF.rightB);
					 contrastSlider->setValue((int)imgF.leftB);
					 brightnessSlider->setRange((int)imgF.leftB,(int)imgF.rightB);
					 brightnessSlider->setValue((int)imgF.rightB);				   
                }				
                */
                //updateHist();
                slotScrUpdate();
                return true;
		} 
	}
    return QMainWindow::eventFilter(obj, event);
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

void MainWindow::setMenu()
{
    //BEGIN menu "file"
    fileMenu = menuBar()->addMenu(tr("file"));// aiaaaeyai iai? file
    //add action "open fits"
    openFitsAct = new QAction(tr("&open fits"), this);// aiaaaeyai "aaenoaea" open fits
    openFitsAct->setShortcut(tr("Ctrl+O"));//iacia?aai "ai?y?o? eeaaeoo" Ctrl+O aey aaenoaey open fits
    openFitsAct->setStatusTip(tr("open fits file"));// iacia?aai "iianeaceo"
    fileMenu->addAction(openFitsAct);
    connect(openFitsAct, SIGNAL(triggered()), this, SLOT(slotOpenFitsFile()));// iacia?aai neio, eioi?ue aoaao auiieiyouny i?e auai?a aaiiiai ioieoa iai?
    //
    openEmptyAct = new QAction(tr("&open empty"), this);
    openFitsAct->setStatusTip(tr("open empty"));// iacia?aai "iianeaceo"
    fileMenu->addAction(openEmptyAct);
    connect(openEmptyAct, SIGNAL(triggered()), this, SLOT(slotOpenEmpty()));
    //
    saveFitsAct = new QAction(tr("&save fits"), this);// aiaaaeyai "aaenoaea" open fits
    saveFitsAct->setShortcut(tr("Ctrl+S"));//iacia?aai "ai?y?o? eeaaeoo" Ctrl+O aey aaenoaey open fits
    saveFitsAct->setStatusTip(tr("save fits file"));// iacia?aai "iianeaceo"
    fileMenu->addAction(saveFitsAct);
    connect(saveFitsAct, SIGNAL(triggered()), this, SLOT(slotSaveFitsFile()));
    //
    //
    saveFitsAsAct = new QAction(tr("&save fits as..."), this);// aiaaaeyai "aaenoaea" open fits
    //saveFitsAsAct->setShortcut(tr("Ctrl+S"));//iacia?aai "ai?y?o? eeaaeoo" Ctrl+O aey aaenoaey open fits
    saveFitsAsAct->setStatusTip(tr("save fits file as..."));// iacia?aai "iianeaceo"
    fileMenu->addAction(saveFitsAsAct);
    connect(saveFitsAsAct, SIGNAL(triggered()), this, SLOT(slotSaveFitsFileAs()));

    //
    fileMenu->addSeparator();
    // inoaeuiua ?acaaeu iai? e eo "aaenoaey" (iioee) aiaaaey?ony ii oie ?a noaia, ?oi e open fits
    viewFMAct = new QAction(tr("&File Manager"), this);//
    viewFMAct->setShortcut(tr("Ctrl+M"));
    fileMenu->addAction(viewFMAct);
    connect(viewFMAct, SIGNAL(triggered()), this, SLOT(slotFrameManagerAct()));
    //
    fileMenu->addSeparator();
    //
    loadHeaderAct = new QAction(tr("&load header"), this);
    loadHeaderAct->setStatusTip(tr("load header"));// iacia?aai "iianeaceo"
    fileMenu->addAction(loadHeaderAct);
    connect(loadHeaderAct, SIGNAL(triggered()), this, SLOT(slotLoadHeader()));
    //
    //
    loadHttpHeaderAct = new QAction(tr("&load http header"), this);
    loadHttpHeaderAct->setStatusTip(tr("load http header"));// iacia?aai "iianeaceo"
    fileMenu->addAction(loadHttpHeaderAct);
    connect(loadHttpHeaderAct, SIGNAL(triggered()), this, SLOT(slotLoadHttpHeader()));
    //
    fileMenu->addSeparator();
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

    //add action "exit"
    exitAct = new QAction(tr("&exit"), this);
    exitAct->setShortcut(tr("Ctrl+X"));
    exitAct->setStatusTip(tr("exit"));
    fileMenu->addAction(exitAct);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(slotExit()));
    //END menu "file"

    /////////////////////////////////////////////////////////////////////////////////
    //BEGIN menu "view"
    viewMenu = menuBar()->addMenu(tr("view"));
    //add action "increase"
    incAct = new QAction(tr("&increase"), this);
    incAct->setShortcut(tr("Ctrl+="));
    incAct->setStatusTip(tr("increase"));
    viewMenu->addAction(incAct);
    connect(incAct, SIGNAL(triggered()), this, SLOT(slotIncrease()));
    //add action "decrease"
    decAct = new QAction(tr("&decrease"), this);
    decAct->setShortcut(tr("Ctrl+-"));
    decAct->setStatusTip(tr("decrease"));
    viewMenu->addAction(decAct);
    connect(decAct, SIGNAL(triggered()), this, SLOT(slotDecrease()));
    //add action "background left"
    backLeftAct = new QAction(tr("&background left"), this);
    backLeftAct->setShortcut(tr("Ctrl+F1"));
    backLeftAct->setStatusTip(tr("background left"));
    viewMenu->addAction(backLeftAct);
    connect(backLeftAct, SIGNAL(triggered()), this, SLOT(slotBackLeft()));
    //add action "background right"
    backRightAct = new QAction(tr("&background right"), this);
    backRightAct->setShortcut(tr("Ctrl+F2"));
    backRightAct->setStatusTip(tr("background right"));
    viewMenu->addAction(backRightAct);
    connect(backRightAct, SIGNAL(triggered()), this, SLOT(slotBackRight()));
    //add action "signal left"
    signalLeftAct = new QAction(tr("&signal left"), this);
    signalLeftAct->setShortcut(tr("Ctrl+F3"));
    signalLeftAct->setStatusTip(tr("signal left"));
    viewMenu->addAction(signalLeftAct);
    connect(signalLeftAct, SIGNAL(triggered()), this, SLOT(slotSignalLeft()));
    //add action "signal right"
    signalRightAct = new QAction(tr("&signal right"), this);
    signalRightAct->setShortcut(tr("Ctrl+F4"));
    signalRightAct->setStatusTip(tr("signal right"));
    viewMenu->addAction(signalRightAct);
    connect(signalRightAct, SIGNAL(triggered()), this, SLOT(slotSignalRight()));
    //add action "set default"
    setDefaultAct = new QAction(tr("&set default"), this);
    setDefaultAct->setShortcut(tr("Ctrl+F5"));
    setDefaultAct->setStatusTip(tr("set default"));
    viewMenu->addAction(setDefaultAct);
    connect(setDefaultAct, SIGNAL(triggered()), this, SLOT(slotSetDefault()));
    //add action "set full diapason"
    setFullDiapasonAct = new QAction(tr("&set full diapason"), this);
    setFullDiapasonAct->setShortcut(tr("Ctrl+F6"));
    setFullDiapasonAct->setStatusTip(tr("set full diapason"));
    viewMenu->addAction(setFullDiapasonAct);
    connect(setFullDiapasonAct, SIGNAL(triggered()), this, SLOT(slotSetFullDiapason()));
	
	//
	histAct = new QAction(tr("&histogram"), this);
        histAct->setShortcut(tr("Ctrl+G"));
        histAct->setStatusTip(tr("histogram"));
     viewMenu->addAction(histAct);
     connect(histAct, SIGNAL(triggered()), this, SLOT(slotShowHist()));
    //END menu "view"

    //////////////////////////////////////
    //BEGIN menu "frame"
    frameMenu = menuBar()->addMenu(tr("frame"));
    //add action "next"
    nextAct = new QAction(tr("&next"), this);
    nextAct->setShortcut(tr("Ctrl+."));
    nextAct->setStatusTip(tr("next"));
    frameMenu->addAction(nextAct);
    connect(nextAct, SIGNAL(triggered()), this, SLOT(slotNext()));
    //add action "previous"
    prevAct = new QAction(tr("&previous"), this);
    prevAct->setShortcut(tr("Ctrl+,"));
    prevAct->setStatusTip(tr("previous"));
    frameMenu->addAction(prevAct);
    connect(prevAct, SIGNAL(triggered()), this, SLOT(slotPreviouse()));
    //add action "delete"
    delAct = new QAction(tr("&delete"), this);
    delAct->setShortcut(tr("Ctrl+Delete"));
    delAct->setStatusTip(tr("delete"));
    frameMenu->addAction(delAct);
    connect(delAct, SIGNAL(triggered()), this, SLOT(slotDelete()));

    keyAct = new QAction(tr("&FITS header"), this);
    keyAct->setShortcut(tr("Ctrl+H"));
    keyAct->setStatusTip(tr("FITS header"));
    frameMenu->addAction(keyAct);
    connect(keyAct, SIGNAL(triggered()), this, SLOT(slotKeyAct()));

    //END menu "frame"

    // menu tool
    toolMenu = menuBar()->addMenu(tr("Tools"));
    //
    //test1 action
    marksAct = new QAction(tr("&Marks"), this);
    marksAct->setStatusTip(tr("Marks"));
    marksAct->setShortcut(tr("F10"));
    toolMenu->addAction(marksAct);
    connect(marksAct, SIGNAL(triggered()), this, SLOT(slotMarks()));
    //test2 action
    cMassAct = new QAction(tr("&Mass Centre"), this);
    cMassAct->setStatusTip(tr("Mass Centre"));
    toolMenu->addAction(cMassAct);
    connect(cMassAct, SIGNAL(triggered()), this, SLOT(slotCenterMassButton()));


    getMJDAct = new QAction(tr("&Get MJD"), this);
    getMJDAct->setStatusTip(tr("Get MJD"));
    toolMenu->addAction(getMJDAct);
    connect(getMJDAct, SIGNAL(triggered()), this, SLOT(slotGetMJDfromHeader()));

    getRadecAct = new QAction(tr("&Get RA De"), this);
    getRadecAct->setStatusTip(tr("Get RA DE"));
    toolMenu->addAction(getRadecAct);
    connect(getRadecAct, SIGNAL(triggered()), this, SLOT(slotGetRaDefromHeader()));

    objAct = new QAction(tr("&Object panel"), this);
    objAct->setStatusTip(tr("Object"));
    toolMenu->addAction(objAct);
    connect(objAct, SIGNAL(triggered()), this, SLOT(slotObjectPanel()));

    findHstarsAct = new QAction(tr("&findHstars"), this);
    findHstarsAct->setStatusTip(tr("Find stars"));
    toolMenu->addAction(findHstarsAct);
    connect(findHstarsAct, SIGNAL(triggered()), this, SLOT(slotFindHstars()));

///////////////////////////////

    // menu panels
    panelsMenu = menuBar()->addMenu(tr("Panels"));
    //

    //propAct
    propAct = new QAction(tr("&properties panel"), this);
//   propAct->setShortcut(tr("Ctrl+P"));
    propAct->setStatusTip(tr("properties panel"));
    panelsMenu->addAction(propAct);
    connect(propAct, SIGNAL(triggered()), this, SLOT(slotPropAct()));
    //marksAct
    marksPanelAct = new QAction(tr("&marks panel"), this);
//    marksAct->setShortcut(tr("F10"));
    marksPanelAct->setStatusTip(tr("properties panel"));
    panelsMenu->addAction(marksPanelAct);
    connect(marksPanelAct, SIGNAL(triggered()), this, SLOT(slotMarksAct()));

    psfAct = new QAction(tr("&psf panel"), this);
//    psfAct->setShortcut(tr("F10"));
    psfAct->setStatusTip(tr("psf panel"));
    panelsMenu->addAction(psfAct);
    connect(psfAct, SIGNAL(triggered()), this, SLOT(slotPsfAct()));
/*
    conAct = new QAction(tr("&logs"), this);
    conAct->setShortcut(tr("Ctrl+L"));
    conAct->setStatusTip(tr("Logs"));
    panelsMenu->addAction(conAct);
    connect(conAct, SIGNAL(triggered()), this, SLOT(slotConAct()));
*/
    filtersAct = new QAction(tr("&Filters"), this);
    filtersAct->setShortcut(tr("Ctrl+F"));
    filtersAct->setStatusTip(tr("Filters"));
    panelsMenu->addAction(filtersAct);
    connect(filtersAct, SIGNAL(triggered()), this, SLOT(slotFilters()));

    fitsHeaderAct = new QAction(tr("&Header panel"), this);
    fitsHeaderAct->setStatusTip(tr("Header"));
    panelsMenu->addAction(fitsHeaderAct);
    connect(fitsHeaderAct, SIGNAL(triggered()), this, SLOT(slotFitsHeaderPanel()));

    wcsAct = new QAction(tr("&WCS panel"), this);
//    wcsAct->setShortcut(tr("Ctrl+F"));
    wcsAct->setStatusTip(tr("WCS"));
    panelsMenu->addAction(wcsAct);
    connect(wcsAct, SIGNAL(triggered()), this, SLOT(slotWcsWidget()));

    gStatAct = new QAction(tr("&Grid stat panel"), this);
    gStatAct->setStatusTip(tr("Grid stat"));
    panelsMenu->addAction(gStatAct);
    connect(gStatAct, SIGNAL(triggered()), this, SLOT(slotGridStatPanel()));

    setupColorAct = new QAction(tr("&Setup marks color"), this);
    setupColorAct->setStatusTip(tr("Setup marks color"));
    panelsMenu->addAction(setupColorAct);
    connect(setupColorAct, SIGNAL(triggered()), this, SLOT(slotSetupMarksColor()));

///////////////////////////////

}

void MainWindow::setWidgets()//eieoeaeecaoey aea?aoia
{
    imgLabel = new imageLabel(2000,2000,this);//aea?ao aey i?i?eniaee ecia?a?aiey "ia ye?aia"
    hscrBar = new QScrollBar(Qt::Horizontal);//ai?eciioaeuiay iieina i?ie?ooee. Qt::Horizontal - yoi cia?eo, ?oi iieina i?ie?ooee aoaao i?eaioe?iaaia ai?eciioaeuii
    hscrBar->hide();// ni?y?ai iiea yoo iieineo, aaau ioe?uouo oaeeia aua iao
    vscrBar = new QScrollBar(Qt::Vertical);//aa?oeeaeuiay iieina i?ie?ooee
    vscrBar->hide();
    cWidget = new QWidget;//oaio?aeuiue aea?ao-eiioaeia?
    cWidget->setMinimumSize(600,400);
    QGridLayout *gridLayout = new QGridLayout(cWidget);//aea?ao "naoea" eee "oaaeeoa"
    gridLayout->addWidget(imgLabel,0,0);// iiiauaai aea?ao aey i?i?eniaee ecia?a?aiey "ia ye?aia" (imgLabel) a eaaue aa?oiee oaie oaio?aeuiiai aea?aoa-eiioaeia?a
    gridLayout->addWidget(vscrBar,0,1);//ni?aaa io imgLabel iiiauaai iieineo aa?oeeaeuiie i?ie?ooee
    gridLayout->addWidget(hscrBar,1,0);//iia imgLabel iiiauaai iieineo ai?eciioaeuiie i?ie?ooee
//	cWidget->setMouseTracking(1);
//	cWidget->setEnabled(1);
//	cWidget->ignore(1);
    setCentralWidget(cWidget);//onoaiaaeeaaai oaio?aeuiue aea?ao-eiioaeia?, eae aeaaiue aea?ao iauaeoa MainWindow (aeaaiia ieii). A ?acoeuoaoa oaio?aeuiue aea?ao-eiioaeia? aoaao aaoiiaoe?anee au?aaieaaouny ii ?acia?ai aioo?aiiaai iiey aeaaiiai ieia i?e eciaiaiee aai ?acia?a
    connect(hscrBar,SIGNAL(actionTriggered(int)),this,SLOT(slotScrUpdate()));// naycuaaai niauoey (neaiaeu) eciaiaiey ninoiyiey iiein i?ie?ooee n niioaaonoao?ueie neioaie (ia?aaio?eeaie niauoee)
    connect(vscrBar,SIGNAL(actionTriggered(int)),this,SLOT(slotScrUpdate()));
//	connect(cWidget, SIGNAL(wheelEvent(QWheelEvent*)), this, SLOT(slotWheelEvent(QWheelEvent*)));
//////////////////////////////
    QWidget *propWidget = new QWidget;
    //
    comboTel = new QComboBox();
    connect(comboTel, SIGNAL(highlighted(int)), this, SLOT(slotTelescopeChanged(int)));
    connect(comboTel, SIGNAL(currentIndexChanged(int)), this, SLOT(slotTelescopeChanged(int)));

    newInsButton = new QPushButton(tr("new"));
    connect(newInsButton, SIGNAL(clicked()), this, SLOT(slotNewInsButtonClicked()));

    remInsButton = new QPushButton(tr("rem"));
    connect(remInsButton, SIGNAL(clicked()), this, SLOT(slotRemInsButtonClicked()));

    saveSettButton = new QPushButton(tr("save"));
    connect(saveSettButton, SIGNAL(clicked()), this, SLOT(slotSaveButtonClicked()));
    saveSettButton->setEnabled(0);

    nName = new newNameDlg(this);
    nName->formWidget->hide();
    connect(nName, SIGNAL(signalExit()), this, SLOT(slotNewInsExit()));

    //
    /*	 flSpinBox = new QDoubleSpinBox;
         flSpinBox -> setDecimals(3);
         flSpinBox -> setPrefix (tr("focal length = "));
         flSpinBox -> setSuffix (tr(" mm"));
         flSpinBox -> setRange(0.0,40000.0);
         flSpinBox -> setSingleStep (10);
         flSpinBox -> setValue(3467.000);*/
    //
    xpsSpinBox = new QDoubleSpinBox;
    xpsSpinBox -> setDecimals(2);
    xpsSpinBox -> setPrefix (tr("pixel x-size = "));
    xpsSpinBox -> setSuffix (tr(" mas/pix"));
    xpsSpinBox -> setRange(0.0,100000.0);
    xpsSpinBox -> setSingleStep (10);
    xpsSpinBox -> setValue(0.0);
    connect(xpsSpinBox, SIGNAL(valueChanged(double)), this, SLOT(slotScxChanged(double)));
    //
    ypsSpinBox = new QDoubleSpinBox;
    ypsSpinBox -> setDecimals(2);
    ypsSpinBox -> setPrefix (tr("pixel y-size = "));
    ypsSpinBox -> setSuffix (tr(" mas/pix"));
    ypsSpinBox -> setRange(0.0,100000.0);
    ypsSpinBox -> setSingleStep (10);
    ypsSpinBox -> setValue(0.0);
    connect(ypsSpinBox, SIGNAL(valueChanged(double)), this, SLOT(slotScyChanged(double)));
    //
    fovSpinBox = new QDoubleSpinBox;
    fovSpinBox -> setDecimals(1);
    fovSpinBox -> setPrefix (tr("fov = "));
    fovSpinBox -> setSuffix (tr(" %"));
    fovSpinBox -> setRange(0.0,500.0);
    fovSpinBox ->setSingleStep(10);
    fovSpinBox -> setValue(100.0);
    connect(fovSpinBox, SIGNAL(valueChanged(double)), this, SLOT(slotFovChanged(double)));
    //
    rotAngSpinBox = new QDoubleSpinBox;
    rotAngSpinBox -> setDecimals(2);
    rotAngSpinBox -> setPrefix (tr("pos angle = "));
    rotAngSpinBox -> setSuffix (tr(" deg"));
    rotAngSpinBox -> setRange(-360,360);
    rotAngSpinBox -> setSingleStep (1);
    rotAngSpinBox -> setValue(0);
    connect(rotAngSpinBox, SIGNAL(valueChanged(double)), this, SLOT(slotAngleChanged(double)));
    //
    apeSpinBox = new QDoubleSpinBox;
    apeSpinBox -> setDecimals(1);
    apeSpinBox -> setPrefix (tr("aperture = "));
    apeSpinBox -> setSuffix (tr(" pix"));
    apeSpinBox -> setRange(1.0,10000.0);
    apeSpinBox ->setSingleStep(1);
    apeSpinBox -> setValue(imgLabel->aperture);
    connect(apeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(slotApeChanged(double)));
    //
    currDirLabel = new QLabel(tr("current folder = "));
    currDirButton = new QPushButton(tr("..."));
    connect(currDirButton, SIGNAL(clicked()), this, SLOT(slotCurrDirButtonClicked()));
    updateHistCheckBox=new QCheckBox(tr("update histogram"));
    /*	 ucacDirLabel = new QLabel(tr("ucac2 folder = "));
         ucacDirButton = new QPushButton(tr("..."));
         connect(ucacDirButton, SIGNAL(clicked()), this, SLOT(slotUcacDirButtonClicked()));
         usnobDirLabel = new QLabel(tr("usnob folder = "));
         usnobDirButton = new QPushButton(tr("..."));
         connect(usnobDirButton, SIGNAL(clicked()), this, SLOT(slotUSNOBDirButtonClicked()));*/
    logDirLabel = new QLabel(tr("log folder = "));
    logDirButton = new QPushButton(tr("..."));
    connect(logDirButton, SIGNAL(clicked()), this, SLOT(slotLogDirButtonClicked()));
    /*
    rbCatUCAC2 = new QRadioButton(tr("ucac2"));
    rbCatUSNOB = new QRadioButton(tr("usnob"));
    rbCatUCAC2->setChecked(true);
    */
    catSelect = new QComboBox(propWidget);

    connect(catSelect, SIGNAL(highlighted(int)), this, SLOT(slotCatalogChanged(int)));
    connect(catSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(slotCatalogChanged(int)));


         proxyPort = new QSpinBox;
         proxyPort -> setRange(0,10000);
         proxyPort -> setValue(3128);
         proxyPort -> setPrefix(tr("proxy port = "));
         proxyPort -> setFixedWidth(200);
         proxyHost = new QLineEdit;
         proxyHost -> setFixedWidth(200);
         proxyHost -> setText("proxy.gao.spb.ru");
         //QLabel *wcsCatLabel = new QLabel("wcs catalog = ");
         /*wcsLineEdit = new QLineEdit;
         wcsLineEdit -> setFixedWidth(100);
         wcsLineEdit -> setText("ucac2");*/
         useProxy=new QCheckBox(tr("use proxy"));
         useProxy->setCheckState(Qt::Checked);
         obsCode= new QLineEdit;
         obsCode -> setFixedWidth(200);
         obsCode -> setText("084");
    //
    QGridLayout *propGrid = new QGridLayout(propWidget);
    propGrid->addWidget(comboTel,0,0);
    propGrid->addWidget(newInsButton,0,1);
//	 propGrid -> addWidget(flSpinBox,1,0);
    propGrid -> addWidget(xpsSpinBox,1,0);
    propGrid -> addWidget(saveSettButton,1,1);
    propGrid -> addWidget(ypsSpinBox,2,0);
    propGrid->addWidget(remInsButton,2,1);
    propGrid -> addWidget(fovSpinBox,3,0);
    propGrid -> addWidget(rotAngSpinBox,4,0);
    propGrid -> addWidget(apeSpinBox,5,0);
    propGrid -> addWidget(currDirLabel,6,0);
    propGrid -> addWidget(currDirButton,6,1);
    /*	 propGrid -> addWidget(ucacDirLabel,7,0);
         propGrid -> addWidget(ucacDirButton,7,1);
         propGrid -> addWidget(usnobDirLabel,8,0);
         propGrid -> addWidget(usnobDirButton,8,1);*/
    propGrid -> addWidget(logDirLabel,7,0);
    propGrid -> addWidget(logDirButton,7,1);
    //propGrid -> addWidget(rbCatUCAC2,8,0);
    //propGrid -> addWidget(rbCatUSNOB,8,1);
    propGrid->addWidget(catSelect,8,0);
    propGrid -> addWidget(updateHistCheckBox,9,0);
         propGrid -> addWidget(proxyHost,10,0);
         propGrid -> addWidget(proxyPort,11,0);
         propGrid -> addWidget(useProxy,12,0);
         propGrid -> addWidget(obsCode,13,0);
         //propGrid -> addWidget(wcsCatLabel,14,0);
         //propGrid -> addWidget(wcsLineEdit,14,1);
    propGrid->setRowStretch(14, 1);
    propGrid->setColumnStretch(2, 1);
    //propGrid -> addWidget(radeLineEdit,0,1);
    //+++
    dockPropWidget = new QDockWidget(tr("properties"),this,Qt::Widget);
    dockPropWidget->setWidget(propWidget);
    dockPropWidget->setFloating(true);
    addDockWidget(Qt::TopDockWidgetArea,dockPropWidget);

    dockPropWidget->hide();
    dockPropWidget->setObjectName("propertyWidget");

//  Marks widget

    QWidget *markWidget = new QWidget;

//set position
    QGroupBox *posmBox = new QGroupBox(tr("set position"),markWidget);
    QGridLayout *posmLayout = new QGridLayout(posmBox);

    QLabel *radeLabel = new QLabel("RA and DEC:");
    radeLineEdit = new QLineEdit;
    radeLineEdit->setInputMask("99 99 99.999 #99 99 99.99 99.9 99.9");
    radeLineEdit->setText ("00 00 00.000 +00 00 00.00 06.0 16.0");
    radeLineEdit->setFixedWidth(230);
    connect(radeLineEdit, SIGNAL( textChanged(QString)), this, SLOT(slotRaDeChanged(QString)));

    QLabel *timeLabel = new QLabel("Time:");
    timeLineEdit = new QLineEdit;
    connect(timeLineEdit, SIGNAL( textChanged(QString)), this, SLOT(slotTimeChanged(QString)));

    QLabel *expLabel = new QLabel("Exposure:");
    expLineEdit = new QLineEdit;

    getRadeButton = new QPushButton(tr("<-h"));
    connect(getRadeButton, SIGNAL(clicked()), this, SLOT(slotFromHeader()));
    getMjdButton = new QPushButton(tr("<-h"));
    connect(getMjdButton, SIGNAL(clicked()), this, SLOT(slotGetMJDfromHeader()));

    mfcButton = new QPushButton(tr("marks from catalog"));
    connect(mfcButton, SIGNAL(clicked()), this, SLOT(slotMarks()));
    clearCatMarksButton = new QPushButton(tr("clear cat marks"));
    connect(clearCatMarksButton, SIGNAL(clicked()), this, SLOT(clearCatMarks()));
    measureCatMarksButton = new QPushButton(tr("measure cat marks"));
    connect(measureCatMarksButton, SIGNAL(clicked()), this, SLOT(slotMeasureCatMarkedButtonClicked()));

    posmLayout->addWidget(radeLabel, 0, 0);
    posmLayout->addWidget(radeLineEdit, 0, 1);
    posmLayout->addWidget(getRadeButton, 0, 2);
    posmLayout->addWidget(timeLabel, 1, 0);
    posmLayout->addWidget(timeLineEdit, 1, 1);
    posmLayout->addWidget(getMjdButton, 1, 2);
    posmLayout->addWidget(expLabel, 2, 0);
    posmLayout->addWidget(expLineEdit, 2, 1);
    posmLayout->addWidget(mfcButton, 3, 0);
    posmLayout->addWidget(measureCatMarksButton, 3, 1);
    posmLayout->addWidget(clearCatMarksButton, 3, 2);
    posmLayout->setRowStretch(4, 2);
    posmLayout->setColumnStretch(3, 2);

//////Marks
//	QWidget *mcontrolWidget = new QWidget;
    QGroupBox *mcontrolBox = new QGroupBox(tr("marks"),markWidget);
    QGridLayout *mcontrolLayout = new QGridLayout(mcontrolBox);

    QLabel *activGridLabel = new QLabel(tr("Activ grid:"));
    activMarksGrid = new QComboBox;

    //QPixmap *ipixPm, *catPm, *refPm, *objPm;
    //QIcon ipixIcon, catIcon, refIcon, objIcon;
    ipixPm = new QPixmap(10, 10);
    catPm = new QPixmap(10, 10);
    refPm = new QPixmap(10, 10);
    objPm = new QPixmap(10, 10);



    ipixPm->fill(ipixCol);
    catPm->fill(catCol);
    refPm->fill(refCol);
    objPm->fill(objCol);



    ipixPmB = new QPixmap(*ipixPm);

    ipixIcon.addPixmap(*ipixPm);

    //ipixBtnIcon = ipixIcon;
    //ipixBtnIcon.addPixmap(*ipixPmB);

    catIcon.addPixmap(*catPm);
    refIcon.addPixmap(*refPm);
    objIcon.addPixmap(*objPm);
    /*
    activMarksGrid->insertItem(0, QIcon("./icons/white.bmp"), tr("All"), 0);
    activMarksGrid->insertItem(1, QIcon("./icons/yellow.bmp"), tr("IPix"), 0);
    activMarksGrid->insertItem(2, QIcon("./icons/blue.bmp"), tr("Cat"), 0);
    activMarksGrid->insertItem(3, QIcon("./icons/red.bmp"), tr("Ref"), 0);
    activMarksGrid->insertItem(4, QIcon("./icons/green.bmp"), tr("Obj"), 0);
*/
    activMarksGrid->insertItem(0, QIcon("./icons/white.bmp"), tr("All"), 0);
    activMarksGrid->insertItem(1, ipixIcon, tr("IPix"), 0);
    activMarksGrid->insertItem(2, catIcon, tr("Cat"), 0);
    activMarksGrid->insertItem(3, refIcon, tr("Ref"), 0);
    activMarksGrid->insertItem(4, objIcon, tr("Obj"), 0);

    clearButton = new QPushButton(tr("clear marks"));
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clearMarks()));
    cMassButton = new QPushButton(tr("Center of Mass"));
    connect(cMassButton, SIGNAL(clicked()), this, SLOT(slotCenterMassButton()));
    measureMarksButton = new QPushButton(tr("measure marks"));
    connect(measureMarksButton, SIGNAL(clicked()), this, SLOT(slotMeasureMarkedButtonClicked()));

//////IPix
    QGroupBox *ipixBox = new QGroupBox(tr("IPix"),markWidget);
    QGridLayout *ipixLayout = new QGridLayout(ipixBox);

    saveMarksButton = new QPushButton(tr("Save Marks"));
    connect(saveMarksButton, SIGNAL(clicked()), this, SLOT(slotSaveMarksButtonClicked()));
    loadMarksButton = new QPushButton(tr("Load Marks"));
    connect(loadMarksButton, SIGNAL(clicked()), this, SLOT(slotLoadMarksButtonClicked()));

    flipMarksHButton = new QPushButton(tr("flipH"));
    connect(flipMarksHButton, SIGNAL(clicked()), this, SLOT(slotFlipMarksH()));
    flipMarksVButton = new QPushButton(tr("flipV"));
    connect(flipMarksVButton, SIGNAL(clicked()), this, SLOT(slotFlipMarksV()));

    ipixLayout->addWidget(saveMarksButton, 0, 0);
    ipixLayout->addWidget(loadMarksButton, 0, 1, 1, 2);
    ipixLayout->addWidget(flipMarksHButton, 1, 0);
    ipixLayout->addWidget(flipMarksVButton, 1, 1, 1, 2);

//////////
    QGroupBox *tasksBox = new QGroupBox(tr("Tasks"),markWidget);
    QGridLayout *tasksLayout = new QGridLayout(tasksBox);

    identifyButton = new QPushButton(tr("Identify"));
    connect(identifyButton, SIGNAL(clicked()), this, SLOT(slotIdentify()));
    identifyAutoButton = new QPushButton(tr("Identify Auto"));
    connect(identifyAutoButton, SIGNAL(clicked()), this, SLOT(slotIdentifyAuto()));

    findCloserStarsButton = new QPushButton(tr("Find Stars"));
    connect(findCloserStarsButton, SIGNAL(clicked()), this, SLOT(slotFindCloserStars()));
    findCloserObjectsButton = new QPushButton(tr("Get Objects"));
    connect(findCloserObjectsButton, SIGNAL(clicked()), this, SLOT(slotGetObjects()));
    makeReductionButton = new QPushButton(tr("Make Reduction"));
    connect(makeReductionButton, SIGNAL(clicked()), this, SLOT(slotMakeReduction()));

    tasksLayout->addWidget(identifyButton, 0, 0);
    tasksLayout->addWidget(identifyAutoButton, 0, 1, 1, 2);
    tasksLayout->addWidget(findCloserStarsButton, 1, 0);
    tasksLayout->addWidget(findCloserObjectsButton, 1, 1);
    tasksLayout->addWidget(makeReductionButton, 1, 2);
//////////

    mcontrolLayout->addWidget(activGridLabel, 0, 0);
    mcontrolLayout->addWidget(activMarksGrid, 0, 1, 1, 2);

    mcontrolLayout->addWidget(clearButton, 1, 0, 1, 3);
    mcontrolLayout->addWidget(cMassButton, 2, 0);
    mcontrolLayout->addWidget(measureMarksButton, 2, 1, 1, 2);

    mcontrolLayout->addWidget(ipixBox, 3, 0, 1, 3);
    mcontrolLayout->addWidget(tasksBox, 4, 0, 1, 3);

    mcontrolLayout->setColumnStretch(2, 2);
  //  mcontrolLayout->setRowStretch(5, 2);


/////////////////////////////
//	moveMarks	/////////////
    moveCheckBox=new QCheckBox(tr("move marks"));
    if (mainFrame==1) moveCheckBox->setChecked(Qt::Checked);
    else moveCheckBox->setChecked(Qt::Unchecked);
    connect(moveCheckBox, SIGNAL(stateChanged ( int )), this, SLOT(slotMoveCheckBoxStateChanged(int)));
    setOcCheckBox=new QCheckBox(tr("set optical center"));
    saveMarksPos = new QCheckBox(tr("keep marks position"));
    saveMarksPos->setChecked(Qt::Checked);

    rbMoveAll = new QRadioButton(tr("move all"));
    rbMoveSel = new QRadioButton(tr("move selected"));
    rbMoveAll -> setChecked(true);

    QGroupBox *mvBox = new QGroupBox(tr("moving type"),markWidget);
    QGridLayout *mvLayout = new QGridLayout(mvBox);
/*    mvLayout->addWidget(moveCheckBox, 0, 0);
    mvLayout->addWidget(setOcCheckBox, 0, 1);
    mvLayout->setColumnStretch(1, 2);
    mvLayout->addWidget(rbMoveAll, 1, 0);
    mvLayout->addWidget(rbMoveSel, 1, 1);*/
    mvLayout->addWidget(saveMarksPos, 0, 0);
    mvLayout->setRowStretch(1, 2);
///////////////
    QVBoxLayout *markGrid = new QVBoxLayout(markWidget);
    markGrid->setSpacing(5);
    markGrid->addWidget(posmBox, 1);
    markGrid->addWidget(mcontrolBox, 2);
    markGrid->addWidget(mvBox, 3);
    markGrid->addStretch(4);

    markGrid->setMargin(5);
    dockMarkWidget = new QDockWidget(tr("marking panel"),this,Qt::Widget);
    dockMarkWidget->setWidget(markWidget);
    dockMarkWidget->setFloating(true);
    addDockWidget(Qt::BottomDockWidgetArea,dockMarkWidget);
    dockMarkWidget->hide();
    dockMarkWidget->setObjectName("marksWidget");
    ///////////////////////////////////////////
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
    dockKeyWidget->setObjectName("keyWidget");
//Console for logs
/*    conWidget = new QWidget;
    conText = new QTextEdit(conWidget);
    conText->setReadOnly(true);
    conText->setTextColor(Qt::white);
    conText->setTextBackgroundColor(Qt::black);
    conText->resize(1280, 1000);
    conScrollArea = new QScrollArea(conWidget);
    conScrollArea->setBackgroundRole(QPalette::Shadow);
    conScrollArea->setWidget(conText);
    dockConWidget = new QDockWidget(tr("logs panel"),this,Qt::Widget);
    dockConWidget->setWidget(conWidget);
    dockConWidget->setMinimumSize(625,175);
    dockConWidget->setFloating(true);
    addDockWidget(Qt::BottomDockWidgetArea,dockConWidget);
    dockConWidget->hide();
    dockConWidget->setObjectName("consoleWidget");*/

//	out = new QTextStream(conText);
    /////////////////////////////////////////////
    //View Panel
    ////////////////////////////////////
    magnifyWidget = new QWidget;

//////////////	File Manager Widget
    QWidget *fmWidget = new QWidget;
    fmWidget->resize(500, 200);
    fmWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    frameList = new QTableWidget(0, 2, this);
    frameList->setColumnCount(3);

    QStringList frameListHeader;
//	frameListHeader << "*" << tr("Names");
    frameItem = new QTableWidgetItem(tr("Name"));
    frameItem->setTextAlignment(Qt::AlignLeft);
    frameList->setHorizontalHeaderItem(0, frameItem);

    frameItem = new QTableWidgetItem(tr("Path"));
    frameItem->setTextAlignment(Qt::AlignLeft);
    frameList->setHorizontalHeaderItem(1, frameItem);

    frameItem = new QTableWidgetItem(tr("WCS"));
    frameItem->setTextAlignment(Qt::AlignCenter);
    frameList->setHorizontalHeaderItem(2, frameItem);

    frameList->setColumnWidth(0, 100);
    frameList->setColumnWidth(1, 150);
    frameList->setColumnWidth(2, 35);
    frameList->setCornerButtonEnabled(false);

    firstButton = new QPushButton("");
    firstButton->setIcon(QIcon("./icons/first_2.bmp"));

    prevButton = new QPushButton("");
    prevButton->setIcon(QIcon(".\\icons\\prev_2.bmp"));

    nextButton = new QPushButton("");
    nextButton->setIcon(QIcon(".\\icons\\next_2.bmp"));

    lastButton = new QPushButton("");
    lastButton->setIcon(QIcon(".\\icons\\last_2.bmp"));

    openButton = new QPushButton("");
    openButton->setIcon(QIcon(".\\icons\\plus.bmp"));

    closeButton = new QPushButton("");
    closeButton->setIcon(QIcon(".\\icons\\cross.bmp"));

    checkAllButton = new QPushButton(tr("Check All"));
    unCheckAllButton = new QPushButton(tr("UnCheck All"));

    QGridLayout *fmGrid = new QGridLayout(fmWidget);
    fmGrid->addWidget(frameList, 0, 0, 6, 1);
    fmGrid->addWidget(firstButton,0,1);
    fmGrid->addWidget(prevButton,1,1);
    fmGrid->addWidget(nextButton,2,1);
    fmGrid->addWidget(lastButton,3,1);
    fmGrid->addWidget(openButton,4,1);
    fmGrid->addWidget(closeButton,5,1);
    fmGrid->addWidget(checkAllButton,6,0);
    fmGrid->addWidget(unCheckAllButton,6,1);
    fmGrid->setRowStretch(7, 2);

    connect(frameList, SIGNAL(cellClicked(int, int)), this, SLOT(slotFrameListClicked(int , int )));
//Navigate buttons
    connect(firstButton, SIGNAL(clicked()), this, SLOT(slotFirst()));
    connect(prevButton, SIGNAL(clicked()), this, SLOT(slotPreviouse()));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(slotNext()));
    connect(lastButton, SIGNAL(clicked()), this, SLOT(slotLast()));
    connect(openButton, SIGNAL(clicked()), this, SLOT(slotOpenFitsFile()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(slotDelete()));

    connect(checkAllButton, SIGNAL(clicked()), this, SLOT(slotCheckAllFiles()));
    connect(unCheckAllButton, SIGNAL(clicked()), this, SLOT(slotUnCheckAllFiles()));


    dockFMgrWidget = new QDockWidget(tr("Frame Manager"),this,Qt::Widget);
    dockFMgrWidget->setWidget(fmWidget);
    dockFMgrWidget->setFloating(true);
    addDockWidget(Qt::TopDockWidgetArea,dockFMgrWidget);

    dockFMgrWidget->hide();
    dockFMgrWidget->setObjectName("fileManagerWidget");
///////////////////////////////////////

    QWidget *psfWidget = new QWidget;
    lorentzRadioButton = new QRadioButton(tr("lorentzian"));
    gaussianRadioButton = new QRadioButton(tr("gaussian"));
    moffatRadioButton = new QRadioButton(tr("moffat"));
    cMassRadioButton = new QRadioButton(tr("Center of Mass"));
    cMassRadioButton->setChecked(true);
    orderSpinBox = new QDoubleSpinBox;
    orderSpinBox -> setDecimals(1);
    orderSpinBox -> setPrefix (tr("degree of the model = "));
    orderSpinBox -> setRange(1.0,4.0);
    orderSpinBox -> setSingleStep (0.1);
    orderSpinBox -> setValue(1.0);
    //
    QWidget *gbWidget = new QWidget;
    QGroupBox *mBox = new QGroupBox(tr("psf model"),gbWidget);
    QBoxLayout *mLayout = new QBoxLayout(QBoxLayout::LeftToRight,mBox);
    mLayout->addWidget(lorentzRadioButton);
    mLayout->addWidget(gaussianRadioButton);
    mLayout->addWidget(moffatRadioButton);
    mLayout->addWidget(cMassRadioButton);
    gbWidget->setFixedSize (3*mBox->width(), 2*mBox->height());
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
    measureMarkedButton->setEnabled(true);
    connect(measureMarkedButton, SIGNAL(clicked()), this, SLOT(slotMeasureIPixMarkedButtonClicked()));
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
    psfGrid->setRowStretch(6,3);
    //
    dockPsfWidget = new QDockWidget(tr("psf panel"),this,Qt::Widget);
    dockPsfWidget->setWidget(psfWidget);
    dockPsfWidget->setFloating(true);
    addDockWidget(Qt::RightDockWidgetArea,dockPsfWidget);

    dockPsfWidget->hide();
    dockPsfWidget->setObjectName("psfWidget");
///////////////////////////////////////////

    //	Filters widget

    QWidget *filtersWidget = new QWidget;

    clearIpixButtonF = new QPushButton(tr("clear Ipix"));
    connect(clearIpixButtonF, SIGNAL(clicked()), this, SLOT(clearIpixMarks()));

    levelBinLabel = new QLabel;
    levelBinLabel->setText("0");

    levelBinSlider = new QSlider(Qt::Horizontal);
    levelBinSlider->setTickPosition(QSlider::TicksAbove);
    levelBinSlider->setRange(0, 1);
    levelBinSlider->setValue(0);
    levelBinSlider->setPageStep(20);
    connect(levelBinSlider, SIGNAL(valueChanged(int)), this, SLOT(slotLevelBinChanged(int)));

    aperBinLabel = new QLabel;
    aperBinLabel->setText("2");

    aperBinSlider = new QSlider(Qt::Horizontal);
    aperBinSlider->setTickPosition(QSlider::TicksAbove);
    aperBinSlider->setRange(1, 5000);
    aperBinSlider->setValue(2);
    aperBinSlider->setPageStep(5);
    connect(aperBinSlider, SIGNAL(valueChanged(int)), this, SLOT(slotAperBinChanged(int)));

    markBinFilterButton = new QPushButton(tr("marks bin filter"));
    connect(markBinFilterButton, SIGNAL(clicked()), this, SLOT(slotBinFilter()));

    QGroupBox *binFilterBox = new QGroupBox(tr("bin filter"),filtersWidget);
    QVBoxLayout *binFilterLayout = new QVBoxLayout(binFilterBox);
    binFilterLayout->addWidget(levelBinLabel);
    binFilterLayout->addWidget(levelBinSlider);
    binFilterLayout->addWidget(aperBinLabel);
    binFilterLayout->addWidget(aperBinSlider);
    binFilterLayout->addWidget(markBinFilterButton);



    QGroupBox *findStarsBox = new QGroupBox(tr("bin filter"),filtersWidget);
    QGridLayout *findStarsLayout = new QGridLayout(findStarsBox);

    starTargetSpinBox = new QSpinBox();
    starTargetSpinBox->setRange(1,100);
    starTargetSpinBox->setValue(10);
    findStarsButton = new QPushButton(tr("find stars"));
    connect(findStarsButton, SIGNAL(clicked()), this, SLOT(slotFindStars()));

    findStarsLayout->addWidget(starTargetSpinBox, 0, 0);
    findStarsLayout->addWidget(findStarsButton, 0, 1);

    QVBoxLayout *filtersLayout = new QVBoxLayout(filtersWidget);
    filtersLayout->addWidget(clearIpixButtonF);
    filtersLayout->addWidget(binFilterBox);
    filtersLayout->addWidget(findStarsBox);
    filtersLayout->addStretch(2);

    dockFiltersWidget = new QDockWidget(tr("Filters panel"),this,Qt::Widget);
    dockFiltersWidget->setWidget(filtersWidget);
    markGrid->addStretch(4);

    dockFiltersWidget->setMinimumSize(300,150);
    dockFiltersWidget->setFloating(true);
    addDockWidget(Qt::BottomDockWidgetArea,dockFiltersWidget);
    dockFiltersWidget->hide();
    dockFiltersWidget->setObjectName("filterWidget");

/////////////////////////// FITS header widget
    QWidget *fHeaderWidget = new QWidget;

    telLabel = new QLabel;
    telLabel->setText("TELESCOP");
    telLE = new QLineEdit;

    plateNLabel = new QLabel;
    plateNLabel->setText("PLATE-N");
    plateNLE = new QLineEdit;

    objLabel = new QLabel;
    objLabel->setText("OBJECT");
    objLE = new QLineEdit;

    numOfExpLabel = new QLabel;
    numOfExpLabel->setText("N-EXP");
    numOfExpLE = new QLineEdit;

    expNumLabel = new QLabel;
    expNumLabel->setText("Exposure num");
    expNumSel = new QComboBox;
    connect(expNumSel, SIGNAL(currentIndexChanged (int)), this, SLOT(slotExpNumChanged(int)));

    dateObsLabel = new QLabel;
    dateObsLabel->setText("DATE-OBS");
    dateObsLE = new QLineEdit;

    uTimeLabel = new QLabel;
    uTimeLabel->setText("U");
    uTimeLE = new QLineEdit;

    raLabel = new QLabel;
    raLabel->setText("RA");
    raLE = new QLineEdit;

    deLabel = new QLabel;
    deLabel->setText("DEC");
    deLE = new QLineEdit;

    expLabel = new QLabel;
    expLabel->setText("EXPOSURE");
    expLE = new QLineEdit;
//	expLE->setFrameStyle(frameStyle);

    obsLabel = new QLabel;
    obsLabel->setText("OBSERVER");
    obsLE = new QLineEdit;


    QGridLayout *fitsHeaderGrid = new QGridLayout(fHeaderWidget);
    fitsHeaderGrid->setColumnStretch(1, 1);
    fitsHeaderGrid->setColumnMinimumWidth(1, 250);
    /*
        fitsHeaderGrid->addWidget(headerLabel, 0, 0);
        fitsHeaderGrid->addWidget(headerFNLabel, 0, 1);
        fitsHeaderGrid->addWidget(headerButton, 0, 2);

        fitsHeaderGrid->addWidget(tiffLabel, 1, 0);
        fitsHeaderGrid->addWidget(tiffFNLabel, 1, 1);
        fitsHeaderGrid->addWidget(tiffButton, 1, 2);
    */
    fitsHeaderGrid->addWidget(telLabel, 0, 0);
    fitsHeaderGrid->addWidget(telLE, 0, 1);

    fitsHeaderGrid->addWidget(plateNLabel, 1, 0);
    fitsHeaderGrid->addWidget(plateNLE, 1, 1);

    fitsHeaderGrid->addWidget(objLabel, 2, 0);
    fitsHeaderGrid->addWidget(objLE, 2, 1);

    fitsHeaderGrid->addWidget(numOfExpLabel, 3, 0);
    fitsHeaderGrid->addWidget(numOfExpLE, 3, 1);

    fitsHeaderGrid->addWidget(expNumLabel, 4, 0);
    fitsHeaderGrid->addWidget(expNumSel, 4, 1);

    fitsHeaderGrid->addWidget(dateObsLabel, 5, 0);
    fitsHeaderGrid->addWidget(dateObsLE, 5, 1);

    fitsHeaderGrid->addWidget(uTimeLabel, 6, 0);
    fitsHeaderGrid->addWidget(uTimeLE, 6, 1);

    fitsHeaderGrid->addWidget(expLabel, 7, 0);
    fitsHeaderGrid->addWidget(expLE, 7, 1);

    fitsHeaderGrid->addWidget(raLabel, 8, 0);
    fitsHeaderGrid->addWidget(raLE, 8, 1);

    fitsHeaderGrid->addWidget(deLabel, 9, 0);
    fitsHeaderGrid->addWidget(deLE, 9, 1);



    fitsHeaderGrid->addWidget(obsLabel, 10, 0);
    fitsHeaderGrid->addWidget(obsLE, 10, 1);

    fitsHeaderGrid->setColumnStretch(1, 2);
    fitsHeaderGrid->setRowStretch(11,2);
    /*
        fitsHeaderGrid->addWidget(flipH, 11, 0);
        fitsHeaderGrid->addWidget(flipV, 11, 1);
        fitsHeaderGrid->addWidget(IsInv, 11, 2);
    */
    dockFitsHeaderWidget = new QDockWidget(tr("Fits Header panel"),this,Qt::Widget);

    qDebug() << "\ndockFitsHeaderWidget " << dockFitsHeaderWidget << "\n";
    dockFitsHeaderWidget->setWidget(fHeaderWidget);
    dockFitsHeaderWidget->setMinimumSize(300,300);
    dockFitsHeaderWidget->setFloating(true);
    addDockWidget(Qt::BottomDockWidgetArea,dockFitsHeaderWidget);
    dockFitsHeaderWidget->hide();
    dockFitsHeaderWidget->setObjectName("fitsHeaderWidget");


///////////////////////////////////////

        histLabel = new QLabel;
     histLabel->setBackgroundRole(QPalette::Dark);
     histLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
     histLabel->setFixedSize(650,650);
     histLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
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
    dockHist->setObjectName("histWidget");
///////////////////////////////////////

         //	WCS widget

    QWidget *wcsWidget = new QWidget;

    QLabel *wcsLabelT1 = new QLabel("CTYPE1");
    QLabel *wcsLabelT2 = new QLabel("CTYPE2");
    ct1ValLabel = new QLabel();
    ct2ValLabel = new QLabel();

    QLabel *wcsLabel0 = new QLabel("CRPIX1");
    QLabel *wcsLabel1 = new QLabel("CRPIX2");
    QLabel *wcsLabel2 = new QLabel("CRVAL1");
    QLabel *wcsLabel3 = new QLabel("CRVAL2");
    QLabel *wcsLabel4 = new QLabel("CDELT1");
    QLabel *wcsLabel5 = new QLabel("CDELT2");
    QLabel *wcsLabel6 = new QLabel("CROTA1");
    QLabel *wcsLabel7 = new QLabel("CROTA2");
    QLabel *wcsLabel8 = new QLabel("CD1_1");
    QLabel *wcsLabel9 = new QLabel("CD1_2");
    QLabel *wcsLabel10 = new QLabel("CD2_1");
    QLabel *wcsLabel11 = new QLabel("CD2_2");
    QLabel *curL;
    for(int i=0; i<12; i++)
    {
        curL = new QLabel;
        curL->setAlignment(Qt::AlignLeft);
        wcsList << curL;
    }
/*
    QLabel *sxLabel = new QLabel("Sx");
    sxValLabel = new QLabel();
    QLabel *syLabel = new QLabel("Sy");
    syValLabel = new QLabel();
*/


    QGroupBox *wcsLabelsBox = new QGroupBox(tr("WCS"),wcsWidget);
    QGridLayout *wcsLabelsLayout = new QGridLayout(wcsLabelsBox);
  /*  wcsLabelsLayout->addWidget(wcsLabelT1, 0, 0);
    wcsLabelsLayout->addWidget(ct1ValLabel, 0, 1);
    wcsLabelsLayout->addWidget(wcsLabelT2, 1, 0);
    wcsLabelsLayout->addWidget(ct2ValLabel, 1, 1);*/

    wcsLabelsLayout->addWidget(wcsLabel0, 0, 0);
    wcsLabelsLayout->addWidget(wcsLabel1, 1, 0);
    wcsLabelsLayout->addWidget(wcsLabel2, 2, 0);
    wcsLabelsLayout->addWidget(wcsLabel3, 3, 0);
    wcsLabelsLayout->addWidget(wcsLabel4, 4, 0);
    wcsLabelsLayout->addWidget(wcsLabel5, 5, 0);
    wcsLabelsLayout->addWidget(wcsLabel6, 6, 0);
    wcsLabelsLayout->addWidget(wcsLabel7, 7, 0);
    wcsLabelsLayout->addWidget(wcsLabel8, 8, 0);
    wcsLabelsLayout->addWidget(wcsLabel9, 9, 0);
    wcsLabelsLayout->addWidget(wcsLabel10, 10, 0);
    wcsLabelsLayout->addWidget(wcsLabel11, 11, 0);
    for(int i=0; i<12; i++) wcsLabelsLayout->addWidget(wcsList[i], i, 1);
    /*wcsLabelsLayout->addWidget(sxLabel, 12, 0);
    wcsLabelsLayout->addWidget(sxValLabel, 12, 1);
    wcsLabelsLayout->addWidget(syLabel, 13, 0);
    wcsLabelsLayout->addWidget(syValLabel, 13, 1);*/
    wcsLabelsLayout->setRowStretch(12, 2);

    wcsSpinBox = new QSpinBox;
    wcsSpinBox->setRange(4,20);
    wcsSpinBox->setPrefix(tr("wcsN = "));
    wcsSpinBox->setValue(6);

    dropWcsButton = new QPushButton(tr("Drop WCS"));
    connect(dropWcsButton, SIGNAL(clicked()), this, SLOT(slotDropWCS()));
    writeWcsButton = new QPushButton(tr("Write WCS"));
    connect(writeWcsButton, SIGNAL(clicked()), this, SLOT(slotWriteWCS()));

    wcsButton = new QPushButton(tr("wcs"));
    connect(wcsButton, SIGNAL(clicked()), this, SLOT(slotWCSButtonClicked()));

    wcsLoadButton = new QPushButton(tr("Load WCS"));
    connect(wcsLoadButton, SIGNAL(clicked()), this, SLOT(slotLoadWCS()));
    wcsSaveButton = new QPushButton(tr("SaveWCS"));
    connect(wcsSaveButton, SIGNAL(clicked()), this, SLOT(slotSaveWCS()));

    smaxSpinBox = new QDoubleSpinBox;
    smaxSpinBox -> setDecimals(1);
    smaxSpinBox -> setPrefix (tr("Smax = "));
 //   smaxSpinBox -> setSuffix (tr(" pix"));
    smaxSpinBox -> setRange(1.0,100000.0);
    smaxSpinBox ->setSingleStep(10);
    smaxSpinBox -> setValue(500);


    QGridLayout *wcsLayout = new QGridLayout(wcsWidget);
    wcsLayout->addWidget(wcsLabelsBox, 0, 0, 1, 3);
    wcsLayout->addWidget(wcsButton, 1, 0);
    wcsLayout->addWidget(wcsSpinBox, 1, 1);
    wcsLayout->addWidget(dropWcsButton, 2, 0);
    wcsLayout->addWidget(smaxSpinBox, 2, 1);
    wcsLayout->addWidget(writeWcsButton, 2, 2);
    wcsLayout->addWidget(wcsLoadButton, 3, 1);
    wcsLayout->addWidget(wcsSaveButton, 3, 2);
    wcsLayout->setRowStretch(4, 3);
    wcsLayout->setColumnStretch(3, 3);

    dockWcsWidget = new QDockWidget(tr("WCS panel"),this,Qt::Widget);
    dockWcsWidget->setWidget(wcsWidget);
    dockWcsWidget->setFloating(true);
    dockWcsWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    addDockWidget(Qt::TopDockWidgetArea,dockWcsWidget);
    dockWcsWidget->hide();
    dockWcsWidget->setObjectName("wcsWidget");

///////////////////////////////////////////////////////

    QWidget *gStatPanel = new QWidget;
/*
    QLabel *yellPict = new QLabel;
//    QLabel *yellLabel;
    QLabel *bluePict = new QLabel;
 //   QLabel *blueLabel;
    QLabel *redPict = new QLabel;
//    QLabel *redLabel;
    QLabel *greenPict = new QLabel;
//    QLabel *greenLabel;

    yellPict->setPixmap(QPixmap("./icons/yellow.bmp"));
    bluePict->setPixmap(QPixmap("./icons/blue.bmp"));
    redPict->setPixmap(QPixmap("./icons/red.bmp"));
    greenPict->setPixmap(QPixmap("./icons/green.bmp"));

    yellLabel = new QLabel("yellow");
    blueLabel = new QLabel("blue");
    redLabel = new QLabel("red");
    greenLabel = new QLabel("green");
*/
    ipixBtn = new QPushButton("ipix");
    catBtn = new QPushButton("cat");
    refBtn = new QPushButton("ref");
    objBtn = new QPushButton("obj");

    ipixPmB = new QPixmap(10, 10);
    catPmB = new QPixmap(10, 10);
    refPmB = new QPixmap(10, 10);
    objPmB = new QPixmap(10, 10);

    ipixPmB->fill(QColor(ipixColStr));
    catPmB->fill(catCol);
    refPmB->fill(refCol);
    objPmB->fill(objCol);

    ipixBtnIcon.addPixmap(*ipixPmB);
    catBtnIcon.addPixmap(*catPmB);
    refBtnIcon.addPixmap(*refPmB);
    objBtnIcon.addPixmap(*objPmB);

    ipixBtn->setIcon(QIcon(*ipixPmB));
    catBtn->setIcon(catBtnIcon);
    refBtn->setIcon(refBtnIcon);
    objBtn->setIcon(objBtnIcon);
    ipixBtn->setCheckable(1);
    catBtn->setCheckable(1);
    refBtn->setCheckable(1);
    objBtn->setCheckable(1);
    ipixBtn->setChecked(1);
    catBtn->setChecked(1);
    refBtn->setChecked(1);
    objBtn->setChecked(1);
    /*
    activMarksGrid->insertItem(1, ipixIcon, tr("IPix"), 0);
    activMarksGrid->insertItem(2, catIcon, tr("Cat"), 0);
    activMarksGrid->insertItem(3, refIcon, tr("Ref"), 0);
    activMarksGrid->insertItem(4, objIcon, tr("Obj"), 0);
    */
    QHBoxLayout *gStatLayout = new QHBoxLayout(gStatPanel);
/*
    gStatLayout->addWidget(yellPict);
    gStatLayout->addWidget(yellLabel);
    gStatLayout->addWidget(bluePict);
    gStatLayout->addWidget(blueLabel);
    gStatLayout->addWidget(redPict);
    gStatLayout->addWidget(redLabel);
    gStatLayout->addWidget(greenPict);
    gStatLayout->addWidget(greenLabel);
    */
    gStatLayout->addWidget(ipixBtn);
    gStatLayout->addWidget(catBtn);
    gStatLayout->addWidget(refBtn);
    gStatLayout->addWidget(objBtn);
    gStatLayout->setStretch(5, 2);

  /*  QWidget* pw = new QWidget(this);
    pw->setMaximumSize(0, 0);*/
 //   QSize tbSize(8, 8);
 //   setIconSize(tbSize);
    QToolBar *gridStatTB = new QToolBar("gridStatPanel", this);
    gridStatTB->setIconSize(QSize(8, 8));
    gridStatTB->setObjectName("gridStatPanel");
    gridStatTB->addWidget(gStatPanel);

    addToolBar(gridStatTB);

/*
    dockGridStatWidget = new QDockWidget(this,Qt::Widget);
    dockGridStatWidget->setWidget(gStatPanel);
    dockGridStatWidget->setFloating(true);
    dockGridStatWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    dockGridStatWidget->setFixedSize(150, 50);
//    dockGridStatWidget->setTitleBarWidget(pw);
    addDockWidget(Qt::TopDockWidgetArea,dockGridStatWidget);
    dockGridStatWidget->hide();
    dockGridStatWidget->setObjectName("gridStatWidget");
*/
///////////////////////////////////////////////////////
    /*     scaledLabel = new QLabel;
         scaledLabel->setBackgroundRole(QPalette::Base);
         scaledLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
         scaledLabel->setFixedSize(200,200);

         boxScLayout = new QBoxLayout(QBoxLayout::TopToBottom,magnifyWidget);
         boxScLayout->addWidget(imgLabel);
    //     boxScLayout->addWidget(apertWidget);
     //    boxScLayout->addWidget(gridWidget);
    //	 boxScLayout->addWidget(statusSW);
         //boxScLayout->addWidget(histLabel);
         //boxScLayout->addWidget(gammaSpinBox);fullrangeCheckBox

         dockScWidget = new QDockWidget(tr("view panel"),this,Qt::Widget);
         dockScWidget->setMinimumSize(250,450);
         dockScWidget->setWidget(magnifyWidget);
         addDockWidget(Qt::LeftDockWidgetArea,dockScWidget);*/

}

void MainWindow::setSettings()
{

    stimproSett = new QSettings("stimpro.ini", QSettings::IniFormat);
//general
   currDir = stimproSett->value("general/curr_dir", "./").toString();
   currDirLabel->setText("Current folder = "+currDir);
   logDir = stimproSett->value("general/log_path", "./").toString();
   logDirLabel->setText("log folder = "+logDir);



//gridstatw
   ipixColStr = stimproSett->value("gridstatw/ipixCol", "#FFFF00").toString();
   catColStr = stimproSett->value("gridstatw/catCol", "#0000FF").toString();
   refColStr = stimproSett->value("gridstatw/refCol", "#FF0000").toString();
   objColStr = stimproSett->value("gridstatw/objCol", "#008000").toString();
   bgColStr = stimproSett->value("gridstatw/bgCol", "#000000").toString();

   ipixCol.setNamedColor(ipixColStr);
   catCol.setNamedColor(catColStr);
   refCol.setNamedColor(refColStr);
   objCol.setNamedColor(objColStr);
   bgCol.setNamedColor(bgColStr);

   ipixBtnCol.setNamedColor(ipixColStr);
   /*catCol.setNamedColor(catColStr);
   refCol.setNamedColor(refColStr);
   objCol.setNamedColor(objColStr);*/


    settings = new QSettings("conf/conf.ini", QSettings::IniFormat);

    telescopeFile = settings->value("telescope/telescopeFile", "./conf/telescopes.ini").toString();
    instrNum = settings->value("telescope/instrNum", 0).toInt();
    //instruments = new insSettings(telescopeFile);
    //comboTel->addItems(instruments->namesList);
    //slotTelescopeChanged(instrNum);

 //general
    //imgLabel->aperture = settings->value("general/aperture").toInt();
    apeSpinBox ->setValue(settings->value("general/aperture").toInt());
    //obsName = settings->value("general/
    isRedRef = settings->value("general/isRedRef", 0).toInt();
    headType = settings->value("general/headType", 0).toInt();
//
   useTmu = settings->value("tmu/useTmu", 0).toInt();
   tmuFile = settings->value("tmu/tmuFile", "file.txt").toString();
//identify
    identType = settings->value("identify/identType", 0).toInt();
    targNum = settings->value("identify/targNum", 4).toInt();
//wcs
    wcsSpinBox->setValue(settings->value("wcs/minRefStars").toInt());
    sigmaN = settings->value("wcs/sigma", 3).toDouble();

//catalogs
    observatoryCat = settings->value("catalogs/observatoryCat", "./../../../data/cats/Obs.txt").toString();
    mag_1 = settings->value("catalogs/mag0", 6.0).toDouble();
    mag_2 = settings->value("catalogs/mag1", 16.0).toDouble();

//processes
    starCat = new catFinder;
    starCat->exeName = settings->value("ucac2/exeName").toString();
    starCat->exePath = settings->value("ucac2/exePath").toString();
    starCat->catType = settings->value("ucac2/catType").toInt();
    starCat->catName = settings->value("ucac2/catName").toString();
    starCat->catPath = settings->value("ucac2/catPath").toString();
    starCatList << starCat;
    //
    starCat = new catFinder;
    starCat->exeName = settings->value("usnob/exeName").toString();
    starCat->exePath = settings->value("usnob/exePath").toString();
    starCat->catType = settings->value("usnob/catType").toInt();
    starCat->catName = settings->value("usnob/catName").toString();
    starCat->catPath = settings->value("usnob/catPath").toString();
    starCatList << starCat;
    //
    starCat = new catFinder;
    starCat->exeName = settings->value("ucac3/exeName").toString();
    starCat->exePath = settings->value("ucac3/exePath").toString();
    starCat->catType = settings->value("ucac3/catType").toInt();
    starCat->catName = settings->value("ucac3/catName").toString();
    starCat->catPath = settings->value("ucac3/catPath").toString();
    starCatList << starCat;
    //
    starCat = new catFinder;
    starCat->exeName = settings->value("lspm/exeName").toString();
    starCat->exePath = settings->value("lspm/exePath").toString();
    starCat->catType = settings->value("lspm/catType").toInt();
    starCat->catName = settings->value("lspm/catName").toString();
    starCat->catPath = settings->value("lspm/catPath").toString();
    starCatList << starCat;
    //
    starCat = new catFinder;
    starCat->exeName = settings->value("lspmFind/exeName").toString();
    starCat->exePath = settings->value("lspmFind/exePath").toString();
    starCat->catType = settings->value("lspmFind/catType").toInt();
    starCat->catName = settings->value("lspmFind/catName").toString();
    starCat->catPath = settings->value("lspmFind/catPath").toString();
    starCatList << starCat;
    //
    //qDebug() << QString("starCatList count: %1\n").arg(starCatList.count());
    ast_eph_prog = settings->value("processes/ast_eph_prog", "./mpeph.exe").toString();
    ast_eph_prog_folder = settings->value("processes/ast_eph_prog_folder", "./").toString();
    get_http_prog = settings->value("processes/get_http_prog", "./getHttpHeader.exe").toString();
    get_http_prog_folder = settings->value("processes/get_http_prog_folder", "./").toString();
    skybot_prog = settings->value("processes/skybot_prog", "./skybotclient.exe").toString();
    skybot_prog_folder = settings->value("processes/skybot_prog_folder", "./").toString();

    int i;
    int catSz = starCatList.count();

    for(i=0; i<catSz;i++)
    {
        //qDebug() << QString("catType= %1\n").arg(starCatList.at(i)->catType);
        catSelect->insertItem(starCatList.at(i)->catType, starCatList.at(i)->catName);
    }
    currentCat = settings->value("catalogs/currentCatalog", 0).toInt();
    catSelect->setCurrentIndex(currentCat);
//marks
    isMove2corner = settings->value("marks/isMove2corner", 0).toInt();

//network
    proxyHost->clear();
    proxyHost->insert(settings->value("network/proxyHost", "proxy.gao.spb.ru").toString());
    proxyPort->setValue(settings->value("network/proxyPort", 3128).toInt());
    useProxy->setChecked(settings->value("network/useProxy", 0).toInt());
//plateDB
    plateHostName = settings->value("plateDB/plateHostName", "www.puldb.ru/db/plates/fits.php").toString();
//celestial
    obsCode->clear();
    obsCode->insert(settings->value("celestial/obsName", "084").toString());
    maxObjDisp = settings->value("celestial/maxObjDisp", 2).toDouble();
//objectsFind
    magObj0 = settings->value("objectsFind/mag0", 6.0).toDouble();
    magObj1 = settings->value("objectsFind/mag1", 16.0).toDouble();
}

void MainWindow::initInstr()
{
    qDebug() << "\nMainWindow::initInstr()\n";
    comboTel->clear();
    fitsdata *curFits = imgLabel->getCurFits();
    if(curFits!=NULL)
    {
        comboTel->addItems(curFits->instr->namesList);

        slotTelescopeChanged(instrNum);
    }
}

void MainWindow::setSettName()
{
    qDebug() << "\nMainWindow::setSettName\n";
    fitsdata *curFits = imgLabel->getCurFits();
    if(curFits!=NULL)
    {
        comboTel->setCurrentIndex(curFits->instr->curInst);

        xpsSpinBox->setValue(grad_to_mas(curFits->instr->scx));
        qDebug() << "\nscx\t" << curFits->instr->scx;
        ypsSpinBox->setValue(grad_to_mas(curFits->instr->scy));
        qDebug() << "\nscy\t" << curFits->instr->scy;
        fovSpinBox->setValue(curFits->instr->fov);
        rotAngSpinBox->setValue(curFits->instr->rang);

        imgLabel->setInstProp(mas_to_grad(xpsSpinBox->value()), mas_to_grad(ypsSpinBox->value()), fovSpinBox->value(), rotAngSpinBox->value());
    }
//	saveSettButton->setEnabled(instruments->isEdt);
}

//BEGIN menu slots
//menu file
void MainWindow::slotOpenFitsFile()// neio aey ioe?uoey fits oaeea
{
    fitsdata *fitsd;
    QStringList fitsFileNames = QFileDialog::getOpenFileNames(// iieo?aai nienie oaeeia aey ioe?uoey
                                    this,
                                    tr("Choose files"),
                                    currDir,
                                    "CCD (*.img *.fit *.fits *.fts)");
    for (int i=0;i<fitsFileNames.count();i++)// oeee aey ioe?uoey anao oaeeia ii eiaiai a nienea fitsFileNames
    {
        fitsd = new fitsdata;
        if(useTmu) fitsd->initTmu(tmuFile);
        if(fitsd->openFile(fitsFileNames[i], headType))
        {
            QMessageBox::critical(0,"Error",QString("Error while opening fits file %1").arg(fitsFileNames[i]),QMessageBox::Ok,0,0);
            delete fitsd;
            continue;
        }
        fitsd->is_scaled = 0;
        fitsd->sigmaN = sigmaN;
        imgLabel->fitsdataList << fitsd;// aiaaaeyai eieoeaeece?iaaiiue oeacaoaeu ia iauaeo fitsdata a nienie oeacaoaeae ia yoe iauaeou imgLabel->fitsdataList
        if(isRedRef&&(!obsList->getobsynumO(obsCode->text().toAscii().data())))
        {
            //fitsd->initRefractParam(obsList->record);
        }
//		marksGrid *mGr = new marksGrid();
//		imgLabel->marksList << mGr;
        //QMessageBox::information(0,"stimpro",fitsd->fitsHeader,QMessageBox::Ok,0,0);
        slotScrUpdate();// iaiiaeyai ecia?a?aiea ia ye?aia
    }

    if (fitsFileNames.count()>0)
    {



//		setWindowTitle("stimpro:"+imgLabel->fitsdataList[imgLabel->frameIndex]->fileName);// iieacuaaai eiy oaeea a iiea caaieiaea aeaaiiai ieia
        fillFrameList();
//		frameList->selectRow(imgLabel->frameIndex);
        /*
        		fitsd = imgLabel->getCurFits();
        		levelBinSlider->setRange(fitsd->minp, 3.0*fitsd->maxp);
        		levelBinSlider->setValue(fitsd->maxp);
        		radeLineEdit->setText(mas_to_hms(grad_to_mas(fitsd->WCSdata[2]), " ",3)+" "+mas_to_damas(grad_to_mas(fitsd->WCSdata[3]), " ",2)+QString("%1 ").arg(mag_1, 4, 'f', 1, '0')+QString("%1").arg(mag_2, 4, 'f', 1, '0'));

        		DATEOBS dobs = getDATEOBSfromMJD(fitsd->MJD);
        		timeLineEdit->setText(getStrFromDATEOBS(dobs, " ", 2, 4));
        		expLineEdit->setText("0");
        */
        slotSetCurFrame(imgLabel->frameIndex);
        setCurDirFile(fitsFileNames[fitsFileNames.count()-1]);
    }
};

void MainWindow::slotOpenEmpty()
{
    fitsdata *fitsd = new fitsdata();
    fitsd->openEmptyFile();
    qDebug() << "\nCreated\n";

    imgLabel->fitsdataList << fitsd;
    qDebug() << "\nfitsdataList added\n";

    setWindowTitle("stimpro: empty");// iieacuaaai eiy oaeea a iiea caaieiaea aeaaiiai ieia
    qDebug() << "\nsetWindowTitle\n";
    fillFrameList();
    qDebug() << "\nfillFrameList\n";
    frameList->selectRow(imgLabel->frameIndex);
    qDebug() << "\nselectRow\n";
    fitsd = imgLabel->getCurFits();
    levelBinSlider->setRange(fitsd->minp, 3.0*fitsd->maxp);
    levelBinSlider->setValue(fitsd->maxp);

    slotSetCurFrame(imgLabel->frameIndex);
    //setCurDirFile(fitsFileNames[fitsFileNames.count()-1]);
}

void MainWindow::slotLoadHeader()
{
    qDebug() << "\nslot load header\n";
    fitsdata *fitsd = imgLabel->getCurFits();
    if(fitsd==NULL)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Open Empty"));
        msgBox.setText(tr("There is no file opened.\nDo you want to load empty file?"));
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        msgBox.setEscapeButton(QMessageBox::No);


        if (msgBox.exec()==QMessageBox::Yes)
        {
            slotOpenEmpty();
        }
        else return;
    }

    QString fileName = QFileDialog::getOpenFileName(this,
                       tr("Open Fits Header"),
                       currDir,
                       tr("All Files (*);;Text Files (*.txt)"));

    if(fitsd->loadHeaderFile(fileName))
    {
        QMessageBox msgBox(QMessageBox::Warning, tr("Can't open Header file"), "Mentioned FITS Header file has't exist", 0, this);
        msgBox.addButton(QMessageBox::Ok);
        msgBox.exec();
    }
    else slotSetHeaderPanel();

}

void MainWindow::slotSetHeaderPanel()
{
    fitsdata *fitsd = imgLabel->getCurFits();

        int i;

        int tObsNum = 0;
        expList.clear();
        expNumSel->clear();
        QString dateStr, timeStr, kVal;
        QStringList tList;
        ExposureRec* eRec;

        int endObs = 0;
        double tObs0, tObs1, time;
        DATEOBS dateObs0, dateObs1;
        HeadRecord *hRec;



        fitsd->headList.getKeyName("OBJECT", &kVal);
        objLE->setText(kVal);


        fitsd->headList.getKeyName("PLATE-N", &kVal);
        plateNLE->setText(kVal.simplified());

        fitsd->headList.getKeyName("RA", &kVal);
        raLE->setText(kVal);

        fitsd->headList.getKeyName("DEC", &kVal);
        deLE->setText(kVal);

        fitsd->headList.getKeyName("N-EXP", &kVal);
        numOfExpLE->setText(kVal);

        fitsd->headList.getKeyName("U", &kVal);
        uTimeLE->setText(kVal);

        fitsd->headList.getKeyName("OBSERVER", &kVal);
        obsLE->setText(kVal);

        fitsd->headList.getKeyName("TELESCOP", &kVal);
        telLE->setText(kVal);


        //eRec = new ExposureRec;

        int sz = fitsd->expList->exps.size();

        for(i=0; i<sz; i++)
        {

            expNumSel->insertItem(i, QString("%1").arg(i, 3));
        }

        expNumSel->insertItem(sz, QString("Mean"));

        expNumSel->setCurrentIndex(expNumSel->count()-1);
        qDebug() << QString("setCurrentIndex\n");
/*
        if(isRedRef)
        {
            if(!obsList->getobsynumO(obsCode->text().toAscii().data())) fitsd->initRefractParam(obsList->record);
            //else qDebug() << QString("can't find %1 obs\n").arg(obsName);
        }
*/
        slotFitsHeaderPanel();
}

void MainWindow::slotLoadHttpHeader()
{
    fitsdata *fitsd = imgLabel->getCurFits();
    if(fitsd==NULL)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Open Empty"));
        msgBox.setText(tr("There is no file opened.\nDo you want to load empty file?"));
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        msgBox.setEscapeButton(QMessageBox::No);


        if (msgBox.exec()==QMessageBox::Yes)
        {
            slotOpenEmpty();
        }
        else return;
    }



    http = new QHttp(this);

    QTextCodec *codecHttp = QTextCodec::codecForName("Latin-1");

    plateNLE->setText(QString("10476"));


    getHttpDlg *getHDlg = new getHttpDlg(0);
    getHDlg->ui_plateNum->setText(plateNLE->text());
    if(!getHDlg->exec()) return;

    telLE->setText(getHDlg->ui_instr->currentText());
    plateNLE->setText(getHDlg->ui_plateNum->text());
/*
    plateHostName = QString("www.puldb.ru/db/plates/fits.php");//settings->value("general/hostName").toString();// понятно... имя хоста, вдруг изменят. Поэтому разумно его держать в настройках
    proxyName = QString("proxy.gao.spb.ru");//settings->value("general/proxyName").toString();// ну это прокси
    //QString replyDir = QString("./");//settings->value("general/replyDir").toString();// это папка, в которую выдается файлик с результатом
    proxyPort = 3128;//settings->value("general/proxyPort").toInt();// не по порядку правда - это порт прокси
    useProxy = 1;//settings->value("general/useProxy").toInt();// 0 - не используем прокси, 1 - используем
*/

    QString pnStr;


    QTextCodec *codec1 = QTextCodec::codecForName("Windows-1251");
    Q_ASSERT( codec1 );
    QByteArray bpn = codec1->fromUnicode(getHDlg->ui_plateNum->text());
    //qDebug() << QString("bpn: %1\n").arg(bpn.constData());
    //pnStr = bpn.toPercentEncoding("-/._~=?&");
    qDebug() << QString("pnStr: %1\n").arg(pnStr);
    pnStr = getHDlg->ui_plateNum->text();


    QProcess outerProcess;
    QStringList outerArguments;
    outerArguments.clear();
    QString ucac2find_folder("");

    outerProcess.setWorkingDirectory(get_http_prog_folder);
    outerProcess.setProcessChannelMode(QProcess::MergedChannels);
    outerProcess.setReadChannel(QProcess::StandardOutput);
    outerArguments << getHDlg->ui_instr->currentText() << pnStr;
    qDebug() << get_http_prog << outerArguments.join(" ");
    outerProcess.start(get_http_prog, outerArguments);

    outerProcess.waitForFinished(-1);
    QTextStream catStream(outerProcess.readAllStandardOutput());

    //QByteArray httpData = catStream.readAll();// http->readAll();//чтение данных (ответа)

    fitsd = imgLabel->getCurFits();
    if(!fitsd->readHttpHeader(catStream.readAll()))
    {
        slotSetHeaderPanel();
        setWindowTitle(QString("stimpro: %1").arg(plateNLE->text()));
        slotFromHeader();
    }
}
/*
void MainWindow::slotProcessingData(bool error)
{
    int i, sz;
    QString kName, kVal, tstr;
    QStringList headVals, keyVals;
    HeadRecord *headRec;
    int pos0, pos1;

    if (error)//если ошибка - сообщить в консоль и выйти из приложения
        {
                qDebug() << "error has occured.\n";
        }
        else
        {
            fitsdata *fitsd = imgLabel->getCurFits();
            fitsd->headList.clear();

            qDebug() << "data is being loaded...\n";
            QByteArray httpData = http->readAll();//чтение данных (ответа)

            if(!fitsd->readHttpHeader(httpData))
            {
                slotSetHeaderPanel();
                setWindowTitle(QString("stimpro: %1").arg(plateNLE->text()));
                slotFromHeader();
            }
        }
}



void MainWindow::slotStateChanged(int state)
{
        if (eventMessages)
        {
                QTextStream stream(stdout);
                QString str_state;
                switch(state)
                {
                        case QHttp::Unconnected:
                          str_state = "There is no connection to the host.";
                          break;
                        case QHttp::HostLookup:
                          str_state = "A host name lookup is in progress.";
                          break;
                        case QHttp::Connecting:
                          str_state = "An attempt to connect to the host is in progress.";
                          break;
                        case QHttp::Sending:
                          str_state = "The client is sending its request to the server.";
                          break;
                        case QHttp::Reading:
                          str_state = "The client's request has been sent and the client is reading the server's response.";
                          break;
                        case QHttp::Connected:
                          str_state = "The connection to the host is open, but the client is neither sending a request, nor waiting for a response.";
                          break;
                        case QHttp::Closing:
                          str_state = "The connection is closing down, but is not yet closed.";
                          break;
                }
                qDebug() << str_state << "\n";
        }
}

void MainWindow::slotRequestFinished(int id, bool error)
{
        if(error)
        {
            QTextStream stream(stdout);
                qDebug() << http->errorString() << "\n";
        }
}
*/
void MainWindow::slotExpNumChanged(int eNum)
{
    qDebug() << QString("eNum= %1\texpList= %2\n").arg(eNum).arg(expList.size());
//    if(eNum>expNumSel->||eNum<0) return;
    fitsdata *fcurr = imgLabel->getCurFits();
    ExposureRec* eRec;
    if(fcurr!=NULL)
    {
        eRec = new ExposureRec;
        //if((eNum<fcurr->expList->exps.size())&&!(eNum<0))
        //{
            fcurr->expList->getExp(eRec, eNum);

        /*}
        else
        {

            fcurr->expList->getMean(eRec);

        }*/
        dateObsLE->setText(eRec->timeStr());
        expLE->setText(QString("%1").arg(eRec->expVal));
        fcurr->MJD = eRec->expTime;
        fcurr->exptime = eRec->expVal;
    }
}



void MainWindow::fillFrameList()
{
    QCheckBox *frameCheck;
    frameList->clearContents();
    for(int i=0; i<imgLabel->fitsdataList.size(); i++)
    {
        frameList->setRowCount(i+1);
        frameItem = new QTableWidgetItem(imgLabel->fitsdataList[i]->fileName.section('/', -1));
        if (imgLabel->fitsdataList[i]->is_checked) frameItem->setCheckState(Qt::Checked);
        else frameItem->setCheckState(Qt::Unchecked);
        frameList->setItem(i, 0, frameItem);
        frameItem = new QTableWidgetItem(imgLabel->fitsdataList[i]->fileName.section('/', 0, -1));
        frameList->setItem(i, 1, frameItem);
        frameItem = new QTableWidgetItem();
        if(imgLabel->fitsdataList[i]->WCSdata[12]) frameItem->setIcon(QIcon(".\\icons\\Check2.bmp"));
        else frameItem->setIcon(QIcon(".\\icons\\Delete2.bmp"));
        frameList->setItem(i, 2, frameItem);
    }
   // frameList->resizeColumnToContents(0);
  //  frameList->resizeColumnToContents(1);

}

void MainWindow::slotFrameListClicked(int r, int c)
{
//	QMessageBox::information(0,"stimpro",QString("%1 %2").arg(r).arg(c),QMessageBox::Ok,0,0);
    this->slotSetCurFrame(r);
    /*	switch(c)
    	{
    		case 0:
    		imgLabel->fitsdataList[r]->is_checked = !imgLabel->fitsdataList[r]->is_checked;
    		break;
    		case 1:
    		this->slotSetCurFrame(r);
    		break;
    	}*/
}

void MainWindow::slotSetCurFrame(int i)
{
    qDebug() << "slotSetCurFrame\n";
    int pi;
    QTableWidgetItem *frItem;
    if ((i<(imgLabel->fitsdataList.count()))&&(i>=0))
    {
        frItem = frameList->item(imgLabel->frameIndex, 0);
        if(frItem!=NULL)frItem->setCheckState(Qt::Unchecked);
        imgLabel->frameIndex=i;//
        setWindowTitle("stimpro:"+imgLabel->fitsdataList[imgLabel->frameIndex]->fileName);// iieacuaaai eiy oaeea a iiea caaieiaea aeaaiiai ieia

        frameList->selectRow(i);
        //
        pi = frameList->currentRow();
        frItem = frameList->item(pi, 0);
        frItem->setCheckState(Qt::Checked);
        frItem = frameList->item(pi, 2);


		errStr.clear();
		posStr.clear();
//		setWindowTitle("stimpro:"+imgLabel->fitsdataList[imgLabel->frameIndex]->fileName);// iieacuaaai eiy oaeea a iiea caaieiaea aeaaiiai ieia
//		fillFrameList();
//		frameList->selectRow(imgLabel->frameIndex);

        fitsdata* fitsd = imgLabel->getCurFits();
        levelBinSlider->setRange(fitsd->minp, 3.0*fitsd->maxp);
        levelBinSlider->setValue(fitsd->maxp);

        if(fitsd->WCSdata[12]) frItem->setIcon(QIcon(".\\icons\\Check2.bmp"));
        else frItem->setIcon(QIcon(".\\icons\\Delete2.bmp"));
//        radeLineEdit->setText(mas_to_hms(grad_to_mas(fitsd->WCSdata[2]), " ",3)+" "+mas_to_damas(grad_to_mas(fitsd->WCSdata[3]), " ",2)+QString("%1 ").arg(mag_1, 4, 'f', 1, '0')+QString("%1").arg(mag_2, 4, 'f', 1, '0'));

        DATEOBS dobs = getDATEOBSfromMJD(fitsd->MJD);
        timeLineEdit->setText(QString(getStrFromDATEOBS(dobs, " ", 2, 4)));
        if(!fitsd->is_empty) expLineEdit->setText(QString("%1").arg(fitsd->exptime, 4, 'f', 0));

        updateWCS(&fitsd->WCSdata[0]);



        if(fitsd->instr==NULL)
        {
            fitsd->initInst(telescopeFile, instrNum);

            //slotTelescopeChanged(instrNum);
        }
        initInstr();
        imgLabel->setAperture(fitsd->getApeSize(apeSpinBox->value()));
    }
    else
    {
        updateWCS();
    }
    slotUpdateGridStat();
    slotScrUpdate();//i?i?eniaea ecia?a?aiey
}

void MainWindow::slotUpdateGridStat()
{
    qDebug() << "slotUpdateGridStat\n";
    fitsdata* fitsd = imgLabel->getCurFits();
    if(fitsd!=NULL)
    {
        //ipixBtn->setText();
        ipixBtn->setText(QString("%1").arg(fitsd->marksGIpix->marks.size()));
        catBtn->setText(QString("%1").arg(fitsd->marksG->marks.size()));
        refBtn->setText(QString("%1").arg(fitsd->refMarks->marks.size()));
        objBtn->setText(QString("%1").arg(fitsd->objMarks->marks.size()));
    }
    else
    {
        ipixBtn->setText("0");
        catBtn->setText("0");
        refBtn->setText("0");
        objBtn->setText("0");
    }
}

void MainWindow::slotFindHstars()
{
    fitsdata *fitsd = imgLabel->getCurFits();
    int starg = starTargetSpinBox->value();
    if(fitsd!=NULL)
    {
        if(fitsd->findHstars(fitsd->getApeSize(apeSpinBox->value()), starg)) QMessageBox::warning(0, "Find Hstars error", "Target number of stars are not achieved");
    }
    slotScrUpdate();
}

void MainWindow::slotOpenScaledFItsFile()
{
    QStringList fitsFileNames = QFileDialog::getOpenFileNames(
                                    this,
                                    tr("Choose files"),
                                    "",
                                    "CCD (*.img *.fit *.fits *.fts)");
    for (int i=0;i<fitsFileNames.count();i++)
    {
        fitsdata *fitsd = new fitsdata(fitsFileNames[i]);
        fitsd->is_scaled = 1;
        imgLabel->fitsdataList << fitsd;
        //QMessageBox::information(0,"stimpro",fitsd->fitsHeader,QMessageBox::Ok,0,0);
        slotScrUpdate();
    }
};


void MainWindow::slotExit()//auoia ec i?eei?aiey
{
    qDebug() << "exit\n";
    slotSaveDefaultPanels();
    slotSaveSettings();
    QApplication::exit();
};

void MainWindow::slotSaveSettings()
{
    //general
       stimproSett->setValue("general/curr_dir", currDir);
       stimproSett->setValue("general/log_path", logDir);

    //gridstatw
       stimproSett->setValue("gridstatw/ipixCol", ipixColStr);
       stimproSett->setValue("gridstatw/catCol", catColStr);
       stimproSett->setValue("gridstatw/refCol", refColStr);
       stimproSett->setValue("gridstatw/objCol", objColStr);
       stimproSett->setValue("gridstatw/bgCol", bgColStr);

    stimproSett->sync();

    //settings->setValue("general/curr_dir", currDir);

    //settings->setValue("general/log_path", logDir);

    settings->setValue("general/aperture", imgLabel->aperture);
    settings->setValue("wcs/wcsN",  wcsSpinBox->value());
//    settings->setValue("general/isRedRef",  isRedRef);
//    isRedRef = settings->value("general/isRedRef", 0).toInt();
//telescope
    settings->setValue("telescope/instrNum", comboTel->currentIndex());

//catalogs
    //settings->setValue("catalogs/observatoryCat", observatoryCat);
    settings->setValue("catalogs/currentCatalog", currentCat);

//processes
//    settings->setValue("processes/ucac2find", starCat.exeName);


 //   settings->setValue("processes/ast_eph_prog", ast_eph_prog);
 //   settings->setValue("processes/ast_eph_prog_folder", ast_eph_prog_folder);
//marks
    settings->setValue("marks/isMove2corner", isMove2corner);

//network

    settings->setValue("network/proxyHost", proxyHost->text());
    settings->setValue("network/proxyPort", proxyPort->value());
    settings->setValue("network/useProxy", (int)useProxy->isChecked());
//plateDB
    settings->setValue("plateDB/plateHostName", plateHostName);
//celestial

    settings->setValue("celestial/obsName", obsCode->text());
    settings->setValue("celestial/maxObjDisp", maxObjDisp);

    settings->sync();
}


void MainWindow::slotIncrease()
{
    if (imgLabel->scale<=16)
    {
        imgLabel->scale = imgLabel->scale*1.2;//oaaaeaaai ianooaa i?i?eniaee ecia?a?aiey (ianooaa ia aieuoa, ?ai 16)
        slotScrUpdate();// iaiiaeyai ecia?a?aiea ia ye?aia
    }
};

void MainWindow::slotDecrease()
{
    if (imgLabel->scale>0.025)
    {
        imgLabel->scale = imgLabel->scale/1.2;//oiaiuoaai ianooaa i?i?eniaee ecia?a?aiey a aaa ?aca (ianooaa ia iaiuoa, ?ai 1/16)
        slotScrUpdate();// iaiiaeyai ecia?a?aiea ia ye?aia
    }
};

void MainWindow::slotNext()
{
    slotSetCurFrame(imgLabel->frameIndex+1);
    /*	if(imgLabel->frameIndex<(imgLabel->fitsdataList.count()-1))
    	{
    		imgLabel->frameIndex++;//oaaee?aiea oaeouaai eiaaena ia 1
    		setWindowTitle("stimpro:"+imgLabel->fitsdataList[imgLabel->frameIndex]->fileName);// iieacuaaai eiy oaeea a iiea caaieiaea aeaaiiai ieia
    		slotScrUpdate();//i?i?eniaea ecia?a?aiey
    	}*/
};


void MainWindow::slotPreviouse()
{
//	if(imgLabel->frameIndex>0)
//	{
    slotSetCurFrame(imgLabel->frameIndex-1);//oiaiuoaiea oaeouaai eiaaena ia 1
//		setWindowTitle("stimpro:"+imgLabel->fitsdataList[imgLabel->frameIndex]->fileName);// iieacuaaai eiy oaeea a iiea caaieiaea aeaaiiai ieia
//		slotScrUpdate();//i?i?eniaea ecia?a?aiey
//	}
};

void MainWindow::slotFirst()
{
    slotSetCurFrame(0);
    /*
    	if(imgLabel->fitsdataList.count()>0)
    	{
    		imgLabel->frameIndex = 0;//
    		setWindowTitle("stimpro:"+imgLabel->fitsdataList[imgLabel->frameIndex]->fileName);// iieacuaaai eiy oaeea a iiea caaieiaea aeaaiiai ieia
    		slotScrUpdate();//i?i?eniaea ecia?a?aiey
    	}*/
}

void MainWindow::slotLast()
{
    slotSetCurFrame(imgLabel->fitsdataList.count()-1);
    /*	if(imgLabel->fitsdataList.count()>0)
    	{
    		imgLabel->frameIndex = imgLabel->fitsdataList.count()-1;//
    		setWindowTitle("stimpro:"+imgLabel->fitsdataList[imgLabel->frameIndex]->fileName);// iieacuaaai eiy oaeea a iiea caaieiaea aeaaiiai ieia
    		slotScrUpdate();//i?i?eniaea ecia?a?aiey
    	}*/
}


void MainWindow::slotDelete()
{
    if (imgLabel->fitsdataList.count()>0)
    {
        fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];//ia?aaaai oeacaoaeu oaeouaai iauaeoa fitsdata ii eiaaeno a fitsd
        fitsd->marksG->clearMarks();
        imgLabel->fitsdataList.removeAt(imgLabel->frameIndex);// oaaeyai oeacaoaeu ec nienea
//        delete fitsd;//inaiai?aaai iaiyou
        if ((imgLabel->frameIndex==imgLabel->fitsdataList.count()))imgLabel->frameIndex--;//anee oaeouee eiaaen iineaaiee a nienea, oiaiuoaai aai ia 1
        if (imgLabel->frameIndex<0)
        {
            imgLabel->frameIndex=0;
            setWindowTitle("stimpro");
            imgLabel->repaint();
        }// anee oaaeai iineaaiee iauaeo nienea, oae?aai eiy oaeea ec caaieiaea ieia e onoaiaaeeaaai oaeouee eiaaen a 0
        else setWindowTitle("stimpro:"+imgLabel->fitsdataList[imgLabel->frameIndex]->fileName);//eia?a, ia?aaaeuaaai caaieiaie aeaaiiai ieia a niioaaonoaea n oaeouei eiaaenii
        slotSetCurFrame(imgLabel->frameIndex);
        slotScrUpdate();//i?i?eniaea ecia?a?aiey
    }
    fillFrameList();
    frameList->selectRow(imgLabel->frameIndex);
};

/////////////////////////////////////////

void MainWindow::slotBackLeft()//1. niauaiea eaaie a?aieou "uaee i?i?eniaee" aeaai
{
    if (imgLabel->fitsdataList.count()>0)//?aaioaai, anee anou n ?ai ?aaioaou
    {
        fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];//ia?aaaai oeacaoaeu oaeouaai iauaeoa fitsdata ii eiaaeno a fitsd
        fitsd->minp=fitsd->minp-fitsd->st;// oiaiuoaai fitsd->minp ia fitsd->st (oaa eciaiaiey "uaee i?i?eniaee")
        if (fitsd->minp<fitsd->minv)fitsd->minp=fitsd->minv;
        slotScrUpdate();//i?i?eniaea ecia?a?aiey
    }
};
void MainWindow::slotBackRight()//2. niauaiea eaaie a?aieou "uaee i?i?eniaee" ai?aai
{
    if (imgLabel->fitsdataList.count()>0)
    {
        fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
        fitsd->minp=fitsd->minp+fitsd->st;
        if (fitsd->minp>fitsd->maxv)fitsd->minp=fitsd->maxv;
        slotScrUpdate();
        //QMessageBox::information(0,"stimpro",QString("%1").arg(fitsd->st,8,'f',0,QLatin1Char( ' ' )),QMessageBox::Ok,0,0);
    }
};
void MainWindow::slotSignalLeft()//3. niauaiea i?aaie a?aieou "uaee i?i?eniaee" aeaai
{
    if (imgLabel->fitsdataList.count()>0)
    {
        fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
        fitsd->maxp=fitsd->maxp-fitsd->st;
        if (fitsd->maxp<fitsd->minv)fitsd->maxp=fitsd->minv;
        slotScrUpdate();
    }
};
void MainWindow::slotSignalRight()//4. niauaiea i?aaie a?aieou "uaee i?i?eniaee" ai?aai
{
    if (imgLabel->fitsdataList.count()>0)
    {
        fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
        fitsd->maxp=fitsd->maxp+fitsd->st;
        if (fitsd->maxp>fitsd->maxv)fitsd->maxp=fitsd->maxv;
        slotScrUpdate();
    }
};

void MainWindow::slotSetDefault()//5. aica?auaiea e ia?aiao?ai ii-oiie?aie?, au?eneaiiui i?e ioe?uoee fits oaeea
{
    if (imgLabel->fitsdataList.count()>0)
    {
        fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
        fitsd->maxp=fitsd->fpix+fitsd->hw;
        fitsd->minp=fitsd->fpix-fitsd->hw;
        fitsd->st=2*fitsd->hw/25;
        slotScrUpdate();
    }
};

void MainWindow::slotSetFullDiapason()//6. onoaiiaea "uaee i?i?eniaee" ?aaiie anaio aeaiaciio cia?aiee ia ieenaeyo
{
    if (imgLabel->fitsdataList.count()>0)
    {
        fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
        fitsd->maxp=fitsd->maxv;
        fitsd->minp=fitsd->minv;
        fitsd->st=(fitsd->maxv-fitsd->minv)/25;
        slotScrUpdate();
    }
}
//END menu slots
void MainWindow::slotMarks()
{

    /*
    	double *pp;
    	pp = new double[2];

    	pp[0] = 1.0;
    	pp[1] = 2.0;

    	tL << pp;
    */
//	marksGrid *curMarks = this->imgLabel->marksList.at();
    double fov;
    long max[2];
    long amax;
    if (imgLabel->fitsdataList.count()>0)
    {
        if (saveMarksPos->checkState()==Qt::Unchecked)imgLabel->fitsdataList[imgLabel->frameIndex]->marksG->setMarksCenter(0.0, 0.0);
        imgLabel->setInstProp(mas_to_grad(xpsSpinBox->value()), mas_to_grad(ypsSpinBox->value()), fovSpinBox->value(), grad_to_rad(rotAngSpinBox->value()));

        QString comLine = radeLineEdit->text();
        comLine = comLine.trimmed();
        QStringList operands = comLine.split(" ");
        double ra_oc = hms_to_mas(operands[0] +" "+ operands[1] +" "+ operands[2]," ");
        double de_oc = damas_to_mas(operands[3] +" "+ operands[4] +" "+ operands[5]," ");
        QString str = operands[6];
        mag_1 = str.toDouble();
        str = operands[7];
        mag_2 = str.toDouble();

        /*int catN;
        if (rbCatUCAC2->isChecked())catN = 0;
        if (rbCatUSNOB->isChecked())catN = 1;*/

        fitsdata *fitsd = imgLabel->getCurFits();
        marksGrid *mGr = imgLabel->getCurMarks();
        if(!fitsd->WCSdata[12])
        {
            fitsd->WCSdata[2] = mas_to_grad(ra_oc);
            fitsd->WCSdata[3] = mas_to_grad(de_oc);
        }
        fitsd->MJD = getMJDfromStrFTN(timeLineEdit->text(), 0);
        qDebug() << "\nGetMarksGrid\n";
        qDebug() << "\nWCS2\t" << fitsd->WCSdata[2] << "\t" << fitsd->WCSdata[3] << "\n";
        /*		if(!fitsd->is_empty) fov =  fovSpinBox->value()*fitsd->detFov();
        		else
        		{
        			max[0] = max[1] = 0;
        			for(int i=0; i<mGr->marks.size(); i++)
        			{
        				if(max[0]<mGr->marks[i]->mTanImg[0]) max[0] = mGr->marks[i]->mTanImg[0];
        				if(max[1]<mGr->marks[i]->mTanImg[1]) max[1] = mGr->marks[i]->mTanImg[1];
        			}
        			qDebug() << "\nnaxes\n";
        			qDebug() << max[0] << "\t" << max[1] << "\n";
        //			fcurr->imgArr->naxes[0] = max[0];
        //			fcurr->imgArr->naxes[1] = max[1];
        			qDebug() << fitsd->imgArr->naxes[0] << "\t" << fitsd->imgArr->naxes[1] << "\n";
        			if(max[0]>max[1]) amax = max[0];
        			else amax = max[1];
        			fov = fovSpinBox->value()*0.01*amax*mGr->scales[0]/60000.0/fitsd->scFactor;
        		}*/
//		imgLabel->getMarksGrid(0, fovSpinBox->value(), mag_1, mag_2);
        fitsd->marksG->clearMarks();
        fitsd->getMarksGrid(starCatList.at(catSelect->currentIndex()), fovSpinBox->value(), mag_1, mag_2);
 //       fitsd->detTan();
//		if(!fitsd->is_empty) mGr->detTan(fitsd->WCSdata, fitsd->imgArr->naxes);
//		else mGr->detTan(fitsd->WCSdata, max);
        fitsd->cleanObjects(fitsd->getApeSize(apeSpinBox->value()));
        slotScrUpdate();
    }
}

int MainWindow::getNumMarks(double magn0, double magn1, int num)
{
    double fov;
    long max[2];
    long amax;

    if (imgLabel->fitsdataList.count()>0)
    {
        if (saveMarksPos->checkState()==Qt::Unchecked)imgLabel->fitsdataList[imgLabel->frameIndex]->marksG->setMarksCenter(0.0, 0.0);
        imgLabel->setInstProp(mas_to_grad(xpsSpinBox->value()), mas_to_grad(ypsSpinBox->value()), fovSpinBox->value(), grad_to_rad(rotAngSpinBox->value()));

        QString comLine = radeLineEdit->text();
        comLine = comLine.trimmed();
        QStringList operands = comLine.split(" ");
        double ra_oc = hms_to_mas(operands[0] +" "+ operands[1] +" "+ operands[2]," ");
        double de_oc = damas_to_mas(operands[3] +" "+ operands[4] +" "+ operands[5]," ");
        QString str = operands[6];
        mag_1 = str.toDouble();
        str = operands[7];
        mag_2 = str.toDouble();

        fitsdata *fitsd = imgLabel->getCurFits();
        marksGrid *mGr = imgLabel->getCurMarks();
        if(!fitsd->WCSdata[12])
        {
            fitsd->WCSdata[2] = mas_to_grad(ra_oc);
            fitsd->WCSdata[3] = mas_to_grad(de_oc);
        }
        fitsd->MJD = getMJDfromStrFTN(timeLineEdit->text(), 0);
        qDebug() << "\nGetMarksGrid\n";
        qDebug() << "\nWCS2\t" << fitsd->WCSdata[2] << "\t" << fitsd->WCSdata[3] << "\n";
        fitsd->marksG->clearMarks();
        fitsd->getMarksGrid(starCatList.at(catSelect->currentIndex()), fovSpinBox->value(), magn0, magn1, num);
        slotScrUpdate();
    }
        return 0;
}
/*
void MainWindow::slotIdentify()
{
    if(imgLabel->fitsdataList.count()>0)
    {
        fitsdata *fitsd = imgLabel->getCurFits();
        marksGrid *mGr = imgLabel->getCurMarks();
        marksGrid *mGrI = imgLabel->getCurMarksIpix();
        fitsd->detPpix(apeSpinBox->value(), 1);
        slotMeasureMarkedButtonClicked(mGr);
//        marksGrid *maG = new marksGrid;
//        int resAuto = imgLabel->identAuto(maG);

//        mGr->marks.clear();
//        mGrI->marks.clear();
/*
        if(resAuto)
        {
            qDebug() << "\nautoident fail\n";
            QMessageBox::information(0, "resAuto", "autoident fail");
            return;
        }
        qDebug() << "\nwcsSpinBox\n";
/
        if(fitsd->detWCS(wcsSpinBox->value(), mGr, smaxSpinBox->value()))
        {
            slotSetCurFrame(imgLabel->frameIndex);
            mGr->marks.clear();
    //		mGrI->marks.clear();
    //		slotMarks();
            fitsd->detTan();

            slotScrUpdate();
        }
        else QMessageBox::information(0, "detWCS", "detWCS fail");
    }

}
*/
void MainWindow::slotIdentify()
{
    qDebug() << QString("MainWindow::slotIdentify\n");
    if(imgLabel->fitsdataList.count()>0)
    {
        fitsdata *fitsd = imgLabel->getCurFits();
        marksGrid *mGr = imgLabel->getCurMarks();
        marksGrid *mGrI = imgLabel->getCurMarksIpix();
        fitsd->detPpix(mGrI, fitsd->getApeSize(apeSpinBox->value()));



        //int resAuto = imgLabel->identAuto(rotAngSpinBox->value(), identType);
        int resAuto = fitsd->identAuto(fitsd->refMarks, rotAngSpinBox->value(), targNum, identType);

        if(resAuto)
        {
            qDebug() << "\nautoident fail\n";
            QMessageBox::information(0, "resAuto", "autoident fail");
            return;
        }
        else qDebug() << "\nidentification successful\n\n";
        mGr->clearMarks();
        mGrI->marks.clear();
        qDebug() << "\nwcsSpinBox\n";

        wcsParams.minRefStars =  wcsSpinBox->value();//settings->value("wcs/minRefStars").toInt());
        wcsParams.sigma = sigmaN;// = settings->value("wcs/sigma", 3).toDouble();
        wcsParams.maxres = settings->value("wcs/maxres", 300).toDouble();
        wcsParams.maxresMAG = settings->value("wcs/maxresMAG", 3).toDouble();
        //wcsParams.minRefMag = settings->value("wcs/minRefMag", 8).toDouble();
        wcsParams.redType = settings->value("wcs/redType", 0).toDouble();
        wcsParams.sMax = settings->value("wcs/sMax", 500).toDouble();
        wcsParams.weights = settings->value("wcs/weights", 0).toDouble();

        //if(fitsd->detWCS(wcsSpinBox->value(), smaxSpinBox->value()))slotSetCurFrame(imgLabel->frameIndex);

        if(fitsd->detWCS1(wcsParams))
        {
            slotSetCurFrame(imgLabel->frameIndex);

    //		slotMarks();
            fitsd->detTan();
     //   mGr->marks.clear();
     //   mGrI->marks.clear();
            if(QMessageBox::question(0, "WCS save dialog", "Save WCS?", QMessageBox::Save, QMessageBox::Cancel)==QMessageBox::Save) slotSaveWCS();

        }
        else QMessageBox::information(0, "detWCS error", QString("detWCS fail\nSx = %1 mas\tSy = %2 mas").arg(grad_to_mas(fitsd->Sx)).arg(grad_to_mas(fitsd->Sy)));

        slotScrUpdate();
    }

}
/*
void MainWindow::slotDeleteCloserThan(int minRange)
{
    int i, j;
    fitsdata *fitsd = imgLabel->getCurFits();
    if(fitsd==NULL) return;
    marksGrid *mGr = imgLabel->getCurMarksIpix();




}
*/

void MainWindow::slotIdentifyAuto()
{
    int frI, j;
    QTableWidgetItem *frItem;
 //   if (imgLabel->fitsdataList.count()>0)
 //   {
        frI = imgLabel->fitsdataList.count();
        for(j=0; j<frI; j++)
        {
            frItem = frameList->item(j, 0);
            if(frItem->checkState()==Qt::Unchecked) continue;
            slotSetCurFrame(j);

            fitsdata *fitsd = imgLabel->getCurFits();
            marksGrid *mGr = imgLabel->getCurMarks();
            marksGrid *mGrI = imgLabel->getCurMarksIpix();
            /*if(!fitsd->is_empty)slotMarks();
            else *///fitsd->getMarksGrid(getNumMarks(6.0, 15.0, 100);
            slotMarks();

           if ((!fitsd->is_empty)&&(!fitsd->WCSdata[12])) if(slotFindStars())
            {
                QMessageBox::information(this, "Failed", "Automatic Identification is failed");
                clearIpixMarks();
                return;
            }

    //        if (!fitsd->is_empty) fitsd->detPpix(imgLabel->aperture, mGrI);
      //      qDebug() << "\ndetPpix\n";
           if (!fitsd->is_empty)slotMeasureIPixMarkedButtonClicked();
           /*slotMarksUnify1(apeSpinBox->value()/2.0);
           slotMeasureIPixMarkedButtonClicked();
           slotMarksUnify1(apeSpinBox->value());*/
    //        slotMeasureMarksGrid(mGrI);
            /*for (int i=0; i<mGrI->marks.size(); i++)
            {
                qDebug() << "\n" << mGrI->marks[i]->mTanImg[0] << "\t" << mGrI->marks[i]->mTanImg[1] << "\t" << mGrI->marks[i]->mTanImg[2];
            }*/

           slotIdentify();
/*
            int resAuto = fitsd->identAuto(fitsd->refMarks, rotAngSpinBox->value());


            if (resAuto)
            {
                qDebug() << "\nautoident fail\n";
                QMessageBox::information(0, "resAuto", "autoident fail");
                continue;
            }

           if(fitsd->detWCS(wcsSpinBox->value(), smaxSpinBox->value()))
            {
               if(QMessageBox::question(0, "WCS save dialog", "Save WCS?", QMessageBox::Save, QMessageBox::Cancel)==QMessageBox::Save)fitsd->saveWCS();
                slotSetCurFrame(imgLabel->frameIndex);
                mGr->marks.clear();

                fitsd->detTan();

                slotScrUpdate();
            }
            else QMessageBox::information(0, "detWCS", "detWCS fail");
            */
        }
  //  }
}

void MainWindow::slotMakeReduction()
{
    fitsdata *fitsd = imgLabel->getCurFits();
    if(fitsd!=NULL)
    {

        fitsd->makeReduction(QString("./conf/ruler3.ini"), "./");

        slotScrUpdate();
    }
}

void MainWindow::slotFindCloserStars()
{
    fitsdata *fitsd = imgLabel->getCurFits();
    if(fitsd!=NULL)
    {
        fitsd->findCloserStars(fitsd->getApeSize(apeSpinBox->value()));
        slotScrUpdate();
    }
}

void MainWindow::slotFindCloserObjects()
{
    fitsdata *fitsd = imgLabel->getCurFits();
    if(fitsd!=NULL)
    {

    }
}

void MainWindow::slotCenterMassButton()
{
    fitsdata *curF = imgLabel->getCurFits();
    if(curF==NULL) return;
    int curI = activMarksGrid->currentIndex();
    marksGrid *mgr;
    switch(curI)
    {
        case 1:
        mgr = curF->marksGIpix;
        break;
        case 2:
        mgr = curF->marksG;
        break;
        default:
        mgr = NULL;
        break;
    }
    slotCenterMass(mgr);
}

void MainWindow::slotCenterMass(marksGrid *mgr)
{
    if ((imgLabel->fitsdataList.count()>0)&&mgr!=NULL)//anee anou ioe?uoua oaeeu, oi ?enoai
    {
        fitsdata *fitsd = imgLabel->getCurFits();//>fitsdataList[imgLabel->frameIndex];// iieo?aai oeacaoaeu ia oaeouee ICN-eaa?
//		uchar* imgdata = baseImg->bits();// iieo?aai oeacaoaeu ia aooa? "ae?ooaeuiiai ye?aia". Anee n?aa ienaou aaiiua, oi iie aoaoo aini?eieiaouny eae ae?ooaeuiia ecia?a?aiea, eioi?ia o?aieony a iaiyoe e ia iieacuaaaony ia ye?aia
//		marksGrid *mGr = fitsd->marksG;
//		QMessageBox::information(NULL, tr("Marks num"), QString("%1").arg(fitsd->marksG->marks.size()), QMessageBox::Ok);
        //fitsd->detPpix(imgLabel->aperture, mgr);
 //       marksGrid *mGr = imgLabel->getCurMarks();
//        marksGrid *mGrI = imgLabel->getCurMarksIpix();
        fitsd->moveMassCenter(mgr, imgLabel->aperture);
        slotScrUpdate();
//		QMessageBox::information(NULL, tr("Marks num"), QString("%1").arg(fitsd->marksG->marks.size()), QMessageBox::Ok);
    }
}



void MainWindow::resizeEvent(QResizeEvent *event)// ia?aii?aaaeaiea iaoiaa resizeEvent (niauoea eciaiaiea ?acia?a) aeaaiiai ieia
{
    slotScrUpdate();// ia?a?eniaea ecia?a?aiey
};

void MainWindow::slotScrUpdate()// ia?a?eniaea ecia?a?aiey
{
    qDebug() << "\nslotScrUpdate()\n";
    long naxes0, naxes1;
    if (imgLabel->fitsdataList.count()>0)//?aaioaai, anee anou ioe?uoua oaeeu
    {
        slotUpdateGridStat();
        fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];// iieo?aai oeacaoaeu
        fitsd->detNaxes(&naxes0, &naxes1);

        imgLabel->imgW = naxes0*imgLabel->scale;//au?eneyai ?acia?u n o?aoii ianooaaa
        imgLabel->imgH = naxes1*imgLabel->scale;

        qDebug() << "\nimgH\n";
        if (imgLabel->imgW>imgLabel->width())hscrBar->show();//else hscrBar->hide();// anee ?acia?u ecia?a?aiey n o?aoii oaeouaai ianooaaa aieuoa ?acia?a aea?aoa aey i?i?eniaee ecia?a?aiey "ia ye?aia" (imgLabel),
        if (imgLabel->imgH>imgLabel->height())vscrBar->show();//else vscrBar->hide();//oi iieacaou (show()) niioaaonoao?uo? iieino i?ie?ooee
        int limitX1 = imgLabel->width()/2;// ii?aaaeyai i?aaaeu ne?ieeeiaa aey iiein i?ie?ooee
        int limitX2 = imgLabel->imgW-imgLabel->width()/2;
        int limitY1 = imgLabel->height()/2;
        int limitY2 = imgLabel->imgH-imgLabel->height()/2;
        hscrBar->setRange(limitX1,limitX2);
        vscrBar->setRange(limitY1,limitY2);
        hscrBar->setSingleStep(imgLabel->width()/4);
        vscrBar->setSingleStep(imgLabel->height()/4);
        hscrBar->setPageStep(imgLabel->width()/4);
        vscrBar->setPageStep(imgLabel->height()/4);
        if (imgLabel->cX<limitX1) hscrBar->setValue(limitX1);// iaiyai iiei?aiey "aae?eia" iiein i?ie?ooee oae, ?oiau iiiaaaei ana ecia?a?aiea
        if (imgLabel->cX>limitX2) hscrBar->setValue(limitX2);
        if (imgLabel->cY<limitY1) vscrBar->setValue(limitY1);
        if (imgLabel->cY>limitY2) vscrBar->setValue(limitY2);
        imgLabel->cX = hscrBar->value();// ia?aaaai iiei?aiey "aae?eia" iiein i?ie?ooee, eae eii?aeiaou oaio?a i?yiioaieuieea ecia?a?aiey aey i?i?eniaee a imgLabel
        imgLabel->cY = vscrBar->value();
        qDebug() << "\ncY\n";
//		radeLineEdit->setText(mas_to_hms(grad_to_mas(fitsd->WCSdata[2]), " ",3)+" "+mas_to_damas(grad_to_mas(fitsd->WCSdata[3]), " ",2)+QString("%1 ").arg(mag_1, 4, 'f', 1, '0')+QString("%1").arg(mag_2, 4, 'f', 1, '0'));
        qDebug() << "\nradeLineEdit\n";
//		imgLabel->repaint();// aucuaaai i?i?eniaeo, ?oiau ioia?aceou aaaaaiiua eciaiaiey
    }
    else
    {
        hscrBar->hide();
        vscrBar->hide();
    }
    qDebug() << "\nrepaint\n";
    imgLabel->repaint();// aucuaaai i?i?eniaeo, ?oiau ioia?aceou aaaaaiiua eciaiaiey
};

void MainWindow::slotPropAct()
{
    dockPropWidget->show();
};

void MainWindow::slotFrameManagerAct()
{
    dockFMgrWidget->show();
};


void MainWindow::slotAngleChanged(double ang)
{
    fitsdata *curFits = imgLabel->getCurFits();
    if(curFits!=NULL)
    {
        setSettName();

    //instruments->rang = ang;
        curFits->instr->isEdt = 1;
        saveSettButton->setEnabled(curFits->instr->isEdt);
//	QMessageBox::information(this, "info", QString("%1").arg(ang*atan(1)*4/180.0));
        if (imgLabel->fitsdataList.count()>0)
        {
            //this->imgLabel->setRotAngle(grad_to_rad(ang));
    //		imgLabel->getMarksGrid(0, fovSpinBox->value(), 10, 12);
            fitsdata *fitsd = imgLabel->getCurFits();
            marksGrid *mGr = imgLabel->getCurMarks();
            mGr->detTan(fitsd->WCSdata, fitsd->imgArr->naxes);
            slotScrUpdate();
        }
    }

};

void MainWindow::slotTelescopeChanged(int index)
{
    qDebug() << "\nMainWindow::slotTelescopeChanged\n";
    fitsdata *curFits = imgLabel->getCurFits();
    if(curFits!=NULL)
    {
        qDebug() << QString("index: %1\n").arg(index);
        instrNum=index;
        if(index>=0)curFits->instr->getNumIns(index);
        setSettName();
    }
}
/*
void MainWindow::slotWheelEvent(QWheelEvent *event)
{
QMessageBox::information(0, "info", "info");
	if(event->delta()>0.0) slotIncrease();
	if(event->delta()<0.0) slotDecrease();
}
*/
//wheelEvent
void MainWindow::wheelEvent(QWheelEvent *event)
{
//QMessageBox::information(0, "info", "info");
    fitsdata *curFits = imgLabel->getCurFits();
    if(curFits!=NULL)
    {
        if (!mainFrame)
        {
    //    	imgLabel->cX = Xo*imgLabel->scale;
    //    	imgLabel->cY = Yo*imgLabel->scale;
            if (event->delta()>0.0) slotIncrease();
            if (event->delta()<0.0) slotDecrease();
        }
        else
        {
            if (rbMouse)rotAngSpinBox->setValue(curFits->instr->rang+0.20*(event->delta()/fabs(event->delta())));
            else if (ctrlB)
            {
                xpsSpinBox->setValue(grad_to_mas(curFits->instr->scx)+10*(event->delta()/fabs(event->delta())));
                ypsSpinBox->setValue(grad_to_mas(curFits->instr->scy)+10*(event->delta()/fabs(event->delta())));
            }
            /*		imgLabel->changeScales(10);
                            imgLabel->reMarks();
                            slotScrUpdate();*/
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button()==Qt::RightButton) rbMouse = 1;
    if (mainFrame&&(event->button()==Qt::LeftButton))
    {
        lbMouse = 1;
        imgLabel->setCursor(Qt::ClosedHandCursor);
        xM = event->x();
        yM = event->y();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button()==Qt::RightButton) rbMouse = 0;
    if (mainFrame&&(event->button()==Qt::LeftButton))
    {
        lbMouse = 0;
        imgLabel->setCursor(Qt::OpenHandCursor);
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
	fitsdata *fData = imgLabel->getCurFits();
    if (imgLabel->fitsdataList.count()>0)
    {
    	QSize sbar = menuBar()->size();
    	Xo = (imgLabel->cX-imgLabel->width()/2 + event->x() - imgLabel->x())/imgLabel->scale;//?????????? ????? ??????? ???? ?????????????? ? ???????????? ?????????? ????????? ????????? ???-?????, ??????? ???? ?????????? ?? ???????????, ? ????? ???????? ??????
		Yo = (imgLabel->cY-imgLabel->height()/2 + event->y() - imgLabel->y() - sbar.height())/imgLabel->scale;
//        qDebug() << "\n" << event->x() - imgLabel->x() << ":" <<  event->y() - imgLabel->y() - sbar.height() << "\n";
//        qDebug() << "\n" << Xo + (event->x() - imgLabel->x())/imgLabel->scale << ":" <<  Yo + (event->y() - imgLabel->y() - sbar.height())/imgLabel->scale << "\n";
		posStr.clear();
		posStr.append(QString("%1 : %2 : %3").arg(Xo).arg(Yo).arg(fData->getPixValue(Xo, Yo)));

		if(fData->WCSdata[12])
		{
			double ra, de;
                        getPixPosToDegWCS(&ra, &de, Xo, Yo, fData->WCSdata);
                        posStr.append("|" + deg_to_hms(ra, " ", 3)+ "|" + deg_to_damas(de, " ", 3));
		}
		
		slotStatBarUpdate();
//        statusBar()->showMessage(QString("%1 : %2").arg(Xo).arg(Yo));
    }

    if (mainFrame&&lbMouse&&(imgLabel->fitsdataList.count()>0))
    {
//        fitsdata *fData = imgLabel->fitsdataList.at(imgLabel->frameIndex);
//		fData->marksG->moveMarks(event->x() - xM, event->y() - yM);
//		imgLabel->dX += (event->x() - xM)/imgLabel->scale;
//		imgLabel->dY += (event->y() - yM)/imgLabel->scale;
        fData->marksG->moveMarks((event->x() - xM)/imgLabel->scale, (event->y() - yM)/imgLabel->scale);
        fData->marksG->detTan(imgLabel->fitsdataList[imgLabel->frameIndex]->WCSdata, imgLabel->fitsdataList[imgLabel->frameIndex]->imgArr->naxes);
        qDebug() << "\neventX " << event->x() << "\teventY " << event->y() << "\n";
        slotScrUpdate();
        xM = event->x();
        yM = event->y();
//		imgLabel->cX = imgLabel->cX;

    }

//	event->accept();

}

void MainWindow::imageNavigation(QKeyEvent *keyEvent)
{
//  QMessageBox::information(0,"debug","pressed",QMessageBox::Ok,0,0);
    //if (keyEvent-> modifiers () == Qt::ControlModifier)
    //{
    if (keyEvent->key() == Qt::Key_Less) slotPreviouse();
    if (keyEvent->key() == Qt::Key_Greater)	slotNext();
    if (keyEvent->key() == Qt::Key_Delete) slotDelete();
    if (keyEvent->key() == Qt::Key_Control) ctrlB = 1;
    if (keyEvent->key() == Qt::Key_F9) slotWCSButtonClicked();
    if (keyEvent->key() == Qt::Key_F12)
    {
        if (!mainFrame)moveCheckBox->setChecked(Qt::Checked);
        else moveCheckBox->setChecked(Qt::Unchecked);
    }
};

void MainWindow::slotKeyReleased(QKeyEvent *keyEvent)
{
//	QMessageBox::information(0, "info", "released");
    if (keyEvent->key() == Qt::Key_Control) ctrlB = 0;
}

void MainWindow::slotNewInsButtonClicked()
{
    nName->formWidget->show();
}

void MainWindow::slotNewInsExit()
{
//	QMessageBox::information(0, "info", "info");
    fitsdata *curFits = imgLabel->getCurFits();
    if(curFits!=NULL)
    {
        curFits->instr->addIns(nName->ui_newNameLine->displayText());
        saveSettButton->setEnabled(curFits->instr->isEdt);
        comboTel->addItem(nName->ui_newNameLine->displayText());
        setSettName();
    }
}

void MainWindow::slotSaveButtonClicked()
{
    fitsdata *curFits = imgLabel->getCurFits();
    if(curFits!=NULL)
    {
        curFits->instr->save();
        saveSettButton->setEnabled(curFits->instr->isEdt);
    }

//	comboTel->clear();
    /*	comboTel->addItems(instruments->namesList);
    	slotTelescopeChanged(instruments->curInst);

    	setSettName();*/
}

void MainWindow::slotRemInsButtonClicked()
{
    fitsdata *curFits = imgLabel->getCurFits();
    if(curFits!=NULL)
    {
        curFits->instr->delIns(comboTel->currentIndex());
        comboTel->removeItem(comboTel->currentIndex());
    }
}

void MainWindow::slotScxChanged(double scx)
{
    fitsdata *curFits = imgLabel->getCurFits();
    if(curFits!=NULL)
    {
        curFits->instr->scx = mas_to_grad(scx);
        curFits->instr->isEdt = 1;

        setSettName();
        //imgLabel->setScx(mas_to_grad(scx));
        //imgLabel->setScy(mas_to_grad(scx));
        imgLabel->reMarks();
        slotScrUpdate();

    }
}

void MainWindow::slotScyChanged(double scy)
{
    fitsdata *curFits = imgLabel->getCurFits();
    if(curFits!=NULL)
    {
        curFits->instr->scy = mas_to_grad(scy);
        curFits->instr->isEdt = 1;

        setSettName();
        //imgLabel->setScx(mas_to_grad(scy));
        //imgLabel->setScy(mas_to_grad(scy));
        imgLabel->reMarks();
        slotScrUpdate();

    }
}

void MainWindow::slotFovChanged(double fov)
{
    fitsdata *curFits = imgLabel->getCurFits();
    if(curFits!=NULL)
    {
        curFits->instr->fov = fov;
        curFits->instr->isEdt = 1;
        saveSettButton->setEnabled(curFits->instr->isEdt);


        //	QMessageBox::information(this, "info", QString("%1").arg(ang*atan(1)*4/180.0));
    }
}

void MainWindow::slotMarksAct()
{
    dockMarkWidget->show();
}
void MainWindow::clearCatMarks()
{
    fitsdata *fitsd = imgLabel->getCurFits();
    if(fitsd!=NULL)
    {
        fitsd->marksG->clearMarks();
        slotScrUpdate();
    }
}

void MainWindow::clearMarks()
{
    if (imgLabel->fitsdataList.count()>0)
    {
        switch(activMarksGrid->currentIndex())
        {
            case 0:
            {
                imgLabel->fitsdataList[imgLabel->frameIndex]->marksG->clearMarks();
                imgLabel->fitsdataList[imgLabel->frameIndex]->marksGIpix->clearMarks();
                imgLabel->fitsdataList[imgLabel->frameIndex]->refMarks->clearMarks();
                imgLabel->fitsdataList[imgLabel->frameIndex]->objMarks->clearMarks();
            }
            break;
            case 1:
            {
                imgLabel->fitsdataList[imgLabel->frameIndex]->marksGIpix->clearMarks();
            }
            break;
            case 2:
            {
                imgLabel->fitsdataList[imgLabel->frameIndex]->marksG->clearMarks();
            }
            break;
            case 3:
            {
                imgLabel->fitsdataList[imgLabel->frameIndex]->refMarks->clearMarks();
            }
            break;
            case 4:
            {
                imgLabel->fitsdataList[imgLabel->frameIndex]->objMarks->clearMarks();
            }
            break;
        }
        slotScrUpdate();
    }
}

void MainWindow::clearIpixMarks()
{
    if (imgLabel->fitsdataList.count()>0)
    {
        imgLabel->fitsdataList[imgLabel->frameIndex]->marksGIpix->clearMarks();
        slotScrUpdate();
    }
}


void MainWindow::slotApeChanged(double ape)
{
    imgLabel->setAperture(ape);
    aperBinSlider->setValue(floor(ape*0.15+0.5));
    slotScrUpdate();
}

void MainWindow::slotKeyAct()
{
/////////////////////////////////////////////////////////////
    if (imgLabel->fitsdataList.count()<1)
    {
        QMessageBox::warning(this, tr("warn"), tr("no files"));
        return;
    }
    dockKeyWidget->setWindowTitle(tr("fits header"));
    QFont f("Courier", 8, QFont::Bold);
    keyLabel->setFont(f);
    keyLabel->setTextFormat(Qt::PlainText);
    keyLabel->setText(imgLabel->fitsdataList[imgLabel->frameIndex]->fitsHeader);
    keyLabel->adjustSize();
    ////////////////////////////////////////////////////////////
    dockKeyWidget->show();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(imgLabel->fitsdataList[imgLabel->frameIndex]->fitsHeader);
};

void MainWindow::slotConAct()
{
    dockConWidget->show();
    conText->insertPlainText("test");
}


void MainWindow::slotMoveCheckBoxStateChanged(int state)
{
    if (state==Qt::Checked)
    {
        mainFrame = 1;
        imgLabel->setCursor(Qt::OpenHandCursor);
    }
    else
    {
        mainFrame = 0;
        imgLabel->setCursor(Qt::ArrowCursor);
    }
}

void MainWindow::slotPsfAct()
{
    dockPsfWidget->show();
}

void MainWindow::slotWCSButtonClicked()
{
    if(imgLabel->fitsdataList.count()>0)
    {
        fitsdata *fitsd = imgLabel->getCurFits();
        marksGrid *mGr = imgLabel->getCurMarks();
        marksGrid *mRef = fitsd->refMarks;
        if(!fitsd->is_empty)
        {
            fitsd->detPpix(mGr, fitsd->getApeSize(apeSpinBox->value()));
            slotMeasureMarksGrid(mGr);

            mRef->copy(mGr);
         //   mRef->detTan(&fitsd->WCSdata[0], &fitsd->naxes[0]);
            slotScrUpdate();
        }
  /*      int i, sz;
        sz = mGr->marks.size();
        for(i=0; i<sz; i++)
        {
            fitsd->refMarks->marks << mGr->marks.at(i);
            mGr->marks.removeAt(i);
            
        }*/
//        mGr->marks.removeAll();
//        marksGrid *maG = new marksGrid;
//        int resAuto = imgLabel->identAuto(maG);

//        mGr->marks.clear();
//        mGrI->marks.clear();
/*
        if(resAuto)
        {
            qDebug() << "\nautoident fail\n";
            QMessageBox::information(0, "resAuto", "autoident fail");
            return;
        }
        qDebug() << "\nwcsSpinBox\n";
*/

        //if(fitsd->detWCS(wcsSpinBox->value(), smaxSpinBox->value()))
        if(fitsd->detWCS1(wcsParams))
        {
            slotSetCurFrame(imgLabel->frameIndex);
            mGr->marks.clear();
    //            mGrI->marks.clear();
    //		slotMarks();
            fitsd->detTan();

            slotScrUpdate();
        }
        else QMessageBox::information(0, "detWCS error", QString("detWCS fail\nSx = %1\tSy = %2").arg(grad_to_mas(fitsd->Sx)).arg(grad_to_mas(fitsd->Sy)));
    }

}

        /*
void MainWindow::slotWCSButtonClicked()
{
    slotDropWCS();
    int i;
    qDebug() << "Begin\n";
    if (imgLabel->fitsdataList.count()<=0) return;
//	slotCenterMass();
    fitsdata *curF = imgLabel->getCurFits();
    marksGrid *marksG = imgLabel->getCurMarks();
    double mag, wcsMag1 = 6, wcsMag2 = 10;
    bool singulAR;
    double delta = orderSpinBox->value();
    int x,y,psft,NofV = 8,wcsN = wcsSpinBox->value();//wcsN - ieieiaeuiia ?enei caaca aey aienuaaiey
    int ape = imgLabel->aperture;
    STDATA stdata;
    double *pX, *pY, *wcsKsi, *wcsEta, *psfR, *wcsRA, *wcsDE, *wcsM;
    int pNum = 0;
    slotCenterMass();
    curF->detPpix(imgLabel->aperture, 0);
    int szP = marksG->marks.size();
    pX = new double[szP];
    pY = new double[szP];
    double *P = new double[szP];
    wcsKsi = new double[szP];
    wcsEta = new double[szP];
    wcsRA = new double[szP];
    wcsDE = new double[szP];
    wcsM = new double[szP];
    psfR = new double[NofV];
    // oeee ioai?a e ecia?aiey caaca.
    qDebug() << "Cikl\n";
    while ((pNum<wcsN)&&(wcsMag2<25))
    {
        for (i=0;i<szP;i++)// i?iaoai ecia?eou ana caacau, oaiaaioeaeuiua eii?aeiaou eioi?uo anou a iao?eoa imgF.p_tangpos
        {
            mag = marksG->marks[i]->mEkv[2];//MEFP(*imgF.p_tangpos,i,2);
            x = marksG->marks[i]->mTanImg[0];//objl[i].x();// yoi i?eia?iua ieenaeuiua eii?aeiaou caaca ec eaoaeiaa. Iie iieo?a?ony iinea caa?ocee iaoie ec eaoaiaa e ia?aiauaiey iaoie ai niaiaaaiey n ecia?a?aieyie caaca.
            y = marksG->marks[i]->mTanImg[1];//objl[i].y();
            if ((mag>wcsMag1)&&(mag<wcsMag2)&&(x>0)&&(y>0)&&(x<curF->imgArr->naxes[0])&&(y<curF->imgArr->naxes[1]))
            {//anee caacaa iiiaaaao a eaa? e aa aeane a i?aaaeao io wcsMag1 ai wcsMag2, oi iuoaainy ecia?eou caacao
                if (cMassRadioButton->isChecked())
                {
                    pX[pNum] = x;//-curF->imgArr->naxes[0]/2;// << psfR[5]-imgWidth/2;// yoi ieenaeuiay eii?aeiaoa X
                    pY[pNum] = y;//-curF->imgArr->naxes[1]/2;// << psfR[6]-imgHeight/2;// yoi ieenaeuiay eii?aeiaoa Y
                    wcsKsi[pNum] = marksG->marks[i]->mTan[0];// << MEFP(*imgF.p_tangpos,i,0);//Aaeuoa oaiaaioeaeuiua e yeaaoi?eaeuiua eii?aeiaou
                    wcsEta[pNum] = marksG->marks[i]->mTan[1];// << MEFP(*imgF.p_tangpos,i,1);
                    wcsRA[pNum] = marksG->marks[i]->mEkv[0];// << MEFP(*imgF.p_refcat,i,0);
                    wcsDE[pNum] = marksG->marks[i]->mEkv[1];// << MEFP(*imgF.p_refcat,i,1);
                    wcsM[pNum] = marksG->marks[i]->mEkv[2];
                    qDebug() << "\n" << marksG->marks[i]->mTan[0] << "\t" << marksG->marks[i]->mTan[1] << "\n";
                    pNum++;
                }
                else
                {
                    if (lorentzRadioButton->isChecked())
                    {
                        psft = 0;
                        StartConditionsL(marksG->marks[i]->mPpix, delta, P);
                    }//n?eoaai ioeaaia i?eaee?aiea
                    if (gaussianRadioButton->isChecked())
                    {
                        psft = 1;
                        NofV = 7;
                        StartConditionsG(marksG->marks[i]->mPpix, P);
                    }
                    if (moffatRadioButton->isChecked())
                    {
                        psft = 2;
                        StartConditionsM(marksG->marks[i]->mPpix, delta, P);
                    }
                    int *actv = new int[NofV];
                    for (int j=0;j<NofV;j++) actv[j] = 1;
//	            psfR = NLSLevenbergMarquardt(marksG->marks[i].mPpix, P, psft, actv, NofV, 10, 10, 1e-7, 1e-6);//aienuaaai i?ioeeu
//	            delete data.Buffer;
                    delete actv;

                    //O oaay ana ?oi eanaaony PSF io?ii caiaieou ia Oaio? Iann
                    if (psfR[0]!=-1)
                    {//anee aienaeny i?ioeeu, oiaaa aiaaaeyai aaiiua i caacaa
                        /*	               pX << psfR[5]-curF->imgArr->naxes[0]/2;// yoi ieenaeuiay eii?aeiaoa X
                        	               pY << psfR[6]-curF->imgArr->naxes[1]/2;// yoi ieenaeuiay eii?aeiaoa Y
                        	               wcsKsi << MEFP(*imgF.p_tangpos,i,0);//Aaeuoa oaiaaioeaeuiua e yeaaoi?eaeuiua eii?aeiaou
                        	               wcsEta << MEFP(*imgF.p_tangpos,i,1);
                        	               wcsRA << MEFP(*imgF.p_refcat,i,0);
                                               wcsDE << MEFP(*imgF.p_refcat,i,1);/
                    }
                }

            }
        }
//      QMessageBox::information(0, "inf", QString("%1").arg(pNum));
        if (pNum<wcsN)
        {//anee iinea oeeea for (int i=0;i<imgF.p_tangpos->size1();i++) caaca iaa?aeinu iaainoaoi?ii,
            // oaaee?ei aa?oiee i?aaae ii caacaiie aaee?eia, a ie?iee i?aaae i?e?aaiyai noa?iio aa?oiaio (?oiau ecaa?aou aoaeae)
            //e niiaa iiaoi?ei iiene caaca
            wcsMag1 = wcsMag2;
            wcsMag2 = wcsMag1+1;
        }
    }

    marksGrid *resGrid = new marksGrid;
    for (i = 0; i<pNum; i++)
    {
        resGrid->addEkvMark(wcsRA[i], wcsDE[i], wcsM[i]);
        resGrid->marks[i]->mTan[0] = wcsKsi[i];
        resGrid->marks[i]->mTan[1] = wcsEta[i];
        resGrid->marks[i]->mTanImg[0] = pX[i];
        resGrid->marks[i]->mTanImg[1] = pY[i];
    }
    curF->detWCS(wcsN, resGrid, smaxSpinBox->value());
    
    if (!curF->WCSdata[12])
    {
        statusBar()->showMessage("error in WCS calculation");
    }
    else
    {
//		mstr = QString( "%1" ).arg( curF->Sx,6,'f',1,QLatin1Char( ' ' ))+" ";
//		mstr = mstr+QString( "%1" ).arg( curF->Sy,6,'f',1,QLatin1Char( ' ' ));
		errStr.clear();
		errStr.append(QString( "%1" ).arg( curF->Sx*1000.0,6,'f',1,QLatin1Char( ' ' ))+" "+QString( "%1" ).arg( curF->Sy*1000.0,6,'f',1,QLatin1Char( ' ' )));
		slotStatBarUpdate();
//        statusBar()->showMessage(QString( "%1" ).arg( curF->Sx*1000.0,6,'f',1,QLatin1Char( ' ' ))+" "+QString( "%1" ).arg( curF->Sy*1000.0,6,'f',1,QLatin1Char( ' ' )));
    }
    slotSetCurFrame(imgLabel->frameIndex);
}
*/
void MainWindow::slotDropWCS()
{
    fitsdata *curF = imgLabel->getCurFits();

    curF->dropWCS();
    //curF->WCSdata[12]=0;

    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Drop WCS"));
    msgBox.setText(tr("Remove from file too?"));
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setEscapeButton(QMessageBox::No);


    if (msgBox.exec()==QMessageBox::Yes)
    {


        QString mstr(curF->fileName);
        QByteArray curr_fn = mstr.toAscii();
        char *ffname = curr_fn.data();
        fitsfile *fptR;
        int statuS = 0;//status
        fits_open_file(&fptR, ffname , READWRITE,&statuS);

        fits_delete_key(fptR, "CTYPE1", &statuS);
        statuS = 0;
        fits_delete_key(fptR, "CTYPE2", &statuS);
        statuS = 0;
        fits_delete_key(fptR, "CRPIX1", &statuS);
        statuS = 0;
        fits_delete_key(fptR, "CRPIX2", &statuS);
        statuS = 0;
        fits_delete_key(fptR, "CRVAL1", &statuS);
        statuS = 0;
        fits_delete_key(fptR, "CRVAL2", &statuS);
        statuS = 0;
        fits_delete_key(fptR, "CDELT1", &statuS);
        statuS = 0;
        fits_delete_key(fptR, "CDELT2", &statuS);
        statuS = 0;
        fits_delete_key(fptR, "CROTA1", &statuS);
        statuS = 0;
        fits_delete_key(fptR, "CROTA2", &statuS);
        statuS = 0;
        fits_delete_key(fptR, "CD1_1", &statuS);
        statuS = 0;
        fits_delete_key(fptR, "CD1_2", &statuS);
        statuS = 0;
        fits_delete_key(fptR, "CD2_1", &statuS);
        statuS = 0;
        fits_delete_key(fptR, "CD2_2", &statuS);
        statuS = 0;
        fits_delete_key(fptR, "WCSRFCAT", &statuS);
        statuS = 0;
        fits_delete_key(fptR, "CUNIT1", &statuS);
        statuS = 0;
        fits_delete_key(fptR, "CUNIT2", &statuS);
        statuS = 0;
        fits_delete_key(fptR, "WCSUWE1", &statuS);
        statuS = 0;
        fits_delete_key(fptR, "WCSUWE2", &statuS);
        statuS = 0;

        fits_close_file(fptR, &statuS);
    }
    updateWCS(curF->WCSdata);
}

void MainWindow::slotWriteWCS()
{
    fitsdata *curF = imgLabel->getCurFits();

    if (!curF->WCSdata[12]) return;
    QString mstr(curF->fileName);
    QByteArray curr_fn = mstr.toAscii();
    char *ffname = curr_fn.data();
    fitsfile *fptR;
    int statuS = 0;//status

    fits_open_file(&fptR, ffname , READWRITE,&statuS);
    mstr = "RA---TAN";
    QByteArray baStr1 = mstr.toAscii();
    char *chStr1 = baStr1.data();
    fits_update_key(fptR, TSTRING, "CTYPE1", chStr1, " ", &statuS);
    statuS = 0;
    mstr = "DEC--TAN";
    QByteArray baStr2 = mstr.toAscii();
    char *chStr2 = baStr2.data();
    fits_update_key(fptR, TSTRING, "CTYPE2", chStr2, " ", &statuS);
    statuS = 0;
    double prToFit = curF->WCSdata[0];
    fits_update_key(fptR, TDOUBLE, "CRPIX1", &prToFit, "ref pix 1", &statuS);
    statuS = 0;
    prToFit = curF->WCSdata[1];
    fits_update_key(fptR, TDOUBLE, "CRPIX2", &prToFit, "ref pix 2", &statuS);
    statuS = 0;
    prToFit = curF->WCSdata[2];
    fits_update_key(fptR, TDOUBLE, "CRVAL1", &prToFit, "RA at ref pix", &statuS);
    statuS = 0;
    prToFit = curF->WCSdata[3];
    fits_update_key(fptR, TDOUBLE, "CRVAL2", &prToFit, "DEC at ref pix", &statuS);
    statuS = 0;
    prToFit = curF->WCSdata[4];
    fits_update_key(fptR, TDOUBLE, "CDELT1", &prToFit, "scale1 [deg/pix]", &statuS);
    statuS = 0;
    prToFit = curF->WCSdata[5];
    fits_update_key(fptR, TDOUBLE, "CDELT2", &prToFit, "scale2 [deg/pix]", &statuS);
    statuS = 0;
    prToFit = curF->WCSdata[6];
    fits_update_key(fptR, TDOUBLE, "CROTA1", &prToFit, "rotation1 [deg]", &statuS);
    statuS = 0;
    prToFit = curF->WCSdata[7];
    fits_update_key(fptR, TDOUBLE, "CROTA2", &prToFit, "rotation2 [deg]", &statuS);
    statuS = 0;
    prToFit = curF->WCSdata[8];
    fits_update_key(fptR, TDOUBLE, "CD1_1", &prToFit, "WCS CD matrix", &statuS);
    statuS = 0;
    prToFit = curF->WCSdata[9];
    fits_update_key(fptR, TDOUBLE, "CD1_2", &prToFit, "WCS CD matrix", &statuS);
    statuS = 0;
    prToFit = curF->WCSdata[10];
    fits_update_key(fptR, TDOUBLE, "CD2_1", &prToFit, "WCS CD matrix", &statuS);
    statuS = 0;
    prToFit = curF->WCSdata[11];
    fits_update_key(fptR, TDOUBLE, "CD2_2", &prToFit, "WCS CD matrix", &statuS);
    statuS = 0;
    getCatName(&mstr, curF->marksG->ctype);
    QByteArray baStr3 = mstr.toAscii();
    char *chStr3 = baStr3.data();
    fits_update_key(fptR, TSTRING, "WCSRFCAT", chStr3, "wcs catalog", &statuS);
    statuS = 0;
    mstr = "deg";
    QByteArray baStr4 = mstr.toAscii();
    char *chStr4 = baStr4.data();
    fits_update_key(fptR, TSTRING, "CUNIT1", chStr4, "unit 1", &statuS);
    statuS = 0;
    QByteArray baStr5 = mstr.toAscii();
    char *chStr5 = baStr5.data();
    fits_update_key(fptR, TSTRING, "CUNIT2", chStr5, "unit 2", &statuS);
    statuS = 0;
    fits_update_key(fptR, TDOUBLE, "WCSUWE1", &curF->Sx, "unit weight error in mas", &statuS);
    statuS = 0;
    fits_update_key(fptR, TDOUBLE, "WCSUWE2", &curF->Sy, "unit weight error in mas", &statuS);
    statuS = 0;
    fits_close_file(fptR, &statuS);
}

void MainWindow::slotLevelBinChanged(int lev)
{
    levelBinLabel->setText(QString("%1").arg(lev));
}

void MainWindow::slotAperBinChanged(int lev)
{
    aperBinLabel->setText(QString("%1").arg(lev));
}

void MainWindow::slotGridStatPanel()
{
 //   gridStat->show();
 //   dockGridStatWidget->show();
}

int MainWindow::slotBinFilter()
{
//	QMessageBox::information(0, "Bin Filter", QString("Start"));\
    qDebug() << "\n" << "Bin filter" << "\n";
    if (imgLabel->fitsdataList.count()<=0) return 1;

    fitsdata *fcurr = imgLabel->getCurFits();
    if(fcurr!=NULL)
    {
        marksGrid *mGr = imgLabel->getCurMarksIpix();
        //img2d *binImg = new img2d(USHORT_TYPE, fcurr->imgArr->naxes[0], fcurr->imgArr->naxes[1]);

        long i, sz;
        sz = fcurr->imgArr->size;
        long levl = levelBinSlider->value();
        long levlMax = levelBinSlider->maximum();

        int apSize = fcurr->getApeSize(aperBinSlider->value());

        qDebug() << QString("binFilter: levlMax: %1\tapSize: %2\tsz: %3\n").arg(levlMax).arg(apSize).arg(sz);

    //	QMessageBox::information(0, "Bin Filter", QString("Size %1").arg(sz));

        double *val = new double[sz];
        double ppx;
        long j;
        unsigned short pp1 = 0;
        long k, l;
        long p, q;
        imgAperture *imA = new imgAperture(apSize*apSize);

    //	fcurr->imgArr->get(val, 0, 0, fcurr->imgArr->naxes[0], fcurr->imgArr->naxes[1]);
    //	binImg->set(val, 0, 0, fcurr->imgArr->naxes[0], fcurr->imgArr->naxes[1]);
        int n=0;
        for (i=0; i<sz; i++)
        {
           // qDebug() << QString("i: %1\n").arg(i);
            ppx = fcurr->imgArr->getPos(i);
           // qDebug() << QString("ppx: %1\n").arg(ppx);
            if ((ppx>=levl)&&(ppx<=levlMax))
            {
                val[i] = 1.0;
                n++;
                //qDebug() << "\nppx:" << ppx << "\n";
            }
            else  val[i] = 0.0;
        }
        qDebug() << "\nnlevl:" << n << "\n";
        qDebug() << "\nFIND Ape:\n";
        int pos;

        for (i=0; i<sz; i++)
        {
            //qDebug() << QString("val[%1]: %2\n").arg(i).arg(val[i]);

            if (val[i]>0)
            {
                l = i/fcurr->imgArr->naxes[0];
                k = i - l*fcurr->imgArr->naxes[0];

           //                 qDebug() << "\n>0:" << k << ":" << l<< ":" << val[i] << "\n";
                ApeData(imA, val, k, l, apSize, fcurr->imgArr->naxes[0], fcurr->imgArr->naxes[1]);
          //                  qDebug() << "\n" << k << ":" << l<< ":" << val[i] << "\n";
                if (imA->minADC>0.0)
                {
            //                        qDebug() << "\nape\t" << k << ":" << l<< ":" << imA->minADC << "\n";
                    mGr->addImgMark(k, l);
                    //qDebug() << "mark added\n";

                    for (p=k-apSize; p<=k+apSize; p++)
                    {
                        for (q=l-apSize; q<=l+apSize; q++)
                            if (sqrt((p-k)*(p-k) + (q-l)*(q-l))<apSize)
                            {
                                pos = q*fcurr->imgArr->naxes[0]+p;
    //							qDebug() << "\n\tpos:" << pos << "\n";
                                if ((pos>=0)&&(pos<sz))val[pos] = 0;
                            }
                    }
                }
                //delete [] imA->buffer;
            }
        }
        delete imA;

        if(mGr->marks.size()>2500) return 1;
    //	QMessageBox::information(0, "Bin Filter", QString("End %1").arg(mGr->marks.size()));
    //    slotMarksUnify1(imgLabel->aperture);
     //     slotMarksUnify1(apSize);
     //       slotMarksUnify1(imgLabel->aperture/3.0);
    //        slotMarksUnify1(imgLabel->aperture/2.0);
           // slotMarksUnify1(imgLabel->aperture*2);

        slotMarksUnify1(apSize*2);
        slotCenterMass(mGr);
        slotMarksUnify1(apSize*2);
        slotCenterMass(mGr);
        slotMarksUnify1(apSize*3);
        slotCenterMass(mGr);
        slotMarksUnify1(apSize*4);
        slotCenterMass(mGr);
        slotScrUpdate();
//	QMessageBox::information(0, "Bin Filter", QString("slotMarksUnify %1").arg(mGr->marks.size()));
    /*	slotMarksUnify(apSize*4);
    	QMessageBox::information(0, "Bin Filter", QString("slotMarksUnify %1").arg(mGr->marks.size()));*/
        return 0;
    }
    return 1;
}

void MainWindow::slotFilters()
{
    dockFiltersWidget->show();
}
void MainWindow::slotWcsWidget()
{
    dockWcsWidget->show();
}

void MainWindow::updateWCS(double *wcsV)
{
    if(wcsV!=NULL)
    {
        if(wcsV[12])
        {
            //ct1ValLabel
            for(int i=0; i<12; i++)
            {
                qDebug() << QString("wcs[%1]: %2\n").arg(i).arg(wcsV[i], 15, 'e');
                wcsList[i]->setText(QString("%1").arg(wcsV[i], 15, 'g'));
            }
            //sxValLabel->setText(QString("%1").arg(grad_to_mas(Sx)));
            //syValLabel->setText(QString("%1").arg(grad_to_mas(Sy)));
        }
        else
        {
            //wcsList[0]->setText("N/A");
            //wcsList[1]->setText("N/A");
            wcsList[0]->setText(QString("%1").arg(wcsV[0]));
            wcsList[1]->setText(QString("%1").arg(wcsV[1]));
            wcsList[2]->setText(QString("%1").arg(wcsV[2]));
            wcsList[3]->setText(QString("%1").arg(wcsV[3]));
            for(int i=4; i<12; i++) wcsList[i]->setText("N/A");
            //sxValLabel->setText(QString("N/A"));
            //syValLabel->setText(QString("N/A"));
        }
        radeLineEdit->setText(mas_to_hms(grad_to_mas(wcsV[2]), " ",3)+" "+mas_to_damas(grad_to_mas(wcsV[3]), " ",2)+QString("%1 ").arg(mag_1, 4, 'f', 1, '0')+QString("%1").arg(mag_2, 4, 'f', 1, '0'));
    }
    else
    {
         for(int i=0; i<12; i++) wcsList[i]->setText("N/A");
          //sxValLabel->setText(QString("N/A"));
          //syValLabel->setText(QString("N/A"));
    }
}

void MainWindow::slotLoadWCS()
{
    QString wcsFileName = QFileDialog::getOpenFileName(
                                this,
                                tr("Load WCS File"),
                                currDir,
                                "All Files (*.wcs)");
    if(wcsFileName=="") return;
    fitsdata *fcurr = imgLabel->getCurFits();

    if (fcurr!=NULL)
    {
        if(!fcurr->loadWCSFile(wcsFileName)) updateWCS(fcurr->WCSdata);
    }
}

void MainWindow::slotSaveWCS()
{
    fitsdata *fcurr = imgLabel->getCurFits();
    if (fcurr!=NULL)
    {
        if(fcurr->is_empty)
        {
            QString wcsFileName = QFileDialog::getSaveFileName(
                                    this,
                                    tr("Save marks"),
                                    //currDir,
                                    QString("%1.wcs").arg(fcurr->fileName),
                                    tr("Marks List (*.wcs)"));

            qDebug() << "\nwcsFileName " << wcsFileName << "\n";
            if(!fcurr->saveWCSFile(wcsFileName)) updateWCS(fcurr->WCSdata);
        }
        else fcurr->saveWCS();
    }
}

void MainWindow::slotMarksUnify(int dist)	//iauaaeiaiea aeeceeo iaoie oaio?ii iann
{
    fitsdata *fcurr = imgLabel->getCurFits();
    marksGrid *mGr = imgLabel->getCurMarksIpix();

    int i, j;
    int sz = mGr->marks.size();

    int x0, y0, x1, y1, xc, yc;
    qDebug() << "\nmarks num\t" << sz << "\n";
    int summ1, summ2, num1;

    for (i=0; i<mGr->marks.size(); i++)
    {
        qDebug() << "\nuni\t" << i << ":";
        x0 = mGr->marks[i]->mTanImg[0];
        y0 = mGr->marks[i]->mTanImg[1];
        summ1 = x0;
        summ2 = y0;
        num1 = 1;
        sz = mGr->marks.size();
        for (j=sz-1; j>i; j--)
        {
//			if(i==j) continue;
            //qDebug() << j << ":" << mGr->marks.size() << "\n";

            x1 = mGr->marks[j]->mTanImg[0];
            y1 = mGr->marks[j]->mTanImg[1];

            if (sqrt((x0 - x1)*(x0 - x1) + (y0 - y1)*(y0 - y1))<=(dist*2))
            {
                summ1 += x1;
                summ2 += y1;
                num1++;
//				qDebug() << j << "\n";
//				xc = (x1 + x0)/2;
//				yc = (y1 + y0)/2;
//				mGr->marks[j]->mTanImg[0] = xc;
//				mGr->marks[j]->mTanImg[1] = yc;
                mGr->remMark(j);
//				slotScrUpdate();
//				QMessageBox::information(0, "Bin Filter", QString("%1").arg(mGr->marks.size()));
//				break;
            }
            xc = summ1/num1;
            yc = summ2/num1;
            mGr->marks[i]->mTanImg[0] = xc;
            mGr->marks[i]->mTanImg[1] = yc;
        }
    }
}

void MainWindow::slotMarksUnify1(int dist)	//iauaaeiaiea aeeceeo iaoie oaio?ii iann
{
    fitsdata *fcurr = imgLabel->getCurFits();
    marksGrid *mGr = imgLabel->getCurMarksIpix();

    marksP* mTemp;
    QList <marksP*> mktList;

    int i, j, k;
    int sz = mGr->marks.size();
    int sz1;

    int x0, y0, x1, y1, xc, yc;
    qDebug() << "\nmarks num\t" << sz << "\n";
    int summ1, summ2, num1;
//    int p0, p1;

    qDebug() << "\ndist Aperture: " << dist << "\n";

    for (i=0; i<mGr->marks.size(); i++)
    {
        mktList.clear();
        qDebug() << "\nuni\t" << i << ":";
        x0 = mGr->marks[i]->mTanImg[0];
        y0 = mGr->marks[i]->mTanImg[1];

        mTemp = new marksP;
        mTemp->mTanImg[0] = x0;
        mTemp->mTanImg[1] = y0;
        mktList << mTemp;
//        summ1 = x0;
//        summ2 = y0;
//        num1 = 1;

        k=-1;
        do
        {
            k++;
//            qDebug() << "k= " << k << "\n";
            x0 = mktList[k]->mTanImg[0];
            y0 = mktList[k]->mTanImg[1];
            sz = mGr->marks.size();
            for (j=sz-1; j>i; j--)
            {
                x1 = mGr->marks[j]->mTanImg[0];
                y1 = mGr->marks[j]->mTanImg[1];

                if(sqrt((x0 - x1)*(x0 - x1) + (y0 - y1)*(y0 - y1))<=dist)
                {
                    mTemp = new marksP;
                    mTemp->mTanImg[0] = x1;
                    mTemp->mTanImg[1] = y1;
                    mktList << mTemp;
                    mGr->remMark(j);
                    break;
                }
            }

            sz1 = mktList.size();
        }while(k<sz1-1);

        summ1 = summ2 = 0.0;
        num1 = mktList.size();
        for(k=0; k<num1; k++)
        {
            summ1 += mktList[k]->mTanImg[0];
            summ2 += mktList[k]->mTanImg[1];
        }
        xc = summ1/num1;
        yc = summ2/num1;
        mGr->marks[i]->mTanImg[0] = xc;
        mGr->marks[i]->mTanImg[1] = yc;

 //       slotScrUpdate();
//        QMessageBox::information(0, "Bin Filter", QString("%1").arg(mGr->marks.size()));
    }
}

void MainWindow::slotSaveMarksButtonClicked() //nio?aiyai iaoee a oaee
{
    QString marksFileName = QFileDialog::getSaveFileName(
                                this,
                                tr("Save marks"),
                                currDir,
                                tr("Marks List (*.*)"));

    qDebug() << "\nmarksFileName " << marksFileName << "\n";
    fitsdata *fcurr = imgLabel->getCurFits();
    marksGrid *mGr = imgLabel->getCurMarksIpix();
    QString mSep = settings->value("marks/marksFileSep").toString();
    QString mColumn = settings->value("marks/marksLoadFileColumn").toString();
    if (mGr!=NULL)
    {
        fcurr->detPpix(mGr, imgLabel->aperture);
        mGr->saveTanImg(marksFileName, mSep, mColumn);
    }
}

void MainWindow::slotLoadMarksButtonClicked()
{
    long max[2];
    QString marksFileName = QFileDialog::getOpenFileName(
                                this,
                                tr("Load Marks"),
                                currDir,
                                "All Files (*.*)");
    fitsdata *fcurr = imgLabel->getCurFits();
    marksGrid *mGr = imgLabel->getCurMarksIpix();
    QString mSep = settings->value("marks/marksFileSep").toString();
    QString mColumn = settings->value("marks/marksLoadFileColumn").toString();
    if (mGr!=NULL)
    {
        mGr->loadTanImg(marksFileName, mSep, mColumn);
        if(fcurr->is_empty)
        {
            if(isMove2corner) mGr->moveToCorner(fcurr->naxes);
            fcurr->fileName = QString(marksFileName);
            if(!fcurr->WCSdata[12])if(!fcurr->loadWCSFile(QString("%1.wcs").arg(marksFileName))) updateWCS(fcurr->WCSdata);
        }
        fcurr->detNaxes();
        slotSetCurFrame(imgLabel->frameIndex);
    }
    setCurDirFile(marksFileName);

    slotScrUpdate();
}

void MainWindow::slotMoveMarksToCorner()
{
    fitsdata *fcurr = imgLabel->getCurFits();
    marksGrid *mGr = imgLabel->getCurMarksIpix();

    if(fcurr!=NULL&&mGr!=NULL)
    {
        mGr->moveToCorner(fcurr->naxes);
    }
}

void MainWindow::setCurDirFile(QString fName)
{
    QFileInfo fi(fName);
    currDir = fi.absolutePath();
    slotUpdateCurDir();
}

void MainWindow::slotCurrDirButtonClicked()
{
    currDir = QFileDialog::getExistingDirectory(this, tr("Set Current Folder"),
              currDir,
              QFileDialog::ShowDirsOnly
              | QFileDialog::DontResolveSymlinks);
    slotUpdateCurDir();

}

void MainWindow::slotUpdateCurDir()
{
    currDirLabel->setText("Current folder = "+currDir);
    //stimproSett->beginGroup("general");
    stimproSett->setValue("general/curr_dir", currDir);
    //stimproSett->endGroup();
    //stimproSett->sync();
}

/*
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

void MainWindow::slotUSNOBDirButtonClicked()
{
imgF.usnob_path = QFileDialog::getExistingDirectory(this, tr("Set USNOB Folder"),
                                                 currDir,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
 usnobDirLabel->setText("usnob folder = "+imgF.usnob_path);
 settings->beginGroup("catalogs");
 settings->setValue("usnob_path", imgF.usnob_path);
 settings->endGroup();
}
*/
void MainWindow::slotLogDirButtonClicked()
{
    logDir = QFileDialog::getExistingDirectory(this, tr("Set LOG Folder"),
             currDir,
             QFileDialog::ShowDirsOnly
             | QFileDialog::DontResolveSymlinks);
    logDirLabel->setText("log folder = "+logDir);
    settings->beginGroup("general");
    settings->setValue("log_path", logDir);
    settings->endGroup();
}

void MainWindow::slotFitsHeaderPanel()
{
    dockFitsHeaderWidget->show();
}

void MainWindow::slotFromHeader()
{
    slotGetRaDefromHeader();
    slotGetMJDfromHeader();
    fitsdata *fitsd = imgLabel->getCurFits();
    if(fitsd!=NULL)
    {
        if(fitsd->marksGIpix->marks.size()==0) slotLoadMarksButtonClicked();
    }
}

void MainWindow::slotGetRaDefromHeader()
{

    QString tStr = raLE->text() + " " + deLE->text() + " " + QString("%1 ").arg(mag_1, 4, 'f', 1, '0')+QString("%1").arg(mag_2, 4, 'f', 1, '0');
    qDebug() << "\ntStr: " << tStr << "\n";
    radeLineEdit->setText(tStr);
}

void MainWindow::slotGetMJDfromHeader()
{
    int i, cPos;
    QString oName;
    double uTimeJD, uTimeMJD;
    double d_day, eJD;
    double jd0, sJD, utc;

    fitsdata *curF = imgLabel->getCurFits();
    if (curF!=NULL)
    {
//		curF->MJD = getMJDfromStrFTN(dateObsLE->text(), QString(expLE->text()).toDouble());

        getFromHeaderDlg *headSetup = new getFromHeaderDlg;
        headSetup->ui_isConvertToUTC->setCheckState(Qt::Checked);

     //   int sz = obsList->nstr;

     /*   for(i=0; i<sz; i++)
        {
            obsList->getobsynum(i);
            headSetup->ui_obsList->insertItem(i, QString("%1\t%2").arg(obsList->record->num).arg(obsList->record->name));
        }
        */
        headSetup->ui_obsName->setText(obsCode->text());

        /*cPos = obsList->getobsynumO_alt(obsCode->text().toAscii().data());
        if(cPos==-1) cPos=0;
        qDebug() << QString("cPos %1\n").arg(cPos);
        headSetup->ui_obsList->setCurrentIndex(cPos);*/
   /*
        for(int j=0; j<sz; j++)
    {
        qDebug() << QString("i:%1\t").arg(j);
        headSetup-
        expNumSel->insertItem(j, QString("%1").arg(j+1));
        qDebug() << QString("expList.at(i)=%1\t").arg(expList.at(j)->expStr);
        time += getMJDfromStrFTN(expList.at(j)->expStr, 0);
        qDebug() << QString("exposList.at(i)= %1\n").arg(expList.at(j)->expVal);
        expTime += expList.at(j)->expVal;
    }
*/
        int sz = expNumSel->count();
        qDebug() << QString("expList size= %1\n").arg(sz);



        for(int j=0; j<sz; j++)
        {
            qDebug() << QString("i:%1\t").arg(j);
            headSetup->ui_expList->insertItem(j, expNumSel->itemText(j));
        }
//        headSetup->ui_expList->insertItem(sz-1, QString("Mean"));
        headSetup->ui_expList->setCurrentIndex(sz-1);

        //if(!headSetup->exec()) return;
        curF->setExposure(headSetup->ui_expList->currentIndex());

        curF->MJD += uTimeLE->text().trimmed().toDouble()/86400.0;
/*
        if(obsList->getobsynumO(headSetup->ui_obsName->text().toAscii().data())) return;

        curF->setSpecTime((int)(headSetup->ui_isConvertToUTC->checkState()==Qt::Checked), (int)(headSetup->ui_isSpec->checkState()==Qt::Checked), obsList->record->Long);*/

    /*
        //curF->MJD = getMJDfromStrFTN(curF->expList.at(headSetup->ui_expList->currentIndex())->expStr, 0);
        jd0 = UT0JD(mjd2jd(curF->MJD), &sJD);
        utc = sJD;
        if(headSetup->ui_isConvertToUTC->checkState()==Qt::Checked)
        {
            qDebug() << QString("obsNum %1\n").arg(headSetup->ui_obsName->text());
            if(obsList->getobsynumO(headSetup->ui_obsName->text().toAscii().data())) return;


            sJD2UT1(jd0, sJD, obsList->record->Long, &utc);
            uTimeJD = jd0 + utc;
            //s2UTC(mjd2jd(curF->MJD), obsList->record->Long, &uTimeJD);
            qDebug() << QString("jd= %1\tlong=%2\tJD=%3\n").arg(mjd2jd(getMJDfromStrFTN(dateObsLE->text(), 0))).arg(obsList->record->Long).arg(uTimeJD);
            curF->MJD = uTimeMJD = jd2mjd(uTimeJD);


        }
        if(headSetup->ui_isSpec->checkState()==Qt::Checked)
        {
            d_day=curF->MJD*2.0;
            d_day-=chet(d_day);
            eJD = ((double)(int)d_day)/2.0;
            d_day = (curF->MJD - eJD) - 0.5;
            qDebug() << QString("MJD= %1\teJD = %2\td_day = %3\n").arg(curF->MJD).arg(eJD).arg(d_day);
            if(d_day>0.0&&d_day<0.2917)
            {
                //curF->MJD += 1.0;
                jd0+=1.0;
                //jd0 = UT0JD(mjd2jd(curF->MJD), &sJD);
                sJD2UT1(jd0, sJD, obsList->record->Long, &utc);
                //curF->MJD = jd2mjd(jd0 + utc);
            }


        }
        curF->MJD = jd2mjd(jd0 + utc);
        qDebug() << QString("uTime: %1\n").arg(uTimeLE->text().trimmed().toDouble());
        curF->MJD += uTimeLE->text().trimmed().toDouble()/86400.0;
        //curF->exptime = expLE->text().toDouble();*/
        timeLineEdit->setText(QString(getStrFromDATEOBS(getDATEOBSfromMJD(curF->MJD), " ", 2, 4)));
        expLineEdit->setText(expLE->text());
//		qDebug() << "\nMJD\t" << curF->MJD << "\n";
    }
}

void MainWindow::slotRaDeChanged(QString str)
{
    QStringList operands = str.split(" ");
    double ra_oc = hms_to_mas(operands[0] +" "+ operands[1] +" "+ operands[2]," ");
    double de_oc = damas_to_mas(operands[3] +" "+ operands[4] +" "+ operands[5]," ");
    QString str0 = operands[6];
    mag_1 = str0.toDouble();
    str0 = operands[7];
    mag_2 = str0.toDouble();
    fitsdata *fcurr = imgLabel->getCurFits();
    if (fcurr!=NULL)
    {
        fcurr->WCSdata[2] = mas_to_grad(ra_oc);
        fcurr->WCSdata[3] = mas_to_grad(de_oc);
    }

//	double hms_to_mas(QString ra, QString spl_symb);
//	double damas_to_mas(QString de, QString spl_symb);
}

void MainWindow::slotTimeChanged(QString)
{}

void MainWindow::slotFlipMarksH()
{
    marksGrid *mgr = imgLabel->getCurMarksIpix();
    mgr->flipH();
}

void MainWindow::slotFlipMarksV()
{
    marksGrid *mgr = imgLabel->getCurMarksIpix();
    mgr->flipV();
}

int MainWindow::slotFindStars()
{
    marksGrid *mgr = imgLabel->getCurMarksIpix();
    int snum = 0;
    int starg = starTargetSpinBox->value();
    int levmin, levcur, levstep, lev0;
    levmin = levelBinSlider->minimum();
    lev0 = levcur = levelBinSlider->value();
    levstep = (levcur-levmin)*0.1;
    int stepNum = 0;
    int stepMax = 10;
    do
    {
        if(stepNum>stepMax) break;
        levelBinSlider->setValue(levcur);
        mgr->marks.clear();
        if(slotBinFilter()) return 1;
        levcur -= levstep;
        snum = mgr->marks.size();
        if(snum>1000) return 1;
        stepNum++;
    }
    while(snum<starg);
    levelBinSlider->setValue(lev0);
    slotScrUpdate();
    return 0;
}

void MainWindow::slotStatBarUpdate()
{
	statusBar()->showMessage(posStr + "|" + errStr);
}

void MainWindow::slotShowHist()
{
if (dockHist->isHidden()) 
                    {
                      dockHist->show();
                    }//end if

}

void MainWindow::slotUpdateHistButtonClicked()
{
	
	updateHist();
}

void MainWindow::updateHist()
{
	fitsdata *fitsd = imgLabel->getCurFits();
	if(fitsd!=NULL)
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
		 long hsize = fitsd->hsize;//fitsd->maxv - fitsd->minv + 1;// imgF.ADCmax - imgF.ADCmin + 1;
		 int x,y;
		 double maxH = log(fitsd->maxH);
		 double lnH;
		 pen.setColor(Qt::white);
		 paint.setPen(pen);
		 for(long i = 0; i<hsize; i++)
		 {
	        x = 5+(int)((wh-10)*(double)i/hsize);
			if (fitsd->hD[i]!=0) 
			{
                            lnH = log(fitsd->hD[i]);
                            y = (hh-5) - (int)((hh-10)*(lnH/maxH));
			}
			else y = hh-5;
			
	        paint.drawLine(x,hh-5,x,y);		
		 }
		 //
		 pen.setColor(Qt::red);
		 paint.setPen(pen);
		 paint.drawLine(fitsd->minp,hh-5,fitsd->minp,5);
		 paint.drawLine(fitsd->maxp,hh-5,fitsd->maxp,5);
		 paint.end();
		 //
		 histLabel->clear();
		 
	     histLabel->setPicture(picture);
     	histLabel->adjustSize();
    }
};

void MainWindow::slotMeasureCatMarkedButtonClicked()
{
   fitsdata *fitsd = imgLabel->getCurFits();
   if(fitsd!=NULL)
   {
       fitsd->copyImgGrid(fitsd->marksG, fitsd->marksGIpix);
       slotScrUpdate();
       slotMeasureMarksGrid(fitsd->marksGIpix);
       slotScrUpdate();
   }
}

void MainWindow::slotMeasureIPixMarkedButtonClicked()
{
   fitsdata *fitsd = imgLabel->getCurFits();
   if(fitsd!=NULL)
   {
       slotMeasureMarksGrid(fitsd->marksGIpix);
       slotScrUpdate();
   }
}

void MainWindow::slotMeasureMarkedButtonClicked()
{
   fitsdata *fitsd = imgLabel->getCurFits();
   if(fitsd!=NULL)
   {
       switch(activMarksGrid->currentIndex())
       {
           case 0:
           slotMeasureMarksGrid(fitsd->marksG);
           slotMeasureMarksGrid(fitsd->marksGIpix);
           break;
           case 1:
           slotMeasureMarksGrid(fitsd->marksGIpix);
           break;
           case 2:
           slotMeasureMarksGrid(fitsd->marksG);
           break;
           case 3:
           slotMeasureMarksGrid(fitsd->refMarks);
           break;
           case 4:
           slotMeasureMarksGrid(fitsd->objMarks);
           break;
       }
       slotScrUpdate();
   }
}
//model - модель PSF (0 - профиль Лоренца, 1 - профиль Моффата, 2 - профиль Гаусса со степенью)

void MainWindow::slotMeasureMarksGrid(marksGrid *mgr)
{
    qDebug() << "slotMeasureMarkedButtonClicked\n";
    measureParam mPar;
    mPar.model = 3;//settings->value("psf/model").toInt();//PSF model: 0 - Lorentz PSF, 1 - Gauss PSF, 2 - Moffat PSF
    if(lorentzRadioButton->isChecked()) mPar.model = 0;
    if(gaussianRadioButton->isChecked()) mPar.model = 1;
    if(moffatRadioButton->isChecked()) mPar.model = 2;
    int i, t;


fitsdata *fitsd = imgLabel->getCurFits();

        mPar.nofit = inSpinBox->value();
        mPar.delta = orderSpinBox->value();
        mPar.ringradius = imgLabel->aperture;
        mPar.ringwidth = 6;
        mPar.lb = 1;
        mPar.sg = 1;
        mPar.aperture = imgLabel->aperture;


        fitsd->measureMarksGrid(mgr, mPar);

        slotScrUpdate();
}

void MainWindow::slotDetRef()
{

}

void MainWindow::slotGetObjects()
{
    if(getEphemerides()) return;
    //slotFindObjImg();
}

int MainWindow::getEphemerides()
{    
    findObjectDlg *foDlg = new findObjectDlg;
    QProcess outerProcess;
    QStringList outerArguments;
    int i, sz, num;
    QString oName;

    long naxes[2];

    fitsdata *fitsd = imgLabel->getCurFits();

    //fitsd->ast_eph_prog = ast_eph_prog;
    //fitsd->ast_eph_prog_folder =  ast_eph_prog_folder;


    QString objStr, sMjd;

    //sMjd = QString("%1").arg(getMJDfromStrFTN(timeLineEdit->text(), 0), 11, 'f',7);
    foDlg->ui_isUseExpNum->setChecked(0);
    if(fitsd->is_empty)
    {
        desc2NumName(objLE->text(), &num, &oName);
        qDebug() << QString("num: %1").arg(num);

        foDlg->ui_astNameLE->setText(oName);
        foDlg->ui_astNumLE->setText(QString("%1").arg(num));
        foDlg->ui_astNameType->setChecked(true);
        foDlg->ui_astNumType->setChecked(false);
        foDlg->ui_lspmType->setChecked(false);
        foDlg->ui_astNameLE->setEnabled(1);
    }
    else
    {
        objStr = fitsd->fileName.section("_", 0, 0);
        objStr = objStr.section("/", -1, -1);
        if((objStr.lastIndexOf("a")==objStr.size()-1)||(objStr.lastIndexOf("b")==objStr.size()-1)||(objStr.lastIndexOf("c")==objStr.size()-1)||(objStr.lastIndexOf("d")==objStr.size()-1)) objStr.chop(1);

        if((objStr.indexOf("J")==0)&&((objStr.indexOf("+")!=-1)||(objStr.indexOf("-")!=-1)))
        {
            foDlg->ui_lspmName->setText(objStr);
            //foDlg->ui_astNumLE->setText(QString("%1").arg(num));
            foDlg->ui_astNameType->setChecked(false);
            foDlg->ui_astNumType->setChecked(false);
            foDlg->ui_lspmType->setChecked(true);
            foDlg->ui_lspmName->setEnabled(1);
        }
        else
        {
            foDlg->ui_astNumLE->setText(objStr);
            foDlg->ui_astNameType->setChecked(false);
            foDlg->ui_astNumType->setChecked(true);
            foDlg->ui_lspmType->setChecked(false);
            foDlg->ui_astNumLE->setEnabled(1);
        }

    }

    int res = foDlg->exec();

    qDebug() << QString("res: %1\n").arg(res);

    if(res==QDialog::Accepted)
    {
    imgLabel->setInstProp(mas_to_grad(xpsSpinBox->value()), mas_to_grad(ypsSpinBox->value()), fovSpinBox->value(), grad_to_rad(rotAngSpinBox->value()));
        if(foDlg->ui_astNameType->isChecked())
        {
            fitsd->getMpephName(foDlg->ui_astNameLE->text(), ast_eph_prog, ast_eph_prog_folder, magObj0,  magObj1);
        }
        if(foDlg->ui_astNumType->isChecked())
        {
            fitsd->getMpephNum(foDlg->ui_astNumLE->text(), ast_eph_prog, ast_eph_prog_folder, magObj0,  magObj1);
        }
        if(foDlg->ui_lspmType->isChecked())
        {
            if(!(starCatList.size()<4)) fitsd->getLspmObject(foDlg->ui_lspmName->text(), starCatList[3]->exeName, starCatList[3]->exePath, starCatList[3]->catPath);
        }
        if(foDlg->ui_lspmFindType->isChecked())
        {
            if(!(starCatList.size()<5)) fitsd->findLspmCat(starCatList[4]->exeName, starCatList[4]->exePath, starCatList[4]->catPath, fovSpinBox->value()/2.0, magObj0,  magObj1);
        }
        if(foDlg->ui_skybotFindType->isChecked())
        {
        qDebug() << QString("skybotFindType\n");
        QStringList objNames;
        //tryMpeph = foDlg->ui_isUseExpNum->isChecked();
            if(foDlg->ui_isUseExpNum->isChecked())
            {
                fitsd->findSkybotNamesList(&objNames, skybot_prog, skybot_prog_folder, fovSpinBox->value()/2.0, obsCode->text(), magObj0,  magObj1);
                sz = objNames.size();
                for(i=0; i<sz; i++) fitsd->getMpephName(objNames.at(i), ast_eph_prog, ast_eph_prog_folder, magObj0,  magObj1);

            }
            else fitsd->findSkybot(skybot_prog, skybot_prog_folder, fovSpinBox->value()/2.0, obsCode->text(), magObj0,  magObj1);
 //fitsd->findLspmCat(starCatList[4]->exeName, starCatList[4]->exePath, starCatList[4]->catPath, fovSpinBox->value());
        }
        /*else
        {
            qDebug() << "nothing is checked\n";
            return 1;
        }*/
        sz = fitsd->objMarks->marks.size();
        qDebug() << "obj num: " << sz << "\n";
        if(sz==0)
        {
            return 1;
        }
        for(i=0; i<sz; i++)
        {

            qDebug() << QString("Object %1: x: %2\ty:%3\n").arg(i).arg(fitsd->objMarks->marks.at(i)->mTanImg[0]).arg(fitsd->objMarks->marks.at(i)->mTanImg[1]);
            qDebug() << QString("Object %1: ksi: %2\teta:%3\n").arg(i).arg(fitsd->objMarks->marks.at(i)->mTan[0]).arg(fitsd->objMarks->marks.at(i)->mTan[1]);
        }
            //getRaDeToTang(ra,de,ra_c,de_c,tp);
        isUseEN = (int)foDlg->ui_isUseExpNum->isChecked();

        fitsd->cleanObjects(fitsd->getApeSize(apeSpinBox->value()));
    }
    else return 1;

    slotScrUpdate();
    //fitsd->findCloserObjects(apeSpinBox->value()*maxObjDisp);

  //  slotFindObjImg();

    return 0;


}

void MainWindow::slotObjectPanel()
{
    if(getEphemerides()) return;
    //slotFindObjImg();
}

void MainWindow::slotFindObjImg()
{
    fitsdata *fitsd = imgLabel->getCurFits();
    if(!fitsd->is_empty) return;
    marksGrid *mgImg = fitsd->marksGIpix;
    marksGrid *mgObj = fitsd->objMarks;
    double iApe = fitsd->getApeSize(apeSpinBox->value());
    double maxDist = iApe*maxObjDisp;
    int i, j, szO, szI;
    double dist, distMin;
    int pos, posMin;
    szO = mgObj->marks.size();
    long nax[2];
    fitsd->detNaxes(&nax[0], &nax[1]);
    int expfnum;

    qDebug() << QString("obj size: %1\n").arg(szO);
    if(isUseEN)
    {
        expfnum = numOfExpLE->text().toInt();
    }
    else expfnum = 1;

    double dRa;

    for(i=0; i<szO; i++)
    {
        posMin = -1;
        distMin = sqrt(nax[0]*nax[0] + nax[1]*nax[1]);
        szI = mgImg->marks.size();
        qDebug() << QString("img size: %1\n").arg(szI);

        qDebug() << QString("Obj pos: %1 %2\n").arg(mgObj->marks.at(i)->mTanImg[0]).arg(mgObj->marks.at(i)->mTanImg[1]);
        for(j=0; j<szI; j++)
        {
            dist = sqrt(pow(mgObj->marks.at(i)->mTanImg[0] - mgImg->marks.at(j)->mTanImg[0], 2.0) + pow(mgObj->marks.at(i)->mTanImg[1] - mgImg->marks.at(j)->mTanImg[1], 2.0));
            qDebug() << QString("%1:%2:%3\n").arg(i).arg(j).arg(dist);
            if(dist<distMin)
            {
                distMin = dist;
                posMin = j;
            }
        }
        qDebug() << QString("pos min: %1\tdist min: %2\tmaxDist: %3").arg(posMin).arg(distMin).arg(maxDist);

        if((distMin<=maxDist)&&(posMin!=-1))
        {
            if(expfnum>1)
            {


            }
    //        qDebug() << QString("mgObjEkv: %1\t%2\t%3\n").arg(mgObj->marks.at(i)->mEkv[0]).arg(mgObj->marks.at(i)->mEkv[1]).arg(mgObj->marks.at(i)->mEkv[2]);
      //      qDebug() << QString("ocObj: %1\t%2\t%3\n").arg(mgObj->marks.at(i)->ocObj->ra).arg(mgObj->marks.at(i)->ocObj->de).arg(mgObj->marks.at(i)->ocObj->mag0);
            qDebug() << QString("pos min: x= %1\ty= %2\n").arg(mgImg->marks.at(posMin)->mTanImg[0]).arg(mgImg->marks.at(posMin)->mTanImg[1]);
            QMessageBox::warning(0, "PixPos", QString("Object:\npos min: x= %1\ty= %2\ndistMin= %3").arg(mgImg->marks.at(posMin)->mTanImg[0], 13, 'f', 5).arg(mgImg->marks.at(posMin)->mTanImg[1], 13, 'f', 5).arg(distMin));
            mgObj->marks.at(i)->copyImg(*mgImg->marks.at(posMin));
      //      qDebug() << QString("mgObjEkv: %1\t%2\t%3\n").arg(mgObj->marks.at(i)->mEkv[0]).arg(mgObj->marks.at(i)->mEkv[1]).arg(mgObj->marks.at(i)->mEkv[2]);
      //      qDebug() << QString("ocObj: %1\t%2\t%3\n").arg(mgObj->marks.at(i)->ocObj->ra).arg(mgObj->marks.at(i)->ocObj->de).arg(mgObj->marks.at(i)->ocObj->mag0);
            /*mgObj->marks.at(i)->mTanImg[0] = mgImg->marks.at(posMin)->mTanImg[0];
            mgObj->marks.at(i)->mTanImg[1] = mgImg->marks.at(posMin)->mTanImg[1];
            mgImg->marks.removeAt(posMin);*/
        }
        else
        {
            QMessageBox::warning(0, "Find Object", QString("Nearest measured mark is too far from obect mark\npos min: %1\tdist min: %2\tmaxDist: %3").arg(posMin).arg(distMin).arg(maxDist));
            if(fitsd->is_empty) mgObj->marks.removeAt(i);
        }
    }


    slotScrUpdate();
}

void MainWindow::slotCheckAllFiles()
{
    int i, n;
    QTableWidgetItem *frItem;

    n = frameList->rowCount();
    for(i=0; i<n;i++)
    {
        frItem = frameList->item(i, 0);
        frItem->setCheckState(Qt::Checked);
    }
}

void MainWindow::slotUnCheckAllFiles()
{
    int i, n;
    QTableWidgetItem *frItem;

    n = frameList->rowCount();
    for(i=0; i<n;i++)
    {
        frItem = frameList->item(i, 0);
        frItem->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::slotCatalogChanged(int curIndex)
{
    currentCat = curIndex;
}

void MainWindow::slotSetupMarksColor()
{
    setupMarksColor scolDlg;

    scolDlg.initColorNames(ipixColStr, catColStr, refColStr, objColStr, bgColStr);
    if(scolDlg.exec()==QDialog::Accepted) scolDlg.getColorNames(&ipixColStr, &catColStr, &refColStr, &objColStr, &bgColStr);

    ipixCol.setNamedColor(ipixColStr);
    catCol.setNamedColor(catColStr);
    refCol.setNamedColor(refColStr);
    objCol.setNamedColor(objColStr);
    bgCol.setNamedColor(bgColStr);

    slotUpdateColors();
}

void MainWindow::slotUpdateColors()
{
    imgLabel->setColors(bgCol, ipixCol, catCol, refCol, objCol);

    ipixPm->fill(ipixCol);
    catPm->fill(catCol);
    refPm->fill(refCol);
    objPm->fill(objCol);
    ipixIcon.addPixmap(*ipixPm);
    catIcon.addPixmap(*catPm);
    refIcon.addPixmap(*refPm);
    objIcon.addPixmap(*objPm);

    activMarksGrid->clear();
    activMarksGrid->insertItem(0, QIcon("./icons/white.bmp"), tr("All"), 0);
    activMarksGrid->insertItem(1, ipixIcon, tr("IPix"), 0);
    activMarksGrid->insertItem(2, catIcon, tr("Cat"), 0);
    activMarksGrid->insertItem(3, refIcon, tr("Ref"), 0);
    activMarksGrid->insertItem(4, objIcon, tr("Obj"), 0);
}

void MainWindow::slotSaveFitsFile()
{
    fitsdata *fitsd = imgLabel->getCurFits();
    if(fitsd!=NULL)
    {
        fitsd->saveFits();
    }
}

void MainWindow::slotSaveFitsFileAs()
{
    QString fitsFileName;
    QStringList nameList;
    fitsdata *fitsd = imgLabel->getCurFits();
    if(fitsd!=NULL)
    {
        QFileDialog dialog(this);
        dialog.selectNameFilter("CCD (*.img *.fit *.fits *.fts)");
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.selectFile(fitsd->fileName);

        //dialog.
        if (dialog.exec())
        {
            nameList = dialog.selectedFiles();

        /*QString fitsFileName = QFileDialog::getSaveFileName(// iieo?aai nienie oaeeia aey ioe?uoey
                                        this,
                                        tr("Choose files"),
                                        currDir,
                                        "CCD (*.img *.fit *.fits *.fts)");
*/
            fitsd->saveFitsAs(nameList.at(0));
        }
    }
}
