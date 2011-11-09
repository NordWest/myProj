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

private slots:
    void slotUpdateTable();
    void slotStatBarUpdate();

    void slotViewSettWindow();

private:
    Ui::MainWindow *ui;
    QTimer *updater;
    QPushButton *settButton;
    QToolBar *mainToolBar;

    QLineEdit *sysTimeEdit;
    QLineEdit *starTimeEdit;
    QLineEdit *jdTimeEdit;
    QTableWidget *mainTable;

    QMenu *fileMenu;
    QAction *viewSettAct;
};

#endif // MAINWINDOW_H
