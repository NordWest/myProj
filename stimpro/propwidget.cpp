#include "propwidget.h"
#include "ui_propwidget.h"

propWidget::propWidget(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::propWidget)
{
    ui->setupUi(this);
}

propWidget::~propWidget()
{
    delete ui;
}
