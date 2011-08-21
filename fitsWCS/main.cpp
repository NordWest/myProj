#include <QCoreApplication>
#include <QtCore>
#include "./../libs/fitsdata.h"
#include "./../libs/astro.h"
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


int main(int argc, char *argv[])//fitsWCS file.fit [conf.ini]
{
    qInstallMsgHandler(customMessageHandler);
    QString msgstr;
    QCoreApplication app(argc, argv);
    QTextStream stream(stdout);

    QTextCodec *codec1 = QTextCodec::codecForName("Windows-1251");
    Q_ASSERT( codec1 );
///////// 1. Reading settings ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QString fileName = QString(argv[1]);
    QString logFileName = fileName+".wcs.log";
    QFile* logFile = new QFile(logFileName);
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered)) clog = new QDataStream(logFile);

    //QString wcsFileName = QString("%1.wcs").arg(fileName);
    QString wcsLockFile = QString("%1.lock").arg(fileName);

    QString filePath = fileName.left(fileName.lastIndexOf("/")+1);
    if(filePath=="") filePath = QString("./");
    qDebug() << QString("fileName: %1\n").arg(fileName);
    qDebug() << QString("filePath: %1\n").arg(filePath);
    qDebug() << QString("logFileName: %1\n").arg(logFileName);

    QString cfgFile;
    if(argc>2) cfgFile = codec1->toUnicode(argv[2]);
    else cfgFile = QString("./conf/conf.ini");

    //if(QFile::exists(logFileName)) return 1;
    //QFile* logFile = new QFile(logFileName);



    fitsdata *fitsd = new fitsdata;
    if(fitsd->openFile(fileName)) return 1;




    int currentCat, isMove2corner;
    double maxObjDisp;
    catFinder *starCat;
    QList <catFinder*> starCatList;
    QString obsCode;
    int sz, i;
    //BEGIN settings
    QSettings *sett = new QSettings(cfgFile, QSettings::IniFormat);

    QStringList allSett;
    allSett << sett->allKeys();
    sz = allSett.size();
    qDebug() << QString("\n\nconf.ini:\n");
    for(i=0; i<sz; i++) qDebug() << QString("%1=%2\n").arg(allSett.at(i)).arg(sett->value(allSett.at(i)).toString());
    qDebug() << "\n\n";
    QString ruler3File = sett->value("general/ruler3File", "./conf/ruler3.ini").toString();
    QString telescopeFile = sett->value("telescope/telescopeFile", "./conf/telescopes.ini").toString();
    int insNum = sett->value("telescope/instrNum", 2).toInt();
    //insSettings *instruments = new insSettings("./conf/telescopes.ini");
    //instruments->getNumIns(instruments->curInst);

    int mpeWaitTime = sett->value("General/mpeWaitTime").toInt();
    int sbWaitTime = sett->value("General/sbWaitTime").toInt();

    int expMin = sett->value("General/expMin", 0).toInt();

    QString resFolder = sett->value("general/resFolder", "./resDefault").toString();
    QDir resDir("./");
    if(resDir.mkpath(resFolder))qDebug() << "\nresFolder created\n";
    else qDebug() << "\nresFolder don't create\n";
    resDir.setPath(resFolder);

    int reFindWCS = sett->value("general/reFindWCS", 0).toInt();
    int saveRefindWCS = sett->value("general/saveRefindWCS", 0).toInt();

    int identType = sett->value("identify/identType", 0).toInt();
    int targNum = sett->value("identify/targNum", 5).toInt();

    reductionParams wcsParams;

    wcsParams.maxres = sett->value("wcs/maxres", 300).toDouble();
    wcsParams.maxresMAG = sett->value("wcs/maxresMAG", 10).toDouble();
    //wcsParams.minRefMag = sett->value("wcs/minRefMag", 8).toDouble();
    //wcsParams.maxRefMag = sett->value("wcs/maxRefMag", 15).toDouble();
    wcsParams.redType = sett->value("wcs/redType", 0).toDouble();
    wcsParams.sMax = sett->value("wcs/sMax", 500).toDouble();
    wcsParams.uweMax = sett->value("wcs/uweMax", 500).toDouble();
    wcsParams.weights = sett->value("wcs/weights", 0).toDouble();
    wcsParams.minRefStars = sett->value("wcs/minRefStars", 4).toInt();
    wcsParams.sigma = sett->value("wcs/sigmaN", 3).toDouble();

    int starsNum = sett->value("wcs/starsNum", 100).toInt();


    int lspmFind = sett->value("objectsFind/lspmFind").toInt();
    int skybotFind = sett->value("objectsFind/skybotFind").toInt();
    int tryMpeph = sett->value("objectsFind/tryMpeph").toInt();
    int mpephType = sett->value("objectsFind/mpephType", 0).toInt();
    double magObj0 = sett->value("objectsFind/mag0", 6.0).toDouble();
    double magObj1 = sett->value("objectsFind/mag1", 15.0).toDouble();


    starCat = new catFinder;
    starCat->exeName = sett->value("ucac2/exeName").toString();
    starCat->exePath = sett->value("ucac2/exePath").toString();
    starCat->catType = sett->value("ucac2/catType").toInt();
    starCat->catName = sett->value("ucac2/catName").toString();
    starCat->catPath = sett->value("ucac2/catPath").toString();
    starCatList << starCat;
    //
    starCat = new catFinder;
    starCat->exeName = sett->value("usnob/exeName").toString();
    starCat->exePath = sett->value("usnob/exePath").toString();
    starCat->catType = sett->value("usnob/catType").toInt();
    starCat->catName = sett->value("usnob/catName").toString();
    starCat->catPath = sett->value("usnob/catPath").toString();
    starCatList << starCat;
    //
    starCat = new catFinder;
    starCat->exeName = sett->value("ucac3/exeName").toString();
    starCat->exePath = sett->value("ucac3/exePath").toString();
    starCat->catType = sett->value("ucac3/catType").toInt();
    starCat->catName = sett->value("ucac3/catName").toString();
    starCat->catPath = sett->value("ucac3/catPath").toString();
    starCatList << starCat;
    starCat = new catFinder;
    starCat->exeName = sett->value("lspm/exeName").toString();
    starCat->exePath = sett->value("lspm/exePath").toString();
    starCat->catType = sett->value("lspm/catType").toInt();
    starCat->catName = sett->value("lspm/catName").toString();
    starCat->catPath = sett->value("lspm/catPath").toString();
    starCatList << starCat;
    //
    starCat = new catFinder;
    starCat->exeName = sett->value("lspmFind/exeName").toString();
    starCat->exePath = sett->value("lspmFind/exePath").toString();
    starCat->catType = sett->value("lspmFind/catType").toInt();
    starCat->catName = sett->value("lspmFind/catName").toString();
    starCat->catPath = sett->value("lspmFind/catPath").toString();
    starCatList << starCat;
    //
    //qDebug() << QString("starCatList count: %1\n").arg(starCatList.count());
    QString ast_eph_prog = sett->value("processes/ast_eph_prog", "./mpeph.exe").toString();
    QString ast_eph_prog_folder = sett->value("processes/ast_eph_prog_folder", "./").toString();
    QString skybot_prog = sett->value("processes/skybot_prog", "./skybotclient.exe").toString();
    QString skybot_prog_folder = sett->value("processes/skybot_prog_folder", "./").toString();

    currentCat = sett->value("catalogs/currentCatalog", 0).toInt();
    QString observatoryCat = sett->value("catalogs/observatoryCat", "./../../../data/cats/Obs.txt").toString();
    double mag0 = sett->value("catalogs/mag0", 6.0).toDouble();
    double mag1 = sett->value("catalogs/mag1", 15.0).toDouble();
    isMove2corner = sett->value("marks/isMove2corner", 0).toInt();

//celestial
    //obsCode->clear();
    obsCode = sett->value("celestial/obsName", "084").toString();
    maxObjDisp = sett->value("celestial/maxObjDisp", 2).toDouble();

    observatory *obsList = new observatory;
    obsList->init(observatoryCat.toAscii().data(), OBS_SIZE);

    if(fitsd->exptime<expMin)
    {
        qDebug() << QString("Fits exposure %1 smaller then expMin=%2").arg(fitsd->exptime).arg(expMin);

        qInstallMsgHandler(0);
        return 0;
    }

    QString dateCode;
    QString nFile;
    /*DATEOBS dObs;


    dObs = getDATEOBSfromMJD(fitsd->MJD);
    dateCode = QString("%1").arg((int)dObs.year, 4, 10, QLatin1Char( '0' ));
    dateCode.append(QString("%1").arg((int)dObs.month, 2, 10, QLatin1Char( '0' )));
    dateCode.append(QString("%1").arg((int)dObs.day, 2, 10, QLatin1Char( '0' )));
    dateCode.append(QString("%1").arg((int)dObs.hour, 2, 10, QLatin1Char( '0' )));
    dateCode.append(QString("%1").arg((int)dObs.min, 2, 10, QLatin1Char( '0' )));
    dateCode.append(QString("%1").arg((int)dObs.sec, 2, 10, QLatin1Char( '0' )));
    dateCode.append(QString("%1").arg((int)((int)(dObs.sec*10) - ((int)dObs.sec)*10), 1, 10, QLatin1Char( '0' )));*/

    mjdDateCode(&dateCode, fitsd->MJD);

    nFile = QString("%1%2.fit").arg(resFolder).arg(dateCode);
    if(QDir().exists(nFile))
    {

        qDebug() << QString("file %1 exists\n").arg(nFile);

        qInstallMsgHandler(0);
        return 0;
    }

    if(fitsd->WCSdata[12]&&!reFindWCS)
    {
        qDebug() << QString("wcs in file %1 obtained\n").arg(fileName);
        QFile().copy(fileName, nFile);

        qInstallMsgHandler(0);
        return 0;
    }

    QDir wDir(filePath);
    if(wDir.exists(wcsLockFile))
    {
        qDebug() << QString("file %1 locked\n").arg(fileName);

        qInstallMsgHandler(0);
        return 0;
    }

    QFile lockFile(wcsLockFile);
    lockFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    lockFile.close();



    fitsd->initInst(telescopeFile, insNum);

    //fitsd->findHstars();
    qDebug() << "\ninit ended\n";

    fitsd->marksG->clearMarks();
    fitsd->getMarksGrid(starCatList.at(currentCat), fitsd->instr->fov, mag0, mag1, starsNum);

    //fitsd->detTan();
    //fitsd->WCSdata[12] = 0;
    //fitsd->copyImgGrid(fitsd->marksG, fitsd->marksGIpix);
    //fitsd->marksGIpix->copy(fitsd->marksG);

    QSettings *settM;
    measureParam mesPar;
    settM = new QSettings(ruler3File, QSettings::IniFormat);
    mesPar.model = settM->value("psf/model", 3).toInt();//settings->value("psf/model").toInt();//PSF model: 0 - Lorentz PSF, 1 - Gauss PSF, 2 - Moffat PSF, 3 - CoM
    mesPar.nofit = settM->value("psf/nofit", 10).toInt();
    mesPar.delta = settM->value("psf/delta", 1.2).toDouble();
    mesPar.ringradius = settM->value("psf/ringradius", 10).toInt();
    mesPar.ringwidth = settM->value("psf/ringwidth", 6).toInt();
    mesPar.lb = settM->value("psf/lb", 1).toInt();
    mesPar.sg = settM->value("psf/subgrad", 1).toInt();
    mesPar.aperture = settM->value("psf/aperture", 10).toInt();


    fitsd->marksGIpix->clearMarks();
    fitsd->findHstars(mesPar.aperture, targNum);
    //fitsd->copyImgGrid(fitsd->marksG, fitsd->marksGIpix);
    fitsd->moveMassCenter(fitsd->marksGIpix, mesPar.aperture);
    fitsd->measureMarksGrid(fitsd->marksGIpix, mesPar);
    //fitsd->detTan();

    int resAuto = fitsd->identAuto(fitsd->refMarks, fitsd->instr->rang, targNum, identType);

    qDebug() << QString("resAuto: %1\n").arg(resAuto);

    if(fitsd->detWCS1(wcsParams))fitsd->saveFitsAs(nFile);
    //fitsd->detTan();
    //fitsd->marksGIpix->clearMarks();


    QDir(filePath).remove(wcsLockFile);






    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);

    //if(!resRed) QDir(filePath).remove(logFileName);

    return 0;
}
