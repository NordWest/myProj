#include <QtCore/QCoreApplication>

#include "./../libs/fitsdata.h"
#include "./../astro/astro.h"
#include "./../libs/comfunc.h"
#include "./../libs/observatory.h"
#include "./../libs/redStat.h"

static QTextStream* clog = 0;
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


void detRt(double *r, double ra, double de)
{
    r[0] = cos(ra)*cos(de);
    r[1] = sin(ra)*cos(de);
    r[2] = sin(de);
}

void detT0(double *T, double *r)
{
    T[0] = -(r[1])/(sqrt(r[0]*r[0]+r[1]*r[1]));
    T[1] = (r[0])/(sqrt(r[0]*r[0]+r[1]*r[1]));
    T[2] = 0.0;
    T[3] = -(r[0]*r[2])/(sqrt(r[0]*r[0]+r[1]*r[1]));
    T[4] = -(r[1]*r[2])/(sqrt(r[0]*r[0]+r[1]*r[1]));
    T[5] = sqrt(r[0]*r[0]+r[1]*r[1]);
    T[6] = r[0];
    T[7] = r[1];
    T[8] = r[2];
}

void t2enz(double *T, double *ksi, double *eta, double *zeta)
{
    ksi[0] = T[0];
    ksi[1] = T[1];
    ksi[2] = T[2];
    eta[0] = T[3];
    eta[1] = T[4];
    eta[2] = T[5];
    zeta[0] = T[6];
    zeta[1] = T[7];
    zeta[2] = T[8];
}

void detAii(double *Aii, double *T1, double *T2)
{
    double *ksi1 = new double[3];
    double *eta1 = new double[3];
    double *zeta1 = new double[3];
    double *ksi2 = new double[3];
    double *eta2 = new double[3];
    double *zeta2 = new double[3];

    t2enz(T1, ksi1, eta1, zeta1);
    t2enz(T2, ksi2, eta2, zeta2);

    Aii[0] = Smul3(ksi2, ksi1);
    Aii[1] = Smul3(ksi2, eta1);
    Aii[2] = Smul3(ksi2, zeta1);
    Aii[3] = Smul3(eta2, ksi1);
    Aii[4] = Smul3(eta2, eta1);
    Aii[5] = Smul3(eta2, zeta1);
    Aii[6] = Smul3(zeta2, ksi1);
    Aii[7] = Smul3(zeta2, eta1);
    Aii[8] = Smul3(zeta2, zeta1);
/*
    delete [] ksi1;
    delete [] eta1;
    delete [] zeta1;
    delete [] ksi2;
    delete [] eta2;
    delete [] zeta2;
*/
}


int main(int argc, char *argv[]) //mergeFits err_budget.txt resFolder
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

    QFile* logFile = new QFile("./mergeFits.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QTextStream(logFile);

    if(argc<3)
    {
        qDebug() << "argc= " << argc << "\n";
        return 1;
    }

    QList <catFinder*> starCatList;
    QString obsCode;
    int szi, i, posMean, j, szj, p, szp, pos;
    int serSz, si;
    QString dateCode;
    QString resFile;
    double tMean, tmin, ti;


    QList <errBudgetFile*> ebSeriesList;
//command line  ///////////////////////////////////////
    double ra, de, x1, y1, x2, y2, cosPP, ksi1, eta1, ksi2, eta2, iF;
    double *xT0 = new double[3];
    double *xTi = new double[3];
    double *r1 = new double[3];
    double *r2 = new double[3];

    double *T0 = new double[9];
    double *Ti = new double[9];
    double *TiT = new double[9];
    double *Ai = new double[9];

    real_1d_array xarr;		//Массивы координат опорных точек функции f(x, y) по обеим осям
    real_1d_array yarr;

    real_2d_array fI;		//Массивы значений
    //real_2d_array fY;
    //real_2d_array fN;

    spline2dinterpolant coefI;

    errBudgetFile errB;
    errBudgetFile *errBtemp;
    errB.init(argv[1]);

    QString resFolder = QString(argv[2]);

    //sortErrBList(errB.errList);
    detErrBSeriesList(errB.errList, &ebSeriesList, 9);
    serSz = ebSeriesList.size();
    qDebug() << QString("find %1 series\n").arg(serSz);
    for(si=0; si< serSz; si++)
    {
        qDebug() << QString("################ Serie %1 #################\n").arg(si);
        errBtemp = ebSeriesList.at(si);
        szi = errBtemp->errList.size();
        for(i=0; i< szi; i++)
        {
            qDebug() << QString("time %1: %2 | %3\n").arg(i).arg(errBtemp->errList.at(i)->MJD, 12, 'f', 6).arg(errBtemp->errList.at(i)->originName);
        }
    }


    QString obsName;
    int *nums;

    for(si=0; si< serSz; si++)
    {
        qDebug() << QString("################ Serie %1 #################\n").arg(si);
        errBtemp = ebSeriesList.at(si);

///////////////////////////////////////
        szi = errBtemp->errList.size();
        if(szi<2) continue;
        tMean = 0.0;
        for(i=0; i<szi; i++)
        {
            tMean += errBtemp->errList.at(i)->MJD;
            qDebug() << QString("time %1: %2 | %3\n").arg(i).arg(errBtemp->errList.at(i)->MJD, 12, 'f', 6).arg(errBtemp->errList.at(i)->originName);
        }
        tMean /= szi;
        posMean = 0;
        tmin = fabs(errBtemp->errList.at(0)->MJD-tMean);
        for(i=1; i<szi; i++)
        {
            ti = fabs(errBtemp->errList.at(i)->MJD-tMean);
            if(ti<tmin)
            {
                tmin = ti;
                posMean = i;
            }
        }

        qDebug() << QString("posMean: %1\ttmin: %2\n").arg(posMean).arg(tmin);

        fitsdata fitsM, fitsT;

        fitsM.clear();

        detPlateName(&obsName, errBtemp->errList.at(posMean)->originName, 1);
        fitsM.openFile(errBtemp->errList.at(posMean)->originName);
        int n = fitsM.workFrame.width();
        int m = fitsM.workFrame.height();

        szi = n*m;
        nums = new int[szi];
        for(i=0; i<szi; i++) nums[i]=1;



        detRt(xT0, errBtemp->errList.at(posMean)->RAoc, errBtemp->errList.at(posMean)->DEoc);
        /*xT0[0] = cos(errBtemp->errList.at(posMean)->RAoc)*cos(errBtemp->errList.at(posMean)->DEoc);
        xT0[1] = sin(errBtemp->errList.at(posMean)->RAoc)*cos(errBtemp->errList.at(posMean)->DEoc);
        xT0[2] = sin(errBtemp->errList.at(posMean)->DEoc);*/

        detT0(T0, xT0);



        //spline2dinterpolant coefY;
        //spline2dinterpolant coefN;

        xarr.setlength(n);
        yarr.setlength(m);

        for(i=0; i<n; i++) xarr(i) = i;
        for(i=0; i<m; i++) yarr(i) = i;

        fI.setlength(n, m);


        szp = errBtemp->errList.size();
        for(p=0; p<szp; p++)
        {
            if(i==posMean) continue;

            //fitsT.clear();
            if(fitsT.openFile(errBtemp->errList.at(p)->originName))
            {
                qDebug() << "openFile error\n";
                return 1;
            }


    /*
            for(j=0; j<m; j++)
            {
                for(i=0; i<n; i++)
                {
                    fI(i, j) = fitsT.imgArr->getImgPixValue(i, j);
                }
            }
    */

    /*
            spline2dbuildbicubic(xarr, yarr, fI, n, m, coefI);
            qDebug() << QString("\n%1\n").arg(p);
            spline2dinterpolant interI(coefI);
    */


            detRt(xTi, errBtemp->errList.at(p)->RAoc, errBtemp->errList.at(p)->DEoc);
            detT0(Ti, xTi);
            detAii(Ai, Ti, T0);

            for(j=0; j<m; j++)
            {
                for(i=0; i<n; i++)
                {
                    detTan6const(&ksi1, &eta1, i, j, errBtemp->errList.at(p)->xParams, errBtemp->errList.at(p)->yParams);
                    r1[0] = ksi1;
                    r1[1] = eta1;
                    r1[2] = 1;
                    ksi2 = (Ai[0]*ksi1 + Ai[1]*eta1 + Ai[2])/(Ai[7]*ksi1 + Ai[8]*eta1 + Ai[9]);
                    eta2 = (Ai[3]*ksi1 + Ai[4]*eta1 + Ai[5])/(Ai[7]*ksi1 + Ai[8]*eta1 + Ai[9]);
                    detXcYc6const(&x2, &y2, ksi2, eta2, errBtemp->errList.at(posMean)->xParams, errBtemp->errList.at(posMean)->yParams);
                    //iF = spline2dcalc(interI, i, j);
                    iF = fitsM.imgArr->getImgPixValue(i, j);
                    pos = fitsM.imgArr->detPos(x2, y2);
                    fitsM.imgArr->setVal(fitsM.imgArr->getImgPixValue(x2, y2)+iF, pos);
                    nums[pos]++;
                }
            }


        }

        for(j=0; j<m; j++)
        {
            for(i=0; i<n; i++)
            {
                pos = fitsT.imgArr->detPos(i, j);
                fitsM.imgArr->setVal(fitsT.imgArr->getImgPixValue(i, j)/(1.0*nums[pos]), pos);
            }
        }


        delete [] nums;

        QString rfName = resFolder+QDir().separator()+obsName+".fit";
        qDebug() << QString("rfName: %1").arg(rfName);
        fitsM.saveFitsAs(rfName);
    }

    qDebug() << "\n\nEND mergeFits\n\n";

    logFile->close();
    delete clog;
    clog = 0;
    delete logFile;
    qInstallMsgHandler(0);

    return 0;//a.exec();
}
