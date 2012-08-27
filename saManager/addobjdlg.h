#ifndef ADDOBJDLG_H
#define ADDOBJDLG_H

#include <QDialog>

namespace Ui {
class addObjDlg;
}

class addObjDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit addObjDlg(QWidget *parent = 0);
    ~addObjDlg();
    
private:
    Ui::addObjDlg *ui;
};

#endif // ADDOBJDLG_H
