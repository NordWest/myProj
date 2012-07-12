#include <QtCore>
#include <QtSql>

#include "./../libs/fitsdata.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if(argc<2) return;

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("ccdobs_nap");
    db.setUserName("fitsloader");
    db.setPassword("fitsloader");
    //db.setUserName("root");
    //db.setPassword("GToiBna72r");
    bool ok = db.open();

    qDebug() << QString("DB connect: %1").arg((int)ok);

    QSettings sett("./ccdobsDB.ini", QSettings::IniFormat);

    QString workDir = sett.value("general/workDir", "./").toString();

    fitsdata fdata;

    fdata.openFile(QString(argv[1]));
    
    return 0;//a.exec();
}
