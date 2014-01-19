//это заголовочный файл класса miriadeEph. Пусть название класса не смущает.
#include <QtCore>
#include <QtNetwork>
#include <astro.h>

class miriadeEph: public QObject
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
        int useNetworkIni;
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
        //QTextStream stream;
	
private slots:// самый минимум слотов. нам ведь нужно чтобы прога могла "узнать":  
    void slotProcessingData(bool error);//получить и обработать данные;
	void slotStateChanged(int state);//состояние запроса;
	//void slotSslErrors(QList<QSslError> sslErr);
	void slotRequestFinished(int id, bool error);//узнать, что выполнение запроса завершено;
 public:
        miriadeEph(QCoreApplication *app);
};
