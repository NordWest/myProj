#include <QtCore>
#include <QtNetwork>

class cdsfind: public QObject
{
Q_OBJECT

 private:
	QCoreApplication *cdsfapp;
	QHttp *http;
	QSettings *settings;
	QString hostName;
	QString proxyName;
	int proxyPort;
	int useProxy;
	int eventMessages;
        QString eq;
	
private slots:
    void slotProcessingData(bool error);
    void slotStateChanged(int state);
    void slotRequestFinished(int id, bool error);
 public:
	cdsfind(QCoreApplication *app);
};
