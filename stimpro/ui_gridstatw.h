/********************************************************************************
** Form generated from reading UI file 'gridstatw.ui'
**
** Created: Tue Apr 12 15:08:34 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GRIDSTATW_H
#define UI_GRIDSTATW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_gridStatW
{
public:
    QLabel *refPict;
    QLabel *refLabel;
    QLabel *catLabel;
    QLabel *catPict;
    QLabel *ipixPict;
    QLabel *objLabel;
    QLabel *objPict;
    QLabel *ipixLabel;

    void setupUi(QWidget *gridStatW)
    {
        if (gridStatW->objectName().isEmpty())
            gridStatW->setObjectName(QString::fromUtf8("gridStatW"));
        gridStatW->resize(278, 20);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(gridStatW->sizePolicy().hasHeightForWidth());
        gridStatW->setSizePolicy(sizePolicy);
        gridStatW->setWindowTitle(QString::fromUtf8(""));
        refPict = new QLabel(gridStatW);
        refPict->setObjectName(QString::fromUtf8("refPict"));
        refPict->setGeometry(QRect(140, 0, 21, 16));
        refLabel = new QLabel(gridStatW);
        refLabel->setObjectName(QString::fromUtf8("refLabel"));
        refLabel->setGeometry(QRect(160, 0, 46, 16));
        catLabel = new QLabel(gridStatW);
        catLabel->setObjectName(QString::fromUtf8("catLabel"));
        catLabel->setGeometry(QRect(90, 0, 46, 16));
        catPict = new QLabel(gridStatW);
        catPict->setObjectName(QString::fromUtf8("catPict"));
        catPict->setGeometry(QRect(70, 0, 21, 16));
        ipixPict = new QLabel(gridStatW);
        ipixPict->setObjectName(QString::fromUtf8("ipixPict"));
        ipixPict->setGeometry(QRect(0, 0, 21, 16));
        objLabel = new QLabel(gridStatW);
        objLabel->setObjectName(QString::fromUtf8("objLabel"));
        objLabel->setGeometry(QRect(230, 0, 46, 16));
        objPict = new QLabel(gridStatW);
        objPict->setObjectName(QString::fromUtf8("objPict"));
        objPict->setGeometry(QRect(210, 0, 21, 16));
        ipixLabel = new QLabel(gridStatW);
        ipixLabel->setObjectName(QString::fromUtf8("ipixLabel"));
        ipixLabel->setGeometry(QRect(20, 0, 46, 16));

        retranslateUi(gridStatW);

        QMetaObject::connectSlotsByName(gridStatW);
    } // setupUi

    void retranslateUi(QWidget *gridStatW)
    {
        refPict->setText(QString());
        refLabel->setText(QString());
        catLabel->setText(QString());
        ipixPict->setText(QString());
        objLabel->setText(QString());
        objPict->setText(QString());
        ipixLabel->setText(QString());
        Q_UNUSED(gridStatW);
    } // retranslateUi

};

namespace Ui {
    class gridStatW: public Ui_gridStatW {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRIDSTATW_H
