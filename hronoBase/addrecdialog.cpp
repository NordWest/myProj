#include "addrecdialog.h"
#include "ui_addrecdialog.h"

addRecDialog::addRecDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::addRecDialog)
{
    m_ui->setupUi(this);

    dateRec = m_ui->dateEdit;
    timeRealRec = m_ui->timeReal;
    timeHronoRec = m_ui->timeHrono;

    comment = m_ui->comment;
}

addRecDialog::~addRecDialog()
{
    delete m_ui;
}

void addRecDialog::changeEvent(QEvent *e)
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
