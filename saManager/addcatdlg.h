#ifndef ADDCATDLG_H
#define ADDCATDLG_H

#include <QDialog>

#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QFileDialog>
#include "./../libs/skyarealf.h"

namespace Ui {
class addCatDlg;
}

class addCatDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit addCatDlg(QWidget *parent = 0);
    ~addCatDlg();

    QPushButton *broseCatBtn;
    QLineEdit *catFile;
    QLineEdit *catName;
    QComboBox *catType;

    QString workPath;

    catRecord cRec;
    void setCatRec(catRecord &catRec);

private slots:
    void on_broseCatBtn_clicked();

private:
    Ui::addCatDlg *ui;


};

#endif // ADDCATDLG_H
