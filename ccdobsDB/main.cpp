#include <QtCore>
#include <QtSql>

#include "./../libs/fitsdata.h"
#include <astro.h>
#include <mb.h>

struct ccdRecord
{
    QString dateObsCode;
    QDateTime dtObs;
    double mjdEpoch;
    double ra, de;
    double expTime;
    QString target;
    QString season;
    QString relFileName;
    QString observer;
    QDate obsDate;
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    setlocale(LC_NUMERIC, "C");

    //if(argc<2) return 1;
    QString tstr;

    QStringList dirList;
    QStringList dataFiles, filters, wfList;
    QString fileName, tFile;
    QDir tDir, sDir;


    int year, mth, i, szi, j, szj;
    double day;

    ccdRecord ccd_rec;
    fitsdata fdata;

    QSettings sett("./ccdobsDB.ini", QSettings::IniFormat);

    QString seasonName = sett.value("general/seasonName", "2012y").toString();

    QString db_server = sett.value("db/server", "localhost").toString();
    QString db_name = sett.value("db/name", "ccdobs_nap").toString();
    QString db_user = sett.value("db/user", "fitsloader").toString();
    QString db_pass = sett.value("db/pass", "fitsloader").toString();

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(db_server);
    db.setDatabaseName(db_name);
    db.setUserName(db_user);
    db.setPassword(db_pass);

    bool ok = db.open();

    qDebug() << QString("DB connect: %1").arg((int)ok);

    if(!ok) exit(1);

    QSqlQuery query(db);
    QString queryStr;

    queryStr.clear();
    queryStr = QString("SELECT localPath FROM seasons WHERE name='%1'").arg(seasonName);
    qDebug() << QString("queryStr: %1\n").arg(queryStr);

    if(!query.exec(queryStr)) exit(1);

    query.first();
    QString locPath = query.value(0).toString();

    qDebug() << QString("Season Path: %1\n").arg(locPath);

    sDir.setPath(locPath);

    filters << "*.fit";

    QDirIterator it(locPath, QDir::Dirs|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        tFile = it.next();
        dirList << tFile;
    }

    szi = dirList.size();
    for(i=0; i<szi; i++)
    {
        if(dirList.at(i).indexOf("dark")!=-1||dirList.at(i).indexOf("flat")!=-1||dirList.at(i).indexOf("draft")!=-1) continue;
        qDebug() << dirList.at(i) << "\n\n";
        tDir.setPath(dirList.at(i));
        dataFiles = tDir.entryList(filters, QDir::Files, QDir::Name);// | QDir::Reversed);
        qDebug() << QString("filesNum: %1\n\n").arg(dataFiles.size());
        szj = dataFiles.size();
        wfList.clear();

        for(j=0; j<szj; j++)
        {
            if(dataFiles.at(j).indexOf("focus")==0)
            {
                qDebug() << QString("SKIP: %1\n").arg(dataFiles.at(j));
                continue;
            }
            fileName = QString("%1/%2").arg(dirList.at(i)).arg(dataFiles.at(j));


            if(fdata.openFile(fileName)) continue;

            ccd_rec.mjdEpoch = fdata.MJD;
            mjdDateCode(&ccd_rec.dateObsCode, ccd_rec.mjdEpoch);
            tstr = ccd_rec.dateObsCode;
            tstr.chop(1);
            qDebug() << QString("tstr: %1\n").arg(tstr);
            ccd_rec.dtObs = QDateTime().fromString(tstr, "yyyyMMddhhmmss");
            ccd_rec.ra = fdata.WCSdata[2];
            ccd_rec.de = fdata.WCSdata[3];
            ccd_rec.expTime = fdata.exptime;
            fdata.headList.getKeyName("TARGET", &tstr);
            ccd_rec.target = tstr.section("\'", 1, 1);
            ccd_rec.season = seasonName;
            ccd_rec.relFileName ="/"+sDir.relativeFilePath( QFileInfo(QString(fileName)).absoluteFilePath());
            fdata.headList.getKeyName("OBSERVER", &tstr);
            ccd_rec.observer = tstr.section("\'", 1, 1);
            int jdn = dat2JDN(ccd_rec.dtObs.date().year(), ccd_rec.dtObs.date().month(), ccd_rec.dtObs.date().day()+ccd_rec.dtObs.time().hour()/24.0-0.5);
            dat2YMD(jdn, &year, &mth, &day);
            ccd_rec.obsDate.setYMD(year, mth, (int) day);


            //qDebug() << QString("obscode: %1\nDATETIMEOBS: %2\nmjdEpoch: %3\nra: %4\nde: %5\nTarget: %6\nPath: %7\ndate: %8\ndate: %9\n").arg(ccd_rec.dateObsCode).arg(ccd_rec.dtObs.toString("yyyy-MM-dd hh:mm:ss")).arg(ccd_rec.mjdEpoch).arg(ccd_rec.ra).arg(ccd_rec.de).arg(ccd_rec.target).arg(ccd_rec.originName).arg(ccd_rec.obsDate.toString("yyyy-MM-dd")).arg(QString("%1 %2 %3").arg(year).arg(mth).arg(day));




            queryStr.clear();
            queryStr = QString("INSERT INTO fitsheader (obscode, DATETIMEOBS, mjdEpoch, ra, de, Target, season, relFileName, observer, exptime, obsDate) VALUE (\'%1\', \'%2\', \'%3\', \'%4\', \'%5\', \'%6\', \'%7\', \'%8\', \'%9\', \'%10\', \'%11\')").arg(ccd_rec.dateObsCode).arg(ccd_rec.dtObs.toString("yyyy-MM-dd hh:mm:ss")).arg(ccd_rec.mjdEpoch, 15, 'f', 7, QLatin1Char('0')).arg(ccd_rec.ra, 13, 'f', 8, QLatin1Char('0')).arg(ccd_rec.de, 13, 'f', 8, QLatin1Char('0')).arg(ccd_rec.target).arg(ccd_rec.season).arg(ccd_rec.relFileName).arg(ccd_rec.observer).arg(ccd_rec.expTime).arg(ccd_rec.obsDate.toString("yyyy-MM-dd"));
            qDebug() << QString("queryStr: %1").arg(queryStr);

            query.exec(queryStr);

            /*
            query.exec("SELECT * FROM 'fitsheader'");
        */


            while (query.next()) {
                     tstr = query.value(0).toString();
                     qDebug() << QString("query: %1\n").arg(tstr);
                 }
        }
    }



    db.close();
    
    return 0;//a.exec();
}
