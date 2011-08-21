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

int main(int argc, char *argv[])        //.exe n.lor.000.wcs [http|file]key plateName|headFile
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);
//    QTextCodec *codec1 = QTextCodec::codecForName("KOI8-R");
    QTextCodec *codec1 = QTextCodec::codecForName("Windows-1251");

    Q_ASSERT( codec1 );

    QString logFileName = QString("tiff2fitsCut.log");

    QFile* logFile = new QFile(logFileName);
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered)) clog = new QDataStream(logFile);


    QString plateName;


//    QString tifFName = QString(argv[1]);
    QString wcsFName = codec1->toUnicode(argv[1]);

 //   QString curDir = tifFName.left(tifFName.lastIndexOf("\\"));
 //   qDebug() << QString("curDir: %1\n").arg(curDir);

    int currentCat, isMove2corner, i, sz, resRed;
    double maxObjDisp;
    catFinder *starCat;
    QList <catFinder*> starCatList;
    QString filePath, pnStr;
    QString obsCode;
    QString uTime;
    //BEGIN settings
    QSettings *sett = new QSettings("./conf/conf.ini", QSettings::IniFormat);

    QString telescopeFile = sett->value("telescope/telescopeFile", "./conf/telescopes.ini").toString();
    int insNum = sett->value("telescope/instrNum", 2).toInt();

    //insSettings *instruments = new insSettings("./conf/telescopes.ini");
    //instruments->getNumIns(instruments->curInst);

    int isConvertToUTC = sett->value("General/isConvertToUTC").toInt();
    int isSpec = sett->value("General/isSpec").toInt();
    int expNum = sett->value("General/expNum").toInt();

    int apeSize = sett->value("General/aperture").toInt();

    int mpeWaitTime = sett->value("General/mpeWaitTime", 100000).toInt();
    int sbWaitTime = sett->value("General/sbWaitTime", 1000000).toInt();

    int isRedRef = sett->value("general/isRedRef", 0).toInt();

    int plNameType = sett->value("general/plNameType", 0).toInt();

    int lspmFind = sett->value("objectsFind/lspmFind").toInt();
    int skybotFind = sett->value("objectsFind/skybotFind").toInt();
    int tryMpeph = sett->value("objectsFind/tryMpeph").toInt();
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
    //
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
    QString get_http_prog = sett->value("processes/get_http_prog", "./getHttpHeader.exe").toString();
    QString get_http_prog_folder = sett->value("processes/get_http_prog_folder", "./").toString();


    currentCat = sett->value("catalogs/currentCatalog", 0).toInt();
    QString observatoryCat = sett->value("catalogs/observatoryCat", "./../../../data/cats/Obs.txt").toString();
    isMove2corner = sett->value("marks/isMove2corner", 0).toInt();
    double mag0 = sett->value("catalogs/mag0", 6.0).toDouble();
    double mag1 = sett->value("catalogs/mag1", 15.0).toDouble();

//celestial
    //obsCode->clear();
    obsCode = sett->value("celestial/obsName", "084").toString();
    maxObjDisp = sett->value("celestial/maxObjDisp", 2).toDouble();

    QString mSep = sett->value("marks/marksFileSep").toString();
    QString mColumn = sett->value("marks/marksLoadFileColumn").toString();

    observatory *obsList = new observatory;
    obsList->init(observatoryCat.toAscii().data(), OBS_SIZE);

    fitsdata *fitsd = new fitsdata();
//    fitsd->fileName = fileName;

    fitsd->initInst(telescopeFile, insNum);


    QByteArray bpn;

    QString a3 = codec1->toUnicode(argv[3]);

//получить заголовок пластинки
    if(atoi(argv[2]))   //0-http; 1-file
    {
        if(fitsd->loadHeaderFile(a3))
        {
            qDebug() << "\nloadHeaderFile error\n";
            QDir(filePath).remove(logFileName);
            return 1;
        }
    }
    else
    {

        qDebug() << QString("argc= %1").arg(argc);

        //bpn = codec1->fromUnicode(QString(argv[3]));
        pnStr = a3;
/*
        switch(plNameType)
        {
        case 0:
            {
                if(argc==4)
                {
                    bpn = codec1->fromUnicode(QString(argv[4]));
                    //qDebug() << QString("bpn: %1\n").arg(bpn.constData());
                //    pnStr = bpn.toPercentEncoding("-/._~=?&");
                }
                else
                {
                    bpn = codec1->fromUnicode(filePath.section("\\", -2, -2));
                //    pnStr = filePath.section("\\", -2, -2);
                }
                pnStr = bpn.toPercentEncoding("-/._~=?&");
            }
            break;
        case 1:
               pnStr = fileName.section("\\", -1, -1).section(".", 0, 0);
               //qDebug() << QString("pnStr0: %1\n").arg(pnStr);
               pnStr = pnStr.right(pnStr.size()-2);
           break;
        }
        qDebug() << QString("pnStr: %1\n").arg(pnStr);
*/

        QProcess outerProcess;
        QStringList outerArguments;
        outerArguments.clear();
//           QString ucac2find_folder("");

        outerProcess.setWorkingDirectory(get_http_prog_folder);
        outerProcess.setProcessChannelMode(QProcess::MergedChannels);
        outerProcess.setReadChannel(QProcess::StandardOutput);
        //outerArguments << instruments->curName.toLower() << pnStr;
        outerArguments << QString("na") << pnStr;
        qDebug() << get_http_prog << outerArguments.join(" ");
        outerProcess.start(get_http_prog, outerArguments);

        outerProcess.waitForFinished(-1);
        QTextStream catStream(outerProcess.readAllStandardOutput());

        //QByteArray httpData = catStream.readAll();// http->readAll();//чтение данных (ответа)


        if(fitsd->readHttpHeader(catStream.readAll()))
        {
            qDebug() << "\nreadHttpHeader error\n";
            QDir(filePath).remove(logFileName);
            return 1;
        }
    }
    if(fitsd->loadWCSFile(wcsFName))
    {
        qDebug() << "\nloadWCSFile error\n";
        QDir(filePath).remove(logFileName);
        return 1;
    }
    obsList->getobsynumO(obsCode.toAscii().data());
    if(isRedRef) fitsd->initRefractParam(obsList->record);
    fitsd->setExposure(expNum);
    if(!fitsd->headList.getKeyName("U", &uTime)) fitsd->MJD += uTime.trimmed().toDouble()/86400.0;

    //fitsd->setSpecTime(isConvertToUTC, isSpec, obsList->record->Long);
//выставить параметры инструмента
    //fitsd->marksG->setInstrProp(instruments->scx, instruments->scy, instruments->rang);
    //fitsd->marksGIpix->setInstrProp(instruments->scx, instruments->scy, instruments->rang);
/*//открыть метки
    fitsd->marksGIpix->loadTanImg(fileName, mSep, mColumn);
    if(isMove2corner) fitsd->marksGIpix->moveToCorner();*/
//из каталога
    fitsd->marksG->clearMarks();
    fitsd->getMarksGrid(starCatList.at(currentCat), fitsd->instr->fov, mag0, mag1, -1);//, fitsd->detNaxes()*60*fitsd->marksG->scales[0]);
    fitsd->detNaxes(&fitsd->naxes[0], &fitsd->naxes[1]);
    fitsd->detTan();
//объект
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
    fitsd->cleanObjects(apeSize);

    QString iniSuff = codec1->toUnicode(argv[1]);
    iniSuff = iniSuff.left(iniSuff.lastIndexOf("."));
    QString resName = QString("%1.mks").arg(iniSuff);
    QFile rFile(resName);
    rFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    rFile.close();


/////////////////////////////////////////////////////////////////////////////////////////////

    int sz0 = fitsd->marksG->marks .size();
    for(i=0; i<sz0; i++)
    {
        fitsd->marksG->marks.at(i)->P = new double[21];
        fitsd->marksG->marks.at(i)->P[19] = apeSize*apeSize;
        fitsd->marksG->marks.at(i)->P[3] = 0;
    }

    fitsd->marksG->saveTanImg(resName, mSep, mColumn);

    sz0 = fitsd->objMarks->marks.size();
    qDebug() << QString("objSize= %1\n").arg(sz0);
    for(i=0; i<sz0; i++)
    {
        fitsd->objMarks->marks.at(i)->P = new double[21];
        fitsd->objMarks->marks.at(i)->P[19] = apeSize*apeSize;
        fitsd->objMarks->marks.at(i)->P[3] = 0;
    }

    fitsd->objMarks->saveTanImg(resName, mSep, mColumn);


/////////////////////////////////////////////////////////////////////////////////////////////





qInstallMsgHandler(0);


    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

//    qInstallMsgHandler(0);
    return 0;
}
