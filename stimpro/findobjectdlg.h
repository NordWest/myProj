#ifndef FINDOBJECTDLG_H
#define FINDOBJECTDLG_H

#include <QtGui/QDialog>
#include <QRadioButton>
#include <QLineEdit>
#include <QCheckBox>

namespace Ui {
    class findObjectDlg;
}

class findObjectDlg : public QDialog {
    Q_OBJECT
public:
    findObjectDlg(QWidget *parent = 0);
    ~findObjectDlg();

    QCheckBox *ui_astNameType;
    QCheckBox *ui_astNumType;
    QLineEdit *ui_astNameLE;
    QLineEdit *ui_astNumLE;
    QCheckBox *ui_lspmType;
    QLineEdit *ui_lspmName;
    QCheckBox *ui_isUseExpNum;

    QCheckBox *ui_lspmFindType;
    QCheckBox *ui_skybotFindType;


protected:
    void changeEvent(QEvent *e);

private:
    Ui::findObjectDlg *m_ui;

private slots:
    void on_skybotFindType_clicked();
    void on_lspmFindType_clicked();
    void on_lspmType_clicked();
    void on_astNameType_clicked();
    void on_astNumType_clicked();
};

#endif // FINDOBJECTDLG_H
