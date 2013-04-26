#ifndef ADDLSPMDLG_H
#define ADDLSPMDLG_H

#include <QDialog>
#include <QLineEdit>
#include <QTableWidget>
#include <QFileDialog>
//#include "./../libs/mpccat.h"
#include "./../libs/sscatFB.h"
#include "./../libs/sscat.h"

namespace Ui {
class addLspmDlg;
}

class addLspmDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit addLspmDlg(QWidget *parent = 0);
    ~addLspmDlg();

    QLineEdit *nameLE;
    QPushButton *searchBtn;
    QPushButton *clearBtn;
    QTableWidget *objTable;

    sscatFB *cat;

    QList <sscat_rec*> lspmList;

    
private slots:
    void on_searchBtn_clicked();
    void slotUpdateObjTable();

    void on_clearBtn_clicked();

    void on_pushButton_clicked();

private:
    Ui::addLspmDlg *ui;
};

#endif // ADDLSPMDLG_H
