#include <QtCore>
#include <QtNetwork>

class skybotclient: public QObject
{
Q_OBJECT


 private:
	QCoreApplication *skybotclient_app;
	QHttp *http;
	QSettings *settings;
	QString hostName;
	QString proxyName;
	int proxyPort;
	int useProxy;
        //int useNetworkCfg;
        QByteArray resStr;

	//QString outStr;
	
private slots:  
    void slotProcessingData(bool error);


 public:
	skybotclient(QCoreApplication *app);
};
