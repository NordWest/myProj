#include "catsview.h"
#include "ui_catsview.h"

catsView::catsView(catList *cl, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::catsView)
{
//    if(cat_list==NULL) quit();
    cat_list = cl;

    ui->setupUi(this);

    hLayout = qFindChild<QHBoxLayout *>(this, "horizontalLayout");
    tableView = qFindChild<QTableWidget *>(this, "tableView");
    addCatBtn = qFindChild<QPushButton *>(this, "addCatButton");
    delCatBtn = qFindChild<QPushButton *>(this, "delCatButton");

    QStringList vhLabels;
    vhLabels << QString(tr("Name"));
    vhLabels << QString(tr("Type"));
    vhLabels << QString(tr("File"));
    tableView->setColumnCount(3);
    tableView->setColumnWidth(2, 300);
    tableView->setHorizontalHeaderLabels(vhLabels);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    updateCatsTable();
}

catsView::~catsView()
{
    delete ui;
}

void catsView::on_addCatButton_clicked()
{
    addCatDlg addC;
    catRecord cRec;
    addC.workPath = workPath;

    if(addC.exec()==QDialog::Accepted)
    {
        cRec.catFile = addC.catFile->text();
        cRec.name = addC.catName->text();
        cRec.catType = addC.catType->currentIndex();
        cat_list->addRec(cRec);

        updateCatsTable();
    }
}

void catsView::updateCatsTable()
{
    tableView->clearContents();
    tableView->setRowCount(0);
    QTableWidgetItem *newItem;
    int i, sz;
    sz = cat_list->size();

/*
    tableView->insertRow(0);
    newItem = new QTableWidgetItem(QString("test"));
    tableView->setItem(0, 0, newItem);
*/
    for(i=0; i<sz; i++)
    {
        tableView->insertRow(i);

        newItem = new QTableWidgetItem(QString("%1").arg(cat_list->at(i)->name));
        newItem->setTextAlignment(Qt::AlignLeft);
        tableView->setItem(i, 0, newItem);

        newItem = new QTableWidgetItem(cat_list->at(i)->getCatTypeName());
        newItem->setTextAlignment(Qt::AlignLeft);
        tableView->setItem(i, 1, newItem);

        newItem = new QTableWidgetItem(QString("%1").arg(cat_list->at(i)->catFile));
        newItem->setTextAlignment(Qt::AlignLeft);
        tableView->setItem(i, 2, newItem);
    }

}

void catsView::on_delCatButton_clicked()
{
    QList <QTableWidgetItem *> selI;
    int sz = tableView->selectionModel()->selectedRows().count();// >selectedItems();
    for(int i=0; i<sz; i++)
    {
        cat_list->recList.removeAt(tableView->selectionModel()->selectedRows().at(i).row());
    }
    updateCatsTable();
}

void catsView::on_editCatBtn_clicked()
{
    catRecord *cRec = cat_list->at(tableView->selectionModel()->selectedRows().at(0).row());
    catRecord nRec;
    addCatDlg addC;
    addC.setCatRec(*cRec);
    /*addC.catFile->setText(cRec->catFile);
    addC.catName->setText(cRec->name);
    addC.catType->setCurrentIndex();*/

    if(addC.exec()==QDialog::Accepted)
    {
        cRec->catFile = addC.catFile->text();
        cRec->name = addC.catName->text();
        cRec->catType = addC.catType->currentIndex();
        cat_list->addRec(cRec);

        updateCatsTable();
    }
}
