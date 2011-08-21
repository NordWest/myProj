#include "getfromheaderdlg.h"
#include "ui_getfromheaderdlg.h"

getFromHeaderDlg::getFromHeaderDlg(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::getFromHeaderDlg)
{
    m_ui->setupUi(this);

    ui_isConvertToUTC = qFindChild<QCheckBox*>(this, "isConvertToUTC");
    ui_isSpec = qFindChild<QCheckBox*>(this, "isSpec");
    ui_obsName = qFindChild<QLineEdit*>(this, "obsName");
    ui_expList = qFindChild<QComboBox*>(this, "expList");
}

getFromHeaderDlg::~getFromHeaderDlg()
{
    delete m_ui;
}

void getFromHeaderDlg::changeEvent(QEvent *e)
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
