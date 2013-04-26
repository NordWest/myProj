#include "addmpcdlg.h"
#include "ui_addmpcdlg.h"

addMpcDlg::addMpcDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addMpcDlg)
{
    ui->setupUi(this);

    nameLE = qFindChild<QLineEdit *>(this, "nameFilter");
    searchBtn = qFindChild<QPushButton *>(this, "searchBtn");
    clearBtn = qFindChild<QPushButton *>(this, "clearBtn");
    objTable = qFindChild<QTableWidget *>(this, "objTable");
}

addMpcDlg::~addMpcDlg()
{
    delete ui;
}

void addMpcDlg::on_searchBtn_clicked()
{
    //mpcList.clear();
    QStringList namesList = nameLE->text().split(",");

    for(int i=0; i<namesList.size();i++)
    {
        if(!cat->GetRecName(namesList.at(i).toAscii().data()))
        {
            mpcrec *nRec = new mpcrec;
            cat->record->copyTo(nRec);
            mpcList << nRec;
        }
    }
    slotUpdateObjTable();

}

void addMpcDlg::slotUpdateObjTable()
{
    int i, sz;
    QTableWidgetItem *newItem;

    objTable->clearContents();
    objTable->setRowCount(0);

    sz = mpcList.size();
    for(i=0; i<sz; i++)
    {
        objTable->insertRow(i);

        newItem = new QTableWidgetItem(QString("%1").arg(mpcList.at(i)->name));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 0, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(mpcList.at(i)->a, 10, 'f', 7));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 1, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(mpcList.at(i)->ecc, 10, 'f', 7));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 2, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(mpcList.at(i)->w, 10, 'f', 7));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 3, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(mpcList.at(i)->Node, 10, 'f', 7));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 4, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(mpcList.at(i)->inc, 10, 'f', 7));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 5, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(mpcList.at(i)->meanA, 10, 'f', 7));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 6, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(mpcList.at(i)->n, 10, 'f', 7));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 7, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(mpcList.at(i)->H, 10, 'f', 7));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 8, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(mpcList.at(i)->G, 10, 'f', 7));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 9, newItem);
    }
}

void addMpcDlg::on_clearBtn_clicked()
{
    mpcList.clear();
    slotUpdateObjTable();
}

void addMpcDlg::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
                    this,
                    tr("Choose taskList file"),
                "./");

    QFile inFile(fileName);
    inFile.open(QFile::ReadOnly);
    QTextStream inStm(&inFile);

    QStringList resStrList;
    QString nameStr;

    while(!inStm.atEnd())
    {
        resStrList << inStm.readLine().section(" ", 0, 0).simplified();
    }

    nameLE->setText(resStrList.join(","));
}
