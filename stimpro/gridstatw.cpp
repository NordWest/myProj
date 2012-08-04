#include "gridstatw.h"
#include "ui_gridstatw.h"

gridStatW::gridStatW(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::gridStatW)
{
    m_ui->setupUi(this);

    ui_ipixPict = qFindChild<QLabel *>(this, "ipixPict");
    ui_ipixLabel = qFindChild<QLabel *>(this, "ui_ipixLabel");
    ui_catPict = qFindChild<QLabel *>(this, "ui_catPict");
    ui_catLabel = qFindChild<QLabel *>(this, "ui_catLabel");
    ui_refPict = qFindChild<QLabel *>(this, "ui_refPict");
    ui_refLabel = qFindChild<QLabel *>(this, "ui_refLabel");
    ui_objPict = qFindChild<QLabel *>(this, "ui_objPict");
    ui_objLabel = qFindChild<QLabel *>(this, "ui_objLabel");
/*
    QPicture ipict;
    ipict.load("./icons/yellow.bmp");
    ui_yellPict->setPicture(ipict);
    ipict.load("./icons/blue.bmp");
    ui_bluePict->setPicture(ipict);
    ipict.load("./icons/red.bmp");
    ui_redPict->setPicture(ipict);
    ipict.load("./icons/green.bmp");
    ui_greenPict->setPicture(ipict);
    */
}

gridStatW::~gridStatW()
{
    delete m_ui;
}

void gridStatW::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
