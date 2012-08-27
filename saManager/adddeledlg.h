#ifndef ADDDELEDLG_H
#define ADDDELEDLG_H

#include <QDialog>
#include <QCheckBox>

namespace Ui {
class addDeLeDlg;
}

class addDeLeDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit addDeLeDlg(QWidget *parent = 0);
    ~addDeLeDlg();

    QCheckBox *ui_sun;
    QCheckBox *ui_mercury;
    QCheckBox *ui_venus;
    QCheckBox *ui_mars;
    QCheckBox *ui_jupiter;
    QCheckBox *ui_saturn;
    QCheckBox *ui_uranus;
    QCheckBox *ui_neptune;
    QCheckBox *ui_pluto;

    QCheckBox *ui_all;

    
private:
    Ui::addDeLeDlg *ui;

private slots:
    void slotChangePlanets();
    void slotChangeAll(int state);
};

#endif // ADDDELEDLG_H
