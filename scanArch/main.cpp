#include <QtCore>

#include "./../libs/fitsdata.h"

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

struct plateDirs;

void prepareDirs(QStringList *dirs);

void useWCS(QList <plateDirs*> plDirList, QString wcs2mks_prog, QString wcs2mks_prog_folder, int plNameType, int isWCS2MKS = 1);
int getPathPlName(QString *plName, QString fPath);

void useLor(QList <plateDirs*> plDirList);
void useLorNum(QList <plateDirs*> plDirList);

struct plateDirs
{
    QString plPath, plName;
    int expNum;
    QStringList eMks, wMks, sMks, nMks;
};

int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);

    QString logFileName = QString("sarch.log");
                          QFile* logFile = new QFile(logFileName);
                          if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
                              clog = new QDataStream(logFile);

    //set Settings

    QSettings *sett = new QSettings("./conf/sarch.ini", QSettings::IniFormat);

    QString workDir;

    QTextCodec *codec1 = QTextCodec::codecForName("KOI8-R");
    Q_ASSERT( codec1 );

    QTextCodec *codec2 = QTextCodec::codecForName("CP1251");
    Q_ASSERT( codec2 );

    if(argc==2) workDir = codec2->toUnicode(argv[1]);
    else workDir = sett->value("general/workDir", "./").toString();
    qDebug() << QString("workDir: %1\n").arg(workDir);


    //procs
    QString aone_prog = sett->value("procs/aone_prog", "getHttpHeader.exe").toString();
    QString aone_prog_folder = sett->value("procs/aone_prog_folder", "./").toString();
    int aone_prog_wait = sett->value("procs/aone_prog_wait", -1).toInt();

    //end Settings

    QTime timer;
    timer.start();

    QString tFile;
    QStringList dirList;
    int dirNum;
    int i;
    int resAO;
    QProcess aoProcess;
    QStringList outerArguments;

    //prepare dirs
    //dirList << workDir;
    QDirIterator it(workDir, QDirIterator::Subdirectories);
     while (it.hasNext()) {
         //qDebug() << it.filePath();
         tFile = it.next();
 //        qDebug() << QString("tFile: %1\n").arg(tFile);
         //if((tFile.lastIndexOf("/..")==tFile.lastIndexOf("/"))||(tFile.lastIndexOf("/.")==tFile.lastIndexOf("/"))||(tFile.right(1)==QString("."))) continue;
         if(tFile.lastIndexOf("/")==-1) dirList << tFile.left(tFile.lastIndexOf("\\"));
         else dirList << tFile.left(tFile.lastIndexOf("/"));

     }



     //QList <plateDirs*> pDirList;

     prepareDirs(&dirList);
     dirNum = dirList.size();
     qDebug() << QString("dirNum: %1\n").arg(dirNum);
     qDebug() << "dirList:\n" << dirList.join("\n");

////////////////////////////
     for(i=0; i<dirNum; i++)
     {
         aoProcess.setWorkingDirectory(aone_prog_folder);
         outerArguments.clear();
         outerArguments << dirList.at(i);
         qDebug() << aone_prog << outerArguments.join(" ");
         aoProcess.start(aone_prog, outerArguments);

         resAO = aoProcess.waitForFinished(aone_prog_wait);
         qDebug() << QString("resAO= %1\n").arg(resAO);
         if(!resAO)
         {
             aoProcess.close();
             continue;
         }
     }


    qDebug() << QString("time elapsed: %1 min\n").arg(timer.elapsed()/1000.0/60.0);

    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);


    return 0;//a.exec();
}

void prepareDirs(QStringList *dirs)
{
    int i, j, sz;
    QString tStr;
   ;

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

void useWCS(QList <plateDirs*> plDirList, QString wcs2mks_prog, QString wcs2mks_prog_folder, int plNameType, int isWCS2MKS)
{
    qDebug() << "useWCS\n";
        int i, sz0, j, sz1;
        QDir curFolder;
        QStringList nFilter, fList;
        QString dataStr, plName;
        QProcess mksProcess;
        QStringList outerArguments;
        QTextStream mksStream;
        QString oName, pName;
        int resMks;

        QTextCodec *codec1 = QTextCodec::codecForName("KOI8-R");
        Q_ASSERT( codec1 );
        nFilter.clear();
        nFilter << QString("*.wcs");
        curFolder.setNameFilters(nFilter);
        sz0 = plDirList.size();
        for(i=0; i<sz0; i++)
        {
            curFolder.setPath(plDirList.at(i)->plPath);

            fList = curFolder.entryList();
            qDebug() << QString("fList:\n%1\n").arg(fList.join("\n"));

            sz1 = fList.size();

            for(j=0; j<sz1; j++)
            {
                resMks = 0;
                oName = curFolder.path()+"/"+fList.at(j);
                if(isWCS2MKS)
                {
                    outerArguments.clear();
                    mksProcess.setWorkingDirectory(wcs2mks_prog_folder);
                    mksProcess.setProcessChannelMode(QProcess::MergedChannels);
                    mksProcess.setReadChannel(QProcess::StandardOutput);

                    outerArguments << oName << QString("%1").arg(plNameType) << plDirList.at(i)->plName;
                    qDebug() << wcs2mks_prog << " " << outerArguments.join(" ");
                    mksProcess.start(wcs2mks_prog, outerArguments);
                    resMks = mksProcess.waitForFinished(-1);
                }

                pName = oName.left(oName.lastIndexOf("."))+".mks";
                qDebug() << QString("pName: %1\n").arg(pName);

                if(resMks||(!isWCS2MKS&&(curFolder.exists(pName))))
                {

                    if(fList.at(j).lastIndexOf("e")==0) {plDirList.at(i)->eMks << pName;qDebug()<< "\ne\n";}
                    if(fList.at(j).lastIndexOf("s")==0){ plDirList.at(i)->sMks << pName;qDebug()<< "\ns\n";}
                if(fList.at(j).lastIndexOf("w")==0){ plDirList.at(i)->wMks << pName;qDebug()<< "\nw\n";}
            if(fList.at(j).lastIndexOf("n")==0){ plDirList.at(i)->nMks << pName;qDebug()<< "\nn\n";}
                }
                //mksStream.setString(&QString(mksProcess.readAllStandardOutput()));
                //dataStr = mksStream.readAll();
            }
        }
}

void useLor(QList <plateDirs*> plDirList)
{
    QDir curFolder;
    QStringList nFilter, fList;
    int sz0 = plDirList.size();
    int i, j, sz1;
    QString tStr, pName;

    nFilter.clear();
    nFilter << QString("*.lor");
    curFolder.setNameFilters(nFilter);

    for(i=0; i<sz0; i++)
    {
        curFolder.setPath(plDirList.at(i)->plPath);
        fList = curFolder.entryList();
        qDebug() << QString("fList:\n%1\n").arg(fList.join("\n"));
        sz1 = fList.size();
        for(j=0; j<sz1; j++)
        {
            tStr = fList.at(j);
            if(tStr.lastIndexOf(".tif.lor")!=-1) continue;
            qDebug() << tStr << "\n";
            pName = curFolder.path()+"/"+tStr;
            qDebug() << QString("pName: %1\n").arg(pName);

            if(fList.at(j).lastIndexOf("e")==0) {plDirList.at(i)->eMks << pName;qDebug()<< "\ne\n";}
            if(fList.at(j).lastIndexOf("s")==0){ plDirList.at(i)->sMks << pName;qDebug()<< "\ns\n";}
        if(fList.at(j).lastIndexOf("w")==0){ plDirList.at(i)->wMks << pName;qDebug()<< "\nw\n";}
    if(fList.at(j).lastIndexOf("n")==0){ plDirList.at(i)->nMks << pName;qDebug()<< "\nn\n";}

        }
    }
}

void useLorNum(QList <plateDirs*> plDirList)
{
    QDir curFolder;
    QStringList nFilter, fList;
    int sz0 = plDirList.size();
    int i, j, sz1;
    QString tStr, pName;

    nFilter.clear();
    nFilter << "*.lor.000" << "*.lor.001" << "*.lor.002" << "*.lor.003" << "*.lor.004" << "*.lor.005" << "*.lor.006";
    curFolder.setNameFilters(nFilter);

    for(i=0; i<sz0; i++)
    {
        curFolder.setPath(plDirList.at(i)->plPath);
        fList = curFolder.entryList();
        qDebug() << QString("fList:\n%1\n").arg(fList.join("\n"));
        sz1 = fList.size();
        for(j=0; j<sz1; j++)
        {
            tStr = fList.at(j);
   //         if(tStr.lastIndexOf(".tif.lor")!=-1) continue;
            qDebug() << tStr << "\n";
            pName = curFolder.path()+"/"+tStr;
            qDebug() << QString("pName: %1\n").arg(pName);

            if(fList.at(j).lastIndexOf("e")==0) {plDirList.at(i)->eMks << pName;qDebug()<< "\ne\n";}
            if(fList.at(j).lastIndexOf("s")==0){ plDirList.at(i)->sMks << pName;qDebug()<< "\ns\n";}
        if(fList.at(j).lastIndexOf("w")==0){ plDirList.at(i)->wMks << pName;qDebug()<< "\nw\n";}
    if(fList.at(j).lastIndexOf("n")==0){ plDirList.at(i)->nMks << pName;qDebug()<< "\nn\n";}

        }
    }
}


/*
int getPathPlName(QString *plName, QString fPath)
{
    QString pnStr;

    QTextCodec *codec1 = QTextCodec::codecForName("KOI8-R");
    Q_ASSERT( codec1 );

    switch(plNameType)
    {
    case 0:
       pnStr = fPath.section("/", -1, -1);
       break;
    case 1:
       pnStr = fPath.section("/", -1, -1).section(".", 0, 0);
       pnStr = pnStr.right(pnStr.size()-2);
       break;
    }
    qDebug() << QString("pnStr: %1\n").arg(pnStr);


    QByteArray bpn = codec1->fromUnicode(pnStr);
    //qDebug() << QString("bpn: %1\n").arg(bpn.constData());
    *plName = bpn.toPercentEncoding("-/._~=?&");
    qDebug() << QString("plName: %1\n").arg(*plName);
}
*/
