#ifndef ADDRECDIALOG_H
#define ADDRECDIALOG_H

#include <QtGui/QDialog>
#include <QComboBox>
#include <QDateEdit>
#include <QTimeEdit>
#include <QLineEdit>

namespace Ui {
    class addRecDialog;
}

class addRecDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(addRecDialog)
public:
    explicit addRecDialog(QWidget *parent = 0);
    virtual ~addRecDialog();

    QDateEdit *dateRec;
    QTimeEdit *timeRealRec;
    QTimeEdit *timeHronoRec;

    QLineEdit *comment;

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::addRecDialog *m_ui;
};

#endif // ADDRECDIALOG_H
