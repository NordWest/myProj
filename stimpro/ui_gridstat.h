/********************************************************************************
** Form generated from reading ui file 'gridstat.ui'
**
** Created: Thu 17. Dec 22:57:14 2009
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_GRIDSTAT_H
#define UI_GRIDSTAT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_gridStat
{
public:
    QLabel *yellPict;
    QLabel *yellLabel;
    QLabel *bluePict;
    QLabel *blueLabel;
    QLabel *redPict;
    QLabel *redLabel;
    QLabel *greenPict;
    QLabel *greenLabel;

    void setupUi(QDialog *gridStat)
    {
        if (gridStat->objectName().isEmpty())
            gridStat->setObjectName(QString::fromUtf8("gridStat"));
        gridStat->resize(282, 19);
        yellPict = new QLabel(gridStat);
        yellPict->setObjectName(QString::fromUtf8("yellPict"));
        yellPict->setGeometry(QRect(0, 0, 21, 16));
        yellLabel = new QLabel(gridStat);
        yellLabel->setObjectName(QString::fromUtf8("yellLabel"));
        yellLabel->setGeometry(QRect(20, 0, 46, 16));
        bluePict = new QLabel(gridStat);
        bluePict->setObjectName(QString::fromUtf8("bluePict"));
        bluePict->setGeometry(QRect(70, 0, 21, 16));
        blueLabel = new QLabel(gridStat);
        blueLabel->setObjectName(QString::fromUtf8("blueLabel"));
        blueLabel->setGeometry(QRect(90, 0, 46, 16));
        redPict = new QLabel(gridStat);
        redPict->setObjectName(QString::fromUtf8("redPict"));
        redPict->setGeometry(QRect(140, 0, 21, 16));
        redLabel = new QLabel(gridStat);
        redLabel->setObjectName(QString::fromUtf8("redLabel"));
        redLabel->setGeometry(QRect(160, 0, 46, 16));
        greenPict = new QLabel(gridStat);
        greenPict->setObjectName(QString::fromUtf8("greenPict"));
        greenPict->setGeometry(QRect(210, 0, 21, 16));
        greenLabel = new QLabel(gridStat);
        greenLabel->setObjectName(QString::fromUtf8("greenLabel"));
        greenLabel->setGeometry(QRect(230, 0, 46, 16));

        retranslateUi(gridStat);

        QMetaObject::connectSlotsByName(gridStat);
    } // setupUi

    void retranslateUi(QDialog *gridStat)
    {
        gridStat->setWindowTitle(QApplication::translate("gridStat", "Dialog", 0, QApplication::UnicodeUTF8));
        yellPict->setText(QString());
        yellLabel->setText(QString());
        blueLabel->setText(QString());
        redPict->setText(QString());
        redLabel->setText(QString());
        greenPict->setText(QString());
        greenLabel->setText(QString());
        Q_UNUSED(gridStat);
    } // retranslateUi

};

namespace Ui {
    class gridStat: public Ui_gridStat {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRIDSTAT_H
