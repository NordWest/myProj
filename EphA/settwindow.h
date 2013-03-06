#ifndef SETTWINDOW_H
#define SETTWINDOW_H

#include <QTabWidget>
#include <QDoubleSpinBox>

namespace Ui {
    class settWindow;
}

class settWindow : public QTabWidget
{
    Q_OBJECT

public:
    explicit settWindow(QWidget *parent = 0);
    ~settWindow();

    QDoubleSpinBox *ra0SpinBox, *ra1SpinBox;
    QDoubleSpinBox *dec0SpinBox, *dec1SpinBox;
    QDoubleSpinBox *magn0SpinBox, *magn1SpinBox;
    //double minRa;
    //double maxRa;

private slots:
    void on_ra1Slider_sliderMoved(int position);

    void on_ra0Slider_sliderMoved(int position);

    void on_ra0SpinBox_valueChanged(double arg1);

    void on_ra1SpinBox_valueChanged(double arg1);

    void on_dec0Slider_sliderMoved(int position);

    void on_dec1Slider_sliderMoved(int position);

    void on_dec0SpinBox_valueChanged(double arg1);

    void on_dec1SpinBox_valueChanged(double arg1);

    void on_magn0Slider_sliderMoved(int position);

    void on_magn1Slider_sliderMoved(int position);

    void on_magn0SpinBox_valueChanged(double arg1);

    void on_magn1SpinBox_valueChanged(double arg1);

private:
    Ui::settWindow *ui;
};

#endif // SETTWINDOW_H
