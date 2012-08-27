#include "addobjdlg.h"
#include "ui_addobjdlg.h"

addObjDlg::addObjDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addObjDlg)
{
    ui->setupUi(this);
}

addObjDlg::~addObjDlg()
{
    delete ui;
}
