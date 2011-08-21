/********************************************************************************
** Form generated from reading UI file 'maindlg.ui'
**
** Created: Mon Feb 7 14:06:41 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINDLG_H
#define UI_MAINDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mainDlg
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *tiffLayout;
    QLabel *labelTiffFileName;
    QLineEdit *tiffFileName;
    QPushButton *tiffFileButton;
    QHBoxLayout *headerLayout;
    QCheckBox *headerChBox;
    QLineEdit *headerFileName;
    QPushButton *headerFileButton;
    QHBoxLayout *wcsLayout;
    QCheckBox *wcsChBox;
    QLineEdit *wcsFileName;
    QPushButton *wcsFileButton;
    QHBoxLayout *paramLayout;
    QCheckBox *hFlipChBox;
    QCheckBox *vFlipChBox;
    QCheckBox *negativeChBox;
    QTextEdit *textEdit;
    QProgressBar *progBar;
    QPushButton *procButton;

    void setupUi(QDialog *mainDlg)
    {
        if (mainDlg->objectName().isEmpty())
            mainDlg->setObjectName(QString::fromUtf8("mainDlg"));
        mainDlg->resize(400, 531);
        buttonBox = new QDialogButtonBox(mainDlg);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(40, 490, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        verticalLayoutWidget = new QWidget(mainDlg);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 0, 381, 481));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        tiffLayout = new QHBoxLayout();
        tiffLayout->setObjectName(QString::fromUtf8("tiffLayout"));
        labelTiffFileName = new QLabel(verticalLayoutWidget);
        labelTiffFileName->setObjectName(QString::fromUtf8("labelTiffFileName"));

        tiffLayout->addWidget(labelTiffFileName);

        tiffFileName = new QLineEdit(verticalLayoutWidget);
        tiffFileName->setObjectName(QString::fromUtf8("tiffFileName"));

        tiffLayout->addWidget(tiffFileName);

        tiffFileButton = new QPushButton(verticalLayoutWidget);
        tiffFileButton->setObjectName(QString::fromUtf8("tiffFileButton"));

        tiffLayout->addWidget(tiffFileButton);


        verticalLayout->addLayout(tiffLayout);

        headerLayout = new QHBoxLayout();
        headerLayout->setObjectName(QString::fromUtf8("headerLayout"));
        headerChBox = new QCheckBox(verticalLayoutWidget);
        headerChBox->setObjectName(QString::fromUtf8("headerChBox"));

        headerLayout->addWidget(headerChBox);

        headerFileName = new QLineEdit(verticalLayoutWidget);
        headerFileName->setObjectName(QString::fromUtf8("headerFileName"));
        headerFileName->setEnabled(false);

        headerLayout->addWidget(headerFileName);

        headerFileButton = new QPushButton(verticalLayoutWidget);
        headerFileButton->setObjectName(QString::fromUtf8("headerFileButton"));
        headerFileButton->setEnabled(false);

        headerLayout->addWidget(headerFileButton);


        verticalLayout->addLayout(headerLayout);

        wcsLayout = new QHBoxLayout();
        wcsLayout->setObjectName(QString::fromUtf8("wcsLayout"));
        wcsChBox = new QCheckBox(verticalLayoutWidget);
        wcsChBox->setObjectName(QString::fromUtf8("wcsChBox"));
        wcsChBox->setBaseSize(QSize(13, 0));

        wcsLayout->addWidget(wcsChBox);

        wcsFileName = new QLineEdit(verticalLayoutWidget);
        wcsFileName->setObjectName(QString::fromUtf8("wcsFileName"));
        wcsFileName->setEnabled(false);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(wcsFileName->sizePolicy().hasHeightForWidth());
        wcsFileName->setSizePolicy(sizePolicy);
        wcsFileName->setBaseSize(QSize(30, 0));

        wcsLayout->addWidget(wcsFileName);

        wcsFileButton = new QPushButton(verticalLayoutWidget);
        wcsFileButton->setObjectName(QString::fromUtf8("wcsFileButton"));
        wcsFileButton->setEnabled(false);

        wcsLayout->addWidget(wcsFileButton);


        verticalLayout->addLayout(wcsLayout);

        paramLayout = new QHBoxLayout();
        paramLayout->setObjectName(QString::fromUtf8("paramLayout"));
        hFlipChBox = new QCheckBox(verticalLayoutWidget);
        hFlipChBox->setObjectName(QString::fromUtf8("hFlipChBox"));

        paramLayout->addWidget(hFlipChBox);

        vFlipChBox = new QCheckBox(verticalLayoutWidget);
        vFlipChBox->setObjectName(QString::fromUtf8("vFlipChBox"));

        paramLayout->addWidget(vFlipChBox);

        negativeChBox = new QCheckBox(verticalLayoutWidget);
        negativeChBox->setObjectName(QString::fromUtf8("negativeChBox"));

        paramLayout->addWidget(negativeChBox);


        verticalLayout->addLayout(paramLayout);

        textEdit = new QTextEdit(verticalLayoutWidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));

        verticalLayout->addWidget(textEdit);

        progBar = new QProgressBar(verticalLayoutWidget);
        progBar->setObjectName(QString::fromUtf8("progBar"));
        progBar->setValue(24);

        verticalLayout->addWidget(progBar);

        procButton = new QPushButton(verticalLayoutWidget);
        procButton->setObjectName(QString::fromUtf8("procButton"));

        verticalLayout->addWidget(procButton);


        retranslateUi(mainDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), mainDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), mainDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(mainDlg);
    } // setupUi

    void retranslateUi(QDialog *mainDlg)
    {
        mainDlg->setWindowTitle(QApplication::translate("mainDlg", "Dialog", 0, QApplication::UnicodeUTF8));
        labelTiffFileName->setText(QApplication::translate("mainDlg", "TiffFile", 0, QApplication::UnicodeUTF8));
        tiffFileButton->setText(QApplication::translate("mainDlg", "...", 0, QApplication::UnicodeUTF8));
        headerChBox->setText(QApplication::translate("mainDlg", "Header", 0, QApplication::UnicodeUTF8));
        headerFileButton->setText(QApplication::translate("mainDlg", "...", 0, QApplication::UnicodeUTF8));
        wcsChBox->setText(QApplication::translate("mainDlg", "WCS", 0, QApplication::UnicodeUTF8));
        wcsFileButton->setText(QApplication::translate("mainDlg", "...", 0, QApplication::UnicodeUTF8));
        hFlipChBox->setText(QApplication::translate("mainDlg", "Horizontal Flip", 0, QApplication::UnicodeUTF8));
        vFlipChBox->setText(QApplication::translate("mainDlg", "Vertical Flip", 0, QApplication::UnicodeUTF8));
        negativeChBox->setText(QApplication::translate("mainDlg", "Negative", 0, QApplication::UnicodeUTF8));
        procButton->setText(QApplication::translate("mainDlg", "PushButton", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class mainDlg: public Ui_mainDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINDLG_H
