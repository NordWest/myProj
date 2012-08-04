/********************************************************************************
** Form generated from reading UI file 'wakeupdlg.ui'
**
** Created: Fri 1. Apr 04:45:56 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WAKEUPDLG_H
#define UI_WAKEUPDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_wakeUpDlg
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;

    void setupUi(QDialog *wakeUpDlg)
    {
        if (wakeUpDlg->objectName().isEmpty())
            wakeUpDlg->setObjectName(QString::fromUtf8("wakeUpDlg"));
        wakeUpDlg->resize(400, 300);
        buttonBox = new QDialogButtonBox(wakeUpDlg);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        label = new QLabel(wakeUpDlg);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 361, 181));
        QFont font;
        font.setPointSize(36);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        retranslateUi(wakeUpDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), wakeUpDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), wakeUpDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(wakeUpDlg);
    } // setupUi

    void retranslateUi(QDialog *wakeUpDlg)
    {
        wakeUpDlg->setWindowTitle(QApplication::translate("wakeUpDlg", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("wakeUpDlg", "WAKE UP!!!", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class wakeUpDlg: public Ui_wakeUpDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WAKEUPDLG_H
