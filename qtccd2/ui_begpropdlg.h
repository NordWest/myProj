/********************************************************************************
** Form generated from reading UI file 'begpropdlg.ui'
**
** Created: Thu 17. Mar 19:53:39 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BEGPROPDLG_H
#define UI_BEGPROPDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_begPropDlg
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *obsLabel;
    QComboBox *obsBox;
    QHBoxLayout *horizontalLayout_4;
    QLabel *filterLabel;
    QComboBox *filtersBox;
    QHBoxLayout *horizontalLayout_2;
    QDoubleSpinBox *focalBox;
    QHBoxLayout *horizontalLayout_3;
    QSpinBox *tempBox;

    void setupUi(QDialog *begPropDlg)
    {
        if (begPropDlg->objectName().isEmpty())
            begPropDlg->setObjectName(QString::fromUtf8("begPropDlg"));
        begPropDlg->resize(400, 202);
        buttonBox = new QDialogButtonBox(begPropDlg);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(50, 160, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        verticalLayoutWidget = new QWidget(begPropDlg);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(0, 0, 401, 151));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        obsLabel = new QLabel(verticalLayoutWidget);
        obsLabel->setObjectName(QString::fromUtf8("obsLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(obsLabel->sizePolicy().hasHeightForWidth());
        obsLabel->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(obsLabel);

        obsBox = new QComboBox(verticalLayoutWidget);
        obsBox->setObjectName(QString::fromUtf8("obsBox"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(obsBox->sizePolicy().hasHeightForWidth());
        obsBox->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(obsBox);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        filterLabel = new QLabel(verticalLayoutWidget);
        filterLabel->setObjectName(QString::fromUtf8("filterLabel"));
        sizePolicy.setHeightForWidth(filterLabel->sizePolicy().hasHeightForWidth());
        filterLabel->setSizePolicy(sizePolicy);
        filterLabel->setMinimumSize(QSize(45, 0));

        horizontalLayout_4->addWidget(filterLabel);

        filtersBox = new QComboBox(verticalLayoutWidget);
        filtersBox->setObjectName(QString::fromUtf8("filtersBox"));
        sizePolicy1.setHeightForWidth(filtersBox->sizePolicy().hasHeightForWidth());
        filtersBox->setSizePolicy(sizePolicy1);

        horizontalLayout_4->addWidget(filtersBox);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        focalBox = new QDoubleSpinBox(verticalLayoutWidget);
        focalBox->setObjectName(QString::fromUtf8("focalBox"));
        focalBox->setValue(33);

        horizontalLayout_2->addWidget(focalBox);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        tempBox = new QSpinBox(verticalLayoutWidget);
        tempBox->setObjectName(QString::fromUtf8("tempBox"));
        tempBox->setMinimum(-100);

        horizontalLayout_3->addWidget(tempBox);


        verticalLayout_2->addLayout(horizontalLayout_3);


        retranslateUi(begPropDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), begPropDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), begPropDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(begPropDlg);
    } // setupUi

    void retranslateUi(QDialog *begPropDlg)
    {
        begPropDlg->setWindowTitle(QApplication::translate("begPropDlg", "Dialog", 0, QApplication::UnicodeUTF8));
        obsLabel->setText(QApplication::translate("begPropDlg", "Observer", "\320\235\320\260\320\261\320\273\321\216\320\264\320\260\321\202\320\265\320\273\321\214", QApplication::UnicodeUTF8));
        filterLabel->setText(QApplication::translate("begPropDlg", "Filter", 0, QApplication::UnicodeUTF8));
        focalBox->setPrefix(QApplication::translate("begPropDlg", "focal length = ", 0, QApplication::UnicodeUTF8));
        focalBox->setSuffix(QApplication::translate("begPropDlg", " mm", 0, QApplication::UnicodeUTF8));
        tempBox->setPrefix(QApplication::translate("begPropDlg", "air temperature = ", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class begPropDlg: public Ui_begPropDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BEGPROPDLG_H
