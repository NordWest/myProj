#include "settwindow.h"
#include "ui_settwindow.h"

settWindow::settWindow(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::settWindow)
{
    ui->setupUi(this);

    ra0SpinBox = qFindChild<QDoubleSpinBox*>(this, "ra0SpinBox");
    ra1SpinBox = qFindChild<QDoubleSpinBox*>(this, "ra1SpinBox");

    isAutoRA = qFindChild<QCheckBox*>(this, "isAutoRA");
    dMeriSpinBox = qFindChild<QDoubleSpinBox*>(this, "dMeriSpinBox");

    dec0SpinBox = qFindChild<QDoubleSpinBox*>(this, "dec0SpinBox");
    dec1SpinBox = qFindChild<QDoubleSpinBox*>(this, "dec1SpinBox");

    magn0SpinBox = qFindChild<QDoubleSpinBox*>(this, "magn0SpinBox");
    magn1SpinBox = qFindChild<QDoubleSpinBox*>(this, "magn1SpinBox");

    expSpinBox = qFindChild<QSpinBox*>(this, "exptimeSpinBox");
    orderCombo = qFindChild<QComboBox*>(this, "orderCombo");
}

settWindow::~settWindow()
{
    delete ui;
}

void settWindow::on_ra0Slider_sliderMoved(int position)
{
    ui->ra0SpinBox->setValue(position);
}

void settWindow::on_ra1Slider_sliderMoved(int position)
{
    ui->ra1SpinBox->setValue(position);
}



void settWindow::on_ra0SpinBox_valueChanged(double arg1)
{
    ui->ra0Slider->setValue(arg1);
}

void settWindow::on_ra1SpinBox_valueChanged(double arg1)
{
    ui->ra1Slider->setValue(arg1);
}

void settWindow::on_dec0Slider_sliderMoved(int position)
{
    ui->dec0SpinBox->setValue(position);
}

void settWindow::on_dec1Slider_sliderMoved(int position)
{
    ui->dec1SpinBox->setValue(position);
}

void settWindow::on_dec0SpinBox_valueChanged(double arg1)
{
    ui->dec0Slider->setValue(arg1);
}

void settWindow::on_dec1SpinBox_valueChanged(double arg1)
{
    ui->dec1Slider->setValue(arg1);
}

void settWindow::on_magn0Slider_sliderMoved(int position)
{
    ui->magn0SpinBox->setValue(position);
}

void settWindow::on_magn1Slider_sliderMoved(int position)
{
    ui->magn1SpinBox->setValue(position);
}

void settWindow::on_magn0SpinBox_valueChanged(double arg1)
{
    ui->magn0Slider->setValue(arg1);
}

void settWindow::on_magn1SpinBox_valueChanged(double arg1)
{
    ui->magn1Slider->setValue(arg1);
}

void settWindow::on_dMeriSlider_sliderMoved(int position)
{
    ui->dMeriSpinBox->setValue(position);
}

void settWindow::on_dMeriSpinBox_valueChanged(double arg1)
{
    ui->dMeriSlider->setValue(arg1);
}

void settWindow::on_isAutoRA_clicked(bool checked)
{

}

void settWindow::on_isAutoRA_stateChanged(int arg1)
{
    ui->dMeriSlider->setEnabled(arg1);
    ui->dMeriSpinBox->setEnabled(arg1);
}
