#include "httpheader.h"

httpHeaderRequest::httpHeaderRequest(QCoreApplication *app)
{
    headfapp = app;
    QTextStream stream(stdout);

    settings = new QSettings("getHttpHeader.ini",QSettings::IniFormat);
    hostName = settings->value("general/hostName", "www.puldb.ru").toString();
    instr =  settings->value("general/instrName", "na").toString();

    proxyName = settings->value("general/proxyName", "").toString();
    replyDir = settings->value("general/replyDir", "./").toString();
    proxyPort = settings->value("general/proxyPort").toInt();
    useProxy = settings->value("general/useProxy").toInt();
    eventMessages = settings->value("general/eventMessages").toInt();
    msgout = settings->value("general/msgout", 0).toInt();//std out
    int useNetworkIni = settings->value("general/useNetworkIni", 0).toInt();
    if(useNetworkIni)
    {
        QSettings netSett("network.ini",QSettings::IniFormat);
        proxyName = netSett.value("general/proxyName").toString();
        proxyPort = netSett.value("general/proxyPort").toInt();
        useProxy = netSett.value("general/useProxy").toInt();
    }

    if (msgout==1) stream << "mpeph has started...\n";
    ////////////////////////////////////////////////////
    http = new QHttp(this);
    if (msgout==1) stream << "useProxy...\n";
    if (useProxy==1)
    {
            http->setProxy(proxyName, proxyPort);
    }
    if (msgout==1) stream << "setHost...\n";
    http->setHost(hostName);
    connect(http, SIGNAL(done(bool)), this, SLOT(slotProcessingData(bool)));
    connect(http, SIGNAL(stateChanged(int)), this, SLOT(slotStateChanged(int)));
    connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(slotRequestFinished(int, bool)));
    //
    if (msgout==1) stream << "arguments...\n";
    //instr = headfapp->arguments().at(1);
    //QString plateName;
    QTextCodec *codec1 = QTextCodec::codecForName("Windows-1251");
    Q_ASSERT( codec1 );
    //qDebug() << "\nargv[2]: " << codec1->toUnicode(headfapp->arguments().at(2).toAscii().data()) << "\n";
    //char **argv;
    //argv = headfapp->argv();
    //qDebug() << "argv: " << argv[2] << "\n";
    QByteArray bpn = codec1->fromUnicode(headfapp->arguments().at(1));
    //QByteArray bpn = headfapp->arguments().at(2).toAscii().data();
    //qDebug() << QString("bpn: %1\n").arg(bpn.constData());
    plateName = bpn.toPercentEncoding("-/._~=?&");
    //plateName = headfapp->arguments().at(2);//.toPercentEncoding("-/._~=?&");
    //plateName = headfapp->arguments().at(2);
    //plateName = codec1->toUnicode(headfapp->arguments().at(2).toLocal8Bit());
    //plateName = codec1->toUnicode(argv[2]);
    //qDebug() << QString("pnStr: %1\n").arg(plateName);

   // plateName = headfapp->arguments().at(2);

    QString requestStr = "/db/plates/fits.php?instr="+instr+"&num="+plateName;
    if (msgout==1) stream << "requestStr= " << requestStr << "\n";
    http->get(requestStr);
}

void httpHeaderRequest::slotProcessingData(bool error)
{
QTextStream stream(stdout);

    if (error)
        {
                if (msgout==1) stream << "error has occured.\n";
        }
        else
        {


            if (msgout==1) stream << "data is being loaded...\n";
            QByteArray httpData = http->readAll();

            //QTextCodec *codec1 = QTextCodec::codecForName("windows-1251");
            //QTextCodec *codec2 = QTextCodec::codecForName("IBM 866");

    QString codecName;

    codecName = "CP1251";

    QTextCodec *codec1 = QTextCodec::codecForName(codecName.toAscii().constData());
    Q_ASSERT( codec1 );

            //QString str = codec1->toUnicode(httpData);
            QString str = codec1->toUnicode(httpData.data());
            QStringList strL;

            //QStringList strL = str.split("\n");
            //int sizeS = strL.size();
            int p0 = str.lastIndexOf("<pre>")+7;
            int p1 = str.lastIndexOf("</pre>");
            //int sz = str.size();

            str = str.mid(p0, p1-p0);
            strL = str.split("\n", QString::SkipEmptyParts);

            stream << strL.join("\n") << "\n";
            //stream << httpData;

            headfapp->quit();
        }
        //headfapp->quit();
}

void httpHeaderRequest::slotStateChanged(int state)
{
        if (eventMessages)
        {
                QTextStream stream(stdout);
                QString str_state;
                switch(state)
                {
                        case QHttp::Unconnected:
                          str_state = "There is no connection to the host.";
                          break;
                        case QHttp::HostLookup:
                          str_state = "A host name lookup is in progress.";
                          break;
                        case QHttp::Connecting:
                          str_state = "An attempt to connect to the host is in progress.";
                          break;
                        case QHttp::Sending:
                          str_state = "The client is sending its request to the server.";
                          break;
                        case QHttp::Reading:
                          str_state = "The client's request has been sent and the client is reading the server's response.";
                          break;
                        case QHttp::Connected:
                          str_state = "The connection to the host is open, but the client is neither sending a request, nor waiting for a response.";
                          break;
                        case QHttp::Closing:
                          str_state = "The connection is closing down, but is not yet closed.";
                          break;
                }
                if (msgout==1) stream << str_state << "\n";
        }
}

void httpHeaderRequest::slotRequestFinished(int id, bool error)
{
        if(error)
        {
            QTextStream stream(stdout);
                if (msgout==1) stream << http->errorString() << "\n";
        }
        //headfapp->quit();
}
