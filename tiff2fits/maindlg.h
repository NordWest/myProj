#ifndef MAINDLG_H
#define MAINDLG_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QTextEdit>
#include <QFileDialog>
#include <QString>
#include <QPushButton>
#include <QProgressBar>
#include <QMessageBox>

//#include "./../libs/astro.h"
#include "./../libs/fitsdata.h"


namespace Ui {
    class mainDlg;
}

class mainDlg : public QDialog
{
    Q_OBJECT

public:
    explicit mainDlg(QWidget *parent = 0);
    ~mainDlg();

    //fitsdata *fptr;
    fitsfile *fptr_out;

    QLineEdit *ui_tiffFileName;

    QLineEdit *ui_headerFileName;
    QCheckBox *ui_headerChBox;
    QPushButton *ui_headerFileButton;

    QLineEdit *ui_wcsFileName;
    QCheckBox *ui_wcsChBox;
    QPushButton *ui_wcsFileButton;

    QTextEdit *ui_textEdit;

    QCheckBox *ui_hFlipChBox;
    QCheckBox *ui_vFlipChBox;
    QCheckBox *ui_negativeChBox;

    QProgressBar *ui_progBar;
    QPushButton *ui_procButton;

    void tiffProc();
    void bmpProc();


private:
    Ui::mainDlg *ui;

private slots:
    void on_procButton_clicked();
    void on_wcsFileButton_clicked();
    void on_headerFileButton_clicked();
    void on_wcsChBox_stateChanged(int val);
    void on_headerChBox_stateChanged(int val);
    void on_tiffFileButton_clicked();
};

#endif // MAINDLG_H
