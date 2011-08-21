#include <QCoreApplication>
#include <QtCore>
/*
#include <QTextStream>
#include <QDataStream>
#include <QFile>
*/
#include "./../libs/redStat.h"
#include "./../libs/observatory.h"
#include "./../libs/vectGrid3D.h"
#include "./../libs/fitsdata.h"

//#include <mgl/mgl_eps.h>

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

void mCoefFunc(QString coefFileName, QString mCoefFileName, QString dataSep, int coefDeg);


void task0func();

struct diapResiduals
{
    int diapsNum;
    double *diaps;
    QList <residualFile*> resListDiap;
    void initMesList(QList <measurementRec*> mesList);
//    vectGrid3D *vectF;
};

struct diapU3
{
    int diapsNum;
    double *diaps;
    QList <marksGrid*> mgList;
//    vectGrid3D *vectF;
};

//int detCurf3(double *Zi, double *x, double *y, int num, int deg);
//int detCurf3(QString *resFunc, double *Zi, double *x, double *y, int num, int deg);

//void summNum(double *x, double *dx, int num, int sNum);

//double polyY(double *Zi, double x, int deg);

struct report3data
{
/*    double x0, x1, y0, y1;
    int cx, cy;
    QString labelX, labelY, plotParam;*/
    int isDetCurv, curvDeg;
    int aveNum;
    QString colSep;
    int mDegree;
    int doMcoef;
};

struct report4data
{
    double x0, x1, y0, y1;  //диапазоны
    int xLevNum, yLevNum;             //кол-во клеток по осям
    int isFindDiap;
//    QString labelX, labelY, plotParam;
//    int isDetCurv, curvDeg;
    int isSysCorr;
    int doWeght;
    QString sysCorrFile;
    int nmin;
    double rMax;
};



int main(int argc, char *argv[])    //r3Stat.exe errBudger.txt residuals.txt aggregate_eq.txt
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);
    QTextStream dataStream, dataStream1;
    //QFile rFile(argv[1]);
    //rFile.open(QIODevice::ReadOnly| QIODevice::Text);
    //dataStream.setDevice(&rFile);

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);

    diapResiduals magEq;
    reductionStat rStat;
    platesStat plStat;
    QList <measurementRec*> mesList;
    platesStat plStatMPC;


    double s, Fi, Long, MJD;
    double raOc, deOc;
    double gm1, t;
    double ksiZ, etaZ;
    double s1, s2, cosP, sZ;
    double ksi, eta, ksiOC, etaOC;
    double *r, *dr;
    double P1, P2, P3;
    double Az, Zed;
    double ra0, dec0;

    double dKsi, dEta, dDec;

    QString dStr;
    residualsRec *resRec;
    errBudgetRec ebRec;

    double *mLevels;
    double *xLevels;
    double *yLevels;
    double *decLevels;

    multidim *xVectField;
    multidim *xDispVectField;
    multidim *yVectField;
    multidim *yDispVectField;
    multidim *vectNumField;
    int axesNum;
    int *axes;
    int *pos;
    QString dataLine;
    QFile *resDataF;

    int mLevNum, xLevNum, yLevNum, tLevNum;
    double mLevNumBeg, mLevStep;
//	int taskNum;
    int numTot;

    double naxeX[2];
    double naxeY[2];

    vectGrid3D *vectF;
    QByteArray tmp;

    double dX, dY;
    double axd;
    int diapNum;
    int diniKey;
    int rNum, di;
    long ni;
    long sz;

    double *vect;

    QString report5Dir;
    QString report4Dir;
    QString report6Dir;

    QString tStr;

    int pMin;
    QTextCodec *codec1 = QTextCodec::codecForName("KOI8-R");
    Q_ASSERT( codec1 );

    double meanKsi, meanEta, rmsKsi, rmsEta;
    int recNum = 0;
    int i;
    meanKsi = meanEta = rmsKsi = rmsEta = 0.0;

    QSettings *sett = new QSettings("r3Stat.ini", QSettings::IniFormat);
    QString workDir = sett->value("general/workDir", ".").toString();
    //int taskNum = sett->value("general/taskNum", 0).toInt();
    int redType = sett->value("general/redType", 0).toInt();            //0 - common; 1 - obj

    int plNameType = sett->value("general/plNameType", 0).toInt();

    QString obsCat = sett->value("general/observCat", ".").toString();


    int isSysCorr = sett->value("general/isSysCorr", 0).toInt();
    QString sysCorrFile = sett->value("general/sysCorrFile").toString();
    int isFindDiap = sett->value("diaps/isFindDiap").toInt();
    double expMin = sett->value("general/expmin", 180).toDouble();

    int isUsePlatesList = sett->value("general/isUsePlatesList").toInt();
    QString plFName = sett->value("general/plFName", "./plates.txt").toString();

    int isReport0 = sett->value("reports/isReport0", 1).toInt();
    int isReport1 = sett->value("reports/isReport1", 1).toInt();
    int isReport2 = sett->value("reports/isReport2", 1).toInt();
    int isReport3 = sett->value("reports/isReport3", 1).toInt();
    int isReport4 = sett->value("reports/isReport4", 1).toInt();
    int isReport5 = sett->value("reports/isReport5", 1).toInt();
    int isReport6 = sett->value("reports/isReport6", 1).toInt();

    int isMeasurementsTxt = sett->value("reports/isMeasurementsTxt", 1).toInt();
    int isMesReports = sett->value("reports/isMesReports", 1).toInt();

    int r1aveNum = sett->value("report1/aveNum", 1).toInt();


    report3data r3data;
    r3data.isDetCurv = sett->value("report3/isDetCurv", 0).toInt();
    r3data.curvDeg = sett->value("report3/curvDeg", 2).toInt();
    r3data.aveNum = sett->value("report3/aveNum", 2).toInt();
    qDebug() << QString("aveNum= %1\n").arg(r3data.aveNum);
    r3data.colSep = sett->value("report3/colSep").toString();
    r3data.mDegree = sett->value("report3/mDegree", 2).toInt();
    r3data.doMcoef = sett->value("report3/doMcoef", 0).toInt();

    report4data r4data;
    r4data.isFindDiap = sett->value("report4/isFindDiap").toInt();
    r4data.x0 = sett->value("report4/naxeX0", 0).toDouble();
    r4data.x1 = sett->value("report4/naxeX1", 0).toDouble();
    r4data.y0 = sett->value("report4/naxeY0", 0).toDouble();
    r4data.y1 = sett->value("report4/naxeY1", 0).toDouble();
    r4data.xLevNum = sett->value("report4/xLevNum", 1).toDouble();
    r4data.yLevNum = sett->value("report4/yLevNum", 1).toDouble();
    r4data.isSysCorr = sett->value("report4/isSysCorr", 0).toInt();
    r4data.sysCorrFile = sett->value("report4/sysCorrFile").toString();
    r4data.doWeght = sett->value("report4/doWeght", 0).toInt();
    r4data.nmin = sett->value("report4/nmin", 0).toInt();
    r4data.rMax = sett->value("report4/rMax", 0).toDouble();

    report4data r5data;
    r5data.isFindDiap = sett->value("report5/isFindDiap").toInt();
    r5data.x0 = sett->value("report5/naxeX0", 0).toDouble();
    r5data.x1 = sett->value("report5/naxeX1", 0).toDouble();
    r5data.y0 = sett->value("report5/naxeY0", 0).toDouble();
    r5data.y1 = sett->value("report5/naxeY1", 0).toDouble();
    r5data.xLevNum = sett->value("report5/xLevNum", 1).toDouble();
    r5data.yLevNum = sett->value("report5/yLevNum", 1).toDouble();
    r5data.isSysCorr = sett->value("report5/isSysCorr", 0).toInt();
    r5data.sysCorrFile = sett->value("report5/sysCorrFile").toString();
    r5data.doWeght = sett->value("report5/doWeght", 0).toInt();
    r5data.nmin = sett->value("report5/nmin", 0).toInt();
    r5data.rMax = sett->value("report5/rMax", 0).toDouble();

    //do3sigma
    int d3s0 = sett->value("do3sigma/step0", 0).toInt();
    int d3s1 = sett->value("do3sigma/step1", 0).toInt();
    int d3s2 = sett->value("do3sigma/step2", 0).toInt();
    int d3s3 = sett->value("do3sigma/step3", 0).toInt();
    int d3s4 = sett->value("do3sigma/step4", 0).toInt();
    //int d3s5 = sett->value("do3sigma/step5", 0).toInt();


    QString diapStr = sett->value("diaps/magDiaps").toString();



    QString *ebFileName, *resFileName, *mpeFileName, *ssFileName;
    ebFileName = resFileName = mpeFileName = ssFileName = NULL;

    QString reportDirName = workDir+"/reports";
    QDir reportDir(workDir);
    reportDir.mkdir("reports");
    reportDir.cd("./reports");

    if(!redType)
    {
        ebFileName = new QString(workDir + "/err_budget.txt");
        resFileName = new QString(workDir + "/residuals.txt");
    }
    else
    {
        ebFileName = new QString(workDir + "/err_budget_obj.txt");
        resFileName = new QString(workDir + "/residuals_obj.txt");
        mpeFileName = new QString(workDir + "/aggregate_eq.txt");
        ssFileName = new QString(workDir + "/aggregate_sstar.txt");
    }

    qDebug() << QString("ebFileName: %1\n").arg(*ebFileName);

    //reductionStat rStat;
    rStat.init(ebFileName, resFileName, mpeFileName, ssFileName);

    qDebug() << QString("eb.size= %1\nres.size= %2\neq.size= %3\nss.size= %4\nmesurements.size= %5\n").arg(rStat.ebFile->errList.size()).arg(rStat.resFile->resList.size()).arg(rStat.mpeFile->ocList.size()).arg(rStat.ssFile->ocList.size()).arg(rStat.mesList.size());

    if(d3s0) rStat.mpeFile->do3sigma(0.95, 3);

    qDebug() << QString("\neq.size 3-sigma= %1\n").arg(rStat.mpeFile->ocList.size());


    QStringList mdList = diapStr.split("|");

    magEq.diapsNum = mLevNum = mdList.size();
    magEq.diaps = new double[magEq.diapsNum];

    residualFile* resFile;
    for(i=0; i<magEq.diapsNum-1; i++)
    {
        magEq.diaps[i] = mdList.at(i).toDouble();
        resFile = new residualFile;
        magEq.resListDiap << resFile;
    }

    magEq.diaps[magEq.diapsNum-1] = mdList.at(magEq.diapsNum-1).toInt();


    int szi, szj, szk;
    int j, k;

    measurementRec* sRec;
    //QTextStream dataStream;
    QFile r1File;

    QFile r3File;
    QTextStream dataStream3;
    QFile rFile;

    errBudgetFile *errB;

    r1File.setFileName(reportDirName+"/reportTot.txt");
    r1File.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    dataStream1.setDevice(&r1File);

    QString dataStr;
    QString plateName;

    measurementRec* mesRec;
    eqPlatesStat eqPStat;

    QString versName;
    //measurementStatRec* msRec;
    measurementStatRec* msRec = new measurementStatRec;

    if(isUsePlatesList) plStat.initPlatesNamesFile(plFName);
    qDebug() << QString("part plates size= %1\n").arg(plStat.platesList.size());
    plStat.init(rStat.statList, plNameType);
    qDebug() << QString("part plates size1= %1\n").arg(plStat.platesList.size());

    int szRes;
    double t0, t1;
    t1 = 0;
    t0 = 60000.0;
    int year, mth;
    double day, pyear;

//    int pk=0;
//do
//{
    //qDebug() << QString("ssFile->ocList.size= %1").arg(rStat.ssFile->ocList.size());


    switch(redType)
    {
    case 0:
        {

            plStat.saveReport0(reportDirName+"/report0.txt", &rStat, &mesList);

            magEq.initMesList(mesList);

        }
        break;
    case 1:
        {
            if(isUsePlatesList) plStatMPC.initPlatesNamesFile(plFName);
            qDebug() << QString("part plates size= %1\n").arg(plStatMPC.platesList.size());
            //plStatMPC.init(rStat.statList, plNameType);
            //qDebug() << QString("part plates size1= %1\n").arg(plStatMPC.platesList.size());
/*
            if(isReport0)
            {
                rFile.setFileName(reportDirName+"/report0mpc.txt");
                rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
                dataStream.setDevice(&rFile);
            }
*/
            eqFile *astStat = new eqFile;
            qDebug() << QString("0: ssFile->ocList.size= %1").arg(rStat.ssFile->ocList.size());
            eqPStat.init(&rStat, plNameType);
            qDebug() << QString("1: ssFile->ocList.size= %1").arg(rStat.ssFile->ocList.size());
            szi = eqPStat.asterList.size();
            qDebug() << QString("asterList.size= %1").arg(szi);
            for(i=0; i<szi; i++)
            {
                plStatMPC.init(eqPStat.asterList.at(i)->mStatList, plNameType);

            }

            plStatMPC.saveReport0(reportDirName+"/report0mpc.txt", &rStat, &mesList);

            magEq.initMesList(mesList);
/*
            szi = mesList.size();
            for(i=0; i<szi; i++)
            {
                mesRec = mesList.at(i);
                for(k=0; k<mesRec->mpeList.size(); k++) astStat->ocList << mesRec->mpeList.at(k);
            }


            astStat->countCols("4,5,6");
            astStat->saveAs(reportDirName+"/agg_eq.txt");
*/

            //r1File.setFileName(reportDirName+"/eqSeries.txt");
            //r1File.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
            //dataStream1.setDevice(&r1File);

            //eqSeriesRec *eqsR = new eqSeriesRec;

            //ocRec* ocR = new ocRec;

            reportDir.mkdir("eqSeries");

            eqFile *eqTemp = new eqFile;
            eqFile *eqTempS = new eqFile;
            eqFile *eqTotStemp = new eqFile;
            eqFile *eqTotS = new eqFile;
            eqFile *eqAgg = new eqFile;
            QList <eqFile*> eqList;

            eqTotS->clear();
            szi = eqPStat.asterList.size();
            qDebug() << QString("aster num: %1\n").arg(szi);
            for(i=0; i<szi; i++)
            {
                qDebug() << QString("asterName: %1\n").arg(eqPStat.asterList.at(i)->asterName);
                eqTemp->clear();
                eqTotStemp->ocList.clear();
                szj = eqPStat.asterList.at(i)->plStat->platesList.size();
                qDebug() << QString("aster mesurements num: %1\n").arg(szj);
                for(j=0; j<szj; j++)
                {
                    eqPStat.asterList.at(i)->plStat->platesList.at(j)->getMinUWE(msRec);
                    qDebug() << "msRec: " << msRec << "\n";
                    qDebug() << "msRec timeCode: " << msRec->mesureTimeCode << "\n";
                    mesRec = rStat.getMeasurement(msRec->mesureTimeCode);
                    qDebug() << "mesRec: " << mesRec << "\n";
                    if(mesRec!=NULL) eqTemp->ocList << mesRec->mpeList;
                }
                qDebug() << QString("eqTemp size: %1\n").arg(eqTemp->ocList.size());
                if(d3s1) eqTemp->do3sigma(0.05, 3.0);
                qDebug() << QString("eqTemp do3sigma size: %1\n").arg(eqTemp->ocList.size());
                eqAgg->ocList << eqTemp->ocList;
                eqList.clear();
                eqTemp->findSeries(&eqList);
                szj = eqList.size();
                qDebug() << QString("eq series size: %1\n").arg(szj);
                eqTempS->clear();
                for(j=0; j<szj; j++)
                {
                    qDebug() << QString("serie %1: %2 obs\n").arg(j).arg(eqList.at(j)->ocList.size());
                    if(d3s2) eqList.at(j)->do3sigma(0.05, 3.0);
                    //eqList.at(j)->countCols("0,1,2,3,4,5,6");
                    //eqList.at(j)->countMM();
                     qDebug() << QString("do3sigma size: %1\n").arg(eqList.at(j)->ocList.size());
                     ocRec* ocR = new ocRec;
                    eqList.at(j)->getSeriesRec(ocR);

                    eqTempS->ocList << ocR;

                    //eqsR->rec2s(&tStr);
                   //dataStream1 << QString("%1").arg(tStr);
                }
                if(d3s3) eqTempS->do3sigma(0.05, 3.0);
                eqTempS->countCols("4,5,6");
                eqTempS->saveAs(QString("%1/eqSeries/%2_eqs.txt").arg(reportDirName).arg(eqPStat.asterList.at(i)->asterName.simplified()), 2);

                eqTotS->ocList << eqTempS->ocList;
            }

            if(d3s4) eqTotS->do3sigma(0.05, 3.0);

            eqTotS->countCols("4,5,6");
            eqTotS->saveAs(reportDirName+"/eqSeries.txt", 0);

            eqAgg->countCols("4,5,6");
            eqAgg->saveAs(reportDirName+"/agg_eq.txt", 0);


            //r1File.close();










            qDebug() << "\nsstarFile\n";
            if(isReport0)
            {
                rFile.close();
                rFile.setFileName(reportDirName+"/report0sstar.txt");
                rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
                dataStream.setDevice(&rFile);
            }
            sstarFile *ssFile = new sstarFile;
            ssPlatesStat ssPlStat;
            qDebug() << QString("ssFile->ocList.size= %1").arg(rStat.ssFile->ocList.size());
            ssPlStat.init(&rStat);

            szi = ssPlStat.ssList.size();
            qDebug() << QString("asterList.size= %1").arg(szi);
            for(i=0; i<szi; i++)
            {
                szj = ssPlStat.ssList.at(i)->plStat->platesList.size();
                qDebug() << QString("platesList.size= %1").arg(szj);
                for(j=0; j<szj; j++)
                {
                    //ssPlStat.ssList.at(i)->plStat->platesList.at(j)->getMinUWE(msRec);
                    pMin = ssPlStat.ssList.at(i)->plStat->platesList.at(j)->getMinUWEpos();
                    if(pMin!=-1) msRec = ssPlStat.ssList.at(i)->plStat->platesList.at(j)->mStatList.at(pMin);
                    qDebug() << "msRec: " << msRec << "\n";
                    if(msRec!=NULL)
                    {
                        qDebug() << QString("msRec mesureTimeCode: %1\n").arg(msRec->mesureTimeCode);
                        mesRec = rStat.getMeasurement(msRec->mesureTimeCode);


                        //eqPStat.asterList.at(i)->plStat->platesList.at(j)->getMinUWE(msRec);
                        qDebug() << "msRec: " << msRec << "\n";
                        qDebug() << QString("msRec mesureTimeCode: %1\n").arg(msRec->mesureTimeCode);
                        mesRec = rStat.getMeasurement(msRec->mesureTimeCode);
                        mesList << mesRec;
                        //for(k=0; k<mesRec->mpeList.size(); k++) astStat->ocList << mesRec->mpeList.at(k);
                        ssFile->ocList << mesRec->sstarList;
                        msRec->getVersName(&versName);

                        dat2YMD(mjd2jd(mesRec->errBud->MJD), &year, &mth, &day);
                        pyear = partOfYear(year, mth, day);

                        dataStr = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12").arg(ssPlStat.ssList.at(i)->plStat->platesList.at(j)->plateName).arg(pyear, 7, 'f', 2).arg((int)mesRec->errBud->MJD).arg(versName).arg(msRec->Nx).arg(msRec->UWEx).arg(msRec->Ny).arg(msRec->UWEy).arg(msRec->rmsOneOCksi).arg(msRec->rmsOneOCeta).arg(msRec->rmsMeanOCksi).arg(msRec->rmsMeanOCeta);

                        szk = mesRec->mpeList.size();
                        for(k=0; k<szk; k++)dataStr.append(QString("|%1|%2|%3").arg(mesRec->mpeList.at(k)->ocRaCosDe).arg(mesRec->mpeList.at(k)->ocDe).arg(mesRec->mpeList.at(k)->name.simplified()));

                        szk = mesRec->sstarList.size();
                        for(k=0; k<szk; k++)dataStr.append(QString("|%1|%2|%3").arg(mesRec->sstarList.at(k)->ksiOC).arg(mesRec->sstarList.at(k)->etaOC).arg(mesRec->sstarList.at(k)->lspmName.simplified()));
                        dataStr.append(QString("|%1").arg(msRec->originName));
                    }
                    else
                    {
                        dataStr = QString("%1").arg(ssPlStat.ssList.at(i)->plStat->platesList.at(j)->plateName);

                    }
                    if(isReport0) dataStream << dataStr << "\n";



                    //mesList << mesRec;

                }
            }
            ssFile->countCols("6,7");
            ssFile->saveAs(reportDirName+"/agg_sstar.txt");

        }
        break;
    }
    if(isReport0) rFile.close();
    r1File.close();


/*

//mesList

    if(isMeasurementsTxt)
    {
        rFile.setFileName(reportDirName+"/measurements.txt");
        rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
        dataStream.setDevice(&rFile);
    }

    if(isMesReports)
    {
        reportDir.remove("err_budget.txt");
        reportDir.remove("residuals.txt");
        reportDir.remove("err_budget_obj.txt");
        reportDir.remove("residuals_obj.txt");

        resFile = new residualFile;
        tStr = QString(reportDirName+"/residuals.txt");
        resFile->init(tStr.toAscii().data());

        tStr = QString(reportDirName+"/err_budget.txt");
        errB = new errBudgetFile;
        errB->init(tStr.toAscii().data());
       /*
        r1File.setFileName(reportDirName+"/err_budget.txt");
        r1File.open(QIODevice::WriteOnly  |  QIODevice::Truncate);
        r1File.close();

        r1File.setFileName(reportDirName+"/resi.txt");
        r1File.open(QIODevice::WriteOnly  |  QIODevice::Truncate);
        r1File.close();/
    }

    szi = mesList.size();

//residualFile *resFile;



    for(i=0; i<szi; i++)
    {
        mesRec = mesList[i];
        mesRec->errBud->rec2s(&dataStr);
        dataStream << QString("#%1\n").arg(dataStr);

        if(isMesReports)
        {
            resFile->resList << mesRec->resList;

            errB->errList << mesRec->errBud;
        }

        if(isMeasurementsTxt)
        {
            szj = mesRec->resList.size();
            for(j=0; j<szj; j++)
            {
                mesRec->resList[j]->rec2s(&dataStr);
                dataStream << QString("%1").arg(dataStr);
            }
            szj = mesRec->mpeList.size();
            for(j=0; j<szj; j++)
            {
                mesRec->mpeList[j]->rec2s(&dataStr);
                dataStream << QString("%1").arg(dataStr);
            }
            szj = mesRec->sstarList.size();
            for(j=0; j<szj; j++)
            {
                mesRec->sstarList[j]->rec2s(&dataStr);
                dataStream << QString("%1").arg(dataStr);
            }
        }
    }
    if(isMesReports)
    {
        resFile->save();
        errB->save();
    }


    if(isMeasurementsTxt) rFile.close();

/////////

    QFile r2FileXcoef;
    QTextStream dataStream2Xcoef;
    QString r3NameXcoef;

    QFile r2FileYcoef;
    QTextStream dataStream2Ycoef;
    QString r3NameYcoef;

    QFile r2FileMcoef;
    QTextStream dataStream2Mcoef;
    QString r3NameMcoef;

    rFile.setFileName(reportDirName+"/mgEq.txt");
    rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStream.setDevice(&rFile);

    r1File.setFileName(reportDirName+"/ocMagDisp.txt");
    r1File.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStream1.setDevice(&r1File);

    r3File.setFileName(reportDirName+"/magOC.txt");
    r3File.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStream3.setDevice(&r3File);




    QFile r2FileX;
    QTextStream dataStream2X;
    QString r3NameX;

    QFile r2FileY;
    QTextStream dataStream2Y;
    QString r3NameY;

    QString resFunc;

    //dataStream << "#" << diapStr << "\n";
    //dataStream << QString("#%1|%2\n").arg(t0).arg(t1);

    QString report3Dir = reportDirName + "/report3";
    reportDir.mkdir("report3");

    double *x, *y, *mX, *mY;
    double *dx, *dy, *dmX, *dmY;
    double val;
    int p;
    int dotNum;
    QStringList dataStrL;



    r3NameXcoef = QString("%1/xCoef.txt").arg(report3Dir);
    r3NameYcoef = QString("%1/yCoef.txt").arg(report3Dir);
    qDebug() << QString("xCoef name: %1\nyCoef: %2\n").arg(r3NameXcoef).arg(r3NameYcoef);

    r2FileXcoef.setFileName(r3NameXcoef);
    r2FileXcoef.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStream2Xcoef.setDevice(&r2FileXcoef);

    r2FileYcoef.setFileName(r3NameYcoef);
    r2FileYcoef.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStream2Ycoef.setDevice(&r2FileYcoef);


    for(k=0; k<magEq.diapsNum-1; k++)
    {

       magEq.resListDiap.at(k)->remSigma(3.0);

       dataStrL.clear();

       dataStrL << QString("%1").arg(magEq.diaps[k+1]);
       dataStrL << QString("%1").arg(magEq.resListDiap.at(k)->meanKsi);
       dataStrL << QString("%1").arg(magEq.resListDiap.at(k)->rmsMeanKsi);
       dataStrL << QString("%1").arg(magEq.resListDiap.at(k)->meanEta);
       dataStrL << QString("%1").arg(magEq.resListDiap.at(k)->rmsMeanEta);
       dataStrL << QString("%1").arg(magEq.resListDiap.at(k)->resList.size());
  //     dataStrL << QString("%1")
       dataStream << dataStrL.join(r3data.colSep) << "\n";
       //dataStream << QString("%1%7%2%7%3%7%4%7%5%7%6\n").arg(magEq.diaps[k+1]).arg(magEq.resListDiap.at(k)->meanKsi).arg(magEq.resListDiap.at(k)->rmsMeanKsi).arg(magEq.resListDiap.at(k)->meanEta).arg(magEq.resListDiap.at(k)->rmsMeanEta).arg(magEq.resListDiap.at(k)->resList.size()).arg(r3data.colSep);



       r3NameX = QString("%1/%2_diapX.txt").arg(report3Dir).arg(k, 2, 10, QLatin1Char('0'));
       r3NameY = QString("%1/%2_diapY.txt").arg(report3Dir).arg(k, 2, 10, QLatin1Char('0'));
       qDebug() << QString("nameX: %1\nnameY: %2\n").arg(r3NameX).arg(r3NameY);

       r2FileX.setFileName(r3NameX);
       r2FileX.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
       dataStream2X.setDevice(&r2FileX);

       r2FileY.setFileName(r3NameY);
       r2FileY.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
       dataStream2Y.setDevice(&r2FileY);

       szi =  magEq.resListDiap.at(k)->resList.size();
       dotNum = (szi/r3data.aveNum);

       //sort

       x = new double[szi];
       y = new double[szi];
       dx = new double[szi];
       dy = new double[szi];

       mX = new double[szi];
       dmX = new double[szi];
       mY = new double[szi];
       dmY = new double[szi];

       //k=0;
       for(i=0; i<szi; i++)
       {
           x[i] = magEq.resListDiap.at(k)->resList.at(i)->x;
           y[i] = magEq.resListDiap.at(k)->resList.at(i)->y;
           dx[i] = magEq.resListDiap.at(k)->resList.at(i)->Dx;
           dy[i] = magEq.resListDiap.at(k)->resList.at(i)->Dy;

           mX[i] = magEq.resListDiap.at(k)->resList.at(i)->mag;
           dmX[i] = magEq.resListDiap.at(k)->resList.at(i)->ksiOC*cos(grad2rad(magEq.resListDiap.at(k)->resList.at(i)->de));
           mY[i] = magEq.resListDiap.at(k)->resList.at(i)->mag;
           dmY[i] = magEq.resListDiap.at(k)->resList.at(i)->etaOC;
       }

       sortX(x, dx, szi);
       /*
       for(i=0; i<szi-1; i++)
       {
           for(j=i+1; j<szi; j++)
           {
               if(x[j]<x[i])
               {
                   val = x[i];
                   x[i] = x[j];
                   x[j] = val;
                   val = dx[i];
                   dx[i] = dx[j];
                   dx[j] = val;
               }
           }
       }
       /
       sortX(y, dy, szi);
       /*
       for(i=0; i<szi-1; i++)
       {
           for(j=i+1; j<szi; j++)
           {
               if(y[j]<y[i])
               {
                   val = y[i];
                   y[i] = y[j];
                   y[j] = val;
                   val = dy[i];
                   dy[i] = dy[j];
                   dy[j] = val;
               }
           }
       }/

       sortX(mX, dmX, szi);
       sortX(mY, dmY, szi);

       summNum(x, dx, szi, r3data.aveNum);
       /*
       j=0;
       p=0;
       for(i=1; i<szi; i++)
       {
           x[j] += x[i];
           dx[j] += dx[i];
           y[j] += y[i];
           dy[j] += dy[i];
           p++;
           if(p==r3data.aveNum||(i==szi-1))
           {
               x[j] /= 1.0*r3data.aveNum;
               dx[j] /= 1.0*r3data.aveNum;
               y[j] /= 1.0*r3data.aveNum;
               dy[j] /= 1.0*r3data.aveNum;
               j++;
               x[j] = 0.0;
               dx[j] = 0.0;
               y[j] = 0.0;
               dy[j] = 0.0;
               p=0;
           }
       }
       /

       summNum(y, dy, szi, r3data.aveNum);
       summNum(mX, dmX, szi, r3data.aveNum);
       summNum(mY, dmY, szi, r3data.aveNum);

       double *xCoeff = new double[r3data.curvDeg+1];

       if(r3data.isDetCurv)
       {
           //detCurf3(xCoeff, x, dx, dotNum, r3data.curvDeg);
           detCurf3(&resFunc, xCoeff, x, dx, dotNum, r3data.curvDeg);
           qDebug() << "resFunc: " << resFunc << "\n";
       }

       double *yCoeff = new double[r3data.curvDeg+1];

       if(r3data.isDetCurv)
       {
           //detCurf3(yCoeff, y, dy, dotNum, r3data.curvDeg);
           detCurf3(&resFunc, yCoeff, y, dy, dotNum, r3data.curvDeg);
           qDebug() << "resFunc: " << resFunc << "\n";
       }

       dataStrL.clear();
       dataStrL << QString("%1").arg((magEq.diaps[k]+magEq.diaps[k+1])/2.0);
       for(i=0; i<r3data.curvDeg+1; i++)
       {
            dataStrL << QString("%1").arg(xCoeff[i], 10, 'e');
       }

       dataStream2Xcoef << dataStrL.join(r3data.colSep) << "\n";

       dataStrL.clear();
       dataStrL << QString("%1").arg((magEq.diaps[k]+magEq.diaps[k+1])/2.0);
       for(i=0; i<r3data.curvDeg+1; i++)
       {
            dataStrL << QString("%1").arg(yCoeff[i], 10, 'e');
       }

       dataStream2Ycoef << dataStrL.join(r3data.colSep) << "\n";
/*
       for(i=0; i<szi; i++)
       {
           dataStream1 << QString("%1|%2|%3\n").arg(magEq.resListDiap.at(k)->resList.at(i)->mag, 5, 'f', 2).arg(magEq.resListDiap.at(k)->resList.at(i)->ksiOC*cos(grad2rad(magEq.resListDiap.at(k)->resList.at(i)->de)), 8, 'f', 2).arg(magEq.resListDiap.at(k)->resList.at(i)->etaOC, 8, 'f', 2);
           dataStream2X << QString("%1|%2\n").arg(magEq.resListDiap.at(k)->resList.at(i)->x).arg(magEq.resListDiap.at(k)->resList.at(i)->Dx, 6, 'f', 3);
           dataStream2Y << QString("%1|%2\n").arg(magEq.resListDiap.at(k)->resList.at(i)->y).arg(magEq.resListDiap.at(k)->resList.at(i)->Dy, 6, 'f', 3);
           dataStream3 << QString("%1|%2\n").arg(magEq.resListDiap.at(k)->resList.at(i)->mag-magEq.resListDiap.at(k)->resList.at(i)->magOC).arg(magEq.resListDiap.at(k)->resList.at(i)->magOC, 6, 'f', 3);
       }
/
       for(i=0; i<dotNum; i++)
       {
           dataStream1 << QString("%1 %4 %2 %4 %3\n").arg(mX[i], 5, 'f', 2).arg(dmX[i], 8, 'f', 2).arg(dmY[i], 8, 'f', 2).arg(r3data.colSep);
           dataStream2X << QString("%1 %3 %2\n").arg(x[i]).arg(dx[i], 6, 'f', 3).arg(r3data.colSep);
           dataStream2Y << QString("%1 %3 %2\n").arg(y[i]).arg(dy[i], 6, 'f', 3).arg(r3data.colSep);
           dataStream3 << QString("%1 %3 %2\n").arg(mX[i]-dmX[i]).arg(dmX[i], 6, 'f', 3).arg(r3data.colSep);
       }

       r2FileX.close();
       r2FileY.close();

       r3NameX = QString("%1/%2_diapX_corr.txt").arg(report3Dir).arg(k, 2, 10, QLatin1Char('0'));
       r3NameY = QString("%1/%2_diapY_corr.txt").arg(report3Dir).arg(k, 2, 10, QLatin1Char('0'));
       qDebug() << QString("nameX: %1\nnameY: %2\n").arg(r3NameX).arg(r3NameY);

       r2FileX.setFileName(r3NameX);
       r2FileX.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
       dataStream2X.setDevice(&r2FileX);

       r2FileY.setFileName(r3NameY);
       r2FileY.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
       dataStream2Y.setDevice(&r2FileY);

       for(i=0; i<dotNum; i++)
       {
           dataStream2X << QString("%1 %3 %2\n").arg(x[i]).arg(dx[i]-polyY(xCoeff, x[i], r3data.curvDeg), 6, 'f', 3).arg(r3data.colSep);
           dataStream2Y << QString("%1 %3 %2\n").arg(y[i]).arg(dy[i]-polyY(yCoeff, y[i], r3data.curvDeg), 6, 'f', 3).arg(r3data.colSep);
       }

       r2FileX.close();
       r2FileY.close();



       delete [] x;
       delete [] dx;
       delete [] y;
       delete [] dy;
       delete [] mX;
       delete [] dmX;
       delete [] mY;
       delete [] dmY;

       r2FileX.close();
       r2FileY.close();


    }

    r2FileXcoef.close();
    r2FileYcoef.close();

    rFile.close();
    r1File.close();
    r3File.close();

//mDegree
    qDebug() << "\nmDegree\n";

/*
    double *mCoeffX, *mCoeffY;
    int dNum, coefNum;
    //QVector <double> vect0, vect1;
    QStringList opers, coefLines;
    int paramsNum;
/
    if(r3data.doMcoef)
    {

        mCoefFunc(r3NameXcoef, QString(report3Dir+"/mCoefX.txt"), r3data.colSep, r3data.mDegree);

        mCoefFunc(r3NameYcoef, QString(report3Dir+"/mCoefY.txt"), r3data.colSep, r3data.mDegree);

    }



    qDebug() << "end of report3\n";

    rFile.close();
    r1File.close();
    r3File.close();

    r2FileXcoef.close();
    r2FileYcoef.close();
/////////////////////////////////////////////////////////////////
//report1
    observatory obs;
    if(!obs.init(obsCat.toAscii().data(), OBS_SIZE))
    {
        if(obs.getobsynumO("084"))
        {
            qDebug() << "can't find observatory\n";
        }
        else qDebug() << "observatory has found\n";

        Fi = obs.record->getFi();
        Long = obs.record->Long;
        qDebug() << QString("Fi= %1\tLong= %2\n").arg(Fi).arg(Long);

        qDebug() << QString("task1\n");

        rFile.setFileName(reportDirName+"/report1.txt");
        rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
        dataStream.setDevice(&rFile);



        //szi = rStat.mesList.size();

        szi = mesList.size();
        QVector <double> X, dX;

        for(i=0; i<szi; i++)
        {
            mesRec = mesList.at(i);
            MJD = mesRec->errBud->MJD;
            raOc = mesRec->errBud->RAoc;
            deOc = mesRec->errBud->DEoc;
            ra0 = grad2rad(raOc);
            dec0 = grad2rad(deOc);



            qDebug() << QString("raOc= %1\tdeOc= %2\n").arg(grad2rad(raOc)).arg(grad2rad(deOc));
            jdUT1_to_GMST1(&gm1, mjd2jd(MJD));
            s = 2.0*PI*gm1 + Long;
            t = s - grad2rad(raOc);
            qDebug() << QString("s= %1\tFi= %2\n").arg(s).arg(Fi);



            getDegToTang(&ksiZ, &etaZ, rad2grad(s), rad2grad(Fi), raOc, deOc);
            sZ = sqrt(ksiZ*ksiZ + etaZ*etaZ);
            qDebug() << QString("Z: %1\t%2\t%3\n").arg(ksiZ).arg(etaZ).arg(sZ);

            szj = mesRec->resList.size();
            for(j=0; j<szj; j++)
            {
                ksi = mesRec->resList.at(j)->ksi;
                ksiOC = mesRec->resList.at(j)->ksiOC;
                eta = mesRec->resList.at(j)->eta;
                etaOC = mesRec->resList.at(j)->etaOC;
                s1 = sqrt(ksi*ksi + eta*eta);
                cosP = (ksi*ksiZ + eta*etaZ)/(s1*sZ);
                //r = s1*cosP;
                X << s1*cosP;
                s2 = sqrt(ksiOC*ksiOC + etaOC*etaOC);
                cosP = (ksiOC*ksiZ + etaOC*etaZ)/(s2*sZ);
                //dr = s2*cosP;
                dX << s2*cosP;
                //qDebug() << QString("r= %1\tdr= %2\n").arg(r).arg(dr);
                //dataStream << QString("%1|%2\n").arg(r).arg(dr);
            }

        }

        szi = X.size();
        r = new double[szi];
        dr = new double[szi];

        for(i=0; i<szi; i++)
        {
            r[i] = X[i];
            dr[i] = dX[i];
        }

        sortX(r, dr, szi);
        summNum(r, dr, szi, r1aveNum);

        dotNum = (szi/r1aveNum);

        for(i=0; i<dotNum; i++) dataStream << QString("%1|%2\n").arg(r[i]).arg(dr[i]);

        rFile.close();

        qDebug() << QString("task2\n");

        rFile.setFileName(reportDirName+"/report2.txt");
        rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
        dataStream.setDevice(&rFile);

        szi = plStat.platesList.size();
        /*
        plStat.platesList.at(i)->getMinUWE(msRec);
        qDebug() << "msRec: " << msRec << "\n";
        qDebug() << QString("msRec mesureTimeCode: %1\n").arg(msRec->mesureTimeCode);
        mesRec = rStat.getMeasurement(msRec->mesureTimeCode);
        qDebug() << "mesRec: " << mesRec << "\n";
        mesList << mesRec;

        msRec->getVersName(&versName);/



        for(i=0; i<szi; i++)
        {
            qDebug() << "msRec: " << msRec << "\n";
            //plStat.platesList.at(i)->getMinUWE(msRec);
            pMin = plStat.platesList.at(i)->getMinUWEpos();
            if(pMin!=-1) msRec = plStat.platesList.at(i)->mStatList.at(pMin);
            qDebug() << "msRec: " << msRec << "\n";
            qDebug() << QString("msRec mesureTimeCode: %1\n").arg(msRec->mesureTimeCode);
            mesRec = rStat.getMeasurement(msRec->mesureTimeCode);
            qDebug() << "mesRec: " << mesRec << "\n";
            msRec->getVersName(&versName);

            qDebug() << QString("plateName: %1\tversName: %2\n").arg(plStat.platesList.at(i)->plateName).arg(versName);

            MJD = mesRec->errBud->MJD;
            raOc = mesRec->errBud->RAoc;
            deOc = mesRec->errBud->DEoc;
            ra0 = grad2rad(raOc);
            dec0 = grad2rad(deOc);

            qDebug() << QString("raOc= %1\tdeOc= %2\n").arg(grad2rad(raOc)).arg(grad2rad(deOc));
            jdUT1_to_GMST1(&gm1, mjd2jd(MJD));
            s = 2.0*PI*gm1 + Long;
            t = s - grad2rad(raOc);
            qDebug() << QString("s= %1\tFi= %2\n").arg(s).arg(Fi);

            P1 = sin(dec0)*sin(Fi) + cos(dec0)*cos(Fi)*cos(t);
            P2 = cos(dec0)*sin(t);
            P3 = -sin(dec0)*cos(Fi) + cos(dec0)*sin(Fi)*cos(t);
            Az = atan2(P2, P3);
            Zed = atan2(P3, P1)/cos(Az);

            dataStr = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12\n").arg(plStat.platesList.at(i)->plateName).arg(versName).arg(msRec->Nx).arg(msRec->UWEx).arg(msRec->Ny).arg(msRec->UWEy).arg(msRec->rmsOneOCksi).arg(msRec->rmsOneOCeta).arg(msRec->rmsMeanOCksi).arg(msRec->rmsMeanOCeta).arg(rad2grad(Az)).arg(rad2grad(Zed));
            dataStream << dataStr;

        }
/*
        QString plName;
        szi = mesList.size();
        for(i=0; i<szi; i++)
        {
            mesRec = mesList.at(i);

            versName = mesRec->errBud->originName.section(".", -1, -1);
            if(versName=="aver") versName = mesRec->errBud->originName.section(".", -2, -2);
            getPlateName(mesRec->errBud->originName, &plName, plNameType);
            qDebug() << QString("plateName: %1\tversName: %2\n").arg(plName).arg(versName);

            MJD = mesRec->errBud->MJD;
            raOc = mesRec->errBud->RAoc;
            deOc = mesRec->errBud->DEoc;
            ra0 = grad2rad(raOc);
            dec0 = grad2rad(deOc);

            qDebug() << QString("raOc= %1\tdeOc= %2\n").arg(grad2rad(raOc)).arg(grad2rad(deOc));
            jdUT1_to_GMST1(&gm1, mjd2jd(MJD));
            s = 2.0*PI*gm1 + Long;
            t = s - grad2rad(raOc);
            qDebug() << QString("s= %1\tFi= %2\n").arg(s).arg(Fi);

            P1 = sin(dec0)*sin(Fi) + cos(dec0)*cos(Fi)*cos(t);
            P2 = cos(dec0)*sin(t);
            P3 = -sin(dec0)*cos(Fi) + cos(dec0)*sin(Fi)*cos(t);
            Az = atan2(P2, P3);
            Zed = atan2(P3, P1)/cos(Az);

            dataStr = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12\n").arg(plName).arg(versName).arg(msRec->Nx).arg(msRec->UWEx).arg(msRec->Ny).arg(msRec->UWEy).arg(msRec->rmsOneOCksi).arg(msRec->rmsOneOCeta).arg(msRec->rmsMeanOCksi).arg(msRec->rmsMeanOCeta).arg(rad2grad(Az)).arg(rad2grad(Zed));
            dataStream << dataStr;
        }
        /


        rFile.close();


    }
    else qDebug() << QString("init obs failed\n");

    rFile.close();

//report4

    if(isReport4)
    {

        report4Dir = reportDirName + "/report4";
        reportDir.mkdir("report4");

        if(r4data.isSysCorr)
        {
                vectF = new vectGrid3D;
                tmp = r4data.sysCorrFile.toAscii();
                vectF->init(tmp.data());
                vectF->printCoefs();
        }

        di = mesList.size();
        qDebug() << QString("mesList size = %1\n").arg(di);

        if(r4data.isSysCorr)
        {
            di = mesList.size();
            //qDebug() << QString("mesList size= %1\n").arg(di);
                for(i=0; i<di; i++)
                {
                    mesRec = mesList.at(i);

                    rNum = mesRec->resList.size();

                    for(j=0; j<rNum; j++)
                    {
                        resRec = mesList.at(i)->resList.at(j);
               /*         if(!vectF->int2D(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, &ni))
                        {
           //                 dDec = -0.5082077*resRec->de + 18.1648793;
                                dDec = 0.0;
                                resRec->ksiOC-= dKsi;
                                resRec->etaOC-= (dEta+dDec);

                                resRec->ksi-= mas_to_grad(dKsi);
                                resRec->eta-= mas_to_grad(dEta+dDec);
                        }
                    }
                    /
                        if(!vectF->int2Drad(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, r4data.rMax, r4data.nmin))
                        {
           //                 dDec = -0.5082077*resRec->de + 18.1648793;
                                dDec = 0.0;
                                resRec->ksiOC-= dKsi;
                                resRec->etaOC-= (dEta+dDec);

                                resRec->ksi-= mas_to_grad(dKsi);
                                resRec->eta-= mas_to_grad(dEta+dDec);
                        }
                    }
                }
        }

        qDebug() << QString("isFindDiap = %1\n").arg(r4data.isFindDiap);


        if(r4data.isFindDiap)
        {
            diniKey = 1;
            di = mesList.size();
            //qDebug() << QString("mesList size= %1\n").arg(di);
                for(i=0; i<di; i++)
                {
                        //printf("\nfindDiap: %f\%\n", (double)i*100.0/(double)di);

                        mesRec = mesList.at(i);

                        rNum = mesRec->resList.size();
                        //qDebug() << QString("rNum= %1\n").arg(rNum);
                        for(j=0; j<rNum; j++)
                        {

                                resRec = mesList.at(i)->resList.at(j);

                               // if(mesRec->errBud->exptime<expMin) continue;


                          //      if((fabs(resRec->ksiOC)<maxEta)&&(fabs(resRec->etaOC)<maxEta))
                          //      {
                                        if(diniKey)
                                        {
                                                naxeX[1] = naxeX[0] = resRec->ksi;
                                                naxeY[1] = naxeY[0] = resRec->eta;
                                                diniKey = 0;
                                        }
                                        else
                                        {
                                                if(naxeX[0]>resRec->ksi) naxeX[0] = resRec->ksi;
                                                if(naxeX[1]<resRec->ksi) naxeX[1] = resRec->ksi;
                                                if(naxeY[0]>resRec->eta) naxeY[0] = resRec->eta;
                                                if(naxeY[1]<resRec->eta) naxeY[1] = resRec->eta;
                                        }
                            //    }
                        }
                }
                if(fabs(naxeX[0])>fabs(naxeX[1])) axd = fabs(naxeX[1]);
                else axd = fabs(naxeX[0]);
                naxeX[0] = -axd;
                naxeX[1] = axd;

                if(fabs(naxeY[0])>fabs(naxeY[1])) axd = fabs(naxeY[1]);
                else axd = fabs(naxeY[0]);
                naxeY[0] = -axd;
                naxeY[1] = axd;
        }
        else
        {
                naxeX[0] = r4data.x0/3600.0;
                naxeX[1] = r4data.x1/3600.0;
                naxeY[0] = r4data.y0/3600.0;
                naxeY[1] = r4data.y1/3600.0;
        }

        qDebug() << "naxeX= " << naxeX[0] << " - " << naxeX[1] << "\n";
        qDebug() << "naxeY= " << naxeY[0] << " - " << naxeY[1] << "\n";





        diapNum = magEq.resListDiap.size();

        qDebug() << QString("\nmLevNum= %1\nxLevNum= %2\nyLevNum= %3\n").arg(mLevNum).arg(r4data.xLevNum).arg(r4data.yLevNum);

        mLevels = new double[mLevNum];
        xLevels = new double[r4data.xLevNum];
        yLevels = new double[r4data.yLevNum];

        dX = fabs(naxeX[1] - naxeX[0])/(double)(r4data.xLevNum-1);
        qDebug() << "dX= " << dX << "\n";
        qDebug() << "xLevels: ";
        for(i=0; i<r4data.xLevNum; i++)
        {
            xLevels[i] = naxeX[0] + dX*i;
            qDebug() << xLevels[i] << " ";
        }
        qDebug() << "\n";

        dY = fabs(naxeY[1] - naxeY[0])/(double)(r4data.yLevNum-1);
        qDebug() << "dY= " << dX << "\n";
        qDebug() << "yLevels: ";
        for(i=0; i<r4data.yLevNum; i++)
        {
            yLevels[i] = naxeY[0] + dY*i;
            qDebug() << yLevels[i] << " ";
        }
        qDebug() << "\n";

        /*mLevels[0] = 0.0;
        for(i=1; i<mLevNum-1; i++) mLevels[i] = (mLevNumBeg+i-1)*1.0;
        mLevels[mLevNum-1] = 20.0;/
        qDebug() << "mLevels: ";
        for(i=0; i<mLevNum; i++)
        {
            mLevels[i] = mdList.at(i).toDouble();
            qDebug() << mLevels[i] << " ";
        }
        qDebug() << "\n";

        qDebug() << "\nvectFdata\n";

        vectGrid3D *vectFdata = new vectGrid3D();

        vectFdata->setLevels(xLevels, r4data.xLevNum, yLevels, r4data.yLevNum, mLevels, mLevNum);



        vect = new double[3];

        for(i=0; i<di; i++)
        {
                printf("\nsetPoint: %f\%\n", (double)i*100.0/(double)di);

                mesRec = mesList.at(i);

                rNum = mesRec->resList.size();
                for(j=0; j<rNum; j++)
                {

                    resRec = mesList.at(i)->resList.at(j);
                    vect[0] = resRec->ksi;
                    vect[1] = resRec->eta;
                    vect[2] = resRec->mag;
                    vectFdata->addPoint(vect, resRec->ksiOC, resRec->etaOC);
                }
        }
        vectFdata->detMean();
        if(r4data.doWeght) vectFdata->doWeght();
        vectFdata->initVF();

        vectFdata->printCoefs();

        vectFdata->saveVF(report4Dir+"/res.vf");

        vectFdata->saveDotList(report4Dir, "|");


    }

    //report5
    if(isReport5)
    {

        report5Dir = reportDirName + "/report5";
        reportDir.mkdir("report5");

        vectGrid3D *vectF5;// = new vectGrid3D;
       // QByteArray tmp;

        if(r5data.isSysCorr)
        {
                vectF5 = new vectGrid3D;
                tmp = r5data.sysCorrFile.toAscii();
                vectF5->init(tmp.data());
                vectF5->printCoefs();
        }


        di = mesList.size();
        qDebug() << QString("mesList size = %1\n").arg(di);

        if(r5data.isSysCorr)
        {
            di = mesList.size();
            //qDebug() << QString("mesList size= %1\n").arg(di);
                for(i=0; i<di; i++)
                {
                    mesRec = mesList.at(i);

                    rNum = mesRec->resList.size();

                    for(j=0; j<rNum; j++)
                    {
                        resRec = mesList.at(i)->resList.at(j);
               /*         if(!vectF->int2D(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, &ni))
                        {
           //                 dDec = -0.5082077*resRec->de + 18.1648793;
                                dDec = 0.0;
                                resRec->ksiOC-= dKsi;
                                resRec->etaOC-= (dEta+dDec);

                                resRec->ksi-= mas_to_grad(dKsi);
                                resRec->eta-= mas_to_grad(dEta+dDec);
                        }
                    }
                    /
                        if(!vectF5->int2Drad(resRec->x, resRec->y, resRec->mag, &dKsi, &dEta, r5data.rMax, r5data.nmin))
                        {
           //                 dDec = -0.5082077*resRec->de + 18.1648793;
                                dDec = 0.0;
                                resRec->Dx-= dKsi;
                                resRec->Dy-= (dEta+dDec);

                                resRec->x-= dKsi;
                                resRec->y-= dEta+dDec;
                        }
                    }
                }
        }







        qDebug() << QString("isFindDiap = %1\n").arg(r5data.isFindDiap);

        if(r5data.isFindDiap)
        {
            diniKey = 1;
            di = mesList.size();
            //qDebug() << QString("mesList size= %1\n").arg(di);
                for(i=0; i<di; i++)
                {
                        //printf("\nfindDiap: %f\%\n", (double)i*100.0/(double)di);

                        mesRec = mesList.at(i);

                        rNum = mesRec->resList.size();
                        //qDebug() << QString("rNum= %1\n").arg(rNum);
                        for(j=0; j<rNum; j++)
                        {

                                resRec = mesList.at(i)->resList.at(j);

                               // if(mesRec->errBud->exptime<expMin) continue;

                          //      if((fabs(resRec->ksiOC)<maxEta)&&(fabs(resRec->etaOC)<maxEta))
                          //      {
                                        if(diniKey)
                                        {
                                            naxeX[1] = naxeX[0] = resRec->x;
                                            naxeY[1] = naxeY[0] = resRec->y;
                                            diniKey = 0;
                                        }
                                        else
                                        {
                                            if(naxeX[0]>resRec->x) naxeX[0] = resRec->x;
                                            if(naxeX[1]<resRec->x) naxeX[1] = resRec->x;
                                            if(naxeY[0]>resRec->y) naxeY[0] = resRec->y;
                                            if(naxeY[1]<resRec->y) naxeY[1] = resRec->y;
                                        }
                            //    }
                        }
                }
                if(fabs(naxeX[0])>fabs(naxeX[1])) axd = fabs(naxeX[1]);
                else axd = fabs(naxeX[0]);
                naxeX[0] = -axd;
                naxeX[1] = axd;

                if(fabs(naxeY[0])>fabs(naxeY[1])) axd = fabs(naxeY[1]);
                else axd = fabs(naxeY[0]);
                naxeY[0] = -axd;
                naxeY[1] = axd;
        }
        else
        {
                naxeX[0] = r5data.x0;
                naxeX[1] = r5data.x1;
                naxeY[0] = r5data.y0;
                naxeY[1] = r5data.y1;
        }

        qDebug() << "naxeX= " << naxeX[0] << " - " << naxeX[1] << "\n";
        qDebug() << "naxeY= " << naxeY[0] << " - " << naxeY[1] << "\n";

        diapNum = magEq.resListDiap.size();

        qDebug() << QString("\nmLevNum= %1\nxLevNum= %2\nyLevNum= %3\n").arg(mLevNum).arg(r5data.xLevNum).arg(r5data.yLevNum);

        mLevels = new double[mLevNum];
        xLevels = new double[r5data.xLevNum];
        yLevels = new double[r5data.yLevNum];

        dX = fabs(naxeX[1] - naxeX[0])/(double)(r5data.xLevNum-1);
        qDebug() << "dX= " << dX << "\n";
        qDebug() << "xLevels: ";
        for(i=0; i<r5data.xLevNum; i++)
        {
            xLevels[i] = naxeX[0] + dX*i;
            qDebug() << xLevels[i] << " ";
        }
        qDebug() << "\n";

        dY = fabs(naxeY[1] - naxeY[0])/(double)(r5data.yLevNum-1);
        qDebug() << "dY= " << dX << "\n";
        qDebug() << "yLevels: ";
        for(i=0; i<r5data.yLevNum; i++)
        {
            yLevels[i] = naxeY[0] + dY*i;
            qDebug() << yLevels[i] << " ";
        }
        qDebug() << "\n";

        /*mLevels[0] = 0.0;
        for(i=1; i<mLevNum-1; i++) mLevels[i] = (mLevNumBeg+i-1)*1.0;
        mLevels[mLevNum-1] = 20.0;/
        qDebug() << "mLevels: ";
        for(i=0; i<mLevNum; i++)
        {
            mLevels[i] = mdList.at(i).toDouble();
            qDebug() << mLevels[i] << " ";
        }
        qDebug() << "\n";

        qDebug() << "\nvectFdata\n";

        vectGrid3D *vectF5data = new vectGrid3D();

        vectF5data->setLevels(xLevels, r5data.xLevNum, yLevels, r5data.yLevNum, mLevels, mLevNum);



        //double *vect = new double[3];

        for(i=0; i<di; i++)
        {
                printf("\nsetPoint: %f\%\n", (double)i*100.0/(double)di);

                mesRec = mesList.at(i);

                rNum = mesRec->resList.size();
                for(j=0; j<rNum; j++)
                {

                    resRec = mesList.at(i)->resList.at(j);
                    vect[0] = resRec->x;
                    vect[1] = resRec->y;
                    vect[2] = resRec->mag;
                    vectF5data->addPoint(vect, resRec->Dx, resRec->Dy);
                }
        }
        qDebug() << QString("numTot= %1\n").arg(vectF5data->numTot);
        vectF5data->detMean();
        if(r5data.doWeght) vectF5data->doWeght();
        vectF5data->initVF();

        vectF5data->printCoefs();

        vectF5data->saveVF(report5Dir+"/res.vf");

        vectF5data->saveDotList(report5Dir, "|");


    }


//report6

    if(isReport6)
    {

        QSettings *settConf = new QSettings("./conf/conf.ini", QSettings::IniFormat);
        insSettings *instruments = new insSettings("./conf/telescopes.ini");
        instruments->getNumIns(instruments->curInst);

        int mpeWaitTime = sett->value("General/mpeWaitTime").toInt();
        int sbWaitTime = sett->value("General/sbWaitTime").toInt();

        int lspmFind = sett->value("objectsFind/lspmFind").toInt();
        int skybotFind = sett->value("objectsFind/skybotFind").toInt();
        int tryMpeph = sett->value("objectsFind/tryMpeph").toInt();
        double magObj0 = sett->value("objectsFind/mag0", 6.0).toDouble();
        double magObj1 = sett->value("objectsFind/mag1", 15.0).toDouble();

        QList <catFinder *> starCatList;

        catFinder *starCat = new catFinder;
        starCat->exeName = settConf->value("ucac2/exeName").toString();
        starCat->exePath = settConf->value("ucac2/exePath").toString();
        starCat->catType = settConf->value("ucac2/catType").toInt();
        starCat->catName = settConf->value("ucac2/catName").toString();
        starCat->catPath = settConf->value("ucac2/catPath").toString();
        starCatList << starCat;
        //
        starCat = new catFinder;
        starCat->exeName = settConf->value("usnob/exeName").toString();
        starCat->exePath = settConf->value("usnob/exePath").toString();
        starCat->catType = settConf->value("usnob/catType").toInt();
        starCat->catName = settConf->value("usnob/catName").toString();
        starCat->catPath = settConf->value("usnob/catPath").toString();
        starCatList << starCat;
        //
        starCat = new catFinder;
        starCat->exeName = settConf->value("ucac3/exeName").toString();
        starCat->exePath = settConf->value("ucac3/exePath").toString();
        starCat->catType = settConf->value("ucac3/catType").toInt();
        starCat->catName = settConf->value("ucac3/catName").toString();
        starCat->catPath = settConf->value("ucac3/catPath").toString();
        starCatList << starCat;
        //
        starCat = new catFinder;
        starCat->exeName = settConf->value("lspm/exeName").toString();
        starCat->exePath = settConf->value("lspm/exePath").toString();
        starCat->catType = settConf->value("lspm/catType").toInt();
        starCat->catName = settConf->value("lspm/catName").toString();
        starCat->catPath = settConf->value("lspm/catPath").toString();
        starCatList << starCat;
        //
        starCat = new catFinder;
        starCat->exeName = settConf->value("lspmFind/exeName").toString();
        starCat->exePath = settConf->value("lspmFind/exePath").toString();
        starCat->catType = settConf->value("lspmFind/catType").toInt();
        starCat->catName = settConf->value("lspmFind/catName").toString();
        starCat->catPath = settConf->value("lspmFind/catPath").toString();
        starCatList << starCat;
        //
        //qDebug() << QString("starCatList count: %1\n").arg(starCatList.count());
        QString ast_eph_prog = settConf->value("processes/ast_eph_prog", "./mpeph.exe").toString();
        QString ast_eph_prog_folder = settConf->value("processes/ast_eph_prog_folder", "./").toString();
        QString skybot_prog = settConf->value("processes/skybot_prog", "./skybotclient.exe").toString();
        QString skybot_prog_folder = settConf->value("processes/skybot_prog_folder", "./").toString();
        QString get_http_prog = settConf->value("processes/get_http_prog", "./getHttpHeader.exe").toString();
        QString get_http_prog_folder = settConf->value("processes/get_http_prog_folder", "./").toString();
        QString ucac3find_prog = settConf->value("processes/ucac3find", "./ucac3find.exe").toString();
        QString ucac3find_prog_folder = settConf->value("processes/ucac3find_folder", "./").toString();

        int currentCat = settConf->value("catalogs/currentCatalog", 0).toInt();
        QString observatoryCat = settConf->value("catalogs/observatoryCat", "./../../../data/cats/Obs.txt").toString();

        QString obsCode = settConf->value("celestial/obsName", "084").toString();
        double maxObjDisp = settConf->value("celestial/maxObjDisp", 2).toDouble();

        double mag0 = sett->value("catalogs/mag0", 6.0).toDouble();
        double mag1 = sett->value("catalogs/mag1", 15.0).toDouble();

        fitsdata *fitsd = new fitsdata;

      //  double mJD, ra, de;

        di = mesList.size();
        //QList <ucac3Rec*> u3List;
        int u3Summ, resSumm;
        u3Summ = resSumm = 0;

        for(i=0; i<di; i++)
        {
            mesRec = mesList.at(i);



            fitsd->setPos(mesRec->errBud->MJD, mesRec->errBud->RAoc, mesRec->errBud->DEoc);

            //выставить параметры инструмента
                fitsd->marksG->setInstrProp(instruments->scx, instruments->scy, instruments->rang);
                fitsd->marksGIpix->setInstrProp(instruments->scx, instruments->scy, instruments->rang);
            //открыть метки
                //fitsd->marksGIpix->loadTanImg(fileName, mSep, mColumn);
                //if(isMove2corner) fitsd->marksGIpix->moveToCorner();
            //из каталога
                fitsd->marksG->clearMarks();
                fitsd->getMarksGrid(starCatList.at(currentCat), instruments->fov, mag0, mag1, -1);
                fitsd->detTan();
            //объект
                fitsd->mpeWaitTime = mpeWaitTime;
                fitsd->sbWaitTime = sbWaitTime;
                if(lspmFind)
                {
                    if(!(starCatList.size()<5)) fitsd->findLspmCat(starCatList[4]->exeName, starCatList[4]->exePath, starCatList[4]->catPath, instruments->fov, magObj0, magObj1);
                }
                if(skybotFind)
                {
                    qDebug() << QString("skybotFindType\n");
                    QStringList objNames;
                    //tryMpeph = foDlg->ui_isUseExpNum->isChecked();
                    if(tryMpeph)
                    {
                        fitsd->findSkybotNamesList(&objNames, skybot_prog, skybot_prog_folder, instruments->fov, obsCode, magObj0, magObj1);
                        sz = objNames.size();
                        for(i=0; i<sz; i++)fitsd->getMpephName(objNames.at(i), ast_eph_prog, ast_eph_prog_folder, magObj0, magObj1);
                    }
                    else fitsd->findSkybot(skybot_prog, skybot_prog_folder, instruments->fov, obsCode, magObj0, magObj1);
                //fitsd->findLspmCat(starCatList[4]->exeName, starCatList[4]->exePath, starCatList[4]->catPath, fovSpinBox->value());
                }

               mesRec->detMarksList(fitsd->marksG->marks);

               szj = mesRec->u3MarksList.size();
               u3Summ += szj;
               resSumm += mesRec->resList.size();
               qDebug() << QString("u3List size: %1\tresList size: %2").arg(szj).arg(mesRec->resList.size());

               for(j=0; j<szj; j++)
               {
                   //qDebug() << QString("ra: %1|%2|%3\n").arg(mesRec->u3MarksList[j]->u3Rec->ra).arg(mesRec->u3MarksList[j]->resRec->ra).arg(grad_to_mas(fabs(mesRec->u3MarksList[j]->u3Rec->ra - mesRec->u3MarksList[j]->resRec->ra)));
                   //qDebug() << QString("de: %1|%2|%3\n").arg(mesRec->u3MarksList[j]->u3Rec->dec).arg(mesRec->u3MarksList[j]->resRec->de).arg(grad_to_mas(fabs(mesRec->u3MarksList[j]->u3Rec->dec - mesRec->u3MarksList[j]->resRec->de)));
                   //qDebug() << QString("dist: %1\n\n").arg(sqrt(pow(mesRec->u3MarksList[j]->u3Rec->ra - mesRec->u3MarksList[j]->resRec->ra, 2.0) + pow(mesRec->u3MarksList[j]->u3Rec->dec - mesRec->u3MarksList[j]->resRec->de, 2.0)));
               }

        }

        qDebug() << QString("u3Summ: %1\tresSumm: %2\tident q: %3\n").arg(u3Summ).arg(resSumm).arg(u3Summ/(1.0*resSumm));



        diapU3 colEq;
        QString deDiapsStr = sett->value("report6/deDiaps", "-90|90").toString();
        QString colSep6 = sett->value("report6/colSep", "|").toString();
        QStringList deDiapsList = deDiapsStr.split("|");

        colEq.diapsNum = deDiapsList.size();
        colEq.diaps = new double[colEq.diapsNum];

        qDebug() << QString("diapsNum= %1\n").arg(colEq.diapsNum);
        marksGrid *tGrid;
        for(i=0; i<colEq.diapsNum; i++)
        {
            colEq.diaps[i] = deDiapsList.at(i).toDouble();
            qDebug() << QString("diap[%1]= %2\n").arg(i).arg(colEq.diaps[i]);
            tGrid = new marksGrid;
            colEq.mgList << tGrid;
        }

        double cIndex;
        marksP *tMark;

        report6Dir = reportDirName + "/report6";
        reportDir.mkdir("report6");

        QDir tDir;
        tDir.setPath(report6Dir);
        QStringList filesList, filtersL;
        filtersL << "*";
        filesList = tDir.entryList(filtersL);
        for(i=0; i<filesList.size(); i++) tDir.remove(filesList.at(i));

        di = mesList.size();
        //qDebug() << QString("mesList size= %1\n").arg(di);
            for(i=0; i<di; i++)
            {
                    //printf("\nfindDiap: %f\%\n", (double)i*100.0/(double)di);

                mesRec = mesList.at(i);

                rNum = mesRec->u3MarksList.size();
                //qDebug() << QString("rNum= %1\n").arg(rNum);
                for(j=0; j<rNum; j++)
                {

                     tMark = mesList.at(i)->u3MarksList.at(j);

                     MJD = tMark->resRec->mJD;
                     raOc = tMark->resRec->ra;
                     deOc = tMark->resRec->de;
                     ra0 = grad2rad(raOc);
                     dec0 = grad2rad(deOc);

                     qDebug() << QString("raOc= %1\tdeOc= %2\tksiOC= %3\tetaOC= %4\n").arg(grad2rad(raOc)).arg(grad2rad(deOc)).arg(tMark->resRec->ksiOC).arg(tMark->resRec->etaOC);
                     jdUT1_to_GMST1(&gm1, mjd2jd(MJD));
                     s = 2.0*PI*gm1 + Long;
                     t = s - grad2rad(raOc);
                     qDebug() << QString("s= %1\tFi= %2\n").arg(s).arg(Fi);

                     P1 = sin(dec0)*sin(Fi) + cos(dec0)*cos(Fi)*cos(t);
                     P2 = cos(dec0)*sin(t);
                     P3 = -sin(dec0)*cos(Fi) + cos(dec0)*sin(Fi)*cos(t);
                     Az = atan2(P2, P3);
                     Zed = atan2(P3, P1)/cos(Az);

                     //cIndex = tMark->u3Rec->smB - tMark->u3Rec->smR2;

                     //qDebug() << QString("cIndex= %1\n").arg(cIndex);
                     qDebug() << QString("Zed= %1\n").arg(rad2grad(Zed));

                     for(k=0; k<colEq.diapsNum-1; k++)
                     {

                         if(rad2grad(Zed)>colEq.diaps[k]&&rad2grad(Zed)<colEq.diaps[k+1])
                         {
                             r3NameX = QString("%1/%2_diapX.txt").arg(report6Dir).arg(k);
                             r3NameY = QString("%1/%2_diapY.txt").arg(report6Dir).arg(k);
                             qDebug() << QString("nameX: %1\nnameY: %2\n").arg(r3NameX).arg(r3NameY);

                             r2FileX.setFileName(r3NameX);
                             //if(!reportDir.exists(r3NameX)) r2FileX.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
                             //else
                             r2FileX.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Append);
                             dataStream2X.setDevice(&r2FileX);

                             r2FileY.setFileName(r3NameY);
                             //if(!reportDir.exists(r3NameY)) r2FileY.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
                             //else
                             r2FileY.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Append);
                             dataStream2Y.setDevice(&r2FileY);

                             cIndex = tMark->u3Rec->jmag - tMark->u3Rec->kmag;
                             dataStream2X << QString("%1 %3 %2\n").arg(cIndex).arg(tMark->resRec->Dx).arg(colSep6);
                             dataStream2Y << QString("%1 %3 %2\n").arg(cIndex).arg(tMark->resRec->Dy).arg(colSep6);

                             r2FileX.close();
                             r2FileY.close();

                             colEq.mgList.at(k)->marks << tMark;
                         }

                     }


                }
            }


/*

            for(k=0; k<colEq.diapsNum-1; k++)
            {

                r3NameX = QString("%1/%2_diapX.txt").arg(report6Dir).arg(k);
                r3NameY = QString("%1/%2_diapY.txt").arg(report6Dir).arg(k);
                qDebug() << QString("nameX: %1\nnameY: %2\n").arg(r3NameX).arg(r3NameY);

                r2FileX.setFileName(r3NameX);
                r2FileX.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
                dataStream2X.setDevice(&r2FileX);

                r2FileY.setFileName(r3NameY);
                r2FileY.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
                dataStream2Y.setDevice(&r2FileY);

                for(i=0; i<colEq.mgList[k]->marks.size(); i++)
                {
                    tMark = colEq.mgList[k]->marks[i];
                    cIndex = tMark->u3Rec->jmag - tMark->u3Rec->kmag;
                    dataStream2X << QString("%1%3%2\n").arg(cIndex).arg(tMark->resRec->ksiOC).arg(colSep6);
                    dataStream2Y << QString("%1%3%2\n").arg(cIndex).arg(tMark->resRec->etaOC).arg(colSep6);
                }
                r2FileX.close();
                r2FileY.close();
            }

/


    }

    di = mesList.size();

    r3NameX = QString("%1/resListOC.txt").arg(reportDirName);
    r3NameY = QString("%1/marksListOC.txt").arg(reportDirName);

    r2FileX.setFileName(r3NameX);
    r2FileX.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStream2X.setDevice(&r2FileX);

    r2FileY.setFileName(r3NameY);
    r2FileY.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStream2Y.setDevice(&r2FileY);

    qDebug() << QString("mesList size: %1\n").arg(di);

    for(i=0; i<di; i++)
    {
        mesRec = mesList.at(i);

        rNum = mesRec->resList.size();
        qDebug() << QString("%1: resList size: %2\n").arg(i).arg(rNum);
        for(j=0; j<rNum; j++)
        {
            dataStream2X << QString("%1|%2\n").arg(mesRec->resList.at(j)->ksiOC).arg(mesRec->resList.at(j)->etaOC);
        }
        rNum = mesRec->u3MarksList.size();
        qDebug() << QString("%1: u3List size: %2\n").arg(i).arg(rNum);
        for(j=0; j<rNum; j++)
        {
            dataStream2Y << QString("%1|%2\n").arg(mesRec->u3MarksList.at(j)->resRec->ksiOC).arg(mesRec->u3MarksList.at(j)->resRec->etaOC);
        }


    }

    r2FileX.close();
    r2FileY.close();
*/

    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);


    return 0;//a.exec();
}
/*
int detCurf3(QString *resFunc, double *Zi, double *x, double *y, int num, int deg)
{
    int i, j, k;
    deg++;
    double *z = new double[deg];
    double *c = new double[deg*num];
    double *we = new double[num];

    int *EXCLIND;
    double *D = new double[deg*deg];
    int RN;
    //QStringList tStrL;

    for(i=0; i<num; i++)
    {
        //tStrL.clear();

        for(j=0; j<deg; j++)
        {
            c[i*deg+j] = pow(x[i], j);

            //tStrL << QString("Z0*%1").arg(c[i+j]);
        }

        we[i] = 1;
    }

    EXCLIND = new int [num];
    D=new double[deg*deg];
    RN=0;
    double UWE;
    //iLSM(deg, num, 500, EXCLIND, z, c, y, UWE, D, 3, RN, we);
    lsm(deg, num, z, c, y, UWE, D);
    //slsm(deg, num, z, c, y);

    QStringList opers;
    QStringList opers1;

    qDebug() << QString("RN: %1\tUWE: %2\n").arg(RN).arg(UWE);

    int sLog;
    for(i=0;i<deg; i++)
    {
        Zi[i] = z[i];
        if(z[i]>0) opers << "+";
        else opers << "-";
        sLog = (int)(log10(fabs(z[i]))/fabs(log10(fabs(z[i]))));
        opers << QString("(%1*10^(%2))*x^%3").arg(fabs(z[i])*pow(10, -((int)log10(fabs(z[i])))+1)).arg((int)log10(fabs(z[i]))+sLog).arg(i);
        qDebug() << QString("sLog: %1\n").arg(sLog);

        opers1 << QString("((%1)*x^%3").arg(z[i]).arg(i);
    }

    resFunc->clear();
    resFunc->insert(0, opers.join(""));
    qDebug() << QString("resFunc: %1\n").arg(opers.join(""));
    qDebug() << opers1.join("+") << "\n";

    delete [] we;
    delete [] D;
    delete [] z;
    delete [] c;
}

/*
int detCurf3(double *Zi, double *x, double *y, int num, int deg)
{
    int i, j, k;
    deg++;
    //int num = resList.size();
    double *z = new double[deg];
    double *c = new double[deg*num];
    double *we = new double[num];
    //double *x = new double[num];
    //double *y = new double[num];



    int *EXCLIND;
    double *D = new double[deg*deg];
    int RN;
    QString tStr;
    QStringList tStrL;

    for(i=0; i<num; i++)
    {
   /*     if(!ax)
        {
            x[i] = resList.at(i)->x;
            y[i] = resList.at(i)->Dx;
        }
        else
        {
            x[i] = resList.at(i)->y;
            y[i] = resList.at(i)->Dy;
        }
/
       // qDebug() << QString("x= %1\ty= %2\n").arg(x[i]).arg(y[i]);
        tStr.clear();
        tStrL.clear();

        for(j=0; j<deg; j++)
        {
            c[i+j] = pow(x[i], deg-j-1);
            //for(k=j+1; k<deg;k++) c[i+j] *= x[i];

            tStrL << QString("Z0*%1").arg(c[i+j]);
            //qDebug() << QString("c[%1]= %2\n").arg(j).arg(c[i+j]);
        }

       // qDebug() << tStrL.join(" + ") + " = " + QString("%1\n").arg(y[i]);
/*
        c[i] = pow(x[i], 3);
        c[i+1] = pow(x[i], 2);
        c[i+2] = pow(x[i], 1);
        c[i+3] = 1;
/
        we[i] = 1;
    }

    EXCLIND = new int [num];
   // ZKSI=new double[3];
    D=new double[deg*deg];
    RN=0;
    double UWE;
    //iLSM(deg, num, 500, EXCLIND, z, c, y, UWE, D, 3, RN, we);
    lsm(deg, num, z, c, y, UWE, D, we);

    //resFunc->clear();
    QStringList opers;
    QStringList opers1;

    qDebug() << QString("RN: %1\tUWE: %2\n").arg(RN).arg(UWE);

    int sLog;
    for(i=0;i<deg-1; i++)
    {
        Zi[i] = z[i];
        if(z[i]>0) opers << "+";
        else opers << "-";
        sLog = (int)(log10(fabs(z[i]))/fabs(log10(fabs(z[i]))));
        opers << QString("(%1*10^(%2))*x^%3").arg(fabs(z[i])*pow(10, -((int)log10(fabs(z[i])))+1)).arg((int)log10(fabs(z[i]))+sLog).arg(deg-i-1);
        qDebug() << QString("sLog: %1\n").arg(sLog);

        opers1 << QString("((%1)*x^%3").arg(z[i]).arg(deg-i-1);
    }
    if(z[deg-1]>0) opers << "+";
    else opers << "-";
    sLog = (int)(log10(fabs(z[deg-1]))/fabs(log10(fabs(z[deg-1]))));
    opers << QString("(%1*10^(%2))").arg(fabs(z[deg-1])*pow(10, -((int)log10(fabs(z[deg-1]))+1))).arg((int)log10(fabs(z[deg-1]))+sLog);
    opers1 << QString("%1").arg(z[deg-1]);
    QString resFunc;
    resFunc.clear();
/*    for(i=0;i<deg; i++)
    {
        if(z[i]>0) resFunc.append(QString("+"));
        resFunc.append(opers[i]);
    }/
    qDebug() << QString("resFunc: %1\n").arg(opers.join(""));
    qDebug() << opers1.join("+") << "\n";

    delete [] we;
    delete [] D;
    delete [] z;
    delete [] c;
/*
    double resX;

    for(i=0; i<num; i++)
    {
        resX = 0;
        for(j=0; j<deg; j++)
        {
            resX += z[j]*c[i+j];
        }
        qDebug() << QString("res: %1\t%2\t%3\n").arg(resX).arg(y[i]).arg(resX-y[i]);
    }
    /
}
/
void summNum(double *x, double *dx, int num, int sNum)
{
    int i, j, p;
    j=0;
    p=0;
    qDebug() << QString("sNum= %1\n").arg(sNum);

    for(i=0; i<num; i++)
    {


        if(p==0)
        {
            x[j] = x[i];
            dx[j] = dx[i];
        }
        else
        {
            x[j] += x[i];
            dx[j] += dx[i];
        }
        p++;
        if(p==sNum||(i==num-1))
        {
            x[j] /= 1.0*p;
            dx[j] /= 1.0*p;

            //qDebug() << QString("j= %1\tp= %2\tx= %3\tdx= %4\n").arg();

            j++;
            //i++;
            //x[j] = x[i];
            //dx[j] = dx[i];

            p=0;
        }

    }
}

double polyY(double *Zi, double x, int deg)
{
    int j;
    double res;
    res = 0;
    for(j=0; j<deg+1; j++)
    {
        res += Zi[j]*pow(x, j);
    }
    return res;
}

void mCoefFunc(QString coefFileName, QString mCoefFileName, QString dataSep, int coefDeg)
{
    int i, j;
    double *x, *dx;
    double *mCoeffX;
    int dNum, coefNum;
    QString resFunc;
    //QVector <double> vect0, vect1;
    QStringList opers, coefLines, dataStrL;

    QFile r2FileXcoef, rFile;
    QTextStream dataStream2Xcoef, dataStream;

    r2FileXcoef.setFileName(coefFileName);
    r2FileXcoef.open(QIODevice::ReadOnly|QIODevice::Text);
    dataStream2Xcoef.setDevice(&r2FileXcoef);
/*
    r2FileYcoef.setFileName(r3NameYcoef);
    r2FileYcoef.open(QIODevice::ReadOnly|QIODevice::Text);
    dataStream2Ycoef.setDevice(&r2FileYcoef);
/
    rFile.setFileName(mCoefFileName);
    rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStream.setDevice(&rFile);

    coefLines.clear();
    while(!dataStream2Xcoef.atEnd())
    {
        coefLines << dataStream2Xcoef.readLine();
/*
        opers = dataStr.split(r3data.colSep);
        X << opers.at(0).toDouble();
        paramsNum = opers.size() - 1;
        X << dataStr.section(r3data.colSep, 0, 0);
        dX << dataStr.section(r3data.colSep, 1, 1);/
    }

    dNum = coefLines.size();
    opers = coefLines.at(0).split(dataSep);
    coefNum = opers.size() - 1;

    qDebug() << QString("coefNum= %1\tdNum= %2\n").arg(coefNum).arg(dNum);

    for(i=1; i<coefNum+1; i++)
    {
        x = new double[dNum];
        dx = new double[dNum];

        for(j=0; j<dNum; j++)
        {
            opers = coefLines.at(j).split(dataSep);
            x[j] = opers.at(0).toDouble();
            dx[j] = opers.at(i).toDouble();

            qDebug() << QString("%1: x= %2\tdx=%3\n").arg(j).arg(x[j]).arg(dx[j]);
        }


        mCoeffX = new double[coefDeg+1];

        detCurf3(&resFunc, mCoeffX, x, dx, dNum, coefDeg);

        qDebug() << QString("resFunc M: %1\n").arg(resFunc);

        dataStrL.clear();
        //dataStrL << QString("%1").arg((magEq.diaps[k]+magEq.diaps[k+1])/2.0);
        for(j=0; j<coefDeg+1; j++)
        {
             dataStrL << QString("%1").arg(mCoeffX[j], 10, 'e');
        }

        dataStream << dataStrL.join(dataSep) << "\n";



        delete [] mCoeffX;
        delete [] x;
        delete [] dx;
    }

    r2FileXcoef.close();
    rFile.close();
}
*/
void diapResiduals::initMesList(QList <measurementRec*> mesList)
{
    int szRes, i, j, k, szi;
    measurementRec* mesRec;

    qDebug() << QString("szRes: %1\n").arg(szRes);
    szi = mesList.size();
    for(i=0; i<szi; i++)
    {
        mesRec = mesList.at(i);
        szRes = mesRec->resList.size();
        for(j=0; j<szRes; j++)
        {
            for(k=0; k<diapsNum-1; k++)
            {
                if(mesRec->resList.at(j)->mag>diaps[k]&&mesRec->resList.at(j)->mag<diaps[k+1])
                {
                    resListDiap.at(k)->resList << mesRec->resList.at(j);
                }
            }
        }
    }
}
