/********************************************************************************
** Form generated from reading UI file 'getHttpDlg.ui'
**
** Created: Tue Apr 12 15:08:34 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GETHTTPDLG_H
#define UI_GETHTTPDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_getHttpDlg
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *label;
    QLabel *label_2;
    QComboBox *instr;
    QLineEdit *plateNum;

    void setupUi(QDialog *getHttpDlg)
    {
        if (getHttpDlg->objectName().isEmpty())
            getHttpDlg->setObjectName(QString::fromUtf8("getHttpDlg"));
        getHttpDlg->setWindowModality(Qt::NonModal);
        getHttpDlg->resize(400, 108);
        getHttpDlg->setModal(true);
        buttonBox = new QDialogButtonBox(getHttpDlg);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(310, 10, 81, 241));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        gridLayoutWidget = new QWidget(getHttpDlg);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(10, 10, 291, 91));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(gridLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        label_2 = new QLabel(gridLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        instr = new QComboBox(gridLayoutWidget);
        instr->setObjectName(QString::fromUtf8("instr"));

        gridLayout->addWidget(instr, 0, 1, 1, 1);

        plateNum = new QLineEdit(gridLayoutWidget);
        plateNum->setObjectName(QString::fromUtf8("plateNum"));

        gridLayout->addWidget(plateNum, 1, 1, 1, 1);

        QWidget::setTabOrder(plateNum, buttonBox);
        QWidget::setTabOrder(buttonBox, instr);

        retranslateUi(getHttpDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), getHttpDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), getHttpDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(getHttpDlg);
    } // setupUi

    void retranslateUi(QDialog *getHttpDlg)
    {
        getHttpDlg->setWindowTitle(QApplication::translate("getHttpDlg", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("getHttpDlg", "Instrument:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("getHttpDlg", "Plate number:", 0, QApplication::UnicodeUTF8));
        instr->clear();
        instr->insertItems(0, QStringList()
         << QApplication::translate("getHttpDlg", "na", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("getHttpDlg", "26", 0, QApplication::UnicodeUTF8)
        );
    } // retranslateUi

};

namespace Ui {
    class getHttpDlg: public Ui_getHttpDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GETHTTPDLG_H
