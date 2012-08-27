#ifndef ADDTASKDLG_H
#define ADDTASKDLG_H

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QSlider>
#include <QSpinBox>
#include <QComboBox>


namespace Ui {
class addTaskDlg;
}

class addTaskDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit addTaskDlg(QWidget *parent = 0);
    ~addTaskDlg();

    QLineEdit *name;
    QComboBox *catName;
    QLineEdit *expLE;
    QSlider *expSlider;
    QSpinBox *numN;
    QSpinBox *numTot;
    QLineEdit *meriLE;
    QSlider *meriSlider;
    QLineEdit *descText;
    QSpinBox *tExc;

    
private:
    Ui::addTaskDlg *ui;

private slots:
    void slotChangeExp();
    void slotChangeMeri();


};

#endif // ADDTASKDLG_H
