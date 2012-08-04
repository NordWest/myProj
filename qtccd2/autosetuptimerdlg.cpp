#include "autosetuptimerdlg.h"
#include "ui_autosetuptimerdlg.h"

autoSetupTimerDlg::autoSetupTimerDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::autoSetupTimerDlg)
{
    ui->setupUi(this);

    timerStarted = 0;

    ui_cancelButton = qFindChild<QPushButton *>(this, "cancelButton");
    ui_timeRemainLabel = qFindChild<QLabel *>(this, "timeRemainLabel");
    ui_doneCheck = qFindChild<QCheckBox *>(this, "doneCheck");

    //ui_timeRemainLabel

    connect(ui_cancelButton, SIGNAL(clicked()), this, SLOT(slotCancelTimer()));


    timerOne = new QTimer;
}

autoSetupTimerDlg::~autoSetupTimerDlg()
{
    delete ui;
}

void autoSetupTimerDlg::slotCancelTimer()
{

    emit(timerStoped());
}

void autoSetupTimerDlg::on_autoSetupTimerDlg_rejected()
{
    slotCancelTimer();
}
