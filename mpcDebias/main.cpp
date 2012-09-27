#include <QtCore/QCoreApplication>
#include <QtSql>

#include "./../libs/mpcfile.h"
#include "./../libs/comfunc.h"
#include "./../libs/ringpix.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    setlocale(LC_NUMERIC, "C");

    int i, sz;
    QTextStream out_stream;

    mpcRec mpR;
    QString catName, catPref, tstr;

    QFile resFile;
    QTextStream resStm;

    QFile fout("./fout.dat");
    fout.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);

    out_stream.setDevice(&fout);

    QSettings *settings = new QSettings("./mpcDebias.ini",QSettings::IniFormat);

    //objSphere
     long nsMax = settings->value("general/nsMax", 64).toLongLong();
    catPref = settings->value("general/catPref", "t2").toString();


     //QSettings sett("./ccdobsDB.ini", QSettings::IniFormat);

     QString db_server = settings->value("db/server", "localhost").toString();
     QString db_name = settings->value("db/name", "ccdobs_nap").toString();
     QString db_user = settings->value("db/user", "fitsloader").toString();
     QString db_pass = settings->value("db/pass", "fitsloader").toString();

     QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
     db.setHostName(db_server);
     db.setDatabaseName(db_name);
     db.setUserName(db_user);
     db.setPassword(db_pass);

     bool ok = db.open();

     qDebug() << QString("DB connect: %1").arg((int)ok);

     if(!ok) exit(1);


     QSqlQuery query(db);
    QSqlError lastErr;
     QString queryStr;




        QString mpcFile(argv[1]);
        QString fileNameRes = QString(argv[2]);



        QFile inFile(mpcFile);
        if(!inFile.open(QIODevice::ReadOnly))
        {
            qDebug() << QString("File %1 not open. stop\n").arg(mpcFile);
            return 1;
        }
        QTextStream inStm(&inFile);
/*
        QFile resFile(fileNameRes);
        if(!resFile.open(QIODevice::WriteOnly | QIODevice::Append))
        {
            qDebug() << QString("File %1 not open. stop\n").arg(fileNameRes);
            return 1;
        }
        QTextStream resStm(&resFile);
*/
        QFileInfo mpcI(mpcFile);
        QString wDirName = QString(mpcI.absolutePath());

        double dect, rat, lam, beta;
        long ipix, ipixMax;

        //nsMax = 32;//8192;
        //QVector <int> iNum;
        //QVector <> deRA;


        ipixMax = nsMax*nsMax*12;
        //iNum.fill(0, ipixMax);

        //int *iNum = new int[ipixMax];
        //for(i=0; i<ipixMax; i++) iNum[i] = 0;
        int oNum=0;
        double dx, dy;

        int uc;

        resFile.setFileName(QString("%1/objSphere.txt").arg(wDirName).arg(fileNameRes));

        if(resFile.open(QFile::WriteOnly | QFile::Truncate))
        {
            resStm.setDevice(&resFile);

            for(i=0; i<ipixMax; i++)
            {
                pix2ang_ring( nsMax, i, &dect, &rat);
                dect = dect-M_PI/2.0;
                /*if(isZonal)
                {
                    dect = asin(0.5*sin(dect)*(s2-s1) + 0.5*(s2+s1));
                    //rat = asin(0.5*sin(rat)*(rs2-rs1) + 0.5*(rs2+rs1));
                }*/

                queryStr = QString("SELECT healStore, r0, d0, %1tmN, %1tmMiss, %1tmXbar, %1tmXsig, %1tmYbar, %1tmYsig, %1tmFract, %1tmRadius FROM xcat  WHERE healStore=%2").arg(catPref).arg(ipix);

                query.first();
                rat = query.value(1).toDouble();
                dect = query.value(2).toDouble();
                dx = query.value(5).toDouble()/206265.0;
                dy = query.value(7).toDouble()/206265.0;

                resStm << QString("%1|%2|%3|%4|%5|%6\n").arg(rat, 15, 'e', 10).arg(dect, 15, 'e', 10).arg(rat-dx, 15, 'e', 10).arg(dect-dy, 15, 'e', 10).arg(dx, 15, 'e', 10).arg(dy, 15, 'e', 10);
            }

            resFile.close();

        }

/*
        while(!inStm.atEnd())
        {
            mpR.fromStr(inStm.readLine());

            dect = grad2rad(mpR.dec());
            rat = grad2rad(mpR.ra());

            oNum++;

            //mpR.getMpNumber(mpNum);
            //mpR.getObsCode(obsCode);
            //mjd = mpR.mjd();
            //getDATEOBSfromMJD(&date_obs, mjd);
            mpR.getCatName(catName);

            ang2pix_ring(nsMax, dect+M_PI/2.0, rat, &ipix);

            queryStr.clear();

            catPref = "t2";
            uc = 0;

            if(QString().compare(catName, "Tycho-2", Qt::CaseInsensitive)==0) catPref = "t2";
            else if(QString().compare(catName, "UCAC2", Qt::CaseInsensitive)==0) catPref = "uc";
            else if(QString().compare(catName, "USNO-A1.0", Qt::CaseInsensitive)==0) catPref = "a1";
            else if(QString().compare(catName, "USNO-A2.0", Qt::CaseInsensitive)==0) catPref = "a2";
            else if(QString().compare(catName, "USNO-B1.0", Qt::CaseInsensitive)==0) catPref = "ub";
            else if(QString().compare(catName, "GSC-2.2", Qt::CaseInsensitive)==0) catPref = "g2";
            else uc = 1;

            if(!uc)
            {

                queryStr = QString("SELECT healStore, r0, d0, %1tmN, %1tmMiss, %1tmXbar, %1tmXsig, %1tmYbar, %1tmYsig, %1tmFract, %1tmRadius FROM xcat  WHERE healStore=%2").arg(catPref).arg(ipix);
                //queryStr = QString("SELECT healStore r0 d0 FROM xcat WHERE healStore=%1").arg(ipix);

                qDebug() << QString("queryStr: %1").arg(queryStr);

                if(!query.exec(queryStr))
                {
                    lastErr = query.lastError();
                    qDebug() << QString("exec error:%1\n").arg(lastErr.databaseText());
                }

                /*qDebug() << QString("query size: %1\n").arg(query.size());
                while (query.next()) {
                         tstr = query.value(0).toString();
                         qDebug() << QString("query: %1\n").arg(tstr);
                     }/

                query.first();
                dx = query.value(5).toDouble();
                dy = query.value(7).toDouble();

            }

        }

*/


    
    return 0;//a.exec();
}
