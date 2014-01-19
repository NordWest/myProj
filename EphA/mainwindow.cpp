#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setlocale(LC_NUMERIC, "C");

    setWindowTitle("EphA");
    datetimeStr = "";

    logFile.setFileName("./epha.log");
    logFile.open(QFile::WriteOnly | QFile::Truncate);
    logStm.setDevice(&logFile);

    QSettings sett("./saGrade.ini", QSettings::IniFormat);

    QString mpcCatName = sett.value("general/mpcCatName", "mpcorb.dat").toString();
    QString deleCatName = sett.value("general/deleCatName", "dele.cat").toString();
    QString obsCatName = sett.value("general/obsCatName", "Obs.txt").toString();
    QString installDir = sett.value("general/installDir", "./").toString();
    QString obsCode = sett.value("general/obsCode", "084").toString();

    procData miriadeProcData;
    miriadeProcData.name = sett.value("miriadeProcData/name", "./mpeph.exe").toString();
    miriadeProcData.folder = sett.value("miriadeProcData/folder", "./").toString();
    miriadeProcData.waitTime = sett.value("miriadeProcData/waitTime", -1).toInt();
    if(miriadeProcData.waitTime>0) miriadeProcData.waitTime *= 1000;

    sa.init(obsCatName, deleCatName, installDir);
    sa.obs_pos->set_obs_parpam(GEOCENTR_NUM, CENTER_SUN, SK_EKVATOR, obsCode.toAscii().data());

    setMenu();
    setWidgets();
    setSettings();

    timeUpd = new QTimer(this);
    connect(timeUpd, SIGNAL(timeout()), this, SLOT(slotUpdateTime()));
    tabUpd = new QTimer(this);
    connect(tabUpd, SIGNAL(timeout()), this, SLOT(slotUpdateTable()));

    updaterEnabled = 0;
    ui->expProgBar->setRange(0, 100);
    ui->expProgBar->setValue(0);

    timeUpd->setSingleShot(0);
    slotUpdateTime();

    tabEla = new QElapsedTimer;

    rFile.init("./res.lst");
    slotInitResTable();
    slotUpdateTable();

    slotViewSettWindow();

//Networking

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }
        networkSession = new QNetworkSession(config, this);
        connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

        //statusLabel->setText(tr("Opening network session."));
        networkSession->open();
    } else {
        sessionOpened();
    }

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(sendNextObject()));

    slotGrade();
}

void MainWindow::sessionOpened()
{
    // Save the used configuration
    /*
    if (networkSession) {
        QNetworkConfiguration config = networkSession->configuration();
        QString id;
        if (config.type() == QNetworkConfiguration::UserChoice)
            id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
        else
            id = config.identifier();

        QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
        settings.endGroup();
    }
*/
//! [0] //! [1]
    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::LocalHost, 3467)) {
        QMessageBox::critical(this, tr("Fortune Server"),
                              tr("Unable to start the server: %1.")
                              .arg(tcpServer->errorString()));
        close();
        return;
    }

//! [0]
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    //statusLabel->setText(tr("The server is running on\n\nIP: %1\nport: %2\n\n"
                            //"Run the Fortune Client example now.")
                         //.arg(ipAddress).arg(tcpServer->serverPort()));
//! [1]
}

//! [4]
void MainWindow::sendCurObject()
{
//! [5]
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
//! [4] //! [6]
    out << (quint16)0;
    out << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(sysTimeEdit->text()).arg(nameLabel->text(), -16).arg(raLabel->text()).arg(decLabel->text()).arg(magnLabel->text()).arg(expLabel->text()).arg(tasksLabel->text()).arg(catNameLabel->text());
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
//! [6] //! [7]

    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));
//! [7] //! [8]

    clientConnection->write(block);
    clientConnection->disconnectFromHost();
//! [5]
}

void MainWindow::sendNextObject()
{
    slotViewNextObj();
//! [5]
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
//! [4] //! [6]
    out << (quint16)0;
    out << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(sysTimeEdit->text()).arg(nameLabel->text(), -16).arg(raLabel->text()).arg(decLabel->text()).arg(magnLabel->text()).arg(expLabel->text()).arg(tasksLabel->text()).arg(catNameLabel->text());
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
//! [6] //! [7]

    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));
//! [7] //! [8]

    clientConnection->write(block);
    clientConnection->disconnectFromHost();
//! [5]
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

    saveResAct = new QAction(tr("&Save res file"), this);
    saveResAct->setShortcut(tr("Ctrl+S"));
    saveResAct->setStatusTip(tr("save res file"));
    fileMenu->addAction(saveResAct);
    connect(saveResAct, SIGNAL(triggered()), this, SLOT(slotSaveResFileWindow()));

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

    clearAct = new QAction(tr("&clear table"), this);
    //clearAct->setShortcut(tr("Ctrl+n"));//
    clearAct->setStatusTip(tr("clear table"));//
    viewMenu->addAction(clearAct);
    connect(clearAct, SIGNAL(triggered()), this, SLOT(slotClearTable()));

    add2seqAtc = new QAction(tr("&Add to seq list"), this);
    viewMenu->addAction(add2seqAtc);
    connect(add2seqAtc, SIGNAL(triggered()), this, SLOT(slotAdd2Seq()));

    instrMenu = menuBar()->addMenu(tr("Instruments"));
    gradeAct = new QAction(tr("&grade"), this);
    gradeAct->setShortcut(tr("Ctrl+g"));//
    gradeAct->setStatusTip(tr("grade"));//
    instrMenu->addAction(gradeAct);
    connect(gradeAct, SIGNAL(triggered()), this, SLOT(slotGrade()));

    updLocCatsAct = new QAction(tr("&Update Local Cats"), this);
    updLocCatsAct->setStatusTip(tr("Update Local Cats"));//
    instrMenu->addAction(updLocCatsAct);
    connect(updLocCatsAct, SIGNAL(triggered()), this, SLOT(slotUpdLocCats()));
}

void MainWindow::setWidgets()
{
    mainTable = qFindChild<QTableWidget*>(this, "tableWidget");
    mainTable->setColumnCount(11);
    QStringList vhLabelsT;
    vhLabelsT << QString(tr("   "));
    vhLabelsT << QString(tr("Name"));
    vhLabelsT << QString(tr("RA"));
    vhLabelsT << QString(tr("DEC"));
    vhLabelsT << QString(tr("Magn"));
    vhLabelsT << QString(tr("Tasks"));
    vhLabelsT << QString(tr("t"));
    vhLabelsT << QString(tr("mu_ra"));
    vhLabelsT << QString(tr("mu_dec"));
    vhLabelsT << QString(tr("Exp"));
    vhLabelsT << QString(tr("Catalog"));

    mainTable->setHorizontalHeaderLabels(vhLabelsT);
    mainTable->setColumnWidth(0, 20);
    mainTable->setColumnWidth(1, 200);
    mainTable->setColumnWidth(2, 150);
    mainTable->setColumnWidth(3, 150);

    mainTable->setContextMenuPolicy(Qt::ActionsContextMenu);
    mainTable->addAction(add2seqAtc);

    sysTimeEdit = new QLineEdit(this);
    sysTimeEdit->setMaximumWidth(150);
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

    mainToolBar->addSeparator();

    quitButton = new QPushButton(tr("Quit"));
    quitButton->setIcon(QIcon("icons/quit.jpg"));
    mainToolBar->addWidget(quitButton);
    connect(quitButton, SIGNAL(clicked()), this, SLOT(slotExit()));

    connect(mainTable->horizontalHeader(),SIGNAL(sectionClicked(int)),
                  this,SLOT(slotHeaderClicked(int)));

    settDock = new QDockWidget("Settings", this, Qt::Widget);

    settDock->setMinimumSize(400, 300);
    settW = new settWindow(settDock);
    settDock->setWidget(settW);
    settDock->setFloating(1);

    viewMenu->addAction(settDock->toggleViewAction());

    addDockWidget(Qt::NoDockWidgetArea, settDock);

    infoDock = new QDockWidget("Information", this, Qt::Widget);
    infoDock->setMinimumSize(100, 200);
    infoDock->setFloating(1);
    infoWidget = new QWidget(infoDock);

    QGridLayout *vertLA = new QGridLayout(infoWidget);

    QLabel *nLab = new QLabel("Name");
    vertLA->addWidget(nLab, 0, 0);
    nameLabel = new QLabel("name");
    vertLA->addWidget(nameLabel, 0, 1);
    nameLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QLabel *raLab = new QLabel("RA");
    vertLA->addWidget(raLab, 1, 0);
    raLabel = new QLabel("ra");
    vertLA->addWidget(raLabel, 1, 1);
    raLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QLabel *decLab = new QLabel("DEC");
    vertLA->addWidget(decLab, 2, 0);
    decLabel = new QLabel("dec");
    vertLA->addWidget(decLabel, 2, 1);
    decLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QLabel *mLab = new QLabel("Mang");
    vertLA->addWidget(mLab, 3, 0);
    magnLabel = new QLabel("magn");
    vertLA->addWidget(magnLabel, 3, 1);
    magnLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QLabel *mraLab = new QLabel("mu_RA");
    vertLA->addWidget(mraLab, 4, 0);
    muraLabel = new QLabel("mu_ra");
    vertLA->addWidget(muraLabel, 4, 1);
    muraLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QLabel *mdecLab = new QLabel("mu_DEC");
    vertLA->addWidget(mdecLab, 5, 0);
    mudecLabel = new QLabel("mu_dec");
    vertLA->addWidget(mudecLabel, 5, 1);
    mudecLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QLabel *eLab = new QLabel("Exp");
    vertLA->addWidget(eLab, 6, 0);
    expLabel = new QLabel("exp");
    vertLA->addWidget(expLabel, 6, 1);
    expLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QLabel *tsLab = new QLabel("Tasks");
    vertLA->addWidget(tsLab, 7, 0);
    tasksLabel = new QLabel("tasks");
    vertLA->addWidget(tasksLabel, 7, 1);
    tasksLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QLabel *catLab = new QLabel("Catalog");
    vertLA->addWidget(catLab, 8, 0);
    catNameLabel = new QLabel("Catalog");
    vertLA->addWidget(catNameLabel, 8, 1);
    catNameLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    vertLA->setRowStretch(9, 1);

    miriUpdateButton = new QPushButton(tr("Miriade update"));
    vertLA->addWidget(miriUpdateButton, 10, 0, 1, -1);
    miriUpdateButton->setEnabled(0);
    connect(miriUpdateButton, SIGNAL(clicked()), this, SLOT(slotUpdateMiri()));

    infoDock->setWidget(infoWidget);
    addDockWidget(Qt::RightDockWidgetArea, infoDock);
    infoDock->setFloating(0);
    viewMenu->addAction(infoDock->toggleViewAction());
}

void MainWindow::slotOpenResFileWindow()
{
    QString resFileName = QFileDialog::getOpenFileName(
                                    this,
                                    tr("Choose file"),
                                    "./",
                                    "lst (*.lst);;Text files (*.txt)");
    rFile.init(resFileName);


}

void MainWindow::slotSaveResFileWindow()
{
    QString resFileName = QFileDialog::getSaveFileName(
                                    this,
                                    tr("Choose file"),
                                    "./",
                                    "lst (*.lst)");
    rFile.saveAs(resFileName);
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
        newItem->setTextAlignment(Qt::AlignCenter);
        mainTable->setItem(i, 0, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(rRec->name));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 1, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(mas_to_hms(grad_to_mas(rRec->ra), " ", 1)));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 2, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(mas_to_damas(grad_to_mas(rRec->dec), " ", 2)));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 3, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(rRec->magn, 6, 'f', 2));//, 5, 'f', 1, QLatin1Char('0')));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 4, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(rRec->tasks.join("|")));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 5, newItem);

        newItem = new QTableWidgetItem(QString("0"));//, 5, 'f', 1, QLatin1Char('0')));
        newItem->setTextAlignment(Qt::AlignCenter);
        mainTable->setItem(i, 6, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(rRec->muRacosD, 6, 'f', 0));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 7, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(rRec->muDec, 6, 'f', 0));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 8, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(rRec->exp));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 9, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(rRec->catName.simplified()));
        newItem->setTextAlignment(Qt::AlignLeft);
        mainTable->setItem(i, 10, newItem);

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
    if(updatesEnabled())tabUpd->stop();
    timeUpd->stop();

    switch(colNum)
    {
    case 1:
        mainTable->sortByColumn(colNum);
        break;
    case 2:
        sortRa();
        break;
    case 3:
        sortDec();
        break;
    case 4:
        sortMagn();
        break;
    }

    if(updatesEnabled())tabUpd->start();
    timeUpd->start();

}

void MainWindow::setSettings()
{
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

//grade settings
    QSettings settG("./saGrade.ini", QSettings::IniFormat);

    mpcCatName = settG.value("general/mpcCatName", "mpcorb.dat").toString();
    deleCatName = settG.value("general/deleCatName", "dele.cat").toString();
    obsCatName = settG.value("general/obsCatName", "Obs.txt").toString();
    installDir = settG.value("general/installDir", "./").toString();
    obsCode = settG.value("general/obsCode", "084").toString();

    miriadeProcData.name = settG.value("miriadeProcData/name", "./mpeph.exe").toString();
    miriadeProcData.folder = settG.value("miriadeProcData/folder", "./").toString();
    miriadeProcData.waitTime = settG.value("miriadeProcData/waitTime", -1).toInt();
    if(miriadeProcData.waitTime>0) miriadeProcData.waitTime *= 1000;
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
    if(settDock->isHidden())settDock->show();
    else settDock->hide();
}

void MainWindow::slotViewNextObj()
{
    QList <QTableWidgetItem*> itemsSel;

//    timeUpd->stop();
//    tabUpd->stop();

    itemsSel = mainTable->selectedItems();

    QModelIndex curIndexItem;
    int curRow, i, sz, j;
    sz = mainTable->rowCount();
    curIndexItem = mainTable->currentIndex();
    curRow = curIndexItem.row();
    logListItem *curItem = new logListItem;
    curItem->item = itemsSel.at(0);
    curItem->timeStamp = QDateTime().currentDateTime();
    logItemList << curItem;
    //QMessageBox::information(0, "slotViewNextObj", QString("curRow:%1").arg(curRow));
    int ordType = settW->orderCombo->currentIndex();
    QList <int> indexes;
    if(ordType==0)
    {
        for(i=curRow+1; i<sz; i++)
        {
            if(mainTable->isRowHidden(i)) continue;
            mainTable->selectRow(i);
            return;
        }
    }

    int expLen, expRnd, expCur, taskNum;
    QString taskName;


        QString textAt;
        QStringList curTasks, tTasks;

        sz = mainTable->rowCount();
        for(i=0; i<sz; i++)
        {
            if(mainTable->isRowHidden(i)) continue;
            indexes << i;
            if(ordType==2)
            {
                tTasks = mainTable->item(i, 5)->text().split(",");
                for(j=0; j<tTasks.size(); j++)
                {
                    if(!curTasks.contains(tTasks.at(j))) curTasks << tTasks.at(j);
                }
            }
        }

        sz = logItemList.size();
        for(i=0; i<sz; i++)
        {
            indexes.removeOne(logItemList.at(i)->item->row());
        }

        if(ordType==2)
        {
            sz = indexes.size();
            for(i=0; i<sz; i++)
            {
                tTasks = mainTable->item(indexes.at(i), 5)->text().split(",");
                for(j=0; j<tTasks.size(); j++)
                {
                    if(!curTasks.contains(tTasks.at(j))) curTasks << tTasks.at(j);
                }
            }

            sz = curTasks.size();
            expLen =0;
            for(i=0; i<sz; i++)
            {
                expLen += sa.task_list.getTaskName(curTasks.at(i))->exp;
            }
            srand(time(NULL));
            expRnd = rand() % expLen;
            expLen = 0;
            for(i=0; i<sz; i++)
            {
                expCur = sa.task_list.getTaskName(curTasks.at(i))->exp;
                if((expLen+expCur)>expRnd) break;
                expLen += expCur;

            }
            taskName = curTasks.at(i);
            qDebug() << QString("taskName: %1").arg(taskName);

            if(QString().compare(taskName, "Planets")==0)
            {
                qDebug() << QString("taskName: %1").arg(taskName);
            }

            sz = indexes.size();
            for(i=sz-1; i>=0; i--)
            {
                if(!mainTable->item(indexes[i], 5)->text().split(",").contains(taskName)) indexes.removeAt(i);
            }
        }


        sz = indexes.size();
        expLen = 0;
        for(i=0; i<sz; i++)
        {
            textAt = mainTable->item(indexes.at(i), 9)->text();
            expLen += textAt.toInt();
        }

        srand(time(NULL));
        expRnd = rand() % expLen;

        expLen = 0;
        for(i=0; i<sz; i++)
        {

            expCur = mainTable->item(indexes.at(i), 9)->text().toInt();
            if((expCur+expLen)>expRnd)
            {
                qDebug() << QString("selectRow: %1\n").arg(indexes.at(i));
                mainTable->selectRow(indexes.at(i));
                break;
            }
            expLen += expCur;
        }

//        timeUpd->start();
//        tabUpd->start();

}

void MainWindow::slotViewPrevObj()
{
    int p, i, sz;
    sz = logItemList.size();
    if(sz<1) return;

    p=-1;
    for(i=0; i<sz; i++)
    {
        if(mainTable->isRowHidden(logItemList.at(i)->item->row())) continue;
        p = logItemList.last()->item->row();
        break;
    }
    if(p==-1) return;

    mainTable->selectRow(p);
    logItemList.removeLast();
}

void MainWindow::sortRa()
{
    rFile.sortT(s);
    logItemList.clear();
    mainTable->clear();
    mainTable->setRowCount(0);
    slotInitResTable();
}

void MainWindow::sortDec()
{
    rFile.sortDec();
    logItemList.clear();
    mainTable->clear();
    mainTable->setRowCount(0);
    slotInitResTable();
}

void MainWindow::sortMagn()
{
    rFile.sortMagn();
    logItemList.clear();
    mainTable->clear();
    mainTable->setRowCount(0);
    slotInitResTable();
}

void MainWindow::slotUpdateTime()
{
    int i, sz, cs, res;
    double ra, dec, magn;
    double minRa, maxRa;
    double minDec, maxDec;
    double minMagn, maxMagn;
    double meriDist;
    int expTime;

    QString raStr;
    QTableWidgetItem *newItem;
    lam = sa.obs_pos->obs->record->Long;

    sysDateTime = QDateTime().currentDateTime();
    datetimeStr = sysDateTime.toString("yyyy-MM-dd HH:mm:ss");

    dat2JD_time(&jDay, sysDateTime.date().year(), sysDateTime.date().month(), sysDateTime.date().day(), sysDateTime.time().hour(), sysDateTime.time().minute(), sysDateTime.time().second());
    jdTimeStr = QString("%1 | %2").arg(jDay, 13, 'f', 5).arg(sysDateTime.date().toJulianDay());

    sa.initVisualProp(jDay);

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

        raStr = mainTable->item(i, 2)->text();
        ra = mas_to_grad(hms_to_mas(raStr, " "))/15.0;
        raStr = mainTable->item(i, 3)->text();
        dec = mas_to_grad(damas_to_mas(raStr, " "));
        magn = mainTable->item(i, 4)->text().toDouble();

        meriDist = ra - rad2grad(s)/15.0;
        if(meriDist<-12) meriDist+=24;
        if(meriDist>12) meriDist-=24;

        newItem = mainTable->item(i, 6);
        newItem->setText(QString("%1").arg(meriDist, 6, 'f', 2));

        res = (dec<=maxDec)&&(dec>=minDec)&&(magn<=maxMagn)&&(magn>=minMagn);

        if(cs) res *= (ra<maxRa)||(ra>minRa);
        else res *= (ra<maxRa)&&(ra>minRa);
        mainTable->setRowHidden(i, !res);
    }

    if(updaterEnabled)
    {
        expTime = settW->expSpinBox->value()*1000;
        ui->expProgBar->setValue(100.0*tabEla->elapsed()/(expTime*1.0));
    }

    timeUpd->start(10000);
}

void MainWindow::slotUpdateTable()
{
    int i;
    int sz = rFile.recList.size();
    double ra, dec, dT;
    dT = jDay - rFile.jdUTC;
    resRecord *rRec;
    QTableWidgetItem *newItem;
    for(i=0; i<sz; i++)
    {
        rRec = rFile.at(i);
        dec = rRec->dec + mas_to_grad(rRec->muDec*86400.0*dT);
        ra = rRec->ra + mas_to_grad(rRec->muRacosD/cos(grad2rad(dec))*86400.0*dT);
        rRec->getRaDec(dT, ra, dec);

        newItem = mainTable->item(i, 2);
        newItem->setText(QString("%1").arg(mas_to_hms(grad_to_mas(ra), " ", 1)));
        newItem = mainTable->item(i, 3);
        newItem->setText(QString("%1").arg(mas_to_damas(grad_to_mas(dec), " ", 2)));
    }

    if(updaterEnabled)
    {
        slotViewNextObj();
        tabUpd->start(settW->expSpinBox->value()*1000);
        tabEla->restart();
    }
}

void MainWindow::slotGrade()
{
    sa.set_opt(settW->ra0SpinBox->value(), settW->ra0SpinBox->value(), settW->dec0SpinBox->value(), settW->dec1SpinBox->value(), settW->magn0SpinBox->value(), settW->magn1SpinBox->value());
    slotClearTable();
    sa.grade(rFile);
    slotInitResTable();
}

void MainWindow::slotUpdLocCats()
{
    sa.updateLocalCats();
    slotGrade();
}

void MainWindow::slotClearTable()
{
    mainTable->setRowCount(0);
    mainTable->clearContents();
//
    logItemList.clear();
    rFile.clear();
}

void MainWindow::slotAdd2Seq()
{
}

void MainWindow::slotStatBarUpdate()
{
        sysTimeEdit->setText(datetimeStr);
        jdTimeEdit->setText(jdTimeStr);
        starTimeEdit->setText(QString("s: %1").arg(starTimeStr));
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

void MainWindow::slotUpdateMiri()
{
    QProcess outerProcess;
    QStringList outerArguments;
    QString objDataStr, sJD;
    mpephRec mpephR;

    outerArguments.clear();

    outerArguments << QString("-name=%1").arg(nameLabel->text().simplified().replace(" ", "_"));

    sJD = QString("%1").arg(sa.obs_pos->ctime.UTC(), 15, 'f',7);

    outerArguments << QString("-ep=%1").arg(sJD);

    if(QString().compare(catNameLabel->text(), "Planets", Qt::CaseInsensitive)==0) outerArguments << "-type=planet";
    if(QString().compare(catNameLabel->text(), "mpccat", Qt::CaseInsensitive)==0) outerArguments << "-type=aster";

    outerArguments << QString("-observer=%1").arg(sa.obs_pos->getObsCode());

    qDebug() << outerArguments.join(" ") << "\n";

    outerProcess.setWorkingDirectory(miriadeProcData.folder);
    outerProcess.setProcessChannelMode(QProcess::MergedChannels);
    outerProcess.setReadChannel(QProcess::StandardOutput);

    outerProcess.start(miriadeProcData.name, outerArguments);

    if(!outerProcess.waitForFinished(miriadeProcData.waitTime))
    {
        qDebug() << "\nmiriadeProc finish error\n";
        return;
    }

    QTextStream objStream(outerProcess.readAllStandardOutput());

    while (!objStream.atEnd())
    {
        objDataStr = objStream.readLine();


        if(objDataStr.size()<1) continue;
        if(objDataStr.at(0)=='#') continue;

        qDebug() << objDataStr << "\n";

        if(mpephR.fromMiriStr(objDataStr))
        {
            qDebug() << QString("mpephR.fromMiriStr error\n");
            return;
        }

        raLabel->setText(mas_to_hms(grad_to_mas(mpephR.ra), " ", 1));
        decLabel->setText(mas_to_damas(grad_to_mas(mpephR.de), " ", 2));
        magnLabel->setText(QString("%1").arg(mpephR.Vmag, 6, 'f', 2));
        muraLabel->setText(QString("%1").arg(mpephR.muRaCosDe, 6, 'f', 0));
        mudecLabel->setText(QString("%1").arg(mpephR.muDe, 6, 'f', 0));
    }

    return;
}

void MainWindow::on_tableWidget_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    if(current==NULL) return;

    nameLabel->setText(mainTable->item(current->row(), 1)->text());
    raLabel->setText(mainTable->item(current->row(), 2)->text());
    decLabel->setText(mainTable->item(current->row(), 3)->text());
    magnLabel->setText(mainTable->item(current->row(), 4)->text());
    tasksLabel->setText(mainTable->item(current->row(), 5)->text());
    muraLabel->setText(mainTable->item(current->row(), 7)->text());
    mudecLabel->setText(mainTable->item(current->row(), 8)->text());
    expLabel->setText(mainTable->item(current->row(), 9)->text());
    catNameLabel->setText(mainTable->item(current->row(), 10)->text());
    miriUpdateButton->setEnabled((QString().compare(catNameLabel->text(), "mpccat", Qt::CaseInsensitive)==0)||(QString().compare(catNameLabel->text(), "Planets", Qt::CaseInsensitive)==0));
    slotAddLog();
}

void MainWindow::slotAddLog()
{
    logStm << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(sysTimeEdit->text()).arg(nameLabel->text(), -16).arg(raLabel->text()).arg(decLabel->text()).arg(magnLabel->text()).arg(expLabel->text()).arg(tasksLabel->text()).arg(catNameLabel->text());
    logStm.flush();
}

void MainWindow::on_tableWidget_itemActivated(QTableWidgetItem *item)
{
}

void MainWindow::slotExit()
{
    saveSettings();
    QApplication::exit();
};

void MainWindow::closeEvent(QCloseEvent *event)
 {
    slotExit();
 }
