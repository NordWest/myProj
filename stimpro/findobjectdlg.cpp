#include "findobjectdlg.h"
#include "ui_findobjectdlg.h"

findObjectDlg::findObjectDlg(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::findObjectDlg)
{
    m_ui->setupUi(this);

    ui_astNameType = qFindChild<QCheckBox *>(this, "astNameType");
    ui_astNumType = qFindChild<QCheckBox *>(this, "astNumType");
    ui_astNameLE = qFindChild<QLineEdit *>(this, "astNameLE");
    ui_astNumLE = qFindChild<QLineEdit *>(this, "astNumLE");
    ui_lspmType = qFindChild<QCheckBox *>(this, "lspmType");
    ui_lspmName = qFindChild<QLineEdit *>(this, "lspmName");
    ui_isUseExpNum = qFindChild<QCheckBox *>(this, "isUseExpNum");

    ui_lspmFindType = qFindChild<QCheckBox *>(this, "lspmFindType");
    ui_skybotFindType = qFindChild<QCheckBox *>(this, "skybotFindType");
}

findObjectDlg::~findObjectDlg()
{
    delete m_ui;
}

void findObjectDlg::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void findObjectDlg::on_astNumType_clicked()
{
    ui_astNameType->setChecked(false);
    ui_astNumType->setChecked(true);
    ui_lspmType->setChecked(false);

    ui_lspmFindType->setChecked(false);
    ui_skybotFindType->setChecked(false);

    ui_astNumLE->setEnabled(1);
    ui_astNameLE->setEnabled(0);
    ui_lspmName->setEnabled(0);
    ui_astNumLE->setFocus();
}

void findObjectDlg::on_astNameType_clicked()
{
    ui_astNameType->setChecked(true);
    ui_astNumType->setChecked(false);
    ui_lspmType->setChecked(false);

    ui_lspmFindType->setChecked(false);
    ui_skybotFindType->setChecked(false);

    ui_astNumLE->setEnabled(0);
    ui_astNameLE->setEnabled(1);
    ui_lspmName->setEnabled(0);
    ui_astNameLE->setFocus();
}

void findObjectDlg::on_lspmType_clicked()
{
    ui_astNameType->setChecked(false);
    ui_astNumType->setChecked(false);
    ui_lspmType->setChecked(true);

    ui_lspmFindType->setChecked(false);
    ui_skybotFindType->setChecked(false);

    ui_astNumLE->setEnabled(0);
    ui_astNameLE->setEnabled(0);
    ui_lspmName->setEnabled(1);
    ui_lspmName->setFocus();
}

void findObjectDlg::on_lspmFindType_clicked()
{
    ui_astNameType->setChecked(false);
    ui_astNumType->setChecked(false);
    ui_lspmType->setChecked(false);

    ui_astNumLE->setEnabled(0);
    ui_astNameLE->setEnabled(0);
    ui_lspmName->setEnabled(0);
}

void findObjectDlg::on_skybotFindType_clicked()
{
    ui_astNameType->setChecked(false);
    ui_astNumType->setChecked(false);
    ui_lspmType->setChecked(false);

    ui_astNumLE->setEnabled(0);
    ui_astNameLE->setEnabled(0);
    ui_lspmName->setEnabled(0);
}
