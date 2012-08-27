#include "addtaskdlg.h"
#include "ui_addtaskdlg.h"

addTaskDlg::addTaskDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addTaskDlg)
{
    ui->setupUi(this);

    name = qFindChild<QLineEdit *>(this, "name");
    catName= qFindChild<QComboBox *>(this, "catName");
    expLE = qFindChild<QLineEdit *>(this, "expLE");
    expSlider= qFindChild<QSlider *>(this, "expSlider");
    numN= qFindChild<QSpinBox *>(this, "numN");
    numTot= qFindChild<QSpinBox *>(this, "numTot");
    meriLE = qFindChild<QLineEdit *>(this, "meriLE");
    meriSlider= qFindChild<QSlider *>(this, "meriSlider");
    descText= qFindChild<QLineEdit *>(this, "descText");
    tExc= qFindChild<QSpinBox *>(this, "tExc");

    connect(expSlider, SIGNAL(valueChanged(int)), this, SLOT(slotChangeExp()));
    connect(meriSlider, SIGNAL(valueChanged(int)), this, SLOT(slotChangeMeri()));
}

addTaskDlg::~addTaskDlg()
{
    delete ui;
}

void addTaskDlg::slotChangeExp()
{
    expLE->setText(QString("%1").arg(expSlider->value()));
}

void addTaskDlg::slotChangeMeri()
{
    meriLE->setText(QString("%1").arg(meriSlider->value()));
}


