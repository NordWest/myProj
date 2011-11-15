#include <QCoreApplication>
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


int main(int argc, char *argv[])//fitsWCS file.fit [options]
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication app(argc, argv);

    setlocale(LC_NUMERIC, "C");

    QFile* logFile;

    if(argc<2)
    {
        qDebug() << "argc= " << argc << "\n";
        return 1;
    }

    QList <catFinder*> starCatList;
    QString obsCode;
    int sz, i;
    QString dateCode;
    QString nFile;

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

    QString cfgFileName = "./fitsWCS.ini";
    int resdirDef = 0;
    QString optName, optVal, optStr;
    QString resFolder;

        for(i=2; i<argc; i++)
        {
            optStr = QString(argv[i]);
            optName = optStr.section("=", 0, 0);
            optVal = optStr.section("=", 1, 1);
            if(QString::compare(optName, "config", Qt::CaseSensitive)==0)
            {
                cfgFileName = optVal;
            }
            else if(QString::compare(optName, "resFolder", Qt::CaseSensitive)==0)
            {
                resdirDef=1;
                resFolder = optVal;
            }
            else
            {
                qDebug() << QString("Error option %1.\nUseage: config|resFolder\n").arg(optName);
                return 1;
            }

        }
///////// 1. Reading settings

        QSettings *sett = new QSettings(cfgFileName, QSettings::IniFormat);

    //general
        int refindWCS = sett->value("general/refindWCS", 0).toInt();
        double fovp = sett->value("general/fovp", 1.0).toDouble();        //field of view percent, [0.0 - 1.0]
        int log_level = sett->value("general/log_level", 0).toInt();
        int expMin = sett->value("general/expMin", 0).toInt();

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

    //psf
        measureParam mesPar;
        mesPar.model = sett->value("psf/model", 3).toInt();//settings->value("psf/model").toInt();//PSF model: 0 - Lorentz PSF, 1 - Gauss PSF, 2 - Moffat PSF, 3 - CoM
        mesPar.nofit = sett->value("psf/nofit", 10).toInt();
        mesPar.delta = sett->value("psf/delta", 1.2).toDouble();
        mesPar.ringradius = sett->value("psf/ringradius", 10).toInt();
        mesPar.ringwidth = sett->value("psf/ringwidth", 6).toInt();
        mesPar.lb = sett->value("psf/lb", 1).toInt();
        mesPar.sg = sett->value("psf/subgrad", 1).toInt();
        mesPar.apRadius = sett->value("psf/apRadius", 10).toInt();

    //  catalogs    /////
        QString catIni = sett->value("catalogs/catIni", "./catalogs.ini").toString();
        int catProgType = sett->value("catalogs/catProgType", 0).toInt();
        double mag0 = sett->value("catalogs/mag0", 6.0).toDouble();
        double mag1 = sett->value("catalogs/mag1", 16.0).toDouble();

    //observatory
        QString observatoryCat = sett->value("observatory/observatoryCat", "./Obs.txt").toString();
        obsCode = sett->value("observatory/obsCode", "084").toString();
    ///////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QString fileName = QString(argv[1]);

    if(log_level>0)
    {
        QString logFileName = fileName+".wcs.log";
        logFile = new QFile(logFileName);
        if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered)) clog = new QDataStream(logFile);
    }

    //QString wcsFileName = QString("%1.wcs").arg(fileName);
    QString wcsLockFile = QString("%1.lock").arg(fileName);
    QString logFileName = fileName+".wcs.log";

    QFileInfo fi(fileName);
    QString filePath = fi.absolutePath();

    if(QDir().exists(wcsLockFile)) return 1;
    else
    {
        logFile = new QFile(logFileName);
        if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered)) clog = new QDataStream(logFile);
        QFile lockFile(wcsLockFile);
        lockFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
        lockFile.close();
    }

    if(log_level>0)
    {
        qDebug() << QString("fileName: %1\n").arg(fileName);
        qDebug() << QString("filePath: %1\n").arg(filePath);
        qDebug() << QString("logFileName: %1\n").arg(logFileName);
    }

////////////////////////////////////////////////////////////////

    if(log_level>0)
    {
        QStringList allSett;
        allSett << sett->allKeys();
        sz = allSett.size();
        qDebug() << QString("\n\nconf: %1\n").arg(cfgFileName);
        for(i=0; i<sz; i++) qDebug() << QString("%1=%2\n").arg(allSett.at(i)).arg(sett->value(allSett.at(i)).toString());
        qDebug() << "\n\n";
    }

////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

    fitsdata *fitsd = new fitsdata;
    if(fitsd->openFile(fileName))
    {
        if(log_level>0) qDebug() << QString("Fits file %1 is not opened\n").arg(fileName);
        QDir().remove(wcsLockFile);
        return 1;
    }

    if(fitsd->exptime<expMin)
    {
        if(log_level>0) qDebug() << QString("Fits exposure %1 smaller then expMin=%2").arg(fitsd->exptime).arg(expMin);

        qInstallMsgHandler(0);
        QDir().remove(wcsLockFile);
        return 1;
    }

/////////////////////////////

    initCatList(&starCatList, catIni);
    if(log_level>0)qDebug() << QString("starCatList count: %1\n").arg(starCatList.count());

/////////////////////////////

    observatory *obsList = new observatory;
    obsy *obsPos;
    if(obsList->init(observatoryCat.toAscii().data(), OBS_SIZE))
    {
        if(log_level>0) qDebug() << QString("obsCat is not opened\n");
        QDir().remove(wcsLockFile);
        return 1;
    }

/////////////////////////////

    if(resdirDef)
    {
        mjdDateCode(&dateCode, fitsd->MJD);

        QDir rDir(resFolder);

        nFile = rDir.absolutePath()+rDir.separator()+dateCode+".fit";
        if(log_level>0) qDebug() << QString("nfile: %1\n").arg(nFile);
        if(rDir.exists(nFile))
        {
            if(log_level>0) qDebug() << QString("file %1 exists\n").arg(nFile);
            qInstallMsgHandler(0);
            QDir().remove(wcsLockFile);
            return 0;
        }
    }

/////////////////////////////

    if(fitsd->WCSdata[12]&&!refindWCS)
    {
        if(log_level>0) qDebug() << QString("wcs in file %1 is present\n").arg(fileName);
        if(resdirDef) QFile().copy(fileName, nFile);

        qInstallMsgHandler(0);
        QDir().remove(wcsLockFile);
        return 0;
    }


/////////////////////////////

    insSettings instr(insSettFile);
    instr.getNumIns(instrNum);
    fitsd->setInstrSettings(instr);

    if(obsList->getobsynumO(obsCode.toAscii().data()))
    {
        if(log_level>0) qDebug() << QString("obsCode is not found\n");
        QDir().remove(wcsLockFile);
        return 1;
    }
    obsPos = obsList->record;
    fitsd->initObsPos(obsList->record);

/////////////////////////////

    double fov = fovp*fitsd->detFov();

    if(log_level>0) qDebug() << "\n\nfind WCS\n\n";
    fitsd->catMarks->clearMarks();
    if(getMarksGrid(fitsd->catMarks, starCatList.at(catProgType), catProgType, fitsd->MJD, fitsd->WCSdata[2], fitsd->WCSdata[3], fov, mag0, mag1, -1))
    {
        if(log_level>0) qDebug() << QString("getMarksGrid error\n");
        QDir().remove(wcsLockFile);
        return 2;
    }
    fitsd->detTan();

    fitsd->findHstars(mesPar.apRadius*2, identNum);
    fitsd->moveMassCenter(fitsd->ipixMarks, mesPar.apRadius*2);
    if(log_level>0) qDebug() << QString("premeasured stars num: %1\n").arg(fitsd->ipixMarks->marks.size());
    fitsd->measureMarksGrid(fitsd->ipixMarks, mesPar);

    int resAuto = identAuto(fitsd->refMarks, fitsd->catMarks, fitsd->ipixMarks, &fitsd->WCSdata[0], identNum, identType, maxNum);

    if(log_level>0) qDebug() << QString("resAuto: %1\n").arg(resAuto);

    if(fitsd->detWCS1(wcsParams))
    {
        if(log_level>0) qDebug() << QString("detWCS1 error\n");
        QDir().remove(wcsLockFile);
        return 1;
    }
    fitsd->ipixMarks->clearMarks();

/////////////////////////////
    if(resdirDef)
    {
        fitsd->saveFitsAs(nFile);
    }
    else fitsd->saveFits();

    QDir().remove(wcsLockFile);

    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);

    return 0;
}
