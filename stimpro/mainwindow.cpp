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
        QString msg = trUtf8("РЅР¬РҐРђР™Р® РћРџРҐ РќР Р™РџРЁР РҐРҐ РўР®Р