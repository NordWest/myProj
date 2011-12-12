#include <QCoreApplication>
#include <QtCore>
/*
#include <QTextStream>
#include <QDataStream>
#include <QFile>
*/
#include "./../libs/uneven.h"
#include "./../libs/redStat.h"
#include "./../libs/observatory.h"
#include "./../libs/vectGrid3D.h"
#include "./../libs/fitsdata.h"
#include "./../libs/comfunc.h"
#include "./../libs/mpccat.h"


//#include <mgl/mgl_eps.h>

static QDataStream* clog1 = 0;
void customMessageHandler(QtMsgType type, const char* msg)
{
    static const char* msgType[] =
    {
        "Debug    : ",
        "Warning  : ",
        "Critical : ",
        "Fatal    : "
    };

 //   static QTextStream cout1(stdout);
 //   static QTextStream cerr(stderr);

//    cerr << msgType[type] << msg << endl;
    if(clog1 && clog1->device())
        *clog1 << type << msg;
    if(type == QtFatalMsg)
    {
       // cerr << "aborting..." << endl;

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

//void mCoefFunc(QString coefFileName, QString mCoefFileName, QString dataSep, int coefDeg);
int initMdCoef(QString mdCorr, QList<double*> *mdCoef, int *mdCoefsz, QString colSep);
void detCoefM(QVector <double> *dCoef, QList <double*> mdCoef, int mdCoefsz, double mag);
int detDcorr(int corrModel, double x, double y, double *dx, double *dy, QVector <double> dCoefX, QVector <double> dCoefY);

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
    int mDegree;
    int doMcoef;
};

struct report12data
{
    double x0, x1;
    double y0, y1;
    cleanParam cp;
    int redDeg;
};

struct report4data
{
    double x0, x1, y0, y1;  //РґРёР°РїР°Р·РѕРЅС‹
    int xLevNum, yLevNum;             //РєРѕР»-РІРѕ РєР»РµС‚РѕРє РїРѕ РѕСЃСЏРј
    int isFindDiap;
//    QString labelX, labelY, plotParam;
//    int isDetCurv, curvDeg;
    int isSysCorr;
    int doWeght;
    QString sysCorrFile;
    int nmin;
    double rMax;
};

struct report50data
{
    double x0, x1, y0, y1;  //РґРёР°РїР°Р·РѕРЅС‹
    int xLevNum, yLevNum;             //РєРѕР»-РІРѕ РєР»РµС‚РѕРє РїРѕ РѕСЃСЏРј
    int isFindDiap;
    QString mdCorrX, mdCorrY;
//    QString labelX, labelY, plotParam;
//    int isDetCurv, curvDeg;
    int isSysCorr;
    QString colSep;
    int corrModel;
};

void report12(report12data r12data, QList <measurementRec*> &mesList, QString reportDirName);
void report13(QList<measurementRec*> &mesList, QString reportDirName);


int main(int argc, char *argv[])    //r3StatPL
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


    QTextStream dataStream, dataStream1;
    //QFile rFile(argv[1]);
    //rFile.open(QIODevice::ReadOnly| QIODevice::Text);
    //dataStream.setDevice(&rFile);

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog1 = new QDataStream(logFile);

    //QTextCodec *codec2 = QTextCodec::codecForName("Windows-1251");

    diapResiduals magEq;
    reductionStat rStat;
    platesStat plStat;
    QList <measurementRec*> mesList;
    //platesStat plStatMPC;
    //platesStat plStatMPCr;


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
    //double dX, dY;

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
    double xc, yc;


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
    QString report51Dir;
    QString report4Dir;
    QString report6Dir;

    QString tStr;
    QFile rFileX, rFileY, rFileM;
    QTextStream dataStreamX, dataStreamY, dataStreamM;

    QFile r2FileX;
    QTextStream dataStream2X;
    QString r3NameX;

    QFile r2FileY;
    QTextStream dataStream2Y;
    QString r3NameY;

    QString resFunc;

    double *x, *y, *mX, *mY;
    double *dx, *dy, *dmX, *dmY;
    double val;
    int p;
    int dotNum;
    QStringList dataStrL;


    double meanKsi, meanEta, rmsKsi, rmsEta;
    int recNum = 0;
    int i;
    meanKsi = meanEta = rmsKsi = rmsEta = 0.0;

    QString workPath;

    errBudgetFile *errB;
    int szi, szj, szk;
    int j, k;
    QFile r1File;

    QFile r3File;
    QTextStream dataStream3;
    QFile rFile;
    residualFile* resFile;
    QString *ebFileName, *resFileName, *objFileName;//, *ssFileName;
    QString dataStr;
    measurementRec* mesRec;
    objectsStat objPStat;
    QString versName;
    int szRes;
    double t0, t1;
    t1 = 0;
    t0 = 60000.0;
    int year, mth;
    double day, pyear;
    int pMin;
    //QTextCodec *codec1 = QTextCodec::codecForName("KOI8-R");
    //Q_ASSERT( codec1 );
/////////////////////// sett    /////////////////////////////////////////
    QSettings *sett = new QSettings("r3StatPL.ini", QSettings::IniFormat);
    //QString workDir = sett->value("general/workDir", ".").toString();
    if(argc==1) workPath = sett->value("general/workPath", "./").toString();
    else workPath = QString(argv[1]);
    QDir workDir(workPath);
    workPath = workDir.absolutePath()+QDir().separator();
    qDebug() << QString("workPath: %1\n").arg(workPath);
    //int taskNum = sett->value("general/taskNum", 0).toInt();
    //int redType = sett->value("general/redType", 0).toInt();            //0 - common; 1 - obj

    int plNameType = sett->value("general/plNameType", 0).toInt();

    QString obsCat = sett->value("general/observCat", ".").toString();

    int isSysCorr = sett->value("general/isSysCorr", 0).toInt();
    QString sysCorrFile = sett->value("general/sysCorrFile").toString();
    int isFindDiap = sett->value("diaps/isFindDiap").toInt();
    double expMin = sett->value("general/expmin", 180).toDouble();

    int isUsePlatesList = sett->value("general/isUsePlatesList").toInt();
    QString plFName = sett->value("general/plFName", "./plates.txt").toString();
    QString excMesFile = sett->value("general/excMesFile", "./excMes.txt").toString();

    double mgEqSigma = sett->value("general/mgEqSigma", 3.0).toDouble();
    QString colSep = sett->value("general/colSep").toString();




    int isReportObj = sett->value("reports/isReportObj", 0).toInt();
    int isReport0 = sett->value("reports/isReport0", 0).toInt();
    int isReport1 = sett->value("reports/isReport1", 0).toInt();
    int isReport11 = sett->value("reports/isReport11", 0).toInt();
    int isReport12 = sett->value("reports/isReport12", 0).toInt();
    int isReport13 = sett->value("reports/isReport13", 0).toInt();
    int isReport2 = sett->value("reports/isReport2", 0).toInt();
    int isReport3 = sett->value("reports/isReport3", 0).toInt();
    int isReport4 = sett->value("reports/isReport4", 0).toInt();
    int isReport5 = sett->value("reports/isReport5", 0).toInt();
    int isReport50 = sett->value("reports/isReport50", 0).toInt();
    int isReport51 = sett->value("reports/isReport51", 0).toInt();
    int isReport6 = sett->value("reports/isReport6", 0).toInt();
    int isReport60 = sett->value("reports/isReport60", 0).toInt();
    int isReport61 = sett->value("reports/isReport61", 0).toInt();

    int isMeasurementsTxt = sett->value("reports/isMeasurementsTxt", 1).toInt();
    int isMesReports = sett->value("reports/isMesReports", 1).toInt();


    double objSigma = sett->value("reportObj/objSigma", 3.0).toDouble();
    double objAggSigma = sett->value("reportObj/objAggSigma", 0.0).toDouble();
    int saveEq = sett->value("reportObj/saveEq", 0).toInt();
    int cCols = sett->value("reportObj/cCols", 0).toInt();
    int saveMpc = sett->value("reportObj/saveMpc", 0).toInt();
    int saveAgg = sett->value("reportObj/saveAgg", 0).toInt();
    QString mpcOrbCat = sett->value("reportObj/mpcOrbCat", "").toString();
    int isSeries = sett->value("reportObj/isSeries", 0).toInt();
    int d3s2 = sett->value("reportObj/d3s2", 0).toInt();
    int d3s3 = sett->value("reportObj/d3s3", 0).toInt();
    int d3s4 = sett->value("reportObj/d3s4", 0).toInt();

    int isMinUWE = sett->value("report0/isMinUWE", 0).toInt();
    int isVersSeq = sett->value("report0/isVersSeq", 0).toInt();
    int isDropObj = sett->value("report0/isDropObj", 0).toInt();
    QString versSeq = sett->value("report0/versSeq", "33|32|23|22|21").toString();
    int minObj = sett->value("report0/minObj", 0).toInt();


    int r1aveNum = sett->value("report1/aveNum", 1).toInt();

    report12data r12data;
    r12data.x0 = sett->value("report12/x0", 0).toDouble();
    r12data.x1 = sett->value("report12/x1", 20000).toDouble();
    r12data.y0 = sett->value("report12/y0", 0).toDouble();
    r12data.y1 = sett->value("report12/y1", 20000).toDouble();
    r12data.cp.xi = sett->value("report12/Xi", 4).toInt();
    r12data.cp.g = sett->value("report12/g", 0.95).toDouble();
    r12data.cp.q = sett->value("report12/q", 0.1).toDouble();
    r12data.redDeg = sett->value("report12/redDeg", 1).toInt();

    report3data r3data;
    r3data.isDetCurv = sett->value("report3/isDetCurv", 0).toInt();
    r3data.curvDeg = sett->value("report3/curvDeg", 2).toInt();
    r3data.aveNum = sett->value("report3/aveNum", 2).toInt();
    qDebug() << QString("aveNum= %1\n").arg(r3data.aveNum);
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

    report50data r50data;
    r50data.isSysCorr = sett->value("report50/isSysCorr", 0).toInt();
    r50data.mdCorrX = sett->value("report50/mdCorrX").toString();
    r50data.mdCorrY = sett->value("report50/mdCorrY").toString();
    r50data.colSep = sett->value("report50/colSep").toString();
    r50data.corrModel = sett->value("report50/corrModel", 0).toInt();
    r50data.isFindDiap = sett->value("report50/isFindDiap", 0).toInt();
    r50data.x0 = sett->value("report50/naxeX0", 0).toDouble();
    r50data.x1 = sett->value("report50/naxeX1", 0).toDouble();
    r50data.y0 = sett->value("report50/naxeY0", 0).toDouble();
    r50data.y1 = sett->value("report50/naxeY1", 0).toDouble();
    r50data.xLevNum = sett->value("report50/xLevNum", 1).toDouble();
    r50data.yLevNum = sett->value("report50/yLevNum", 1).toDouble();

    report4data r51data;
    r51data.isFindDiap = sett->value("report51/isFindDiap").toInt();
    r51data.x0 = sett->value("report51/naxeX0", 0).toDouble();
    r51data.x1 = sett->value("report51/naxeX1", 0).toDouble();
    r51data.y0 = sett->value("report51/naxeY0", 0).toDouble();
    r51data.y1 = sett->value("report51/naxeY1", 0).toDouble();
    r51data.xLevNum = sett->value("report51/xLevNum", 1).toDouble();
    r51data.yLevNum = sett->value("report51/yLevNum", 1).toDouble();
    r51data.isSysCorr = sett->value("report51/isSysCorr", 0).toInt();
    r51data.sysCorrFile = sett->value("report51/sysCorrFile").toString();
    r51data.doWeght = sett->value("report51/doWeght", 0).toInt();
    r51data.nmin = sett->value("report51/nmin", 0).toInt();
    r51data.rMax = sett->value("report51/rMax", 0).toDouble();


    QString diapStr = sett->value("diaps/magDiaps").toString();
/////////////////////////////////////////////////////////////////////////////

    QTextStream excStm;
    QFile excFile(excMesFile);
    excFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    excStm.setDevice(&rFile);
    QStringList excList;
    excList = excStm.readAll().split("\n");
    excFile.close();


    ebFileName = resFileName = objFileName = NULL;

    QString reportDirName = workPath+"reports"+QDir().separator();
    QDir reportDir(workPath);
    reportDir.mkdir("reports");
    reportDir.cd("./reports");

    ebFileName = new QString(workPath + "err_budget.txt");
    resFileName = new QString(workPath + "residuals.txt");
    objFileName = new QString(workPath + "objRes.txt");
/*
    if(!redType)
    {
        ebFileName = new QString(workPath + "err_budget.txt");
        resFileName = new QString(workPath + "residuals.txt");
    }
    else
    {
        ebFileName = new QString(workPath + "err_budget_obj.txt");
        resFileName = new QString(workPath + "residuals_obj.txt");
        mpeFileName = new QString(workPath + "aggregate_eq.txt");
        ssFileName = new QString(workPath + "aggregate_sstar.txt");
    }
*/
//    qDebug() << QString("ebFileName: %1\n").arg(*ebFileName);

    //reductionStat rStat;
    rStat.init(ebFileName, resFileName, objFileName);
    rStat.removeMesList(excList);


    //qDebug() << QString("ssFile->ocList.size= %1").arg(rStat.ssFile->ocList.size());



    QStringList mdList = diapStr.split("|");

    magEq.diapsNum = mLevNum = mdList.size();
    magEq.diaps = new double[magEq.diapsNum];


    for(i=0; i<magEq.diapsNum-1; i++)
    {
        magEq.diaps[i] = mdList.at(i).toDouble();
        resFile = new residualFile;
        magEq.resListDiap << resFile;
    }

    magEq.diaps[magEq.diapsNum-1] = mdList.at(magEq.diapsNum-1).toInt();




    //measurementRec* sRec;
    //QTextStream dataStream;



/*
    r1File.setFileName(reportDirName+"reportTot.txt");
    r1File.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    dataStream1.setDevice(&r1File);
*/

    //QString plateName;


    //eqPlatesStat eqPStat;



    measurementStatRec* msRec = new measurementStatRec;

    if(isUsePlatesList) plStat.initPlatesNamesFile(plFName);
    qDebug() << QString("part plates size= %1\n").arg(plStat.platesList.size());
    plStat.init(rStat.statList, plNameType);
    qDebug() << QString("part plates size1= %1\n").arg(plStat.platesList.size());



//    int pk=0;
//do
//{
    //qDebug() << QString("ssFile->ocList.size= %1").arg(rStat.ssFile->ocList.size());




    reductionStat rStatSel;

    /*



    switch(redType)
    {
    case 0:
        {
           /* if(isReport0)
            {
                rFile.setFileName(reportDirName+"report0.txt");
                rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
                dataStream.setDevice(&rFile);
            }*/


/*
            szi = plStat.platesList.size();
            for(i=0; i<szi; i++)
            {
                //msRec = NULL;
                qDebug() << "msRec: " << msRec << "\n";

                //plStat.platesList.at(i)->getMinUWE(msRec);
                pMin = plStat.platesList.at(i)->getMinUWEpos();
                if(pMin!=-1)
                //qDebug() << "msRec: " << msRec << "\n";
                //if(msRec!=NULL)
                {
                    msRec->copy(plStat.platesList.at(i)->mStatList.at(pMin));

                    qDebug() << QString("msRec mesureTimeCode: %1\n").arg(msRec->mesureTimeCode);
                    mesRec = rStat.getMeasurement(msRec->mesureTimeCode);
                    qDebug() << "mesRec: " << mesRec << "\n";
                    mesList << mesRec;

                    msRec->getVersName(&versName);
                    //versName = msRec->originName.section(".", -1, -1);
                    //if(versName=="aver") versName = msRec->originName.section(".", -2, -2);
                    qDebug() << QString("plateName: %1\tversName: %2\n").arg(plStat.platesList.at(i)->plateName).arg(versName);

                    dat2YMD(mjd2jd(mesRec->errBud->MJD), &year, &mth, &day);
                    pyear = partOfYear(year, mth, day);

                    dataStr = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12").arg(plStat.platesList.at(i)->plateName).arg(pyear, 7, 'f', 2).arg((int)mesRec->errBud->MJD).arg(versName).arg(msRec->Nx).arg(msRec->UWEx).arg(msRec->Ny).arg(msRec->UWEy).arg(msRec->rmsOneOCksi).arg(msRec->rmsOneOCeta).arg(msRec->rmsMeanOCksi).arg(msRec->rmsMeanOCeta);

                    szj = mesRec->mpeList.size();
                    for(j=0; j<szj; j++)dataStr.append(QString("|%1|%2|%3").arg(mesRec->mpeList.at(j)->ocRaCosDe).arg(mesRec->mpeList.at(j)->ocDe).arg(mesRec->mpeList.at(j)->name.simplified()));

                    szj = mesRec->sstarList.size();
                    for(j=0; j<szj; j++)dataStr.append(QString("|%1|%2|%3").arg(mesRec->sstarList.at(j)->ksiOC).arg(mesRec->sstarList.at(j)->etaOC).arg(mesRec->sstarList.at(j)->lspmName.simplified()));
                    dataStr.append(QString("|%1").arg(msRec->originName));


                    ///////////////////////////////
                    szRes = mesRec->resList.size();
                    qDebug() << QString("szRes: %1\n").arg(szRes);
                    for(j=0; j<szRes; j++)
                    {
                        for(k=0; k<magEq.diapsNum-1; k++)
                        {
                            if(mesRec->resList.at(j)->mag>magEq.diaps[k]&&mesRec->resList.at(j)->mag<magEq.diaps[k+1])
                            {
                                magEq.resListDiap.at(k)->resList << mesRec->resList.at(j);
                            }
                        }
                    }

                    if(mesRec->errBud->MJD<t0) t0 = mesRec->errBud->MJD;
                    if(mesRec->errBud->MJD>t1) t1 = mesRec->errBud->MJD;
            /*
                   // if(redType)
                   // {
                        szk = plStat.platesList.at(i)->mStatList.size();
                        for(k=0; k<szk; k++)
                        {
                            msRec = plStat.platesList.at(i)->mStatList.at(k);
                            mesRec = rStat.getMeasurement(msRec->mesureTimeCode);
                            msRec->getVersName(&versName);
                            dataStr = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10").arg(plStat.platesList.at(i)->plateName).arg(versName).arg(msRec->Nx).arg(msRec->UWEx).arg(msRec->Ny).arg(msRec->UWEy).arg(msRec->rmsOneOCksi).arg(msRec->rmsOneOCeta).arg(msRec->rmsMeanOCksi).arg(msRec->rmsMeanOCeta);

                            szj = mesRec->mpeList.size();
                            for(j=0; j<szj; j++)dataStr.append(QString("|%1|%2|%3").arg(mesRec->mpeList.at(j)->ocRa).arg(mesRec->mpeList.at(j)->ocDe).arg(mesRec->mpeList.at(j)->name.simplified()));

                            szj = mesRec->sstarList.size();
                            for(j=0; j<szj; j++)dataStr.append(QString("|%1|%2|%3").arg(mesRec->sstarList.at(j)->ksiOC).arg(mesRec->sstarList.at(j)->etaOC).arg(mesRec->sstarList.at(j)->lspmName.simplified()));
                            dataStr.append(QString("|%1").arg(msRec->originName));
                            dataStream1 << dataStr << "\n";
                        }
                   // }
            /
                }
                else
                {
                    dataStr = QString("%1").arg(plStat.platesList.at(i)->plateName);

                }
                if(isReport0) dataStream << dataStr << "\n";
            }
            /
  //  pk++;
//}while(pk<2);
        }
        break;
        /*
    case 1:
        {
            //if(isUsePlatesList) plStatMPC.initPlatesNamesFile(plFName);
            //qDebug() << QString("part plates size= %1\n").arg(plStatMPC.platesList.size());
            //plStatMPC.init(rStat.statList, plNameType);
            //qDebug() << QString("part plates size1= %1\n").arg(plStatMPC.platesList.size());
/*
            if(isReport0)
            {
                rFile.setFileName(reportDirName+"report0mpc.txt");
                rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
                dataStream.setDevice(&rFile);
            }
/
            eqFile *astStat = new eqFile;
            qDebug() << QString("0: ssFile->ocList.size= %1").arg(rStat.ssFile->ocList.size());
            eqPStat.init(&rStat, plNameType);
            qDebug() << QString("1: ssFile->ocList.size= %1").arg(rStat.ssFile->ocList.size());
            szi = eqPStat.asterList.size();
            qDebug() << QString("asterList.size= %1").arg(szi);

            QString reportAsterDir = reportDirName + "reportAster";
            reportDir.mkdir(reportAsterDir);
            eqFile *eqTemp = new eqFile;
            for(i=0; i<szi; i++)
            {
                eqTemp->clear();
                plStatMPCr.platesList.clear();
                plStatMPC.init(eqPStat.asterList.at(i)->mStatList, plNameType);
                plStatMPCr.init(eqPStat.asterList.at(i)->mStatList, plNameType);

                if(isVersSeq) plStatMPCr.saveReport0Seq(QString("%1/%2.r0.txt").arg(reportAsterDir).arg(eqPStat.asterList.at(i)->asterName.simplified()), versSeq, excList,  plNameType, &rStat);
                else plStatMPCr.saveReport0(QString("%1/%2.r0.txt").arg(reportAsterDir).arg(eqPStat.asterList.at(i)->asterName.simplified()), isMinUWE, plNameType, &rStat);
                //plStatMPCr.saveReport0(QString("%1/%2.r0.txt").arg(reportAsterDir).arg(eqPStat.asterList.at(i)->asterName.simplified()), isMinUWE, plNameType, &rStat);

                szj = eqPStat.asterList.at(i)->plStat->platesList.size();
                for(j=0; j<szj; j++)
                {
                    eqPStat.asterList.at(i)->plStat->platesList.at(j)->getMinUWE(msRec);
                    qDebug() << "msRec: " << msRec << "\n";
                    qDebug() << "msRec timeCode: " << msRec->mesureTimeCode << "\n";
                    mesRec = rStat.getMeasurement(msRec->mesureTimeCode);
                    qDebug() << "mesRec: " << mesRec << "\n";
                    if(mesRec!=NULL) eqTemp->ocList << mesRec->mpeList;
                }
                eqTemp->countCols("4,5,6");
                eqTemp->saveAs(QString("%1/%2_eq.txt").arg(reportAsterDir).arg(eqPStat.asterList.at(i)->asterName.simplified()), 2);
                /*
                szj = eqPStat.asterList.at(i)->plStat->platesList.size();
                qDebug() << QString("platesList.size= %1").arg(szj);
                for(j=0; j<szj; j++)
                {
                    //eqPStat.asterList.at(i)->plStat->platesList.at(j)->getMinUWE(msRec);
                    pMin = eqPStat.asterList.at(i)->plStat->platesList.at(j)->getMinUWEpos();
                    if(pMin!=-1) msRec = eqPStat.asterList.at(i)->plStat->platesList.at(j)->mStatList.at(pMin);
                    qDebug() << "msRec: " << msRec << "\n";
                    if(msRec!=NULL)
                    {
                        qDebug() << QString("msRec mesureTimeCode: %1\n").arg(msRec->mesureTimeCode);
                        mesRec = rStat.getMeasurement(msRec->mesureTimeCode);
                        mesList << mesRec;
                        for(k=0; k<mesRec->mpeList.size(); k++) astStat->ocList << mesRec->mpeList.at(k);

                        msRec->getVersName(&versName);

                        dat2YMD(mjd2jd(mesRec->errBud->MJD), &year, &mth, &day);
                        pyear = partOfYear(year, mth, day);

                        dataStr = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12").arg(eqPStat.asterList.at(i)->plStat->platesList.at(j)->plateName).arg(pyear, 7, 'f', 2).arg((int)mesRec->errBud->MJD).arg(versName).arg(msRec->Nx).arg(msRec->UWEx).arg(msRec->Ny).arg(msRec->UWEy).arg(msRec->rmsOneOCksi).arg(msRec->rmsOneOCeta).arg(msRec->rmsMeanOCksi).arg(msRec->rmsMeanOCeta);

                        szk = mesRec->mpeList.size();
                        for(k=0; k<szk; k++)dataStr.append(QString("|%1|%2|%3").arg(mesRec->mpeList.at(k)->ocRa).arg(mesRec->mpeList.at(k)->ocDe).arg(mesRec->mpeList.at(k)->name.simplified()));

                        szk = mesRec->sstarList.size();
                        for(k=0; k<szk; k++)dataStr.append(QString("|%1|%2|%3").arg(mesRec->sstarList.at(k)->ksiOC).arg(mesRec->sstarList.at(k)->etaOC).arg(mesRec->sstarList.at(k)->lspmName.simplified()));
                        dataStr.append(QString("|%1").arg(msRec->originName));
                        //if(isReport0) dataStream << dataStr << "\n";

                        ///////////////////////////////
                        szRes = mesRec->resList.size();
                        qDebug() << QString("szRes: %1\n").arg(szRes);
                        for(int p=0; p<szRes; p++)
                        {
                            for(k=0; k<magEq.diapsNum-1; k++)
                            {
                                if(mesRec->resList.at(j)->mag>magEq.diaps[k]&&mesRec->resList.at(p)->mag<magEq.diaps[k+1])
                                {
                                    magEq.resListDiap.at(k)->resList << mesRec->resList.at(p);
                                }
                            }
                        }

                        if(mesRec->errBud->MJD<t0) t0 = mesRec->errBud->MJD;
                        if(mesRec->errBud->MJD>t1) t1 = mesRec->errBud->MJD;
                    }
                    else
                    {
                        dataStr = QString("%1").arg(eqPStat.asterList.at(i)->plStat->platesList.at(j)->plateName);

                    }
                    if(isReport0) dataStream << dataStr << "\n";
                }
                /
            }

            if(isVersSeq) plStatMPC.saveReport0Seq(reportDirName+"report0mpc.txt", versSeq, excList,  plNameType, &rStat, &mesList);
            else plStatMPC.saveReport0(reportDirName+"report0mpc.txt", isMinUWE, plNameType, &rStat, &mesList);

            //plStatMPC.saveReport0(reportDirName+"report0mpc.txt", isMinUWE, plNameType, &rStat, &mesList);
            rStat.getMeasurementsList(mesList, &rStatSel);

            magEq.initMesList(mesList);

            szi = mesList.size();
            for(i=0; i<szi; i++)
            {
                mesRec = mesList.at(i);
                for(k=0; k<mesRec->mpeList.size(); k++) astStat->ocList << mesRec->mpeList.at(k);
            }


            astStat->countCols("4,5,6");
            astStat->saveAs(reportDirName+"agg_eq.txt");



            if(isReport0)
            {
                rFile.close();
                rFile.setFileName(reportDirName+"report0sstar.txt");
                rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
                dataStream.setDevice(&rFile);
            }

            qDebug() << "\nsstarFile\n";
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
                        for(k=0; k<mesRec->sstarList.size(); k++) ssFile->ocList << mesRec->sstarList.at(k);
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
            ssFile->saveAs(reportDirName+"agg_sstar.txt");

        }
        break;*/
/* case 1:
                {*/
/*
    QString tfName;
                    sstarFile ssF;
                    eqFile eqF;
                    objPStat.init(&rStat, plNameType);
                    szi = objPStat.objList.size();
                    qDebug() << QString("objList.size= %1").arg(szi);

                    QString reportObjDir = reportDirName + "reportObj";
                    reportDir.mkdir(reportObjDir);
                    szi = objPStat.objList.size();
                    qDebug() << QString("objects size: %1\n").arg(szi);

                    eqFile aggEqF;
                    sstarFile aggSsF;
                    QFile mpcFile;
                    QFile mpcAggFile;
                    QTextStream mpcStm, mpcAggStm;
                    int objNum;
                    QString tstr;

                    mpccat mpoCat;
                    if(saveMpc)
                    {
                        if(mpoCat.init(mpcOrbCat.toAscii().data())) qDebug() << QString("mpoCat init error!\n");

                        mpcAggFile.setFileName(reportObjDir+"/agg_mpc.txt");
                        mpcAggFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
                        mpcAggStm.setDevice(&mpcAggFile);
                    }

                        for(i=0; i<szi; i++)
                        {
                            eqF.clear();
                            ssF.clear();
                            //szj = objPStat.objList.at(i);
                            qDebug() << QString("objName: %1\n").arg(objPStat.objList.at(i)->objName);
                            if(objSigma>0.0) objPStat.objList.at(i)->do3Sigma(&rStat, objSigma);
                            objPStat.objList.at(i)->getMpList(&rStat, &eqF.ocList);
                            objPStat.objList.at(i)->getSstarList(&rStat, &ssF.ocList);

                            qDebug() << QString("eq size: %1\n").arg(eqF.ocList.size());
                            tfName = QString("%1/%2").arg(reportObjDir).arg(objPStat.objList.at(i)->objName.simplified());
                            if(eqF.ocList.size()>0)
                            {

                                if(cCols)eqF.countCols("4,5,6");
                                if(saveEq)
                                {
                                    eqF.saveAs(tfName+"_eq.txt");
                                    if(saveAgg) aggEqF.ocList << eqF.ocList;
                                }
                                if(saveMpc)
                                {
                                    mpcFile.setFileName(tfName+"_mpc.txt");
                                    mpcFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
                                    mpcStm.setDevice(&mpcFile);

                                    if(mpoCat.GetRecName(objPStat.objList.at(i)->objName.simplified().toAscii().data()))
                                    {
                                        qDebug() << QString("objName is not found\n");
                                        objNum = 0;
                                    }
                                    else
                                    {
                                        objNum = mpoCat.record->getNum();
                                        qDebug() << QString("objNum: %1\n").arg(objNum);
                                        //qDebug() << QString("number: %1\n").arg(mpoCat.record->number);
                                    }

                                    szj = eqF.ocList.size();
                                    for(j=0; j<szj; j++)
                                    {
                                        eqF.ocList.at(j)->rec2MPC(&tstr, "084", objNum);
                                        mpcStm << tstr << "\n";
                                        if(saveAgg) mpcAggStm << tstr << "\n";
                                    }

                                    mpcFile.close();
                                }
                            }


                            if(ssF.ocList.size()>0)
                            {
                                //tfName = QString("%1/%2").arg(reportObjDir).arg(objPStat.objList.at(i)->objName.simplified());
                                if(cCols)eqF.countCols("6,7,8");
                                //if(saveEq)
                                //{
                                    ssF.saveAs(tfName+"_sstar.txt");
                                    if(saveAgg) aggSsF.ocList << ssF.ocList;
                                //}

                            }



                        }

                    if(saveAgg)
                    {
                        qDebug() << "saveAgg\n";
                        if(saveEq)
                        {
                            qDebug() << "saveEq\n";
                            if(cCols)aggEqF.countCols("4,5,6");

                            aggEqF.saveAs(reportObjDir+"/agg_eq.txt");
                        }
                        if(objAggSigma>0.0)
                        {
                            qDebug() << "aggSsF.do3sigma\n";
                            aggSsF.do3sigma(0.0, objSigma);
                        }
                        if(cCols)
                        {
                            qDebug() << "cCols\n";
                            aggSsF.countCols("6,7,8");
                        }

                        aggSsF.saveAs(reportObjDir+"/agg_sstar.txt");
                    }

                    if(saveMpc) mpcAggFile.close();



                }
            /*    break;
    }
*/
    if(isDropObj) plStat.dropObj(rStat);
    if(isVersSeq) plStat.selVersSeq(versSeq.split("|"), &rStat, &mesList, minObj);
    else  if(isMinUWE) plStat.selMinUWE(&rStat, &mesList);
    else plStat.selMesList(&rStat, &mesList);
    //if(isVersSeq) plStat.saveReport0Seq(reportDirName+"report0.txt", versSeq, excList,  plNameType, &rStat, &mesList);
    //else plStat.saveReport0(reportDirName+"report0.txt", isMinUWE, plNameType, &rStat, &mesList);
    if(isReport0) saveReport0(reportDirName+"report0.txt", mesList, plNameType);

    rStat.getMeasurementsList(mesList, &rStatSel);

    magEq.initMesList(mesList);

    //if(isReport0) rFile.close();
    //r1File.close();


//mesList

    if(isMeasurementsTxt)
    {
        rFile.setFileName(reportDirName+"measurements.txt");
        rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
        dataStream.setDevice(&rFile);
    }

    if(isMesReports)
    {
        reportDir.remove("err_budget.txt");
        reportDir.remove("residuals.txt");
        reportDir.remove("objRes.txt");

        rStatSel.saveMesList(reportDirName+"err_budget.txt", reportDirName+"residuals.txt", reportDirName+"objRes.txt");
/*
        if(!redType)
        {
            rStatSel.saveMesList(reportDirName+"err_budget.txt", reportDirName+"residuals.txt");
        }
        else
        {
            rStatSel.saveMesList(reportDirName+"err_budget_obj.txt", reportDirName+"residuals_obj.txt", reportDirName+"aggregate_eq.txt", reportDirName+"aggregate_sstar.txt");
        }
        */
    }

    szi = mesList.size();

//residualFile *resFile;


    if(isMeasurementsTxt)
    {
        //errB = new errBudgetFile;
        for(i=0; i<szi; i++)
        {
            mesRec = mesList[i];
            mesRec->errBud->rec2s(&dataStr);
            dataStream << QString("#%1\n").arg(dataStr);
/*
            if(isMesReports)
            {
                resFile->resList << mesRec->resList;

                errB->errList << mesRec->errBud;
            }
*/
  //          if(isMeasurementsTxt)
  //          {
                szj = mesRec->resList.size();
                for(j=0; j<szj; j++)
                {
                    mesRec->resList[j]->rec2s(&dataStr);
                    dataStream << QString("%1\n").arg(dataStr);
                }

                szj = mesRec->objList.size();
                for(j=0; j<szj; j++)
                {
                    mesRec->objList[j]->rec2s(&dataStr);
                    dataStream << QString("%1\n").arg(dataStr);
                }
                /*
                szj = mesRec->sstarList.size();
                for(j=0; j<szj; j++)
                {
                    mesRec->sstarList[j]->rec2s(&dataStr);
                    dataStream << QString("%1\n").arg(dataStr);
                }
                */
    //        }
        }
    }
 /*   if(isMesReports)
    {
        resFile->save();
        errB->save();
    }
*/


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

    rFile.setFileName(reportDirName+"mgEq.txt");
    rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStream.setDevice(&rFile);

    for(k=0; k<magEq.diapsNum-1; k++)
    {
        if(mgEqSigma>1e-2) magEq.resListDiap.at(k)->remSigma(mgEqSigma);
        //magEq.resListDiap.at(k)->detStat();
        magEq.resListDiap.at(k)->detStat();

       dataStrL.clear();

       dataStrL << QString("%1").arg((magEq.diaps[k+1]+magEq.diaps[k])/2.0);
       dataStrL << QString("%1").arg(magEq.resListDiap.at(k)->meanKsi);
       dataStrL << QString("%1").arg(magEq.resListDiap.at(k)->rmsMeanKsi);
       dataStrL << QString("%1").arg(magEq.resListDiap.at(k)->meanEta);
       dataStrL << QString("%1").arg(magEq.resListDiap.at(k)->rmsMeanEta);
       dataStrL << QString("%1").arg(magEq.resListDiap.at(k)->resList.size());
  //     dataStrL << QString("%1")
       dataStream << dataStrL.join(colSep) << "\n";
    }

    r1File.setFileName(reportDirName+"ocMagDisp.txt");
    r1File.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStream1.setDevice(&r1File);

    r3File.setFileName(reportDirName+"magOC.txt");
    r3File.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStream3.setDevice(&r3File);





    if(isReportObj)
    {
        QString reportObjDir = reportDir.absolutePath() + reportDir.separator() + "objStat";
        reportDir.mkdir(reportObjDir);
        reportDir.mkdir(reportObjDir+QDir().separator()+"mpeph");
        QString mpeDir(reportObjDir+QDir().separator()+"mpeph");
        reportDir.mkdir(reportObjDir+QDir().separator()+"sstar");
        QString sstarDir(reportObjDir+QDir().separator()+"sstar");
        if(isSeries)
        {
            reportDir.mkdir(reportObjDir+QDir().separator()+"eqSeries");
            reportDir.mkdir(reportObjDir+QDir().separator()+"orSeries");
        }

        objResidualFile objTemp;// = new objResidualFile;
        objResidualFile objAggTemp;// = new objResidualFile;
        objResidualFile objRejTemp;
        objResidualFile objRejAgg;
        objResidualFile *orTempS = new objResidualFile;
        objResidualFile *orTotS = new objResidualFile;
        objSeries serTemp;
        objResRec* orRec;
        QList <objResidualFile*> orList;

        eqFile eqTemp;// = new objResidualFile;
        eqFile aggEqTemp;// = new objResidualFile;
        eqFile rejEqTemp;
        eqFile rejEqTot;
        eqFile *eqTempS = new eqFile;
        eqFile *eqTotS = new eqFile;
        QList <eqFile*> eqList;

        ocRec* ocR;

        sstarFile ssTemp;
        sstarFile aggSsTemp;

        objectsStat objStat;
        objStat.init(&rStat, plNameType);
        szi = objStat.objList.size();
        qDebug() << QString("obj num: %1\n").arg(szi);

        QFile mpcAggFile, mpcFile;
        QFile ssAggFile, ssFile;
        QTextStream mpcAggStm, mpcStm, ssAggStm;
        QString tfName, objName, tstr, catName;

        ///eqFile eqF;
        int objNum;

        ocRec *ocrec;
        sstarRes *ssrec;
        QList <objResRec*> rejList;



        mpccat mpoCat;
        if(saveMpc)
        {
            if(mpoCat.init(mpcOrbCat.toAscii().data())) qDebug() << QString("mpoCat init error!\n");

            mpcAggFile.setFileName(reportObjDir+"/mpc.txt");
            mpcAggFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
            mpcAggStm.setDevice(&mpcAggFile);

            ssAggFile.setFileName(reportObjDir+"/sstar.txt");
            ssAggFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
            ssAggStm.setDevice(&ssAggFile);


        }

            for(i=0; i<szi; i++)
            {
                objTemp.clear();
                objRejTemp.ocList.clear();
                //ssF.clear();
                //szj = objPStat.objList.at(i);
                qDebug() << QString("objName: %1\n").arg(objStat.objList.at(i)->objName);
                if(objSigma>0.0)
                {
                    objStat.objList.at(i)->do3Sigma(NULL, objSigma, &objRejTemp.ocList);

                }
                //objStat.objList.at(i)->getOCList(&rStat, &objTemp.ocList);
                objTemp.ocList << objStat.objList.at(i)->objList;

                qDebug() << QString("or size: %1\n").arg(objTemp.ocList.size());
                objName = objStat.objList.at(i)->objName.simplified();
                catName = objStat.objList.at(i)->catName.simplified();
                if(QString().compare(catName, "mpeph")==0)
                {
                    tfName = QString("%1/%2").arg(mpeDir).arg(objName);
                }
                else if(QString().compare(catName, "LSPM")==0)
                {
                    tfName = QString("%1/%2").arg(sstarDir).arg(objName);
                }
                else tfName = QString("%1/%2").arg(reportObjDir).arg(objName);
                szj = objTemp.ocList.size();
                objRejAgg.ocList << objRejTemp.ocList;
                qDebug() << QString("objTemp.ocList.size: %1\n").arg(szj);
                if(objRejTemp.ocList.size()>0) objRejTemp.saveAs(tfName+"_rej.txt");
                if(szj>0)
                {
                    objTemp.saveAs(tfName+"_or.txt");
                    objAggTemp.ocList << objTemp.ocList;
                    if(isSeries)
                    {
                        serTemp.serieList.clear();
                        orList.clear();
                        detSeriesList(objTemp.ocList, &serTemp.serieList, 9);
                        //bjTemp.findSeries(&eqList);

                        szj = serTemp.serieList.size();
                        qDebug() << QString("or series size: %1\n").arg(szj);
                        serTemp.saveAs_Full(QString("%1/orSeries.txt").arg(reportObjDir));
                        /*orTempS->clear();
                        for(j=0; j<szj; j++)
                        {
                            qDebug() << QString("serie %1: %2 obs\n").arg(j).arg(orList.at(j)->ocList.size());
                            if(d3s2) orList.at(j)->do3sigma(0.05, objSigma);
                            //eqList.at(j)->countCols("0,1,2,3,4,5,6");
                            //eqList.at(j)->countMM();
                             qDebug() << QString("do3sigma size: %1\n").arg(orList.at(j)->ocList.size());
                             orRec = new objResRec;
                            orList.at(j)->getSeriesRec(orRec);

                            orTempS->ocList << orRec;

                            //eqsR->rec2s(&tStr);
                           //dataStream1 << QString("%1").arg(tStr);
                        }
                        if(d3s3) orTempS->do3sigma(0.05, objSigma);
                        orTempS->countCols("4,5,6");
                        orTempS->saveAs(QString("%1/orSeries/%2_ors.txt").arg(reportObjDir).arg(objName));

                        orTotS->ocList << orTempS->ocList;
                        */
                    }

                    if(QString().compare(catName, "mpeph")==0)
                    {

                        eqTemp.clear();
                        szj = objTemp.ocList.size();
                        for(j=0; j<szj; j++)
                        {
                            ocrec = new ocRec;
                            objTemp.ocList.at(j)->toOcRec(ocrec);
                            eqTemp.ocList << ocrec;
                        }
                        eqTemp.sortOClist();

                        if(saveAgg) aggEqTemp.ocList << eqTemp.ocList;

                        if(objRejTemp.ocList.size()>0)
                        {
                            objRejTemp.saveAs(tfName+"_rej.txt");
                            eqTemp.clear();
                            for(j=0; j<szj; j++)
                            {
                                ocrec = new ocRec;
                                objTemp.ocList.at(j)->toOcRec(ocrec);
                                eqTemp.ocList << ocrec;
                            }
                            eqTemp.sortOClist();
                        }

                        if(isSeries)
                        {
                            serTemp.saveAs_MoveModel(QString("%1/orSeries_mm.txt").arg(reportObjDir));

                            eqList.clear();
                            eqTemp.findSeries(&eqList);

                            szj = eqList.size();
                            qDebug() << QString("eq series size: %1\n").arg(szj);
                            eqTempS->clear();
                            for(j=0; j<szj; j++)
                            {
                                qDebug() << QString("serie %1: %2 obs\n").arg(j).arg(eqList.at(j)->ocList.size());
                                if(d3s2) eqList.at(j)->do3sigma(0.05, objSigma);
                                //eqList.at(j)->countCols("0,1,2,3,4,5,6");
                                //eqList.at(j)->countMM();
                                 qDebug() << QString("do3sigma size: %1\n").arg(eqList.at(j)->ocList.size());
                                 ocR = new ocRec;
                                eqList.at(j)->getSeriesRec(ocR);

                                eqTempS->ocList << ocR;

                                //eqsR->rec2s(&tStr);
                               //dataStream1 << QString("%1").arg(tStr);
                            }
                            if(d3s3) eqTempS->do3sigma(0.05, objSigma);
                            eqTempS->countCols("4,5,6");
                            eqTempS->saveAs(QString("%1_eqs.txt").arg(tfName), 2);

                            eqTotS->ocList << eqTempS->ocList;
                        }

                        if(cCols)eqTemp.countCols("4,5,6");
                        if(saveEq)
                        {
                            eqTemp.saveAs(tfName+"_eq.txt");

                        }
                        if(saveMpc)
                        {
                            mpcFile.setFileName(tfName+"_mpc.txt");
                            mpcFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
                            mpcStm.setDevice(&mpcFile);

                            if(mpoCat.GetRecName(objName.toAscii().data()))
                            {
                                qDebug() << QString("objName is not found\n");
                                objNum = 0;
                            }
                            else
                            {
                                objNum = mpoCat.record->getNum();
                                qDebug() << QString("objNum: %1\n").arg(objNum);
                                //qDebug() << QString("number: %1\n").arg(mpoCat.record->number);
                            }

                            szj = eqTemp.ocList.size();
                            for(j=0; j<szj; j++)
                            {
                                eqTemp.ocList.at(j)->rec2MPC(&tstr, "084", objNum);
                                mpcStm << tstr << "\n";
                                if(saveAgg) mpcAggStm << tstr << "\n";
                            }

                            mpcFile.close();
                        }
                    }
                    if(QString().compare(catName, "LSPM")==0)
                    {

                        ssTemp.clear();
                        szj = objTemp.ocList.size();
                        for(j=0; j<szj; j++)
                        {
                            ssrec = new sstarRes;
                            objTemp.ocList.at(j)->toSstarRes(ssrec);
                            ssTemp.ocList << ssrec;
                        }


                        if(cCols)ssTemp.countCols("6,7,8");
                        if(saveEq)
                        {
                            ssTemp.saveAs(tfName+"_sstar.txt");
                            if(saveAgg) aggSsTemp.ocList << ssTemp.ocList;
                        }
                        if(isSeries)serTemp.saveAs_Mean(QString("%1/orSeries_mean.txt").arg(reportObjDir));
                    }
                }

            }

        if(saveAgg)
        {
            qDebug() << "saveAgg\n";
            objAggTemp.saveAs(reportObjDir+"/objRes.txt");
            objRejAgg.saveAs(reportObjDir+"/objRes_rej.txt");
            if(saveEq)
            {
                qDebug() << "saveEq\n";
                if(cCols)aggEqTemp.countCols("4,5,6");

                aggEqTemp.sortOClist();
                aggEqTemp.saveAs(reportObjDir+"/eq.txt");

                aggSsTemp.countCols("6,7,8");
                aggSsTemp.saveAs(reportObjDir+"/sstar.txt");
            }
            if(isSeries)
            {
                if(d3s4) orTotS->do3sigma(0.05, objSigma);

                //orTotS->countCols("5,6,7");
                //orTotS->saveAs(reportDirName+"/orSeries.txt");

                if(d3s4) eqTotS->do3sigma(0.05, objSigma);

                eqTotS->countCols("4,5,6");
                eqTotS->saveAs(reportObjDir+"/eqSeries.txt", 0);
            }
            /*
            if(objAggSigma>0.0)
            {
                qDebug() << "aggSsF.do3sigma\n";
                aggSsF.do3sigma(0.0, objSigma);
            }
            if(cCols)
            {
                qDebug() << "cCols\n";
                aggSsF.countCols("6,7,8");
            }

            aggSsF.saveAs(reportObjDir+"/agg_sstar.txt");*/
        }

        if(saveMpc) mpcAggFile.close();



    }



    if(isReport3)
    {
    //dataStream << "#" << diapStr << "\n";
    //dataStream << QString("#%1|%2\n").arg(t0).arg(t1);

    QString report3Dir = reportDirName + "report3";
    reportDir.mkdir("report3");





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

//       if(mgEqSigma>1e-2) magEq.resListDiap.at(k)->remSigma(mgEqSigma);
//       else magEq.resListDiap.at(k)->detStat();

       dataStrL.clear();

/*

       dataStrL << QString("%1").arg((magEq.diaps[k+1]+magEq.diaps[k])/2.0);
       dataStrL << QString("%1").arg(magEq.resListDiap.at(k)->meanKsi);
       dataStrL << QString("%1").arg(magEq.resListDiap.at(k)->rmsMeanKsi);
       dataStrL << QString("%1").arg(magEq.resListDiap.at(k)->meanEta);
       dataStrL << QString("%1").arg(magEq.resListDiap.at(k)->rmsMeanEta);
       dataStrL << QString("%1").arg(magEq.resListDiap.at(k)->resList.size());
  //     dataStrL << QString("%1")
       dataStream << dataStrL.join(r3data.colSep) << "\n";*/
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
           dmX[i] = magEq.resListDiap.at(k)->resList.at(i)->ksiOC;//*cos(grad2rad(magEq.resListDiap.at(k)->resList.at(i)->de));
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
       */
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
       }*/

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
       */

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

       dataStream2Xcoef << dataStrL.join(colSep) << "\n";

       dataStrL.clear();
       dataStrL << QString("%1").arg((magEq.diaps[k]+magEq.diaps[k+1])/2.0);
       for(i=0; i<r3data.curvDeg+1; i++)
       {
            dataStrL << QString("%1").arg(yCoeff[i], 10, 'e');
       }

       dataStream2Ycoef << dataStrL.join(colSep) << "\n";
/*
       for(i=0; i<szi; i++)
       {
           dataStream1 << QString("%1|%2|%3\n").arg(magEq.resListDiap.at(k)->resList.at(i)->mag, 5, 'f', 2).arg(magEq.resListDiap.at(k)->resList.at(i)->ksiOC*cos(grad2rad(magEq.resListDiap.at(k)->resList.at(i)->de)), 8, 'f', 2).arg(magEq.resListDiap.at(k)->resList.at(i)->etaOC, 8, 'f', 2);
           dataStream2X << QString("%1|%2\n").arg(magEq.resListDiap.at(k)->resList.at(i)->x).arg(magEq.resListDiap.at(k)->resList.at(i)->Dx, 6, 'f', 3);
           dataStream2Y << QString("%1|%2\n").arg(magEq.resListDiap.at(k)->resList.at(i)->y).arg(magEq.resListDiap.at(k)->resList.at(i)->Dy, 6, 'f', 3);
           dataStream3 << QString("%1|%2\n").arg(magEq.resListDiap.at(k)->resList.at(i)->mag-magEq.resListDiap.at(k)->resList.at(i)->magOC).arg(magEq.resListDiap.at(k)->resList.at(i)->magOC, 6, 'f', 3);
       }
*/
       for(i=0; i<dotNum; i++)
       {
           dataStream1 << QString("%1 %4 %2 %4 %3\n").arg(mX[i], 5, 'f', 2).arg(dmX[i], 8, 'f', 2).arg(dmY[i], 8, 'f', 2).arg(colSep);
           dataStream2X << QString("%1 %3 %2\n").arg(x[i]).arg(dx[i], 6, 'f', 3).arg(colSep);
           dataStream2Y << QString("%1 %3 %2\n").arg(y[i]).arg(dy[i], 6, 'f', 3).arg(colSep);
           dataStream3 << QString("%1 %3 %2\n").arg(mX[i]-dmX[i]).arg(dmX[i], 6, 'f', 3).arg(colSep);
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
           dataStream2X << QString("%1 %3 %2\n").arg(x[i]).arg(dx[i]-polyY(xCoeff, x[i], r3data.curvDeg), 6, 'f', 3).arg(colSep);
           dataStream2Y << QString("%1 %3 %2\n").arg(y[i]).arg(dy[i]-polyY(yCoeff, y[i], r3data.curvDeg), 6, 'f', 3).arg(colSep);
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
*/
    if(r3data.doMcoef)
    {

        mCoefFunc(r3NameXcoef, QString(report3Dir+"/mCoefX.txt"), colSep, r3data.mDegree);

        mCoefFunc(r3NameYcoef, QString(report3Dir+"/mCoefY.txt"), colSep, r3data.mDegree);

    }



    qDebug() << "end of report3\n";


    rFile.close();
    r1File.close();
    r3File.close();

    r2FileXcoef.close();
    r2FileYcoef.close();
}
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

        if(isReport1)
        {
            qDebug() << QString("task1\n");

            rFile.setFileName(reportDirName+"report1.txt");
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
        }

        if(isReport2)
        {
            qDebug() << QString("task2\n");

            rFile.setFileName(reportDirName+"report2.txt");
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

            msRec->getVersName(&versName);*/



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
                detVersName(&versName, msRec->originName);

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
                Zed = acos(P1);

                Az = atan2(P2/sin(Zed), P3/sin(Zed));
                //Zed = atan2(P2/sin(Az), P1);

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
            */


            rFile.close();
        }


    }
    else qDebug() << QString("init obs failed\n");

//report11

    if(isReport11)
    {
        rFile.setFileName(reportDirName+"report11.txt");
        rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
        dataStream.setDevice(&rFile);

        szi = mesList.size();


        for(i=0; i<szi; i++)
        {
            mesRec = mesList.at(i);

            xc = mesRec->errBud->Xoc;
            yc = mesRec->errBud->Yoc;
            szj = mesRec->resList.size();

            for(j=0; j< szj; j++)
            {
                dX = fabs(mesRec->resList.at(j)->x-xc);
                dY = fabs(mesRec->resList.at(j)->y-yc);

                dataStream << QString("%1|%2|%3|%4\n").arg(dX).arg(fabs(mesRec->resList.at(j)->Dx)).arg(dY).arg(fabs(mesRec->resList.at(j)->Dy));
            }
        }
        rFile.close();
    }

    if(isReport12)
    {
        report12(r12data, mesList, reportDirName);

    }

    if(isReport13)
    {
        report13(mesList, reportDirName);
    }

//report4
    vect = new double[3];

    if(isReport4)
    {

        report4Dir = reportDirName + "report4";
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
/*
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
*/
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
                                dKsi = 0.0;
                                dEta = 0.0;

                                if(r4data.isSysCorr)vectF->int2Drad(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, r4data.rMax, r4data.nmin);
                          //      if((fabs(resRec->ksiOC)<maxEta)&&(fabs(resRec->etaOC)<maxEta))
                          //      {
                                        if(diniKey)
                                        {
                                                naxeX[1] = naxeX[0] = resRec->ksi - dKsi;
                                                naxeY[1] = naxeY[0] = resRec->eta - dEta;
                                                diniKey = 0;
                                        }
                                        else
                                        {
                                                if(naxeX[0]>(resRec->ksi - dKsi)) naxeX[0] = resRec->ksi - dKsi;
                                                if(naxeX[1]<(resRec->ksi - dKsi)) naxeX[1] = resRec->ksi - dKsi;
                                                if(naxeY[0]>(resRec->eta - dEta)) naxeY[0] = resRec->eta - dEta;
                                                if(naxeY[1]<(resRec->eta - dEta)) naxeY[1] = resRec->eta - dEta;
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
        mLevels[mLevNum-1] = 20.0;*/
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





        for(i=0; i<di; i++)
        {
                printf("\rsetPoint: %f\%\t\t", (double)i*100.0/(double)di);

                mesRec = mesList.at(i);

                rNum = mesRec->resList.size();
                for(j=0; j<rNum; j++)
                {

                    resRec = mesList.at(i)->resList.at(j);

                    dKsi = 0.0;
                    dEta = 0.0;

                    if(r4data.isSysCorr)vectF->int2Drad(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, r4data.rMax, r4data.nmin);

                    vect[0] = resRec->ksi - dKsi;
                    vect[1] = resRec->eta - dEta;
                    vect[2] = resRec->mag;
                    vectFdata->addPoint(vect, resRec->ksiOC - dKsi, resRec->etaOC - dEta);
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

        qDebug() << "report5\n";

        report5Dir = reportDirName + "report5";
        reportDir.mkdir("report5");

        vectGrid3D *vectF5;// = new vectGrid3D;
       // QByteArray tmp;

        if(r5data.isSysCorr)
        {
                vectF5 = new vectGrid3D;
                tmp = r5data.sysCorrFile.toAscii();
                vectF5->init(tmp.data());
                vectF5->initVF();
                vectF5->printCoefs();
        }


        di = mesList.size();
        qDebug() << QString("mesList size = %1\n").arg(di);
/*
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

*/





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
                                            //qDebug() << QString("dini naxes: %1 - %2 | %3 - %4\n").arg(naxeX[0]).arg(naxeX[1]).arg(naxeY[0]).arg(naxeY[1]);
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
                        qDebug() << QString("naxes: %1 - %2 | %3 - %4\n").arg(naxeX[0]).arg(naxeX[1]).arg(naxeY[0]).arg(naxeY[1]);
                }
                /*if(fabs(naxeX[0])>fabs(naxeX[1])) axd = fabs(naxeX[1]);
                else axd = fabs(naxeX[0]);
                naxeX[0] = -axd;
                naxeX[1] = axd;

                if(fabs(naxeY[0])>fabs(naxeY[1])) axd = fabs(naxeY[1]);
                else axd = fabs(naxeY[0]);
                naxeY[0] = -axd;
                naxeY[1] = axd;*/
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
        mLevels[mLevNum-1] = 20.0;*/
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
        //long nint;

        qDebug() << QString("di= %1\n").arg(di);

        for(i=0; i<di; i++)
        {
                printf("\nsetPoint: %f\%\n", (double)i*100.0/(double)di);

                mesRec = mesList.at(i);

                rNum = mesRec->resList.size();
                for(j=0; j<rNum; j++)
                {

                    resRec = mesList.at(i)->resList.at(j);

                    dKsi = 0.0;
                    dEta = 0.0;

                    //if(r5data.isSysCorr) vectF5->int2D(resRec->x, resRec->y, resRec->mag, &dKsi, &dEta, &nint);
                    if(r5data.isSysCorr) vectF5->int2Drad(resRec->x, resRec->y, resRec->mag, &dKsi, &dEta, r5data.rMax, r5data.nmin);

                    vect[0] = resRec->x - dKsi;
                    vect[1] = resRec->y - dEta;
                    vect[2] = resRec->mag;
                    vectF5data->addPoint(vect, resRec->Dx - dKsi, resRec->Dy - dEta);
                }
        }
        qDebug() << QString("numTot= %1\n").arg(vectF5data->numTot);
        vectF5data->detMean();
        if(r5data.doWeght) vectF5data->doWeght();
        vectF5data->initVF();

        vectF5data->printCoefs();

        vectF5data->saveVF(report5Dir+"/res.vf");

        vectF5data->saveDotList(report5Dir, "|", "", 2000, 20);


        qDebug() << "\nreport5 end\n";
    }



/////////// report50

    QString report50Dir;

    if(isReport50)
    {

        qDebug() << "\nreport50\n";

        report50Dir = reportDirName + "report50";
        reportDir.mkdir("report50");

        vectGrid3D *vectF50;// = new vectGrid3D;
       // QByteArray tmp;

        QList <double*> mdCoefX, mdCoefY;
        QVector <double> dCoefX, dCoefY;
        int mdCoefXsz, mdCoefYsz, dCoefXsz, dCoefYsz;

        if(r50data.isSysCorr)
        {
            initMdCoef(r50data.mdCorrX, &mdCoefX, &mdCoefXsz, r50data.colSep);
            initMdCoef(r50data.mdCorrY, &mdCoefY, &mdCoefYsz, r50data.colSep);

            dCoefXsz = mdCoefX.size();
            dCoefYsz = mdCoefY.size();
        }


        di = mesList.size();
        qDebug() << QString("mesList size = %1\n").arg(di);
/*
        if(r50data.isSysCorr)
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
                        detCoefM(&dCoefX, mdCoefX, mdCoefXsz, resRec->mag);
                        detCoefM(&dCoefY, mdCoefY, mdCoefYsz, resRec->mag);

                        if(!detDcorr(r50data.corrModel, resRec->x, resRec->y, &dKsi, &dEta, dCoefX, dCoefY))
                        {
                            qDebug() << QString("x= %1\tdKsi= %2\ty= %3\tdEta= %4\n").arg(resRec->x).arg(dKsi).arg(resRec->y).arg(dEta);
                            resRec->x-= dKsi;
                            resRec->y-= dEta;

                            resRec->Dx-= dKsi;
                            resRec->Dy-= dEta;
                        }


                    }
                }
        }

*/





        qDebug() << QString("isFindDiap = %1\n").arg(r50data.isFindDiap);

        if(r50data.isFindDiap)
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

                                dKsi = 0.0;
                                dEta = 0.0;
                                if(r50data.isSysCorr)
                                {
                                    detCoefM(&dCoefX, mdCoefX, mdCoefXsz, resRec->mag);
                                    detCoefM(&dCoefY, mdCoefY, mdCoefYsz, resRec->mag);



                                    detDcorr(r50data.corrModel, resRec->x, resRec->y, &dKsi, &dEta, dCoefX, dCoefY);
                                }



                               // if(mesRec->errBud->exptime<expMin) continue;

                          //      if((fabs(resRec->ksiOC)<maxEta)&&(fabs(resRec->etaOC)<maxEta))
                          //      {
                                        if(diniKey)
                                        {
                                            naxeX[1] = naxeX[0] = resRec->x - dKsi;
                                            naxeY[1] = naxeY[0] = resRec->y - dEta;
                                            diniKey = 0;
                                        }
                                        else
                                        {
                                            if(naxeX[0]>(resRec->x - dKsi)) naxeX[0] = resRec->x - dKsi;
                                            if(naxeX[1]<(resRec->x - dKsi)) naxeX[1] = resRec->x - dKsi;
                                            if(naxeY[0]>(resRec->y - dEta)) naxeY[0] = resRec->y - dEta;
                                            if(naxeY[1]<(resRec->y - dEta)) naxeY[1] = resRec->y - dEta;
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
                naxeX[0] = r50data.x0;
                naxeX[1] = r50data.x1;
                naxeY[0] = r50data.y0;
                naxeY[1] = r50data.y1;
        }

        qDebug() << "naxeX= " << naxeX[0] << " - " << naxeX[1] << "\n";
        qDebug() << "naxeY= " << naxeY[0] << " - " << naxeY[1] << "\n";

        diapNum = magEq.resListDiap.size();

        qDebug() << QString("\nmLevNum= %1\nxLevNum= %2\nyLevNum= %3\n").arg(mLevNum).arg(r50data.xLevNum).arg(r50data.yLevNum);

        mLevels = new double[mLevNum];
        xLevels = new double[r50data.xLevNum];
        yLevels = new double[r50data.yLevNum];

        dX = fabs(naxeX[1] - naxeX[0])/(double)(r50data.xLevNum-1);
        qDebug() << "dX= " << dX << "\n";
        qDebug() << "xLevels: ";
        for(i=0; i<r50data.xLevNum; i++)
        {
            xLevels[i] = naxeX[0] + dX*i;
            qDebug() << xLevels[i] << " ";
        }
        qDebug() << "\n";

        dY = fabs(naxeY[1] - naxeY[0])/(double)(r50data.yLevNum-1);
        qDebug() << "dY= " << dX << "\n";
        qDebug() << "yLevels: ";
        for(i=0; i<r50data.yLevNum; i++)
        {
            yLevels[i] = naxeY[0] + dY*i;
            qDebug() << yLevels[i] << " ";
        }
        qDebug() << "\n";

        /*mLevels[0] = 0.0;
        for(i=1; i<mLevNum-1; i++) mLevels[i] = (mLevNumBeg+i-1)*1.0;
        mLevels[mLevNum-1] = 20.0;*/
        qDebug() << "mLevels: ";
        for(i=0; i<mLevNum; i++)
        {
            mLevels[i] = mdList.at(i).toDouble();
            qDebug() << mLevels[i] << " ";
        }
        qDebug() << "\n";

        qDebug() << "\nvectFdata\n";

        vectGrid3D *vectF50data = new vectGrid3D();

        vectF50data->setLevels(xLevels, r50data.xLevNum, yLevels, r50data.yLevNum, mLevels, mLevNum);



        //double *vect = new double[3];

        for(i=0; i<di; i++)
        {
                printf("\nsetPoint: %f\%\n", (double)i*100.0/(double)di);

                mesRec = mesList.at(i);

                rNum = mesRec->resList.size();
                for(j=0; j<rNum; j++)
                {

                    resRec = mesList.at(i)->resList.at(j);

                    dKsi = 0.0;
                    dEta = 0.0;
                    if(r50data.isSysCorr)
                    {
                        detCoefM(&dCoefX, mdCoefX, mdCoefXsz, resRec->mag);
                        detCoefM(&dCoefY, mdCoefY, mdCoefYsz, resRec->mag);

                        detDcorr(r50data.corrModel, resRec->x, resRec->y, &dKsi, &dEta, dCoefX, dCoefY);
                    }

                    vect[0] = resRec->x - dKsi;
                    vect[1] = resRec->y - dEta;
                    vect[2] = resRec->mag;
                    vectF50data->addPoint(vect, resRec->Dx - dKsi, resRec->Dy - dEta);
                }
        }
        qDebug() << QString("numTot= %1\n").arg(vectF50data->numTot);
        vectF50data->detMean();
        vectF50data->initVF();

        vectF50data->printCoefs();

        vectF50data->saveVF(report50Dir+"/res.vf");

        vectF50data->saveDotList(report50Dir, "|");


    }

                    //report5
                    if(isReport51)
                    {

                        report51Dir = reportDirName + "report51";
                        reportDir.mkdir("report51");

                        vectGrid3D *vectF51;// = new vectGrid3D;
                       // QByteArray tmp;

                        if(r51data.isSysCorr)
                        {
                                vectF51 = new vectGrid3D;
                                tmp = r51data.sysCorrFile.toAscii();
                                vectF51->init(tmp.data());
                                vectF51->initVF();
                                vectF51->printCoefs();
                        }


                        di = mesList.size();
                        qDebug() << QString("mesList size = %1\n").arg(di);
                /*
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

                */





                        qDebug() << QString("isFindDiap = %1\n").arg(r51data.isFindDiap);

                        if(r51data.isFindDiap)
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
                                                            qDebug() << QString("dini naxes: %1 - %2 | %3 - %4\n").arg(naxeX[0]).arg(naxeX[1]).arg(naxeY[0]).arg(naxeY[1]);
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
                                        qDebug() << QString("naxes: %1 - %2 | %3 - %4\n").arg(naxeX[0]).arg(naxeX[1]).arg(naxeY[0]).arg(naxeY[1]);
                                }
                                /*if(fabs(naxeX[0])>fabs(naxeX[1])) axd = fabs(naxeX[1]);
                                else axd = fabs(naxeX[0]);
                                naxeX[0] = -axd;
                                naxeX[1] = axd;

                                if(fabs(naxeY[0])>fabs(naxeY[1])) axd = fabs(naxeY[1]);
                                else axd = fabs(naxeY[0]);
                                naxeY[0] = -axd;
                                naxeY[1] = axd;*/
                        }
                        else
                        {
                                naxeX[0] = r51data.x0;
                                naxeX[1] = r51data.x1;
                                naxeY[0] = r51data.y0;
                                naxeY[1] = r51data.y1;
                        }

                        qDebug() << "naxeX= " << naxeX[0] << " - " << naxeX[1] << "\n";
                        qDebug() << "naxeY= " << naxeY[0] << " - " << naxeY[1] << "\n";

                        diapNum = magEq.resListDiap.size();

                        qDebug() << QString("\nmLevNum= %1\nxLevNum= %2\nyLevNum= %3\n").arg(mLevNum).arg(r51data.xLevNum).arg(r51data.yLevNum);

                        mLevels = new double[mLevNum];
                        xLevels = new double[r51data.xLevNum];
                        yLevels = new double[r51data.yLevNum];

                        dX = fabs(naxeX[1] - naxeX[0])/(double)(r5data.xLevNum-1);
                        qDebug() << "dX= " << dX << "\n";
                        qDebug() << "xLevels: ";
                        for(i=0; i<r51data.xLevNum; i++)
                        {
                            xLevels[i] = naxeX[0] + dX*i;
                            qDebug() << xLevels[i] << " ";
                        }
                        qDebug() << "\n";

                        dY = fabs(naxeY[1] - naxeY[0])/(double)(r51data.yLevNum-1);
                        qDebug() << "dY= " << dX << "\n";
                        qDebug() << "yLevels: ";
                        for(i=0; i<r51data.yLevNum; i++)
                        {
                            yLevels[i] = naxeY[0] + dY*i;
                            qDebug() << yLevels[i] << " ";
                        }
                        qDebug() << "\n";

                        /*mLevels[0] = 0.0;
                        for(i=1; i<mLevNum-1; i++) mLevels[i] = (mLevNumBeg+i-1)*1.0;
                        mLevels[mLevNum-1] = 20.0;*/
                        qDebug() << "mLevels: ";
                        for(i=0; i<mLevNum; i++)
                        {
                            mLevels[i] = mdList.at(i).toDouble();
                            qDebug() << mLevels[i] << " ";
                        }
                        qDebug() << "\n";

                        qDebug() << "\nvectFdata\n";

                        vectGrid3D *vectF51data = new vectGrid3D();

                        vectF51data->setLevels(xLevels, r51data.xLevNum, yLevels, r51data.yLevNum, mLevels, mLevNum);



                        //double *vect = new double[3];
                        long nint;

                        qDebug() << QString("di= %1\n").arg(di);

                        for(i=0; i<di; i++)
                        {
                                printf("\nsetPoint: %f\%\n", (double)i*100.0/(double)di);

                                mesRec = mesList.at(i);

                                rNum = mesRec->resList.size();
                                for(j=0; j<rNum; j++)
                                {

                                    resRec = mesList.at(i)->resList.at(j);

                                    dKsi = 0.0;
                                    dEta = 0.0;

                                    //if(r5data.isSysCorr) vectF51->int2D(resRec->x, resRec->y, resRec->mag, &dKsi, &dEta, &nint);
                                    if(r51data.isSysCorr) vectF51->int2DradM(resRec->x, resRec->y, resRec->mag, &dKsi, &dEta, r5data.rMax, r5data.nmin);

                                    vect[0] = resRec->x - dKsi;
                                    vect[1] = resRec->y - dEta;
                                    vect[2] = resRec->mag;
                                    vectF51data->addPoint(vect, resRec->Dx - dKsi, resRec->Dy - dEta);
                                }
                        }
                        qDebug() << QString("numTot= %1\n").arg(vectF51data->numTot);
                        vectF51data->detMean();
                        if(r51data.doWeght) vectF51data->doWeght();
                        vectF51data->initVF();

                        vectF51data->printCoefs();

                        vectF51data->saveVF(report51Dir+"/res.vf");

                        vectF51data->saveDotList(report51Dir, "|");


                    }

/////////////////////////////////////////////////////////////////////////////////

//report6
/*
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
/*
        for(i=0; i<di; i++)
        {
            mesRec = mesList.at(i);

            fitsd->clear();

            fitsd->openEmptyFile();

            fitsd->setPos(mesRec->errBud->MJD, mesRec->errBud->RAoc, mesRec->errBud->DEoc);

            //РІС‹СЃС‚Р°РІРёС‚СЊ РїР°СЂР°РјРµС‚СЂС‹ РёРЅСЃС‚СЂСѓРјРµРЅС‚Р°
                fitsd->marksG->setInstrProp(instruments->scx, instruments->scy, instruments->rang);
                fitsd->marksGIpix->setInstrProp(instruments->scx, instruments->scy, instruments->rang);
            //РѕС‚РєСЂС‹С‚СЊ РјРµС‚РєРё
                //fitsd->marksGIpix->loadTanImg(fileName, mSep, mColumn);
                //if(isMove2corner) fitsd->marksGIpix->moveToCorner();
            //РёР· РєР°С‚Р°Р»РѕРіР°
                fitsd->marksG->clearMarks();
                fitsd->getMarksGrid(starCatList.at(currentCat), instruments->fov, mag0, mag1, -1);
                fitsd->detTan();
            //РѕР±СЉРµРєС‚
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
/*
               for(j=0; j<szj; j++)
               {
                   //qDebug() << QString("ra: %1|%2|%3\n").arg(mesRec->u3MarksList[j]->u3Rec->ra).arg(mesRec->u3MarksList[j]->resRec->ra).arg(grad_to_mas(fabs(mesRec->u3MarksList[j]->u3Rec->ra - mesRec->u3MarksList[j]->resRec->ra)));
                   //qDebug() << QString("de: %1|%2|%3\n").arg(mesRec->u3MarksList[j]->u3Rec->dec).arg(mesRec->u3MarksList[j]->resRec->de).arg(grad_to_mas(fabs(mesRec->u3MarksList[j]->u3Rec->dec - mesRec->u3MarksList[j]->resRec->de)));
                   //qDebug() << QString("dist: %1\n\n").arg(sqrt(pow(mesRec->u3MarksList[j]->u3Rec->ra - mesRec->u3MarksList[j]->resRec->ra, 2.0) + pow(mesRec->u3MarksList[j]->u3Rec->dec - mesRec->u3MarksList[j]->resRec->de, 2.0)));
               }
/
        }
/




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

        report6Dir = reportDirName + "report6";
        reportDir.mkdir("report6");

        QDir tDir;
        tDir.setPath(report6Dir);
        QStringList filesList, filtersL;
        filtersL << "*";
        filesList = tDir.entryList(filtersL);
        for(i=0; i<filesList.size(); i++) tDir.remove(filesList.at(i));

        double sgra, sgde, dtRa, dtDe, t0;
/*
        if(isReport61)
        {
            rFile.setFileName(reportDirName+"report61.txt");
            rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
            //dataStream.setDevice(&rFile);
            rFile.close();
        }

        di = mesList.size();
        //qDebug() << QString("mesList size= %1\n").arg(di);
        //fitsd->openEmptyFile();
            for(i=0; i<di; i++)
            {
                    //printf("\nfindDiap: %f\%\n", (double)i*100.0/(double)di);



                mesRec = mesList.at(i);





                //mesRec = mesList.at(i);

                //fitsd->clear();



                fitsd->setPos(mesRec->errBud->MJD, mesRec->errBud->RAoc, mesRec->errBud->DEoc);

                //РІС‹СЃС‚Р°РІРёС‚СЊ РїР°СЂР°РјРµС‚СЂС‹ РёРЅСЃС‚СЂСѓРјРµРЅС‚Р°
                    fitsd->marksG->setInstrProp(instruments->scx, instruments->scy, instruments->rang);
                    fitsd->marksGIpix->setInstrProp(instruments->scx, instruments->scy, instruments->rang);
                //РѕС‚РєСЂС‹С‚СЊ РјРµС‚РєРё
                    //fitsd->marksGIpix->loadTanImg(fileName, mSep, mColumn);
                    //if(isMove2corner) fitsd->marksGIpix->moveToCorner();
                //РёР· РєР°С‚Р°Р»РѕРіР°
                    fitsd->marksG->clearMarks();
                    fitsd->getMarksGrid(starCatList.at(currentCat), instruments->fov, mag0, mag1, -1);
                    fitsd->detTan();
                //РѕР±СЉРµРєС‚
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



                t0 = mesRec->errBud->MJD;


                rNum = mesRec->u3MarksList.size();
                //qDebug() << QString("rNum= %1\n").arg(rNum);
                for(j=0; j<rNum; j++)
                {

                     tMark = mesRec->u3MarksList.at(j);

                     if(isReport60)
                     {
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

                     if(isReport61)
                     {

                         dtRa = fabs(tMark->u3Rec->cepra - getYearFromMJD(mesRec->errBud->MJD));
                         dtDe = fabs(tMark->u3Rec->cepdc - getYearFromMJD(mesRec->errBud->MJD));

                         qDebug() << QString("ceph cat: %1\t%2\n").arg(tMark->u3Rec->cepra).arg(tMark->u3Rec->cepdc);
                         //qDebug() << QString("ceph: %1\t%2\n").arg(cephra).arg(cephde);
                         qDebug() << QString("dt: %1\t%2\n").arg(dtRa).arg(dtDe);
                         qDebug() << QString("sigra: %1\tsigdc: %2\tsigpmr: %3\tsigpmd: %4\n").arg(tMark->u3Rec->sigra).arg(tMark->u3Rec->sigdc).arg(tMark->u3Rec->sigpmr).arg(tMark->u3Rec->sigpmd);

                         sgra = sqrt(pow(tMark->u3Rec->sigra, 2.0) + pow(tMark->u3Rec->sigpmr*dtRa, 2.0));
                         sgde = sqrt(pow(tMark->u3Rec->sigdc, 2.0) + pow(tMark->u3Rec->sigpmd*dtDe, 2.0));
                         qDebug() << QString("dt: %1\t%2\n").arg(sgra).arg(sgde);

                         rFile.setFileName(reportDirName+"report61.txt");
                         rFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Append);
                         dataStream.setDevice(&rFile);

                         //dataStream << QString("%1|%2|%3|0\n").arg(mesRec->resList.at(j)->mag-mesRec->resList.at(j)->magOC).arg(fabs(tMark->resRec->ksiOC)).arg(fabs(tMark->resRec->etaOC));
                         //dataStream << QString("%1|%2|%3|1\n").arg(mesRec->resList.at(j)->mag-mesRec->resList.at(j)->magOC).arg(sgra).arg(sgde);

                         dataStream << QString("%1|%2|%3|0\n").arg(mesRec->resList.at(j)->mag-mesRec->resList.at(j)->magOC).arg(fabs(tMark->resRec->ksiOC-sgra)).arg(fabs(tMark->resRec->etaOC-sgde));

                         rFile.close();

                     }


                }

                mesRec->u3MarksList.clear();
            }

            qDebug() << QString("u3Summ: %1\tresSumm: %2\tident q: %3\n").arg(u3Summ).arg(resSumm).arg(u3Summ/(1.0*resSumm));

            if(isReport61)
            {
                rFile.close();
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

    r3NameX = QString("%1resListOC.txt").arg(reportDirName);
    r3NameY = QString("%1marksListOC.txt").arg(reportDirName);

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
    qDebug() << "\nend\n";


    delete clog1;
    clog1 = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);


    return 0;//a.exec();
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
*/


void diapResiduals::initMesList(QList <measurementRec*> mesList)
{
    int szRes, i, j, k, szi;
    measurementRec* mesRec;

    //qDebug() << QString("szRes: %1\n").arg(szRes);
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


void report12(report12data r12data, QList <measurementRec*> &mesList, QString reportDirName)
{
    int i, j, szi, szj;
    QFile rFileX, rFileY;
    QTextStream dataStreamX, dataStreamY;
    measurementRec *mesRec;
    residualsRec *resRec;

    rFileX.setFileName(reportDirName+"ocRefKsi.txt");
    rFileX.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStreamX.setDevice(&rFileX);

    rFileY.setFileName(reportDirName+"ocRefEta.txt");
    rFileY.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStreamY.setDevice(&rFileY);
/*
    rFileM.setFileName(reportDirName+"ocRefMag.txt");
    rFileM.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStreamM.setDevice(&rFileM);
*/
    szi = mesList.size();

    QVector <double> tVectX;
    QVector <double> fVectX;

    QVector <double> tVectY;
    QVector <double> fVectY;

    double *fData, *tData, *rData;
    int *nData;
    int tlen, flen;
    int len1, p, k, k0, k1, pos;
    double s0, s1, dx, corrVal;
    int polyDeg = 3;
    double *polyCoefX, *polyCoefY;
    QStringList tStrL;


    QList <harmParam*> resList;


    s0 = 0;
    s1 = 0;
    k0=0;
    k1=0;

    for(i=0; i<szi; i++)
    {
        mesRec = mesList.at(i);


        szj = mesRec->resList.size();

        for(j=0; j< szj; j++)
        {
            resRec = mesRec->resList.at(j);



            /*if(resRec->isRefKsi) dataStreamX << QString("%1|%2\n").arg(resRec->mag-resRec->magOC).arg(resRec->ksiOC);
            if(resRec->isRefEta) dataStreamY << QString("%1|%2\n").arg(resRec->mag-resRec->magOC).arg(resRec->etaOC);
            if(resRec->isRefMag) dataStreamM << QString("%1|%2\n").arg(resRec->mag-resRec->magOC).arg(resRec->magOC);*/
            if(resRec->isRefKsi)
            {
                dataStreamX << QString("%1\t%2\n").arg(resRec->x).arg(resRec->Dx);
                s0 +=resRec->Dx*resRec->Dx;
                k0++;
            }
            if(resRec->isRefEta)
            {
                dataStreamY << QString("%1\t%2\n").arg(resRec->y).arg(resRec->Dy);
                s1 +=resRec->Dy*resRec->Dy;
                k1++;
            }
            //if(resRec->isRefMag) dataStreamM << QString("%1\t%2\n").arg(resRec->pixmag).arg(resRec->Dpixmag);

            if((resRec->x>=r12data.x0)&&(resRec->x<=r12data.x1))
            {
                tVectX << resRec->x;
                fVectX << resRec->Dx;
            }

            if((resRec->y>=r12data.y0)&&(resRec->y<=r12data.y1))
            {
                tVectY << resRec->y;
                fVectY << resRec->Dy;
            }

        }
    }
    rFileX.close();
    rFileY.close();

    s0 = sqrt(s0)/k0;
    s1 = sqrt(s1)/k1;

    qDebug() << QString("prev: s0= %1\t s1= %2\n").arg(s0).arg(s1);



    tlen = tVectX.size();
    flen = fVectX.size();
    if(tlen==flen)
    {
        tlen = r12data.redDeg;
        qDebug() << QString("tlen: %1\n").arg(tlen);
        fData = new double[tlen];
        tData = new double[tlen];
        //rData = new double[tlen];
        nData = new int[tlen];
        dx = (r12data.x1-r12data.x0)/tlen;
        qDebug() << QString("dx: %1\n").arg(dx);

        for(i=0; i<tlen; i++)
        {
            tData[i] = 0.0;
            fData[i] = 0.0;
            nData[i] = 0;
        }

        for(i=0; i<flen; i++)
        {
            pos = (tVectX.at(i)-r12data.x0)/dx;
            if(pos<0||pos>=tlen)qDebug() << QString("pos: %1").arg(pos);
            tData[pos] += tVectX.at(i);
            fData[pos] += fVectX.at(i);
            nData[pos]++;
        }

        tVectX.clear();
        fVectX.clear();

        for(i=0; i<tlen; i++)
        {

            if(nData[i]>=20)
            {//qDebug() << QString("nData: %1\n").arg(nData[i]);
            tVectX << (tData[i] / nData[i]);
            fVectX << (fData[i] / nData[i]);
            }
            //qDebug() << QString("vect[%1]: %2\t%3\n").arg(i).arg(tData[i]).arg(fData[i]);
        }

            tlen = tVectX.size();

            delete [] fData;
            delete [] tData;

            fData = new double[tlen];
            tData = new double[tlen];

            for(i=0; i<tlen; i++)
            {
                tData[i] = tVectX[i];
                fData[i] = fVectX[i];
                //qDebug() << QString("vect[%1]: %2\t%3\n").arg(i).arg(tData[i]).arg(fData[i]);
            }
            polyCoefX = new double[polyDeg];
            makePoly(tData, fData, tlen, polyCoefX, polyDeg);
            tStrL.clear();
            for(i=0; i<polyDeg; i++) tStrL << QString("%1*x^%2").arg(polyCoefX[i]).arg(i);
            qDebug() << QString("polyCoefX: %1\n").arg(tStrL.join(" + "));

            s0 = 0;
            //s1 = 0;
            k0=0;

            for(i=0; i<tlen; i++)
            {
                fData[i] = fData[i] - detPoly(tData[i], polyCoefX, polyDeg);
                s0 += fData[i]*fData[i];
                k0++;
            }

            s0 = sqrt(s0)/k0;
            qDebug() << QString("resX poly: s0= %1\n").arg(s0);


        /*
        if(r12data.redDeg>1)
        {
            sortTvect(tData, fData, tlen);
            div_t divresult;
            divresult = div(tlen, r12data.redDeg);
            len1 = divresult.quot;
            if(divresult.rem>0) len1++;

            p=0;
            k=0;
            s1 = s0 = 0;

            for(i=0; i<tlen; i++)
            {
                s0 += tData[i];
                s1 += fData[i];
                p++;
                if((p==r12data.redDeg)||(i==(tlen-1)))
                {
                    tData[k] = s0/(1.0*p);
                    fData[k] = s1/(1.0*p);
                    k++;
                    p=0;
                    s0=0;
                    s1=0;
                }

            }
            tlen = k-1;

        }
*/
        makeClean(tData, fData, tlen, r12data.cp, resList, 1);
        qDebug() << QString("resX harm sz: %1\n").arg(resList.size());
        for(i=0; i<resList.size(); i++) qDebug() << QString("resList[i]= %1*cos(2PIt/ %2 + %3)\n").arg(resList[i]->A).arg(resList[i]->P).arg(resList[i]->Fi);

        //detHarm(tData[i], resList);

        rFileX.setFileName(reportDirName+"ocRefKsi_corr.txt");
        rFileX.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
        dataStreamX.setDevice(&rFileX);
/*
        rFileY.setFileName(reportDirName+"ocRefEta_corr.txt");
        rFileY.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
        dataStreamY.setDevice(&rFileY);
  */
        s0 = 0;
        //s1 = 0;
        k0=0;
        for(i=0; i<szi; i++)
        {
            mesRec = mesList.at(i);


            szj = mesRec->resList.size();

            for(j=0; j< szj; j++)
            {
                resRec = mesRec->resList.at(j);
                corrVal = resRec->Dx-detHarm(resRec->x, resList);

                if(resRec->isRefKsi)
                {
                    s0 +=corrVal*corrVal;
                    //s1 +=resRec->Dy*resRec->Dy;
                    k0++;
                    dataStreamX << QString("%1\t%2\n").arg(resRec->x).arg(corrVal);
                }
                //dataStreamY << QString("%1\t%2\n").arg(resRec->y).arg(resRec->Dy);
            }
        }

        rFileX.close();

        delete [] fData;
        delete [] tData;
        //delete [] rData;
        delete [] nData;
    }

    s0 = sqrt(s0)/k0;
    //s1 = sqrt(s1)/k;

    qDebug() << QString("resX: s0= %1\n").arg(s0);

    resList.clear();

    tlen = tVectY.size();
    flen = fVectY.size();
    if(tlen==flen)
    {

        tlen = r12data.redDeg;
        fData = new double[tlen];
        tData = new double[tlen];
        rData = new double[tlen];
        nData = new int[tlen];
        dx = (r12data.y1-r12data.y0)/tlen;

        for(i=0; i<tlen; i++)
        {
            tData[i] = 0.0;
            fData[i] = 0.0;
            nData[i] = 0;
        }

        for(i=0; i<flen; i++)
        {
            pos = (tVectY.at(i)-r12data.y0)/dx;
            tData[pos] += tVectY.at(i);
            fData[pos] += fVectY.at(i);
            nData[pos]++;
        }

        tVectY.clear();
        fVectY.clear();

        for(i=0; i<tlen; i++)
        {
            //qDebug() << QString("nData: %1\n").arg(nData[i]);
            if(nData[i]>=20)
            {
            tVectY << (tData[i] / nData[i]);
            fVectY << (fData[i] / nData[i]);
            }
            //qDebug() << QString("vect[%1]: %2\t%3\n").arg(i).arg(tData[i]).arg(fData[i]);
        }

            tlen = tVectY.size();

            delete [] fData;
            delete [] tData;

            fData = new double[tlen];
            tData = new double[tlen];

            for(i=0; i<tlen; i++)
            {
                tData[i] = tVectY[i];
                fData[i] = fVectY[i];
            }
            polyCoefY = new double[polyDeg];
            makePoly(tData, fData, tlen, polyCoefY, polyDeg);
            tStrL.clear();
            for(i=0; i<polyDeg; i++) tStrL << QString("%1*x^%2").arg(polyCoefY[i]).arg(i);
            qDebug() << QString("polyCoefY: %1\n").arg(tStrL.join(" + "));

            s0 = 0;
            //s1 = 0;
            k0=0;

            for(i=0; i<tlen; i++)
            {
                fData[i] = fData[i] - detPoly(tData[i], polyCoefY, polyDeg);
                s0 += fData[i]*fData[i];
                k0++;
            }

            s0 = sqrt(s0)/k0;
            qDebug() << QString("resY poly: s0= %1\n").arg(s0);

/*
        if(r12data.redDeg>1)
        {
            sortTvect(tData, fData, tlen);
            div_t divresult;
            divresult = div(tlen, r12data.redDeg);
            len1 = divresult.quot;
            if(divresult.rem>0) len1++;

            p=0;
            k=0;
            s1 = s0 = 0;

            for(i=0; i<tlen; i++)
            {
                s0 += tData[i];
                s1 += fData[i];
                p++;
                if((p==r12data.redDeg)||(i==(tlen-1)))
                {
                    tData[k] = s0/(1.0*p);
                    fData[k] = s1/(1.0*p);
                    k++;
                    p=0;
                    s0=0;
                    s1=0;
                }

            }
            tlen = k-1;
        }
*/
        makeClean(tData, fData, tlen, r12data.cp, resList, 1);

        detHarm(tData[i], resList);

        rFileX.setFileName(reportDirName+"ocRefEta_corr.txt");
        rFileX.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
        dataStreamX.setDevice(&rFileX);
/*
        rFileY.setFileName(reportDirName+"ocRefEta_corr.txt");
        rFileY.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
        dataStreamY.setDevice(&rFileY);
  */
        s1 = 0;
        k1=0;
        for(i=0; i<szi; i++)
        {
            mesRec = mesList.at(i);


            szj = mesRec->resList.size();

            for(j=0; j< szj; j++)
            {
                resRec = mesRec->resList.at(j);
                corrVal = resRec->Dy-detHarm(resRec->y, resList);

                if(resRec->isRefEta)
                {
                    dataStreamX << QString("%1\t%2\n").arg(resRec->y).arg(corrVal);
                    s1 +=corrVal*corrVal;
                    k1++;
                }
                //dataStreamY << QString("%1\t%2\n").arg(resRec->y).arg(resRec->Dy);
            }
        }

        rFileX.close();
    }

    s1 = sqrt(s1)/k1;

    qDebug() << QString("resY: s0= %1\n").arg(s1);


//      rFileM.close();
}

void report13(QList<measurementRec*> &mesList, QString reportDirName)
{
    int i, j, szi, szj;
    QFile rFileX, rFileY, rFileM;
    QTextStream dataStreamX, dataStreamY, dataStreamM;
    measurementRec *mesRec;
    residualsRec *resRec;

    rFileX.setFileName(reportDirName+"ocMagRefKsi.txt");
    rFileX.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStreamX.setDevice(&rFileX);

    rFileY.setFileName(reportDirName+"ocMagRefEta.txt");
    rFileY.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStreamY.setDevice(&rFileY);

    rFileM.setFileName(reportDirName+"ocMagRefMag.txt");
    rFileM.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    dataStreamM.setDevice(&rFileM);

    szi = mesList.size();


    for(i=0; i<szi; i++)
    {
        mesRec = mesList.at(i);


        szj = mesRec->resList.size();

        for(j=0; j< szj; j++)
        {
            resRec = mesRec->resList.at(j);
            /*if(resRec->isRefKsi) dataStreamX << QString("%1|%2\n").arg(resRec->mag-resRec->magOC).arg(resRec->ksiOC);
            if(resRec->isRefEta) dataStreamY << QString("%1|%2\n").arg(resRec->mag-resRec->magOC).arg(resRec->etaOC);
            if(resRec->isRefMag) dataStreamM << QString("%1|%2\n").arg(resRec->mag-resRec->magOC).arg(resRec->magOC);*/
            if(resRec->isRefKsi) dataStreamX << QString("%1\t%2\n").arg(resRec->x).arg(resRec->Dx);
            if(resRec->isRefEta) dataStreamY << QString("%1\t%2\n").arg(resRec->y).arg(resRec->Dy);
            if(resRec->isRefMag) dataStreamM << QString("%1\t%2\n").arg(resRec->pixmag).arg(resRec->Dpixmag);

        }
    }
    rFileX.close();
    rFileY.close();
    rFileM.close();
}
/*
void report4(report4data r4data, QDir reportDir)
{
    QString report4Dir;
    vectGrid3D *vectF;
    int rNum, di, diniKey, i, j;
    double dKsi, dEta, axd;
    double naxeX[2];
    double naxeY[2];
    double *mLevels;
    double *xLevels;
    double *yLevels;

    report4Dir = reportDir.absolutePath() + QDir().separator() + "report4";
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
/*
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
/
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
                            dKsi = 0.0;
                            dEta = 0.0;

                            if(r4data.isSysCorr)vectF->int2Drad(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, r4data.rMax, r4data.nmin);
                      //      if((fabs(resRec->ksiOC)<maxEta)&&(fabs(resRec->etaOC)<maxEta))
                      //      {
                                    if(diniKey)
                                    {
                                            naxeX[1] = naxeX[0] = resRec->ksi - dKsi;
                                            naxeY[1] = naxeY[0] = resRec->eta - dEta;
                                            diniKey = 0;
                                    }
                                    else
                                    {
                                            if(naxeX[0]>(resRec->ksi - dKsi)) naxeX[0] = resRec->ksi - dKsi;
                                            if(naxeX[1]<(resRec->ksi - dKsi)) naxeX[1] = resRec->ksi - dKsi;
                                            if(naxeY[0]>(resRec->eta - dEta)) naxeY[0] = resRec->eta - dEta;
                                            if(naxeY[1]<(resRec->eta - dEta)) naxeY[1] = resRec->eta - dEta;
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





    for(i=0; i<di; i++)
    {
            printf("\nsetPoint: %f\%\n", (double)i*100.0/(double)di);

            mesRec = mesList.at(i);

            rNum = mesRec->resList.size();
            for(j=0; j<rNum; j++)
            {

                resRec = mesList.at(i)->resList.at(j);

                dKsi = 0.0;
                dEta = 0.0;

                if(r4data.isSysCorr)vectF->int2Drad(resRec->ksi, resRec->eta, resRec->mag, &dKsi, &dEta, r4data.rMax, r4data.nmin);

                vect[0] = resRec->ksi - dKsi;
                vect[1] = resRec->eta - dEta;
                vect[2] = resRec->mag;
                vectFdata->addPoint(vect, resRec->ksiOC - dKsi, resRec->etaOC - dEta);
            }
    }
    vectFdata->detMean();
    if(r4data.doWeght) vectFdata->doWeght();
    vectFdata->initVF();

    vectFdata->printCoefs();

    vectFdata->saveVF(report4Dir+"/res.vf");

    vectFdata->saveDotList(report4Dir, "|");


}
*/
