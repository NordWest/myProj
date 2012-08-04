#ifndef GRIDSTAT_H
#define GRIDSTAT_H

#include <QtGui/QWidget>

class QLabel;
#include <QPicture>

namespace Ui {
    class gridStat;
}

class gridStat : public QWidget {
    Q_OBJECT
public:
    gridStat(QWidget *parent = 0);
    ~gridStat();

    QLabel *ui_yellPict;
    QLabel *ui_yellLabel;
    QLabel *ui_bluePict;
    QLabel *ui_blueLabel;
    QLabel *ui_redPict;
    QLabel *ui_redLabel;
    QLabel *ui_greenPict;
    QLabel *ui_greenLabel;


protected:
    void changeEvent(QEvent *e);

private:
    Ui::gridStat *m_ui;
};

#endif // GRIDSTAT_H
