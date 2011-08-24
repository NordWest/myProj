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

    QString logFileName = QString("wcs2mks.log");

    QFile* logFile = new QFile(logFileName);
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered)) clog = new QDataStream(logFile);


    //QString plateName;
    QProcess outerProcess;
    QStringList outerArguments;
    QString dateStr, timeStr, descS, oName;
    int oNum;
    int i, sz;
    QList <catFinder*> starCatList;
    QString filePath, pnStr;
    QString obsCode;
    QString uTime;

//    QString tifFName = QString(argv[1]);
    QString wcsFName = codec1->toUnicode(argv[1]);
    QFileInfo fi(wcsFName);
    filePath = fi.absoluteFilePath();

 //   QString curDir = tifFName.left(tifFName.lastIndexOf("\\"));
 //   qDebug() << QString("curDir: %1\n").arg(curDir);


    //BEGIN settings
    QSettings *sett = new QSettings("./wcs2mks.ini", QSettings::IniFormat);

//general
    int expNum = sett->value("General/expNum").toInt();
    int apeSize = sett->value("General/aperture").toInt();
    int plNameType = sett->value("general/plNameType", 0).toInt();
    int useUtCorr = sett->value("general/useUtCorr", 0).toInt();
    double fovp = sett->value("general/fovp", 1.0).toDouble();        //field of view percent, [0.0 - 1.0]
    int saveCatMks = sett->value("general/saveCatMks", 0).toInt();
    int saveObjMks = sett->value("general/saveObjMks", 0).toInt();

//insSettings
    QString insSettFile = sett->value("insSettings/insSettFile", "./conf/telescopes.ini").toString();
    int instrNum = sett->value("insSettings/instrNum", 0).toInt();

//objects   ////
        int lspmFind = sett->value("objects/lspmFind", 0).toInt();
        int skybotFind = sett->value("objects/skybotFind", 0).toInt();
        int tryMpeph = sett->value("objects/tryMpeph", 0).toInt();
        int mpephType = sett->value("objects/mpephType", 0).toInt();
        double magObj0 = sett->value("objects/mag0", 6.0).toDouble();
        double magObj1 = sett->value("objects/mag1", 15.0).toDouble();

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

    insSettings instr(insSettFile);
    instr.getNumIns(instrNum);
    fitsd->setInstrSettings(instr);

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
            return 1;
        }

        qDebug() << QString("observatory initiated\n");

/////////////////////////////
        if(obsList->getobsynumO(obsCode.toAscii().data()))
        {
            qDebug() << QString("obsCode is not found\n");
            return 1;
        }
        obsPos = obsList->record;
        fitsd->initObsPos(obsList->record);


/////////////////////////////

    QByteArray bpn;

    //QString a3 = codec1->toUnicode(argv[3]);

//получить заголовок пластинки
    //get header

            int doWhat;
            if(argc==4) doWhat = atoi(argv[2]);
            else doWhat = 0;
            if(doWhat)   //0-http; 1-file
            {
                if(fitsd->loadHeaderFile(QString(argv[3])))
                {
                    qDebug() << "\nloadHeaderFile error\n";
                    return 1;
                }
            }
            else
            {

                qDebug() << QString("argc= %1").arg(argc);
                if(argc==4) pnStr = QString(argv[3]);
                else detPlateName(&pnStr, filePath, plNameType);



                qDebug() << QString("pnStr: %1\n").arg(pnStr);



                outerArguments.clear();
                outerProcess.setWorkingDirectory(gethttp_prog_folder);
                outerProcess.setProcessChannelMode(QProcess::MergedChannels);
                outerProcess.setReadChannel(QProcess::StandardOutput);
                outerArguments << pnStr.toAscii().constData();
                qDebug() << gethttp_prog << outerArguments.join(" ") << "\n";
                outerProcess.start(gethttp_prog, outerArguments);

                outerProcess.waitForFinished(gethttp_wait_time);
                //QTextStream catStream(outerProcess.readAllStandardOutput());

                //if(fitsd->readHttpHeader(catStream.readAll()))
                if(fitsd->readHttpHeader(QString(outerProcess.readAllStandardOutput())))
                {
                    qDebug() << "\nreadHttpHeader error\n";
                    return 1;
                }
            }

    /////////////////////////////
            // load wcs
                    if(fitsd->loadWCSFile(wcsFName))
                    {
                        qDebug() << "\nloadWCSFile error\n";
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
    //fitsd->setSpecTime(isConvertToUTC, isSpec, obsList->record->Long);
                    ////////
/*
                            if(fitsd->loadIpixMarks(fileName, mSep, mCol))
                            {
                                qDebug() << "Ipix marks load error\n";
                                return 1;
                            }

                            if(detRect) fitsd->detIpixWorkFrame();
                            else fitsd->workFrame.setCoords(rectX0, rectY0, rectX1, rectY1);
*/
                    /////////
                            double fov = fovp*instr.fov;//->fitsd->detFov();

                            fitsd->catMarks->clearMarks();
                            getMarksGrid(fitsd->catMarks, starCatList.at(catNum), fitsd->MJD, fitsd->WCSdata[2], fitsd->WCSdata[3], fov, mag0, mag1, -1);
                            fitsd->detTan();

                            if(detRect) fitsd->detCatWorkFrame();
                            else fitsd->workFrame.setCoords(rectX0, rectY0, rectX1, rectY1);

                    //////////

                            fov = fitsd->detFov();

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
                                if(!fitsd->headList.getKeyName("OBJECT", &descS))
                                {
                                    desc2NumName(descS, &oNum, &oName);
                                    if(mpephType) getMpephNum(fitsd->objMarks, fitsd->MJD, QString("%1").arg(oNum), mpeph_prog, mpeph_prog_folder, magObj0, magObj1, mpeph_wait_time);
                                    else getMpephName(fitsd->objMarks, fitsd->MJD, oName, mpeph_prog, mpeph_prog_folder, magObj0, magObj1, mpeph_wait_time);
                                }
                            }
                            fitsd->detTanObj();
                    ///////////
    fitsd->cleanObjects(apeSize);

    QString iniSuff = codec1->toUnicode(argv[1]);
    iniSuff = iniSuff.left(iniSuff.lastIndexOf("."));
    QString resName = QString("%1.mks").arg(iniSuff);
    QFile rFile(resName);
    rFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    rFile.close();


/////////////////////////////////////////////////////////////////////////////////////////////

    int sz0 = fitsd->catMarks->marks.size();
    for(i=0; i<sz0; i++)
    {
        fitsd->catMarks->marks.at(i)->P = new double[21];
        fitsd->catMarks->marks.at(i)->P[19] = apeSize*apeSize;
        fitsd->catMarks->marks.at(i)->P[3] = 0;
    }

    if(saveCatMks) fitsd->catMarks->saveTanImg(resName, mSep, mCol);

    sz0 = fitsd->objMarks->marks.size();
    qDebug() << QString("objSize= %1\n").arg(sz0);
    for(i=0; i<sz0; i++)
    {
        fitsd->objMarks->marks.at(i)->P = new double[21];
        fitsd->objMarks->marks.at(i)->P[19] = apeSize*apeSize;
        fitsd->objMarks->marks.at(i)->P[3] = 0;
    }

    if(saveObjMks) fitsd->objMarks->saveTanImg(resName, mSep, mCol);


/////////////////////////////////////////////////////////////////////////////////////////////





qInstallMsgHandler(0);


    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

//    qInstallMsgHandler(0);
    return 0;
}
