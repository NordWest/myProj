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

//    ephthread ephTabUpdater(this);
 //   ephTabUpdater.start();



    timeUpd = new QTimer(this);
    connect(timeUpd, SIGNAL(timeout()), this, SLOT(slotUpdateTime()));
    tabUpd = new QTimer(this);
    connect(tabUpd, SIGNAL(timeout()), this, SLOT(slotUpdateTable()));

    updaterEnabled = 0;
    expTime = 1000*10;
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

    viewSettAct = new QAction(tr("&view sett"), this);
    viewSettAct->setShortcut(tr("Ctrl+U"));
    viewSettAct->setStatusTip(tr("view settings"));
    fileMenu->addAction(viewSettAct);
    connect(viewSettAct, SIGNAL(triggered()), this, SLOT(slotViewSettWindow()));

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

}

void MainWindow::setWidgets()
{
//    mainTree = new QTreeWidget(this);
//    mainTree = qFindChild<QTreeWidget*>(this, "treeWidget");
//    mainTree->setColumnCount();


    mainTable = qFindChild<QTableWidget*>(this, "tableWidget");
    mainTable->setColumnCount(8);
    QStringList vhLabelsT;
    vhLabelsT << QString(tr("Name"));
    vhLabelsT << QString(tr("RA"));
    vhLabelsT << QString(tr("DEC"));
    vhLabelsT << QString(tr("Magn"));
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
/*
    starTimeEdit = new QLineEdit(this);
    starTimeEdit->setMaximumWidth(100);
    starTimeEdit->setReadOnly(1);
    statusBar()->addWidget(starTimeEdit);
*/
    mainToolBar = qFindChild<QToolBar*>(this, "mainToolBar");

    prevButton = new QPushButton(tr("prev"));
    mainToolBar->insertWidget(viewPrevAct, prevButton);
    connect(prevButton, SIGNAL(clicked()), this, SLOT(slotViewPrevObj()));

    nextButton = new QPushButton(tr("next"));
    mainToolBar->insertWidget(viewNextAct, nextButton);
    connect(nextButton, SIGNAL(clicked()), this, SLOT(slotViewNextObj()));

    startButton = new QPushButton(tr("start"));
    mainToolBar->insertWidget(viewStartAct, startButton);
    connect(startButton, SIGNAL(clicked()), this, SLOT(slotStartUpdater()));

    stopButton = new QPushButton(tr("stop"));
    mainToolBar->insertWidget(viewStopAct, stopButton);
    connect(stopButton, SIGNAL(clicked()), this, SLOT(slotStopUpdater()));

    connect(mainTable->horizontalHeader(),SIGNAL(sectionClicked(int)),
                  this,SLOT(slotHeaderClicked(int)));

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

        newItem = new QTableWidgetItem(QString("%1").arg(rRec->mu_ra));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 4, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(rRec->mu_dec));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 5, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(rRec->exp));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 6, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(rRec->tasks.join("|")));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 7, newItem);
    }
}

void MainWindow::slotHeaderClicked(int colNum)
{
    //QMessageBox::information(0, "headerClicked", QString("header num:%1").arg(colNum));

    mainTable->sortByColumn(colNum);
//    mainTable->ro
/*
    switch(colNum)
    {
    case 0:
        mainTable->sortByColumn(colNum);
        break;
    case 1:
        mainTable->sortByColumn(colNum);
        break;
    case 1:
        mainTable->sortByColumn(colNum);
        break;
    case 1:
        mainTable->sortByColumn(colNum);
        break;
    }
*/

}

void MainWindow::setSettings()
{

}

void MainWindow::slotViewSettWindow()
{
    QMessageBox::information(0,"debug","view sett",QMessageBox::Ok);
}

void MainWindow::slotViewNextObj()
{
    QList <QTableWidgetItem*> itemsSel;
    itemsSel = mainTable->selectedItems();
    QModelIndex curIndexItem;
    int curRow;
    curIndexItem = mainTable->currentIndex();
    curRow = curIndexItem.row();

    mainTable->selectRow(curRow+1);
}

void MainWindow::slotViewPrevObj()
{
    //int p = itemList.size();
    //if(p<2) return;
    QString rStr;
    QStringList indList;
    for(int i=0; i<itemList.size();i++) indList << QString("%1").arg(itemList.at(i)->row());
    rStr = QString("%1\n").arg(indList.join(","));

    int p = itemList.last()->row();
    if(itemList.size()<1) return;
    itemList.removeLast();
    mainTable->selectRow(p);
    itemList.removeLast();

    itemList.clear();
    for(int i=0; i<itemList.size();i++) indList << QString("%1").arg(itemList.at(i)->row());
    rStr.append(QString("%1\n").arg(indList.join(",")));

    QMessageBox::information(0, "headerClicked", QString("header num:%1").arg(rStr));
}

void MainWindow::slotUpdateTime()
{
    //QMessageBox::information(0,"debug","ok",QMessageBox::Ok);//
    //double jDay, lam, s;
    int yr, mth, day, hr, min, sec;
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
    starTimeStr = getStrFromS(s, ":", 0);

    slotStatBarUpdate();

    if(updaterEnabled)
    {
        qDebug() << QString("%1:%2\n").arg(tabEla->elapsed()).arg(expTime);
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
        ra = rRec->ra + mas_to_grad(rRec->mu_ra*86400.0*dT);
        dec = rRec->dec + mas_to_grad(rRec->mu_dec*86400.0*dT);

        newItem = mainTable->item(i, 1);
        newItem->setText(QString("%1").arg(mas_to_hms(grad_to_mas(ra), " ", 1)));
        newItem = mainTable->item(i, 2);
        newItem->setText(QString("%1").arg(mas_to_damas(grad_to_mas(dec), " ", 2)));
    }


    if(updaterEnabled)
    {
        tabUpd->start(expTime);
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
        //starTimeEdit->setText(starTimeStr);
}

void MainWindow::slotStartUpdater()
{
    updaterEnabled = 1;
    if(!tabUpd->isActive())
    {
        tabUpd->start(expTime);
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
}

void MainWindow::on_tableWidget_itemActivated(QTableWidgetItem *item)
{
    //if(item!=NULL) itemList << item;
}
