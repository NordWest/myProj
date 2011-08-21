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

struct plateDir
{
    QString plPath, plName;
    int expNum;
    QStringList eMks, wMks, sMks, nMks;
};

void useWCS(plateDir* plDir, QString wcs2mks_prog, QString wcs2mks_prog_folder, QString mksConf, int plNameType, int isWCS2MKS);
void useLorNum(plateDir* plDir);
void useLor(plateDir* plDir);


int main(int argc, char *argv[])    //.exe scanPath
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    //QTextCodec *codec1 = QTextCodec::codecForName("KOI8-R");
    QTextCodec *codec1 = QTextCodec::codecForName("Windows-1251");
    Q_ASSERT( codec1 );

    QString workDir =  codec1->toUnicode(argv[1]);
    QDir curDir;

    curDir.setPath(workDir);
    if(curDir.exists("aone.log")) return 1;

    QString logFileName = workDir + "/aone.log";


    QFile* logFile = new QFile(logFileName);
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
      clog = new QDataStream(logFile);

    QTime timer;
    timer.start();

    int expNum;
    int j;
    int sz0;
    int resHttp, resTcut, resSZ;
    QString plName, pnTempStr, eNumStr, tName;
    QProcess httpProcess;
    QStringList outerArguments;
    QProcess cutProcess, szProcess;

    ///////////////////////////////////////////////////////////////////////////////////////

    //set Settings

    QSettings *sett = new QSettings("./conf/aone.ini", QSettings::IniFormat);

//    QString workDir = sett->value("general/workDir", "./").toString();
    int plNameType = sett->value("general/plNameType", 0).toInt();
    QString mksConf = sett->value("general/mksConf", "./conf/conf.ini").toString();

    //procs
    QString get_http_prog = sett->value("procs/get_http_prog", "getHttpHeader.exe").toString();
    QString get_http_prog_folder = sett->value("procs/get_http_prog_folder", "./").toString();
    int get_http_wait = sett->value("procs/getHttpWait", 100000).toInt();
    QString instrName = sett->value("procs/instrName", "na").toString();

    QString wcs2mks_prog = sett->value("procs/wcs2mks_prog", "wcs2mks.exe").toString();
    QString wcs2mks_prog_folder = sett->value("procs/wcs2mks_prog_folder", "./").toString();
    int mksWaitTime = sett->value("procs/wcs2mksWait", "-1").toInt();

    QString tifcut_prog = sett->value("procs/tifcut_prog", "tifCutter.exe").toString();
    QString tifcut_prog_folder = sett->value("procs/tifcut_prog_folder", "./").toString();
    int tifcutWaitTime = sett->value("procs/tifcutWait", "-1").toInt();

    QString arch7z_prog = sett->value("procs/arch7z_prog", "7z.exe").toString();
    QString arch7z_prog_folder = sett->value("procs/arch7z_prog_folder", "c:/Program Files/7-Zip/").toString();
    QStringList arch7z_arguments = sett->value("procs/arch7z_arguments", "a -t7z").toString().split("|");
    int is7z = sett->value("arch/is7z", 0).toInt();
    int isRemOrig = sett->value("arch/isRemOrig", 0).toInt();


    //marks

    int isUseWCS = sett->value("marks/isUseWCS", 0).toInt();
    int isWCS2MKS = sett->value("marks/isWCS2MKS", 0).toInt();
    int isUseLor = sett->value("marks/isUseLor", 0).toInt();
    int isUseLorNum = sett->value("marks/isUseLorNum", 0).toInt();

    //scanCut
    int isBigImg = sett->value("scanCut/isBigImg", 0).toInt();
    int isFits = sett->value("scanCut/isFits", 0).toInt();

    //end Settings

    fitsdata *ftemp = new fitsdata;
    ftemp->openEmptyFile();

    plateDir plDir;

    QStringList fFilter;
    QDir fDir;
    QStringList fitsList;

    //
    if(workDir.lastIndexOf("/")==-1) plName = workDir.section("\\", -1, -1);
    else plName = workDir.section("/", -1, -1);
  //  QByteArray bpn = codec1->fromUnicode(plName);
    //qDebug() << QString("bpn: %1\n").arg(bpn.constData());
  //  plName = bpn.toPercentEncoding("-/._~=?&");
    qDebug() << QString("plName: %1\n").arg(plName);
    //

    httpProcess.setWorkingDirectory(get_http_prog_folder);
    outerArguments.clear();
    outerArguments << instrName << plName;
    qDebug() << get_http_prog << outerArguments.join(" ");
    httpProcess.start(get_http_prog, outerArguments);

    resHttp = httpProcess.waitForFinished(get_http_wait);
    if(!resHttp)
    {
        httpProcess.close();
        return 1;
    }

    QTextStream httpStream(httpProcess.readAllStandardOutput());

    if(ftemp->readHttpHeader(httpStream.readAll()))
    {
        qDebug() << "header not found\n";
        return 1;
    }
    ftemp->headList.getKeyName("PLATE-N", &pnTempStr);
    qDebug() << QString("|%1|%2|\n").arg(pnTempStr.simplified()).arg(plName);
    if(pnTempStr.simplified().toUpper()!=plName.toUpper())
    {
        qDebug() << "no such plate name\n";
        return 1;
    }
    ftemp->headList.getKeyName("N-EXP", &eNumStr);
    expNum = eNumStr.toInt();

    plDir.expNum = expNum;
    plDir.plName = plName;
    plDir.plPath = workDir;

    //

    if(isUseWCS) useWCS(&plDir, wcs2mks_prog, wcs2mks_prog_folder, mksConf, plNameType, isWCS2MKS);

    if(isUseLor) useLor(&plDir);

    if(isUseLorNum) useLorNum(&plDir);

    fFilter << "*.*";
    int i, fSz;

    if(isBigImg)
    {
        qDebug() << "\nisBigImg\n";

            sz0 = plDir.eMks.size();
            for(j=0; j<sz0; j++)
            {
                tName = plDir.eMks.at(j)+".tif";
                if(!curDir.exists(tName))
                {
                    cutProcess.setWorkingDirectory(tifcut_prog_folder);
                    outerArguments.clear();
                    outerArguments << plDir.plPath+"/e.tif" << plDir.eMks.at(j) << "0";
                    qDebug() << tifcut_prog << outerArguments.join(" ");
                    cutProcess.start(tifcut_prog, outerArguments);

                    resTcut = cutProcess.waitForFinished(-1);
                    qDebug() << QString("resTcut: %1\n").arg(resTcut);
                }
                else resTcut = 1;
                if(resTcut&&is7z)
                {
                    szProcess.setWorkingDirectory(arch7z_prog_folder);
                    outerArguments.clear();
                    outerArguments << arch7z_arguments << plDir.eMks.at(j)+".tif.7z" << tName;
                    qDebug() << arch7z_prog << outerArguments.join(" ");
                    szProcess.start(arch7z_prog, outerArguments);

                    resSZ = szProcess.waitForFinished(-1);

                    QTextStream szStream(szProcess.readAllStandardOutput());
                    qDebug() << szStream.readAll();
                }
                if(resTcut&&isRemOrig) curDir.remove(tName);
            }

            sz0 = plDir.sMks.size();
            for(j=0; j<sz0; j++)
            {
                tName = plDir.sMks.at(j)+".tif";
                if(!curDir.exists(tName))
                {
                    cutProcess.setWorkingDirectory(tifcut_prog_folder);
                    outerArguments.clear();
                    outerArguments << plDir.plPath+"/s.tif" << plDir.sMks.at(j) << "0";
                    qDebug() << tifcut_prog << outerArguments.join(" ");
                    cutProcess.start(tifcut_prog, outerArguments);

                    resTcut = cutProcess.waitForFinished(-1);
                }

                if(resTcut&&is7z)
                {
                    szProcess.setWorkingDirectory(arch7z_prog_folder);
                    outerArguments.clear();
                    outerArguments << arch7z_arguments << plDir.sMks.at(j)+".tif.7z" << plDir.sMks.at(j)+".tif";
                    qDebug() << arch7z_prog << outerArguments.join(" ");
                    szProcess.start(arch7z_prog, outerArguments);

                    resSZ = szProcess.waitForFinished(-1);

                }
                if(resTcut&&isRemOrig) curDir.remove(tName);
            }

            sz0 = plDir.wMks.size();
            for(j=0; j<sz0; j++)
            {
                tName = plDir.wMks.at(j)+".tif";
                if(!curDir.exists(tName))
                {
                    cutProcess.setWorkingDirectory(tifcut_prog_folder);
                    outerArguments.clear();
                    outerArguments << plDir.plPath+"/w.tif" << plDir.wMks.at(j) << "0";
                    qDebug() << tifcut_prog << outerArguments.join(" ");
                    cutProcess.start(tifcut_prog, outerArguments);

                    resTcut = cutProcess.waitForFinished(-1);
                }

                if(resTcut&&is7z)
                {
                    szProcess.setWorkingDirectory(arch7z_prog_folder);
                    outerArguments.clear();
                    outerArguments << arch7z_arguments << plDir.wMks.at(j)+".tif.7z" << plDir.wMks.at(j)+".tif";
                    qDebug() << arch7z_prog << outerArguments.join(" ");
                    szProcess.start(arch7z_prog, outerArguments);

                    resSZ = szProcess.waitForFinished(-1);

                }
                if(resTcut&&isRemOrig) curDir.remove(tName);
            }

            sz0 = plDir.nMks.size();
            for(j=0; j<sz0; j++)
            {
                tName = plDir.nMks.at(j)+".tif";
                if(!curDir.exists(tName))
                {
                    cutProcess.setWorkingDirectory(tifcut_prog_folder);
                    outerArguments.clear();
                    outerArguments << plDir.plPath+"/n.tif" << plDir.nMks.at(j) << "0";
                    qDebug() << tifcut_prog << outerArguments.join(" ");
                    cutProcess.start(tifcut_prog, outerArguments);

                    resTcut = cutProcess.waitForFinished(-1);
                }

                if(resTcut&&is7z)
                {
                    szProcess.setWorkingDirectory(arch7z_prog_folder);
                    outerArguments.clear();
                    outerArguments << arch7z_arguments << plDir.nMks.at(j)+".tif.7z" << plDir.nMks.at(j)+".tif";
                    qDebug() << arch7z_prog << outerArguments.join(" ");
                    szProcess.start(arch7z_prog, outerArguments);

                    resSZ = szProcess.waitForFinished(-1);

                }
                if(resTcut&&isRemOrig) curDir.remove(tName);
            }

    }

    if(isFits)
    {
        qDebug() << "\nisFits\n";

            sz0 = plDir.eMks.size();
            for(j=0; j<sz0; j++)
            {
                tName = plDir.eMks.at(j)+".fits";
                cutProcess.setWorkingDirectory(tifcut_prog_folder);
                outerArguments.clear();
                outerArguments << plDir.plPath+"/e.tif" << plDir.eMks.at(j) << "1";
                qDebug() << tifcut_prog << outerArguments.join(" ");
                cutProcess.start(tifcut_prog, outerArguments);

                resTcut = cutProcess.waitForFinished(-1);

                if(resTcut&&is7z)
                {
                    szProcess.setWorkingDirectory(arch7z_prog_folder);
                    outerArguments.clear();
                    outerArguments << arch7z_arguments << tName+".7z" << tName;
                    qDebug() << arch7z_prog << outerArguments.join(" ");
                    szProcess.start(arch7z_prog, outerArguments);

                    resSZ = szProcess.waitForFinished(-1);

                }
                if(resTcut&&isRemOrig)
                {
                    fDir.setPath(tName);
                    fDir.setNameFilters(fFilter);
                    fitsList = fDir.entryList();
                    fSz = fitsList.size();
                    for(i=0; i<fSz; i++)fDir.remove(fitsList[i]);
                    curDir.rmdir(tName);
                }
            }

            sz0 = plDir.sMks.size();
            for(j=0; j<sz0; j++)
            {
                tName = plDir.sMks.at(j)+".fits";
                cutProcess.setWorkingDirectory(tifcut_prog_folder);
                outerArguments.clear();
                outerArguments << plDir.plPath+"/s.tif" << plDir.sMks.at(j) << "1";
                qDebug() << tifcut_prog << outerArguments.join(" ");
                cutProcess.start(tifcut_prog, outerArguments);

                resTcut = cutProcess.waitForFinished(-1);

                if(resTcut&&is7z)
                {
                    szProcess.setWorkingDirectory(arch7z_prog_folder);
                    outerArguments.clear();
                    outerArguments << arch7z_arguments << tName+".7z" << tName;
                    qDebug() << arch7z_prog << outerArguments.join(" ");
                    szProcess.start(arch7z_prog, outerArguments);

                    resSZ = szProcess.waitForFinished(-1);

                }
                if(resTcut&&isRemOrig)
                {
                    fDir.setPath(tName);
                    fDir.setNameFilters(fFilter);
                    fitsList = fDir.entryList();
                    fSz = fitsList.size();
                    for(i=0; i<fSz; i++)fDir.remove(fitsList[i]);
                    curDir.rmdir(tName);
                }
            }

            sz0 = plDir.wMks.size();
            for(j=0; j<sz0; j++)
            {
                tName = plDir.wMks.at(j)+".fits";
                cutProcess.setWorkingDirectory(tifcut_prog_folder);
                outerArguments.clear();
                outerArguments << plDir.plPath+"/w.tif" << plDir.wMks.at(j) << "1";
                qDebug() << tifcut_prog << outerArguments.join(" ");
                cutProcess.start(tifcut_prog, outerArguments);

                resTcut = cutProcess.waitForFinished(-1);

                if(resTcut&&is7z)
                {
                    szProcess.setWorkingDirectory(arch7z_prog_folder);
                    outerArguments.clear();
                    outerArguments << arch7z_arguments << tName+".7z" << tName;
                    qDebug() << arch7z_prog << outerArguments.join(" ");
                    szProcess.start(arch7z_prog, outerArguments);

                    resSZ = szProcess.waitForFinished(-1);

                }
                if(resTcut&&isRemOrig)
                {
                    fDir.setPath(tName);
                    fDir.setNameFilters(fFilter);
                    fitsList = fDir.entryList();
                    fSz = fitsList.size();
                    for(i=0; i<fSz; i++)fDir.remove(fitsList[i]);
                    curDir.rmdir(tName);
                }
            }

            sz0 = plDir.nMks.size();
            for(j=0; j<sz0; j++)
            {
                tName = plDir.nMks.at(j)+".fits";
                cutProcess.setWorkingDirectory(tifcut_prog_folder);
                outerArguments.clear();
                outerArguments << plDir.plPath+"/n.tif" << plDir.nMks.at(j) << "1";
                qDebug() << tifcut_prog << outerArguments.join(" ");
                cutProcess.start(tifcut_prog, outerArguments);

                resTcut = cutProcess.waitForFinished(-1);

                if(resTcut&&is7z)
                {
                    szProcess.setWorkingDirectory(arch7z_prog_folder);
                    outerArguments.clear();
                    outerArguments << arch7z_arguments << tName+".7z" << tName;
                    qDebug() << arch7z_prog << outerArguments.join(" ");
                    szProcess.start(arch7z_prog, outerArguments);

                    resSZ = szProcess.waitForFinished(-1);

                }
                if(resTcut&&isRemOrig)
                {
                    fDir.setPath(tName);
                    fDir.setNameFilters(fFilter);
                    fitsList = fDir.entryList();
                    fSz = fitsList.size();
                    for(i=0; i<fSz; i++)fDir.remove(fitsList[i]);
                    curDir.rmdir(tName);
                }
            }

    }

   /*
    if(isRemOrig)
    {

            //curDir.setPath(plDir.plPath);
            sz0 = plDir.eMks.size();
            for(j=0; j<sz0; j++)
            {
                if(isBigImg) curDir.remove(plDir.eMks.at(j)+".tif");
                if(isFits)
                {
                    fDir.setPath(plDir.eMks.at(j)+".fits");
                    fDir.setNameFilters(fFilter);
                    fitsList = fDir.entryList();
                    fSz = fitsList.size();
                    for(i=0; i<fSz; i++)fDir.remove(fitsList[i]);
                    curDir.rmdir(plDir.eMks.at(j)+".fits");
                }
            }
            sz0 = plDir.sMks.size();
            for(j=0; j<sz0; j++)
            {
                if(isBigImg) curDir.remove(plDir.sMks.at(j)+".tif");
                if(isFits)
                {
                    fDir.setPath(plDir.sMks.at(j)+".fits");
                    fDir.setNameFilters(fFilter);
                    fitsList = fDir.entryList();
                    fSz = fitsList.size();
                    for(i=0; i<fSz; i++)fDir.remove(fitsList[i]);
                    curDir.rmdir(plDir.sMks.at(j)+".fits");
                }
            }
            sz0 = plDir.wMks.size();
            for(j=0; j<sz0; j++)
            {
                if(isBigImg) curDir.remove(plDir.wMks.at(j)+".tif");
                if(isFits)
                {
                    fDir.setPath(plDir.wMks.at(j)+".fits");
                    fDir.setNameFilters(fFilter);
                    fitsList = fDir.entryList();
                    fSz = fitsList.size();
                    for(i=0; i<fSz; i++)fDir.remove(fitsList[i]);
                    curDir.rmdir(plDir.wMks.at(j)+".fits");
                }
            }
            sz0 = plDir.nMks.size();
            for(j=0; j<sz0; j++)
            {
                if(isBigImg) curDir.remove(plDir.nMks.at(j)+".tif");
                if(isFits)
                {
                    fDir.setPath(plDir.nMks.at(j)+".fits");
                    fDir.setNameFilters(fFilter);
                    fitsList = fDir.entryList();
                    fSz = fitsList.size();
                    for(i=0; i<fSz; i++)fDir.remove(fitsList[i]);
                    curDir.rmdir(plDir.nMks.at(j)+".fits");
                }
            }

    }
*/

    ///////////////////////////////////////////////////////////////////////////////////////

    qDebug() << QString("time elapsed: %1 min\n").arg(timer.elapsed()/1000.0/60.0);

    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);

    return 0;
}


void useWCS(plateDir* plDir, QString wcs2mks_prog, QString wcs2mks_prog_folder, QString mksConf, int plNameType, int isWCS2MKS)
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

            curFolder.setPath(plDir->plPath);

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

                    outerArguments << mksConf << oName << QString("%1").arg(plNameType) << plDir->plName;
                    qDebug() << wcs2mks_prog << " " << outerArguments.join(" ");
                    mksProcess.start(wcs2mks_prog, outerArguments);
                    resMks = mksProcess.waitForFinished(-1);
                }

                pName = oName.left(oName.lastIndexOf("."))+".mks";
                qDebug() << QString("pName: %1\n").arg(pName);

                if(resMks||(!isWCS2MKS&&(curFolder.exists(pName))))
                {

                    if(fList.at(j).indexOf("e")==0){ plDir->eMks << pName;qDebug()<< "\ne\n";}
                    if(fList.at(j).indexOf("s")==0){ plDir->sMks << pName;qDebug()<< "\ns\n";}
                    if(fList.at(j).indexOf("w")==0){ plDir->wMks << pName;qDebug()<< "\nw\n";}
                    if(fList.at(j).indexOf("n")==0){ plDir->nMks << pName;qDebug()<< "\nn\n";}
                }
                //mksStream.setString(&QString(mksProcess.readAllStandardOutput()));
                //dataStr = mksStream.readAll();
            }

}

void useLor(plateDir* plDir)
{
    QDir curFolder;
    QStringList nFilter, fList;

    int i, j, sz1;
    QString tStr, pName;

    nFilter.clear();
    nFilter << QString("*.lor");
    curFolder.setNameFilters(nFilter);


        curFolder.setPath(plDir->plPath);
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

            if(fList.at(j).indexOf("e")==0) {plDir->eMks << pName;qDebug()<< "\ne\n";}
            if(fList.at(j).indexOf("s")==0){ plDir->sMks << pName;qDebug()<< "\ns\n";}
            if(fList.at(j).indexOf("w")==0){ plDir->wMks << pName;qDebug()<< "\nw\n";}
            if(fList.at(j).indexOf("n")==0){ plDir->nMks << pName;qDebug()<< "\nn\n";}

        }

}

void useLorNum(plateDir* plDir)
{
    QDir curFolder;
    QStringList nFilter, fList;

    int i, j, sz1;
    QString tStr, pName;

    nFilter.clear();
    nFilter << "*.lor.000" << "*.lor.001" << "*.lor.002" << "*.lor.003" << "*.lor.004" << "*.lor.005" << "*.lor.006";
    curFolder.setNameFilters(nFilter);


        curFolder.setPath(plDir->plPath);
        fList = curFolder.entryList();
        qDebug() << QString("fList:\n%1\n").arg(fList.join("\n"));
        sz1 = fList.size();
        for(j=0; j<sz1; j++)
        {
            tStr = fList.at(j);
            qDebug() << tStr << "\n";
            pName = curFolder.path()+"/"+tStr;
            qDebug() << QString("pName: %1\n").arg(pName);

            if(fList.at(j).indexOf("e")==0) {plDir->eMks << pName;qDebug()<< "\ne\n";}
            if(fList.at(j).indexOf("s")==0){ plDir->sMks << pName;qDebug()<< "\ns\n";}
            if(fList.at(j).indexOf("w")==0){ plDir->wMks << pName;qDebug()<< "\nw\n";}
            if(fList.at(j).indexOf("n")==0){ plDir->nMks << pName;qDebug()<< "\nn\n";}
        }

}
