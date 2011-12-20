//это заголовочный файл класса cdsfind. Пусть название класса не смущает.
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
        int useNetworkCfg;
	int eventMessages;
	int msgout;
	QString obscode;
	int simplemode;
	QString replyDir;
	QString ntype;
	QString astNumber;
	QString astDes;
	QString obsCode;
        QString centre;
        QString keph;
	int timescale;
	
private slots:// самый минимум слотов. нам ведь нужно чтобы прога могла "узнать":  
    void slotProcessingData(bool error);//получить и обработать данные;
	void slotStateChanged(int state);//состояние запроса;
	//void slotSslErrors(QList<QSslError> sslErr);
	void slotRequestFinished(int id, bool error);//узнать, что выполнение запроса завершено;
 public:
	cdsfind(QCoreApplication *app);
};
