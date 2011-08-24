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
    QSettings *sett = new QSettings("./wcs2mks.ini", QSettings::IniFormat);


    //insSettings *instruments = new insSettings("./conf/telescopes.ini");
    //instruments->getNumIns(instruments->curInst);

    int expNum = sett->value("General/expNum").toInt();
    int apeSize = sett->value("General/aperture").toInt();


    //int mpeWaitTime = sett->value("General/mpeWaitTime", 100000).toInt();
   //int sbWaitTime = sett->value("General/sbWaitTime", 1000000).toInt();

    int isRedRef = sett->value("general/isRedRef", 0).toInt();

    int plNameType = sett->value("general/plNameType", 0).toInt();

    int lspmFind = sett->value("objectsFind/lspmFind").toInt();
    int skybotFind = sett->value("objectsFind/skybotFind").toInt();
    int tryMpeph = sett->value("objectsFind/tryMpeph").toInt();
    int mpephType = sett->value("objects/mpephType", 0).toInt();
    double magObj0 = sett->value("objectsFind/mag0", 6.0).toDouble();
    double magObj1 = sett->value("objectsFind/mag1", 15.0).toDouble();


//  catalogs    /////
    QString catIni = sett->value("catalogs/catIni", "./conf/catalogs.ini").toString();
    int catNum = sett->value("catalogs/catNum", 0).toInt();
    double mag0 = sett->value("catalogs/mag0", 6.0).toDouble();
    double mag1 = sett->value("catalogs/mag1", 16.0).toDouble();
    //
//process
        QString mpeph_prog = sett->value("processes/mpeph_prog", "./mpeph.exe").toString();
        QString mpeph_prog_folder = sett->value("processes/mpeph_prog_folder", "./").toString();
        int mpeph_wait_time = sett->value("processes/mpeph_wait_time", -1).toInt();
        if(mpeph_wait_time>0) mpeph_wait_time *= 1000;
        QString skybot_prog = sett->value("processes/skybot_prog", "./skybotclient.exe").toString();
        QString skybot_prog_folder = sett->value("processes/skybot_prog_folder", "./").toString();
        int skybot_wait_time = sett->value("processes/skybot_wait_time", -1).toInt();
        if(skybot_wait_time>0) skybot_wait_time *= 1000;
        QString gethttp_prog = sett->value("processes/gethttp_prog", "./getHttpHeader.exe").toString();
        QString gethttp_prog_folder = sett->value("processes/gethttp_prog_folder", "./").toString();
        int gethttp_wait_time = sett->value("processes/gethttp_wait_time", -1).toInt();
        if(gethttp_wait_time>0) gethttp_wait_time *= 1000;
        QString utcorr_prog = sett->value("processes/utcorr_prog", "./uTimeCorr.exe").toString();
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


    fitsdata *fitsd = new fitsdata();


/////////////////////////////

        initCatList(&starCatList, catIni);
        qDebug() << QString("starCatList count: %1\n").arg(starCatList.count());

/////////////////////////////

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
        if(obsList->getobsynumO(obsCode.toAscii().data()))
        {
            qDebug() << QString("obsCode is not found\n");
            if(isRemLog)QDir().remove(logFileName);
            return 1;
        }
        obsPos = obsList->record;
        fitsd->initObsPos(obsList->record);


/////////////////////////////

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


    /////////////////////////////

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

    /////////
refractionParam *refParam;
            if(isRedRef)
            {
                qDebug() << "initRefractParam\n";
                refParam = new refractionParam;
                if(initPlateRefParam(refParam, fitsd, obsPos))refParam==NULL;
    /*
                refParam->utc = fitsd->MJD;

                refParam->ra0 = fitsd->WCSdata[2];
                refParam->de0 = fitsd->WCSdata[3];

                if(obsPos!=NULL) refParam->Fi = obsPos->getFi();
                else refParam->Fi = 0.0;
                if(obsPos!=NULL) refParam->Long = obsPos->Long;
                else refParam->Long = 0.0;
                refParam->lam = 0.575;

                QString kVal;
                int p0, p1;
                double temp;

                if(!fitsd->headList.getKeyName("PRESSURE", &kVal))
                {
                    qDebug() << QString("\nPRESSURE |%1|\n").arg(kVal);
                    refParam->press = kVal.toDouble();
                }
                else refParam->press = 760.0;
                if(refParam->press<700.0)refParam->press = 760.0;
                if(!fitsd->headList.getKeyName("TEMPERAT", &kVal))
                {
                    qDebug() << QString("\nTEMPERAT |%1|\n").arg(kVal);
                    refParam->temp = kVal.toDouble();
                }
                else refParam->temp = 0.0;
    */
            }

    ////////
    //if(isRedRef) fitsd->initRefractParam(obsList->record);

            /////////////////////////////

                    ExposureList *expList = new ExposureList;
                    ExposureRec *expRec = new ExposureRec;

                    int expRes = initExpList(expList, fitsd->headList, obsList->record);
                    expList->getExp(expRec, expNum);
                    fitsd->setMJD(expRec->expTime);


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
