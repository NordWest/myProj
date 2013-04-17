#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ephthread.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("EphA");
    timeStr = "";

    setMenu();
    setWidgets();
    setSettings();

    connect(this, SIGNAL(clicked()), this, SLOT(slotViewSettWindow()));


    timeUpd = new QTimer(this);
    connect(timeUpd, SIGNAL(timeout()), this, SLOT(slotUpdateTime()));
    tabUpd = new QTimer(this);
    connect(tabUpd, SIGNAL(timeout()), this, SLOT(slotUpdateTable()));

    updaterEnabled = 0;
    //expTime = 1000*10;
    ui->expProgBar->setRange(0, 100);
    ui->expProgBar->setValue(0);

    timeUpd->setSingleShot(0);
    slotUpdateTime();

    tabEla = new QElapsedTimer;

    rFile.init("./res.lst");
    slotInitResTable();
    slotUpdateTable();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setMenu()
{
    fileMenu = menuBar()->addMenu(tr("file"));

    openResAct = new QAction(tr("&Open res file"), this);
    openResAct->setShortcut(tr("Ctrl+O"));
    openResAct->setStatusTip(tr("open res file"));
    fileMenu->addAction(openResAct);
    connect(openResAct, SIGNAL(triggered()), this, SLOT(slotOpenResFileWindow()));


    exitAct = new QAction(tr("&exit"), this);
    exitAct->setShortcut(tr("Ctrl+X"));
    exitAct->setStatusTip(tr("exit"));
    fileMenu->addAction(exitAct);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(slotExit()));

    viewMenu = menuBar()->addMenu(tr("view"));
    viewNextAct = new QAction(tr("&view next"), this);
    viewNextAct->setShortcut(tr("Ctrl+n"));//
    viewNextAct->setStatusTip(tr("view next"));//
    viewMenu->addAction(viewNextAct);
    connect(viewNextAct, SIGNAL(triggered()), this, SLOT(slotViewNextObj()));

    viewPrevAct = new QAction(tr("&view prev"), this);
    viewPrevAct->setShortcut(tr("Ctrl+p"));//
    viewPrevAct->setStatusTip(tr("view next"));//
    viewMenu->addAction(viewPrevAct);
    connect(viewPrevAct, SIGNAL(triggered()), this, SLOT(slotViewPrevObj()));
/*
    viewSettAct = new QAction(tr("&view sett"), this);
    viewSettAct->setShortcut(tr("Ctrl+U"));
    viewSettAct->setStatusTip(tr("view settings"));
    viewMenu->addAction(viewSettAct);
    connect(viewSettAct, SIGNAL(triggered()), this, SLOT(slotViewSettWindow()));*/
}

void MainWindow::setWidgets()
{
//    mainTree = new QTreeWidget(this);
//    mainTree = qFindChild<QTreeWidget*>(this, "treeWidget");
//    mainTree->setColumnCount();


    mainTable = qFindChild<QTableWidget*>(this, "tableWidget");
    mainTable->setColumnCount(9);
    QStringList vhLabelsT;
    vhLabelsT << QString(tr("Name"));
    vhLabelsT << QString(tr("RA"));
    vhLabelsT << QString(tr("DEC"));
    vhLabelsT << QString(tr("Magn"));
    vhLabelsT << QString(tr("t"));
    vhLabelsT << QString(tr("mu_ra"));
    vhLabelsT << QString(tr("mu_dec"));
    vhLabelsT << QString(tr("Exp"));
    vhLabelsT << QString(tr("Tasks"));
    mainTable->setHorizontalHeaderLabels(vhLabelsT);
    mainTable->setColumnWidth(0, 200);
    mainTable->setColumnWidth(1, 150);
    mainTable->setColumnWidth(2, 150);

    sysTimeEdit = new QLineEdit(this);
    sysTimeEdit->setMaximumWidth(100);
    sysTimeEdit->setReadOnly(1);
    statusBar()->addWidget(sysTimeEdit);

    jdTimeEdit = new QLineEdit(this);
    jdTimeEdit->setMaximumWidth(500);
    jdTimeEdit->setReadOnly(1);
    statusBar()->addWidget(jdTimeEdit);

    starTimeEdit = new QLineEdit(this);
    starTimeEdit->setMaximumWidth(100);
    starTimeEdit->setReadOnly(1);
    statusBar()->addWidget(starTimeEdit);

    mainToolBar = qFindChild<QToolBar*>(this, "mainToolBar");

    startButton = new QPushButton(tr("start"));
    mainToolBar->addWidget(startButton);
    connect(startButton, SIGNAL(clicked()), this, SLOT(slotStartUpdater()));

    stopButton = new QPushButton(tr("stop"));
    mainToolBar->addWidget(stopButton);
    connect(stopButton, SIGNAL(clicked()), this, SLOT(slotStopUpdater()));

    mainToolBar->addSeparator();

    prevButton = new QPushButton(tr("prev"));
    mainToolBar->addWidget(prevButton);
    connect(prevButton, SIGNAL(clicked()), this, SLOT(slotViewPrevObj()));

    nextButton = new QPushButton(tr("next"));
    mainToolBar->addWidget(nextButton);
    connect(nextButton, SIGNAL(clicked()), this, SLOT(slotViewNextObj()));

    mainToolBar->addSeparator();

    viewSettButton = new QPushButton(tr("sett"));
    mainToolBar->addWidget(viewSettButton);
    connect(viewSettButton, SIGNAL(clicked()), this, SLOT(slotViewSettWindow()));


    connect(mainTable->horizontalHeader(),SIGNAL(sectionClicked(int)),
                  this,SLOT(slotHeaderClicked(int)));


    settDock = new QDockWidget("Settings", this, Qt::Widget);
    settDock->setFloating(1);

    settDock->setMinimumSize(400, 300);
    settW = new settWindow(settDock);
    settDock->setWidget(settW);
    addDockWidget(Qt::BottomDockWidgetArea, settDock);

    //settDock->setVisible(0);
    viewMenu->addAction(settDock->toggleViewAction());
    settDock->hide();

    infoDock = new QDockWidget("Information", this, Qt::Widget);
    infoDock->setMinimumSize(100, 200);
    infoDock->setFloating(1);
    infoWidget = new QWidget(infoDock);

    QGridLayout *vertLA = new QGridLayout(infoWidget);

    QLabel *nLab = new QLabel("Name");
    vertLA->addWidget(nLab, 0, 0);
    nameLabel = new QLabel("name");
    vertLA->addWidget(nameLabel, 0, 1);

    QLabel *raLab = new QLabel("RA");
    vertLA->addWidget(raLab, 1, 0);
    raLabel = new QLabel("ra");
    vertLA->addWidget(raLabel, 1, 1);

    QLabel *decLab = new QLabel("DEC");
    vertLA->addWidget(decLab, 2, 0);
    decLabel = new QLabel("dec");
    vertLA->addWidget(decLabel, 2, 1);

    QLabel *mLab = new QLabel("Mang");
    vertLA->addWidget(mLab, 3, 0);
    magnLabel = new QLabel("magn");
    vertLA->addWidget(magnLabel, 3, 1);

    QLabel *mraLab = new QLabel("mu_RA");
    vertLA->addWidget(mraLab, 4, 0);
    muraLabel = new QLabel("mu_ra");
    vertLA->addWidget(muraLabel, 4, 1);

    QLabel *mdecLab = new QLabel("mu_DEC");
    vertLA->addWidget(mdecLab, 5, 0);
    mudecLabel = new QLabel("mu_dec");
    vertLA->addWidget(mudecLabel, 5, 1);

    QLabel *eLab = new QLabel("Exp");
    vertLA->addWidget(eLab, 6, 0);
    expLabel = new QLabel("exp");
    vertLA->addWidget(expLabel, 6, 1);

    QLabel *tsLab = new QLabel("Tasks");
    vertLA->addWidget(tsLab, 7, 0);
    tasksLabel = new QLabel("tasks");
    vertLA->addWidget(tasksLabel, 7, 1);

    infoDock->setWidget(infoWidget);
    addDockWidget(Qt::RightDockWidgetArea, infoDock);

}

void MainWindow::slotOpenResFileWindow()
{
    QString resFileName = QFileDialog::getOpenFileName(
                                    this,
                                    tr("Choose file"),
                                    "./",
                                    "lst (*.lst)");
    rFile.init(resFileName);


}

void MainWindow::slotInitResTable()
{
    int i, sz;
    QTableWidgetItem *newItem;
    sz = rFile.size();
    resRecord *rRec;
    for(i=0; i<sz; i++)
    {
        mainTable->insertRow(i);

        rRec = rFile.at(i);
        newItem = new QTableWidgetItem();

        newItem = new QTableWidgetItem(QString("%1").arg(rRec->name));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 0, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(mas_to_hms(grad_to_mas(rRec->ra), " ", 1)));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 1, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(mas_to_damas(grad_to_mas(rRec->dec), " ", 2)));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 2, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(rRec->magn));//, 5, 'f', 1, QLatin1Char('0')));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 3, newItem);

        newItem = new QTableWidgetItem(QString("0"));//, 5, 'f', 1, QLatin1Char('0')));
        newItem->setTextAlignment(Qt::AlignCenter);
        mainTable->setItem(i, 4, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(rRec->muRacosD));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 5, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(rRec->muDec));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 6, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(rRec->exp));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 7, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(rRec->tasks.join("|")));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 8, newItem);
    }

    slotUpdateTable();
    slotUpdateTime();
    for(i=0; i<sz; i++)
    {
        if(mainTable->isRowHidden(i)) continue;
        mainTable->selectRow(i);
        break;
    }
}

void MainWindow::slotHeaderClicked(int colNum)
{
    //QMessageBox::information(0, "headerClicked", QString("header num:%1").arg(colNum));

    //mainTable->sortByColumn(colNum);
//    mainTable->ro
    if(updatesEnabled())tabUpd->stop();
    timeUpd->stop();


    switch(colNum)
    {
    case 0:
        mainTable->sortByColumn(colNum);
        break;
    case 1:
        sortRa();
        break;
    case 2:
        sortDec();
        break;
    case 3:
        sortMagn();
        break;
    }

    if(updatesEnabled())tabUpd->start();
    timeUpd->start();

}

void MainWindow::setSettings()
{
    //settWindow settW;
    sett = new QSettings("./epha.ini", QSettings::IniFormat);

    if(sett==NULL) return;

//general
    settW->expSpinBox->setValue(sett->value("general/expTime", 0.1).toInt());
    settW->orderCombo->setCurrentIndex(sett->value("general/orderType", 0).toInt());

//skyarea
    settW->isAutoRA->setChecked(sett->value("skyarea/isAutoRA", 0).toInt());
    settW->ra0SpinBox->setValue(sett->value("skyarea/minRa", 0.0).toDouble());
    settW->ra1SpinBox->setValue(sett->value("skyarea/maxRa", 24.0).toDouble());
    settW->dec0SpinBox->setValue(sett->value("skyarea/minDec", -90.0).toDouble());
    settW->dec1SpinBox->setValue(sett->value("skyarea/maxDec", 90.0).toDouble());
    settW->magn0SpinBox->setValue(sett->value("skyarea/minMagn", -10.0).toDouble());
    settW->magn1SpinBox->setValue(sett->value("skyarea/maxMagn", 25.0).toDouble());


    //settDock->show();
}

void MainWindow::saveSettings()
{
   if(sett==NULL) return;

//general
   sett->setValue("general/expTime", settW->expSpinBox->value());
   sett->setValue("skyarea/isAutoRA", settW->orderCombo->currentIndex());

//skyarea
   sett->setValue("skyarea/isAutoRA", (int) settW->isAutoRA->isChecked());
   sett->setValue("skyarea/minRa", settW->ra0SpinBox->value());
   sett->setValue("skyarea/maxRa", settW->ra1SpinBox->value());
   sett->setValue("skyarea/meriDist", settW->dMeriSpinBox->value());
   sett->setValue("skyarea/minDec", settW->dec0SpinBox->value());
   sett->setValue("skyarea/maxDec", settW->dec1SpinBox->value());
   sett->setValue("skyarea/minMagn", settW->magn0SpinBox->value());
   sett->setValue("skyarea/maxMagn", settW->magn1SpinBox->value());

   sett->sync();
}

void MainWindow::slotViewSettWindow()
{
    //QMessageBox::information(0,"debug","view sett",QMessageBox::Ok);
    if(settDock->isVisible())settDock->hide();
    else settDock->show();
}

void MainWindow::slotViewNextObj()
{
    QList <QTableWidgetItem*> itemsSel;
    itemsSel = mainTable->selectedItems();
    QModelIndex curIndexItem;
    int curRow, i, sz;
    sz = mainTable->rowCount();
    curIndexItem = mainTable->currentIndex();
    curRow = curIndexItem.row();

    //QMessageBox::information(0, "slotViewNextObj", QString("curRow:%1").arg(curRow));
    for(i=curRow+1; i<sz; i++)
    {
        if(mainTable->isRowHidden(i)) continue;
        mainTable->selectRow(i);
        break;
    }
}

void MainWindow::slotViewPrevObj()
{
    //int p = itemList.size();
    //if(p<2) return;
    /*QString rStr;
    QStringList indList;
    for(int i=0; i<itemList.size();i++) indList << QString("%1").arg(itemList.at(i)->row());
    rStr = QString("%1\n").arg(indList.join(","));
*/
    int p, i, sz;
    sz = itemList.size();
    if(sz<1) return;

    p=-1;
    for(i=0; i<sz; i++)
    {
        if(mainTable->isRowHidden(itemList.at(i)->row())) continue;
        p = itemList.last()->row();
        break;
    }
    if(p==-1) return;

    mainTable->selectRow(p);
    itemList.removeLast();
    itemList.removeLast();
/*
    itemList.clear();
    for(int i=0; i<itemList.size();i++) indList << QString("%1").arg(itemList.at(i)->row());
    rStr.append(QString("%1\n").arg(indList.join(",")));

    QMessageBox::information(0, "headerClicked", QString("header num:%1").arg(rStr));*/
}

void MainWindow::sortRa()
{
    rFile.sortT(s);
    itemList.clear();
    mainTable->clear();
    mainTable->setRowCount(0);
    slotInitResTable();
}

void MainWindow::sortDec()
{
    rFile.sortDec();
    itemList.clear();
    mainTable->clear();
    mainTable->setRowCount(0);
    slotInitResTable();
}

void MainWindow::sortMagn()
{
    rFile.sortMagn();
    itemList.clear();
    mainTable->clear();
    mainTable->setRowCount(0);
    slotInitResTable();
}

void MainWindow::slotUpdateTime()
{
    //QMessageBox::information(0,"debug","ok",QMessageBox::Ok);//
    //double jDay, lam, s;
    int i, sz, cs, res;
    double ra, dec, magn;
    double minRa, maxRa;
    double minDec, maxDec;
    double minMagn, maxMagn;
    double meriDist;
    int expTime;

    QString raStr;
    int yr, mth, day, hr, min, sec;
    QTableWidgetItem *newItem;
    lam = sa.obs_pos->obs->record->Long;

    sysTime = QTime().currentTime();
    timeStr = sysTime.toString("HH mm ss");
    hr = timeStr.section(" ", 0, 0).toInt();
    min = timeStr.section(" ", 1, 1).toInt();
    sec = timeStr.section(" ", 2, 2).toInt();
    QDate sysDate = QDate().currentDate();
    QString dateStr = sysDate.toString("yyyy M dd");
    yr = dateStr.section(" ", 0, 0).toInt();
    mth = dateStr.section(" ", 1, 1).toInt();
    day = dateStr.section(" ", 2, 2).toInt();
    dat2JD_time(&jDay, yr, mth, day, hr, min, sec);
    jdTimeStr = QString("%1 | %2").arg(jDay, 13, 'f', 5).arg(sysDate.toJulianDay());

    UTC2s(jDay, lam, &s);
    starTimeStr = getStrFromS(s*86400.0/(2.0*PI), ":", 0);

    slotStatBarUpdate();

    if(settW->isAutoRA->isChecked())
    {
        minRa = rad2grad(s)/15.0 - settW->dMeriSpinBox->value();
        maxRa = rad2grad(s)/15.0 + settW->dMeriSpinBox->value();
        if(minRa<0.0) minRa+=24;
        if(maxRa>24.0) minRa-=24;
    }
    else
    {
        minRa = settW->ra0SpinBox->value();
        maxRa = settW->ra1SpinBox->value();
    }

    minDec = settW->dec0SpinBox->value();
    maxDec = settW->dec1SpinBox->value();

    minMagn = settW->magn0SpinBox->value();
    maxMagn = settW->magn1SpinBox->value();




    sz = mainTable->rowCount();
    for(i=0;i<sz;i++)
    {


        res = 0;
        cs = 0;
        if(maxRa<=minRa) cs = 1;

        raStr = mainTable->item(i, 1)->text();
        ra = mas_to_grad(hms_to_mas(raStr, " "))/15.0;
        raStr = mainTable->item(i, 2)->text();
        dec = mas_to_grad(damas_to_mas(raStr, " "));
        magn = mainTable->item(i, 3)->text().toDouble();

        meriDist = ra - rad2grad(s)/15.0;
        if(meriDist<-12) meriDist+=24;
        if(meriDist>12) meriDist-=24;

        newItem = mainTable->item(i, 4);
        newItem->setText(QString("%1").arg(meriDist));

        res = (dec<=maxDec)&&(dec>=minDec)&&(magn<=maxMagn)&&(magn>=minMagn);

        if(cs) res *= (ra<maxRa)||(ra>minRa);
        else res *= (ra<maxRa)&&(ra>minRa);
        mainTable->setRowHidden(i, !res);
    }


    if(updaterEnabled)
    {
        expTime = settW->expSpinBox->value()*1000;
        //qDebug() << QString("%1:%2\n").arg(tabEla->elapsed()).arg(expTime);
        ui->expProgBar->setValue(100.0*tabEla->elapsed()/(expTime*1.0));
    }

    timeUpd->start(1000);
}

void MainWindow::slotUpdateTable()
{
    //QMessageBox::information(0,"debug","ok",QMessageBox::Ok);//
//    double jDay, lam, s;

    int i;
    int sz = rFile.recList.size();
    double ra, dec, dT;
    dT = jDay - rFile.jdTime;
    resRecord *rRec;
    QTableWidgetItem *newItem;
    for(i=0; i<sz; i++)
    {
        rRec = rFile.at(i);
        dec = rRec->dec + mas_to_grad(rRec->muDec*86400.0*dT);
        ra = rRec->ra + mas_to_grad(rRec->muRacosD/cos(grad2rad(dec))*86400.0*dT);

        newItem = mainTable->item(i, 1);
        newItem->setText(QString("%1").arg(mas_to_hms(grad_to_mas(ra), " ", 1)));
        newItem = mainTable->item(i, 2);
        newItem->setText(QString("%1").arg(mas_to_damas(grad_to_mas(dec), " ", 2)));
    }


    if(updaterEnabled)
    {
        slotViewNextObj();
        tabUpd->start(settW->expSpinBox->value()*1000);
        tabEla->restart();
    }


}

void MainWindow::slotClearTable()
{
    mainTable->clearContents();
    mainTable->setRowCount(0);
}


void MainWindow::slotStatBarUpdate()
{
        //statusBar()->showMessage(timeStr);
        sysTimeEdit->setText(timeStr);
        jdTimeEdit->setText(jdTimeStr);
        starTimeEdit->setText(starTimeStr);
}

void MainWindow::slotStartUpdater()
{
    updaterEnabled = 1;
    if(!tabUpd->isActive())
    {
        tabUpd->start(settW->expSpinBox->value()*1000);
        tabEla->start();
    }
}

void MainWindow::slotStopUpdater()
{
    if(tabUpd->isActive()) tabUpd->stop();
    ui->expProgBar->setValue(0.0);
    updaterEnabled = 0;
}

void MainWindow::on_tableWidget_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    //QMessageBox::information(0, "headerClicked", QString("header num:%1").arg(current->row()));
    if(previous!=NULL) itemList << previous;
    nameLabel->setText(mainTable->item(current->row(), 0)->text());
    raLabel->setText(mainTable->item(current->row(), 1)->text());
    decLabel->setText(mainTable->item(current->row(), 2)->text());
    magnLabel->setText(mainTable->item(current->row(), 3)->text());
    muraLabel->setText(mainTable->item(current->row(), 5)->text());
    mudecLabel->setText(mainTable->item(current->row(), 6)->text());
    expLabel->setText(mainTable->item(current->row(), 7)->text());
    tasksLabel->setText(mainTable->item(current->row(), 8)->text());
}

void MainWindow::on_tableWidget_itemActivated(QTableWidgetItem *item)
{
    //if(item!=NULL) itemList << item;
}

void MainWindow::slotExit()//auoia ec i?eei?aiey
{
    //QMessageBox::information(0, "slotExit", QString("slotExit"));
    //qDebug() << "exit\n";
    //slotSaveDefaultPanels();
    //slotSaveSettings();
    saveSettings();
    QApplication::exit();
};

void MainWindow::closeEvent(QCloseEvent *event)
 {
    slotExit();

 }
