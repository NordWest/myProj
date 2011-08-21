#ifndef GETHTTPDLG_H
#define GETHTTPDLG_H

#include <QtUiTools>
#include <QtGui>
#include "ui_getHttpDlg.h"

#include <QDialog>
#include <QWidget>


class QLabel;
class QPushButton;
class QLineEdit;
class QComboBox;
class QFile;


class getHttpDlg : public QDialog
{
    Q_OBJECT
signals:
    void signalExit();
public:
    getHttpDlg(QWidget *parent = 0);
    QWidget *formWidget;

    QLineEdit *ui_plateNum;
    QComboBox *ui_instr;
private slots:
 //   void on_buttonBox_accepted();
private:
     Ui::getHttpDlg ui;
};

#endif // GETHTTPDLG_H
