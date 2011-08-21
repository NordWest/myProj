#ifndef SERIALWIDGET_H
#define SERIALWIDGET_H

#include <QtGui>
#include <QWidget>
//#include "SerialGate.h"
#include <qextserialport.h>

class serialWidget : public QWidget
{
    Q_OBJECT
    QextSerialPort * port;

public slots:
    void writeButtonClicked();
    void readButtonClicked();
    void openPortButtonClicked();
    void closePortButtonClicked();
    void writeProgButtonClicked();

    void startProgram();
    void stopProgram();

public slots:

    void loadProgram(QString progStr);

    void updateState();
    void checkState();

signals:
    void stateChanged();

public:
    serialWidget(QWidget *parent = 0);
    ~serialWidget();

    QTimer *readStateTimer;
    QLabel *inputLabel;
    QLineEdit *outputLine;
    QComboBox *portName;
    int keyState;
    int isProg;
private:


    QPushButton *writeButton;
    QPushButton *readButton;

    QPushButton *openPortButton;
    QPushButton *closePortButton;
    QPushButton *writeProgButton;
};

#endif // WIDGET_H
