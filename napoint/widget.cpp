#include "widget.h"

void fromTextToRADE(QString str, double& ra, double& de)
{
    ra = str.section(' ',0,0).toDouble()*3600+
         str.section(' ',1,1).toDouble()*60+
         str.section(' ',2,2).toDouble();
    double sign;
    if(str.section(' ',3,3)[0]=='-')sign=-1; else sign =1;
    de = sign*(sign*str.section(' ',3,3).toDouble()*3600+
         str.section(' ',4,4).toDouble()*60+
         str.section(' ',5,5).toDouble());
}

QString getRaDecMag(QString str)
{
  bool fl = false;
  for(int i=0;i<str.length();i++)
  {
        if((str[i]==' ')||(str[i]=='+')||(str[i]=='-'))
        {
           if(fl)
           {
            if ((str[i]!='+')&&(str[i]!='-'))
                {
                        str[i] = '0';
                        fl = false;
                }
           }
           else
           {
            fl = true;
           }
        }
        else fl = false;
  }
  return str;
}

double numberOfRASteps(double dra, double scale, double speed, double trackingSpeed)
{
    double nOfSteps=fabs(dra)*scale;
    double t = nOfSteps/speed;
    while(t>1)
    {
        if(dra<0)nOfSteps+=t*trackingSpeed;else nOfSteps-=t*trackingSpeed;
        t=t*trackingSpeed/speed;
    }
    return nOfSteps;
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    portRAName = new QComboBox(this);
    portDEName = new QComboBox(this);
    ports = QextSerialEnumerator::getPorts();
    if(ports.count()>0)
    {
        for(int i=0;i<ports.count();i++)
        {
            portRAName->addItem(ports.at(i).friendName);
            portDEName->addItem(ports.at(i).friendName);
        }
        portRA = new QextSerialPort(ports.at(0).portName);
        portDE = new QextSerialPort(ports.at(0).portName);
    }
    //////buttons
    openPortRAButton = new QPushButton("open port",this);
    closePortRAButton = new QPushButton("close port",this);
    openPortDEButton = new QPushButton("open port",this);
    closePortDEButton = new QPushButton("close port",this);
    closePortRAButton->setDisabled(true);
    closePortDEButton->setDisabled(true);
    ////connect slots to signals
    connect(openPortRAButton, SIGNAL(clicked()), this, SLOT(openPortRAButtonClicked()));
    connect(closePortRAButton, SIGNAL(clicked()), this, SLOT(closePortRAButtonClicked()));
    connect(openPortDEButton, SIGNAL(clicked()), this, SLOT(openPortDEButtonClicked()));
    connect(closePortDEButton, SIGNAL(clicked()), this, SLOT(closePortDEButtonClicked()));
    ///place widgets
    QGroupBox *groupBoxRAport = new QGroupBox("port RA",this);
    QGridLayout* portRAGridLayout = new QGridLayout(groupBoxRAport);
    QGroupBox *groupBoxDEport = new QGroupBox("port DE",this);
    QGridLayout* portDEGridLayout = new QGridLayout(groupBoxDEport);
    portRAGridLayout->addWidget(portRAName,0,0);
    portRAGridLayout->addWidget(openPortRAButton,1,0);
    portRAGridLayout->addWidget(closePortRAButton,2,0);
    portDEGridLayout->addWidget(portDEName,0,1);
    portDEGridLayout->addWidget(openPortDEButton,1,1);
    portDEGridLayout->addWidget(closePortDEButton,2,1);
    groupBoxRAport->setLayout(portRAGridLayout);
    groupBoxDEport->setLayout(portDEGridLayout);
    //control widgets
    nofStepsEdit = new QLineEdit(this);
    speedEdit = new QLineEdit(this);
    intStep = new QRadioButton("integer step",this);
    fracStep = new QRadioButton("fractional step",this);
    forwardMotion = new QRadioButton("forward",this);
    backMotion = new QRadioButton("back",this);
    //////////////////////////////////////////
    startPortRAButton = new QPushButton("start RA",this);
    stopPortRAButton = new QPushButton("stop RA",this);
    startPortDEButton = new QPushButton("start DE",this);
    stopPortDEButton = new QPushButton("stop DE",this);
    connect(startPortRAButton, SIGNAL(clicked()), this, SLOT(startPortRAButtonClicked()));
    connect(stopPortRAButton, SIGNAL(clicked()), this, SLOT(stopPortRAButtonClicked()));
    connect(startPortDEButton, SIGNAL(clicked()), this, SLOT(startPortDEButtonClicked()));
    connect(stopPortDEButton, SIGNAL(clicked()), this, SLOT(stopPortDEButtonClicked()));
    loadProgRAButton = new QPushButton("load RA",this);
    loadProgDEButton = new QPushButton("load DE",this);
    connect(loadProgRAButton, SIGNAL(clicked()), this, SLOT(loadProgRAButtonClicked()));
    connect(loadProgDEButton, SIGNAL(clicked()), this, SLOT(loadProgDEButtonClicked()));
    ///////////////////////////////////////////
    replyLabel = new QLabel("port reply",this);
    ///////////////////////////////////////////
    QGroupBox *groupControl = new QGroupBox("control",this);
    QGridLayout* gridControl = new QGridLayout(groupControl);
    gridControl->addWidget(nofStepsEdit,0,0);
    gridControl->addWidget(speedEdit,0,1);
    QLabel *label1 = new QLabel("number of steps",this);
    QLabel *label2 = new QLabel("steps per sec",this);
    gridControl->addWidget(label1,1,0);
    gridControl->addWidget(label2,1,1);
    QGroupBox *groupFracInt= new QGroupBox("mode",this);
        QVBoxLayout *vboxFracInt = new QVBoxLayout;
         vboxFracInt->addWidget(intStep);
         vboxFracInt->addWidget(fracStep);
         vboxFracInt->addStretch(1);
    groupFracInt->setLayout(vboxFracInt);
    QGroupBox *groupDirection= new QGroupBox("direction",this);
        QVBoxLayout *vboxDirection = new QVBoxLayout;
         vboxDirection->addWidget(forwardMotion);
         vboxDirection->addWidget(backMotion);
         vboxDirection->addStretch(1);
    groupDirection->setLayout(vboxDirection);
    gridControl->addWidget(groupFracInt,2,0);
    gridControl->addWidget(groupDirection,2,1);
    gridControl->addWidget(loadProgRAButton,3,0);
    gridControl->addWidget(startPortRAButton,4,0);
    gridControl->addWidget(stopPortRAButton,5,0);
    gridControl->addWidget(loadProgDEButton,3,1);
    gridControl->addWidget(startPortDEButton,4,1);
    gridControl->addWidget(stopPortDEButton,5,1);
    groupControl->setLayout(gridControl);
    //
    radeLineEdit1 = new QLineEdit(this);
    radeLineEdit1 -> setInputMask("99 99 99.999 #99 99 99.99");
    radeLineEdit1 -> setText ("00 00 00.000 +00 00 00.00");
    radeLineEdit2 = new QLineEdit(this);
    radeLineEdit2 -> setInputMask("99 99 99.999 #99 99 99.99");
    radeLineEdit2 -> setText ("00 00 00.000 +00 00 00.00");
    loadDataFromEAButton1 = new QPushButton("...",this);
    loadDataFromEAButton2 = new QPushButton("...",this);
    parametersButton = new QPushButton("parameters",this);
    moveButton = new QPushButton("move telescope",this);
    moveDeButton = new QPushButton("move in Dec",this);
    moveRaButton = new QPushButton("move in RA",this);
    parkButton = new QPushButton("park telescope",this);
    connect(loadDataFromEAButton1, SIGNAL(clicked()), this, SLOT(loadDataFromEAButton1Clicked()));
    connect(loadDataFromEAButton2, SIGNAL(clicked()), this, SLOT(loadDataFromEAButton2Clicked()));
    connect(parametersButton, SIGNAL(clicked()), this, SLOT(parametersButtonClicked()));
    connect(moveButton, SIGNAL(clicked()), this, SLOT(moveButtonClicked()));
    connect(moveDeButton, SIGNAL(clicked()), this, SLOT(moveDeButtonClicked()));
    connect(moveRaButton, SIGNAL(clicked()), this, SLOT(moveRaButtonClicked()));
    connect(parkButton, SIGNAL(clicked()), this, SLOT(parkButtonClicked()));
    ///
    QGroupBox *groupPoints= new QGroupBox("points",this);
    QGridLayout *gridPoints = new QGridLayout;
    QLabel *labelRADE1 = new QLabel("from: ");
    QLabel *labelRADE2 = new QLabel("to: ");
    gridPoints->addWidget(labelRADE1,0,0);
    gridPoints->addWidget(labelRADE2,1,0);
    gridPoints->addWidget(radeLineEdit1,0,1);
    gridPoints->addWidget(radeLineEdit2,1,1);
    gridPoints->addWidget(loadDataFromEAButton1,0,2);
    gridPoints->addWidget(loadDataFromEAButton2,1,2);
    gridPoints->addWidget(parametersButton,2,1);
    gridPoints->addWidget(moveButton,3,1);
    gridPoints->addWidget(moveDeButton,4,1);
    gridPoints->addWidget(moveRaButton,5,1);
    gridPoints->addWidget(parkButton,6,1);
    groupPoints->setLayout(gridPoints);
    //
    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->addWidget(groupBoxRAport,0,0);
    gridLayout->addWidget(groupBoxDEport,0,1);
    gridLayout->addWidget(groupPoints,1,0);
    gridLayout->addWidget(groupControl,1,1);
    gridLayout->addWidget(replyLabel,2,1);
    //
    fracStep->setChecked(true);
    //forwardMotion->setChecked(true);
    backMotion->setChecked(true);
    nofStepsEdit->setInputMask("99999999");
    nofStepsEdit->setText("00000000");
    speedEdit->setInputMask("99999");
    speedEdit->setText("00089");
    ///////////////////////////////
    ///parameters widget
    parametersWidget = new QWidget;
    speedRA = new QLineEdit(parametersWidget);//speed for pointing in RA
    speedRA->setInputMask("99999");
    speedRA->setText("00000");
    speedDE = new QLineEdit(parametersWidget);//speed for pointing in Dec
    speedDE->setInputMask("99999");
    speedDE->setText("00000");
    speedRAHM = new QLineEdit(parametersWidget);//speed for tracking in RA
    speedRAHM->setInputMask("99999");
    speedRAHM->setText("00000");
    scaleRA = new QLineEdit(parametersWidget);//scale for RA (steps per sec)
    scaleRA->setInputMask("99999");
    scaleRA->setText("00000");
    scaleDE = new QLineEdit(parametersWidget);//scale for Dec (steps per arcsec)
    scaleDE->setInputMask("99999");
    scaleDE->setText("00000");
    luftRA = new QLineEdit(parametersWidget);//luft in RA (steps)
    luftRA->setInputMask("99999");
    luftRA->setText("00000");
    luftDE = new QLineEdit(parametersWidget);//luft in Dec (steps)
    luftDE->setInputMask("99999");
    luftDE->setText("00000");
    maxNumberOfStepDE = new QLineEdit(parametersWidget);//max num of steps in Dec(steps)
    maxNumberOfStepDE->setInputMask("99999999");
    maxNumberOfStepDE->setText("00000000");
    delayRA = new QLineEdit(parametersWidget);//delay RA (steps)
    delayRA->setInputMask("99999999");
    delayRA->setText("00000000");
    ///
    QLabel *speedRALabel = new QLabel("speed in RA (step/sec)",parametersWidget);
    QLabel *speedDELabel = new QLabel("speed in Dec (step/sec)",parametersWidget);
    QLabel *speedRAHMLabel = new QLabel("tracking speed in RA (step/sec) ",parametersWidget);
    QLabel *scaleRALabel = new QLabel("scale in RA (step/sec)",parametersWidget);
    QLabel *scaleDELabel = new QLabel("scale in Dec (step/arcsec)",parametersWidget);
    QLabel *luftRALabel = new QLabel("backlash in RA (step)",parametersWidget);
    QLabel *luftDELabel = new QLabel("backlash in Dec (step)",parametersWidget);
    QLabel *maxNumberOfStepDELabel = new QLabel("max step in Dec",parametersWidget);
    QLabel *delayRALabel = new QLabel("delay in RA (sec)",parametersWidget);
    adoptButton = new QPushButton("apply",parametersWidget);
    connect(adoptButton, SIGNAL(clicked()), this, SLOT(adoptButtonClicked()));
    //
    QGroupBox *groupParameters= new QGroupBox("parameters",parametersWidget);
    QGridLayout *gridParameters = new QGridLayout;
    gridParameters->addWidget(speedRALabel,0,0);
    gridParameters->addWidget(speedRALabel,1,0);
    gridParameters->addWidget(speedDELabel,2,0);
    gridParameters->addWidget(speedRAHMLabel,3,0);
    gridParameters->addWidget(scaleRALabel,4,0);
    gridParameters->addWidget(scaleDELabel,5,0);
    gridParameters->addWidget(luftRALabel,6,0);
    gridParameters->addWidget(luftDELabel,7,0);
    gridParameters->addWidget(maxNumberOfStepDELabel,8,0);
    gridParameters->addWidget(delayRALabel,9,0);
    //
    gridParameters->addWidget(speedRA,0,1);
    gridParameters->addWidget(speedRA,1,1);
    gridParameters->addWidget(speedDE,2,1);
    gridParameters->addWidget(speedRAHM,3,1);
    gridParameters->addWidget(scaleRA,4,1);
    gridParameters->addWidget(scaleDE,5,1);
    gridParameters->addWidget(luftRA,6,1);
    gridParameters->addWidget(luftDE,7,1);
    gridParameters->addWidget(maxNumberOfStepDE,8,1);
    gridParameters->addWidget(delayRA,9,1);
    gridParameters->addWidget(adoptButton,10,1);
    groupParameters->setLayout(gridParameters);
    QGridLayout *gridProp = new QGridLayout(parametersWidget);
    gridProp->addWidget(groupParameters,0,0);
    //////////////////
    settings = new QSettings(QApplication::applicationDirPath()+"/napoint.ini",
                             QSettings::IniFormat);
    speed_RA = settings->value("general/speedRA").toDouble();//speed for pointing in RA
    speed_DE = settings->value("general/speedDE").toDouble();//speed for pointing in Dec
    speed_RAHM = settings->value("general/speedRAHM").toDouble();//speed for tracking in RA
    scale_RA = settings->value("general/scaleRA").toDouble();//scale for RA (steps per minute)
    scale_DE = settings->value("general/scaleDE").toDouble();//scale for Dec (steps per arcmin)
    luft_RA = settings->value("general/luftRA").toDouble();//backlash in RA (steps)
    luft_DE = settings->value("general/luftDE").toDouble();//backlash in Dec (steps)
    maxNumberOfStep_DE = settings->value("general/maxNumberOfStepDE").toDouble();
    currentStep_DE = settings->value("general/currentStepDE").toDouble();
    luftStatus_RA = settings->value("general/luftStatusRA").toInt();
    delay_RA = settings->value("general/delayRA").toInt();
    //////////////////////////////////////
    speedRA->setText(QString("%1").arg(speed_RA,5,'f',0,QLatin1Char( ' ' )));
    speedDE->setText(QString("%1").arg(speed_DE,5,'f',0,QLatin1Char( ' ' )));
    speedRAHM->setText(QString("%1").arg(speed_RAHM,5,'f',0,QLatin1Char( ' ' )));
    scaleRA->setText(QString("%1").arg(scale_RA,5,'f',0,QLatin1Char( ' ' )));
    scaleDE->setText(QString("%1").arg(scale_DE,5,'f',0,QLatin1Char( ' ' )));
    luftRA->setText(QString("%1").arg(luft_RA,5,'f',0,QLatin1Char( ' ' )));
    luftDE->setText(QString("%1").arg(luft_DE,5,'f',0,QLatin1Char( ' ' )));
    maxNumberOfStepDE->setText(QString("%1").arg(maxNumberOfStep_DE,5,'f',0,QLatin1Char( ' ' )));
    delayRA->setText(QString("%1").arg(delay_RA,8,'f',0,QLatin1Char( ' ' )));

    stepDEs = 0;
}

Widget::~Widget()
{

}


void Widget::openPortRAButtonClicked()
{
    portRA->setPortName(ports.at(portRAName->currentIndex()).portName);
    if(portRA->open(QIODevice::ReadWrite | QIODevice::Unbuffered))
    {
        portRA->setBaudRate(BAUD9600);
        portRA->setFlowControl(FLOW_OFF);
        portRA->setParity(PAR_EVEN);
        portRA->setDataBits(DATA_8);
        portRA->setStopBits(STOP_1);
        //set timeouts to 500 ms
        portRA->setTimeout(500);
        portRAName->setDisabled(true);
        openPortRAButton->setDisabled(true);
        closePortRAButton->setDisabled(false);
    }
    else
        QMessageBox::information(0,"napoint",
                  ports.at(portRAName->currentIndex()).friendName +
                  " is not opened",QMessageBox::Ok,0,0);
}
void Widget::closePortRAButtonClicked()
{
    if(portRA->isOpen())
    {
        portRA->close();
        portRAName->setDisabled(false);
        openPortRAButton->setDisabled(false);
        closePortRAButton->setDisabled(true);
    }
}
void Widget::openPortDEButtonClicked()
{
    portDE->setPortName(ports.at(portDEName->currentIndex()).portName);
    if(portDE->open(QIODevice::ReadWrite | QIODevice::Unbuffered))
    {
        portDEName->setDisabled(true);
        openPortDEButton->setDisabled(true);
        closePortDEButton->setDisabled(false);
        portDE->setBaudRate(BAUD9600);
        portDE->setFlowControl(FLOW_OFF);
        portDE->setParity(PAR_EVEN);
        portDE->setDataBits(DATA_8);
        portDE->setStopBits(STOP_1);
        //set timeouts to 500 ms
        portDE->setTimeout(500);
    }
    else
        QMessageBox::information(0,"napoint",
                  ports.at(portDEName->currentIndex()).friendName +
                  " is not opened",QMessageBox::Ok,0,0);
}
void Widget::closePortDEButtonClicked()
{
    if(portDE->isOpen())
    {
        portDE->close();
        portDEName->setDisabled(false);
        openPortDEButton->setDisabled(false);
        closePortDEButton->setDisabled(true);
    }
}

void Widget::loadProgRAButtonClicked(){loadProgramToSMSD3(portRA);}
void Widget::loadProgDEButtonClicked(){loadProgramToSMSD3(portDE);}

void Widget::startPortRAButtonClicked(){startProgramAtSMSD3(portRA);}
void Widget::stopPortRAButtonClicked(){stopProgramAtSMSD3(portRA);}
void Widget::startPortDEButtonClicked(){startProgramAtSMSD3(portDE);}
void Widget::stopPortDEButtonClicked(){stopProgramAtSMSD3(portDE);}

void Widget::loadProgramToSMSD3(QextSerialPort *port)
{
    QStringList sl;//LD1*BG*EN*AL0*SD89*DL*ON*MV*ED*ST1
    sl<<"LD1*"<<"BG*"<<"EN*"<<"AL0*";
    sl<<"SD"+speedEdit->text()+"*";
    if(forwardMotion->isChecked())sl<<"DL*";else sl<<"DR*";
    if(fracStep->isChecked())sl<<"ON*";else sl<<"OF*";
    if(nofStepsEdit->text().toInt()==0) sl<<"MV*";
        else sl<<"MV"+nofStepsEdit->text()+"*";
     sl<<"ED*";
     /////////////
     int k;
     QString str,incStr="";
     //for(int i=0;i<sl.count();i++)incStr+=sl[i];
     QByteArray ba;
     for(int i=0;i<sl.count();i++)
     {
         str = sl[i];
         while(!port->isWritable())QApplication::processEvents();
         k = port->write(str.toAscii(),str.length());
         if(k==-1){QMessageBox::information(0,"port","data is not written",QMessageBox::Ok,0,0);break;}
         while(!port->isReadable())QApplication::processEvents();//inputLabel->setText("waiting");
         ba = port->readAll();
         incStr+=QString(ba);
     }
     replyLabel->setText(incStr);
}

void Widget::loadProgramToSMSD3FromList(QextSerialPort *port, QStringList sl)
{
    int k;
    QString str,incStr="";
    QByteArray ba;
    for(int i=0;i<sl.count();i++)
    {
        str = sl[i];
        while(!port->isWritable());
        port->write(qPrintable(str),str.length());
        //if(k==-1){QMessageBox::information(0,"port","data is not written",QMessageBox::Ok,0,0);break;}
        while(!port->isReadable());//QApplication::processEvents();
        ba = port->readAll();
        incStr+=QString(ba);
    }
    replyLabel->setText(incStr);
}

void Widget::startProgramAtSMSD3(QextSerialPort *port)
{
    QString str = "ST1*";
    while(!port->isWritable())QApplication::processEvents();
    int k=port->write(str.toAscii(),str.length());
    if(k==-1){QMessageBox::information(0,"port","data is not written",QMessageBox::Ok,0,0);}
    else
    {
        while(!port->isReadable())QApplication::processEvents();
        QByteArray ba = port->readAll();
        replyLabel->setText(QString(ba));
    }
}
void Widget::stopProgramAtSMSD3(QextSerialPort *port)
{
    QString str = "ST1*";
    while(!port->isWritable())QApplication::processEvents();
    int k=port->write(str.toAscii(),str.length());
    if(k==-1){QMessageBox::information(0,"port","data is not written",QMessageBox::Ok,0,0);}
    else
    {
        while(!port->isReadable())QApplication::processEvents();
        QByteArray ba = port->readAll();
        replyLabel->setText(QString(ba));
    }
}

//////////////////////////////////////////////////////////////////
void Widget::loadDataFromEAButton1Clicked()
{
   QSettings *ea = new QSettings("LAZA","Eassistant");
   radeLineEdit1->setText(getRaDecMag(ea->value("RADEC").toString()));
}
void Widget::loadDataFromEAButton2Clicked()
{
   QSettings *ea = new QSettings("LAZA","Eassistant");
   radeLineEdit2->setText(getRaDecMag(ea->value("RADEC").toString()));
}
void Widget::parametersButtonClicked(){parametersWidget->show();}

void Widget::moveButtonClicked()
{
    //read positions
    double ra1,de1,ra2,de2;
    fromTextToRADE(radeLineEdit1->text(),ra1,de1);
    fromTextToRADE(radeLineEdit2->text(),ra2,de2);
    /*QMessageBox::information(0,"napoint",
                             QString("%1").arg(ra1,16,'f',3,QLatin1Char( ' ' ))+"\n"+
                             QString("%1").arg(de1,16,'f',3,QLatin1Char( ' ' ))+"\n"+
                             QString("%1").arg(ra2,16,'f',3,QLatin1Char( ' ' ))+"\n"+
                             QString("%1").arg(de2,16,'f',3,QLatin1Char( ' ' ))
                             ,QMessageBox::Ok,0,0);*/
    //BEGIN move in RA
    moveRaButtonClicked();
    /*
    //if(portRA->isOpen())stopProgramAtSMSD3(portRA);
    /////////////LD1*BG*EN*AL0*SD89*DL*ON*MV*ED*ST1
    //int nOfStepsRA = (ra2-ra1)*scale_RA;

    if((ra2-ra1)>43200) ra2 -= 86400;
    if((ra2-ra1)<-43200) ra2 += 86400;

    /*QMessageBox::information(0,"napoint",
                             QString("%1").arg(ra1,16,'f',3,QLatin1Char( ' ' ))+"\n"+
                             QString("%1").arg(de1,16,'f',3,QLatin1Char( ' ' ))+"\n"+
                             QString("%1").arg(ra2,16,'f',3,QLatin1Char( ' ' ))+"\n"+
                             QString("%1").arg(de2,16,'f',3,QLatin1Char( ' ' ))
                             ,QMessageBox::Ok,0,0);/

    if(ra2-ra1>0) ra2+=luft_RA/scale_RA-delay_RA;
    else ra2+=delay_RA;
    double nOfStepsRA = numberOfRASteps(ra2-ra1,scale_RA,speed_RA,speed_RAHM);
    QStringList slRA;
    slRA<<"LD1*"<<"BG*"<<"EN*"<<"AL0*"<<"ON*";
    slRA<<"SD"+QString("%1").arg(speed_RA,5,'f',0,QLatin1Char( '0' ))+"*";
    if(ra2-ra1>0)
    {
        slRA<<"DL*";
        slRA<<"MV"+QString("%1").arg((int)nOfStepsRA,8,'f',0,QLatin1Char( '0' ))+"*";
        slRA<<"DR*";
        slRA<<"MV"+QString("%1").arg(luft_RA,8,'f',0,QLatin1Char( '0' ))+"*";
    }
    else
    {
        slRA<<"DR*";
        slRA<<"MV"+QString("%1").arg((int)nOfStepsRA,8,'f',0,QLatin1Char( '0' ))+"*";
    }
    //
    slRA<<"DR*"<<"SD"+QString("%1").arg(speed_RAHM,5,'f',0,QLatin1Char( '0' ))+"*";
    slRA<<"MV*"<<"ED*";
    QString str="";for(int i=0;i<slRA.count();i++)str+=slRA[i];
    QMessageBox::information(0,"napoint",str,QMessageBox::Ok,0,0);
    qDebug()<<str;
    if(portRA->isOpen())
    {
        stopProgramAtSMSD3(portRA);
        loadProgramToSMSD3FromList(portRA,slRA);
        startProgramAtSMSD3(portRA);
    }
    //END move in RA*/
    //BEGIN move in Dec
    moveDeButtonClicked();
    /*
    //forming the list of commands
    int nOfStepsDE = (de2-de1)*scale_DE;
    if((currentStep_DE+nOfStepsDE>0)&&
       (currentStep_DE+nOfStepsDE<maxNumberOfStep_DE))
    {
        QStringList slDE;//LD1*BG*EN*AL0*SD89*DL*ON*MV*ED*ST1
        slDE<<"LD1*"<<"BG*"<<"EN*"<<"AL0*";
        slDE<<"SD"+QString("%1").arg(speed_DE,5,'f',0,QLatin1Char( '0' ))+"*";
        if(nOfStepsDE>0)slDE<<"DL*";else slDE<<"DR*";
        slDE<<"ON*";
        slDE<<"MV"+QString("%1").arg(abs(nOfStepsDE),8,'f',0,QLatin1Char( '0' ))+"*";
        slDE<<"ED*";
         /////////////
        currentStep_DE+=nOfStepsDE;
        settings->setValue("general/currentStepDE",currentStep_DE);//write number of current step in Dec
        //QString str="";for(int i=0;i<slDE.count();i++)str+=slDE[i];
        //QMessageBox::information(0,"napoint",str,QMessageBox::Ok,0,0);
        if(portDE->isOpen())
        {
            loadProgramToSMSD3FromList(portDE,slDE);
            startProgramAtSMSD3(portDE);
        }
    }
    else
    {
        QMessageBox::information(0,"napoint","Dec is out of range",QMessageBox::Ok,0,0);
        return;
    }
    //END move in Dec*/
}

void Widget::moveDeButtonClicked()
{
    //read positions
    double ra1,de1,ra2,de2;
    fromTextToRADE(radeLineEdit1->text(),ra1,de1);
    fromTextToRADE(radeLineEdit2->text(),ra2,de2);
    //BEGIN move in Dec
    //forming the list of commands
    int nOfStepsDE;
    int ts;
    nOfStepsDE = (de2-de1)*scale_DE;
/*QMessageBox::information(0,"napoint",
                             QString("nOfStepsDE: %1\nstepDEs: %2").arg(nOfStepsDE).arg(stepDEs)+"\n"
                            ,QMessageBox::Ok,0,0);*/
    ts = stepDEs;
    stepDEs = abs(nOfStepsDE)/nOfStepsDE;
    if(nOfStepsDE*ts<0)
    {
        nOfStepsDE += stepDEs*luft_DE;
        /*QMessageBox::information(0,"napoint",
                             QString("nOfStepsDE: %1\nstepDEs: %2").arg(nOfStepsDE).arg(stepDEs)+"\n"
                            ,QMessageBox::Ok,0,0);*/
    }
    if((currentStep_DE+nOfStepsDE>0)&&
       (currentStep_DE+nOfStepsDE<maxNumberOfStep_DE))
    {
        QStringList slDE;//LD1*BG*EN*AL0*SD89*DL*ON*MV*ED*ST1
        slDE<<"LD1*"<<"BG*"<<"EN*"<<"AL0*";
        slDE<<"SD"+QString("%1").arg(speed_DE,5,'f',0,QLatin1Char( '0' ))+"*";
        if(nOfStepsDE>0)slDE<<"DL*";else slDE<<"DR*";
        slDE<<"ON*";
        slDE<<"MV"+QString("%1").arg(abs(nOfStepsDE),8,'f',0,QLatin1Char( '0' ))+"*";
        slDE<<"ED*";
         /////////////
        currentStep_DE+=nOfStepsDE;
        settings->setValue("general/currentStepDE",currentStep_DE);//write number of current step in Dec
        //QString str="";for(int i=0;i<slDE.count();i++)str+=slDE[i];
        //QMessageBox::information(0,"napoint",str,QMessageBox::Ok,0,0);
        if(portDE->isOpen())
        {
            loadProgramToSMSD3FromList(portDE,slDE);
            startProgramAtSMSD3(portDE);
        }
    }
    else
    {
        QMessageBox::information(0,"napoint","Dec is out of range",QMessageBox::Ok,0,0);
        return;
    }
    //END move in Dec*/
}

void Widget::moveRaButtonClicked()
{
    //read positions
    double ra1,de1,ra2,de2;
    fromTextToRADE(radeLineEdit1->text(),ra1,de1);
    fromTextToRADE(radeLineEdit2->text(),ra2,de2);
    /*QMessageBox::information(0,"napoint",
                             QString("%1").arg(ra1,16,'f',3,QLatin1Char( ' ' ))+"\n"+
                             QString("%1").arg(de1,16,'f',3,QLatin1Char( ' ' ))+"\n"+
                             QString("%1").arg(ra2,16,'f',3,QLatin1Char( ' ' ))+"\n"+
                             QString("%1").arg(de2,16,'f',3,QLatin1Char( ' ' ))
                             ,QMessageBox::Ok,0,0);*/
    //BEGIN move in RA
    //if(portRA->isOpen())stopProgramAtSMSD3(portRA);
    /////////////LD1*BG*EN*AL0*SD89*DL*ON*MV*ED*ST1
    //int nOfStepsRA = (ra2-ra1)*scale_RA;

    if((ra2-ra1)>43200) ra2 -= 86400;
    if((ra2-ra1)<-43200) ra2 += 86400;

    if(ra2-ra1>0) ra2+=luft_RA/scale_RA-delay_RA;
    else ra2+=delay_RA;
    double nOfStepsRA = numberOfRASteps(ra2-ra1,scale_RA,speed_RA,speed_RAHM);
    QStringList slRA;
    slRA<<"LD1*"<<"BG*"<<"EN*"<<"AL0*"<<"ON*";
    slRA<<"SD"+QString("%1").arg(speed_RA,5,'f',0,QLatin1Char( '0' ))+"*";
    if(ra2-ra1>0)
    {
        slRA<<"DL*";
        slRA<<"MV"+QString("%1").arg((int)nOfStepsRA,8,'f',0,QLatin1Char( '0' ))+"*";
        slRA<<"DR*";
        slRA<<"MV"+QString("%1").arg(luft_RA,8,'f',0,QLatin1Char( '0' ))+"*";
    }
    else
    {
        slRA<<"DR*";
        slRA<<"MV"+QString("%1").arg((int)nOfStepsRA,8,'f',0,QLatin1Char( '0' ))+"*";
    }
    //
    slRA<<"DR*"<<"SD"+QString("%1").arg(speed_RAHM,5,'f',0,QLatin1Char( '0' ))+"*";
    slRA<<"MV*"<<"ED*";
    QString str="";for(int i=0;i<slRA.count();i++)str+=slRA[i];
    //QMessageBox::information(0,"napoint",str,QMessageBox::Ok,0,0);
    qDebug()<<str;
    if(portRA->isOpen())
    {
        stopProgramAtSMSD3(portRA);
        loadProgramToSMSD3FromList(portRA,slRA);
        startProgramAtSMSD3(portRA);
    }
    //END move in RA*/
}


void Widget::parkButtonClicked()
{
    if(portDE->isOpen())
    {
        QStringList slDE;//LD1*BG*EN*AL0*SD89*DL*ON*MV*ED*ST1
        slDE<<"LD1*"<<"BG*"<<"EN*"<<"AL0*";
        slDE<<"SD"+QString("%1").arg(speed_DE,5,'f',0,QLatin1Char( '0' ))+"*";
        slDE<<"DR*";
        slDE<<"ON*";
        slDE<<"MV"+QString("%1").arg(currentStep_DE,8,'f',0,QLatin1Char( '0' ))+"*";
        slDE<<"ED*";
        currentStep_DE = 0;
        settings->setValue("general/currentStepDE",currentStep_DE);
        loadProgramToSMSD3FromList(portDE,slDE);
        startProgramAtSMSD3(portDE);
    }
}

void Widget::adoptButtonClicked()
{
    speed_RA = speedRA->text().toDouble();
    speed_DE = speedDE->text().toDouble();
    speed_RAHM = speedRAHM->text().toDouble();
    scale_RA = scaleRA->text().toDouble();
    scale_DE = scaleDE->text().toDouble();
    luft_RA = luftRA->text().toDouble();
    luft_DE = luftDE->text().toDouble();
    maxNumberOfStep_DE = maxNumberOfStepDE->text().toDouble();
    delay_RA = delayRA->text().toInt();
    ///////////////////////////////////////////////
    settings->setValue("general/speedRA", speed_RA);
    settings->setValue("general/speedDE",speed_DE);
    settings->setValue("general/speedRAHM",speed_RAHM);
    settings->setValue("general/scaleRA",scale_RA);
    settings->setValue("general/scaleDE",scale_DE);
    settings->setValue("general/luftRA",luft_RA);
    settings->setValue("general/luftDE",luft_DE);
    settings->setValue("general/maxNumberOfStepDE",maxNumberOfStep_DE);
    settings->setValue("general/currentStepDE",currentStep_DE);
    settings->setValue("general/luftStatusRA",luftStatus_RA);
    settings->setValue("general/delayRA",delay_RA);
    parametersWidget->hide();
}

void Widget::closeEvent(QCloseEvent *event)
 {
    //save all parameters
    speed_RA = speedRA->text().toDouble();
    speed_DE = speedDE->text().toDouble();
    speed_RAHM = speedRAHM->text().toDouble();
    scale_RA = scaleRA->text().toDouble();
    scale_DE = scaleDE->text().toDouble();
    luft_RA = luftRA->text().toDouble();
    luft_DE = luftDE->text().toDouble();
    maxNumberOfStep_DE = maxNumberOfStepDE->text().toDouble();
    delay_RA = delayRA->text().toInt();
    ///////////////////////////////////////////////
    settings->setValue("general/speedRA", speed_RA);
    settings->setValue("general/speedDE",speed_DE);
    settings->setValue("general/speedRAHM",speed_RAHM);
    settings->setValue("general/scaleRA",scale_RA);
    settings->setValue("general/scaleDE",scale_DE);
    settings->setValue("general/luftRA",luft_RA);
    settings->setValue("general/luftDE",luft_DE);
    settings->setValue("general/maxNumberOfStepDE",maxNumberOfStep_DE);
    settings->setValue("general/currentStepDE",currentStep_DE);
    settings->setValue("general/luftStatusRA",luftStatus_RA);
    settings->setValue("general/delayRA",delay_RA);
    parametersWidget->close();
    QWidget::close();
    //event->ignore();
 }

