#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    taskTable = qFindChild<QTableWidget *>(this, "taskTable");
    objTable= qFindChild<QTableWidget *>(this, "objTable");
    toolsB= qFindChild<QToolBar *>(this, "mainToolBar");

    vertLt = qFindChild<QVBoxLayout *>(this, "verticalLayout");

    setWindowTitle("saManager");


    setupWidgets();
    setupMenu();

//    textEdit->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
/*
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
 {
     if (obj == this) {
         if (event->type() == QEvent::Close) {
                slotSaveSA();
             return true;
         } else {
             return false;
         }
     } else {
         // pass the event on to the parent class
         return QMainWindow::eventFilter(obj, event);
     }
 }
*/
void MainWindow::setupMenu()
{

    fileMenu = ui->menuBar->addMenu(tr("file"));

    openTaskAct = new QAction(tr("&open task"), this);
    openTaskAct->setShortcut(tr("Ctrl+O"));
    openTaskAct->setStatusTip(tr("open taskList file"));
    fileMenu->addAction(openTaskAct);
    connect(openTaskAct, SIGNAL(triggered()), this, SLOT(slotOpenTaskList()));

    saveTaskAct = new QAction(tr("&save task"), this);
    saveTaskAct->setShortcut(tr("Ctrl+S"));
    saveTaskAct->setStatusTip(tr("save taskList file"));
    fileMenu->addAction(saveTaskAct);
    connect(saveTaskAct, SIGNAL(triggered()), this, SLOT(slotSaveTaskList()));

    saveAsTaskAct = new QAction(tr("&save task as..."), this);
    saveAsTaskAct->setShortcut(tr("Ctrl+Shift+S"));
    saveAsTaskAct->setStatusTip(tr("save taskList file as"));
    fileMenu->addAction(saveAsTaskAct);
    connect(saveAsTaskAct, SIGNAL(triggered()), this, SLOT(slotSaveAsTaskList()));

    taskMenu = ui->menuBar->addMenu(tr("task"));

    addTaskAct = new QAction(tr("&Add task"), this);
    addTaskAct->setShortcut(tr("Ctrl+N"));
    addTaskAct->setStatusTip(tr("add task"));
    taskMenu->addAction(addTaskAct);
    connect(addTaskAct, SIGNAL(triggered()), this, SLOT(slotAddTask()));

    catalogsAct = new QAction(tr("&View catalogs"), this);
    catalogsAct->setShortcut(tr("Ctrl+M"));
    catalogsAct->setStatusTip(tr("view catalogs"));
    taskMenu->addAction(catalogsAct);
    connect(catalogsAct, SIGNAL(triggered()), this, SLOT(slotViewCatalogs()));
}

void MainWindow::setupWidgets()
{
    addTaskBtn = new QPushButton("Add Task");
    toolsB->addWidget(addTaskBtn);
    connect(addTaskBtn, SIGNAL(clicked()), this, SLOT(slotAddTask()));

    delTaskBtn = new QPushButton("Del Task");
    toolsB->addWidget(delTaskBtn);
    connect(delTaskBtn, SIGNAL(clicked()), this, SLOT(slotDelTask()));

    toolsB->addSeparator();

    saveBtn = new QPushButton("Save");
    toolsB->addWidget(saveBtn);
    connect(saveBtn, SIGNAL(clicked()), this, SLOT(slotSaveSA()));

    toolsB->addSeparator();

    closeBtn = new QPushButton("Close");
    toolsB->addWidget(closeBtn);
    connect(closeBtn, SIGNAL(clicked()), this, SLOT(close()));

    QStringList vhLabelsT;
    vhLabelsT << QString(tr(""));
    vhLabelsT << QString(tr("Name"));
    vhLabelsT << QString(tr("catType"));
    vhLabelsT << QString(tr("Exp"));
    vhLabelsT << QString(tr("NinN"));
    vhLabelsT << QString(tr("Ntot"));
    vhLabelsT << QString(tr("dRA"));
    vhLabelsT << QString(tr("Texc"));
    vhLabelsT << QString(tr("Desc"));
    taskTable->setColumnCount(9);
    taskTable->setColumnWidth(0, 20);
    //taskView->setColumnWidth(2, 300);
    taskTable->setHorizontalHeaderLabels(vhLabelsT);

    //taskView->setEditTriggers(QAbstractItemView::NoEditTriggers);


    QStringList vhLabelsO;
    //vhLabelsO << QString(tr(""));
    vhLabelsO << QString(tr("Name"));
    vhLabelsO << QString(tr("Exp"));
    vhLabelsO << QString(tr("Nobs"));
//    vhLabelsO << QString(tr("Desc"));
    objTable->setColumnCount(3);
    //objTable->setColumnWidth(0, 20);
    //taskView->setColumnWidth(2, 300);
    objTable->setHorizontalHeaderLabels(vhLabelsO);
    //taskView->setEditTriggers(QAbstractItemView::NoEditTriggers);


    objTools = new QToolBar;
    vertLt->insertWidget(1, objTools);

    addNameListBtn = new QPushButton("Add Name List");
    objTools->addWidget(addNameListBtn);
    connect(addNameListBtn, SIGNAL(clicked()), this, SLOT(slotAddNameListObj()));

    addFromCatBtn = new QPushButton("Add from cat");
    objTools->addWidget(addFromCatBtn);
    connect(addFromCatBtn, SIGNAL(clicked()), this, SLOT(slotAddFromCatObj()));

    objTools->addSeparator();

    remObjBtn = new QPushButton("Remove selected");
    objTools->addWidget(remObjBtn);
    connect(remObjBtn, SIGNAL(clicked()), this, SLOT(slotRemSelectedObj()));

}

void MainWindow::show()
{
    if(!sArea.initInstallDir(workPath)) slotUpdateTaskList();
    QWidget::show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    slotSaveSA();
//    MainWindow::close();
}

void MainWindow::slotSaveSA()
{
    int i, sz;
    sz = sArea.task_list.size();
    //QTableWidgetItem *tempItem;
    QCheckBox *tBox;

    for(i=0;i<sz;i++)
    {
        tBox = (QCheckBox*)taskTable->cellWidget(i, 0);
        //tBox = (QCheckBox*)tempItem->tableWidget();
        sArea.task_list.at(i)->flag_active = tBox->checkState();// tBox->isChecked();
    }

    sArea.save();


}

void MainWindow::slotAddNameListObj()
{
    QString fileName = QFileDialog::getOpenFileName(
                    this,
                    tr("Choose taskList file"),
                    workPath);

    QString tStr;
    tlRecord *taskR;
    catRecord *catR;
    iniRecord *iRec;
    iniList *iObj;
    int i, sz, cr;

    if(taskTable->selectionModel()->selectedRows().count()==0)QMessageBox::warning(this, tr("Add Names task"),
                                                                                   tr("Please, select proper task"),
                                                                                   QMessageBox::Ok);
    else
    {
        cr = taskTable->selectionModel()->selectedRows().at(0).row();
        taskR = sArea.task_list.recList.at(cr);
        catR = sArea.cat_list.getCatByName(taskR->catName);

        if(catR->catType==0)
        {
            QMessageBox::warning(this, tr("Add Names task"),
                                                                                               tr("Try to use \"Add from cat\" option for DeLe cat"),
                                                                                               QMessageBox::Ok);
            return;

        }

        QFile inFile(fileName);

        if(!inFile.open(QFile::ReadOnly))
        {
            QMessageBox::warning(NULL, "Can't open names file", QString("File %1 not open").arg(fileName), QMessageBox::Ok);
        }
        else
        {
            QTextStream inStm(&inFile);
            iRec = new iniRecord;
            iObj = &taskR->iList;
            while(!inStm.atEnd())
            {
                tStr = inStm.readLine();
                iRec->name = tStr.section(" ", 0, 0);
                iRec->exp = 10;
                iObj->addRec(iRec);
            }

            inFile.close();
            slotUpdateObjList();
        }
    }


}

void MainWindow::slotAddFromCatObj()
{
    QList <QTableWidgetItem*> itemList;
    itemList = taskTable->selectedItems();
    tlRecord *taskR;
    catRecord *catR;
    iniRecord *iRec;
    int i, sz, cr;

    iniList *iObj;
    sz = itemList.size();

    if(sz==0)
    {
        QMessageBox::warning(this, tr("Del task"),
                                                              tr("Please, select proper task"),
                                                              QMessageBox::Ok);
    }
    else
    {
        cr = itemList.at(0)->row();

            taskR = sArea.task_list.recList.at(cr);
            catR = sArea.cat_list.getCatByName(taskR->catName);
            if(catR==NULL)
            {
                QMessageBox::critical(this, tr("slotAddFromCatObj"),
                                                                      tr("Task catName error"),
                                                                      QMessageBox::Ok);
                return;
            }
            iObj = &taskR->iList;
            switch(catR->catType)
            {
                case DELE_CAT_TYPE:
                {

                    addDeLeDlg addDE;

                    addDE.ui_sun->setChecked(!iObj->getRecName(iRec, "Sun"));
                    addDE.ui_venus->setChecked(!iObj->getRecName(iRec, "Venus"));
                    addDE.ui_mars->setChecked(!iObj->getRecName(iRec, "Mars"));
                    addDE.ui_jupiter->setChecked(!iObj->getRecName(iRec, "Jupiter"));
                    addDE.ui_saturn->setChecked(!iObj->getRecName(iRec, "Saturn"));
                    addDE.ui_uranus->setChecked(!iObj->getRecName(iRec, "Uranus"));
                    addDE.ui_neptune->setChecked(!iObj->getRecName(iRec, "Neptune"));
                    addDE.ui_pluto->setChecked(!iObj->getRecName(iRec, "Pluto"));

                    if(addDE.exec()==QDialog::Accepted)
                    {
                        iObj->recList.clear();
                        iRec = new iniRecord;
                        if(addDE.ui_sun->isChecked())
                        {
                            iRec->name = "Sun";
                            iRec->exp = 10;
                            iObj->addRec(iRec);
                        }
                        if(addDE.ui_mercury->isChecked())
                        {
                            iRec->name = "Mercury";
                            iRec->exp = 10;
                            iObj->addRec(iRec);
                        }
                        if(addDE.ui_venus->isChecked())
                        {
                            iRec->name = "Venus";
                            iRec->exp = 10;
                            iObj->addRec(iRec);
                        }
                        if(addDE.ui_mars->isChecked())
                        {
                            iRec->name = "Mars";
                            iRec->exp = 10;
                            iObj->addRec(iRec);
                        }
                        if(addDE.ui_jupiter->isChecked())
                        {
                            iRec->name = "Jupiter";
                            iRec->exp = 10;
                            iObj->addRec(iRec);
                        }
                        if(addDE.ui_saturn->isChecked())
                        {
                            iRec->name = "Saturn";
                            iRec->exp = 10;
                            iObj->addRec(iRec);
                        }
                        if(addDE.ui_uranus->isChecked())
                        {
                            iRec->name = "Uranus";
                            iRec->exp = 10;
                            iObj->addRec(iRec);
                        }
                        if(addDE.ui_neptune->isChecked())
                        {
                            iRec->name = "Neptune";
                            iRec->exp = 10;
                            iObj->addRec(iRec);
                        }
                        if(addDE.ui_pluto->isChecked())
                        {
                            iRec->name = "Pluto";
                            iRec->exp = 10;
                            iObj->addRec(iRec);
                        }
                        slotUpdateObjList();
                    }

                    //planet_num();

                }
                break;
            case MPC_CAT_TYPE:
            {
                addMpcDlg addMPC;
                addMPC.cat = new mpccat;
                if(addMPC.cat->init(catR->catFile.toAscii().data()))
                {
                    QMessageBox::warning(0, "has no cat", "has no cat");
                    break;
                }


                if(addMPC.exec()==QDialog::Accepted)
                {
                    //iniCatName = sArea.getTaskCatName(taskR->name);

                    //QFile

                    sz = addMPC.mpcList.size();
                    iRec = new iniRecord;
                    for(i=0; i<sz; i++)
                    {

                        iRec->name = QString(addMPC.mpcList.at(i)->name).simplified();
                        iRec->exp = 10;
                        iObj->addRec(iRec);
                    }


                    slotUpdateObjList();
                }
            }
            break;

            case LSPM_CAT_TYPE:
            {
                addLspmDlg addLSPM;
                addLSPM.cat = new sscatFB;
                if(addLSPM.cat->init(catR->catFile.toAscii().data()))
                {
                    QMessageBox::warning(0, "has no cat", "has no cat");
                    break;
                }


                if(addLSPM.exec()==QDialog::Accepted)
                {
                    //iniCatName = sArea.getTaskCatName(taskR->name);

                    //QFile

                    sz = addLSPM.lspmList.size();
                    iRec = new iniRecord;
                    for(i=0; i<sz; i++)
                    {

                        iRec->name = QString(addLSPM.lspmList.at(i)->getLSPM()).simplified();
                        iRec->exp = 10;
                        iObj->addRec(iRec);
                    }


                    slotUpdateObjList();
                }
            }
            break;

            }


    }
}

void MainWindow::slotRemSelectedObj()
{
    int i, j, sz;

    QString objName;
    QString taskName;
    QItemSelectionModel *smod = taskTable->selectionModel();

    int taskRow = smod->selectedRows().at(0).row();

    taskName = taskTable->item(taskRow, 1)->text().simplified();

    QList <QTableWidgetItem*> itemList;
    itemList = taskTable->selectedItems();
    sz = objTable->selectionModel()->selectedRows().count();
    for(i=sz-1;i>=0;i--)
    {
        objName = objTable->item(objTable->selectionModel()->selectedRows().at(i).row(), 0)->text().simplified();
        sArea.removeObj(taskName, objName);
    }
    slotUpdateObjList();
}

void MainWindow::slotUpdateObjList()
{
    int i, j, sz;

    QList <QTableWidgetItem*> itemList;
    itemList = taskTable->selectedItems();
    tlRecord *taskR;
    catRecord *catR;
    iniRecord *iRec;
    QTableWidgetItem *newItem;
    QCheckBox *actCheck;


    objTable->clearContents();
    objTable->setRowCount(0);

    if(itemList.size()==0)
    {
        return;
    }

        taskR = sArea.task_list.recList.at(itemList.at(0)->row());
        //if(taskR.iList!=NULL)
        //{
            sz = taskR->iList.size();
            for(j=0; j<sz; j++)
            {
                objTable->insertRow(j);



                newItem = new QTableWidgetItem(QString("%1").arg(taskR->iList.at(j)->name));
                newItem->setTextAlignment(Qt::AlignLeft);
                objTable->setItem(j, 0, newItem);

                newItem = new QTableWidgetItem(QString("%1").arg(taskR->iList.at(j)->exp));
                newItem->setTextAlignment(Qt::AlignLeft);
                objTable->setItem(j, 1, newItem);

                //newItem = new QTableWidgetItem(QString("%1").arg(taskR->iList.at(j)->desc));
                //newItem->setTextAlignment(Qt::AlignLeft);
                //objTable->setItem(j, 3, newItem);
            }
        //}

}

void MainWindow::slotOpenTaskList()
{
    workPath = QFileDialog::getExistingDirectory(
                    this,
                    tr("Choose taskList file"),
                    workPath);

    if(!sArea.initInstallDir(workPath)) slotUpdateTaskList();

}

void MainWindow::slotUpdateTaskList()
{
    statusBar()->showMessage(workPath);

    taskTable->clearContents();
    taskTable->setRowCount(0);
    QTableWidgetItem *newItem;
    int i, sz;
    sz = sArea.task_list.size();
    QCheckBox *actCheck;

/*
    tableView->insertRow(0);
    newItem = new QTableWidgetItem(QString("test"));
    tableView->setItem(0, 0, newItem);
*/
    for(i=0; i<sz; i++)
    {
        taskTable->insertRow(i);

        newItem = new QTableWidgetItem();
        actCheck = new QCheckBox();
        actCheck->setChecked(sArea.task_list.at(i)->flag_active);
        taskTable->setCellWidget(i, 0, actCheck);

        connect(actCheck, SIGNAL(stateChanged ( int )), this, SLOT(slotChangeFlagTask(int)));

        newItem = new QTableWidgetItem(QString("%1").arg(sArea.task_list.at(i)->name));
        newItem->setTextAlignment(Qt::AlignLeft);
        taskTable->setItem(i, 1, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(sArea.task_list.at(i)->catName));
        newItem->setTextAlignment(Qt::AlignLeft);
        taskTable->setItem(i, 2, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(sArea.task_list.at(i)->exp));
        newItem->setTextAlignment(Qt::AlignLeft);
        taskTable->setItem(i, 3, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(sArea.task_list.at(i)->NinN));
        newItem->setTextAlignment(Qt::AlignLeft);
        taskTable->setItem(i, 4, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(sArea.task_list.at(i)->Ntot));
        newItem->setTextAlignment(Qt::AlignLeft);
        taskTable->setItem(i, 5, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(sArea.task_list.at(i)->dRA));
        newItem->setTextAlignment(Qt::AlignLeft);
        taskTable->setItem(i, 6, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(sArea.task_list.at(i)->texc));
        newItem->setTextAlignment(Qt::AlignLeft);
        taskTable->setItem(i, 7, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(sArea.task_list.at(i)->desc));
        newItem->setTextAlignment(Qt::AlignLeft);
        taskTable->setItem(i, 8, newItem);

    }

}

void MainWindow::slotSaveTaskList()
{
}

void MainWindow::slotSaveAsTaskList()
{
}

void MainWindow::slotAddTask()
{
    addTaskDlg addtDlg;
    tlRecord tlRec;
    int i, sz;
    sz = sArea.cat_list.size();
    for(i=0; i<sz; i++)
    {
        addtDlg.catName->insertItem(i, sArea.cat_list.at(i)->name);
    }

    if(addtDlg.exec()==QDialog::Accepted)
    {
        tlRec.flag_active = 1;
        tlRec.name = addtDlg.name->text();
        tlRec.catName = addtDlg.catName->currentText();
        tlRec.exp = addtDlg.expSlider->value();
        tlRec.NinN = addtDlg.numN->value();
        tlRec.Ntot = addtDlg.numTot->value();
        tlRec.dRA = addtDlg.meriSlider->value();
        tlRec.texc = addtDlg.tExc->value();
        tlRec.desc = addtDlg.descText->text();
        //tlRec.dirPath = QString("%1/%2").arg(workPath).arg(tlRec.name);
        sArea.task_list.addRec(tlRec);
        slotUpdateTaskList();
    }
}

void MainWindow::slotDelTask()
{
    QList <QTableWidgetItem*> itemList;
    itemList = taskTable->selectedItems();
    if(itemList.size()==0)
    {
        QMessageBox::warning(this, tr("Del task"),
                                                              tr("Please, select proper task"),
                                                              QMessageBox::Ok);
    }
    else
    {
        for(int i=0; i<itemList.size(); i++)
        {
            sArea.task_list.recList.removeAt(itemList.at(i)->row());
        }
        slotUpdateTaskList();
    }
}

void MainWindow::slotViewCatalogs()
{
    catsView cView(&sArea.cat_list);
    cView.workPath = workPath;
    //cView.cat_list = &sArea.cat_list;
    cView.exec();
    sArea.cat_list.save();
}

void MainWindow::slotChangeFlagTask(int state)
{
    int cRow = taskTable->currentRow();
    sArea.task_list.at(cRow)->flag_active = !sArea.task_list.at(cRow)->flag_active;
}

void MainWindow::on_taskTable_cellClicked(int row, int column)
{
    slotUpdateObjList();
}
