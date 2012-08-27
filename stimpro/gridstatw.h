#ifndef GRIDSTATW_H
#define GRIDSTATW_H

#include <QtGui/QWidget>

class QLabel;
#include <QPicture>

namespace Ui {
    class gridStatW;
}

class gridStatW : public QWidget {
    Q_OBJECT
public:
    gridStatW(QWidget *parent = 0);
    ~gridStatW();

    QLabel *ui_ipixPict;
    QLabel *ui_ipixLabel;
    QLabel *ui_catPict;
    QLabel *ui_catLabel;
    QLabel *ui_refPict;
    QLabel *ui_refLabel;
    QLabel *ui_objPict;
    QLabel *ui_objLabel;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::gridStatW *m_ui;
};

#endif // GRIDSTATW_H
