#include <QtUiTools>
#include <QtGui>

#include "newNameDlg.h"
/*
newNameDlg::newNameDlg(QWidget *parent)
    : QWidget(parent)
{
    QUiLoader loader;

    QFile file("newNameDlg.ui");
    file.open(QFile::ReadOnly);
    formWidget = loader.load(&file, this);
	formWidget->show();
    file.close();

    ui_newNameLine = qFindChild<QLineEdit*>(this, "nameIns");

    QMetaObject::connectSlotsByName(this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(formWidget);
    setLayout(layout);

//    setWindowTitle(tr("New Teleskope"));
}
*/

newNameDlg::newNameDlg(QWidget *parent)
    : QDialog(parent)
{
    QUiLoader loader;
    QFile file("newNameDlg.ui");
    if (file.open(QFile::ReadOnly)) {
        formWidget = loader.load(&file, this);
        file.close();

        ui_newNameLine = qFindChild<QLineEdit*>(this, "nameIns");

        QMetaObject::connectSlotsByName(this);
        formWidget->show();
    }
}

void newNameDlg::on_buttonBox_accepted()
{	
	emit signalExit();
	
}
