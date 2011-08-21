#include <QtCore>

#include "./../libs/mb.h"
#include "./../libs/vectGrid3D.h"

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

#define IM_W 704
#define IM_H 704
#define IM_XC 620
#define IM_YC 512

void mos2im(double *xIm, double *yIm, int *npos, int *mpos, double xMos, double yMos);
void im2mos(double *xMos, double *yMos, int npos, int mpos, double xIm, double yIm);

struct transParam
{
    double Xc, Yc;
    double Xsc, Ysc;

    double scX;
    double rotX;
    double offX;

    double scY;
    double rotY;
    double offY;

    double dOffX;
    double dOffY;
    double distor;

};

void corrDist(double *x1, double *y1, double x0, double y0, transParam tPar);


int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);
   // QCoreApplication a(argc, argv);

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);


    QString fileName = QString(argv[1]);
    QString fileName1 = QString(argv[2]);
    QString rFileName = QString("%1.dcorr.lor").arg(fileName.left(fileName.lastIndexOf(".")));



    //QSettings *sett = new QSettings(argv[2], QSettings::IniFormat);
    QSettings *sett = new QSettings("./damDistCorr.ini", QSettings::IniFormat);

    transParam tPar;

    tPar.Xc = sett->value("general/Xc", 714.866000).toDouble() - IM_XC;
    tPar.Yc = sett->value("general/Yc", 517.058000).toDouble() - IM_YC;
    tPar.Xsc = sett->value("general/Xsc", 0.007000).toDouble();
    tPar.Ysc = sett->value("general/Ysc", -0.007000).toDouble();

    tPar.scX = sett->value("TransParam/scX", 1.0).toDouble();
    tPar.rotX = sett->value("TransParam/rotX", 0).toDouble();
    tPar.offX = sett->value("TransParam/offX", 0).toDouble();

    tPar.scY = sett->value("TransParam/scY", 1).toDouble();
    tPar.rotY = sett->value("TransParam/rotY", 0).toDouble();
    tPar.offY = sett->value("TransParam/offY", 0).toDouble();

    tPar.dOffX = sett->value("TransParam/dOffX", 0).toDouble();
    tPar.dOffY = sett->value("TransParam/dOffY", 0).toDouble();
    tPar.distor = sett->value("TransParam/distor", 0).toDouble();


    QFile inFile(fileName);
    inFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream inSm(&inFile);

    QFile inFile1(fileName1);
    inFile1.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream inSm1(&inFile1);


    //QString lorName = QString(argv[1]);

    QFile resFile(rFileName);
    resFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream resSm(&resFile);

    QFile cFile("./res.txt");
    cFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream cSm(&cFile);

    QFile c1File("./res1.txt");
    c1File.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream cSm1(&c1File);

    QString tStr;
    int i, szi, j, szj;
    double xt, yt, xn, yn;
    double x0, y0, x1, y1;
    double xCorr0, yCorr0, xCorr1, yCorr1;
    int npos, mpos;
    QStringList opers, resL;
    int k = 0;
    double dx0, dx1, dy0, dy1;

    QVector <double> dX, dY;
    QVector <double> dX1, dY1;

    QList <double*> nPos0, sPos0, nPos1, sPos1;

    double *vectT;

    while(!inSm.atEnd())
    {
        vectT = new double[2];
        tStr = inSm.readLine();
        opers = tStr.split(" ", QString::SkipEmptyParts);
        //qDebug() << tStr << "\n";
/////////////////////////////
        vectT[0] = opers.at(1).toDouble();
        vectT[1] = opers.at(2).toDouble();
        nPos0 << vectT;
    }
    inFile.close();
    while(!inSm1.atEnd())
    {
        vectT = new double[2];
        tStr = inSm1.readLine();
        opers = tStr.split(" ", QString::SkipEmptyParts);
        //qDebug() << tStr << "\n";

        vectT[0] = opers.at(1).toDouble();
        vectT[1] = opers.at(2).toDouble();
        sPos0 << vectT;
    }
    inFile1.close();

    qDebug() << QString("nPos size: %1\tsPos size: %2\n").arg(nPos0.size()).arg(sPos0.size());
    int dnum = nPos0.size();

    for(i=0; i<dnum; i++)
    {
        x0 = nPos0[i][0];
        y0 = nPos0[i][1];

        x1 = sPos0[i][0];
        y1 = sPos0[i][1];

        mos2im(&xt, &yt, &npos, &mpos, x0, y0);
        corrDist(&xn, &yn, xt, yt, tPar);
        im2mos(&xCorr0, &yCorr0, npos, mpos, xn, yn);

        mos2im(&xt, &yt, &npos, &mpos, x1, y1);
        corrDist(&xn, &yn, xt, yt, tPar);
        im2mos(&xCorr1, &yCorr1, npos, mpos, xn, yn);

        vectT = new double[2];
        vectT[0] = xCorr0;
        vectT[1] = yCorr0;
        nPos1 << vectT;

        vectT = new double[2];
        vectT[0] = xCorr1;
        vectT[1] = yCorr1;
        sPos1 << vectT;

    }






    double *C0, *L0, *Dx0, *Dy0;
    double *C1, *L1, *Dx1, *Dy1;
    int deg;
    int rn;
    double uweX0, uweY0, fl, maxdiv;
    double uweX1, uweY1;
    int *exclind;
    double *zX0, *zY0;
    double *zX1, *zY1;
    QStringList resZ, resRn, excList;
    fl = 2.0;
    maxdiv = 1;
    deg = 3;

    C0 = new double[dnum*deg];
    L0 = new double[dnum];
    Dx0 = new double[deg*deg];
    Dy0 = new double[deg*deg];
    zX0 = new double[deg];
    zY0 = new double[deg];


    for(i=0; i<dnum; i++)
    {
        C0[i*deg] = sPos0[i][0];
        C0[i*deg+1] = sPos0[i][1];
        C0[i*deg+2] = 1.0;

        L0[i] = nPos0[i][0];
    }

    //iLSM(deg, dnum, maxdiv, exclind, z, C, L, uwe, D, fl, rn);
    lsm(deg, dnum, zX0, C0, L0, uweX0, Dx0);

    qDebug() << QString("zX0: %1\t%2\t%3\n").arg(zX0[0]).arg(zX0[1]).arg(zX0[2]);

    for(i=0; i<dnum; i++) L0[i] = nPos0[i][1];
    lsm(deg, dnum, zY0, C0, L0, uweY0, Dy0);

    qDebug() << QString("zY0: %1\t%2\t%3\n").arg(zY0[0]).arg(zY0[1]).arg(zY0[2]);

    qDebug() << QString("uweX0: %1\tuweY0: %2\n").arg(uweX0).arg(uweY0);


    C1 = new double[dnum*deg];
    L1 = new double[dnum];
    Dx1 = new double[deg*deg];
    Dy1 = new double[deg*deg];
    zX1 = new double[deg];
    zY1 = new double[deg];


    for(i=0; i<dnum; i++)
    {
        C1[i*deg] = sPos1[i][0];
        C1[i*deg+1] = sPos1[i][1];
        C1[i*deg+2] = 1.0;

        L1[i] = nPos1[i][0];
    }

    //iLSM(deg, dnum, maxdiv, exclind, z, C, L, uwe, D, fl, rn);
    lsm(deg, dnum, zX1, C1, L1, uweX1, Dx1);

    qDebug() << QString("zX1: %1\t%2\t%3\n").arg(zX1[0]).arg(zX1[1]).arg(zX1[2]);

    for(i=0; i<dnum; i++) L1[i] = nPos1[i][1];
    lsm(deg, dnum, zY1, C1, L1, uweY1, Dy1);

    qDebug() << QString("zY1: %1\t%2\t%3\n").arg(zY1[0]).arg(zY1[1]).arg(zY1[2]);

    qDebug() << QString("uweX1: %1\tuweY1: %2\n").arg(uweX1).arg(uweY1);


    double *xLevels, *yLevels, *mLevels;
    int xLevNum, yLevNum, mLevNum;

    mLevNum = 2;
    mLevels = new double[2];
    mLevels[0] = 0.0;
    mLevels[1] = 1.0;

    xLevNum = yLevNum = 9;
    double stepX = 88;
    double stepY = 88;

    k=0;

    xLevels = new double[xLevNum];
    yLevels = new double[yLevNum];

    for(double xi=-352.0; xi<=352.0; xi+=88)
    {
        xLevels[k] = xi;
        yLevels[k] = xi;
        k++;
    }


    vectGrid3D *vectFdata0 = new vectGrid3D();

    vectFdata0->setLevels(xLevels, xLevNum, yLevels, yLevNum, mLevels, mLevNum);

    vectGrid3D *vectFdata1 = new vectGrid3D();

    vectFdata1->setLevels(xLevels, xLevNum, yLevels, yLevNum, mLevels, mLevNum);


    double *vectF = new double[3];


    for(i=0; i<dnum; i++)
    {
        x0 = nPos0[i][0];
        y0 = nPos0[i][1];

        x1 = zX0[0]*sPos0[i][0] + zX0[1]*sPos0[i][1] + zX0[2];
        y1 = zY0[0]*sPos0[i][0] + zY0[1]*sPos0[i][1] + zY0[2];

        dx0 = x0 - x1;
        dy0 = y0 - y1;

        xCorr0 = nPos1[i][0];
        yCorr0 = nPos1[i][1];

        xCorr1 = zX1[0]*sPos1[i][0] + zX1[1]*sPos1[i][1] + zX1[2];
        yCorr1 = zY1[0]*sPos1[i][0] + zY1[1]*sPos1[i][1] + zY1[2];

        dx1 = xCorr0 - xCorr1;
        dy1 = yCorr0 - yCorr1;


        dX << dx0;
        dY << dy0;

        dX1 << dx1;
        dY1 << dy1;

        mos2im(&xt, &yt, &npos, &mpos, x0, y0);

        vectF[0] = xt;
        vectF[1] = yt;
        vectF[2] = 0.5;
        vectFdata0->addPoint(vectF, dx0, dy0);
        cSm << QString("%1|%2|%3|%4\n").arg(xt).arg(yt).arg(dx0).arg(dy0);
        mos2im(&xt, &yt, &npos, &mpos, xCorr0, yCorr0);

        vectF[0] = xt;
        vectF[1] = yt;
        vectF[2] = 0.5;
        vectFdata1->addPoint(vectF, dx1, dy1);
        cSm1 << QString("%1|%2|%3|%4\n").arg(xt).arg(yt).arg(dx1).arg(dy1);



    }


    vectFdata0->detMean();
    vectFdata0->initVF();
    vectFdata0->saveVF("./res0.vf");
    vectFdata0->saveDotList("./", "|", "res0_");

    vectFdata1->detMean();
    vectFdata1->initVF();
    vectFdata1->saveVF("./res1.vf");
    vectFdata1->saveDotList("./", "|", "res1_");

    /*


    tStr = inSm.readLine();
    while(!inSm.atEnd())
    {


        tStr = inSm.readLine();
        opers = tStr.split(" ", QString::SkipEmptyParts);
        qDebug() << tStr << "\n";
/////////////////////////////
/*        x0 = opers.at(1).toDouble();
        y0 = opers.at(2).toDouble();

        tStr = inSm1.readLine();
        opers = tStr.split(" ", QString::SkipEmptyParts);
        qDebug() << tStr << "\n";

        x1 = opers.at(1).toDouble();
        y1 = opers.at(2).toDouble();

        dX << x0 - (21824 - x1);
        dY << y0 - (21824 - y1);

        mos2im(&xt, &yt, &npos, &mpos, x0, y0);
        corrDist(&xn, &yn, xt, yt, tPar);
        im2mos(&xCorr0, &yCorr0, npos, mpos, xn, yn);

        mos2im(&xt, &yt, &npos, &mpos, x1, y1);
        corrDist(&xn, &yn, xt, yt, tPar);
        im2mos(&xCorr1, &yCorr1, npos, mpos, xn, yn);

        dx0 = x0 - (21824 - x1);
        dy0 = y0 - (21824 - y1);
        dx1 = xCorr0 - (21824 - xCorr1);
        dy1 = yCorr0 - (21824 - yCorr1);

        qDebug() << QString("x0= %1\txCorr0 = %2\tdx0= %3\n").arg(x0).arg(xCorr0).arg(dx0);
        qDebug() << QString("y0= %1\tyCorr0 = %2\tdy0= %3\n").arg(y0).arg(yCorr0).arg(dy0);
        qDebug() << QString("x1= %1\txCorr1 = %2\tdx1= %3\n").arg(x1).arg(xCorr1).arg(dx1);
        qDebug() << QString("y1= %1\tyCorr1 = %2\tdy1= %3\n").arg(y1).arg(yCorr1).arg(dy1);

        dX1 << xCorr0 - (21824 - xCorr1);
        dY1 << yCorr0 - (21824 - yCorr1);

        cSm << QString("%1|%2|%3|%4\n").arg(x0).arg(y0).arg(dx0).arg(dy0);
        cSm1 << QString("%1|%2|%3|%4\n").arg(xCorr0).arg(yCorr0).arg(dx1).arg(dy1);
/
        ///////////////////////////////////////

        x0 = opers.at(1).toDouble();
        y0 = opers.at(2).toDouble();

        dx0 = x0 - opers.at(5).toDouble();
        dy0 = y0 - opers.at(6).toDouble();
        qDebug() << QString("x0: %1\ty0: %2\n").arg(x0).arg(y0);
        qDebug() << QString("dx0: %1\tdy0: %2\n").arg(dx0).arg(dy0);

        corrDist(&x1, &y1, x0, y0, tPar);
        dx1 = x0-x1;
        dy1 = y0-y1;
        qDebug() << QString("x1: %1\ty1: %2\n").arg(x1).arg(y1);
        qDebug() << QString("dx1: %1\tdy1: %2\n").arg(dx1).arg(dy1);
        //cSm << QString("%1|%2|%3|%4|%5|%6\n").arg(dx0).arg(dx1).arg(dy0).arg(dy1).arg(dx0-dx1).arg(dy0-dy1);
        cSm << QString("%1|%2|%3|%4\n").arg(x0).arg(y0).arg(dx0).arg(dy0);
        cSm1 << QString("%1|%2|%3|%4|%5|%6\n").arg(x1).arg(y1).arg(dx1).arg(dy1).arg(dx0-dx1).arg(dy0-dy1);

        /*
        x0 = opers.at(1).toDouble();
        y0 = opers.at(2).toDouble();

        //qDebug() << QString("x0: %1\ty0: %2\n").arg(x0).arg(y0);

        mos2im(&xt, &yt, &npos, &mpos, x0, y0);
        //qDebug() << QString("npos: %1\tmpos: %2\n").arg(npos).arg(mpos);
        //qDebug() << QString("xt: %1\tyt: %2\n").arg(xt).arg(yt);

        corrDist(&x1, &y1, xt, yt, tPar);
        //qDebug() << QString("x1: %1\ty1: %2\n").arg(x1).arg(y1);
        //qDebug() << QString("dx: %1\tdy: %2\n").arg(x1-xt).arg(y1-yt);

        im2mos(&xt, &yt, npos, mpos, x1, y1);
        //qDebug() << QString("xCorr: %1\tyCorr: %2\n").arg(xt).arg(yt);
        //qDebug() << QString("dx1: %1\tdy1: %2\n").arg(x0-xt).arg(y0-yt);

        //resL.clear();
        //resL << opers.at(0);
        resSm << QString("%1%2%3%4%5\n").arg(opers.at(0), 10).arg(xt, 13, 'f', 3).arg(yt, 13, 'f', 3).arg(opers.at(3), 13).arg(opers.at(4), 13);

        cSm << QString("%1|%2|%3|%4\n").arg(x0).arg(y0).arg(x0-xt).arg(y0-yt);

     /*   opers.removeAt(1);
        opers.insert(1, QString("%1").arg(xt));
        opers.removeAt(2);
        opers.insert(2, QString("%2").arg(xt));
        resSm << opers.join(" ") << "\n";/

        k++;
    }
*/

//    qDebug() << QString("\ntotal %1 records\n").arg(k);


    szi = dX.size();

    double meanDx0, meanDx1, rmsOneDx0, rmsOneDx1, rmsMeanDx0, rmsMeanDx1;
    double meanDy0, meanDy1, rmsOneDy0, rmsOneDy1, rmsMeanDy0, rmsMeanDy1;

    meanDx0 =  meanDx1 = rmsOneDx0 = rmsOneDx1 = rmsMeanDx0 = rmsMeanDx1 = 0.0;
    meanDy0 =  meanDy1 = rmsOneDy0 = rmsOneDy1 = rmsMeanDy0 = rmsMeanDy1 = 0.0;
    for(i=0; i<szi; i++)
    {
        meanDx0 += dX[i];
        meanDx1 += dX1[i];
        meanDy0 += dY[i];
        meanDy1 += dY1[i];
    }

    meanDx0 /= szi;
    meanDx1 /= szi;
    meanDy0 /= szi;
    meanDy1 /= szi;

    for(i=0; i<szi; i++)
    {
        rmsOneDx0 += pow(meanDx0 - dX[i], 2.0);
        rmsOneDx1 += pow(meanDx1 - dX1[i], 2.0);
        rmsOneDy0 += pow(meanDy0 - dY[i], 2.0);
        rmsOneDy1 += pow(meanDy1 - dY1[i], 2.0);
    }

    rmsOneDx0 = sqrt(rmsOneDx0/(szi-1.0));
    rmsOneDx1 = sqrt(rmsOneDx1/(szi-1.0));
    rmsOneDy0 = sqrt(rmsOneDy0/(szi-1.0));
    rmsOneDy1 = sqrt(rmsOneDy1/(szi-1.0));

    rmsMeanDx0 = rmsOneDx0/(szi*1.0);
    rmsMeanDx1 = rmsOneDx1/(szi*1.0);
    rmsMeanDy0 = rmsOneDy0/(szi*1.0);
    rmsMeanDy1 = rmsOneDy1/(szi*1.0);

    qDebug() << QString("meanDx0= %1\tmeanDx1= %2\tmeanDy0= %3\tmeanDy1= %4\n").arg(meanDx0).arg(meanDx1).arg(meanDy0).arg(meanDy1);
    qDebug() << QString("rmsOneDx0= %1\trmsOneDx1= %2\trmsOneDy0= %3\trmsOneDy1= %4\n").arg(rmsOneDx0).arg(rmsOneDx1).arg(rmsOneDy0).arg(rmsOneDy1);
    qDebug() << QString("rmsMeanDx0= %1\trmsMeanDx1= %2\trmsMeanDy0= %3\trmsMeanDy1= %4\n").arg(rmsMeanDx0).arg(rmsMeanDx1).arg(rmsMeanDy0).arg(rmsMeanDy1);


/*


    double Nx0 = 0;
    double Nx1 = 704;
    double Ny0 = 0;
    double Ny1 = 704;
    double stepX = 87.999;
    double stepY = 87.999;



    szi = (fabs(Nx1-Nx0)/stepX)+1;
    szj = (fabs(Ny1-Ny0)/stepY)+1;
    qDebug() << QString("szi= %1\yszj= %2\n").arg(szi).arg(szj);
    int dotNum = szi*szj;
    qDebug() << QString("dotNum= %1\n").arg(dotNum);

    double xp, yp;
    //i=0;
   // j=0;
    for(xp = Nx0; xp<=Nx1; xp+=stepX)
    {
       // j=0;
        for(yp = Ny0; yp<=Ny1; yp+=stepY)
        {
            x0 = xp;
            y0 = yp;

            qDebug() << QString("x0: %1\ty0: %2\n").arg(x0).arg(y0);

            mos2im(&xt, &yt, &npos, &mpos, x0, y0);
            qDebug() << QString("npos: %1\tmpos: %2\n").arg(npos).arg(mpos);
            qDebug() << QString("xt: %1\tyt: %2\n").arg(xt).arg(yt);

            corrDist(&x1, &y1, xt, yt, tPar);
            qDebug() << QString("x1: %1\ty1: %2\n").arg(x1).arg(y1);
            qDebug() << QString("dx: %1\tdy: %2\n").arg(x1-xt).arg(y1-yt);

            im2mos(&xt, &yt, npos, mpos, x1, y1);
            qDebug() << QString("xCorr: %1\tyCorr: %2\n").arg(xt).arg(yt);
            qDebug() << QString("dx1: %1\tdy1: %2\n").arg(x0-xt).arg(y0-yt);

            //resL.clear();
            //resL << opers.at(0);
            cSm << QString("%1|%2|%3|%4\n").arg(x0).arg(y0).arg(x0-xt).arg(y0-yt);

            qDebug() << "\n=============================================\n";
        }
    }
*/




    inFile.close();
    resFile.close();
    cFile.close();
    c1File.close();

    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;

    qInstallMsgHandler(0);

    return 0;
}

void mos2im(double *xIm, double *yIm, int *npos, int *mpos, double xMos, double yMos)
{
    int n, m;
    n = xMos/IM_W;
    m = yMos/IM_H;

    *npos = n;
    *mpos = m;

    double xt, yt;
    xt = xMos - n*IM_W;
    yt = yMos - m*IM_H;
    *xIm = xt - IM_W/2;
    *yIm = yt - IM_H/2;
}

void im2mos(double *xMos, double *yMos, int npos, int mpos, double xIm, double yIm)
{
    double xt, yt;
    xt = xIm + IM_W/2;
    yt = yIm + IM_H/2;

    *xMos = xt + npos*IM_W;
    *yMos = yt + mpos*IM_H;
}

void corrDist(double *x1, double *y1, double x0, double y0, transParam tPar)
{

    x0 = (x0-tPar.Xc)*tPar.Xsc;
    y0 = (y0-tPar.Yc)*tPar.Ysc;
/*    x0 *= tPar.Xsc;
    y0 *= tPar.Ysc;*/

    qDebug() << QString("Xsc= %1\tYsc= %2\n").arg(tPar.Xsc).arg(tPar.Xsc);


    double offX = tPar.offX;
    double offY = tPar.offY;
    double x2, y2;
    double A, a, B, b, C, c, D, d;

    double xt, yt;

    double dx, dy;

    qDebug() << QString("x0: %1\ty0: %2\n").arg(x0).arg(y0);


    A = tPar.scX*x0;
    B = tPar.rotX*y0;
    C = -tPar.offX*(3.0*x0*x0+y0*y0)-tPar.offY*(2.0*x0*y0);

    a = tPar.scY*y0;
    b = tPar.rotY*x0;
    c = -tPar.offY*(x0*x0+3.0*y0*y0)-tPar.offX*(2.0*x0*y0);

    D = tPar.distor*x0*(pow(x0, 2.0) + pow(y0, 2.0));
    d = tPar.distor*y0*(pow(y0, 2.0) + pow(x0, 2.0));

    qDebug() << QString("A: %1\tB: %2\tC: %3\tD: %4\n").arg(A).arg(B).arg(C).arg(D);
    qDebug() << QString("a: %1\tb: %2\tc: %3\td: %4\n").arg(a).arg(b).arg(c).arg(d);

    dx = A + B + C + D;
    dy = a + b + c + d;

    x2 = dx;
    y2 = dy;

    qDebug() << QString("dx: %1\tdy: %2\n").arg(dx).arg(dy);
    qDebug() << QString("x2: %1\ty2: %2\n").arg(x2).arg(y2);
    qDebug() << QString("dx2: %1\tdy2: %2\n").arg(x2-x0).arg(y2-y0);

    //qDebug() << QString("x0= %1\tdx= %2\tx2= %3\n").arg(x0).arg(dx).arg(x2);
    //qDebug() << QString("y0= %1\tdy= %2\ty2= %3\n").arg(y0).arg(dy).arg(y2);
    /*

    //qDebug() << QString("X0: %1\tY0: %2\n").arg(x0).arg(y0);
/*
    double A = tPar.scX*cos(tPar.rotX);
    double B = tPar.scY*sin(tPar.rotY);
    //double C = tPar.distor*(pow((x0 - tPar.dOffX), 2.0) + pow((y0 - tPar.dOffY), 2.0))*(x0 - tPar.dOffX);
    double C = -tPar.scX*offX*cos(tPar.rotX) - tPar.scY*offY*sin(tPar.rotY);

    double a = -tPar.scX*sin(tPar.rotX);
    double b = tPar.scY*cos(tPar.rotY);
    double c = tPar.scX*offX*sin(tPar.rotX) - tPar.scY*offY*cos(tPar.rotY);

    double xt = A*x0 + B*y0 + C;
    double yt = a*x0 + b*y0 + c;
/
    dx = A + B + C + D;
    dy = a + b + c + d;

    qDebug() << QString("dx: %1\tdy: %2\n").arg(dx).arg(dy);
/*
    //double c = tPar.distor*(pow((y0 - tPar.dOffY), 2.0) + pow((x0 - tPar.dOffX), 2.0))*(y0 - tPar.dOffY);

    xt = x0 - dx;
    yt = y0 - dy;

    qDebug() << QString("xt: %1\tyt: %2\n").arg(xt).arg(yt);

    D = tPar.distor*(xt - tPar.dOffX)*(pow((xt - tPar.dOffX), 2.0) + pow((yt - tPar.dOffY), 2.0));
    d = tPar.distor*(yt - tPar.dOffY)*(pow((yt - tPar.dOffY), 2.0) + pow((xt - tPar.dOffX), 2.0));
    qDebug() << QString("A= %1\tB= %2\tC= %3\tD= %4\n").arg(A).arg(B).arg(C).arg(D);
    qDebug() << QString("a= %1\tb= %2\tc= %3\td= %4\n").arg(a).arg(b).arg(c).arg(d);

    //xt -= D;
    //yt -= d;

    //x2 = xt + dx;
    //y2 = yt + dy;

    x2 = x0 - D;
    y2 = y0 - d;
/

    x2 = dx;
    y2 = dy;

    qDebug() << QString("x2: %1\ty2: %2\n").arg(x2).arg(y2);
    qDebug() << QString("dx2: %1\tdy2: %2\n").arg(x2-x0).arg(y2-y0);

    //qDebug() << QString("XT: %1\tYT: %2\n").arg(xt).arg(yt);
/*
    double *mI = new double[4];
    mI[0] = A;
    mI[1] = B;
    mI[2] = a;
    mI[3] = b;

    double *mIn = new double[4];

    double *xnew = new double[2];
    double *xold = new double[2];

    xold[0] = xt - C;
    xold[1] = yt - c;

    if(!invgj(mIn, mI, 2)) qDebug() << "\nобратить не удалось\n";
    mvprod(xnew, mIn, xold, 2, 2);

/*
    A = tPar.scX*cos(-tPar.rotX);
    B = tPar.scY*sin(-tPar.rotY);
    //C = -tPar.scX*(-offX)*cos(-tPar.rotX) - tPar.scY*(-offY)*sin(-tPar.rotY);
    C = -C;

    a = -tPar.scX*sin(-tPar.rotX);
    b = tPar.scY*cos(-tPar.rotY);
    //c = tPar.scX*(-offX)*sin(-tPar.rotX) - tPar.scY*(-offY)*cos(-tPar.rotY);
    c = -c;

    xt += C;
    yt += c;
    x2 = A*xt + B*yt;
    y2 = a*xt + b*yt;
/*
    x2 = x0 + D;
    y2 = y0 + D;/

    x2 = xnew[0];
    y2 = xnew[1];

    //qDebug() << QString("X1: %1\tY1: %2\n").arg(x2).arg(y2);

    //qDebug() << QString("A= %1\tB= %2\tC= %3\tD= %4\n").arg(A).arg(B).arg(C).arg(D);
    //qDebug() << QString("a= %1\tb= %2\tc= %3\td= %4\n").arg(a).arg(b).arg(c).arg(d);

*/
/*    *x1 = x2;
    *y1 = y2;
/*
    *x1 = x2/tPar.Xsc;
    *y1 = y2/tPar.Ysc;
*/

    *x1 = x2/tPar.Xsc + tPar.Xc;
    *y1 = y2/tPar.Ysc + tPar.Yc;
//
}
