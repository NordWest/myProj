#include <QCoreApplication>
#include <QtCore>
#include "./../libs/fitsdata.h"
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



int main(int argc, char *argv[])//ruler3.exe file.fits [conf.ini]
    {  
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication app(argc, argv);

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

    //command line  ///////////////////////////////////////
        QString optName, optVal, optStr, pnStr, headerFileName;
        QString resFolder;
        sysCorrParam *sysCorr = NULL;
        QString cfgFileName = "ruler3.ini";
        int sz, i, oNum;
        int resRed = 0;
        QString descS, oName;
        QList <catFinder*> starCatList;
        QString obsCode;
        int resdirDef=0;
        QString scFile;
        int isSc = 0;

        for(i=1; i<argc; i++)
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
            else if(QString::compare(optName, "syscorr", Qt::CaseSensitive)==0)
            {
                isSc = 1;
                scFile = optVal;

            }
        }





///////// 1. Reading settings ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //stream << "started....\n";

    //BEGIN settings
    QSettings *sett = new QSettings(cfgFileName, QSettings::IniFormat);




    QString redparamIni = sett->value("general/redparamIni", "./conf/redParam.ini").toString();
    int aper = sett->value("general/aperture", 20).toInt();
    //int isRedRef = sett->value("general/isRedRef", 0).toInt();
    if(!resdirDef) resFolder = sett->value("general/resFolder", "./results").toString();
    double fovp = sett->value("general/fovp", 1.0).toDouble();        //field of view percent, [0.0 - 1.0]

    int reFindWCS = sett->value("general/reFindWCS", 0).toInt();
    int saveRefindWCS = sett->value("general/saveRefindWCS", 0).toInt();
    int headType = sett->value("general/headType", -1).toInt();

//  logs
        int useLogLock = sett->value("logs/useLogLock", 0).toInt();
        int isRemLog = sett->value("logs/isRemLog", 0).toInt();
        //QString logFolder = sett->value("logs/logFolder", "./logs").toString();
 //       syscorr
        QString syscorIni = sett->value("syscorr/syscorIni", "./conf/syscorr.ini").toString();
        int useSysCorr = sett->value("syscorr/useSysCorr", 0).toInt();

        //insSettings
            QString insSettFile = sett->value("insSettings/insSettFile", "./conf/telescopes.ini").toString();
            int instrNum = sett->value("insSettings/instrNum", 0).toInt();


        //identify  ///////
            int identType = sett->value("identify/identType", 0).toInt();
            int identNum = sett->value("identify/identNum", 6).toInt();
            int maxNum = sett->value("identify/maxNum", 100).toInt();

        reductionParams wcsParams;

        wcsParams.redType = sett->value("wcs/redType", 0).toInt();
        wcsParams.weights = sett->value("wcs/weights", 0).toDouble();
        wcsParams.minRefStars = sett->value("wcs/minRefStars", 4).toDouble();
        wcsParams.sMax = sett->value("wcs/sMax", 500).toDouble();
        wcsParams.uweMax = sett->value("wcs/uweMax", 500).toDouble();
        wcsParams.sigma = sett->value("wcs/sigma", 3).toDouble();
        wcsParams.maxres = sett->value("wcs/maxres", 300).toDouble();
        wcsParams.maxresMAG = sett->value("wcs/maxresMAG", 30).toDouble();
        wcsParams.maxRefStars = sett->value("wcs/maxRefStars", -1).toInt();

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

        //objects   ////
                int lspmFind = sett->value("objects/lspmFind", 0).toInt();
                int skybotFind = sett->value("objects/skybotFind", 0).toInt();
                int tryMpeph = sett->value("objects/tryMpeph", 0).toInt();
                int mpephType = sett->value("objects/mpephType", 0).toInt();
                double magObj0 = sett->value("objects/mag0", 6.0).toDouble();
                double magObj1 = sett->value("objects/mag1", 15.0).toDouble();
                QString headObjName = sett->value("objects/headObjName", "TARGET").toString();

        //  catalogs    /////
                QString catIni = sett->value("catalogs/catIni", "./conf/catalogs.ini").toString();
                int catProgType = sett->value("catalogs/catProgType", 0).toInt();
                double mag0 = sett->value("catalogs/mag0", 6.0).toDouble();
                double mag1 = sett->value("catalogs/mag1", 16.0).toDouble();

        //process
                QString mpeph_prog = sett->value("processes/mpeph_prog", "./mpeph.exe").toString();
                QString mpeph_prog_folder = sett->value("processes/mpeph_prog_folder", "./").toString();
                int mpeph_wait_time = sett->value("processes/mpeph_wait_time", -1).toInt();
                if(mpeph_wait_time>0) mpeph_wait_time *= 1000;
                QString skybot_prog = sett->value("processes/skybot_prog", "./skybotclient.exe").toString();
                QString skybot_prog_folder = sett->value("processes/skybot_prog_folder", "./").toString();
                int skybot_wait_time = sett->value("processes/skybot_wait_time", -1).toInt();
                if(skybot_wait_time>0) skybot_wait_time *= 1000;


        //observatory
                QString observatoryCat = sett->value("observatory/observatoryCat", "./Obs.txt").toString();
                obsCode = sett->value("observatory/obsCode", "084").toString();



    ///////////////////////////////////////////
    /////

            QString fileName =  QString(argv[1]);

            QDir resDir("./");
            if(resDir.mkpath(resFolder))qDebug() << "\nresFolder created\n";
            else qDebug() << "\nresFolder don't create\n";
            //if(resDir.mkpath(logFolder))qDebug() << "\nlogFolder created\n";
            //else qDebug() << "\nlogFolder don't create\n";

            QFileInfo fi(fileName);
            QString filePath = fi.absoluteFilePath();//fileName.left(fileName.lastIndexOf("\\")+1);
            if(filePath=="") filePath = QString("./");
            QString logFileName = QString("%1.log").arg(fi.absoluteFilePath());

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
            qDebug() << QString("logFileName: %1\n").arg(logFileName);

            QStringList allSett;
            allSett << sett->allKeys();
            sz = allSett.size();
            qDebug() << QString("\n\nconf.ini: %1\n").arg(cfgFileName);
            qDebug() << QString("keys num: %1\n").arg(sz);
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
            if(isRemLog)QDir().remove(logFileName);
            return 1;
        }

        qDebug() << QString("observatory initiated\n");

/////////////////////////////

    fitsdata *fitsd = new fitsdata;
    if(fitsd->openFile(fileName, headType))
    {
        qDebug() << QString("File %1 is not opened\n").arg(fileName);
        return 1;
    }

/////////////////////////////
    //fitsd->initInst(telescopeFile, insNum);
    insSettings instr(insSettFile);
    instr.getNumIns(instrNum);
    fitsd->setInstrSettings(instr);
/////////////////////////////

    if(obsList->getobsynumO(obsCode.toAscii().data()))
    {
        qDebug() << QString("obsCode is not found\n");
        if(isRemLog)QDir().remove(logFileName);
        return 1;
    }
    obsPos = obsList->record;
    fitsd->initObsPos(obsList->record);


/////////////////////////////
    /*
    /////////

            //refractionParam *refParam;
            if(isRedRef)
            {
                qDebug() << "initRefractParam\n";
                refParam = new refractionParam;
                if(initPlateRefParam(refParam, fitsd, obsPos))refParam==NULL;
            }

    ////////
    */



    double fov = fovp*fitsd->detFov();

    if(reFindWCS)
    {
        qDebug() << "\n\nreFind WCS\n\n";
        fitsd->catMarks->clearMarks();
        //getMarksGrid(starCatList.at(currentCat), fitsd->instr->fov, mag0, mag1, starsNum);
        if(getMarksGrid(fitsd->catMarks, starCatList.at(catProgType), catProgType, fitsd->MJD, fitsd->WCSdata[2], fitsd->WCSdata[3], fov, mag0, mag1, -1))
        {
            qDebug() << QString("getMarksGrid error\n");
            if(isRemLog) QDir().remove(logFileName);
            return 2;
        }
        fitsd->detTan();

        //fitsd->detTan();
        //fitsd->WCSdata[12] = 0;
        //fitsd->copyImgGrid(fitsd->catMarks, fitsd->ipixMarks);
        //fitsd->ipixMarks->copy(fitsd->catMarks);

        fitsd->ipixMarks->clearMarks();

        fitsd->findHstars(aper, identNum);
        //fitsd->copyImgGrid(fitsd->catMarks, fitsd->ipixMarks);
        fitsd->moveMassCenter(fitsd->ipixMarks, mesPar.apRadius*2);
        qDebug() << QString("premeasured stars num: %1\n").arg(fitsd->ipixMarks->marks.size());
        fitsd->measureMarksGrid(fitsd->ipixMarks, mesPar);
        //fitsd->detTan();

        int resAuto = identAuto(fitsd->refMarks, fitsd->catMarks, fitsd->ipixMarks, &fitsd->WCSdata[0], identNum, identType, maxNum);

        qDebug() << QString("resAuto: %1\n").arg(resAuto);

        if(fitsd->detWCS1(wcsParams)&&saveRefindWCS)fitsd->saveWCS();
        //fitsd->detTan();
        fitsd->ipixMarks->clearMarks();
    }
    //
    fitsd->catMarks->clearMarks();
    if(getMarksGrid(fitsd->catMarks, starCatList.at(catProgType), catProgType, fitsd->MJD, fitsd->WCSdata[2], fitsd->WCSdata[3], fov, mag0, mag1, -1))
    {
        qDebug() << QString("getMarksGrid error\n");
        if(isRemLog) QDir().remove(logFileName);
        return 2;
    }
    fitsd->detTan();

    fitsd->ipixMarks->clearMarks();
    copyImgGrid(fitsd->catMarks, fitsd->ipixMarks);
    fitsd->moveMassCenter(fitsd->ipixMarks, mesPar.apRadius*2);
    fitsd->measureMarksGrid(fitsd->ipixMarks, mesPar);
//
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
                if(!fitsd->headList.getKeyName(headObjName, &descS))
                {
                    desc2NumName(descS, &oNum, &oName);
                    if(mpephType) getMpephNum(fitsd->objMarks, fitsd->MJD, QString("%1").arg(oNum), mpeph_prog, mpeph_prog_folder, magObj0, magObj1, mpeph_wait_time);
                    else getMpephName(fitsd->objMarks, fitsd->MJD, oName, mpeph_prog, mpeph_prog_folder, magObj0, magObj1, mpeph_wait_time);
                }
            }
            fitsd->detTanObj();
    ///////////
            fitsd->cleanObjects(aper);
            fitsd->findCloserObjects(aper);
            fitsd->findCloserStars(aper);
//reduction

    //int resRed = fitsd->makeReduction(ruler3File, resFolder);
    resRed = fitsd->ruler3(redparamIni, resFolder, NULL, sysCorr);


    qDebug() << QString("\n\nresRed: %1\n").arg(resRed);

/////////////////////////
    logFile->close();
    delete clog;
    clog = 0;

    delete logFile;
    //logFile = 0;
    delete fitsd;
    if(resRed&&isRemLog) QDir().remove(logFileName);

    qInstallMsgHandler(0);


    return 0;

}
