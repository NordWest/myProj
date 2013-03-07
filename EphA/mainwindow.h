#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui>
#include <QMainWindow>
#include <QMessageBox>
#include <QTime>
#include <QLineEdit>
#include <QTableWidget>

#include "settwindow.h"
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
    void saveSettings();

    void sortRa();
    void sortDec();
    void sortMagn();

    QSettings *sett;

    QString timeStr;
    QString starTimeStr;
    QString jdTimeStr;
    QTime sysTime;
    int updaterEnabled;
    QList <QTableWidgetItem *> itemList;

    skyAreaLF sa;
    resList rFile;
    double jDay, lam, s;

    QDockWidget *settDock;
    settWindow *settW;


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

    void slotExit();

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

    QPushButton *viewSettButton;

    QLineEdit *sysTimeEdit;
    QLineEdit *starTimeEdit;
    QLineEdit *jdTimeEdit;
    QTableWidget *mainTable;
    QTreeWidget *mainTree;

    QMenu *fileMenu;
    QAction *openResAct;
    QAction *exitAct;

    QMenu *viewMenu;
    QAction *viewNextAct;
    QAction *viewPrevAct;
    QAction *viewStartAct;
    QAction *viewStopAct;
    QAction *viewSettAct;
    //QAction *viewSettAct;
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
