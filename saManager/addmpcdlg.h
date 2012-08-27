#ifndef ADDMPCDLG_H
#define ADDMPCDLG_H

#include <QDialog>
#include <QLineEdit>
#include <QTableWidget>
#include "./../libs/mpccat.h"

namespace Ui {
class addMpcDlg;
}

class addMpcDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit addMpcDlg(QWidget *parent = 0);
    ~addMpcDlg();

    QLineEdit *nameLE;
    QPushButton *searchBtn;
    QPushButton *clearBtn;
    QTableWidget *objTable;

    mpccat *cat;
    QList <mpcrec*> mpcList;

    
private slots:
    void on_searchBtn_clicked();
    void slotUpdateObjTable();

    void on_clearBtn_clicked();

private:
    Ui::addMpcDlg *ui;
};

#endif // ADDMPCDLG_H
