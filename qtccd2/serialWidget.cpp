#include "serialWidget.h"


serialWidget::serialWidget(QWidget *parent)
    : QWidget(parent)
{
    port = new QextSerialPort("COM1");
    //
    portName = new QComboBox(this);
    for(int i=1;i<=20;i++)portName->addItem("COM"+QString("%1").arg(i,1,10,QLatin1Char( ' ' )));
    outputLine = new QLineEdit;
    writeButton = new QPushButton("write",this);
    connect(writeButton, SIGNAL(clicked()), this, SLOT(writeButtonClicked()));
    readButton = new QPushButton("read",this);
    connect(readButton, SIGNAL(clicked()), this, SLOT(readButtonClicked()));
    openPortButton = new QPushButton("open port",this);
    connect(openPortButton, SIGNAL(clicked()), this, SLOT(openPortButtonClicked()));
    closePortButton = new QPushButton("close port",this);
    connect(closePortButton, SIGNAL(clicked()), this, SLOT(closePortButtonClicked()));
    writeProgButton = new QPushButton("write program",this);
    connect(writeProgButton, SIGNAL(clicked()), this, SLOT(writeProgButtonClicked()));
    inputLabel = new QLabel("read data",this);
    QGridLayout *grid = new QGridLayout(this);
    ///////////////////////////////
    grid->addWidget(portName,0,0);
    grid->addWidget(openPortButton,1,0);
    grid->addWidget(closePortButton,2,0);
    grid->addWidget(outputLine,0,1);
    grid->addWidget(writeButton,0,2);
    grid->addWidget(writeProgButton,0,3);
    grid->addWidget(inputLabel,1,1);
    grid->addWidget(readButton,1,2);
    //
    closePortButton->setDisabled(true);

    keyState=-1;
    isProg = 0;

    readStateTimer = new QTimer;
    connect(readStateTimer,  SIGNAL(timeout()), this, SLOT(updateState()));
}

void serialWidget::writeButtonClicked()
{
    int k = port->write(outputLine->text().toAscii(),
                             outputLine->text().length());
    /*if(port->isReadable())
    {
        QByteArray ba = port->readAll();
        inputLabel->setText(QString(ba));
        checkState();
    }*/
    if(k==-1)
    {
        //QMessageBox::information(0,"port","data is not written",QMessageBox::Ok,0,0);
        inputLabel->setText("data is not written");
        keyState = 7;
        return;
    }
    while(!port->isReadable())
    {
                QApplication::processEvents();
                inputLabel->setText("waiting");
                //checkState();
    }
    QByteArray ba = port->readAll();
    inputLabel->setText(QString(ba));
    checkState();

        //QMessageBox::information(0,"port",QString("%1").arg(k,1,10,QLatin1Char( ' ' ))+" bytes are written",QMessageBox::Ok,0,0);

}

void serialWidget::readButtonClicked()
{
    if(port->isReadable())
    {
        QByteArray ba = port->readAll();
        inputLabel->setText(QString(ba));
        checkState();
    }
    else
    {
       //QMessageBox::information(0,"port",portName->currentText()+" is empty",QMessageBox::Ok,0,0);
       inputLabel->setText(portName->currentText()+" is is empty");
       keyState = 9;
   }

    //emit stateChanged();
}

void serialWidget::openPortButtonClicked()
{
    port->setPortName(portName->currentText());

    if(port->open(QIODevice::ReadWrite | QIODevice::Unbuffered))
    {
        port->setBaudRate(BAUD9600);
        port->setFlowControl(FLOW_OFF);
        port->setParity(PAR_EVEN);
        port->setDataBits(DATA_8);
        port->setStopBits(STOP_1);
        //set timeouts to 500 ms
        port->setTimeout(500);
        portName->setDisabled(true);
        openPortButton->setDisabled(true);
        closePortButton->setDisabled(false);
    }
    else
    {
        //QMessageBox::information(0,"port",portName->currentText()+" is not opened",QMessageBox::Ok,0,0);
        inputLabel->setText(portName->currentText()+" is not opened");
        keyState = 8;
    }

}

void serialWidget::closePortButtonClicked()
{
    if(port->isOpen())
    {
        port->close();
        portName->setDisabled(false);
        openPortButton->setDisabled(false);
        closePortButton->setDisabled(true);
    }
}

void serialWidget::writeProgButtonClicked()
{
    QStringList sl = outputLine->text().split('*');
    int k;
    QString str,incStr="";
    QByteArray ba;
    for(int i=0;i<sl.count();i++)
    {
        str = sl[i]+'*';
        k = port->write(str.toAscii(),str.length());
        if(k==-1)
        {
            //QMessageBox::information(0,"port","data is not written",QMessageBox::Ok,0,0);
            inputLabel->setText("data is not written");
            keyState = 7;
            break;
        }
        while(!port->isReadable())
        {
                    QApplication::processEvents();
                    inputLabel->setText("waiting");
                    checkState();
        }
        ba = port->readAll();
        incStr+=QString(ba);
    }
    inputLabel->setText(incStr);
    checkState();
}

serialWidget::~serialWidget()
{
    delete port;
    port = NULL;
}

void serialWidget::loadProgram(QString progStr)
{
    outputLine->setText(progStr);
    writeProgButtonClicked();
}

void serialWidget::startProgram()
{
    isProg = 1;
    keyState = -1;
    outputLine->setText("ST1*");
    //writeProgButtonClicked();
    writeButtonClicked();

    readStateTimer->start(1000);
    /*
    QTextCodec *codec1 = QTextCodec::codecForName("Windows-1251");
    if(port->isOpen())
    {
        outputLine->clear();
        outputLine->setText(progStr);
        int k = port->write(progStr.toAscii(),
                                 progStr.length());
        if(port->isReadable())
        {
            QByteArray ba = port->readAll();
            inputLabel->setText(codec1->fromUnicode(ba.data()));
        }
        if(k==-1) QMessageBox::information(0,"port","data is not written",QMessageBox::Ok,0,0);
        else
        {
            readStateTimer->start(1000);
        }
    }
    */
}

void serialWidget::stopProgram()
{
    QString progStr = "ST1*";
    readStateTimer->stop();
    if(port->isOpen())
    {
        int k = port->write(progStr.toAscii(),
                                 progStr.length());
        if(port->isReadable())
        {
            QByteArray ba = port->readAll();
            inputLabel->setText(QString(ba));
            checkState();
        }
        if(k==-1)
        {
            //QMessageBox::information(0,"port","data is not written",QMessageBox::Ok,0,0);
            inputLabel->setText("data is not written");
            keyState = 7;
        }

    }
}

void serialWidget::updateState()
{
    readButtonClicked();

    //emit stateChanged();

    if(keyState==0) readStateTimer->start(1000);
}

void serialWidget::checkState()
{
    QStringList sl = inputLabel->text().split('*');
    int kS0;
    for(int i=0; i<sl.count(); i++)
    {
        kS0 = keyState;
        if(sl.at(i).lastIndexOf("E10")==0)
        {
            keyState = 0;
        }
        else if(sl.at(i).lastIndexOf("E12")==0)
        {
            keyState = 1;
        }
        else if(sl.at(i).lastIndexOf("E13")==0)
        {
            keyState = 2;
        }
        else if(sl.at(i).lastIndexOf("E14")==0)
        {
            keyState = 3;
        }
        else if(sl.at(i).lastIndexOf("E15")==0)
        {
            keyState = 4;
        }
        else if(sl.at(i).lastIndexOf("E16")==0)
        {
            keyState = 5;
        }
        else if(sl.at(i).lastIndexOf("E19")==0)
        {
            keyState = 6;
        }
        //else keyState = -1;

        if(kS0!=keyState)
        {
            if(isProg&&keyState) isProg = 0;
            emit stateChanged();
        }
    }
}
