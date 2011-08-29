#include <QtCore>
//#include "./../libs/measurescanproc.h"
//#include "./mesthread.h"
#include "./../libs/fitsdata.h"
#include "./../libs/astro.h"

/* РѕРџРќР¦РџР®Р›Р›Р® Р”РљРЄ РћРќРЇРљР•Р”РќР‘Р®Р Р•РљР­РњРќР¦Рќ РҐ Р›РњРќР¦РќРћРќР РќР’РњРќР¦Рќ РҐР“Р›Р•РџР•РњРҐРЄ РќР РЇР™Р®РњРҐРџРќР‘Р®РњРњРЁРЈ РћРљР®РЇР РҐРњРќР™ РЇ РћРќР›РќР«Р­Р§ measure*/

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

    setlocale(LC_NUMERIC, "C");

    QString srTimeCode;
    detTimeCode(srTimeCode);

    QString logFileName = QString("./logs/srPack%1.log").arg(srTimeCode);
    QFile* logFile = new QFile(logFileName);
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);

    QString codecName;

    #if defined(Q_OS_LINUX)
        codecName = "UTF-8";
    #elif defined(Q_OS_WIN)
        codecName = "CP1251";
    #endif

    QTextCodec *codec1 = QTextCodec::codecForName(codecName.toAscii().constData());
    Q_ASSERT( codec1 );
    QTextCodec::setCodecForCStrings(codec1);

    QString cfgFile = QString("./conf/srPack.ini");
    if(argc==3) cfgFile = QString(argv[2]);

    QSettings *sett = new QSettings(cfgFile, QSettings::IniFormat);

    QString workDir;

    if(argc==1) workDir = QString(sett->value("general/workDir", "./").toByteArray());
    else workDir = QString(argv[1]);
    //QString workDir = codec1->toUnicode(sett->value("general/workDir", "./").toByteArray());//QDir::currentPath();
    QString scansoftDir = sett->value("general/scansoftDir", "c:/scansoft").toString();
    //int procNum = sett->value("general/procNum", "1").toInt();
    QString get_http_prog = sett->value("general/get_http_prog", "getHttpHeader.exe").toString();
    QString get_http_prog_folder = sett->value("general/get_http_prog_folder", "c:/scansoft").toString();
    int getHttpWait = sett->value("general/getHttpWait", 100000).toInt();
    QString instrName = sett->value("general/instrName", "na").toString();

    QString measure_prog = sett->value("general/measure_prog", "measure").toString();
    QString measure_prog_folder = sett->value("general/measure_prog_folder", "c:/scansoft").toString();
    QString start4_prog = sett->value("general/start4_prog", "start4.bat").toString();
    QString start4_prog_folder = sett->value("general/start4_prog_folder", "c:/scansoft").toString();
    int start4_wait = sett->value("general/start4_wait", 100000).toInt();
    QString raw_prog = sett->value("general/raw_prog", "raw4.bat").toString();
    QString raw_prog_folder = sett->value("general/raw_prog_folder", "c:/scansoft").toString();
    int raw_wait = sett->value("general/raw_wait", 1000000).toInt();
    int mWaitTime = sett->value("general/measureWait", "-1").toInt();
    int doStart4 = sett->value("general/doStart4", 1).toInt();
    int doRaw4 = sett->value("general/doRaw4", 1).toInt();
    int isParts = sett->value("general/isParts", 0).toInt();
    int tollim = sett->value("general/tollim", 100).toInt();
    int cmplim = sett->value("general/cmplim", 12).toInt();

    int i;
    QStringList arjList;
    int arjNum, expNum, isDir;

    QString pnStr, curArj, curDir, eNumStr, tFile, tStr, pnTempStr;
    //QStringList tPath;
    //QTextStream catStream;
    //QString dirsStr;

    qDebug() << QString("workDir: %1").arg(workDir);
    qDebug() << QString("scansoftDir: %1").arg(scansoftDir);

    ExposureRec* expRec;
    ExposureList* expList;
    QProcess httpProcess;
    QProcess mesProcess;
    //QString dir_prog = QString("listDir.bat");
    QStringList outerArguments;
    outerArguments.clear();
    int resHttp, resMes;
    QString hOut;
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
         if((tFile.lastIndexOf("/..")==tFile.lastIndexOf("/"))||(tFile.lastIndexOf("/.")==tFile.lastIndexOf("/"))||(tFile.right(1)==QString("."))) continue;
         if((tFile.lastIndexOf(".arj")==tFile.size()-4)||(tFile.lastIndexOf(".ARJ")==tFile.size()-4)) arjList << tFile;



         // /etc/.
         // /etc/..
         // /etc/X11
         // /etc/X11/fs
         // ...

     }

    //prepareDirs(&dirList);


    qDebug() << "arjList:\n" << arjList.join("\n");


    //dirList = dirsStr.split("\n");


    fitsdata *ftemp = new fitsdata;
   // QFile toFile;

    QStringList resDirs;
    QList <int> resRes;

    //QString filter = QString("*.*");
    //QString pnTempStr;

    arjNum = arjList.size();

    QString progType;
    //QDir tDir;
    QDir tDir1;
    int isLog;

    for(i=0; i<arjNum; i++)
    {
        curArj = arjList.at(i);
        qDebug() << QString("curArj: %1\n").arg(curArj);
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
        QDir tDir = QDir(QString("%1.dir").arg(curArj));




        curDir = curArj.left(curArj.lastIndexOf("/"));
        qDebug() << QString("curDir: %1\n").arg(curDir);
        tDir1.setPath(curDir);
        tStr = curArj.section("/", -1, -1);
        progType = tStr.section(".", 0, 0);
        qDebug() << QString("progType: %1").arg(progType);
        resMes = 0;

        isDir = tDir.exists();
        isLog = tDir1.exists(curArj+".log");
        if(isLog)
        {
            qDebug() << QString("zanyato: %1\n").arg(curArj);
            continue;
        }

        if(isDir)
        {
            tDir1.rmpath(tDir.path()+"\\*.*");
        }

        if(((progType==QString("average"))||(progType==QString("email")))&&doStart4)
        {
            mesProcess.setWorkingDirectory(curDir);
            outerArguments.clear();
            outerArguments << tStr;
            if(isParts) outerArguments << QString("%1").arg(tollim);
            qDebug() << start4_prog << outerArguments.join(" ");



            mesProcess.start(start4_prog, outerArguments);

            resMes = mesProcess.waitForFinished(start4_wait);

            if(!resMes)
            {
                mesProcess.close();
                //continue;
            }
        }
        else if((progType==QString("raw"))&&doRaw4)
        {

            pnStr = curArj.section("/", -2, -2);
            qDebug() << QString("pnStr: %1\n").arg(pnStr);

            httpProcess.setWorkingDirectory(get_http_prog_folder);
            outerArguments.clear();
            outerArguments << instrName << pnStr;

            qDebug() << get_http_prog << outerArguments.join("|");
            httpProcess.start(get_http_prog, outerArguments);

            resHttp = httpProcess.waitForFinished(getHttpWait);
            qDebug() << QString("resHttp: %1\n").arg(resHttp);
            if(!resHttp)
            {
                httpProcess.close();
                continue;
            }


            //catStream.flush();
            QTextStream httpStream(httpProcess.readAllStandardOutput());
            hOut = httpStream.readAll();
            //qDebug() << QString("hOut:\n%1\n").arg(hOut);

            ftemp->clear();
            //ftemp->openEmptyFile();
            if(ftemp->readHttpHeader(hOut))
            {
                qDebug() << "header not found\n";
                continue;
            }


            ftemp->headList.getKeyName("PLATE-N", &pnTempStr);
            qDebug() << QString("|%1|%2|\n").arg(pnTempStr.simplified()).arg(pnStr);
            if(pnTempStr.simplified().indexOf(pnStr)==-1)
            //if(pnTempStr.simplified()!=pnStr)
            {
                qDebug() << "no such plate name\n";
                continue;
            }
            //ftemp->headList.getKeyName("N-EXP", &eNumStr);
            //expNum = eNumStr.toInt();

            expList = new ExposureList;
            expRec = new ExposureRec;

            int expRes = initExpList(expList, ftemp->headList, NULL);

            expNum = expList->exps.size();

            mesProcess.setWorkingDirectory(curDir);
            outerArguments.clear();
            outerArguments << tStr << QString("%1").arg(expNum);
            if(isParts) outerArguments << QString("%1").arg(cmplim);
            qDebug() << raw_prog << outerArguments.join(" ");

            isDir = tDir.exists();
            if(isDir)
            {
                qDebug() << QString("zanyato: %1\n").arg(curArj);
                continue;
            }

            mesProcess.start(raw_prog, outerArguments);

            resMes = mesProcess.waitForFinished(raw_wait);

            if(!resMes)
            {
                mesProcess.close();
                //continue;
            }
        }
        else qDebug() << "\nwrong arj\n";

        resRes << resMes;
        resDirs << curDir;
    }


    int rsize = resDirs.size();
    qDebug() << "\nres\n";
    for(i=0; i<rsize; i++)
    {
        qDebug() << QString("%1: %2").arg(resRes.at(i)).arg(resDirs.at(i));
    }


    qInstallMsgHandler(0);
    logFile->close();

    //return a.exec();
    return 0;
}
