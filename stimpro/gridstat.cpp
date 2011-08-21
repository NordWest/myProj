#include "gridstat.h"
#include "ui_gridstat.h"

gridStat::gridStat(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::gridStat)
{
    m_ui->setupUi(this);

    ui_yellPict = qFindChild<QLabel *>(this, "yellPict");
    ui_yellLabel = qFindChild<QLabel *>(this, "ui_yellLabel");
    ui_bluePict = qFindChild<QLabel *>(this, "ui_bluePict");
    ui_blueLabel = qFindChild<QLabel *>(this, "ui_blueLabel");
    ui_redPict = qFindChild<QLabel *>(this, "ui_redPict");
    ui_redLabel = qFindChild<QLabel *>(this, "ui_redLabel");
    ui_greenPict = qFindChild<QLabel *>(this, "ui_greenPict");
    ui_greenLabel = qFindChild<QLabel *>(this, "ui_greenLabel");

    QPicture ipict;
    ipict.load("./icons/yellow.bmp");
    ui_yellPict->setPicture(ipict);
    ipict.load("./icons/blue.bmp");
    ui_bluePict->setPicture(ipict);
    ipict.load("./icons/red.bmp");
    ui_redPict->setPicture(ipict);
    ipict.load("./icons/green.bmp");
    ui_greenPict->setPicture(ipict);
}

gridStat::~gridStat()
{
    delete m_ui;
}

void gridStat::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
