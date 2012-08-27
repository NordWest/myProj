#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui>
#include <QMainWindow>
#include <QMessageBox>
#include <QTime>
#include <QLineEdit>
#include <QTableWidget>


#include "./../libs/skyarea.h"

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

    SkyArea sa;

private slots:
    void slotUpdateTable();
    void slotStatBarUpdate();

    void slotViewSettWindow();
    void slotViewNextObj();
    void slotViewPrevObj();

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

    QMenu *fileMenu;
    QAction *viewSettAct;
    QMenu *viewMenu;
    QAction *viewNextAct;
    QAction *viewPrevAct;
};

#endif // MAINWINDOW_H
