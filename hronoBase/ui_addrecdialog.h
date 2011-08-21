/********************************************************************************
** Form generated from reading ui file 'addrecdialog.ui'
**
** Created: Mon 6. Jul 10:52:49 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_ADDRECDIALOG_H
#define UI_ADDRECDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDateEdit>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QTimeEdit>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_addRecDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QDateEdit *dateEdit;
    QLabel *label_3;
    QTimeEdit *timeReal;
    QLabel *label_4;
    QLabel *label_7;
    QLineEdit *comment;
    QTimeEdit *timeHrono;
    QSpacerItem *verticalSpacer;

    void setupUi(QDialog *addRecDialog)
    {
    if (addRecDialog->objectName().isEmpty())
        addRecDialog->setObjectName(QString::fromUtf8("addRecDialog"));
    addRecDialog->resize(336, 177);
    buttonBox = new QDialogButtonBox(addRecDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setGeometry(QRect(110, 130, 221, 41));
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(buttonBox->sizePolicy().hasHeightForWidth());
    buttonBox->setSizePolicy(sizePolicy);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    gridLayoutWidget = new QWidget(addRecDialog);
    gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
    gridLayoutWidget->setGeometry(QRect(0, 0, 331, 131));
    gridLayout = new QGridLayout(gridLayoutWidget);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    gridLayout->setHorizontalSpacing(6);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    label_2 = new QLabel(gridLayoutWidget);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    gridLayout->addWidget(label_2, 2, 0, 1, 1);

    dateEdit = new QDateEdit(gridLayoutWidget);
    dateEdit->setObjectName(QString::fromUtf8("dateEdit"));
    dateEdit->setCalendarPopup(true);

    gridLayout->addWidget(dateEdit, 2, 2, 1, 1);

    label_3 = new QLabel(gridLayoutWidget);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    gridLayout->addWidget(label_3, 3, 0, 1, 1);

    timeReal = new QTimeEdit(gridLayoutWidget);
    timeReal->setObjectName(QString::fromUtf8("timeReal"));

    gridLayout->addWidget(timeReal, 3, 2, 1, 1);

    label_4 = new QLabel(gridLayoutWidget);
    label_4->setObjectName(QString::fromUtf8("label_4"));

    gridLayout->addWidget(label_4, 4, 0, 1, 1);

    label_7 = new QLabel(gridLayoutWidget);
    label_7->setObjectName(QString::fromUtf8("label_7"));

    gridLayout->addWidget(label_7, 5, 0, 1, 1);

    comment = new QLineEdit(gridLayoutWidget);
    comment->setObjectName(QString::fromUtf8("comment"));

    gridLayout->addWidget(comment, 5, 2, 1, 1);

    timeHrono = new QTimeEdit(gridLayoutWidget);
    timeHrono->setObjectName(QString::fromUtf8("timeHrono"));

    gridLayout->addWidget(timeHrono, 4, 2, 1, 1);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(verticalSpacer, 6, 0, 1, 1);

    QWidget::setTabOrder(dateEdit, timeReal);
    QWidget::setTabOrder(timeReal, timeHrono);
    QWidget::setTabOrder(timeHrono, comment);
    QWidget::setTabOrder(comment, buttonBox);

    retranslateUi(addRecDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), addRecDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), addRecDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(addRecDialog);
    } // setupUi

    void retranslateUi(QDialog *addRecDialog)
    {
    addRecDialog->setWindowTitle(QApplication::translate("addRecDialog", "Dialog", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("addRecDialog", "Date", 0, QApplication::UnicodeUTF8));
    dateEdit->setDisplayFormat(QApplication::translate("addRecDialog", "dd.MM.yyyy", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("addRecDialog", "Time Real", 0, QApplication::UnicodeUTF8));
    timeReal->setDisplayFormat(QApplication::translate("addRecDialog", "hh mm ss.zzz", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("addRecDialog", "Time Hrono", 0, QApplication::UnicodeUTF8));
    label_7->setText(QApplication::translate("addRecDialog", "Comment", 0, QApplication::UnicodeUTF8));
    timeHrono->setDisplayFormat(QApplication::translate("addRecDialog", "hh mm ss.zzz", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(addRecDialog);
    } // retranslateUi

};

namespace Ui {
    class addRecDialog: public Ui_addRecDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDRECDIALOG_H
