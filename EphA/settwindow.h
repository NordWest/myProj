#ifndef SETTWINDOW_H
#define SETTWINDOW_H

#include <QTabWidget>

namespace Ui {
    class settWindow;
}

class settWindow : public QTabWidget
{
    Q_OBJECT

public:
    explicit settWindow(QWidget *parent = 0);
    ~settWindow();

private:
    Ui::settWindow *ui;
};

#endif // SETTWINDOW_H
