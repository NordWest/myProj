#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QKeyEvent>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString fName;
//    QFile fBase;
    int is_edt;
    int colNum;
    QTableWidget *frameList;
    QTableWidgetItem *frameItem;

    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void slotOpen();
    void slotNew();
    void slotInsert();
    void slotInsertAt();
    void slotRemove();
    void slotSave();
    void slotClose();


    void slotEdited();
    void slotSaved();

    bool close();


private:
    Ui::MainWindow *ui;

    void insertStr(QString sLine, int pos);

    void slotKeyRules(QKeyEvent *keyEvent);

};

#endif // MAINWINDOW_H
