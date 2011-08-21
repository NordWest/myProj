/********************************************************************************
** Form generated from reading UI file 'setupmarkscolor.ui'
**
** Created: Tue Apr 12 15:08:34 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETUPMARKSCOLOR_H
#define UI_SETUPMARKSCOLOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGraphicsView>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_setupMarksColor
{
public:
    QDialogButtonBox *buttonBox;
    QGraphicsView *grView;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *ipixBtn;
    QPushButton *catBtn;
    QPushButton *refBtn;
    QPushButton *objBtn;
    QPushButton *bgBtn;

    void setupUi(QDialog *setupMarksColor)
    {
        if (setupMarksColor->objectName().isEmpty())
            setupMarksColor->setObjectName(QString::fromUtf8("setupMarksColor"));
        setupMarksColor->resize(400, 230);
        buttonBox = new QDialogButtonBox(setupMarksColor);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 180, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        grView = new QGraphicsView(setupMarksColor);
        grView->setObjectName(QString::fromUtf8("grView"));
        grView->setGeometry(QRect(10, 10, 381, 71));
        horizontalLayoutWidget = new QWidget(setupMarksColor);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(10, 90, 381, 41));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        ipixBtn = new QPushButton(horizontalLayoutWidget);
        ipixBtn->setObjectName(QString::fromUtf8("ipixBtn"));

        horizontalLayout->addWidget(ipixBtn);

        catBtn = new QPushButton(horizontalLayoutWidget);
        catBtn->setObjectName(QString::fromUtf8("catBtn"));

        horizontalLayout->addWidget(catBtn);

        refBtn = new QPushButton(horizontalLayoutWidget);
        refBtn->setObjectName(QString::fromUtf8("refBtn"));

        horizontalLayout->addWidget(refBtn);

        objBtn = new QPushButton(horizontalLayoutWidget);
        objBtn->setObjectName(QString::fromUtf8("objBtn"));

        horizontalLayout->addWidget(objBtn);

        bgBtn = new QPushButton(setupMarksColor);
        bgBtn->setObjectName(QString::fromUtf8("bgBtn"));
        bgBtn->setGeometry(QRect(170, 140, 75, 23));

        retranslateUi(setupMarksColor);
        QObject::connect(buttonBox, SIGNAL(accepted()), setupMarksColor, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), setupMarksColor, SLOT(reject()));

        QMetaObject::connectSlotsByName(setupMarksColor);
    } // setupUi

    void retranslateUi(QDialog *setupMarksColor)
    {
        setupMarksColor->setWindowTitle(QApplication::translate("setupMarksColor", "Dialog", 0, QApplication::UnicodeUTF8));
        ipixBtn->setText(QApplication::translate("setupMarksColor", "iPix", 0, QApplication::UnicodeUTF8));
        catBtn->setText(QApplication::translate("setupMarksColor", "catPix", 0, QApplication::UnicodeUTF8));
        refBtn->setText(QApplication::translate("setupMarksColor", "refPix", 0, QApplication::UnicodeUTF8));
        objBtn->setText(QApplication::translate("setupMarksColor", "objPix", 0, QApplication::UnicodeUTF8));
        bgBtn->setText(QApplication::translate("setupMarksColor", "bg", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class setupMarksColor: public Ui_setupMarksColor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETUPMARKSCOLOR_H
