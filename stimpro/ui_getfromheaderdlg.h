/********************************************************************************
** Form generated from reading UI file 'getfromheaderdlg.ui'
**
** Created: Tue Apr 12 15:08:34 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GETFROMHEADERDLG_H
#define UI_GETFROMHEADERDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_getFromHeaderDlg
{
public:
    QDialogButtonBox *buttonBox;
    QCheckBox *isConvertToUTC;
    QLabel *obsLabel;
    QCheckBox *isSpec;
    QLabel *expNumLabel;
    QComboBox *expList;
    QLineEdit *obsName;

    void setupUi(QDialog *getFromHeaderDlg)
    {
        if (getFromHeaderDlg->objectName().isEmpty())
            getFromHeaderDlg->setObjectName(QString::fromUtf8("getFromHeaderDlg"));
        getFromHeaderDlg->resize(458, 126);
        getFromHeaderDlg->setModal(true);
        buttonBox = new QDialogButtonBox(getFromHeaderDlg);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(370, 10, 81, 241));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        isConvertToUTC = new QCheckBox(getFromHeaderDlg);
        isConvertToUTC->setObjectName(QString::fromUtf8("isConvertToUTC"));
        isConvertToUTC->setGeometry(QRect(10, 10, 91, 31));
        isConvertToUTC->setChecked(true);
        obsLabel = new QLabel(getFromHeaderDlg);
        obsLabel->setObjectName(QString::fromUtf8("obsLabel"));
        obsLabel->setGeometry(QRect(10, 50, 71, 21));
        isSpec = new QCheckBox(getFromHeaderDlg);
        isSpec->setObjectName(QString::fromUtf8("isSpec"));
        isSpec->setGeometry(QRect(150, 20, 71, 10));
        isSpec->setChecked(true);
        expNumLabel = new QLabel(getFromHeaderDlg);
        expNumLabel->setObjectName(QString::fromUtf8("expNumLabel"));
        expNumLabel->setGeometry(QRect(10, 80, 71, 21));
        expList = new QComboBox(getFromHeaderDlg);
        expList->setObjectName(QString::fromUtf8("expList"));
        expList->setGeometry(QRect(90, 80, 271, 22));
        obsName = new QLineEdit(getFromHeaderDlg);
        obsName->setObjectName(QString::fromUtf8("obsName"));
        obsName->setGeometry(QRect(90, 50, 113, 20));

        retranslateUi(getFromHeaderDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), getFromHeaderDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), getFromHeaderDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(getFromHeaderDlg);
    } // setupUi

    void retranslateUi(QDialog *getFromHeaderDlg)
    {
        getFromHeaderDlg->setWindowTitle(QApplication::translate("getFromHeaderDlg", "Dialog", 0, QApplication::UnicodeUTF8));
        isConvertToUTC->setText(QApplication::translate("getFromHeaderDlg", "S -> UTC", 0, QApplication::UnicodeUTF8));
        obsLabel->setText(QApplication::translate("getFromHeaderDlg", "Observatory", 0, QApplication::UnicodeUTF8));
        isSpec->setText(QApplication::translate("getFromHeaderDlg", "Specific", 0, QApplication::UnicodeUTF8));
        expNumLabel->setText(QApplication::translate("getFromHeaderDlg", "Exposure num", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class getFromHeaderDlg: public Ui_getFromHeaderDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GETFROMHEADERDLG_H
