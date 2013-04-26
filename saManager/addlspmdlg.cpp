#include "addlspmdlg.h"
#include "ui_addlspmdlg.h"

addLspmDlg::addLspmDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addLspmDlg)
{
    ui->setupUi(this);

    nameLE = qFindChild<QLineEdit *>(this, "nameFilter");
    searchBtn = qFindChild<QPushButton *>(this, "searchBtn");
    clearBtn = qFindChild<QPushButton *>(this, "clearBtn");
    objTable = qFindChild<QTableWidget *>(this, "objTable");
}

addLspmDlg::~addLspmDlg()
{
    delete ui;
}

void addLspmDlg::on_searchBtn_clicked()
{
    //mpcList.clear();
    sscat_rec *nRec;
    QStringList namesList = nameLE->text().split(",");

    for(int i=0; i<namesList.size();i++)
    {
        if(cat->GetName(namesList.at(i).toAscii().data())) continue;

        nRec = new sscat_rec;
        cat->record->copyTo(nRec);
        lspmList << nRec;

    }
    slotUpdateObjTable();

}

void addLspmDlg::slotUpdateObjTable()
{
    int i, sz;
    QTableWidgetItem *newItem;

    objTable->clearContents();
    objTable->setRowCount(0);

    sz = lspmList.size();
    for(i=0; i<sz; i++)
    {
        objTable->insertRow(i);

        newItem = new QTableWidgetItem(QString("%1").arg(lspmList.at(i)->getLSPM()));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 0, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(mas_to_hms(grad_to_mas(lspmList.at(i)->RAdeg), ":", 1)));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 1, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(mas_to_damas(grad_to_mas(lspmList.at(i)->DEdeg), ":", 2)));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 2, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(lspmList.at(i)->BJmag, 6, 'f', 2));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 3, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(lspmList.at(i)->pm));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 4, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(lspmList.at(i)->Vmag, 6, 'f', 2));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 5, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(lspmList.at(i)->Jmag, 6, 'f', 2));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 6, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(lspmList.at(i)->Hmag, 6, 'f', 2));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 7, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(lspmList.at(i)->Kmag, 6, 'f', 2));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 8, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(lspmList.at(i)->VJ, 6, 'f', 2));
        newItem->setTextAlignment(Qt::AlignLeft);
        objTable->setItem(i, 9, newItem);
    }
}

void addLspmDlg::on_clearBtn_clicked()
{
    lspmList.clear();
    slotUpdateObjTable();
}

void addLspmDlg::on_pushButton_clicked()
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
