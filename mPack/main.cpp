#include <QtCore>
//#include "./../libs/measurescanproc.h"
//#include "./mesthread.h"
#include "./../libs/fitsdata.h"

/* РѕРџРќР¦РџР®Р›Р›Р® Р”РљРЄ РћРќРЇРљР•Р”РќР‘Р®Р Р•РљР­РњРќР¦Рќ РҐ Р›РњРќР¦РќРћРќР РќР’РњРќР¦Рќ РҐР“Р›Р•РџР•РњРҐРЄ РќР РЇР™Р®РњРҐРџРќР‘Р®РњРњРЁРЈ РћРљР®РЇР РҐРњРќР™ РЇ РћРќР›РќР«Р­Р§ measure*/

static QDataStream* clog = 0;
void customMessageHandler(QtMsgType type, const char* msg)
{
    static const char* msgType[] =
    {
        "Debug    : ",
        "Warning  : ",
        "Critical : ",
        "Fatal    : "
    };

    static QTextStream cout(stdout);
    static QTextStream cerr(stderr);

    cerr << msgType[type] << msg << endl;
    if(clog && clog->device())
        *clog << type << msg;
    if(type == QtFatalMsg)
    {
        cerr << "aborting..." << endl;

#if defined(Q_CC_MSVC) && defined(QT_DEBUG) && defined(_CRT_ERROR) && defined(_DEBUG)
        int reportMode = _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
        _CrtSetReportMode(_CRT_ERROR, reportMode);
        int ret = _CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, QT_VERSION_STR, msg);
        if(ret == 0 && reportMode & _CRTDBG_MODE_WNDW)
            return;
        else if(ret == 1)
            _CrtDbgBreak();
#endif

#if defined(Q_OS_UNIX) && defined(QT_DEBUG)
        abort();
#else
        exit(1);
#endif
    }
}


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
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);

    QString logFileName = QString("./mPack.log");
    QFile* logFile = new QFile(logFileName);
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);

    QString lockFileName;// = QString("pack.lock");


    QTextCodec *codec1 = QTextCodec::codecForName("Windows-1251");
    //QTextCodec *codec2 = QTextCodec::codecForName("Windows-1251");


    QString cfgFile = QString("./conf/mPack.ini");
    if(argc==3) cfgFile = QString(argv[2]);

    QSettings *sett = new QSettings(cfgFile, QSettings::IniFormat);
    QString workDir;

    if(argc==1) workDir = codec1->toUnicode(sett->value("general/workDir", "./").toByteArray());
    else workDir = codec1->toUnicode(argv[1]);

    //QString workDir = sett->value("general/workDir", "./").toString();//QDir::currentPath();
    QString scansoftDir = sett->value("general/scansoftDir", "c:/scansoft").toString();
    //int procNum = sett->value("general/procNum", "1").toInt();
    QString get_http_prog = sett->value("general/get_http_prog", "getHttpHeader.exe").toString();
    QString get_http_prog_folder = sett->value("general/get_http_prog_folder", "c:/scansoft").toString();
    int getHttpWait = sett->value("general/getHttpWait", 100000).toInt();
    QString instrName = sett->value("general/instrName", "na").toString();

    QString measure_prog = sett->value("general/measure_prog", "measure").toString();
    QString measure_prog_folder = sett->value("general/measure_prog_folder", "c:/scansoft").toString();
    int mWaitTime = sett->value("general/measureWait", "-1").toInt();

    QString isNStr = sett->value("general/isN", "n.tif").toString();
    QString isEStr = sett->value("general/isE", "e.tif").toString();
    QString isSStr = sett->value("general/isS", "s.tif").toString();
    QString isWStr = sett->value("general/isW", "w.tif").toString();
    //QString lockFile = sett->value("general/lockFile", "lorentz.log").toString();

    int isMeasureParts = sett->value("general/isMeasureParts", 0).toInt();
    int lim = sett->value("general/lim", 10).toInt();

    int i;
    QStringList dirList;
    int dirNum = dirList.size();
    int isLock, isTif, expNum, isArj, isDone;
    int isN, isE, isW, isS;
    QString pnStr, curDir, eNumStr, tDir, tFile, diskLet;
    QStringList tPath;
    QTextStream catStream;
    QString dirsStr;


    qDebug() << QString("workDir: %1").arg(workDir);
    qDebug() << QString("scansoftDir: %1").arg(scansoftDir);

    QProcess httpProcess;
    QProcess mesProcess;
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
    QDirIterator it(workDir, QDirIterator::Subdirectories);
     while (it.hasNext()) {
         //qDebug() << it.filePath();
         tFile = it.next();
         //qDebug() << QString("tFile: %1\n").arg(tFile);
         //if((tFile.lastIndexOf("/..")==tFile.lastIndexOf("/"))||(tFile.lastIndexOf("/.")==tFile.lastIndexOf("/"))||(tFile.right(1)==QString("."))) continue;
         dirList << tFile.left(tFile.lastIndexOf("/"));



         // /etc/.
         // /etc/..
         // /etc/X11
         // /etc/X11/fs
         // ...

     }

    prepareDirs(&dirList);


    qDebug() << "dirList:\n" << dirList.join("\n");


    //dirList = dirsStr.split("\n");


    fitsdata *ftemp = new fitsdata;
    QFile toFile;

    QStringList resDirs;
    QList <int> resRes;

    QString filter = QString("*.*");
    QString pnTempStr;

    dirNum = dirList.size();



    for(i=0; i<dirNum; i++)
    {
        curDir = dirList.at(i);
        qDebug() << QString("curDir: %1\n").arg(curDir);
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
        lockFileName = QString("%1/mpack.lock").arg(curDir);

        isLock = QFile::exists(lockFileName);

        isN = QFile::exists(QString("%1/%2").arg(curDir).arg(isNStr));
        //qDebug() << QString("N: %1\n").arg(QString("%1/n.tif").arg(curDir));
        isE = QFile::exists(QString("%1/%2").arg(curDir).arg(isEStr));
        isW = QFile::exists(QString("%1/%2").arg(curDir).arg(isWStr));
        isS = QFile::exists(QString("%1/%2").arg(curDir).arg(isSStr));
        isTif = isN&&isW&&isS&&isE;

        isArj = (QFile::exists(QString("%1/average.arj").arg(curDir))||QFile::exists(QString("%1/raw.arj").arg(curDir)));

        if(isLock||isArj)
        {
            qDebug() << QString("zanyato: %1\n").arg(curDir);
            continue;
        }
        if(!isTif)
        {
            qDebug() << QString("it has no tifs: %1\n").arg(curDir);
            qDebug() << QString("n: %1\te: %2\ts:%3\tw:%4\n").arg(isN).arg(isE).arg(isS).arg(isW);
            continue;
        }



        QFile lockFile(lockFileName);
        lockFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
        lockFile.close();



        pnStr = curDir.section("/", -1, -1);

        //pnStr = curDir.section("/", -1, -1);
        qDebug() << QString("pnStr: %1\n").arg(pnStr);

        httpProcess.setWorkingDirectory(get_http_prog_folder);
        outerArguments.clear();
        outerArguments << instrName << pnStr;
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

        ftemp->clear();
        ftemp->openEmptyFile();
        if(ftemp->readHttpHeader(httpStream.readAll()))
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

        //ftemp->headList.getKeyName("N-EXP", &eNumStr);
        expNum = ftemp->expList->exps.size();//eNumStr.toInt();

        mesProcess.setWorkingDirectory(QString(curDir+"/"));
        outerArguments.clear();
        outerArguments << QString("%1").arg(expNum);
        if(isMeasureParts) outerArguments << QString("%1").arg(lim);
        qDebug() << measure_prog << outerArguments.join(" ");

        isDone = QFile::exists(QString("%1/average.arj").arg(curDir))||QFile::exists(QString("%1/raw.arj").arg(curDir));
        if(isDone)
        {
            qDebug() << QString("allready done: %1\n").arg(curDir);
            continue;
        }

        mesProcess.start(measure_prog, outerArguments);

        resMes = mesProcess.waitForFinished(mWaitTime);

        if(!resMes)
        {
            mesProcess.close();
            //continue;
        }

        resRes << resMes;
        resDirs << curDir;

        QDir(curDir).remove(lockFileName);
    }


    int rsize = resDirs.size();
    qDebug() << "\nres\n";
    for(i=0; i<rsize; i++)
    {
        qDebug() << QString("%1: %2").arg(resRes.at(i)).arg(resDirs.at(i));
    }

    qInstallMsgHandler(0);
    //return a.exec();
    return 0;
}
