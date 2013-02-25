#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui>
#include <QMainWindow>
#include <QMessageBox>
#include <QTime>
#include <QLineEdit>
#include <QTableWidget>


#include "./../libs/skyarealf.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setMenu();
    void setWidgets();
    void setSettings();


    QString timeStr;
    QString starTimeStr;
    QString jdTimeStr;
    QTime sysTime;

    skyAreaLF sa;
    resList rFile;

private slots:
    void slotUpdateTable();
    void slotStatBarUpdate();

    void slotOpenResFileWindow();
    void slotViewSettWindow();
    void slotViewNextObj();
    void slotViewPrevObj();
    void slotClearTable();


private:
    Ui::MainWindow *ui;
    QTimer *updater;

    QToolBar *mainToolBar;
    QPushButton *nextButton;
    QPushButton *prevButton;

    QLineEdit *sysTimeEdit;
    QLineEdit *starTimeEdit;
    QLineEdit *jdTimeEdit;
    QTableWidget *mainTable;
    QTreeWidget *mainTree;

    QMenu *fileMenu;
    QAction *openResAct;
    QAction *viewSettAct;
    QMenu *viewMenu;
    QAction *viewNextAct;
    QAction *viewPrevAct;
};

#endif // MAINWINDOW_H
