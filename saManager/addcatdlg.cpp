#include "addcatdlg.h"
#include "ui_addcatdlg.h"

addCatDlg::addCatDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addCatDlg)
{
    ui->setupUi(this);

    broseCatBtn = qFindChild<QPushButton *>(this, "broseCatBtn");
    catFile = qFindChild<QLineEdit *>(this, "catFile");
    catName = qFindChild<QLineEdit *>(this, "catName");
    catType = qFindChild<QComboBox *>(this, "catType");

    catType->addItem("dele");
    catType->addItem("mpccat");
    catType->addItem("sscat");
}

addCatDlg::~addCatDlg()
{
    delete ui;
}

void addCatDlg::setCatRec(catRecord &catRec)
{
    catFile->setText(catRec.catFile);
    catName->setText(catRec.name);
    catType->setCurrentIndex(catRec.catType);
}

void addCatDlg::on_broseCatBtn_clicked()
{
    QString catFileName = QFileDialog::getOpenFileName(
                    this,
                    tr("Choose cat file"),
                    workPath,
                    "Catalog (*.*)");

    catFile->clear();
    catFile->insert(catFileName);

}
