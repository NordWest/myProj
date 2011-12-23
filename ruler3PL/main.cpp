#include <QCoreApplication>
#include <QtCore>

#ifndef FITSTOOLS_H
#include "./../libs/fitstools.h"
#endif

#ifndef FITSDATA_H
#include "./../libs/fitsdata.h"
#endif

#include "./../astro/astro.h"
#include "./../libs/comfunc.h"
#include "./../libs/observatory.h"

static QTextStream* clog = 0;
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

    //cout << msgType[type] << msg << endl;
    cout << msg << endl;
    if(clog && clog->device())
        //*clog << type << msg;
        *clog << msg;

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

int main(int argc, char *argv[])    //ruler3PL.exe file.mks [options] [config=cfg.ini] [plName=10000] [headerFile=./header.txt] [resFolder=./results]
    {  
        qInstallMsgHandler(customMessageHandler);
        QCoreApplication app(argc, argv);
        //QTextStream stream(stdout);
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

        QString uTime;
        QString dateStr, timeStr;
        QProcess outerProcess;
        QStringList outerArguments;
        QString optName, optVal, optStr, pnStr, headerFileName;
        QString logFileName, filePath, wcsFileName;
        QString resFolder, logFolder;
        sysCorrParam *sysCorr = NULL;
        QString cfgFileName = "ruler3PL.ini";
        int useHeaderFile = 0;                         //0-http; 1-file
        int detPlName = 1;
        int resdirDef=0;
        QString scFile, descS, oName;
        int isSc = 0;
        QList <catFinder*> starCatList;
        QString obsCode;
        int sz, i, oNum;
        int resRed = 0;
        int detHname = 0;
        int isLF = 0;

    //command line  ///////////////////////////////////////


        for(i=2; i<argc; i++)
        {
            optStr = QString(argv[i]);
            optName = optStr.section("=", 0, 0);
            optVal = optStr.section("=", 1, 1);
            if(QString::compare(optName, "config", Qt::CaseSensitive)==0)
            {
                cfgFileName = optVal;
            }
            else if(QString::compare(optName, "plName", Qt::CaseSensitive)==0)
            {
                detPlName = 0;
                pnStr = optVal;
            }
            else if(QString::compare(optName, "headerFile", Qt::CaseSensitive)==0)
            {
                detPlName = 0;
                useHeaderFile = 1;
                headerFileName = optVal;
            }
            else if(QString::compare(optName, "resFolder", Qt::CaseSensitive)==0)
            {
                resdirDef=1;
                resFolder = optVal;
            }
            else if(QString::compare(optName, "syscorr", Qt::CaseSensitive)==0)
            {
                isSc = 1;
                scFile = optVal;
            }
            else if(QString::compare(optName, "logFolder", Qt::CaseSensitive)==0)
            {
                isLF = 1;
                logFolder = optVal;
            }
            else
            {
                qDebug() << QString("Error option %1.\nUseage: config|plName|headerFile|resFolder|syscorr\n").arg(optName);
                return 1;
            }
        }

///////// 1. Reading settings ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //QString cfgFile = QString(cfgFileName);

        refractionParam *refParam = NULL;

        qDebug() << "Reading settings....\n";

        QSettings *sett = new QSettings(cfgFileName, QSettings::IniFormat);

        QString redparamIni = sett->value("general/redparamIni", "./redParam.ini").toString();
        int expNum = sett->value("general/expNum", -1).toInt();
        int aper = sett->value("general/aperture", 20).toInt();
        int isRedRef = sett->value("general/isRedRef", 0).toInt();
        int plNameType = sett->value("general/plNameType", 0).toInt();
        int useUtCorr = sett->value("general/useUtCorr", 0).toInt();
        if(!resdirDef) resFolder = sett->value("general/resFolder", "./results").toString();
        double fovp = sett->value("general/fovp", 1.0).toDouble();        //field of view percent, [0.0 - 1.0]
        if(!useHeaderFile)
        {
            useHeaderFile = sett->value("general/useHeaderFile", 0).toInt();
            detHname = 1;
        }
        int saveHeaderFile = sett->value("general/saveHeaderFile", 0).toInt();

//  logs
        int useLogLock = sett->value("logs/useLogLock", 0).toInt();
        int isRemLog = sett->value("logs/isRemLog", 0).toInt();

//  syscorr
        QString syscorIni = sett->value("syscorr/syscorIni", "./syscorr.ini").toString();
        int useSysCorr = sett->value("syscorr/useSysCorr", 0).toInt();

//objects   ////
        int lspmFind = sett->value("objects/lspmFind", 0).toInt();
        int skybotFind = sett->value("objects/skybotFind", 0).toInt();
        int tryMpeph = sett->value("objects/tryMpeph", 0).toInt();
        int mpephType = sett->value("objects/mpephType", 0).toInt();
        double magObj0 = sett->value("objects/mag0", 6.0).toDouble();
        double magObj1 = sett->value("objects/mag1", 16.0).toDouble();
        QString headObjName = sett->value("objects/headObjName", "OBJECT").toString();

//  catalogs    /////
        QString catIni = sett->value("catalogs/catIni", "./catalogs.ini").toString();
        int catProgType = sett->value("catalogs/catProgType", 0).toInt();
        double mag0 = sett->value("catalogs/mag0", 6.0).toDouble();
        double mag1 = sett->value("catalogs/mag1", 16.0).toDouble();

//processes
        QString mpeph_prog = sett->value("processes/mpeph_prog", "./mpeph").toString();
        QString mpeph_prog_folder = sett->value("processes/mpeph_prog_folder", "./").toString();
        int mpeph_wait_time = sett->value("processes/mpeph_wait_time", -1).toInt();
        if(mpeph_wait_time>0) mpeph_wait_time *= 1000;
        QString skybot_prog = sett->value("processes/skybot_prog", "./skybotclient").toString();
        QString skybot_prog_folder = sett->value("processes/skybot_prog_folder", "./").toString();
        int skybot_wait_time = sett->value("processes/skybot_wait_time", -1).toInt();
        if(skybot_wait_time>0) skybot_wait_time *= 1000;
        QString gethttp_prog = sett->value("processes/gethttp_prog", "./getHttpHeader").toString();
        QString gethttp_prog_folder = sett->value("processes/gethttp_prog_folder", "./").toString();
        int gethttp_wait_time = sett->value("processes/gethttp_wait_time", -1).toInt();
        if(gethttp_wait_time>0) gethttp_wait_time *= 1000;
        QString utcorr_prog = sett->value("processes/utcorr_prog", "./uTimeCorr").toString();
        QString utcorr_prog_folder = sett->value("processes/utcorr_prog_folder", "./").toString();
        int utcorr_wait_time = sett->value("processes/utcorr_wait_time", -1).toInt();
        if(utcorr_wait_time>0) utcorr_wait_time *= 1000;

//observatory
        QString observatoryCat = sett->value("observatory/observatoryCat", "./Obs.txt").toString();
        obsCode = sett->value("observatory/obsCode", "084").toString();

//marks /////////////////

        QString mSep = sett->value("marks/mSep", " ").toString();
        QString mCol = sett->value("marks/mCol", "1,2,3,4,5").toString();
        int detRect = sett->value("marks/detRect", 0).toInt();
        double rectX0 = sett->value("marks/rectX0", 0.0).toDouble();
        double rectY0 = sett->value("marks/rectY0", 0.0).toDouble();
        double rectX1 = sett->value("marks/rectX1", 0.0).toDouble();
        double rectY1 = sett->value("marks/rectY1", 0.0).toDouble();

///////////////////////////////////////////////////////////////////////////////////////////////////////

        QString fileName =  QString(argv[1]);

        QDir resDir("./");
        if(resDir.mkpath(resFolder))qDebug() << "\nresFolder created\n";
        else qDebug() << "\nresFolder don't create\n";
        //if(resDir.mkpath(logFolder))qDebug() << "\nlogFolder created\n";
        //else qDebug() << "\nlogFolder don't create\n";

        QFileInfo fi(fileName);
        filePath = fi.absoluteFilePath();//fileName.left(fileName.lastIndexOf("\\")+1);
        if(filePath=="") filePath = QString("./");
        wcsFileName = QString("%1.wcs").arg(fileName);
        //QString logFileName = QString("%1/%2.log").arg(logFolder).arg(fi.fileName());
        //if(isLP) logFileName = QString("%1/%2.log").arg(QDir(logPath).absolutePath()).arg(fi.fileName());
        //else logFileName = QString("%1.log").arg(fi.absoluteFilePath());
        if(isLF)
        {
            QDir().mkdir(logFolder);
            QDir ldir(logFolder);

            logFileName = QString("%1/%2.log").arg(ldir.absolutePath()).arg(fi.fileName());
        }
        else logFileName = QString("%1.log").arg(fi.absoluteFilePath());

        if(detHname) headerFileName = QString("%1.hdr").arg(fileName);

        if(useLogLock&&QDir().exists(logFileName))
        {
            qDebug() << QString("allready scanned\n");
            if(isRemLog)QDir().remove(logFileName);
            return 1;
        }
        QFile* logFile = new QFile(logFileName);
        if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
            clog = new QTextStream(logFile);


        qDebug() << QString("fileName: %1\n").arg(fileName);
        qDebug() << QString("filePath: %1\n").arg(filePath);
        qDebug() << QString("wcsFileName: %1\n").arg(wcsFileName);
        qDebug() << QString("logFileName: %1\n").arg(logFileName);


        QStringList allSett;
        allSett << sett->allKeys();
        sz = allSett.size();
        qDebug() << QString("\n\nconfig: %1\n").arg(cfgFileName);
        for(i=0; i<sz; i++) qDebug() << QString("%1=%2\n").arg(allSett.at(i)).arg(sett->value(allSett.at(i)).toString());
        qDebug() << "\n\n";

/////////////////////////////

        initCatList(&starCatList, catIni);
        qDebug() << QString("starCatList count: %1\n").arg(starCatList.count());

/////////////////////////////
        if(useSysCorr)
        {
            sysCorr = new sysCorrParam;
            if(isSc) sysCorr->init(scFile);
            else  sysCorr->init(syscorIni);
        }

        observatory *obsList = new observatory;
        obsy *obsPos;
        qDebug() << QString("Load observatory cat file: %1\n").arg(observatoryCat);
        if(obsList->init(observatoryCat.toAscii().data(), OBS_SIZE))
        {
            qDebug() << QString("obsCat is not opened\n");
            logFile->close();
            delete clog;
            clog = 0;
            delete logFile;
            if(resRed&&isRemLog) QDir().remove(logFileName);
            qInstallMsgHandler(0);
            return 1;
        }

        qDebug() << QString("observatory initiated\n");

/////////////////////////////

        fitsdata *fitsd = new fitsdata();

////////////////////////////////////////

        if(obsList->getobsynumO(obsCode.toAscii().data()))
        {
            qDebug() << QString("obsCode is not found\n");
            logFile->close();
            delete clog;
            clog = 0;
            delete logFile;
            delete fitsd;
            if(isRemLog) QDir().remove(logFileName);
            qInstallMsgHandler(0);
            return 1;
        }
        obsPos = obsList->record;
        fitsd->initObsPos(obsList->record);


/////////////////////////////



//get header

        if(useHeaderFile)   //0-http; 1-file
        {
            if(fitsd->loadHeaderFile(headerFileName))
            {
                qDebug() << "\nloadHeaderFile error\n";
                logFile->close();
                delete clog;
                clog = 0;
                delete logFile;
                delete fitsd;
                if(isRemLog) QDir().remove(logFileName);
                qInstallMsgHandler(0);
                return 1;
            }
        }
        else
        {

            if(detPlName) detPlateName(&pnStr, fileName, plNameType);

            outerArguments.clear();
            outerProcess.setWorkingDirectory(gethttp_prog_folder);
            outerProcess.setProcessChannelMode(QProcess::MergedChannels);
            outerProcess.setReadChannel(QProcess::StandardOutput);
            outerArguments << pnStr.toAscii().constData();
            qDebug() << gethttp_prog << outerArguments.join(" ") << "\n";
            outerProcess.start(gethttp_prog, outerArguments);

            outerProcess.waitForFinished(gethttp_wait_time);
            if(fitsd->readHeader(QString(outerProcess.readAllStandardOutput())))
            {
                qDebug() << "\nreadHttpHeader error\n";
                logFile->close();
                delete clog;
                clog = 0;
                delete logFile;
                delete fitsd;
                if(isRemLog) QDir().remove(logFileName);
                qInstallMsgHandler(0);
                return 1;
            }
        }

        if(saveHeaderFile)
        {
            qDebug() << QString("saveHeaderFile: %1\n").arg(headerFileName);
            qDebug() << QString("headList.size: %1\n").arg(fitsd->headList.size());
            if(fitsd->saveHeaderFile(headerFileName)) qDebug() << "saveHeaderFile error\n";
        }

/////////////////////////////
// load wcs
        if(fitsd->loadWCSFile(wcsFileName))
        {
            qDebug() << "\nloadWCSFile error\n";
            logFile->close();
            delete clog;
            clog = 0;
            delete logFile;
            delete fitsd;
            if(isRemLog) QDir().remove(logFileName);
            qInstallMsgHandler(0);
            return 1;
        }
/////////////////////////////

        ExposureList *expList = new ExposureList;
        ExposureRec *expRec = new ExposureRec;

        int expRes = initExpList(expList, fitsd->headList, obsList->record);
        expList->getExp(expRec, expNum);
        fitsd->setMJD(expRec->expTime);

//  utcorr

        uTime = QString("0.0");

        if(useUtCorr)
        {
            fitsd->headList.getKeyName("DATE-OBS", &dateStr);
            fitsd->headList.getKeyName("TIME-OBS", &timeStr);

            outerArguments.clear();
            outerProcess.setWorkingDirectory(utcorr_prog_folder);
            outerProcess.setProcessChannelMode(QProcess::MergedChannels);
            outerProcess.setReadChannel(QProcess::StandardOutput);
            qDebug() << QString("argc= %1\n").arg(argc);
            outerArguments  << dateStr << timeStr;
            qDebug() << utcorr_prog << outerArguments.join(" ");
            outerProcess.start(utcorr_prog, outerArguments);

            outerProcess.waitForFinished(utcorr_wait_time);
            QTextStream catStream(outerProcess.readAllStandardOutput());

            uTime = catStream.readAll().section("\n", -1, -1);
        }
        else
        {
            fitsd->headList.getKeyName("U", &uTime);
        }

        qDebug() << QString("uTimeCorr: %1\n").arg(uTime.trimmed().toDouble());
        fitsd->MJD += uTime.trimmed().toDouble()/86400.0;

/////////

        //refractionParam *refParam;
        if(isRedRef)
        {
            qDebug() << "initRefractParam\n";
            refParam = new refractionParam;
            if(initPlateRefParam(refParam, fitsd, obsPos))refParam==NULL;
        }

////////

        qDebug() << "ipixMarks load\n";
        if(fitsd->loadIpixMarks(fileName, mSep, mCol))
        {
            qDebug() << "Ipix marks load error\n";
            logFile->close();
            delete clog;
            clog = 0;
            delete logFile;
            delete fitsd;
            if(isRemLog) QDir().remove(logFileName);
            qInstallMsgHandler(0);
            return 1;
        }

        qDebug() << QString("ipixMarks num: %1\n").arg(fitsd->ipixMarks->marks.size());

        if(detRect) fitsd->detIpixWorkFrame();
        else fitsd->workFrame.setCoords(rectX0, rectY0, rectX1, rectY1);

/////////
        double fov = fovp*fitsd->detFov();

        fitsd->catMarks->clearMarks();
        qDebug() << "catMarks load\n";
        if(getMarksGrid(fitsd->catMarks, starCatList.at(catProgType), catProgType, fitsd->MJD, fitsd->WCSdata[2], fitsd->WCSdata[3], fov, mag0, mag1, -1))
        {
            qDebug() << QString("getMarksGrid error\n");
            logFile->close();
            delete clog;
            clog = 0;
            delete logFile;
            delete fitsd;
            if(isRemLog) QDir().remove(logFileName);
            qInstallMsgHandler(0);
            return 2;
        }

        qDebug() << QString("catMarks num: %1\n").arg(fitsd->catMarks->marks.size());

        fitsd->detTan();

//////////

        if(lspmFind)
        {
            qDebug() << "lspmFind\n";
            if(!(starCatList.size()<3)) findLspmCat(fitsd->objMarks, fitsd->WCSdata[2], fitsd->WCSdata[3], fitsd->MJD, starCatList[2]->exeName, starCatList[2]->exePath, starCatList[2]->catPath, fov, magObj0, magObj1);
        }
        if(skybotFind)
        {
            qDebug() << QString("skybotFindType\n");
            QStringList objNames;

            if(tryMpeph)
            {
                findSkybotNamesList(&objNames, fitsd->WCSdata[2], fitsd->WCSdata[3], fitsd->MJD, skybot_prog, skybot_prog_folder, fov, obsCode, magObj0, magObj1, skybot_wait_time);
                sz = objNames.size();
                for(i=0; i<sz; i++) getMpephName(fitsd->objMarks, fitsd->MJD, objNames.at(i), mpeph_prog, mpeph_prog_folder, magObj0, magObj1, mpeph_wait_time);
            }
            else findSkybot(fitsd->objMarks, fitsd->WCSdata[2], fitsd->WCSdata[3], fitsd->MJD, skybot_prog, skybot_prog_folder, fov, obsCode, magObj0, magObj1, skybot_wait_time);

        }
        else if(tryMpeph)
        {
            qDebug() << "tryMpeph\n";
            if(!fitsd->headList.getKeyName(headObjName, &descS))
            {
                desc2NumName(descS, &oNum, &oName);
                if(mpephType) getMpephNum(fitsd->objMarks, fitsd->MJD, QString("%1").arg(oNum), mpeph_prog, mpeph_prog_folder, magObj0, magObj1, mpeph_wait_time);
                else getMpephName(fitsd->objMarks, fitsd->MJD, oName, mpeph_prog, mpeph_prog_folder, magObj0, magObj1, mpeph_wait_time);
            }
        }
        qDebug() << QString("objMarks num: %1\n").arg(fitsd->objMarks->marks.size());
        fitsd->detTanObj();
///////////

    fitsd->cleanObjects(aper);
    fitsd->findCloserObjects(aper);
    fitsd->findCloserStars(aper);

    fitsd->ruler3(redparamIni, resFolder, refParam, sysCorr);


    qDebug() << QString("\nend ruler3PL\n");

///////////
    logFile->close();
    delete clog;
    clog = 0;
    delete logFile;
    delete fitsd;
    if(isRemLog) QDir().remove(logFileName);
    qInstallMsgHandler(0);

    return 0;
}
