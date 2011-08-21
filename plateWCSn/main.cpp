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



int main(int argc, char *argv[])// plateWCSn conf.ini marks.txt pnType plateNum|headerFile
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);

    QTextCodec *codec1 = QTextCodec::codecForName("Windows-1251");
    Q_ASSERT( codec1 );

    QTextCodec *codec2 = QTextCodec::codecForName("IBM 866");
    Q_ASSERT( codec2 );

    QString cfgFile = codec1->toUnicode(argv[1]);
    QString fileName = codec1->toUnicode(argv[2]);
    QString logFileName = fileName+".wcs.log";

    QFileInfo fi(fileName);
    QString platePath = fi.absolutePath();
    //QString platePath = fileName.left(fileName.lastIndexOf("\\")+1);
    //if(platePath=="") platePath = fileName.left(fileName.lastIndexOf("/")+1);
    //if(platePath=="") platePath = QString("");

    QDir curDir(platePath);
    //qDebug() << QString("fileName: %1\n").arg(fileName);
    //qDebug() << QString("platePath: %1\n").arg(platePath);
    QString wcsFileName = QString("%1.wcs").arg(fileName);
    QString wcsLockFile = QString("%1.lock").arg(fileName);


    //if(curDir.exists(wcsLockFile)||curDir.exists(wcsFileName)) return 1;
    if(QDir().exists(wcsLockFile)||QDir().exists(wcsFileName)) return 1;
    else
    {
        QFile lockFile(wcsLockFile);
        lockFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
        /*QTextStream errStream;
        errStream.setDevice(&lockFile);
        errStream << "";*/
        lockFile.close();
    }





    QFile* logFile = new QFile(logFileName);
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);

    if(argc!=5)
    {
        qDebug() << "argc= " << argc << "\n";
        return 1;
    }
    qDebug() << QString("platePath: %1\n").arg(platePath);

    //QDir curDir(platePath);
    if(!curDir.exists(fi.fileName()))
    {
        qDebug() << QString("file %1 does not exist\n").arg(fi.fileName());
    }

    int currentCat, isMove2corner, useProxy, proxyPort;
    double maxObjDisp;
    catFinder *starCat;
    QList <catFinder*> starCatList;
    QString proxyHost, plateHostName, obsCode;
    int sz, i, j;
    ExposureRec* eRec;
    QList <ExposureRec*> expList;

    int endObs = 0;
    double tObs0, tObs1, time;
    DATEOBS dateObs0, dateObs1;
    HeadRecord *hRec;
    QStringList tList;
    QString telName;


    QSettings *sett = new QSettings(cfgFile, QSettings::IniFormat);

    QStringList allSett;
    allSett << sett->allKeys();
    sz = allSett.size();
    qDebug() << QString("\n\nconf.ini:\n");
    for(i=0; i<sz; i++) qDebug() << QString("%1=%2\n").arg(allSett.at(i)).arg(sett->value(allSett.at(i)).toString());
    qDebug() << "\n\n";

    QString telescopeFile = sett->value("telescope/telescopeFile", "./conf/telescopes.ini").toString();
    int insNum = sett->value("telescope/instrNum", 2).toInt();
    //insSettings *instruments = new insSettings(telescopeFile);

    //QString ruler3File = sett->value("general/ruler3File", "./conf/ruler3.ini").toString();


    double apeSize = sett->value("general/aperture", 30).toDouble();

    int identType = sett->value("identify/identType", 0).toInt();
    int targNum = sett->value("identify/targNum", 6).toInt();
    int maxNum = sett->value("identify/maxNum", 100).toInt();


    reductionParams wcsParams;

    wcsParams.maxres = sett->value("wcs/maxres", 300).toDouble();
    wcsParams.maxresMAG = sett->value("wcs/maxresMAG", 10).toDouble();
    //wcsParams.minRefMag = sett->value("wcs/minRefMag", 8).toDouble();
    //wcsParams.maxRefMag = sett->value("wcs/maxRefMag", 15).toDouble();
    wcsParams.redType = sett->value("wcs/redType", 0).toInt();
    wcsParams.sMax = sett->value("wcs/sMax", 500).toDouble();
    wcsParams.weights = sett->value("wcs/weights", 0).toInt();
    wcsParams.minRefStars = sett->value("wcs/minRefStars", 4).toInt();
    wcsParams.sigma = sett->value("wcs/sigmaN", 3).toDouble();
    int wcsN = sett->value("wcs/wcsN", 4).toInt();

    int starsNum = sett->value("wcs/starsNum", 100).toInt();

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

    currentCat = sett->value("catalogs/currentCatalog", 0).toInt();
    double mag0 = sett->value("catalogs/mag0", 6.0).toDouble();
    double mag1 = sett->value("catalogs/mag1", 16.0).toDouble();
    isMove2corner = sett->value("marks/isMove2corner", 0).toInt();


 //   QString ast_eph_prog = sett->value("processes/ast_eph_prog", "./mpeph.exe").toString();
 //   QString ast_eph_prog_folder = sett->value("processes/ast_eph_prog_folder", "./").toString();
    QString get_http_prog = sett->value("processes/get_http_prog", "./getHttpHeader.exe").toString();
    QString get_http_prog_folder = sett->value("processes/get_http_prog_folder", "./").toString();
//celestial
    //obsCode->clear();
    QString observatoryCat = sett->value("celestial/observatoryCat", "./../../../data/cats/Obs.txt").toString();
    obsCode = sett->value("celestial/obsName", "084").toString();
    maxObjDisp = sett->value("celestial/maxObjDisp", 2).toDouble();

    QString mSep = sett->value("marks/marksFileSep").toString();
    QString mColumn = sett->value("marks/marksLoadFileColumn").toString();


   // fitsdata *plate = new fitsdata();
//codec1->fromUnicode(QString(argv[3]));
  //  fileName = codec1->toUnicode(argv[1]);
    //qDebug() << QString("bpn: %1\n").arg(bpn.constData());
  //  fileName = bpn.toPercentEncoding("-/._~=?&");
   // qDebug() << QString("pnStr: %1\n").arg(pnStr);


    //fileName = QString(argv[1]);


    if(fileName.lastIndexOf("\\")==-1)telName = fileName.section("/", -1, -1).section(".", 0, 0);
    else telName = fileName.section("\\", -1, -1).section(".", 0, 0);
    qDebug() << QString("telName: %1\n").arg(telName);
    //int instNum;
    if(telName.indexOf("n")==0) insNum = 0;
    if(telName.indexOf("e")==0) insNum = 1;
    if(telName.indexOf("s")==0) insNum = 2;
    if(telName.indexOf("w")==0) insNum = 3;
    qDebug() << QString("instNum= %1\n").arg(insNum);

    //instruments->getNumIns(instNum);

    observatory *obsList = new observatory;
    qDebug() << QString("observatoryCat: %1\n").arg(observatoryCat.toAscii().data());
    int obsInitRes = obsList->init(observatoryCat.toAscii().data(), OBS_SIZE);
    qDebug() << QString("obsInitRes: %1\n").arg(obsInitRes);

    fitsdata *fitsd = new fitsdata();
    fitsd->openEmptyFile();
    fitsd->initInst(telescopeFile, insNum);

//РїВ·РїВІРїВ Рїв•‘Рїв–“Рїв•”Рїв•ђРїв•њ РївЊ Рїв•ќРїв••РїВІРїВ РїВІРїв–’РїВІРїв‰Ґ РїВ·РїВ Рїв•ќРїв•ћРїв•ђРїв•”РїВ°Рїв‰ҐРїв•”
    QProcess outerProcess;
    QStringList outerArguments;

 //    codec1 = QTextCodec::codecForName("KOI8-R");
 //    Q_ASSERT( codec1 );

    qDebug() << "getHeader\n";

    if(atoi(argv[3]))   //0-http; 1-file
    {
        fitsd->loadHeaderFile(QString(argv[4]));
    }
    else
    {

        //QByteArray bpn = codec1->fromUnicode(codec1->toUnicode(argv[3]));
        //qDebug() << QString("bpn: %1\n").arg(bpn.constData());
        //QString pnStr = bpn.toPercentEncoding("-/._~=?&");
        QString pnStr = codec1->toUnicode(argv[4]);
        qDebug() << QString("pnStr: %1\n").arg(pnStr);

        outerArguments.clear();

        outerProcess.setWorkingDirectory(get_http_prog_folder);
        outerProcess.setProcessChannelMode(QProcess::MergedChannels);
        outerProcess.setReadChannel(QProcess::StandardOutput);
        //outerArguments << instruments->curName.toLower() << pnStr;
        outerArguments << QString("na") << pnStr;
        qDebug() << get_http_prog << outerArguments.join(" ");
        outerProcess.start(get_http_prog, outerArguments);

        outerProcess.waitForFinished(-1);
        QTextStream catStream(outerProcess.readAllStandardOutput());

        //QByteArray httpData = catStream.readAll();// http->readAll();//Рїв–“Рїв•ђРїв€™РїВ°Рїв•”Рїв€™ Рїв– Рїв•ќРїВ°РїВ°Рїв•—РїС‘ (РїВІРїв•ђРїв–’Рїв€™Рїв•ђРїв•ќ)


        if(fitsd->readHttpHeader(catStream.readAll()))
        {
            qDebug() << "\nreadHttpHeader error\n";
            QDir(platePath).remove(wcsFileName);
            return 1;
        }

    }

//Рїв–’Рїв•—Рїв•ћРїв•ђРїв•ќРїв–’Рїв•”Рїв•ђРїв•њ РїВ·Рїв•ќРїГ·Рїв•ќРївЊЎРїв€™Рїв•ђРїГ·Рїв•— Рїв•”РїВ°Рїв•ћРїв•ђРїГ·Рїв•‘РївЊЎРїв€™РїВ°Рїв•ђРїв•ќ
    //fitsd->marksG->setInstrProp(instruments->scx, instruments->scy, instruments->rang);
    //fitsd->marksGIpix->setInstrProp(instruments->scx, instruments->scy, instruments->rang);
//РїВІРїв•ђРїв‰ҐРїГ·Рїв•—Рїв•ђРїв•њ РївЊЎРїв€™Рїв•ђРїв‰ҐРїв•”
    fitsd->marksGIpix->loadTanImg(fileName, mSep, mColumn);
    if(isMove2corner) fitsd->marksGIpix->moveToCorner(fitsd->naxes);

//time
    if(obsList->getobsynumO(obsCode.toAscii().data()))
    {
        qDebug() << "obs not found\n";
        QDir().remove(wcsLockFile);
        return 2;
    }
    //fitsd->setSpecTime(isConvertToUTC, isSpec, obsList->record->Long);
    /*
    double sJD, eJD, d_day, uTimeJD, utc, jd0, uTimeMJD;
    if(expNum<0) expNum = expList.size()-1;
    fitsd->MJD = getMJDfromStrFTN(expList.at(expNum)->expStr, 0);
    jd0 = UT0JD(mjd2jd(fitsd->currentCatalogMJD), &sJD);
    utc = sJD;
    if(isConvertToUTC)
    {
        qDebug() << QString("obsNum %1\n").arg(obsCode);
        if(obsList->getobsynumO(obsCode.toAscii().data()))
        {
            qDebug() << "obs not found\n";
            QDir(platePath).remove(wcsFileName);
            return 2;
        }


        sJD2UT1(jd0, sJD, obsList->record->Long, &utc);
        uTimeJD = jd0 + utc;
        //s2UTC(mjd2jd(curF->MJD), obsList->record->Long, &uTimeJD);
        //qDebug() << QString("jd= %1\tlong=%2\tJD=%3\n").arg(mjd2jd(getMJDfromStrFTN(dateObsLE->text(), 0))).arg(obsList->record->Long).arg(uTimeJD);
        fitsd->MJD = uTimeMJD = jd2mjd(uTimeJD);


    }
    if(isSpec)
    {
        d_day=fitsd->MJD*2.0;
        d_day-=chet(d_day);
        eJD = ((double)(int)d_day)/2.0;
        d_day = (fitsd->MJD - eJD) - 0.5;
        qDebug() << QString("MJD= %1\teJD = %2\td_day = %3\n").arg(fitsd->MJD).arg(eJD).arg(d_day);
        if(d_day>0.0&&d_day<0.2917)
        {
            //curF->MJD += 1.0;
            jd0+=1.0;
            //jd0 = UT0JD(mjd2jd(curF->MJD), &sJD);
            sJD2UT1(jd0, sJD, obsList->record->Long, &utc);
            //curF->MJD = jd2mjd(jd0 + utc);
        }


    }
    QString uTimeLE;
    fitsd->headList.getKeyName("U", &uTimeLE);
    fitsd->MJD = jd2mjd(jd0 + utc);
    //qDebug() << QString("uTime: %1\n").arg(uTimeLE->text().trimmed().toDouble());
    fitsd->MJD += uTimeLE.trimmed().toDouble()/86400.0;

    QString timeStr = QString(getStrFromDATEOBS(getDATEOBSfromMJD(fitsd->MJD), " ", 2, 4));
    //expLineEdit->setText(expLE->text());
    */
//РївЊ Рїв•ќРїв••РїГ·Рїв•‘РївЊ Рїв•”Рїв•ђРїв•њ РївЊЎРїв€™Рїв•ђРїв‰ҐРїв•” Рїв‰ҐРїв•ќРїв•ђРїв•ќРїВ РїВІРїв••Рїв•ќ
/*
    QString comLine;
    //fitsd->headList.getKeyName()
    comLine = rdStr.trimmed();
    QStringList operands = comLine.split(" ");
    double ra_oc = hms_to_mas(operands[0] +" "+ operands[1] +" "+ operands[2]," ");
    double de_oc = damas_to_mas(operands[3] +" "+ operands[4] +" "+ operands[5]," ");
    /*QString str = operands[6];
    mag_1 = str.toDouble();
    str = operands[7];
    mag_2 = str.toDouble();*/


    //marksGrid *mGr = fitsd->marksG;
 /*   if(!fitsd->WCSdata[12])
    {
        fitsd->WCSdata[2] = mas_to_grad(ra_oc);
        fitsd->WCSdata[3] = mas_to_grad(de_oc);
    }*/
 //   fitsd->MJD = getMJDfromStrFTN(timeStr, 0);
    qDebug() << "\nGetMarksGrid\n";
    qDebug() << "\nWCS2\t" << fitsd->WCSdata[2] << "\t" << fitsd->WCSdata[3] << "\n";
    fitsd->marksG->clearMarks();
    fitsd->getMarksGrid(starCatList.at(currentCat), fitsd->instr->fov, mag0, mag1, starsNum);
    fitsd->detTan();
//find stars
 //   fitsd->findCloserStars(apeSize);
//ident
    qDebug() << QString("refMarksSize= %1\n").arg(fitsd->refMarks->marks.size());
    int resAuto = fitsd->identAuto(fitsd->refMarks, fitsd->instr->rang, targNum, identType, maxNum);

    if(resAuto)
    {
        qDebug() << "\nautoident fail\n";
        //QMessageBox::information(0, "resAuto", "autoident fail");
        QDir().remove(wcsLockFile);
        return 3;
    }
    qDebug() << "\nwcsSpinBox\n";
    fitsd->sigmaN = wcsParams.sigma;



    if(fitsd->detWCS1(wcsParams))
    {
        //slotSetCurFrame(imgLabel->frameIndex);
        //mGr->clearMarks();
//		mGrI->marks.clear();
//		slotMarks();
        fitsd->detTan();
 //   mGr->marks.clear();
 //   mGrI->marks.clear();
        fitsd->saveWCSFile(wcsFileName);//slotSaveWCS();

    }
else qDebug() << QString("detWCS fail\nSx = %1 mas\tSy = %2 mas").arg(grad_to_mas(fitsd->Sx)).arg(grad_to_mas(fitsd->Sy));

    //QDir(platePath).remove(wcsLockFile);
    QDir().remove(wcsLockFile);

    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);

    return 0;
}
