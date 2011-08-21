#include "wakeupdlg.h"
#include "ui_wakeupdlg.h"

wakeUpDlg::wakeUpDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::wakeUpDlg)
{
    ui->setupUi(this);
}

wakeUpDlg::~wakeUpDlg()
{
    delete ui;
}
