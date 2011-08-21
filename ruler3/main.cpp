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



int main(int argc, char *argv[])//ruler3.exe file.fits [conf.ini]
    {  
    qInstallMsgHandler(customMessageHandler);
    QString msgstr;
    QCoreApplication app(argc, argv);
    QTextStream stream(stdout);

    setlocale(LC_NUMERIC, "C");

    QTextCodec *codec1 = QTextCodec::codecForName("Windows-1251");
    Q_ASSERT( codec1 );
///////// 1. Reading settings ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QString fileName = QString(argv[1]);
    QString logFileName = QString("%1.log").arg(fileName);
    QString filePath = fileName.left(fileName.lastIndexOf("/")+1);
    if(filePath=="") filePath = QString("./");
    qDebug() << QString("fileName: %1\n").arg(fileName);
    qDebug() << QString("filePath: %1\n").arg(filePath);
    qDebug() << QString("logFileName: %1\n").arg(logFileName);

    QString cfgFile;
    if(argc>2) cfgFile = codec1->toUnicode(argv[2]);
    else cfgFile = QString("./conf/conf.ini");



    if(QFile::exists(logFileName)) return 1;
    QFile* logFile = new QFile(logFileName);
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);

    //stream << "started....\n";
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
    qDebug() << QString("\n\nconf.ini: %1\n").arg(cfgFile);
    for(i=0; i<sz; i++) qDebug() << QString("%1=%2\n").arg(allSett.at(i)).arg(sett->value(allSett.at(i)).toString());
    qDebug() << "\n\n";
    QString ruler3File = sett->value("general/ruler3File", "./conf/ruler3.ini").toString();
    QString telescopeFile = sett->value("telescope/telescopeFile", "./conf/telescopes.ini").toString();
    int insNum = sett->value("telescope/instrNum", 2).toInt();
    //insSettings *instruments = new insSettings("./conf/telescopes.ini");
    //instruments->getNumIns(instruments->curInst);

    int mpeWaitTime = sett->value("General/mpeWaitTime").toInt();
    int sbWaitTime = sett->value("General/sbWaitTime").toInt();

    QString resFolder = sett->value("general/resFolder", "./resDefault").toString();
    QDir resDir("./");
    if(resDir.mkpath(resFolder))qDebug() << "\nresFolder created\n";
    else qDebug() << "\nresFolder don't create\n";

    int reFindWCS = sett->value("general/reFindWCS", 0).toInt();
    int saveRefindWCS = sett->value("general/saveRefindWCS", 0).toInt();
    int headType = sett->value("general/headType", 0).toInt();



    qDebug() << QString("\n\n1\n");

    reductionParams wcsParams;


    wcsParams.maxres = sett->value("wcs/maxres", 300).toDouble();
    wcsParams.maxresMAG = sett->value("wcs/maxresMAG", 10).toDouble();
    //wcsParams.minRefMag = sett->value("wcs/minRefMag", 8).toDouble();
    //wcsParams.maxRefMag = sett->value("wcs/maxRefMag", 15).toDouble();
    wcsParams.redType = sett->value("wcs/redType", 0).toDouble();
    wcsParams.sMax = sett->value("wcs/sMax", 500).toDouble();
    wcsParams.weights = sett->value("wcs/weights", 0).toDouble();
    wcsParams.minRefStars = sett->value("wcs/minRefStars", 4).toDouble();
    wcsParams.sigma = sett->value("wcs/sigmaN", 3).toDouble();

    int starsNum = sett->value("catalogs/maxStars", 100).toInt();


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

    double mag0 = sett->value("catalogs/mag0", 6.0).toDouble();
    double mag1 = sett->value("catalogs/mag1", 15.0).toDouble();
    isMove2corner = sett->value("marks/isMove2corner", 0).toInt();

    qDebug() << QString("\n\n2\n");

//celestial
    QString observatoryCat = sett->value("celestial/observatoryCat", "./../../../data/cats/Obs.txt").toString();
    //obsCode->clear();
    obsCode = sett->value("celestial/obsName", "084").toString();
    maxObjDisp = sett->value("celestial/maxObjDisp", 2).toDouble();

    observatory *obsList = new observatory;
    qDebug() << QString("\n\n3\n");
    if(obsList->init(observatoryCat.toAscii().data(), OBS_SIZE))
    {
        qDebug() << QString("obsCat is not opened\n");
        return 1;
    }

    qDebug() << QString("\n\n4\n");

    fitsdata *fitsd = new fitsdata;
    if(fitsd->openFile(fileName, headType))
    {
        qDebug() << QString("File %1 is not opened\n").arg(fileName);
        return 1;
    }

    fitsd->initInst(telescopeFile, insNum);

    obsList->getobsynumO(obsCode.toAscii().data());
    fitsd->initObsPos(obsList->record);

    //РћР•РџР•РќРћРџР•Р”Р•РљРҐР Р­ WCS
    if(reFindWCS)
    {
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
        fitsd->copyImgGrid(fitsd->marksG, fitsd->marksGIpix);
        fitsd->moveMassCenter(fitsd->marksGIpix, mesPar.aperture);
        fitsd->measureMarksGrid(fitsd->marksGIpix, mesPar);
        //fitsd->detTan();

        int resAuto = fitsd->identAuto(fitsd->refMarks, fitsd->instr->rang);

        qDebug() << QString("resAuto: %1\n").arg(resAuto);

        if(fitsd->detWCS1(wcsParams)&&saveRefindWCS)fitsd->saveWCS();
        //fitsd->detTan();
        fitsd->marksGIpix->clearMarks();
    }
    //
//Р‘РЁРЇР Р®Р‘РҐР Р­ РћР®РџР®Р›Р•Р РџРЁ РҐРњРЇР РџРЎР›Р•РњР Р®
    //fitsd->marksG->setInstrProp(instruments->scx, instruments->scy, instruments->rang);
    //fitsd->marksGIpix->setInstrProp(instruments->scx, instruments->scy, instruments->rang);
//РҐР“ Р™Р®Р Р®РљРќР¦Р®


    fitsd->marksG->clearMarks();
    fitsd->getMarksGrid(starCatList.at(currentCat), fitsd->instr->fov, mag0, mag1, -1);
    fitsd->detTan();
//РќРђР—Р•Р™Р 
    fitsd->mpeWaitTime = mpeWaitTime;
    fitsd->sbWaitTime = sbWaitTime;
    if(lspmFind)
    {
        if(!(starCatList.size()<5)) fitsd->findLspmCat(starCatList[4]->exeName, starCatList[4]->exePath, starCatList[4]->catPath, fitsd->instr->fov, magObj0, magObj1);
    }
    if(skybotFind)
    {
    qDebug() << QString("skybotFindType\n");
    QStringList objNames;
    //tryMpeph = foDlg->ui_isUseExpNum->isChecked();
        if(tryMpeph)
        {
            fitsd->findSkybotNamesList(&objNames, skybot_prog, skybot_prog_folder, fitsd->instr->fov, obsCode, magObj0, magObj1);
            sz = objNames.size();
            for(i=0; i<sz; i++)fitsd->getMpephName(objNames.at(i), ast_eph_prog, ast_eph_prog_folder, magObj0, magObj1);


        }
        else fitsd->findSkybot(skybot_prog, skybot_prog_folder, fitsd->instr->fov, obsCode, magObj0, magObj1);
//fitsd->findLspmCat(starCatList[4]->exeName, starCatList[4]->exePath, starCatList[4]->catPath, fovSpinBox->value());
    }
    fitsd->detTanObj();
//reduction

    int resRed = fitsd->makeReduction(ruler3File, resFolder);


//РџР®Р“РљРќР’РҐР Р­

//    QDir(filePath).remove(lockFileName);

    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    delete fitsd;

    qInstallMsgHandler(0);

    //if(!resRed) QDir(filePath).remove(logFileName);

    return 0;

}
