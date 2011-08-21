#include <QtCore>
#include <QtNetwork>

class sateph: public QObject
{
Q_OBJECT


 private:
	QCoreApplication *sateph_app;
	QHttp *http;
	QSettings *settings;
	QString hostName;
	QString proxyName;
	int proxyPort;
	int useProxy;
	QString theory;
	QString sattheory;
	QString planet;
	int noutput;
	QString obsCode;
	int timescale;
	int timemode;
	int busy;
	QStringList outputList;
	QString outStr;
	
private slots:  
    void slotProcessingData(bool error);
 public:
	sateph(QCoreApplication *app);
};
