#include "mainwindow.h"


MainWindow::MainWindow()
{
	setWindowTitle("stimproLight");
	//
	stimproSettings = new QSettings(QCoreApplication::applicationDirPath()+"/stimpro.ini",QSettings::IniFormat);
	currentFolder = stimproSettings->value("general/currentFolder").toString();
	//
	setMenu();
	setWidgets();
};


 void MainWindow::setMenu()
 {
	//BEGIN menu "file"
	fileMenu = menuBar()->addMenu(tr("file"));// РґРѕР±Р°РІР»СЏРµРј РјРµРЅСЋ file
	//add action "open fits"
    openFitsAct = new QAction(tr("&open fits"), this);// РґРѕР±Р°РІР»СЏРµРј "РґРµР№СЃС‚РІРёРµ" open fits
    openFitsAct->setShortcut(tr("Ctrl+O"));//РЅР°Р·РЅР°С‡Р°РµРј "РіРѕСЂСЏС‡СѓСЋ РєР»Р°РІРёС€Сѓ" Ctrl+O РґР»СЏ РґРµР№СЃС‚РІРёСЏ open fits
    openFitsAct->setStatusTip(tr("open fits file"));// РЅР°Р·РЅР°С‡Р°РµРј "РїРѕРґСЃРєР°Р·РєСѓ"
    fileMenu->addAction(openFitsAct);
    connect(openFitsAct, SIGNAL(triggered()), this, SLOT(slotOpenFItsFile()));// РЅР°Р·РЅР°С‡Р°РµРј СЃР»РѕС‚, РєРѕС‚РѕСЂС‹Р№ Р±СѓРґРµС‚ РІС‹РїРѕР»РЅСЏС‚СЊСЃСЏ РїСЂРё РІС‹Р±РѕСЂРµ РґР°РЅРЅРѕРіРѕ РїСѓРЅРєС‚Р° РјРµРЅСЋ
	// РѕСЃС‚Р°Р»СЊРЅС‹Рµ СЂР°Р·РґРµР»С‹ РјРµРЅСЋ Рё РёС… "РґРµР№СЃС‚РІРёСЏ" (РѕРїС†РёРё) РґРѕР±Р°РІР»СЏСЋС‚СЃСЏ РїРѕ С‚РѕР№ Р¶Рµ СЃС…РµРјРµ, С‡С‚Рѕ Рё open fits
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
    // add action "view header"
    viewHeaderAct = new QAction(tr("&view header"), this);
    viewHeaderAct->setShortcut(tr("Ctrl+V"));
    viewHeaderAct->setStatusTip(tr("view headern"));
    viewMenu->addAction(viewHeaderAct);
    connect(viewHeaderAct, SIGNAL(triggered()), this, SLOT(slotViewHeader()));
    //view focus X and Y
    viewFocusXAct = new QAction(tr("&view profile X"), this);
    viewFocusXAct->setShortcut(tr("Ctrl+F"));
    viewFocusXAct->setStatusTip(tr("view profile X"));
    viewMenu->addAction(viewFocusXAct);
    connect(viewFocusXAct, SIGNAL(triggered()), this, SLOT(slotViewFocusX()));
    //
    viewFocusYAct = new QAction(tr("&view profile Y"), this);
    viewFocusYAct->setShortcut(tr("Ctrl+G"));
    viewFocusYAct->setStatusTip(tr("view profile Y"));
    viewMenu->addAction(viewFocusYAct);
    connect(viewFocusYAct, SIGNAL(triggered()), this, SLOT(slotViewFocusY()));
    //view hist
    viewHistAct = new QAction(tr("&view histogramm"), this);
    viewHistAct->setShortcut(tr("Ctrl+H"));
    viewHistAct->setStatusTip(tr("view histogramm"));
    viewMenu->addAction(viewHistAct);
    connect(viewHistAct, SIGNAL(triggered()), this, SLOT(slotViewHist()));
    //incStepAct
    incStepAct = new QAction(tr("&step+"), this);
    incStepAct->setShortcut(tr("Ctrl+L"));
    incStepAct->setStatusTip(tr("step+"));
    viewMenu->addAction(incStepAct);
    connect(incStepAct, SIGNAL(triggered()), this, SLOT(slotIncStepAct()));
    //decStepAct
    decStepAct = new QAction(tr("&step-"), this);
    decStepAct->setShortcut(tr("Ctrl+K"));
    decStepAct->setStatusTip(tr("step-"));
    viewMenu->addAction(decStepAct);
    connect(decStepAct, SIGNAL(triggered()), this, SLOT(slotDecStepAct()));
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
	//END menu "frame"
	//BEGIN menu "tools"
	toolsMenu = menuBar()->addMenu(tr("tools"));
	//add action "center of mass"
	cmAct = new QAction(tr("&center of mass"), this);
    cmAct->setShortcut(tr("Ctrl+M"));
    cmAct->setStatusTip(tr("center of mass"));
    toolsMenu->addAction(cmAct);
    connect(cmAct, SIGNAL(triggered()), this, SLOT(slotCenterMass()));
    //add action show/hide aperture
    apertureAct = new QAction(tr("&view aperture"), this);
    apertureAct->setShortcut(tr("Ctrl+A"));
    apertureAct->setStatusTip(tr("view aperture"));
    toolsMenu->addAction(apertureAct);
    connect(apertureAct, SIGNAL(triggered()), this, SLOT(slotViewApertures()));
    //add action inc aperture
    apertureIncAct = new QAction(tr("&aperture+"), this);
    apertureIncAct->setShortcut(tr("Ctrl+]"));
    apertureIncAct->setStatusTip(tr("aperture+"));
    toolsMenu->addAction(apertureIncAct);
    connect(apertureIncAct, SIGNAL(triggered()), this, SLOT(slotIncAperture()));
    //add action dec aperture
    apertureDecAct = new QAction(tr("&aperture-"), this);
    apertureDecAct->setShortcut(tr("Ctrl+["));
    apertureDecAct->setStatusTip(tr("aperture-"));
    toolsMenu->addAction(apertureDecAct);
    connect(apertureDecAct, SIGNAL(triggered()), this, SLOT(slotDecAperture()));
    //add action psf
    psfAct = new QAction(tr("&psf"), this);
    psfAct->setShortcut(tr("Ctrl+F8"));
    psfAct->setStatusTip(tr("psf"));
    toolsMenu->addAction(psfAct);
    connect(psfAct, SIGNAL(triggered()), this, SLOT(slotPSF()));
    //add action show/hide psf
    viewPSFAct = new QAction(tr("&view psf"), this);
    viewPSFAct->setShortcut(tr("Ctrl+P"));
    viewPSFAct->setStatusTip(tr("view psf"));
    toolsMenu->addAction(viewPSFAct);
    connect(viewPSFAct, SIGNAL(triggered()), this, SLOT(slotViewPSF()));
    //
    //СЃР»РµРґСѓСЋС‰Р°СЏ РјРѕРґРµР»СЊ
    nextModelAct = new QAction(tr("&model+"), this);
    nextModelAct->setShortcut(tr("Ctrl+F12"));
    nextModelAct->setStatusTip(tr("model+"));
    toolsMenu->addAction(nextModelAct);
    connect(nextModelAct, SIGNAL(triggered()), this, SLOT(slotNextModel()));
	//РїСЂРµРґС‹РґСѓС‰Р°СЏ РјРѕРґРµР»СЊ
	prevModelAct = new QAction(tr("&model-"), this);
    prevModelAct->setShortcut(tr("Ctrl+F11"));
    prevModelAct->setStatusTip(tr("model-"));
    toolsMenu->addAction(prevModelAct);
    connect(prevModelAct, SIGNAL(triggered()), this, SLOT(slotPrevModel()));
	//QAction *incOrderAct;//СѓРІРµР»РёС‡РёС‚СЊ РїРѕРєР°Р·Р°С‚РµР»СЊ СЃС‚РµРїРµРЅРё РјРѕРґРµР»Рё PSF
	incOrderAct = new QAction(tr("&order+"), this);
    incOrderAct->setShortcut(tr("Ctrl+F10"));
    incOrderAct->setStatusTip(tr("order+"));
    toolsMenu->addAction(incOrderAct);
    connect(incOrderAct, SIGNAL(triggered()), this, SLOT(slotIncOrder()));
	//QAction *decOrderAct;//СѓРјРµРЅСЊС€РёС‚СЊ РїРѕРєР°Р·Р°С‚РµР»СЊ СЃС‚РµРїРµРЅРё РјРѕРґРµР»Рё PSF
	decOrderAct = new QAction(tr("&order-"), this);
    decOrderAct->setShortcut(tr("Ctrl+F9"));
    decOrderAct->setStatusTip(tr("order-"));
    toolsMenu->addAction(decOrderAct);
    connect(decOrderAct, SIGNAL(triggered()), this, SLOT(slotDecOrder()));
    //QAction *orderToUnitAct;//РїСЂРёСЂР°РІРЅСЏС‚СЊ РїРѕРєР°Р·Р°С‚РµР»СЊ СЃС‚РµРїРµРЅРё РјРѕРґРµР»Рё PSF Рє РµРґРёРЅРёС†Рµ
    orderToUnitAct = new QAction(tr("&order=1"), this);
    orderToUnitAct->setShortcut(tr("Ctrl+U"));
    orderToUnitAct->setStatusTip(tr("order=1"));
    toolsMenu->addAction(orderToUnitAct);
    connect(orderToUnitAct, SIGNAL(triggered()), this, SLOT(slotOrderToUnit()));
    ////
    findStarsAct = new QAction(tr("&find stars"), this);
    findStarsAct->setShortcut(tr("Alt+F"));
    findStarsAct->setStatusTip(tr("find stars"));
    toolsMenu->addAction(findStarsAct);
    connect(findStarsAct, SIGNAL(triggered()), this, SLOT(slotFindStars()));
    ////
    matchStarsAct = new QAction(tr("&match stars"), this);
    matchStarsAct->setShortcut(tr("Alt+M"));
    matchStarsAct->setStatusTip(tr("match stars"));
    toolsMenu->addAction(matchStarsAct);
    connect(matchStarsAct, SIGNAL(triggered()), this, SLOT(slotMatchStars()));
    //
    corrAct = new QAction(tr("&corr"), this);
    corrAct->setShortcut(tr("Alt+C"));
    corrAct->setStatusTip(tr("correlation"));
    toolsMenu->addAction(corrAct);
    connect(corrAct, SIGNAL(triggered()), this, SLOT(slotCorrAct()));
    //
    sldAct = new QAction(tr("&line data"), this);
    sldAct->setShortcut(tr("Alt+L"));
    sldAct->setStatusTip(tr("save line data"));
    toolsMenu->addAction(sldAct);
    connect(sldAct, SIGNAL(triggered()), this, SLOT(slotSldAct()));
    //END menu "tools"
    viewFoundStarsAct = new QAction(tr("&view found stars"), this);
    viewFoundStarsAct->setShortcut(tr("Alt+S"));
    viewFoundStarsAct->setStatusTip(tr("view found stars"));
    viewMenu->addAction(viewFoundStarsAct);
    connect(viewFoundStarsAct, SIGNAL(triggered()), this, SLOT(slotViewFoundStars()));
    //
 };
 
 //BEGIN menu slots
//menu file
void MainWindow::slotOpenFItsFile()// СЃР»РѕС‚ РґР»СЏ РѕС‚РєСЂС‹С‚РёСЏ fits С„Р°Р№Р»Р°
{
	QStringList fitsFileNames = QFileDialog::getOpenFileNames(// РїРѕР»СѓС‡Р°РµРј СЃРїРёСЃРѕРє С„Р°Р№Р»РѕРІ РґР»СЏ РѕС‚РєСЂС‹С‚РёСЏ
                    this,
                    tr("Choose files"),
                    currentFolder,
                    "CCD (*.img *.fit *.fits *.fts *.cmp)");
    if(fitsFileNames.count()>0)fitsFileNames.sort();
    for(int i=0;i<fitsFileNames.count();i++)// С†РёРєР» РґР»СЏ РѕС‚РєСЂС‹С‚РёСЏ РІСЃРµС… С„Р°Р№Р»РѕРІ РїРѕ РёРјРµРЅР°Рј РІ СЃРїРёСЃРєРµ fitsFileNames
	{
		fitsdata *fitsd = new fitsdata(fitsFileNames[i]);// РѕС‚РєСЂС‹С‚РёРµ i-С‚РѕРіРѕ С„Р°Р№Р»Р° Рё СЃРѕР·РґР°РЅРёРµ РѕР±СЉРµРєС‚Р° С‚РёРїР° fitsdata
		fitsd->is_scaled = 0;
		imgLabel->fitsdataList << fitsd;// РґРѕР±Р°РІР»СЏРµРј РёРЅРёС†РёР°Р»РёР·РёСЂРѕРІР°РЅРЅС‹Р№ СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РѕР±СЉРµРєС‚ fitsdata РІ СЃРїРёСЃРѕРє СѓРєР°Р·Р°С‚РµР»РµР№ РЅР° СЌС‚Рё РѕР±СЉРµРєС‚С‹ imgLabel->fitsdataList
		//QMessageBox::information(0,"stimpro",fitsd->fitsHeader,QMessageBox::Ok,0,0);
	}
	if(fitsFileNames.count()>0)
	{
		setWindowTitle("stimpro:"+imgLabel->fitsdataList[imgLabel->frameIndex]->fileName);// РїРѕРєР°Р·С‹РІР°РµРј РёРјСЏ С„Р°Р№Р»Р° РІ РїРѕР»Рµ Р·Р°РіРѕР»РѕРІРєР° РіР»Р°РІРЅРѕРіРѕ РѕРєРЅР°
		QFileInfo fi(imgLabel->fitsdataList[imgLabel->frameIndex]->fileName);
		currentFolder = fi.absolutePath();
		stimproSettings->setValue("general/currentFolder",currentFolder);
	}
	slotScrUpdate();
};


void MainWindow::slotExit()//РІС‹С…РѕРґ РёР· РїСЂРёР»РѕР¶РµРЅРёСЏ
{
	QApplication::exit();
};

void MainWindow::slotIncrease()
{
	if(imgLabel->scale!=16)
	{
		hscrbValue = scrollArea->horizontalScrollBar()->value();
		vscrbValue = scrollArea->verticalScrollBar()->value();
		imgLabel->scale = imgLabel->scale*2;//СѓРґРІР°РёРІР°РµРј РјР°СЃС€С‚Р°Р± РїСЂРѕСЂРёСЃРѕРІРєРё РёР·РѕР±СЂР°Р¶РµРЅРёСЏ (РјР°СЃС€С‚Р°Р± РЅРµ Р±РѕР»СЊС€Рµ, С‡РµРј 16)
		slotScrUpdate();// РѕР±РЅРѕРІР»СЏРµРј РёР·РѕР±СЂР°Р¶РµРЅРёРµ РЅР° СЌРєСЂР°РЅРµ
	}
};
void MainWindow::slotDecrease()
{
	if(imgLabel->scale!=0.0625)
	{
		hscrbValue = scrollArea->horizontalScrollBar()->value();
		vscrbValue = scrollArea->verticalScrollBar()->value();
		imgLabel->scale = imgLabel->scale/2;//СѓРјРµРЅСЊС€Р°РµРј РјР°СЃС€С‚Р°Р± РїСЂРѕСЂРёСЃРѕРІРєРё РёР·РѕР±СЂР°Р¶РµРЅРёСЏ РІ РґРІР° СЂР°Р·Р° (РјР°СЃС€С‚Р°Р± РЅРµ РјРµРЅСЊС€Рµ, С‡РµРј 1/16) 
		slotScrUpdate();// РѕР±РЅРѕРІР»СЏРµРј РёР·РѕР±СЂР°Р¶РµРЅРёРµ РЅР° СЌРєСЂР°РЅРµ
	}
};


void MainWindow::slotNext()
{
	if(imgLabel->frameIndex<(imgLabel->fitsdataList.count()-1))
	{
		imgLabel->frameIndex++;//СѓРІРµР»РёС‡РµРЅРёРµ С‚РµРєСѓС‰РµРіРѕ РёРЅРґРµРєСЃР° РЅР° 1
		setWindowTitle("stimpro:"+imgLabel->fitsdataList[imgLabel->frameIndex]->fileName);// РїРѕРєР°Р·С‹РІР°РµРј РёРјСЏ С„Р°Р№Р»Р° РІ РїРѕР»Рµ Р·Р°РіРѕР»РѕРІРєР° РіР»Р°РІРЅРѕРіРѕ РѕРєРЅР°
		hscrbValue = scrollArea->horizontalScrollBar()->value();
		vscrbValue = scrollArea->verticalScrollBar()->value();
		slotScrUpdate();//РїСЂРѕСЂРёСЃРѕРІРєР° РёР·РѕР±СЂР°Р¶РµРЅРёСЏ
	}
};
void MainWindow::slotPreviouse()
{
	if(imgLabel->frameIndex>0)
	{
		imgLabel->frameIndex--;//СѓРјРµРЅСЊС€РµРЅРёРµ С‚РµРєСѓС‰РµРіРѕ РёРЅРґРµРєСЃР° РЅР° 1
		setWindowTitle("stimpro:"+imgLabel->fitsdataList[imgLabel->frameIndex]->fileName);// РїРѕРєР°Р·С‹РІР°РµРј РёРјСЏ С„Р°Р№Р»Р° РІ РїРѕР»Рµ Р·Р°РіРѕР»РѕРІРєР° РіР»Р°РІРЅРѕРіРѕ РѕРєРЅР°
		hscrbValue = scrollArea->horizontalScrollBar()->value();
		vscrbValue = scrollArea->verticalScrollBar()->value();
		slotScrUpdate();//РїСЂРѕСЂРёСЃРѕРІРєР° РёР·РѕР±СЂР°Р¶РµРЅРёСЏ
	}
};
void MainWindow::slotDelete()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];//РїРµСЂРµРґР°РµРј СѓРєР°Р·Р°С‚РµР»СЊ С‚РµРєСѓС‰РµРіРѕ РѕР±СЉРµРєС‚Р° fitsdata РїРѕ РёРЅРґРµРєСЃСѓ РІ fitsd
		imgLabel->fitsdataList.removeAt(imgLabel->frameIndex);// СѓРґР°Р»СЏРµРј СѓРєР°Р·Р°С‚РµР»СЊ РёР· СЃРїРёСЃРєР°
		delete fitsd;//РѕСЃРІРѕР±РѕР¶РґР°РµРј РїР°РјСЏС‚СЊ
		if((imgLabel->frameIndex==imgLabel->fitsdataList.count()))imgLabel->frameIndex--;//РµСЃР»Рё С‚РµРєСѓС‰РёР№ РёРЅРґРµРєСЃ РїРѕСЃР»РµРґРЅРёР№ РІ СЃРїРёСЃРєРµ, СѓРјРµРЅСЊС€Р°РµРј РµРіРѕ РЅР° 1
		if(imgLabel->frameIndex<0) {imgLabel->frameIndex=0;setWindowTitle("stimpro");}// РµСЃР»Рё СѓРґР°Р»РµРЅ РїРѕСЃР»РµРґРЅРёР№ РѕР±СЉРµРєС‚ СЃРїРёСЃРєР°, СѓР±РёСЂР°РµРј РёРјСЏ С„Р°Р№Р»Р° РёР· Р·Р°РіРѕР»РѕРІРєР° РѕРєРЅР° Рё СѓСЃС‚Р°РЅР°РІР»РёРІР°РµРј С‚РµРєСѓС‰РёР№ РёРЅРґРµРєСЃ РІ 0
		else setWindowTitle("stimpro:"+imgLabel->fitsdataList[imgLabel->frameIndex]->fileName);//РёРЅР°С‡Рµ, РїРµСЂРµРґРµР»С‹РІР°РµРј Р·Р°РіРѕР»РѕРІРѕРє РіР»Р°РІРЅРѕРіРѕ РѕРєРЅР° РІ СЃРѕРѕС‚РІРµС‚СЃС‚РІРёРµ СЃ С‚РµРєСѓС‰РёРј РёРЅРґРµРєСЃРѕРј
		imgLabel->repaint();
		//hscrbValue = scrollArea->horizontalScrollBar()->value();
		//vscrbValue = scrollArea->verticalScrollBar()->value();
		//slotScrUpdate();//РїСЂРѕСЂРёСЃРѕРІРєР° РёР·РѕР±СЂР°Р¶РµРЅРёСЏ
	}
};

/////////////////////////////////////////

void MainWindow::slotBackLeft()//1. СЃРјРµС‰РµРЅРёРµ Р»РµРІРѕР№ РіСЂР°РЅРёС†С‹ "С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё" РІР»РµРІРѕ
{
	if(imgLabel->fitsdataList.count()>0)//СЂР°Р±РѕС‚Р°РµРј, РµСЃР»Рё РµСЃС‚СЊ СЃ С‡РµРј СЂР°Р±РѕС‚Р°С‚СЊ
	{
		fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];//РїРµСЂРµРґР°РµРј СѓРєР°Р·Р°С‚РµР»СЊ С‚РµРєСѓС‰РµРіРѕ РѕР±СЉРµРєС‚Р° fitsdata РїРѕ РёРЅРґРµРєСЃСѓ РІ fitsd
		fitsd->minp=fitsd->minp-fitsd->st;// СѓРјРµРЅСЊС€Р°РµРј fitsd->minp РЅР° fitsd->st (С€Р°Рі РёР·РјРµРЅРµРЅРёСЏ "С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё")
		if(fitsd->minp<fitsd->minv)fitsd->minp=fitsd->minv;
		imgLabel->repaint();
		//slotScrUpdate();//РїСЂРѕСЂРёСЃРѕРІРєР° РёР·РѕР±СЂР°Р¶РµРЅРёСЏ
	}
};
void MainWindow::slotBackRight()//2. СЃРјРµС‰РµРЅРёРµ Р»РµРІРѕР№ РіСЂР°РЅРёС†С‹ "С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё" РІРїСЂР°РІРѕ
{
	if(imgLabel->fitsdataList.count()>0)
	{
		fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
		fitsd->minp=fitsd->minp+fitsd->st;
		if(fitsd->minp>fitsd->maxv)fitsd->minp=fitsd->maxv;
		imgLabel->repaint();
		//QMessageBox::information(0,"stimpro",QString("%1").arg(fitsd->st,8,'f',0,QLatin1Char( ' ' )),QMessageBox::Ok,0,0);
	}
};
void MainWindow::slotSignalLeft()//3. СЃРјРµС‰РµРЅРёРµ РїСЂР°РІРѕР№ РіСЂР°РЅРёС†С‹ "С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё" РІР»РµРІРѕ
{
	if(imgLabel->fitsdataList.count()>0)
	{
		fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
		fitsd->maxp=fitsd->maxp-fitsd->st;
		if(fitsd->maxp<fitsd->minv)fitsd->maxp=fitsd->minv;
		imgLabel->repaint();
	}
};
void MainWindow::slotSignalRight()//4. СЃРјРµС‰РµРЅРёРµ РїСЂР°РІРѕР№ РіСЂР°РЅРёС†С‹ "С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё" РІРїСЂР°РІРѕ
{
	if(imgLabel->fitsdataList.count()>0)
	{
		fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
		fitsd->maxp=fitsd->maxp+fitsd->st;
		if(fitsd->maxp>fitsd->maxv)fitsd->maxp=fitsd->maxv;
		imgLabel->repaint();
	}
};

void MainWindow::slotSetDefault()//5. РІРѕР·РІСЂР°С‰РµРЅРёРµ Рє РїР°СЂР°РјРµС‚СЂР°Рј РїРѕ-СѓРјРѕР»С‡Р°РЅРёСЋ, РІС‹С‡РёСЃР»РµРЅРЅС‹Рј РїСЂРё РѕС‚РєСЂС‹С‚РёРё fits С„Р°Р№Р»Р°
{
	if(imgLabel->fitsdataList.count()>0)
	{
		fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
		fitsd->maxp=fitsd->fpix+fitsd->hw;
		fitsd->minp=fitsd->fpix-fitsd->hw;
		fitsd->st=2*fitsd->hw/25;
		imgLabel->repaint();
	}
};

void MainWindow::slotSetFullDiapason()//6. СѓСЃС‚Р°РЅРѕРІРєР° "С‰РµР»Рё РїСЂРѕСЂРёСЃРѕРІРєРё" СЂР°РІРЅРѕР№ РІСЃРµРјСѓ РґРёР°РїР°Р·РѕРЅСѓ Р·РЅР°С‡РµРЅРёР№ РЅР° РїРёРєСЃРµР»СЏС…
{
	if(imgLabel->fitsdataList.count()>0)
	{
		fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
		fitsd->maxp=fitsd->maxv;
		fitsd->minp=fitsd->minv;
		fitsd->st=(fitsd->maxv-fitsd->minv)/25;
		imgLabel->repaint();
	}
}

void MainWindow::slotViewHeader()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
		//QMessageBox::information(0,"stimpro",fitsd->fitsHeader,QMessageBox::Ok,0,0);
		QTextStream stream(stdout);
		stream << fitsd->fileName <<"\n" << fitsd->fitsHeader << "\n";
	}
};

void MainWindow::slotViewFocusX()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		if(imgLabel->focusX) imgLabel->focusX = 0; else imgLabel->focusX = 1;
		imgLabel->repaint();
	}
};

void MainWindow::slotViewFocusY()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		if(imgLabel->focusY) imgLabel->focusY = 0; else imgLabel->focusY = 1;
		imgLabel->repaint();
	}
};

void MainWindow::slotViewHist()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		if(imgLabel->viewHist) imgLabel->viewHist = 0; else imgLabel->viewHist = 1;
		imgLabel->repaint();
	}
};

void  MainWindow::slotIncStepAct()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		imgLabel->fitsdataList[imgLabel->frameIndex]->st = 
			imgLabel->fitsdataList[imgLabel->frameIndex]->st*2;
		if(imgLabel->viewHist) imgLabel->repaint();
	}
};

void  MainWindow::slotDecStepAct()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		imgLabel->fitsdataList[imgLabel->frameIndex]->st = 
			imgLabel->fitsdataList[imgLabel->frameIndex]->st/2;
		if(imgLabel->viewHist) imgLabel->repaint();
	}
};
//menu tools

void MainWindow::slotCenterMass()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
		double* fd = new double[4*imgLabel->aperture*imgLabel->aperture];
		fitsd->getAperture(fd, imgLabel->cX, imgLabel->cY, imgLabel->aperture);
		double cm[2];
		fitsd->cm(cm, fd, imgLabel->cX, imgLabel->cY, imgLabel->aperture);
		delete[] fd;
		QString msg =
			QString("%1").arg(cm[0],10,'f',3,QLatin1Char( ' ' ))+" "+
			QString("%1").arg(cm[1],10,'f',3,QLatin1Char( ' ' ));
		statusBar()->showMessage(msg);
	}
};

void MainWindow::slotViewApertures()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		if(imgLabel->viewAperture) imgLabel->viewAperture = 0; else imgLabel->viewAperture = 1;
		imgLabel->repaint();
	}	
};

void MainWindow::slotIncAperture()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		if(imgLabel->aperture<=100) imgLabel->aperture++;
		statusBar()->showMessage(QString("%1").arg( imgLabel->aperture,5,10,QLatin1Char( ' ' )));
		imgLabel->repaint();
	}
};

void MainWindow::slotDecAperture()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		if(imgLabel->aperture>=3) imgLabel->aperture--;
		statusBar()->showMessage(QString("%1").arg( imgLabel->aperture,5,10,QLatin1Char( ' ' )));
		imgLabel->repaint();
	}	
};

void MainWindow::slotPSF()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
		double* fd = new double[4*imgLabel->aperture*imgLabel->aperture];
		fitsd->getAperture(fd, imgLabel->cX, imgLabel->cY, imgLabel->aperture);
		double* rfd = new double[4*(imgLabel->aperture+10)*(imgLabel->aperture+10)];
		fitsd->getRing(rfd, imgLabel->cX, imgLabel->cY, imgLabel->aperture, imgLabel->aperture+10);
		//fitsd->getBackground(rfd, imgLabel->aperture+10);
		fitsd->subtractBackground(rfd,fd,imgLabel->aperture, imgLabel->aperture+10);
                bool psfok = fitsd->psffitting(fd, imgLabel->aperture, imgLabel->psfmodel);
		delete[] fd;
		delete[] rfd;
		fitsd->psfX = imgLabel->cX;
		fitsd->psfY = imgLabel->cY;
		fitsd->psfRho = imgLabel->aperture;
		fitsd->psfModel = imgLabel->psfmodel;
		QTextStream stream(stdout);
                stream << "PSF parameters: \n";
		for (int i=0; i<8; i++)
			stream << QString("%1").arg( fitsd->p[i],10,'f',3,QLatin1Char( ' ' )) <<'\n';
		stream <<"number of iterations:\n"<< QString("%1").arg( fitsd->n_it,5,10,QLatin1Char( ' ' )) <<'\n';
		stream <<"model:\n"<< QString("%1").arg( imgLabel->psfmodel,5,10,QLatin1Char( ' ' )) <<'\n';
		//stream << "estimator:\n"<<QString("%1").arg( fitsd->p[11],10,'E',3,QLatin1Char( ' ' )) <<"\n";
		//stream << "kappa:\n"<<QString("%1").arg( fitsd->p[12],10,'E',3,QLatin1Char( ' ' )) <<"\n\n";
		stream << "errors:\n";
                //fitsd->p[0] = imgLabel->cX - imgLabel->aperture+fitsd->p[0];
                //fitsd->p[1] = imgLabel->cY - imgLabel->aperture+fitsd->p[1];
		for (int i=8; i<17; i++)
			stream << QString("%1").arg( fitsd->p[i],10,'f',6,QLatin1Char( ' ' )) <<'\n';
		stream << "flux1:\n"<<QString("%1").arg( fitsd->p[17],10,'f',3,QLatin1Char( ' ' )) <<"\n";
		stream << "flux2:\n"<<QString("%1").arg( fitsd->p[18],10,'f',3,QLatin1Char( ' ' )) <<"\n";
		stream << "num:\n"<<QString("%1").arg( fitsd->p[19],10,'f',0,QLatin1Char( ' ' )) <<"\n\n";
		stream << "N:\n"<<QString("%1").arg( fitsd->p[20],10,'f',0,QLatin1Char( ' ' )) <<"\n";
		//stream << "meanStep:\n"<<QString("%1").arg( fitsd->p[21],10,'E',3,QLatin1Char( ' ' )) <<"\n";
		//stream << "lambda:\n"<<QString("%1").arg( fitsd->p[22],10,'E',3,QLatin1Char( ' ' )) <<"\n";
                if (psfok) stream << "OK\n";
                stream << QString("%1").arg(imgLabel->cX - imgLabel->aperture+fitsd->p[0],10,'f',6,QLatin1Char( ' ' )) <<'\n';
                stream << QString("%1").arg(imgLabel->cY - imgLabel->aperture+fitsd->p[1],10,'f',6,QLatin1Char( ' ' )) <<'\n';
	}
};

void MainWindow::slotCorrAct()
{
  if(imgLabel->fitsdataList.count()>0)
    {
      fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
      double corr=0;
      int xc = imgLabel->cX;
      int yc = imgLabel->cY;
      double Jc = fitsd->getPixelValueXY(xc,yc);
      //int n=1;
        //for(int p=imgLabel->cY-n;p<imgLabel->cY+n;p++)
        //    for(int q=imgLabel->cX-n;q<imgLabel->cX+n;q++)
        //        if((q!=imgLabel->cX)&&(p!=imgLabel->cY))corr+=(cv - fitsd->getPixelValueXY(q,p))/(fitsd->getPixelValueXY(q,p)-fitsd->getPixelValueXY(q+1,p+1));
       /* corr=(fitsd->getPixelValueXY(x,y) - fitsd->getPixelValueXY(x-1,y-1))/(fitsd->getPixelValueXY(x-1,y-1) - fitsd->getPixelValueXY(x-2,y-2));
        corr+=(fitsd->getPixelValueXY(x,y) - fitsd->getPixelValueXY(x,y-1))/(fitsd->getPixelValueXY(x,y-1) - fitsd->getPixelValueXY(x,y-2));
        corr+=(fitsd->getPixelValueXY(x,y) - fitsd->getPixelValueXY(x+1,y-1))/(fitsd->getPixelValueXY(x+1,y-1) - fitsd->getPixelValueXY(x+2,y-2));
        //
        corr+=(fitsd->getPixelValueXY(x,y) - fitsd->getPixelValueXY(x+1,y))/(fitsd->getPixelValueXY(x+1,y) - fitsd->getPixelValueXY(x+2,y));
        corr+=(fitsd->getPixelValueXY(x,y) - fitsd->getPixelValueXY(x-1,y))/(fitsd->getPixelValueXY(x-1,y) - fitsd->getPixelValueXY(x-2,y));
        //
        corr=(fitsd->getPixelValueXY(x,y) - fitsd->getPixelValueXY(x-1,y+1))/(fitsd->getPixelValueXY(x-1,y+1) - fitsd->getPixelValueXY(x-2,y+2));
        corr+=(fitsd->getPixelValueXY(x,y) - fitsd->getPixelValueXY(x,y+1))/(fitsd->getPixelValueXY(x,y+1) - fitsd->getPixelValueXY(x,y+2));
        corr+=(fitsd->getPixelValueXY(x,y) - fitsd->getPixelValueXY(x+1,y+1))/(fitsd->getPixelValueXY(x+1,y+1) - fitsd->getPixelValueXY(x+2,y+2));
        corr = corr/8;*/
      double Jb=0;
      for(int y=yc-1;y<=yc+1;y++)
            for(int x=xc-1;x<=xc+1;x++)
               if((x!=xc)&&(y!=yc))Jb+=fitsd->getPixelValueXY(x,y);
        Jb=Jb/8;
        int bsize = 7;
        double r,Jm,J;
        double mu1=0,mu2=0;
        for(int y=yc-bsize;y<=yc+bsize;y++)
            for(int x=xc-bsize;x<=xc+bsize;x++)
            {
                r = sqrt((x-xc)*(x-xc)+(y-yc)*(y-yc));
                if(r>2)
                {
                    Jm = Jc/(1+r*r);
                    J = fitsd->getPixelValueXY(x,y);
                    mu1+=(Jb-Jm)*(Jb-J);
                    mu2+=(Jb-Jm)*(Jb-Jm);
                }

            }
        corr = mu1/mu2;
        //QString msg = QString("%1").arg(corr,10,'f',4,QLatin1Char( ' ' ));
        //statusBar()->showMessage(msg);
        QTextStream stream(stdout);
         stream <<QString("%1").arg(imgLabel->cX,10,'f',4,QLatin1Char( ' ' )) <<'|'
                 <<QString("%1").arg(imgLabel->cY,10,'f',4,QLatin1Char( ' ' )) <<'|'
                 << QString("%1").arg(corr,10,'f',4,QLatin1Char( ' ' )) <<'\n';
    }
}

void MainWindow::slotSldAct()
{
  if(imgLabel->fitsdataList.count()>0)
    {
      fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
      //
      QFile resFile(QApplication::applicationDirPath()+"/out/out.txt");
      resFile.open(QIODevice::Append| QIODevice::Text);
      QTextStream resStream;
      resStream.setDevice(&resFile);
      for(int x=0;x<fitsd->naxes[0];x++)
          resStream
                    << QString( "%1" ).arg(x,5,'f',0,QLatin1Char( ' ' )) << '|'
                    << QString( "%1" ).arg(fitsd->getPixelValueXY(x,imgLabel->cY),10,'f',3,QLatin1Char( ' ' )) << '\n'
                    ;
       resFile.close();
      //
    }
}

void MainWindow::slotFindStars()
{
    if(imgLabel->fitsdataList.count()>0)
        {
            fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
            fitsd->findStars();
            imgLabel->viewFoundStars = 1;
            imgLabel->repaint();
        }
};

void MainWindow::slotMatchStars()
{
    if(imgLabel->fitsdataList.count()>0)
        {
            fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
            if(fitsd->matchStars())QMessageBox::information(0,"stimpro","ok",QMessageBox::Ok,0,0);;
            //imgLabel->viewFoundStars = 1;
            //imgLabel->repaint();
        }
};

void MainWindow::slotViewFoundStars()
{
    if(imgLabel->fitsdataList.count()>0)
        {
            if(imgLabel->viewFoundStars) imgLabel->viewFoundStars = 0; else imgLabel->viewFoundStars = 1;
            imgLabel->repaint();
        }
};

void MainWindow::slotViewPSF()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		if(imgLabel->viewPSF) imgLabel->viewPSF = 0; else imgLabel->viewPSF = 1;
		imgLabel->repaint();
	}	
};

void MainWindow::slotNextModel()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		if(imgLabel->psfmodel<2)imgLabel->psfmodel++;
		statusBar()->showMessage(QString("%1").arg( imgLabel->psfmodel,5,10,QLatin1Char( ' ' )));
	}	
};

void MainWindow::slotPrevModel()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		if(imgLabel->psfmodel>0)imgLabel->psfmodel--;
		statusBar()->showMessage(QString("%1").arg( imgLabel->psfmodel,5,10,QLatin1Char( ' ' )));
	}		
};

void MainWindow::slotIncOrder()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
		fitsd->p[7]+=0.1;
		statusBar()->showMessage(QString("%1").arg( fitsd->p[7],10,'f',3,QLatin1Char( ' ' )));
	}		
};

void MainWindow::slotDecOrder()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
		fitsd->p[7]-=0.1;
		statusBar()->showMessage(QString("%1").arg( fitsd->p[7],10,'f',3,QLatin1Char( ' ' )));
	}		
};

void MainWindow::slotOrderToUnit()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
		for(int i=0;i<8;i++)fitsd->p[i]=0;
		fitsd->p[7]=1;
		fitsd->p[2] = fitsd->fpix;
		statusBar()->showMessage(QString("%1").arg( fitsd->p[7],10,'f',3,QLatin1Char( ' ' )));
	}
};
//END menu slots

void MainWindow::setWidgets()//РёРЅРёС†РёР°Р»РёР·Р°С†РёСЏ РІРёРґР¶РµС‚РѕРІ
{
	scrollArea = new QScrollArea(this);
	scrollArea->setBackgroundRole(QPalette::Dark);
	imgLabel = new imageLabel(2000,2000,scrollArea,statusBar(),this);
	scrollArea->setWidget(imgLabel);
	setCentralWidget(scrollArea);
	connect(scrollArea->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(slotUpdateScrollBars()));
	connect(scrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(slotUpdateScrollBars()));
};

void  MainWindow::slotScrUpdate()
{
	if(imgLabel->fitsdataList.count()>0)
	{
		fitsdata *fitsd = imgLabel->fitsdataList[imgLabel->frameIndex];
		imgLabel->setFixedSize(fitsd->naxes[0]*imgLabel->scale,fitsd->naxes[1]*imgLabel->scale);
		scrollArea->horizontalScrollBar()->setValue(imgLabel->cX*imgLabel->scale-scrollArea->frameRect().width()/2);
		scrollArea->verticalScrollBar()->setValue(imgLabel->cY*imgLabel->scale-scrollArea->frameRect().height()/2);
		imgLabel->repaint();
	}
};

void MainWindow::slotUpdateScrollBars()
{
	imgLabel->repaint();
};


