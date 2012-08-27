#include "adddeledlg.h"
#include "ui_adddeledlg.h"

addDeLeDlg::addDeLeDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addDeLeDlg)
{
    ui->setupUi(this);

    ui_sun = qFindChild<QCheckBox *>(this, "sun_ck");
    ui_mercury = qFindChild<QCheckBox *>(this, "mercury_ck");
    ui_venus = qFindChild<QCheckBox *>(this, "venus_ck");
    ui_mars = qFindChild<QCheckBox *>(this, "mars_ck");
    ui_jupiter = qFindChild<QCheckBox *>(this, "jupiter_ck");
    ui_saturn = qFindChild<QCheckBox *>(this, "saturn_ck");
    ui_uranus = qFindChild<QCheckBox *>(this, "uranus_ck");
    ui_neptune = qFindChild<QCheckBox *>(this, "neptune_ck");
    ui_pluto = qFindChild<QCheckBox *>(this, "pluto_ck");

    ui_all = qFindChild<QCheckBox *>(this, "all_ck");

    connect(ui_sun, SIGNAL(stateChanged(int)), this, SLOT(slotChangePlanets()));
    connect(ui_mercury, SIGNAL(stateChanged(int)), this, SLOT(slotChangePlanets()));
    connect(ui_venus, SIGNAL(stateChanged(int)), this, SLOT(slotChangePlanets()));
    connect(ui_mars, SIGNAL(stateChanged(int)), this, SLOT(slotChangePlanets()));
    connect(ui_jupiter, SIGNAL(stateChanged(int)), this, SLOT(slotChangePlanets()));
    connect(ui_saturn, SIGNAL(stateChanged(int)), this, SLOT(slotChangePlanets()));
    connect(ui_uranus, SIGNAL(stateChanged(int)), this, SLOT(slotChangePlanets()));
    connect(ui_neptune, SIGNAL(stateChanged(int)), this, SLOT(slotChangePlanets()));
    connect(ui_pluto, SIGNAL(stateChanged(int)), this, SLOT(slotChangePlanets()));

    connect(ui_all, SIGNAL(stateChanged(int)), this, SLOT(slotChangeAll(int)));


}

addDeLeDlg::~addDeLeDlg()
{
    delete ui;
}

void addDeLeDlg::slotChangePlanets()
{
    int st = (ui_sun->checkState()==Qt::Checked&&ui_mercury->checkState()==Qt::Checked&&ui_venus->checkState()==Qt::Checked&&ui_mars->checkState()==Qt::Checked&&ui_jupiter->checkState()==Qt::Checked&&ui_saturn->checkState()==Qt::Checked&&ui_uranus->checkState()==Qt::Checked&&ui_neptune->checkState()==Qt::Checked&&ui_pluto->checkState()==Qt::Checked);
     ui_all->blockSignals(true);
    if(st) ui_all->setChecked(Qt::Checked);
    else ui_all->setChecked(Qt::Unchecked);
     ui_all->blockSignals(false);
}

void addDeLeDlg::slotChangeAll(int state)
{
    ui_all->blockSignals(true);
    //int state = ui_all->isChecked();//==Qt::Checked;
    ui_sun->setChecked(state);
    ui_mercury->setChecked(state);
    ui_venus->setChecked(state);
    ui_mars->setChecked(state);
    ui_jupiter->setChecked(state);
    ui_saturn->setChecked(state);
    ui_uranus->setChecked(state);
    ui_neptune->setChecked(state);
    ui_pluto->setChecked(state);
    ui_all->blockSignals(false);
}
