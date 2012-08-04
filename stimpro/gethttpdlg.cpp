#include "gethttpdlg.h"

getHttpDlg::getHttpDlg(QWidget *parent): QDialog(parent)
{
    ui.setupUi(this);
    ui_plateNum = qFindChild<QLineEdit*>(this, "plateNum");
    ui_instr = qFindChild<QComboBox*>(this, "instr");
    /*
    QUiLoader loader;
    QFile file("getHttpDlg.ui");
    if (file.open(QFile::ReadOnly)) {
        formWidget = loader.load(&file, this);
        file.close();

        ui_plateNum = qFindChild<QLineEdit*>(this, "plateNum");
        ui_instr = qFindChild<QComboBox*>(this, "instr");

        QMetaObject::connectSlotsByName(this);
        formWidget->show();
    }*/
}
/*
void getHttpDlg::on_buttonBox_accepted()
{
        emit signalExit();

}

void getHttpDlg::on_buttonBox_rejected()
{
        emit signalExit();

}
*/
