#include "skybotclient.h"

skybotclient::skybotclient(QCoreApplication *app)
{
	
	skybotclient_app = app;
	//BEGIN read settings
	settings = new QSettings("skybotclient.ini",QSettings::IniFormat);
	hostName = settings->value("general/hostName").toString();
	proxyName = settings->value("general/proxyName").toString();
	proxyPort = settings->value("general/proxyPort").toInt();
	useProxy = settings->value("general/useProxy").toInt();
        int useNetworkIni = settings->value("general/useNetworkIni", 0).toInt();
        if(useNetworkIni)
        {
            QSettings *netSett = new QSettings("network.ini",QSettings::IniFormat);
            proxyName = netSett->value("general/proxyName").toString();
            proxyPort = netSett->value("general/proxyPort").toInt();
            useProxy = netSett->value("general/useProxy").toInt();
        }
	//END read settings
	//BEGIN set host
	http = new QHttp(this);
	connect(http, SIGNAL(done(bool)), this, SLOT(slotProcessingData(bool)));
	http->setHost(hostName);
	if (useProxy==1) http->setProxy(proxyName, proxyPort);
	//END set host
	//BEGIN get
        QString getrequest = "/webservices/skybot/skybotconesearch_query.php?-ep="+
		skybotclient_app->arguments().at(1)+
		"&-ra="+skybotclient_app->arguments().at(2)+
		"&-dec="+skybotclient_app->arguments().at(3)+
		"&-rm="+skybotclient_app->arguments().at(4)+
		"&-mime=text&-out=all&-loc="+skybotclient_app->arguments().at(5)+
                "&-filter=0";
        http->get(getrequest);
	//END get
}

void skybotclient::slotProcessingData(bool error)
{
	QTextStream stream(stdout);
	if (error)
	{
		stream << "error has occured.\n";
		skybotclient_app->quit();
	}
	else
		{
			QTextStream responseStream(http->readAll());
			while (!responseStream.atEnd()) stream << responseStream.readLine() <<"\n";
			skybotclient_app->quit();
		}		
}
