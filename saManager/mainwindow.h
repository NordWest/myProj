#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QMessageBox>

#include "catsview.h"
#include "addtaskdlg.h"
#include "adddeledlg.h"
#include "addmpcdlg.h"
#include "addlspmdlg.h"

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

    void setupMenu();
    void setupWidgets();

    QString workPath;

    QTableWidget *taskTable;
    QTableWidget *objTable;

public slots:
    void show();

    
private:
    Ui::MainWindow *ui;

    QToolBar *toolsB;
    QToolBar *objTools;
    QVBoxLayout *vertLt;

    QMenu *fileMenu;
    QAction *openTaskAct;
    QAction *saveTaskAct;
    QAction *saveAsTaskAct;

    QMenu *taskMenu;
    QAction *addTaskAct;
    QAction *catalogsAct;

    QPushButton *addTaskBtn;
    QPushButton *edtTaskBtn;
    QPushButton *delTaskBtn;
    QPushButton *saveBtn;
    QPushButton *closeBtn;

    QPushButton *addNameListBtn;
    QPushButton *addFromCatBtn;
    QPushButton *remObjBtn;
    QPushButton *updCatBtn;

    skyAreaLF sArea;

void closeEvent(QCloseEvent *event);

private slots:
    void slotOpenTaskList();
    void slotUpdateTaskList();
    void slotSaveTaskList();
    void slotSaveAsTaskList();

    void slotAddTask();
    void slotEdtTask();
    void slotDelTask();
    void slotViewCatalogs();

    void slotChangeFlagTask(int state);
    void slotSaveSA();

    void slotAddNameListObj();
    void slotAddFromCatObj();

    void slotRemSelectedObj();

    void slotUpdateLocalCat();

    void slotUpdateObjList();

    void on_taskTable_cellClicked(int row, int column);

protected:
//     bool eventFilter(QObject *obj, QEvent *ev);
};

#endif // MAINWINDOW_H
