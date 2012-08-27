/********************************************************************************
** Form generated from reading UI file 'findobjectdlg.ui'
**
** Created: Tue Apr 12 15:08:34 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FINDOBJECTDLG_H
#define UI_FINDOBJECTDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_findObjectDlg
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *typeSelectGroup;
    QLineEdit *astNameLE;
    QLineEdit *astNumLE;
    QCheckBox *astNameType;
    QCheckBox *astNumType;
    QCheckBox *isUseExpNum;
    QGroupBox *groupBox;
    QLineEdit *lspmName;
    QCheckBox *lspmType;
    QGroupBox *groupBox_2;
    QCheckBox *lspmFindType;
    QCheckBox *skybotFindType;

    void setupUi(QDialog *findObjectDlg)
    {
        if (findObjectDlg->objectName().isEmpty())
            findObjectDlg->setObjectName(QString::fromUtf8("findObjectDlg"));
        findObjectDlg->resize(400, 296);
        buttonBox = new QDialogButtonBox(findObjectDlg);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(50, 260, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        typeSelectGroup = new QGroupBox(findObjectDlg);
        typeSelectGroup->setObjectName(QString::fromUtf8("typeSelectGroup"));
        typeSelectGroup->setGeometry(QRect(10, 0, 381, 81));
        astNameLE = new QLineEdit(typeSelectGroup);
        astNameLE->setObjectName(QString::fromUtf8("astNameLE"));
        astNameLE->setEnabled(false);
        astNameLE->setGeometry(QRect(110, 20, 261, 20));
        astNumLE = new QLineEdit(typeSelectGroup);
        astNumLE->setObjectName(QString::fromUtf8("astNumLE"));
        astNumLE->setGeometry(QRect(110, 50, 261, 20));
        astNameType = new QCheckBox(typeSelectGroup);
        astNameType->setObjectName(QString::fromUtf8("astNameType"));
        astNameType->setGeometry(QRect(9, 20, 91, 20));
        astNumType = new QCheckBox(typeSelectGroup);
        astNumType->setObjectName(QString::fromUtf8("astNumType"));
        astNumType->setGeometry(QRect(10, 50, 91, 20));
        isUseExpNum = new QCheckBox(findObjectDlg);
        isUseExpNum->setObjectName(QString::fromUtf8("isUseExpNum"));
        isUseExpNum->setGeometry(QRect(20, 250, 111, 18));
        groupBox = new QGroupBox(findObjectDlg);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 80, 381, 61));
        lspmName = new QLineEdit(groupBox);
        lspmName->setObjectName(QString::fromUtf8("lspmName"));
        lspmName->setEnabled(false);
        lspmName->setGeometry(QRect(110, 20, 261, 20));
        lspmType = new QCheckBox(groupBox);
        lspmType->setObjectName(QString::fromUtf8("lspmType"));
        lspmType->setGeometry(QRect(10, 20, 91, 20));
        groupBox_2 = new QGroupBox(findObjectDlg);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 140, 380, 100));
        lspmFindType = new QCheckBox(groupBox_2);
        lspmFindType->setObjectName(QString::fromUtf8("lspmFindType"));
        lspmFindType->setGeometry(QRect(10, 20, 120, 17));
        skybotFindType = new QCheckBox(groupBox_2);
        skybotFindType->setObjectName(QString::fromUtf8("skybotFindType"));
        skybotFindType->setGeometry(QRect(10, 50, 110, 17));

        retranslateUi(findObjectDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), findObjectDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), findObjectDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(findObjectDlg);
    } // setupUi

    void retranslateUi(QDialog *findObjectDlg)
    {
        findObjectDlg->setWindowTitle(QApplication::translate("findObjectDlg", "Dialog", 0, QApplication::UnicodeUTF8));
        typeSelectGroup->setTitle(QApplication::translate("findObjectDlg", "mpeph", 0, QApplication::UnicodeUTF8));
        astNameType->setText(QApplication::translate("findObjectDlg", "astName", 0, QApplication::UnicodeUTF8));
        astNumType->setText(QApplication::translate("findObjectDlg", "astNum", 0, QApplication::UnicodeUTF8));
        isUseExpNum->setText(QApplication::translate("findObjectDlg", "use exposure num", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("findObjectDlg", "lspm", 0, QApplication::UnicodeUTF8));
        lspmType->setText(QApplication::translate("findObjectDlg", "lspmName", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("findObjectDlg", "Find objects", 0, QApplication::UnicodeUTF8));
        lspmFindType->setText(QApplication::translate("findObjectDlg", "Find LSPM", 0, QApplication::UnicodeUTF8));
        skybotFindType->setText(QApplication::translate("findObjectDlg", "Skybot", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class findObjectDlg: public Ui_findObjectDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FINDOBJECTDLG_H
