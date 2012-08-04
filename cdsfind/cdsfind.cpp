#include "cdsfind.h"

cdsfind::cdsfind(QCoreApplication *app)
{	
    cdsfapp = app;
    QTextStream stream(stdout);
    //begin load settings
    QString requestStr;
    QSettings *netSett;
    settings = new QSettings(cdsfapp->applicationDirPath()+"/cdsfind.ini",QSettings::IniFormat);
    hostName = settings->value("general/hostName").toString();
    QString hostSuff = settings->value("general/hostSuff", "/cgi-bin/asu-tsv?").toString();
    int useExtNetworkIni = settings->value("general/useExtNetworkIni").toInt();
    if(useExtNetworkIni)
    {
        netSett = new QSettings("network.ini", QSettings::IniFormat);
        proxyName = netSett->value("general/proxyName").toString();
        proxyPort = netSett->value("general/proxyPort").toInt();
        useProxy = netSett->value("general/useProxy").toInt();
    }
    else
    {
        proxyName = settings->value("general/proxyName").toString();
        proxyPort = settings->value("general/proxyPort").toInt();
        useProxy = settings->value("general/useProxy").toInt();
    }
    eventMessages = settings->value("general/eventMessages").toInt();

    //catParam
    eq = settings->value("catParam/eq").toString();
    QString outLayout = settings->value("catParam/outLayout").toString();
    QString vUnit = settings->value("catParam/vUnit").toString();


    //end load settings
    if (eventMessages) stream  << "\nhostName: " << hostName<<'\n';
    //begin setup of http connection
    http = new QHttp(this);

    if (useProxy==1)
    {
        if (eventMessages) stream  << "\nuseProxy 1\n";
        http->setProxy(proxyName, proxyPort);
    }
    http->setHost(hostName);
    //end setup of http connection
    //begin signal-slot
    connect(http, SIGNAL(done(bool)), this, SLOT(slotProcessingData(bool)));
    connect(http, SIGNAL(stateChanged(int)), this, SLOT(slotStateChanged(int)));
    connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(slotRequestFinished(int, bool)));
    //end signal-slot
    //begin forming request
    QString vGeom = app->arguments()[4].section("=",0,0);
    QString vSize=app->arguments()[4].section("=",1,1);
    if (eventMessages) stream << QString("vGeom= %1\tvSize= %2\n").arg(vGeom).arg(vSize);

    //requestStr = "/cgi-bin/asu-tsv?";
    //requestStr = "/viz-bin/asu-tsv?";
    requestStr = hostSuff;
    //QString requestStr = hostName+"/viz-bin/VizieR?";
    //requestStr = QString("http://vizier.u-strasbg.fr/cgi-bin/asu-tsv?-to=4&-from=-2&-this=-2&-out.max=unlimited&-out.form=%7C+-Separated-Values&-order=I&-c=20+54+05.689+%2B37+01+17.38&-c.eq=J2000&-oc.form=dec&-c.r=+10&-c.u=arcmin&-c.geom=r&-source=I%2F304%2Fout&-out=CMC14&CMC14=&-out=f_CMC14&f_CMC14=&-out=RAJ2000&-sort=RAJ2000&RAJ2000=&-out=DEJ2000&DEJ2000=&-out=MJD-51263&MJD-51263=&-out=r%27mag&r%27mag=&-out=u_r%27mag&u_r%27mag=&-out=Nt&Nt=&-out=Na&Na=&-out=Np&Np=&-out=e_RAdeg&e_RAdeg=&-out=e_DEdeg&e_DEdeg=&-out=e_r%27mag&e_r%27mag=&-out=Jmag&Jmag=&-out=Hmag&Hmag=&-out=Ksmag&Ksmag=&-out.all=1&-file=.&-meta=2&-meta.foot=1");
    requestStr+="-source="+app->arguments()[1]+"&";
    requestStr+="-c="+app->arguments()[2];
    requestStr+=" "+app->arguments()[3];
    requestStr+="&-c.eq="+eq;
    requestStr+="&-out.all&-out.max=unlimited&-out.form="+outLayout;
    requestStr+="&-c.geom="+vGeom;
    requestStr+="&-c.r="+vSize;
    requestStr+="&-c.u="+vUnit;
    requestStr = requestStr.toAscii().toPercentEncoding("-/._~=?&:");
    requestStr = hostName + requestStr;
    if (eventMessages) stream << requestStr <<'\n';
    //end forming request
    http->get(requestStr);
    //stream <<hostName<<'\n';
    //http->get("/out/pul31252016085.htm");
}


//?-to=4&-from=-2&-this=-2&-out.max=unlimited&-out.form=%7C+-Separated-Values&-order=I&-c=20+54+05.689+%2B37+01+17.38&-c.eq=J2000&-oc.form=dec&-c.r=+10&-c.u=arcmin&-c.geom=r&-source=I%2F304%2Fout&-out=CMC14&CMC14=&-out=f_CMC14&f_CMC14=&-out=RAJ2000&-sort=RAJ2000&RAJ2000=&-out=DEJ2000&DEJ2000=&-out=MJD-51263&MJD-51263=&-out=r%27mag&r%27mag=&-out=u_r%27mag&u_r%27mag=&-out=Nt&Nt=&-out=Na&Na=&-out=Np&Np=&-out=e_RAdeg&e_RAdeg=&-out=e_DEdeg&e_DEdeg=&-out=e_r%27mag&e_r%27mag=&-out=Jmag&Jmag=&-out=Hmag&Hmag=&-out=Ksmag&Ksmag=&-out.all=1&-file=.&-meta=2&-meta.foot=1
void cdsfind::slotProcessingData(bool error)
{
    QTextStream stream(stdout);
    if (error)
    {
        stream << "error has occured.\n";
        cdsfapp->quit();
    }
    else
    {
        //stream << "data is being loaded...\n";
        QByteArray httpData = http->readAll();
        QString str(httpData);
        stream << str;
        cdsfapp->quit();
    }
}

void cdsfind::slotStateChanged(int state)
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
        stream << str_state << "\n";
    }
}



void cdsfind::slotRequestFinished(int id, bool error)
{
    if(error)
    {
        QTextStream stream(stdout);
        stream << http->errorString() << "\n";
    }
}
