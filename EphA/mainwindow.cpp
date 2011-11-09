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
    updater = new QTimer(this);
    connect(updater, SIGNAL(timeout()), this, SLOT(slotUpdateTable()));
    updater->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotUpdateTable()
{
    //QMessageBox::information(0,"debug","ok",QMessageBox::Ok);//
    double jDay;
    int yr, mth, day, hr, min, sec;

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


    slotStatBarUpdate();
    updater->start(1000);
}

void MainWindow::slotStatBarUpdate()
{
        //statusBar()->showMessage(timeStr);
        sysTimeEdit->setText(timeStr);
        jdTimeEdit->setText(jdTimeStr);
}

void MainWindow::setMenu()
{
    fileMenu = menuBar()->addMenu(tr("file"));
    viewSettAct = new QAction(tr("&view sett"), this);
    viewSettAct->setShortcut(tr("Ctrl+O"));//iacia?aai "ai?y?o? eeaaeoo" Ctrl+O aey aaenoaey open fits
    viewSettAct->setStatusTip(tr("view settings"));// iacia?aai "iianeaceo"
    fileMenu->addAction(viewSettAct);
    connect(viewSettAct, SIGNAL(triggered()), this, SLOT(slotViewSettWindow()));
}

void MainWindow::setWidgets()
{
    sysTimeEdit = new QLineEdit(this);
    sysTimeEdit->setMaximumWidth(100);
    statusBar()->addWidget(sysTimeEdit);

    jdTimeEdit = new QLineEdit(this);
    jdTimeEdit->setMaximumWidth(200);
    statusBar()->addWidget(jdTimeEdit);

    starTimeEdit = new QLineEdit(this);
    starTimeEdit->setMaximumWidth(100);
    statusBar()->addWidget(starTimeEdit);

    mainToolBar = qFindChild<QToolBar*>(this, "mainToolBar");

    settButton = new QPushButton(tr("sett"));
    mainToolBar->insertWidget(viewSettAct, settButton);
    connect(settButton, SIGNAL(clicked()), this, SLOT(slotViewSettWindow()));
}

void MainWindow::setSettings()
{

}

void MainWindow::slotViewSettWindow()
{
    QMessageBox::information(0,"debug","view sett",QMessageBox::Ok);
}
