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
    sysTime = QTime().currentTime();
    timeStr = sysTime.toString("HH mm ss");
    slotStatBarUpdate();
    updater->start(1000);
}

void MainWindow::slotStatBarUpdate()
{
        //statusBar()->showMessage(timeStr);
        sysTimeEdit->setText(timeStr);
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
    sysTimeEdit->setMaximumWidth(50);
    statusBar()->addWidget(sysTimeEdit);

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
