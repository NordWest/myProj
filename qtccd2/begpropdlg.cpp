#include "begpropdlg.h"
#include "ui_begpropdlg.h"

begPropDlg::begPropDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::begPropDlg)
{
    ui->setupUi(this);

    //ui_plateNum = qFindChild<QLineEdit*>(this, "plateNum");
    ui_obsName = qFindChild<QComboBox*>(this, "obsBox");
    ui_filtersBox = qFindChild<QComboBox*>(this, "filtersBox");
    ui_focalBox = qFindChild<QDoubleSpinBox*>(this, "focalBox");
    ui_tempBox = qFindChild<QSpinBox*>(this, "tempBox");
    ui_dlgButtons = qFindChild<QDialogButtonBox*>(this, "buttonBox");

    ui_okButton = ui_dlgButtons->button(QDialogButtonBox::Ok);
    //ui_okButton->setEnabled(0);
    //ui_filtersBox->setCurrentIndex();
    profOk();
}

begPropDlg::~begPropDlg()
{
    delete ui;
}

void begPropDlg::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void begPropDlg::on_obsBox_currentIndexChanged(int index)
{
    profOk();
}

void begPropDlg::profOk()
{
    ui_okButton->setEnabled(ui_obsName->currentIndex()||(ui_obsName->count()==0));
}
