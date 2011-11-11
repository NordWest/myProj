#include <QtCore>

#include "./../libs/fitsdata.h"
#include "./../astro/astro.h"
#include "./../libs/comfunc.h"
#include "./../libs/observatory.h"

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



int main(int argc, char *argv[])// plateWCS conf.ini marks.txt [pnType] [plateNum|headerFile]
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);

    setlocale(LC_NUMERIC, "C");

    QFile* logFile;

    if(argc<3)
    {
        qDebug() << "argc= " << argc << "\n";
        return 1;
    }

    QList <catFinder*> starCatList;
    QString obsCode;
    int sz, i;
    ExposureRec* expRec;
    ExposureList* expList;
    QProcess outerProcess;
    QStringList outerArguments;
    QString uTime, dateStr, timeStr;

    QString codecName;

    #if defined(Q_OS_LINUX)
        codecName = "UTF-8";
    #elif defined(Q_OS_WIN)
        codecName = "CP1251";
    #endif

    QTextCodec *codec1 = QTextCodec::codecForName(codecName.toAscii().constData());
    Q_ASSERT( codec1 );
    QTextCodec::setCodecForCStrings(codec1);


    //command line  ///////////////////////////////////////

    QString cfgFileName = "./plateWCS.ini";
    int doWhat = 0;                         //0-http; 1-file
    int detPlName = 1;
    QString optName, optVal, optStr, pnStr, headerFileName;
    QString resFolder;

        for(i=1; i<argc; i++)
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
                doWhat = 1;
                headerFileName = optVal;
            }
        }


    QSettings *sett = new QSettings(cfgFileName, QSettings::IniFormat);

//general
    int expNum = sett->value("general/expNum", -1).toInt();
    int plNameType = sett->value("general/plNameType", 0).toInt();
    int useUtCorr = sett->value("general/useUtCorr", 0).toInt();
    int refindWCS = sett->value("general/refindWCS", 0).toInt();
    double fovp = sett->value("general/fovp", 1.0).toDouble();        //field of view percent, [0.0 - 1.0]


//insSettings
    QString insSettFile = sett->value("insSettings/insSettFile", "./telescopes.ini").toString();
    int instrNum = sett->value("insSettings/instrNum", 0).toInt();

//identify  ///////
    int identType = sett->value("identify/identType", 0).toInt();
    int identNum = sett->value("identify/identNum", 6).toInt();
    int maxNum = sett->value("identify/maxNum", 100).toInt();

    reductionParams wcsParams;

    wcsParams.redType = sett->value("reduction/redType", 0).toInt();
    wcsParams.weights = sett->value("reduction/weights", 0).toDouble();
    wcsParams.minRefStars = sett->value("reduction/minRefStars", 4).toDouble();
    wcsParams.sMax = sett->value("reduction/sMax", 500).toDouble();
    wcsParams.uweMax = sett->value("reduction/uweMax", 500).toDouble();
    wcsParams.sigma = sett->value("reduction/sigma", 3).toDouble();
    wcsParams.maxres = sett->value("reduction/maxres", 300).toDouble();
    wcsParams.maxresMAG = sett->value("reduction/maxresMAG", 30).toDouble();
    wcsParams.maxRefStars = sett->value("reduction/maxRefStars", -1).toInt();

//  catalogs    /////
    QString catIni = sett->value("catalogs/catIni", "./catalogs.ini").toString();
    int catProgType = sett->value("catalogs/catProgType", 0).toInt();
    double mag0 = sett->value("catalogs/mag0", 6.0).toDouble();
    double mag1 = sett->value("catalogs/mag1", 16.0).toDouble();

//process

    QString gethttp_prog = sett->value("processes/gethttp_prog", "./getHttpHeader").toString();
    QString gethttp_prog_folder = sett->value("processes/gethttp_prog_folder", "./").toString();
    int gethttp_wait_time = sett->value("processes/gethttp_wait_time", -1).toInt();
    QString utcorr_prog = sett->value("processes/utcorr_prog", "./uTimeCorr").toString();
    QString utcorr_prog_folder = sett->value("processes/utcorr_prog_folder", "./").toString();
    int utcorr_wait_time = sett->value("processes/utcorr_wait_time", -1).toInt();



//marks /////////////////
    QString mSep = sett->value("marks/mSep", " ").toString();
    QString mCol = sett->value("marks/mCol", "1,2,3,4,5").toString();
    int detRect = sett->value("marks/detRect", 0).toInt();
    double rectX0 = sett->value("marks/rectX0", 0.0).toDouble();
    double rectY0 = sett->value("marks/rectY0", 0.0).toDouble();
    double rectX1 = sett->value("marks/rectX1", 0.0).toDouble();
    double rectY1 = sett->value("marks/rectY1", 0.0).toDouble();

//observatory
    QString observatoryCat = sett->value("observatory/observatoryCat", "./Obs.txt").toString();
    obsCode = sett->value("observatory/obsCode", "084").toString();
///////////////////////////////////////////////////////////

    QString fileName =  QString(argv[1]);
    QString logFileName = fileName+".wcs.log";
    QString wcsFileName = QString("%1.wcs").arg(fileName);
    QString wcsLockFile = QString("%1.lock").arg(fileName);

    QFileInfo fi(fileName);
    QString filePath = fi.absolutePath();

    if(QDir().exists(wcsLockFile)||(QDir().exists(wcsFileName)&&!refindWCS)) return 1;
    else
    {
        logFile = new QFile(logFileName);
        if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered)) clog = new QDataStream(logFile);
        QFile lockFile(wcsLockFile);
        lockFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
        lockFile.close();
    }

    QStringList allSett;
    allSett << sett->allKeys();
    sz = allSett.size();
    qDebug() << QString("sett size: %1\n").arg(sz);
    qDebug() << QString("\n\nconf.ini:\n");
    for(i=0; i<sz; i++) qDebug() << QString("%1=%2\n").arg(allSett.at(i)).arg(sett->value(allSett.at(i)).toString());
    qDebug() << "\n\n";

///////////////////////////////////////////////////////////

    qDebug() << QString("fileName: %1\n").arg(fileName);
    qDebug() << QString("platePath: %1\n").arg(filePath);

/////////////////////////////

    initCatList(&starCatList, catIni);
    qDebug() << QString("starCatList count: %1\n").arg(starCatList.count());

/////////////////////////////

    observatory *obsList = new observatory;
    obsy *obsPos;
    if(obsList->init(observatoryCat.toAscii().data(), OBS_SIZE))
    {
        qDebug() << QString("obsCat is not opened\n");
        QDir().remove(wcsLockFile);
        return 1;
    }

    qDebug() << QString("observatory initiated\n");

/////////////////////////////

/////////////////////////////

    fitsdata *fitsd = new fitsdata();

    insSettings instr(insSettFile);
    instr.getNumIns(instrNum);
    fitsd->setInstrSettings(instr);

    if(obsList->getobsynumO(obsCode.toAscii().data()))
    {
        qDebug() << QString("obsCode is not found\n");
        QDir().remove(wcsLockFile);
        return 1;
    }
    obsPos = obsList->record;
    fitsd->initObsPos(obsList->record);


/////////////////////////////



//get header

    if(doWhat)   //0-http; 1-file
    {
        if(fitsd->loadHeaderFile(headerFileName))
        {
            qDebug() << "\nloadHeaderFile error\n";
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
        if(fitsd->readHttpHeader(QString(outerProcess.readAllStandardOutput())))
        {
            qDebug() << "\nreadHttpHeader error\n";
            return 1;
        }
    }

/////////////////////////////

/////////////////////////////

        expList = new ExposureList;
        expRec = new ExposureRec;

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
////////

        if(fitsd->loadIpixMarks(fileName, mSep, mCol))
        {
            qDebug() << "Ipix marks load error\n";
            return 1;
        }

        if(detRect) fitsd->detIpixWorkFrame();
        else fitsd->workFrame.setCoords(rectX0, rectY0, rectX1, rectY1);
        fitsd->setRpix();

/////////

        double fov = fovp*fitsd->detFov();

        fitsd->catMarks->clearMarks();
        //getMarksGrid(fitsd->catMarks, starCatList.at(catNum), fitsd->MJD, fitsd->WCSdata[2], fitsd->WCSdata[3], fov, mag0, mag1, -1);
        if(getMarksGrid(fitsd->catMarks, starCatList.at(catProgType), catProgType, fitsd->MJD, fitsd->WCSdata[2], fitsd->WCSdata[3], fov, mag0, mag1, -1))
        {
            qDebug() << QString("getMarksGrid error\n");
            QDir().remove(wcsLockFile);
            return 2;
        }
        fitsd->detTan();

//////////


//ident
    qDebug() << QString("refMarksSize= %1\n").arg(fitsd->refMarks->marks.size());
    int resAuto = identAuto(fitsd->refMarks, fitsd->catMarks, fitsd->ipixMarks, &fitsd->WCSdata[0], identNum, identType, maxNum);

    if(resAuto)
    {
        qDebug() << "\nautoident fail\n";
        QDir().remove(wcsLockFile);
        return 3;
    }

    if(fitsd->detWCS1(wcsParams))
    {
        fitsd->detTan();
        fitsd->saveWCSFile(wcsFileName);
    }
    else
    {
        qDebug() << QString("detWCS fail\nSx = %1 mas\tSy = %2 mas").arg(grad_to_mas(fitsd->Sx)).arg(grad_to_mas(fitsd->Sy));
    }

    QDir().remove(wcsLockFile);

    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);

    return 0;
}
