#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui>
#include <QMainWindow>
#include <QMessageBox>
#include <QTime>
#include <QLineEdit>
#include <QTableWidget>
#include <QTcpServer>
#include <QNetworkSession>
#include <QNetworkConfigurationManager>
#include <QNetworkConfiguration>
#include <QTcpSocket>


#include "settwindow.h"
#include "./../libs/skyarealf.h"
#include "./../libs/redStat.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QTcpServer;
class QNetworkSession;
QT_END_NAMESPACE


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

    QString datetimeStr;
    QString starTimeStr;
    QString jdTimeStr;
    QDateTime sysDateTime;
    int updaterEnabled;
    QList <QTableWidgetItem *> itemList;

    skyAreaLF sa;
    resList rFile;
    double jDay, lam, s;

    QDockWidget *settDock;
    settWindow *settW;

    QFile logFile;
    QTextStream logStm;

    procData miriadeProcData;
    QString mpcCatName, deleCatName, obsCatName, installDir, obsCode;

private slots:
    void slotUpdateTime();
    void slotUpdateTable();
    void slotStatBarUpdate();

    void slotGrade();

    void slotAddLog();

    void slotStartUpdater();
    void slotStopUpdater();

    void slotUpdateMiri();

    void slotOpenResFileWindow();
    void slotSaveResFileWindow();
    void slotInitResTable();
    void slotViewSettWindow();
    void slotViewNextObj();
    void slotViewPrevObj();
    void slotClearTable();

    void slotHeaderClicked(int colNum);

    void sendCurObject();
    void sessionOpened();

    void slotExit();

    void on_tableWidget_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void on_tableWidget_itemActivated(QTableWidgetItem *item);


private:
    Ui::MainWindow *ui;
    QTimer *timeUpd;
    QTimer *tabUpd;
    QElapsedTimer *tabEla;

    QTcpServer *tcpServer;
    QStringList fortunes;
    QNetworkSession *networkSession;

    QToolBar *mainToolBar;
    QPushButton *nextButton;
    QPushButton *prevButton;
    QPushButton *startButton;
    QPushButton *stopButton;
    QPushButton *gradeButton;

    QPushButton *viewSettButton;

    QLineEdit *sysTimeEdit;
    QLineEdit *starTimeEdit;
    QLineEdit *jdTimeEdit;
    QTableWidget *mainTable;
    QTreeWidget *mainTree;

    QPushButton *miriUpdateButton;

    QMenu *fileMenu;
    QAction *openResAct;
    QAction *saveResAct;
    QAction *exitAct;

    QMenu *viewMenu;
    QAction *viewNextAct;
    QAction *viewPrevAct;
    QAction *viewStartAct;
    QAction *viewStopAct;
    QAction *viewSettAct;
    QAction *clearAct;

    QMenu *instrMenu;
    QAction *gradeAct;

    QDockWidget *infoDock;
    QWidget *infoWidget;
    QLabel *nameLabel;
    QLabel *raLabel;
    QLabel *decLabel;
    QLabel *magnLabel;
    QLabel *muraLabel;
    QLabel *mudecLabel;
    QLabel *expLabel;
    QLabel *tasksLabel;
    QLabel *catNameLabel;


protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
