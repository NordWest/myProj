/********************************************************************************
** Form generated from reading UI file 'autosetuptimerdlg.ui'
**
** Created: Fri 1. Apr 04:43:20 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUTOSETUPTIMERDLG_H
#define UI_AUTOSETUPTIMERDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_autoSetupTimerDlg
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *timeRemainLabel;
    QCheckBox *doneCheck;
    QPushButton *cancelButton;

    void setupUi(QWidget *autoSetupTimerDlg)
    {
        if (autoSetupTimerDlg->objectName().isEmpty())
            autoSetupTimerDlg->setObjectName(QString::fromUtf8("autoSetupTimerDlg"));
        autoSetupTimerDlg->resize(284, 163);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(autoSetupTimerDlg->sizePolicy().hasHeightForWidth());
        autoSetupTimerDlg->setSizePolicy(sizePolicy);
        verticalLayoutWidget = new QWidget(autoSetupTimerDlg);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(-1, -1, 282, 161));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        timeRemainLabel = new QLabel(verticalLayoutWidget);
        timeRemainLabel->setObjectName(QString::fromUtf8("timeRemainLabel"));
        sizePolicy.setHeightForWidth(timeRemainLabel->sizePolicy().hasHeightForWidth());
        timeRemainLabel->setSizePolicy(sizePolicy);
        timeRemainLabel->setMinimumSize(QSize(280, 50));
        timeRemainLabel->setBaseSize(QSize(280, 50));
        QFont font;
        font.setPointSize(20);
        timeRemainLabel->setFont(font);
        timeRemainLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(timeRemainLabel);

        doneCheck = new QCheckBox(verticalLayoutWidget);
        doneCheck->setObjectName(QString::fromUtf8("doneCheck"));
        doneCheck->setMinimumSize(QSize(0, 50));

        verticalLayout->addWidget(doneCheck);

        cancelButton = new QPushButton(verticalLayoutWidget);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setMinimumSize(QSize(0, 30));

        verticalLayout->addWidget(cancelButton);


        retranslateUi(autoSetupTimerDlg);

        QMetaObject::connectSlotsByName(autoSetupTimerDlg);
    } // setupUi

    void retranslateUi(QWidget *autoSetupTimerDlg)
    {
        autoSetupTimerDlg->setWindowTitle(QApplication::translate("autoSetupTimerDlg", "Auto Setup Timer", 0, QApplication::UnicodeUTF8));
        timeRemainLabel->setText(QString());
        doneCheck->setText(QApplication::translate("autoSetupTimerDlg", "Plased", 0, QApplication::UnicodeUTF8));
        cancelButton->setText(QApplication::translate("autoSetupTimerDlg", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class autoSetupTimerDlg: public Ui_autoSetupTimerDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTOSETUPTIMERDLG_H
