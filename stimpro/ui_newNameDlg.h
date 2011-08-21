/********************************************************************************
** Form generated from reading UI file 'newNameDlg.ui'
**
** Created: Tue Apr 12 15:08:34 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWNAMEDLG_H
#define UI_NEWNAMEDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_newNameDlg
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *nameIns;
    QLabel *label;

    void setupUi(QDialog *newNameDlg)
    {
        if (newNameDlg->objectName().isEmpty())
            newNameDlg->setObjectName(QString::fromUtf8("newNameDlg"));
        newNameDlg->resize(278, 111);
        buttonBox = new QDialogButtonBox(newNameDlg);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(20, 70, 251, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);
        nameIns = new QLineEdit(newNameDlg);
        nameIns->setObjectName(QString::fromUtf8("nameIns"));
        nameIns->setGeometry(QRect(60, 10, 211, 20));
        label = new QLabel(newNameDlg);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 10, 46, 14));

        retranslateUi(newNameDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), newNameDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), newNameDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(newNameDlg);
    } // setupUi

    void retranslateUi(QDialog *newNameDlg)
    {
        newNameDlg->setWindowTitle(QApplication::translate("newNameDlg", "New Instrument", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("newNameDlg", "Name", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class newNameDlg: public Ui_newNameDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWNAMEDLG_H
