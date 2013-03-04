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
    int updaterEnabled;
    QList <QTableWidgetItem *> itemList;

    skyAreaLF sa;
    resList rFile;
    double jDay, lam, s;
    int expTime;


private slots:
    void slotUpdateTime();
    void slotUpdateTable();
    void slotStatBarUpdate();

    void slotStartUpdater();
    void slotStopUpdater();

    void slotOpenResFileWindow();
    void slotInitResTable();
    void slotViewSettWindow();
    void slotViewNextObj();
    void slotViewPrevObj();
    void slotClearTable();

    void slotHeaderClicked(int colNum);


    void on_tableWidget_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void on_tableWidget_itemActivated(QTableWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QTimer *timeUpd;
    QTimer *tabUpd;
    QElapsedTimer *tabEla;

    QToolBar *mainToolBar;
    QPushButton *nextButton;
    QPushButton *prevButton;
    QPushButton *startButton;
    QPushButton *stopButton;

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
    QAction *viewStartAct;
    QAction *viewStopAct;
};

#endif // MAINWINDOW_H
