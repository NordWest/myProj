#include <QtCore>
#include <QtSql>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("ccdobs_nap");
    db.setUserName("fitsloader");
    db.setPassword("fitsloader");
    //db.setUserName("root");
    //db.setPassword("GToiBna72r");
    bool ok = db.open();

    qDebug() << QString("DB connect: %1").arg((int)ok);
    
    return 0;//a.exec();
}
