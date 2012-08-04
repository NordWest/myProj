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

int main(int argc, char *argv[])    //ruler3PL.exe [0|1]
    {
        qInstallMsgHandler(customMessageHandler);
        QString msgstr, pnStr;
        QCoreApplication app(argc, argv);
        QTextStream stream(stdout);
///////// 1. Reading settings ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      /*  QTextCodec *codec1 = QTextCodec::codecForName("Windows-1251");
        Q_ASSERT( codec1 );

       // QString fileName = QString(argv[1]);
        QString cfgFile = codec1->toUnicode(argv[1]);
        QString fileName =  codec1->toUnicode(argv[2]);
        QString logFileName = QString("%1.log").arg(fileName);
        QString filePath = fileName.left(fileName.lastIndexOf("\\")+1);
        if(filePath=="") filePath = QString("./");
        QDir curDir(filePath);
        QString wcsFileName = QString("%1.wcs").arg(fileName);
        qDebug() << QString("fileName: %1\n").arg(fileName);
        qDebug() << QString("filePath: %1\n").arg(filePath);
        qDebug() << QString("wcsFileName: %1\n").arg(wcsFileName);
        qDebug() << QString("logFileName: %1\n").arg(logFileName);

        if(curDir.exists(logFileName)) return 1;
        QFile* logFile = new QFile(logFileName);
        if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
            clog = new QDataStream(logFile);

        //stream << "started....\n";
        int currentCat, isMove2corner, i, sz, resRed;
        double maxObjDisp;
        catFinder *starCat;
        QList <catFinder*> starCatList;
        QString obsCode;
        QString uTime;
        //BEGIN settings
        QSettings *sett = new QSettings(cfgFile, QSettings::IniFormat);
        //insSettings *instruments = new insSettings("./conf/telescopes.ini");
        //instruments->getNumIns(instruments->curInst);

        QString ruler3File = sett->value("general/ruler3File", "./conf/ruler3.ini").toString();
        QString telescopeFile = sett->value("telescope/telescopeFile", "./conf/telescopes.ini").toString();
        int insNum = sett->value("telescope/instrNum", 2).toInt();

        int isConvertToUTC = sett->value("General/isConvertToUTC").toInt();
        int isSpec = sett->value("General/isSpec").toInt();
        int expNum = sett->value("General/expNum").toInt();

        int mpeWaitTime = sett->value("General/mpeWaitTime").toInt();
        int sbWaitTime = sett->value("General/sbWaitTime").toInt();

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

    //magEqCorr
        QString dataSep = sett->value("magEqCorr/dataSep").toString();
        int isMagEqCorr = sett->value("magEqCorr/isMagEqCorr").toInt();
        QString xCoefFileName = sett->value("magEqCorr/xCoefFileName").toString();
        QString yCoefFileName = sett->value("magEqCorr/yCoefFileName").toString();

    //comaCorr
        int isComaCorr = sett->value("comaCorr/isComaCorr", 0).toInt();
        double cKsi = mas_to_grad((sett->value("comaCorr/cKsi", 0).toDouble()));
        double cEta = mas_to_grad((sett->value("comaCorr/cEta", 0).toDouble()));
        double cMag0 = (sett->value("comaCorr/cMag0", 0).toDouble());

    //vfCorr
        int isVFcorr = sett->value("vfCorr/isVFcorr", 0).toInt();
        QString vfCorrFile = sett->value("vfCorr/vfCorrFile", "vf.txt").toString();
        double rMax = sett->value("vfCorr/rMax", 0).toDouble();
        int nmin = sett->value("vfCorr/nmin", 0).toInt();

        observatory *obsList = new observatory;
        obsList->init(observatoryCat.toAscii().data(), OBS_SIZE);

        fitsdata *fitsd = new fitsdata();
        fitsd->fileName = fileName;

        fitsd->initInst(telescopeFile, insNum);



        if(isMagEqCorr) fitsd->initMagEqXDX(xCoefFileName, yCoefFileName, dataSep);
        if(isVFcorr) fitsd->initVFcorr(vfCorrFile, rMax, nmin);

        QString descS, oName;
        int oNum;

        QByteArray bpn;

    //получить заголовок пластинки
        if(atoi(argv[3]))   //0-http; 1-file
        {
            if(fitsd->loadHeaderFile(codec1->toUnicode(argv[4])))
            {
                qDebug() << "\nloadHeaderFile error\n";
                QDir(filePath).remove(logFileName);
                return 1;
            }
        }
        else
        {
            QTextCodec *codec1 = QTextCodec::codecForName("KOI8-R");
            Q_ASSERT( codec1 );
            qDebug() << QString("argc= %1").arg(argc);
            switch(plNameType)
            {
            case 0:
                {
                    if(argc==5)
                    {
                   //    bpn = codec1->fromUnicode(QString(argv[3]));
                        //qDebug() << QString("bpn: %1\n").arg(bpn.constData());
                    //    pnStr = bpn.toPercentEncoding("-/._~=?&");
                        pnStr = codec1->toUnicode(argv[4]);
                    }
                    else
                    {
                    //    bpn = codec1->fromUnicode(filePath.section("\\", -2, -2));
                    //    pnStr = filePath.section("\\", -2, -2);
                        pnStr = filePath.section("\\", -2, -2);
                    }
                   // pnStr = bpn.toPercentEncoding("-/._~=?&");
                }
                break;
            case 1:
                   pnStr = fileName.section("\\", -1, -1).section(".", 0, 0);
                   //qDebug() << QString("pnStr0: %1\n").arg(pnStr);
                   //pnStr = pnStr.right(pnStr.size()-2);
               break;
            }
            qDebug() << QString("pnStr: %1\n").arg(pnStr);


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
        if(fitsd->loadWCSFile(wcsFileName))
        {
            qDebug() << "\nloadWCSFile error\n";
            QDir(filePath).remove(logFileName);
            return 1;
        }

        //cKsi /= fitsd->WCSdata[4]/7.9;
        //cEta /= fitsd->WCSdata[5]/7.9;

        if(isComaCorr) fitsd->initComaCorr(cKsi, cEta, cMag0);


        obsList->getobsynumO(obsCode.toAscii().data());
        if(isRedRef) fitsd->initRefractParam(obsList->record);
        fitsd->setExposure(expNum);
        fitsd->headList.getKeyName("U", &uTime);
  /*      fitsd->MJD += uTime.trimmed().toDouble()/86400.0;

        //fitsd->setSpecTime(isConvertToUTC, isSpec, obsList->record->Long);
    //выставить параметры инструмента
      //  fitsd->marksG->setInstrProp(instruments->scx, instruments->scy, instruments->rang);
      //  fitsd->marksGIpix->setInstrProp(instruments->scx, instruments->scy, instruments->rang);
    //открыть метки
        fitsd->marksGIpix->loadTanImg(fileName, mSep, mColumn);
        if(isMove2corner) fitsd->marksGIpix->moveToCorner(fitsd->naxes);
    //из каталога
        fitsd->marksG->clearMarks();
        fitsd->getMarksGrid(starCatList.at(currentCat), fitsd->instr->fov, mag0, mag1, -1);
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
        else if(tryMpeph)
        {
            if(!fitsd->headList.getKeyName("OBJECT", &descS))
            {
                desc2NumName(descS, &oNum, &oName);
                fitsd->getMpephName(oName, ast_eph_prog, ast_eph_prog_folder, magObj0, magObj1);
            }
        }
      //  fitsd->cleanObjects(apeSize);
    //редукция
    resRed = fitsd->makeReduction1(ruler3File);

    if(!resRed) QDir(filePath).remove(logFileName);
*/

    qInstallMsgHandler(0);

//

    return 0;
}
