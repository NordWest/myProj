#ifndef PROPWIDGET_H
#define PROPWIDGET_H

#include <QTabWidget>

namespace Ui {
    class propWidget;
}

class propWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit propWidget(QWidget *parent = 0);
    ~propWidget();

private:
    Ui::propWidget *ui;
};

#endif // PROPWIDGET_H
