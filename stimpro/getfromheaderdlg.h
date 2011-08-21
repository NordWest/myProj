#ifndef GETFROMHEADERDLG_H
#define GETFROMHEADERDLG_H

#include <QtGui/QDialog>

class QCheckBox;
class QComboBox;
class QLineEdit;

namespace Ui {
    class getFromHeaderDlg;
}

class getFromHeaderDlg : public QDialog {
    Q_OBJECT
public:
    getFromHeaderDlg(QWidget *parent = 0);
    ~getFromHeaderDlg();

    QCheckBox *ui_isConvertToUTC;
    QCheckBox *ui_isSpec;
    //QComboBox *ui_obsList;
    QLineEdit *ui_obsName;
    QComboBox *ui_expList;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::getFromHeaderDlg *m_ui;
};

#endif // GETFROMHEADERDLG_H
