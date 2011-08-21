#ifndef WAKEUPDLG_H
#define WAKEUPDLG_H

#include <QDialog>

namespace Ui {
    class wakeUpDlg;
}

class wakeUpDlg : public QDialog
{
    Q_OBJECT

public:
    explicit wakeUpDlg(QWidget *parent = 0);
    ~wakeUpDlg();

private:
    Ui::wakeUpDlg *ui;
};

#endif // WAKEUPDLG_H
