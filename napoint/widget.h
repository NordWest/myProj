#ifndef WIDGET_H
#define WIDGET_H

#include <QtGui>
#include <QWidget>
#include <qextserialport.h>
#include <qextserialenumerator.h>

class Widget : public QWidget
{
    Q_OBJECT

private slots:
    void openPortRAButtonClicked();
    void closePortRAButtonClicked();
    void openPortDEButtonClicked();
    void closePortDEButtonClicked();
    void startPortRAButtonClicked();
    void stopPortRAButtonClicked();
    void startPortDEButtonClicked();
    void stopPortDEButtonClicked();
    void loadProgRAButtonClicked();
    void loadProgDEButtonClicked();
    void loadDataFromEAButton1Clicked();
    void loadDataFromEAButton2Clicked();
    void parametersButtonClicked();
    void moveButtonClicked();
    void moveDeButtonClicked();
    void moveRaButtonClicked();
    void adoptButtonClicked();
    void parkButtonClicked();

public:
    Widget(QWidget *parent = 0);
    ~Widget();
    void loadProgramToSMSD3(QextSerialPort *port);
    void startProgramAtSMSD3(QextSerialPort *port);
    void stopProgramAtSMSD3(QextSerialPort *port);
    void loadProgramToSMSD3FromList(QextSerialPort *port, QStringList sl);

private:
    void closeEvent(QCloseEvent *event);
    //ports
    QList<QextPortInfo> ports;
    QextSerialPort *portRA;
    QextSerialPort *portDE;
    //interface elements for ports
    QComboBox *portRAName;
    QComboBox *portDEName;
    QPushButton *openPortRAButton;
    QPushButton *closePortRAButton;
    QPushButton *openPortDEButton;
    QPushButton *closePortDEButton;
    //interface elements for commands
    QLineEdit *nofStepsEdit;
    QLineEdit *speedEdit;
    QRadioButton *intStep;
    QRadioButton *fracStep;
    QRadioButton *forwardMotion;
    QRadioButton *backMotion;
    QPushButton *startPortRAButton;
    QPushButton *stopPortRAButton;
    QPushButton *startPortDEButton;
    QPushButton *stopPortDEButton;
    QPushButton *loadProgRAButton;
    QPushButton *loadProgDEButton;
    QLabel *replyLabel;
    //points
    QLineEdit *radeLineEdit1;
    QLineEdit *radeLineEdit2;
    QPushButton *loadDataFromEAButton1;
    QPushButton *loadDataFromEAButton2;
    QPushButton *parametersButton;
    QPushButton *moveButton;
    QPushButton *parkButton;
    QPushButton *moveDeButton;
    QPushButton *moveRaButton;
    //parameters
    QWidget *parametersWidget;
    QLineEdit *speedRA;//speed for pointing in RA
    QLineEdit *speedDE;//speed for pointing in Dec
    QLineEdit *speedRAHM;//speed for tracking in RA
    QLineEdit *scaleRA;//scale for RA (steps per sec)
    QLineEdit *scaleDE;//scale for Dec (steps per arcsec)
    QLineEdit *luftRA;//luft in RA (steps)
    QLineEdit *luftDE;//luft in Dec (steps)
    QLineEdit *maxNumberOfStepDE;
    QLineEdit *delayRA;//delay RA (sec)
    //
    double speed_RA;//speed for pointing in RA
    double speed_DE;//speed for pointing in Dec
    double speed_RAHM;//speed for tracking in RA
    double scale_RA;//scale for RA (steps per sec)
    double scale_DE;//scale for Dec (steps per arcsec)
    double luft_RA;//luft in RA (steps)
    double luft_DE;//luft in Dec (steps)
    double maxNumberOfStep_DE;//maximum number of steps in Dec
    double currentStep_DE;//current number of step in Dec
    int luftStatus_RA;//status of position of the telescope 0 - last motion to west, 1- to east
    int delay_RA;//time delay in RA in sec(it causes during program loading to device)
    int stepDEs;//signum of prev de step;
    //
    QSettings *settings;
    QPushButton *adoptButton;
};

#endif // WIDGET_H
