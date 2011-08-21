#ifndef HTTPHEADERREQUEST_H
#define HTTPHEADERREQUEST_H

#include <QtCore>
#include <QtNetwork>
#include <QObject>

class httpHeaderRequest : public QObject
{
    Q_OBJECT
    private:
        QCoreApplication *headfapp;
        QHttp *http;
        QSettings *settings;
        QString hostName;
        QString proxyName;
        int proxyPort;
        int useProxy;
        int eventMessages;
        int msgout;
        QString instr;
        int simplemode;
        QString replyDir;
        QString ntype;
        QString plateName;

private slots:
    void slotProcessingData(bool error);
        void slotStateChanged(int state);
        //void slotSslErrors(QList<QSslError> sslErr);
        void slotRequestFinished(int id, bool error);
public:
    httpHeaderRequest(QCoreApplication *app);
};

#endif // HTTPHEADERREQUEST_H
