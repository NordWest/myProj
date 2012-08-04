#ifndef BEGPROPDLG_H
#define BEGPROPDLG_H

#include <QDialog>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QDialogButtonBox>

namespace Ui {
    class begPropDlg;
}

class begPropDlg : public QDialog
{
    Q_OBJECT

public:
    explicit begPropDlg(QWidget *parent = 0);
    ~begPropDlg();

    //QLineEdit *ui_plateNum;
    QComboBox *ui_obsName;
    QComboBox *ui_filtersBox;
    QDoubleSpinBox *ui_focalBox;
    QSpinBox *ui_tempBox;
    QPushButton *ui_okButton;
    //QPushButton *ui_Button;
    QDialogButtonBox *ui_dlgButtons;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::begPropDlg *ui;

    void profOk();

private slots:
    void on_obsBox_currentIndexChanged(int index);
};

#endif // BEGPROPDLG_H
