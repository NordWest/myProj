#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addrecdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->installEventFilter(this);

    setWindowTitle(tr("hrono Base"));

    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(slotOpen()));
    frameList = ui->tableWidget;
    colNum = frameList->columnCount();

    connect(ui->actionInsert, SIGNAL(triggered()), this, SLOT(slotInsert()));
    connect(ui->actionInsertAt, SIGNAL(triggered()), this, SLOT(slotInsertAt()));
    connect(ui->actionRemove, SIGNAL(triggered()), this, SLOT(slotRemove()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(slotSave()));
    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(slotClose()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(slotNew()));

    is_edt = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotOpen()
{
    if(is_edt)
    {
        QMessageBox msgBox;
         msgBox.setText("Base has been edited!");
         msgBox.setInformativeText("Do you want to save your changes?");
         msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
         msgBox.setDefaultButton(QMessageBox::Save);
         int ret = msgBox.exec();
         if(ret==QMessageBox::Save) slotSave();
         if(ret==QMessageBox::Cancel) return;
    }

    slotClose();

    fName = QFileDialog::getOpenFileName(this, tr("Choose files"), "./", "hronoBase (*.txt *.hbs)");
    int i;
    QTextStream in_stream;
    QFile fin(fName);
    fin.open(QIODevice::ReadOnly | QIODevice::Text);

    in_stream.setDevice(&fin);

    QStringList baseStrList;
    while(!in_stream.atEnd()) baseStrList << in_stream.readLine();

    int lnum = baseStrList.size();
    QString baseStr;

    frameList->clearContents();
    for(i=0; i<lnum; i++)
    {
        baseStr = baseStrList.at(i);
        insertStr(baseStr, i);
    }
    slotSaved();
}

void MainWindow::slotNew()
{
    if(is_edt)
    {
        QMessageBox msgBox;
         msgBox.setText("Base has been edited!");
         msgBox.setInformativeText("Do you want to save your changes?");
         msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
         msgBox.setDefaultButton(QMessageBox::Save);
         int ret = msgBox.exec();
         if(ret==QMessageBox::Save) slotSave();
         if(ret==QMessageBox::Cancel) return;
    }

    slotClose();

    fName = QFileDialog::getSaveFileName(this, tr("Choose files"), "./", "hronoBase (*.txt *.hbs)");
    QTextStream in_stream;
    QFile fin(fName);
    fin.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    fin.close();
    setWindowTitle(tr("hrono Base")+" - "+QString("%1").arg(fName));
}

void MainWindow::insertStr(QString sLine, int pos)
{
    QStringList operands;
    operands = sLine.split("|");
    int cNum = operands.size();
//   int i = frameList->rowCount();
//   frameList->setRowCount(i+1);
    frameList->insertRow(pos);

    for(int j=0; j<cNum; j++)
    {
        frameItem = new QTableWidgetItem(operands[j]);
        frameList->setItem(pos, j, frameItem);
        frameList->resizeColumnToContents(j);
    }
}

void MainWindow::slotInsert()
{   
    int i;

    QStringList bStrL;

    addRecDialog insRec;

    if(insRec.exec()==QDialog::Accepted)
    {
        slotEdited();

        bStrL << insRec.dateRec->date().toString("dd.MM.yyyy");
        bStrL << insRec.timeRealRec->time().toString("hh mm ss.zzz");
        bStrL << insRec.timeHronoRec->time().toString("hh mm ss.zzz");

        bStrL << insRec.comment->text();

        QString bStr = bStrL.join("|");
        i = frameList->rowCount();
//        i = frameList->currentRow();
 //       frameList->insertRow(i);
        insertStr(bStr, i);
    }
}

void MainWindow::slotInsertAt()
{
    int i;

    QStringList bStrL;

    addRecDialog insRec;

    if(insRec.exec()==QDialog::Accepted)
    {
        slotEdited();

        bStrL << insRec.dateRec->date().toString("dd.MM.yyyy");
        bStrL << insRec.timeRealRec->time().toString("hh mm ss.zzz");
        bStrL << insRec.timeHronoRec->time().toString("hh mm ss.zzz");

        bStrL << insRec.comment->text();

        QString bStr = bStrL.join("|");
        i = frameList->currentRow();
        if(i>=0)
        {
//            frameList->insertRow(i);
            insertStr(bStr, i);
        }
    }
}

void MainWindow::slotRemove()
{
    int remNum = frameList->currentRow();
    if(remNum>=0)
    {
        if(QMessageBox::question(this, "Deleting record", "Are you shure?", QMessageBox::Yes, QMessageBox::Abort)==QMessageBox::Yes)
        {
            slotEdited();
            frameList->removeRow(frameList->currentRow());
        }
    }
}

void MainWindow::slotSave()
{
    int i, j;

    fName = QFileDialog::getSaveFileName(this, tr("Choose file"), fName, "hronoBase (*.txt *.hbs)");

    int rNum = frameList->rowCount();

    QTextStream out_stream;
    QFile fin(fName);
    fin.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);

    out_stream.setDevice(&fin);
    QStringList bLine;


    for(i=0; i<rNum; i++)
    {
        bLine.clear();
        for(j=0; j<colNum; j++)
        {

            frameItem = frameList->item(i, j);
            bLine << frameItem->text();
        }
        out_stream << bLine.join("|") << "\n";
    }
    fin.close();
    slotSaved();
}

void MainWindow::slotClose()
{
    frameList->clearContents();
    frameList->setRowCount(0);
    fName.clear();
    fName.insert(0,"noname");
    slotSaved();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if ((obj == this))
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (event->type() == QEvent::KeyPress)
        {
            slotKeyRules(keyEvent);
            return true;
        }
    }
    if ((obj == this)&&event->type()==QEvent::Close)
    {
        close();
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::slotKeyRules(QKeyEvent *keyEvent)
{
//  QMessageBox::information(0,"debug","pressed",QMessageBox::Ok,0,0);
    //if (keyEvent-> modifiers () == Qt::ControlModifier)
    //{
    if (keyEvent->key() == Qt::Key_Insert) slotInsert();
    if (keyEvent->key() == Qt::Key_Delete) slotRemove();
/*    if (keyEvent->key() == Qt::Key_Delete) slotDelete();
    if (keyEvent->key() == Qt::Key_Control) ctrlB = 1;
    if (keyEvent->key() == Qt::Key_F9) slotWCSButtonClicked();
    if (keyEvent->key() == Qt::Key_F12)
    {
        if (!mainFrame)moveCheckBox->setChecked(Qt::Checked);
        else moveCheckBox->setChecked(Qt::Unchecked);
    }*/
}

void MainWindow::slotEdited()
{
    is_edt = 1;
    setWindowTitle(tr("hrono Base")+" - "+QString("%1").arg(fName)+"*");
}

void MainWindow::slotSaved()
{
    is_edt = 0;
    setWindowTitle(tr("hrono Base")+" - "+QString("%1").arg(fName));
}

bool MainWindow::close()
{
     if(is_edt)
    {
        QMessageBox msgBox;
         msgBox.setText("Base has been edited!");
         msgBox.setInformativeText("Do you want to save your changes?");
         msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
         msgBox.setDefaultButton(QMessageBox::Save);
         int ret = msgBox.exec();
         if(ret==QMessageBox::Save) slotSave();
         if(ret==QMessageBox::Cancel) return 0;
    }

    slotClose();

//    QMessageBox::information(this, "close", "close");

    return QMainWindow::close();
}
