/********************************************************************************
** Form generated from reading UI file 'propwidget.ui'
**
** Created: Tue Apr 12 15:08:34 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROPWIDGET_H
#define UI_PROPWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_propWidget
{
public:
    QWidget *generalTab;
    QWidget *tab;

    void setupUi(QTabWidget *propWidget)
    {
        if (propWidget->objectName().isEmpty())
            propWidget->setObjectName(QString::fromUtf8("propWidget"));
        propWidget->resize(400, 300);
        generalTab = new QWidget();
        generalTab->setObjectName(QString::fromUtf8("generalTab"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(generalTab->sizePolicy().hasHeightForWidth());
        generalTab->setSizePolicy(sizePolicy);
        propWidget->addTab(generalTab, QString());
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        propWidget->addTab(tab, QString());

        retranslateUi(propWidget);

        propWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(propWidget);
    } // setupUi

    void retranslateUi(QTabWidget *propWidget)
    {
        propWidget->setWindowTitle(QApplication::translate("propWidget", "TabWidget", 0, QApplication::UnicodeUTF8));
        propWidget->setTabText(propWidget->indexOf(generalTab), QApplication::translate("propWidget", "Tab 1", 0, QApplication::UnicodeUTF8));
        propWidget->setTabText(propWidget->indexOf(tab), QApplication::translate("propWidget", "Tab 2", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class propWidget: public Ui_propWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROPWIDGET_H
