#ifndef AUTOSETUPTIMERDLG_H
#define AUTOSETUPTIMERDLG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QTimer>

namespace Ui {
    class autoSetupTimerDlg;
}

class autoSetupTimerDlg : public QWidget
{
    Q_OBJECT

public:
    explicit autoSetupTimerDlg(QWidget *parent = 0);
    ~autoSetupTimerDlg();

    QPushButton *ui_cancelButton;
    QLabel *ui_timeRemainLabel;
    QCheckBox *ui_doneCheck;

    int timerStarted;

    QTimer *timerOne;

private:
    Ui::autoSetupTimerDlg *ui;

private slots:
    void on_autoSetupTimerDlg_rejected();
    void slotCancelTimer();
signals:
    void timerStoped();
};

#endif // AUTOSETUPTIMERDLG_H
