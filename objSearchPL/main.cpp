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

int findIndex(int *tInd, int dLev, int expNum, QVector <int*> *eIndex);
int proofInd(int *tInd, int expNum, QVector <int*> *eIndex);


int main(int argc, char *argv[]) //objSearchPL cfg.ini
{
    //QCoreApplication a(argc, argv);
    qInstallMsgHandler(customMessageHandler);
    QString msgstr, pnStr;
    QCoreApplication app(argc, argv);
    QTextStream stream(stdout);
///////// 1. Reading settings ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QTextCodec *codec1 = QTextCodec::codecForName("Windows-1251");
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

     qDebug() << "started....\n";
     int currentCat, isMove2corner, i, sz, resRed;
     int j, k;
     double maxObjDisp;
     catFinder *starCat;
     QList <catFinder*> starCatList;
     QString obsCode;
     QString uTime;
     //BEGIN settings
     QSettings *sett = new QSettings(cfgFile, QSettings::IniFormat);

     QStringList allSett;
     allSett << sett->allKeys();
     sz = allSett.size();
     qDebug() << QString("\n\nconf.ini:\n");
     for(i=0; i<sz; i++) qDebug() << QString("%1=%2\n").arg(allSett.at(i)).arg(sett->value(allSett.at(i)).toString());
     qDebug() << "\n\n";

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

     int useUtCorr = sett->value("general/useUtCorr", 0).toInt();

     QString resFolder = sett->value("general/resFolder", "./resDefault").toString();
     QDir resDir("./");
     if(resDir.mkpath(resFolder))qDebug() << "\nresFolder created\n";
     else qDebug() << "\nresFolder don't create\n";
     int isRemLog = sett->value("general/isRemLog", 0).toInt();

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
     starCat->catMagName = sett->value("ucac2/catMagName").toString();
     starCat->catPath = sett->value("ucac2/catPath").toString();
     starCatList << starCat;
     //
     starCat = new catFinder;
     starCat->exeName = sett->value("usnob/exeName").toString();
     starCat->exePath = sett->value("usnob/exePath").toString();
     starCat->catType = sett->value("usnob/catType").toInt();
     starCat->catName = sett->value("usnob/catName").toString();
     starCat->catMagName = sett->value("usnob/catMagName").toString();
     starCat->catPath = sett->value("usnob/catPath").toString();
     starCatList << starCat;
     //
     starCat = new catFinder;
     starCat->exeName = sett->value("ucac3/exeName").toString();
     starCat->exePath = sett->value("ucac3/exePath").toString();
     starCat->catType = sett->value("ucac3/catType").toInt();
     starCat->catName = sett->value("ucac3/catName").toString();
     starCat->catMagName = sett->value("ucac3/catMagName").toString();
     starCat->catPath = sett->value("ucac3/catPath").toString();
     starCatList << starCat;
     //
     starCat = new catFinder;
     starCat->exeName = sett->value("lspm/exeName").toString();
     starCat->exePath = sett->value("lspm/exePath").toString();
     starCat->catType = sett->value("lspm/catType").toInt();
     starCat->catName = sett->value("lspm/catName").toString();
     starCat->catMagName = sett->value("lspm/catMagName").toString();
     starCat->catPath = sett->value("lspm/catPath").toString();
     starCatList << starCat;
     //
     starCat = new catFinder;
     starCat->exeName = sett->value("lspmFind/exeName").toString();
     starCat->exePath = sett->value("lspmFind/exePath").toString();
     starCat->catType = sett->value("lspmFind/catType").toInt();
     starCat->catName = sett->value("lspmFind/catName").toString();
     starCat->catMagName = sett->value("lspmFind/catMagName").toString();
     starCat->catPath = sett->value("lspmFind/catPath").toString();
     starCatList << starCat;
     //
     qDebug() << QString("starCatList count: %1\n").arg(starCatList.count());
     QString ast_eph_prog = sett->value("processes/ast_eph_prog", "./mpeph.exe").toString();
     QString ast_eph_prog_folder = sett->value("processes/ast_eph_prog_folder", "./").toString();
     QString skybot_prog = sett->value("processes/skybot_prog", "./skybotclient.exe").toString();
     QString skybot_prog_folder = sett->value("processes/skybot_prog_folder", "./").toString();
     QString get_http_prog = sett->value("processes/get_http_prog", "./getHttpHeader.exe").toString();
     QString get_http_prog_folder = sett->value("processes/get_http_prog_folder", "./").toString();
     QString utCorr_prog = sett->value("processes/utCorr_prog", "./uTimeCorr.exe").toString();
     QString utCorr_prog_folder = sett->value("processes/utCorr_prog_folder", "./").toString();



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

 //vfCorr0
     int isVFcorr0 = sett->value("vfCorr0/isVFcorr", 0).toInt();
     int isBC0 = sett->value("vfCorr0/isBC", 0).toInt();
     QString vfCorrFile0 = sett->value("vfCorr0/vfCorrFile", "vf.txt").toString();
     double rMax0 = sett->value("vfCorr0/rMax", 0).toDouble();
     int nmin0 = sett->value("vfCorr0/nmin", 0).toInt();

 //vfCorr1
     int isVFcorr1 = sett->value("vfCorr1/isVFcorr", 0).toInt();
     int isBC1 = sett->value("vfCorr1/isBC", 0).toInt();
     QString vfCorrFile1 = sett->value("vfCorr1/vfCorrFile", "vf.txt").toString();
     double rMax1 = sett->value("vfCorr1/rMax", 0).toDouble();
     int nmin1 = sett->value("vfCorr1/nmin", 0).toInt();
 //mdCorr
     mdCorr mdcParam;
     int isMdCorr = sett->value("mdCorr/isMdCorr", 0).toInt();
     mdcParam.colSep = sett->value("mdCorr/colSep",  "|").toString();
     mdcParam.mdCorrX = sett->value("mdCorr/mdCorrX", "./mdCorrX.txt").toString();
     mdcParam.mdCorrY = sett->value("mdCorr/mdCorrY", "./mdCorrY.txt").toString();
     mdcParam.corrModel = sett->value("mdCorr/corrModel", 0).toInt();

     observatory *obsList = new observatory;
     obsList->init(observatoryCat.toAscii().data(), OBS_SIZE);

 //fitsi
     QList <fitsdata*> fitsList;

     fitsdata *fitsd;
//
     fitsd = new fitsdata();
     fitsd->fileName = fileName;

     fitsd->initInst(telescopeFile, insNum);

     obsList->getobsynumO(obsCode.toAscii().data());
     fitsd->initObsPos(obsList->record);

     if(isMagEqCorr) fitsd->initMagEqXDX(xCoefFileName, yCoefFileName, dataSep);
     if(isVFcorr0) fitsd->initVFcorr0(vfCorrFile0, rMax0, nmin0, isBC0);
     if(isVFcorr1) fitsd->initVFcorr1(vfCorrFile1, rMax1, nmin1, isBC1);
     if(isMdCorr) fitsd->initMdCorr(mdcParam);

     QString descS, oName;
     int oNum;

     QByteArray bpn;

     int doWhat;
     if(argc==5) doWhat = atoi(argv[3]);
     else doWhat = 0;

 //получить заголовок пластинки
     if(doWhat)   //0-http; 1-file
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

     if(isRedRef) fitsd->initRefractParam();

     if(useUtCorr)
     {
         QProcess outerProcess;
         QStringList outerArguments;
         outerArguments.clear();
//           QString ucac2find_folder("");

         outerProcess.setWorkingDirectory(utCorr_prog_folder);
         outerProcess.setProcessChannelMode(QProcess::MergedChannels);
         outerProcess.setReadChannel(QProcess::StandardOutput);
         //outerArguments << instruments->curName.toLower() << pnStr;
         qDebug() << QString("argc= %1\n").arg(argc);
         outerArguments  << QString("%1").arg(doWhat) << pnStr;
         qDebug() << utCorr_prog << outerArguments.join(" ");
         outerProcess.start(utCorr_prog, outerArguments);

         outerProcess.waitForFinished(-1);
         QTextStream catStream(outerProcess.readAllStandardOutput());

         uTime = catStream.readAll().section("\n", -1, -1);
     }
     else
     {
         fitsd->headList.getKeyName("U", &uTime);

     }

     qDebug() << QString("uTimeCorr: %1\n").arg(uTime.trimmed().toDouble());
     double uTimeValue = uTime.trimmed().toDouble()/86400.0;
     fitsd->MJD += uTimeValue;

     fitsd->marksG->clearMarks();
     fitsd->getMarksGrid(starCatList.at(currentCat), fitsd->instr->fov, mag0, mag1, -1);
     fitsd->detTan();

     fitsd->marksGIpix->loadTanImg(fileName, mSep, mColumn);
     if(isMove2corner) fitsd->marksGIpix->moveToCorner(fitsd->naxes);

     fitsd->mpeWaitTime = mpeWaitTime;
     fitsd->sbWaitTime = sbWaitTime;

     expNum = fitsd->expList->exps.size();
     /*if(lspmFind)
     {
         if(!(starCatList.size()<5)) fitsd->findLspmCat(starCatList[4]->exeName, starCatList[4]->exePath, starCatList[4]->catPath, fitsd->instr->fov, magObj0, magObj1);
     }*/
     for(j=0;j<expNum;j++)
     {
         fitsd->setExposure(j);
         fitsd->MJD += uTimeValue;
         /*
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
         {*/
             if(!fitsd->headList.getKeyName("OBJECT", &descS))
             {
                 desc2NumName(descS, &oNum, &oName);
                 if(mpephType) fitsd->getMpephNum(QString("%1").arg(oNum), ast_eph_prog, ast_eph_prog_folder, magObj0, magObj1);
                 else fitsd->getMpephName(oName, ast_eph_prog, ast_eph_prog_folder, magObj0, magObj1);
             }
         //}
     }





     QString fileNameTemp;
     QString fileNameBase;
     QString wcsFileNameT;
     fileNameBase = fileName.left(fileName.lastIndexOf(".aver"));
     qDebug() << QString("fileNameBase: %1\n").arg(fileNameBase);
     fitsdata *fitsT;

     int varNum = expNum*expNum;

     double *dMin = new double[varNum];

     double dist, distMin;//, distMax;
     int pos, posMin;
     int numI;

     for(i=0; i<expNum; i++)
     {
         fileNameTemp = QString("%1.%2").arg(fileNameBase).arg(i, 3, 10, QLatin1Char( '0' ));
         qDebug() << QString("fileNameTemp: %1\n").arg(fileNameTemp);
         wcsFileNameT = fileNameTemp + ".wcs";
         qDebug() << QString("wcsFileNameT: %1\n").arg(wcsFileNameT);
         fitsT = new fitsdata;
         fitsT->readHeader(fitsd->fitsHeader.split("\n"));
         fitsT->fileName = fitsd->fileName;

         if(fitsT->loadWCSFile(wcsFileNameT))
         {
             qDebug() << QString("\nloadWCSFile %1 error\n").arg(wcsFileNameT);
             QDir(filePath).remove(logFileName);
             return 1;
         }
         //fitst->MJD +=

         if(isComaCorr) fitsT->initComaCorr(cKsi, cEta, cMag0);

         if(isRedRef) fitsT->initRefractParam();

         fitsT->initInst(telescopeFile, insNum);

         //obsList->getobsynumO(obsCode.toAscii().data());
         fitsT->initObsPos(obsList->record);

         if(isMagEqCorr) fitsT->initMagEqXDX(xCoefFileName, yCoefFileName, dataSep);
         if(isVFcorr0) fitsT->initVFcorr0(vfCorrFile0, rMax0, nmin0, isBC0);
         if(isVFcorr1) fitsT->initVFcorr1(vfCorrFile1, rMax1, nmin1, isBC1);
         if(isMdCorr) fitsd->initMdCorr(mdcParam);

         fitsT->marksGIpix->loadTanImg(fileNameBase, mSep, mColumn);
         if(isMove2corner) fitsT->marksGIpix->moveToCorner(fitsd->naxes);

         fitsT->mpeWaitTime = mpeWaitTime;
         fitsT->sbWaitTime = sbWaitTime;
         /*if(lspmFind)
         {
             if(!(starCatList.size()<5)) fitsd->findLspmCat(starCatList[4]->exeName, starCatList[4]->exePath, starCatList[4]->catPath, fitsd->instr->fov, magObj0, magObj1);
         }*/

         fitsT->objMarks->marks << fitsd->objMarks->marks;
         fitsT->detTanObj();

         numI = fitsT->marksGIpix->marks.size();
         for(j=0;j<expNum;j++)
         {
             distMin = fitsd->naxes[0];
             posMin = -1;
             qDebug() << QString("objPos[%1]:\t%2\t%3\n").arg(j).arg(fitsT->objMarks->marks.at(j)->mTanImg[0]).arg(fitsT->objMarks->marks.at(j)->mTanImg[1]);
             for(k=0;k<numI;k++)
             {
                 dist = sqrt(pow(fitsT->objMarks->marks.at(j)->mTanImg[0] - fitsT->marksGIpix->marks.at(k)->mTanImg[0], 2.0) + pow(fitsT->objMarks->marks.at(j)->mTanImg[1] - fitsT->marksGIpix->marks.at(k)->mTanImg[1], 2.0));
                 if(dist<distMin)
                 {
                     distMin = dist;
                     posMin = k;
                 }
             }
             qDebug() << QString("imgMin[%1]:\t%2\t%3\n").arg(posMin).arg(fitsT->marksGIpix->marks.at(posMin)->mTanImg[0]).arg(fitsT->marksGIpix->marks.at(posMin)->mTanImg[1]);
             dMin[i*expNum+j] = distMin;

         }

         fitsList << fitsT;
     }

     for(i=0; i<varNum; i++) qDebug() << QString("dMin[%1]= %2\n").arg(i).arg(dMin[i]);

     QVector <int*> eIndex;

     findIndex(NULL, 0, expNum, &eIndex);

     int iSize = eIndex.size();
     QString rStr;
     double *summExp = new double[iSize];
     //QList <double*> summList;

     double summMin = 1000;
     int sMinPos = -1;

     for(i=0; i<iSize; i++)
     {
         rStr.clear();
         rStr.append(QString("eIndex %1:\t").arg(i));
         summExp[i] = 0.0;

         for(j=0; j<expNum; j++)
         {
             qDebug() << QString("dMin[%1]= %2\n").arg(j*expNum+eIndex[i][j]).arg(dMin[j*expNum+eIndex[i][j]]);
             summExp[i] += dMin[j*expNum+eIndex[i][j]];
             rStr.append(QString("%1\t").arg(eIndex[i][j]));
         }
         rStr.append(QString("summ= %1\n").arg(summExp[i]));

         if(summExp[i]<summMin)
         {
             summMin = summExp[i];
             sMinPos = i;
         }

         qDebug() << rStr;
         //qDebug() << QStrin
     }

     rStr.clear();
     rStr.append(QString("min eIndex %1:\t").arg(sMinPos));
     for(j=0; j<expNum; j++) rStr.append(QString("%1\t").arg(eIndex[sMinPos][j]));
     rStr.append(QString("summ= %1\n").arg(summExp[sMinPos]));

     qDebug() << rStr;
     int objSz;

     for(i=0; i<expNum; i++)
     {
         fitsT = fitsList[i];
         objSz = fitsT->objMarks->marks.size();
         fitsT->setExposure(eIndex[sMinPos][i]);
         fitsT->MJD += uTimeValue;
         for(j=objSz-1; j>=0; j--)
         {
             oNum = j - oNum*(j/expNum);
             qDebug() << QString("i= %1\tj= %2\toNum= %3\n").arg(i).arg(j).arg(oNum);
             if(oNum!=eIndex[sMinPos][i])
             {
                 qDebug() << QString("removeAt %1\n").arg(j);
                 fitsT->objMarks->marks.removeAt(j);
             }
         }
         //редукция
         fitsT->detTanObj();
         fitsT->marksG->marks << fitsd->marksG->marks;
         fitsT->detTan();

         resRed = fitsT->makeReduction1(ruler3File, resFolder);
     }


     //find

     //for(i=0; i<varNum; i++) qDebug() << QString("index[%1]= %2\n").arg(i).arg(i, expNum, expNum, QLatin1Char( '0' ) );

     //findIndex(expNum, &eIndex);




    return 0;
    //return a.exec();
}

int findIndex(int *tInd, int dLev, int expNum, QVector <int*> *eIndex)
{
    qDebug() << QString("findIndex: dLev= %1\teIndex size: %2\n").arg(dLev).arg(eIndex->size());
    int i, j, nt, pi;
    if(dLev==0) tInd = new int[expNum];
    int iSize;// = eIndex->size();
    //if(iSize<expNum)
    //{
    iSize = 0;
    int *nInd;
    QString rStr;
    //int tInd;
    for(i=0; i<expNum; i++)
    {

        nt = 1;
        //if(findIndex(expNum, eIndex))
        for(j=0; j<dLev; j++)if(i==tInd[j])
        {
            nt = 0;
            break;
        }
        if(nt)
        {
            dLev;
            tInd[dLev] = i;
            if(dLev==expNum-1)
            {
                pi = proofInd(tInd, expNum, eIndex);
                if(!pi)
                {
                    nInd = new int[expNum];
                    rStr.clear();
                    rStr.append(QString("%1: ").arg(eIndex->size()));
                    for(j=0; j<expNum; j++)
                    {
                        nInd[j] = tInd[j];
                        rStr.append(QString("%1 ").arg(tInd[j]));
                    }

                    eIndex->append(nInd);

                    qDebug() << rStr << "\n";

                }
                //dLev--;
            }
            else findIndex(tInd, dLev+1, expNum, eIndex);
            //eIndex->append(i);
            //break;
        }
    }
    //}
    //else

    //iSize = eIndex->size();
    //if(iSize<expNum) findIndex(expNum, eIndex);

}

int proofInd(int *tInd, int expNum, QVector <int*> *eIndex)
{
    int i, j, res;
    int iSize = eIndex->size();
    for(i=0; i<iSize; i++)
    {
        res = 1;
        for(j=0; j<expNum; j++)
        {
            res *= (int)(tInd[j]==eIndex->at(i)[j]);
        }
        if(res) return 1;
    }
    return 0;
}
