/********************************************************************************
** Form generated from reading ui file 'mainwindow.ui'
**
** Created: Fri 3. Jul 14:33:20 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QScrollArea>
#include <QtGui/QStatusBar>
#include <QtGui/QTableWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionClose;
    QAction *actionExit;
    QAction *actionInsert;
    QAction *actionRemove;
    QAction *actionNew;
    QAction *actionInsertAt;
    QWidget *centralWidget;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QTableWidget *tableWidget;
    QMenuBar *menuBar;
    QMenu *menuHronoBase;
    QMenu *menuEdit;
    QMenu *menu;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
    if (MainWindow->objectName().isEmpty())
        MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
    MainWindow->resize(454, 593);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
    MainWindow->setSizePolicy(sizePolicy);
    MainWindow->setMinimumSize(QSize(454, 593));
    MainWindow->setMaximumSize(QSize(744, 593));
    actionOpen = new QAction(MainWindow);
    actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
    actionSave = new QAction(MainWindow);
    actionSave->setObjectName(QString::fromUtf8("actionSave"));
    actionClose = new QAction(MainWindow);
    actionClose->setObjectName(QString::fromUtf8("actionClose"));
    actionExit = new QAction(MainWindow);
    actionExit->setObjectName(QString::fromUtf8("actionExit"));
    actionInsert = new QAction(MainWindow);
    actionInsert->setObjectName(QString::fromUtf8("actionInsert"));
    actionRemove = new QAction(MainWindow);
    actionRemove->setObjectName(QString::fromUtf8("actionRemove"));
    actionNew = new QAction(MainWindow);
    actionNew->setObjectName(QString::fromUtf8("actionNew"));
    actionInsertAt = new QAction(MainWindow);
    actionInsertAt->setObjectName(QString::fromUtf8("actionInsertAt"));
    centralWidget = new QWidget(MainWindow);
    centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
    scrollArea = new QScrollArea(centralWidget);
    scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
    scrollArea->setGeometry(QRect(0, 0, 441, 551));
    scrollArea->setWidgetResizable(true);
    scrollAreaWidgetContents = new QWidget();
    scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
    scrollAreaWidgetContents->setGeometry(QRect(0, 0, 437, 547));
    tableWidget = new QTableWidget(scrollAreaWidgetContents);
    if (tableWidget->columnCount() < 4)
        tableWidget->setColumnCount(4);
    QTableWidgetItem *__colItem = new QTableWidgetItem();
    tableWidget->setHorizontalHeaderItem(0, __colItem);
    QTableWidgetItem *__colItem1 = new QTableWidgetItem();
    tableWidget->setHorizontalHeaderItem(1, __colItem1);
    QTableWidgetItem *__colItem2 = new QTableWidgetItem();
    tableWidget->setHorizontalHeaderItem(2, __colItem2);
    QTableWidgetItem *__colItem3 = new QTableWidgetItem();
    tableWidget->setHorizontalHeaderItem(3, __colItem3);
    tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
    tableWidget->setGeometry(QRect(0, 0, 431, 551));
    QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(tableWidget->sizePolicy().hasHeightForWidth());
    tableWidget->setSizePolicy(sizePolicy1);
    scrollArea->setWidget(scrollAreaWidgetContents);
    MainWindow->setCentralWidget(centralWidget);
    menuBar = new QMenuBar(MainWindow);
    menuBar->setObjectName(QString::fromUtf8("menuBar"));
    menuBar->setGeometry(QRect(0, 0, 454, 19));
    menuHronoBase = new QMenu(menuBar);
    menuHronoBase->setObjectName(QString::fromUtf8("menuHronoBase"));
    menuEdit = new QMenu(menuBar);
    menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
    menu = new QMenu(menuBar);
    menu->setObjectName(QString::fromUtf8("menu"));
    MainWindow->setMenuBar(menuBar);
    statusBar = new QStatusBar(MainWindow);
    statusBar->setObjectName(QString::fromUtf8("statusBar"));
    MainWindow->setStatusBar(statusBar);

    menuBar->addAction(menuHronoBase->menuAction());
    menuBar->addAction(menuEdit->menuAction());
    menuBar->addAction(menu->menuAction());
    menuHronoBase->addAction(actionNew);
    menuHronoBase->addAction(actionOpen);
    menuHronoBase->addAction(actionSave);
    menuHronoBase->addAction(actionClose);
    menuHronoBase->addAction(actionExit);
    menuEdit->addAction(actionInsert);
    menuEdit->addAction(actionInsertAt);
    menuEdit->addAction(actionRemove);

    retranslateUi(MainWindow);
    QObject::connect(actionExit, SIGNAL(triggered()), MainWindow, SLOT(close()));

    QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
    MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
    actionOpen->setText(QApplication::translate("MainWindow", "Open base", 0, QApplication::UnicodeUTF8));
    actionSave->setText(QApplication::translate("MainWindow", "Save", 0, QApplication::UnicodeUTF8));
    actionClose->setText(QApplication::translate("MainWindow", "Close", 0, QApplication::UnicodeUTF8));
    actionExit->setText(QApplication::translate("MainWindow", "Exit", 0, QApplication::UnicodeUTF8));
    actionInsert->setText(QApplication::translate("MainWindow", "Insert", 0, QApplication::UnicodeUTF8));
    actionRemove->setText(QApplication::translate("MainWindow", "Remove", 0, QApplication::UnicodeUTF8));
    actionNew->setText(QApplication::translate("MainWindow", "New", 0, QApplication::UnicodeUTF8));
    actionInsertAt->setText(QApplication::translate("MainWindow", "InsertAt", 0, QApplication::UnicodeUTF8));
    tableWidget->horizontalHeaderItem(0)->setText(QApplication::translate("MainWindow", "Date", 0, QApplication::UnicodeUTF8));
    tableWidget->horizontalHeaderItem(1)->setText(QApplication::translate("MainWindow", "Time real", 0, QApplication::UnicodeUTF8));
    tableWidget->horizontalHeaderItem(2)->setText(QApplication::translate("MainWindow", "Time hrono", 0, QApplication::UnicodeUTF8));
    tableWidget->horizontalHeaderItem(3)->setText(QApplication::translate("MainWindow", "Coment", 0, QApplication::UnicodeUTF8));
    menuHronoBase->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
    menuEdit->setTitle(QApplication::translate("MainWindow", "Edit", 0, QApplication::UnicodeUTF8));
    menu->setTitle(QApplication::translate("MainWindow", "?", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
