#include <QtCore>
//#include "./../libs/measurescanproc.h"
//#include "./mesthread.h"
#include "./../libs/fitsdata.h"

/* Программа для последовательного и многопоточного измерения отсканированных пластинок с помощью measure*/

void prepareDirs(QStringList *dirs)
{
    int i, j, sz;
    QString tStr;

    for(i=0; i<dirs->size(); i++)
    {
        tStr = dirs->at(i);
        sz = dirs->size();
        for(j=sz-1; j>i; j--)
        {
            if(tStr==dirs->at(j)) dirs->removeAt(j);
        }

    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    setlocale(LC_NUMERIC, "C");
    QString codecName;

    #if defined(Q_OS_LINUX)
        codecName = "UTF-8";
    #elif defined(Q_OS_WIN)
        codecName = "CP1251";
    #endif

    QTextCodec *codec1 = QTextCodec::codecForName(codecName.toAscii().constData());
    Q_ASSERT( codec1 );
    QTextCodec::setCodecForCStrings(codec1);

    QSettings *sett = new QSettings("./conf/damPack.ini", QSettings::IniFormat);

    int sourceType = sett->value("general/sourceType", 0).toInt();  //0 - workDir; 1 - workList
    QString workDir = sett->value("general/workDir", "./").toString();//QDir::currentPath();
    //QString resDir = sett->value("general/resDir", "./").toString();//QDir::currentPath();
    QString workList = sett->value("general/workList", "./").toString();//QDir::currentPath();
    QString resDir = sett->value("general/resDir", "./").toString();//QDir::currentPath();
    QString scansoftDir = sett->value("general/scansoftDir", "c:/scansoft").toString();
    //int procNum = sett->value("general/procNum", "1").toInt();
    QString get_http_prog = sett->value("general/get_http_prog", "getHttpHeader.exe").toString();
    QString get_http_prog_folder = sett->value("general/get_http_prog_folder", "c:/scansoft").toString();
    int getHttpWait = sett->value("general/getHttpWait", 100000).toInt();
    //QString instrName = sett->value("general/instrName", "na").toString();

    QString measure_prog = sett->value("general/measure_prog", "measure").toString();
    QString measure_prog_folder = sett->value("general/measure_prog_folder", "c:/scansoft").toString();
    int mWaitTime = sett->value("general/measureWait", "-1").toInt();

    int i;
    QStringList dirList;
    int dirNum = dirList.size();
    int isLor, isTif, expNum;
    int isN, isE, isW, isS;
    QString pnStr, curDir, eNumStr, tDir, tFile, diskLet, tStr;
    QStringList tPath, pnList;
    QTextStream catStream;
    QString dirsStr;

    qDebug() << QString("sourceType: %1").arg(sourceType);
    qDebug() << QString("workDir: %1").arg(workDir);
    qDebug() << QString("workList: %1").arg(workList);
    qDebug() << QString("scansoftDir: %1").arg(scansoftDir);

    QProcess httpProcess;
    QProcess mesProcess;
    QProcess sysProcess;
    //QString dir_prog = QString("listDir.bat");
    QStringList outerArguments;
    outerArguments.clear();
    int resHttp, resMes;
/*
    outerProcess.setWorkingDirectory(workDir);
    outerProcess.setProcessChannelMode(QProcess::MergedChannels);
    outerProcess.setReadChannel(QProcess::StandardOutput);
    outerArguments << "";
    qDebug() << dir_prog << outerArguments.join(" ");
    outerProcess.start(dir_prog, outerArguments);

    outerProcess.waitForFinished(-1);
    catStream(outerProcess.readAllStandardOutput());
    dirsStr = catStream.readAll();
*/
    QStringList fList;
    switch(sourceType)
    {
    case 0:

        {

            QDir wDir(workDir);
            QStringList fFilter;
            fFilter << "*.fit";
            wDir.setNameFilters(fFilter);


            fList = wDir.entryList();
        }
        break;

    case 1:
        {
            QFile wListFile(workList);
            if(!wListFile.open(QIODevice::ReadOnly)) return 1;
            QTextStream wStr(&wListFile);

            while(!wStr.atEnd())
            {
                tStr = wStr.readLine();
                if(tStr.at(0)=='#') continue;
                qDebug() << tStr << "\n";
                fList << tStr.section(" ", 1, 1, QString::SectionSkipEmpty);
                pnList << tStr.section(" ", 0, 0, QString::SectionSkipEmpty);
            }
        }
        break;
    }

    qDebug() << "fList:\n" << fList.join("\n");


    //dirList = dirsStr.split("\n");


    fitsdata *ftemp = new fitsdata;
    QFile toFile;

    QStringList resDirs;
    QList <int> resRes;


    QString pnTempStr;

    int fNum = fList.size();

    QFileInfo fi;



    for(i=0; i<fNum; i++)
    {

        /*tPath = curDir.split("\:");
        diskLet = tPath.at(0);
        qDebug() << QString("disk: %1\n").arg(diskLet);
        curDir = tPath.at(1);
        tPath = curDir.split("\\");
        curDir = tPath.join("/");
        qDebug() << tPath.join("/");
        curDir = QString(tPath.join("/"));*/
       //Debug() << QString("curDir: %1\n").arg(curDir);
/*
        QDir dir(curDir);
        QStringList filters;
        filters << filter;//"*_report.txt";
        dir.setNameFilters(filters);
        QStringList outflist = dir.entryList();

        qDebug() << outflist.join("|") << "\n";
*/

        switch(sourceType)
        {
        case 0:

            {


               pnStr = fList.at(i).section(".", 0, 0);
            }
            break;
        case 1:
            {
                pnStr = pnList.at(i);
            }
        }

        //pnStr = curDir.section("/", -1, -1);
        qDebug() << QString("pnStr: %1\n").arg(pnStr);
        if(pnStr.size()<5) continue;

        httpProcess.setWorkingDirectory(get_http_prog_folder);
        outerArguments.clear();
        outerArguments << pnStr;
        qDebug() << get_http_prog << outerArguments.join(" ");
        httpProcess.start(get_http_prog, outerArguments);

        resHttp = httpProcess.waitForFinished(getHttpWait);
        if(!resHttp)
        {
            httpProcess.close();
            continue;
        }


        //catStream.flush();
        QTextStream httpStream(httpProcess.readAllStandardOutput());

        if(ftemp->readHeader(httpStream.readAll()))
        {
            qDebug() << "header not found\n";
            continue;
        }
        ftemp->headList.getKeyName("PLATE-N", &pnTempStr);
        //qDebug() << QString("|%1|%2|\n").arg(QString(codec1->fromUnicode(pnTempStr.simplified()))).arg(pnStr);
        /*if(QString(codec1->fromUnicode(pnTempStr.simplified())).indexOf(pnStr)==-1)
        {
            qDebug() << "no such plate name\n";
            continue;
        }*/

        qDebug() << QString("|%1|%2|\n").arg(pnTempStr.simplified()).arg(pnStr);
        if(pnTempStr.simplified().indexOf(pnStr)==-1)
        {
            qDebug() << "no such plate name\n";
            continue;
        }

        ftemp->headList.getKeyName("N-EXP", &eNumStr);
        expNum = eNumStr.toInt();

        tFile = fList.at(i);
        fi.setFile(tFile);
        if(sourceType==1) workDir = fi.absolutePath();
        mesProcess.setWorkingDirectory(workDir);
        outerArguments.clear();
        outerArguments << fi.fileName() << QString("%1").arg(expNum);
        //if(sourceType==1)
        outerArguments << pnStr << QDir(resDir).absolutePath();


        qDebug() << measure_prog << outerArguments.join(" ");

        /*isLor = QFile::exists(QString("%1/lorentz.log").arg(curDir));
        if(isLor)
        {
            qDebug() << QString("zanyato: %1\n").arg(curDir);
            continue;
        }*/

        mesProcess.start(measure_prog, outerArguments);

        resMes = mesProcess.waitForFinished(mWaitTime);

        if(!resMes)
        {
            mesProcess.close();
            //continue;
        }



        resRes << resMes;
        resDirs << curDir;
    }


    int rsize = resDirs.size();
    qDebug() << "\nres\n";
    for(i=0; i<rsize; i++)
    {
        qDebug() << QString("%1: %2").arg(resRes.at(i)).arg(resDirs.at(i));
    }

    //return a.exec();
    return 0;
}
