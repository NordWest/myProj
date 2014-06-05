#include <QtCore>
#include <QFile>
#include <QSettings>
#include <QTextStream>
#include <QList>
#include <QDebug>
#include <QRgb>
#include <QPen>
#include <QBrush>
#include <QPoint>
#include <QPainter>

#include <math.h>

#include <astro.h>
#include "./../../libs/comfunc.h"
#include <mb.h>
#include "./../../libs/ringpix.h"
#include "./../../libs/sphere.h"

//#define PI 3.141592653589

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

struct ObsObjRec
{
    QString objName;
    QString obsCode;
    int numRa, numDec;
    double meanRa, rmsMeanRa, rmsOneRa;
    double meanDec, rmsMeanDec, rmsOneDec;
};
class ObjObsStat
{
public:
    QList <ObsObjRec*> ooList;
    ObjObsStat(){};
    int init(QString fileName)
    {
        QString tStr;
        ObsObjRec* tRec;
        int isN;
        QStringList opers, op1;
        QFile iniFile(fileName);
        if(!iniFile.open(QFile::ReadOnly)) return 1;
        QTextStream iniStm(&iniFile);
        while(!iniStm.atEnd())
        {
            tRec = new ObsObjRec;
            tStr = iniStm.readLine();
            opers = tStr.split("#");
            tRec->objName = opers.at(0).section("@", 0, 0).simplified();
            tRec->obsCode = opers.at(0).section("@", 1, 1).simplified();
            tRec->numRa = opers.at(1).section("|", 0, 0).toInt();
            tRec->meanRa = opers.at(1).section("|", 1, 1).toDouble();
            tRec->rmsMeanRa = opers.at(1).section("|", 2, 2).toDouble();
            tRec->rmsOneRa = opers.at(1).section("|", 3, 3).toDouble();
            tRec->numDec = opers.at(2).section("|", 0, 0).toInt();
            tRec->meanDec = opers.at(2).section("|", 1, 1).toDouble();
            tRec->rmsMeanDec = opers.at(2).section("|", 2, 2).toDouble();
            tRec->rmsOneDec = opers.at(2).section("|", 3, 3).toDouble();
            ooList << tRec;
        }
        iniFile.close();
        return 0;
    };
    int getStat(QString objN, QString obsC, ObsObjRec* rRes)
    {
        int i, sz;
        sz = ooList.size();
        for(i=0;i<sz;i++)
        {
            if((QString().compare(ooList.at(i)->objName, objN)==0)&&(QString().compare(ooList.at(i)->obsCode, obsC)==0))
            {
                rRes = ooList.at(i);
                return 0;
            }
        }
        return 1;
    }
};

int randomSphere(double *ra, double *de, int num, double raMin, double raMax, double deMin, double deMax, int csys = 0);
int randomSphereHpix(QVector <double> &ra, QVector <double> &de, int nsMax, double raMin, double raMax, double deMin, double deMax, int csys);
void get2Ddisp(double &z0, double &z1);

int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);

    int i;
    double raMin, raMax, deMin, deMax;
    QSettings *sett = new QSettings("sph.ini", QSettings::IniFormat);

    raMin = sett->value("general/raMin", 0).toDouble()*15.0;   //degree
    raMax = sett->value("general/raMax", 24).toDouble()*15.0;   //
    deMin = grad2rad(sett->value("general/deMin", -90).toDouble());        //
    deMax = grad2rad(sett->value("general/deMax", 90).toDouble());        //
    int pointNum = sett->value("general/pointNum", 0).toInt();
    //int nsMax = sett->value("general/nsMax", 32).toInt();

    int rtype = sett->value("general/rtype", 0).toInt();
    //0 - randomSphere
    //1 - randomSphereHpix
    //2 - real points - do mod
    //3 - real points - not to mod, just summ

    QString inpFileName = sett->value("general/inpFileName", "inp.txt").toString();

//input
    QString colSep = sett->value("input/colSep", "|").toString();
    int ct = sett->value("input/ct", 0).toInt();
    int cx = sett->value("input/cx", 0).toInt();
    int cy = sett->value("input/cy", 1).toInt();
    int cxc = sett->value("input/cxc", 2).toInt();
    int cyc = sett->value("input/cyc", 3).toInt();
    int cn = sett->value("input/cn", 4).toInt();
    int isDegree = sett->value("input/isDegree", 0).toInt();


    int isWe = sett->value("general/isWe", 0).toInt();
    int isImpr = sett->value("general/isImpr", 0).toInt();

    double time0 = mas_to_rad(sett->value("rotation/time0", 0).toDouble());

    double epsT[3];
    double *Eps = new double[3];
    Eps[0] = mas_to_rad(sett->value("rotation/w1", 0).toDouble());
    Eps[1] = mas_to_rad(sett->value("rotation/w2", 0).toDouble());
    Eps[2] = mas_to_rad(sett->value("rotation/w3", 0).toDouble());
    double *mMatr = new double[9];
    mMatr[0] = mas_to_rad(sett->value("rotation/M11", 0).toDouble());
    mMatr[1] = mas_to_rad(sett->value("rotation/M12", 0).toDouble());
    mMatr[2] = mas_to_rad(sett->value("rotation/M13", 0).toDouble());
    mMatr[3] = mas_to_rad(sett->value("rotation/M21", 0).toDouble());
    mMatr[4] = mas_to_rad(sett->value("rotation/M22", 0).toDouble());
    mMatr[5] = mas_to_rad(sett->value("rotation/M23", 0).toDouble());
    mMatr[6] = mas_to_rad(sett->value("rotation/M31", 0).toDouble());
    mMatr[7] = mas_to_rad(sett->value("rotation/M32", 0).toDouble());
    mMatr[8] = mas_to_rad(sett->value("rotation/M33", 0).toDouble());

    double *dEps = new double[3];
    dEps[0] = mas_to_rad(sett->value("rotation/dw1", 0).toDouble());
    dEps[1] = mas_to_rad(sett->value("rotation/dw2", 0).toDouble());
    dEps[2] = mas_to_rad(sett->value("rotation/dw3", 0).toDouble());
    double *mMatrD = new double[9];
    mMatrD[0] = mas_to_rad(sett->value("rotation/dM11", 0).toDouble());
    mMatrD[1] = mas_to_rad(sett->value("rotation/dM12", 0).toDouble());
    mMatrD[2] = mas_to_rad(sett->value("rotation/dM13", 0).toDouble());
    mMatrD[3] = mas_to_rad(sett->value("rotation/dM21", 0).toDouble());
    mMatrD[4] = mas_to_rad(sett->value("rotation/dM22", 0).toDouble());
    mMatrD[5] = mas_to_rad(sett->value("rotation/dM23", 0).toDouble());
    mMatrD[6] = mas_to_rad(sett->value("rotation/dM31", 0).toDouble());
    mMatrD[7] = mas_to_rad(sett->value("rotation/dM32", 0).toDouble());
    mMatrD[8] = mas_to_rad(sett->value("rotation/dM33", 0).toDouble());

//objSphere
    long nsMax = sett->value("objSphere/nsMax", 32).toLongLong();
    int isEcl = sett->value("objSphere/isEcl", 0).toInt();
    int pMin = sett->value("objSphere/pMin", 10).toInt();
    double dMin = grad2rad(sett->value("objSphere/dMin", -90).toDouble());
    double dMax = grad2rad(sett->value("objSphere/dMax", 90).toDouble());
    int isZonal = sett->value("objSphere/isZonal", 0).toInt();
    int isZonalB = sett->value("objSphere/isZonalBack", 0).toInt();

    int isUBmod = sett->value("ubMod/isUBmod", 0).toInt();        //
    double kPar = sett->value("ubMod/kPar", 300).toDouble();        //
    double bPar = sett->value("ubMod/bPar", 300).toDouble();
    double disp1;
    double dT;

    double s1 = sin(dMin);
    double s2 = sin(dMax);

    int csys = sett->value("general/coord_sys", 0).toInt();

    double disp = mas_to_rad(sett->value("noice/disp", 0).toDouble());

    QList <double*> objList;
    QList <double*> objListRot;
    double *obj, *objR;

    double x, y, z, z0, z1, lns, s;
    double *ra, *dec;
    double *raC, *decC;
    double *dRa, *dDec;
    double *raO, *decO;
    double lam, beta;
    double h, phi;
    int *numI;
    long ipix;
    int pN = 0;
    double ti;
    QString objName, obsCode;

    QVector <double> tmVect;
    QVector <double> raVect;
    QVector <double> deVect;
    QVector <double> raVectC;
    QVector <double> deVectC;
    QVector <int> numVect;

    QFile iFile;
    QTextStream iStm;
    QString tStr;

    double ocRa, ocDec;
    double ocRa0, ocDec0;

    double rat, dect, ratC, dectC;
    double *raAve, *deAve;

    srand(time(NULL));

//////////////////////////////////////////////////////////////////
    switch(rtype)
    {
        case 0:
        {
            ra = new double[pointNum];
            dec = new double[pointNum];
            randomSphere(ra, dec, pointNum, grad2rad(raMin), grad2rad(raMax), deMin, deMax, csys);
        }
        break;
        case 1:
        {
            randomSphereHpix(raVect, deVect, nsMax, grad2rad(raMin), raMax, deMin, grad2rad(deMax), csys);
            pointNum = raVect.size();
            ra = new double[pointNum];
            dec = new double[pointNum];

            for(i=0; i<pointNum; i++)
            {
                ra[i] = raVect[i];
                dec[i] = deVect[i];
                numVect << 20;
            }
        }
        break;
        case 2:
        {
            iFile.setFileName(argv[1]);
            iFile.open(QFile::ReadOnly);
            iStm.setDevice(&iFile);

            ObjObsStat ooSt;
            ObsObjRec ooRec;
            if(isWe)
            {
                if(ooSt.init(QString("%1.we").arg(QFileInfo(argv[1]).absoluteFilePath().section("_", 0, 0))))
                {
                    qDebug() << "Error init weghts\n\n";
                    return 1;
                }
            }
            int skipNum = 0;
            int obsNum = 0;

            QFile resFile(QString("%1_sph.txt").arg(QFileInfo(argv[1]).absoluteFilePath().section(".", -2, -2)));
            resFile.open(QFile::WriteOnly | QFile::Truncate);
            QTextStream resStm(&resFile);

            double *aMatr = new double[5];

            while(!iStm.atEnd())
            {
                tStr = iStm.readLine();
                objName = tStr.section(colSep, 0, 0).simplified();
                ti = tStr.section(colSep, ct, ct).toDouble();
                rat = tStr.section(colSep, cx, cx).toDouble();
                dect = tStr.section(colSep, cy, cy).toDouble();
                ratC = tStr.section(colSep, cxc, cxc).toDouble();
                dectC = tStr.section(colSep, cyc, cyc).toDouble();
                obsCode = tStr.section(colSep, -1, -1).simplified();

                ocRa0 = (rat-ratC)*cos(dect);
                ocDec0 = dect - dectC;

                if(isDegree)
                {
                    rat = grad2rad(rat);
                    dect = grad2rad(dect);
                }

                if(ooSt.getStat(objName, obsCode, &ooRec))
                {
                    //qDebug() << QString("Skip %1@%2\n").arg(objName).arg(obsCode);
                    skipNum++;
                    //continue;
                    ooRec.rmsOneRa = disp;
                    ooRec.rmsOneDec = disp;
                }

                aMatr[0] = -(sin(dect)*cos(rat));
                aMatr[1] = -(sin(dect)*sin(rat));
                aMatr[2] = cos(dect);

                aMatr[3] = sin(rat);
                aMatr[4] = -cos(rat);

                get2Ddisp(z0, z1);

                dT = ti-time0;
                epsT[0] = Eps[0] + dT*dEps[0];
                epsT[1] = Eps[1] + dT*dEps[1];
                epsT[2] = Eps[2] + dT*dEps[2];

                ocRa = aMatr[0]*epsT[0]+aMatr[1]*epsT[1]+aMatr[2]*epsT[2] - mMatr[2]*sin(dect)*sin(rat) + mMatr[5]*sin(dect)*cos(rat) + mMatr[1]*cos(dect)*cos(2.0*rat) - 0.5*mMatr[0]*cos(dect)*sin(2.0*rat) + 0.5*mMatr[4]*cos(dect)*sin(3.0*rat) + mas2rad(ooRec.rmsOneRa)*z0;
                ocDec = aMatr[3]*epsT[0]+aMatr[4]*epsT[1] - 0.5*mMatr[1]*sin(2.0*dect)*sin(2.0*rat) + mMatr[2]*cos(2.0*dect)*cos(rat) + mMatr[5]*cos(2.0*dect)*sin(rat) - 0.5*mMatr[0]*sin(2.0*dect)*pow(cos(rat), 2.0) - 0.5*mMatr[4]*sin(2.0*dect)*pow(sin(rat), 2.0) + 0.5*mMatr[8]*sin(2.0*dect) + mas2rad(ooRec.rmsOneDec)*z1;

                if(isImpr)
                {
                    ocRa -= ocRa0;
                    ocDec -= ocDec0;
                }


                //ratC = ocRa/cos(dect)+rat;
                //dectC = ocDec+dect;

                //resStm << QString("%1|%2|%3|%4|%5|%6|%7").arg(rad2grad(rat)).arg(dect).arg(ratC).arg(dectC).arg(rad2mas(ocRa)).arg(rad2mas(ocDec));
                //numVect << tStr.section(colSep, cn, cn).toDouble();
                ratC = rat - ocRa/cos(dect);
                dectC = dect - ocDec;
                resStm << QString("%1|%2|%3|%4|%5|%6|%7\n").arg(rat, 15, 'e', 12).arg(dect, 15, 'e', 12).arg(ratC, 15, 'e', 12).arg(dectC, 15, 'e', 12).arg(rad2mas(ocRa), 10, 'f', 2).arg(rad2mas(ocDec), 10, 'f', 2).arg(pMin);
                obsNum++;
            }
            resFile.close();
            qDebug() << QString("obsNum: %1\t%2 skiped\n").arg(obsNum).arg(skipNum);
            return 0;
        }
        break;
        case 3:
        {
            iFile.setFileName(argv[1]);
            iFile.open(QFile::ReadOnly);
            iStm.setDevice(&iFile);

            QFile resFile(QString("%1_hps.txt").arg(QFileInfo(argv[1]).absoluteFilePath().section(".", -2, -2)));
            resFile.open(QFile::WriteOnly | QFile::Truncate);
            QTextStream resStm(&resFile);

            pointNum = nsMax*nsMax*12;
            ra = new double[pointNum];
            dec = new double[pointNum];
            raAve = new double[pointNum];
            deAve = new double[pointNum];
            dRa = new double[pointNum];
            dDec = new double[pointNum];
            numI = new int[pointNum];

            for(i=0; i<pointNum; i++)
            {
                pix2ang_ring( nsMax, i, &dect, &rat);
                dect = dect-M_PI/2.0;
                ra[i] = rat;
                dec[i] = dect;
                raAve[i] = 0.0;
                deAve[i] = 0.0;
                dRa[i] = 0.0;
                dDec[i] = 0.0;
                numI[i] = 0;
            }

            while(!iStm.atEnd())
            {
                tStr = iStm.readLine();
                rat = tStr.section(colSep, cx, cx).toDouble();
                dect = tStr.section(colSep, cy, cy).toDouble();
                ratC = tStr.section(colSep, cxc, cxc).toDouble();
                dectC = tStr.section(colSep, cyc, cyc).toDouble();

                if(isDegree)
                {
                    rat = grad2rad(rat);
                    dect = grad2rad(dect);
                    ratC = grad2rad(ratC);
                    dectC = grad2rad(dectC);
                }
                //numVect << tStr.section(colSep, cn, cn).toDouble();

                if(isEcl==1)
                {
                    //ratC = rat - mas2rad(dRa)/cos(dect);
                    //dectC = dect - mas2rad(dDe);

                    lam = atan2(cos(dect)*sin(rat)*cos(-EKV)-sin(dect)*sin(-EKV), cos(dect)*cos(rat));

                    beta = asin(cos(dect)*sin(rat)*sin(-EKV)+sin(dect)*cos(-EKV));

                    if(beta>PI/2.0) {lam += PI; beta = PI/2.0 - beta;}
                    if(beta<-PI/2.0) {lam += PI; beta = PI/2.0 + beta;}

                    if((lam>2.0*PI)) lam -=2.0*PI;
                    if((lam<0.0)) lam +=2.0*PI;
                    rat = lam;
                    dect = beta;

                    lam = atan2(cos(dectC)*sin(ratC)*cos(-EKV)-sin(dectC)*sin(-EKV), cos(dectC)*cos(ratC));
                    beta = asin(cos(dectC)*sin(ratC)*sin(-EKV)+sin(dectC)*cos(-EKV));

                    if(beta>PI/2.0) {lam += PI; beta = PI/2.0 - beta;}
                    if(beta<-PI/2.0) {lam += PI; beta = PI/2.0 + beta;}

                    if((lam>2.0*PI)) lam -=2.0*PI;
                    if((lam<0.0)) lam +=2.0*PI;
                    ratC = lam;
                    dectC = beta;
                }
                if(isZonal&&(dect<dMin||dect>dMax)) continue;
                if(isZonal)
                {
                    dect = asin((2.0*sin(dect)/(s2-s1))-(s2+s1)/(s2-s1));
                    dectC = asin((2.0*sin(dectC)/(s2-s1))-(s2+s1)/(s2-s1));
                    //rat = asin((2.0*sin(rat)/(rs2-rs1))-(rs2+rs1)/(rs2-rs1));
                }

                //if(dect<dMin||dect>dMax) continue;
                //if((magn<mag0||magn>mag1)&&isMagn) continue;
                ang2pix_ring(nsMax, dect+M_PI/2.0, rat, &ipix);
                if(ipix>pointNum||ipix<0)
                {
                    qDebug() << QString("WARN ipix: %1\n").arg(ipix);
                    continue;
                }
                raAve[ipix] += rat;
                deAve[ipix] += dect;
                dRa[ipix] += (rat - ratC)*cos(dect);
                dDec[ipix] += dect - dectC;

                numI[ipix]++;
            }

            for(i=0; i<pointNum; i++)
            {
                if(numI[i]<pMin)continue;

                raAve[i] /= numI[i];
                deAve[i] /= numI[i];
                dRa[i] /= numI[i];
                dDec[i] /= numI[i];

                rat = ra[i] - dRa[i]/cos(dec[i]);
                dect = dec[i] - dDec[i];
                resStm << QString("%1|%2|%3|%4|%5|%6|%7\n").arg(ra[i], 15, 'e', 12).arg(dec[i], 15, 'e', 12).arg(rat, 15, 'e', 12).arg(dect, 15, 'e', 12).arg(rad2mas(dRa[i]), 15, 'e', 12).arg(rad2mas(dDec[i]), 15, 'e', 12).arg(numI[i]);
            }
            resFile.close();
            return 0;
        }

        break;
    }

//modeling

    double orT0, orT1;
    double *A, *r, *W, *Z;
    double *Ara, *rRa, *Wra, *Zra;
    double *Ade, *rDe, *Wde, *Zde;

    double Dx[3][3];
    double Dra[3][3];
    double Dde[2][2];

    A = new double[3*pointNum*2];
    r = new double[pointNum*2];
    W = new double[pointNum*2];
    Z = new double[3];

    Ara = new double[3*pointNum];
    rRa = new double[pointNum];
    Wra = new double[pointNum];
    Zra = new double[3];

    Ade = new double[2*pointNum];
    rDe = new double[pointNum];
    Wde = new double[pointNum];
    Zde = new double[2];

    int exclind[pointNum];
    double uwe, uweRa, uweDe;
    int rn;

    Z[0] = Z[1] = Z[2] = 0.0;

    QFile oFile("res.dat");
    oFile.open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream oData(&oFile);

    QFile zFile("z.dat");
    zFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream zStm(&zFile);

    raO = new double[pointNum];
    decO = new double[pointNum];
    dRa = new double[pointNum];
    dDec = new double[pointNum];

    qDebug() << QString("pointNum: %1\n").arg(pointNum);

    for(i=0; i<pointNum; i++)
    {
        obj = new double[2];
        objR = new double[2];

        obj[0] = ra[i];
        obj[1] = dec[i];

        objList << obj;

        Ara[i*3] = A[i*3] = -(sin(dec[i])*cos(ra[i]));
        Ara[i*3+1] = A[i*3+1] = -(sin(dec[i])*sin(ra[i]));
        Ara[i*3+2] = A[i*3+2] = cos(dec[i]);

        Ade[i*2] = A[pointNum+i*3] = sin(ra[i]);
        Ade[i*2+1] = A[pointNum+i*3+1] = -cos(ra[i]);
        A[pointNum+i*3+2] = 0;

        Wra[i] = Wde[i] = W[i] = 1.0;
        W[pointNum+i] = 1.0;
    //          Wc[i] = 1.0;

/////////////   dispersion  ////////////////
        do
        {
    //           srand(time(NULL));
            x = 2.0*rand()/(1.0*RAND_MAX) - 1.0;
            y = 2.0*rand()/(1.0*RAND_MAX) - 1.0;
            s = x*x + y*y;
        }while((s==0)||(s>1));
        lns = sqrt(-2.0*log(s)/s);
        z0 = x*lns;
        z1 = y*lns;

        zStm << QString("%1|%2|%3|%4|%5|%6\n").arg(x).arg(y).arg(s).arg(lns).arg(z0).arg(z1);

////////////////////////////////////////////

        objR[0] = A[i*3]*Eps[0]+A[i*3+1]*Eps[1]+A[i*3+2]*Eps[2] - mMatr[2]*sin(dec[i])*sin(ra[i]) + mMatr[5]*sin(dec[i])*cos(ra[i]) + mMatr[1]*cos(dec[i])*cos(2.0*ra[i]) - 0.5*mMatr[0]*cos(dec[i])*sin(2.0*ra[i]) + 0.5*mMatr[4]*cos(dec[i])*sin(3.0*ra[i]) + disp*z0;
        objR[1] = A[pointNum+i*3]*Eps[0]+A[pointNum+i*3+1]*Eps[1]+A[pointNum+i*3+2]*Eps[2] - 0.5*mMatr[1]*sin(2.0*dec[i])*sin(2.0*ra[i]) + mMatr[2]*cos(2.0*dec[i])*cos(ra[i]) + mMatr[5]*cos(2.0*dec[i])*sin(ra[i]) - 0.5*mMatr[0]*sin(2.0*dec[i])*pow(cos(ra[i]), 2.0) - 0.5*mMatr[4]*sin(2.0*dec[i])*pow(sin(ra[i]), 2.0) + 0.5*mMatr[8]*sin(2.0*dec[i]) + disp*z1;

///////////////////////////////////
        if(isUBmod)
        {
            do
            {
        //           srand(time(NULL));
                x = 2.0*rand()/(1.0*RAND_MAX) - 1.0;
                y = 2.0*rand()/(1.0*RAND_MAX) - 1.0;
                s = x*x + y*y;
            }while((s==0)||(s>1));
            lns = sqrt(-2.0*log(s)/s);
            z0 = x*lns;
            z1 = y*lns;

    //////////////////////
            disp1 = kPar*(rand()/(1.0*RAND_MAX)) + bPar;
    //////////////////////
            objR[1] += mas2rad(disp1*z1);
        }

///////////////////////////////////

        rRa[i] = r[i] = objR[0];
        rDe[i] = r[pointNum+i] = objR[1];

        objR[0] = r[i]/cos(dec[i])+ra[i];
        objR[1] = r[pointNum+i]+dec[i];

        objListRot << objR;
        raO[i] = objR[0];
        decO[i] = objR[1];
        dRa[i] = (raO[i]-ra[i])*cos(decO[i]);
        dDec[i] = decO[i]-dec[i];

        oData << QString("%1|%2|%3|%4|%5|%6|%7\n").arg(raO[i], 15, 'e', 12).arg(decO[i], 15, 'e', 12).arg(ra[i], 15, 'e', 12).arg(dec[i], 15, 'e', 12).arg(rad2mas(dRa[i]), 15, 'e', 12).arg(rad2mas(dDec[i]), 15, 'e', 12).arg(numVect[i]);

        //oData << QString("%1 %2 %3 %4 %5 %6\n").arg(rad2grad(obj[0]), 12, 'e', 9).arg(rad2grad(obj[1]), 12, 'e', 9).arg(rad2grad(objR[0]), 12, 'e', 9).arg(rad2grad(objR[1]), 12, 'e', 9).arg(rad2grad((objR[0]-obj[0])*cos(dec[i])), 12, 'e', 9).arg(rad2grad(objR[1]-obj[1]), 12, 'e', 9);
        //oData << QString("%1 %2 %3 %4 %5 %6\n").arg(obj[0], 12, 'e', 9).arg(rad2grad(obj[1]), 12, 'e', 9).arg(rad2grad(objR[0]), 12, 'e', 9).arg(rad2grad(objR[1]), 12, 'e', 9).arg(rad2grad((objR[0]-obj[0])*cos(dec[i])), 12, 'e', 9).arg(rad2grad(objR[1]-obj[1]), 12, 'e', 9);
    }



////////////////////////////////////////////////////////////////////
    qDebug() << QString("Eps: %1\t%2\t%3\n").arg(rad2mas(Eps[0]),12, 'f', 8).arg(rad2mas(Eps[1]),12, 'f', 8).arg(rad2mas(Eps[2]),12, 'f', 8);

    double *EpsC = new double[3];
    double *sgEpsC = new double[3];

    lsmCount(raO, decO, dRa, dDec, pointNum, EpsC, sgEpsC);

    qDebug() << QString("EpsC: %1\t%2\t%3\n").arg(rad2mas(EpsC[0]),12, 'f', 8).arg(rad2mas(EpsC[1]),12, 'f', 8).arg(rad2mas(EpsC[2]),12, 'f', 8);
    qDebug() << QString("sgEpsC: %1\t%2\t%3\n").arg(rad2mas(sgEpsC[0]),12, 'f', 8).arg(rad2mas(sgEpsC[1]),12, 'f', 8).arg(rad2mas(sgEpsC[2]),12, 'f', 8);

/*
    lsm(3, pointNum*2, Z, A, r, uwe, &Dx[0][0], W);

//    slsm(3, pointNum, Z, A, r, W);
//    iLSM(3, pointNum, 0.6, &exclind[0], Z, A, r, uwe, &Dx[0][0], -1, rn, W);
    qDebug() << QString("Z: %1 +- %2\t%3 +- %4\t%5 +- %6\n").arg(rad2mas(Z[0]),12, 'f', 8).arg(rad2mas(sqrt(Dx[0][0])),12, 'f', 8).arg(rad2mas(Z[1]),12, 'f', 8).arg(rad2mas(sqrt(Dx[1][1])),12, 'f', 8).arg(rad2mas(Z[2]),12, 'f', 8).arg(rad2mas(sqrt(Dx[2][2])),12, 'f', 8);
    qDebug() << QString("uwe: %1\n").arg(rad2mas(sqrt(uwe)),12, 'f', 8);
//    slsm(2, pointNum, Zd, Ad, rd, Wd);
  //  qDebug() << "Zd: " << Zd[0] << "\t" << Zd[1] << "\n";
//    qDebug() << QString("Zd: %1\t%2\n").arg(Zd[0],12, 'f', 8).arg(Zd[1],12, 'f', 8);

    lsm(3, pointNum, Zra, Ara, rRa, uweRa, &Dra[0][0], Wra);
    qDebug() << QString("Zra: %1 +- %2\t%3 +- %4\t%5 +- %6\n").arg(rad2mas(Zra[0]),12, 'f', 8).arg(rad2mas(sqrt(Dra[0][0])),12, 'f', 8).arg(rad2mas(Zra[1]),12, 'f', 8).arg(rad2mas(sqrt(Dra[1][1])),12, 'f', 8).arg(rad2mas(Zra[2]),12, 'f', 8).arg(rad2mas(sqrt(Dra[2][2])),12, 'f', 8);
    qDebug() << QString("uweRa: %1\n").arg(rad2mas(sqrt(uweRa)),12, 'f', 8);

    lsm(2, pointNum, Zde, Ade, rDe, uweDe, &Dde[0][0], Wde);
    qDebug() << QString("Zde: %1 +- %2\t%3 +- %4\n").arg(rad2mas(Zde[0]),12, 'f', 8).arg(rad2mas(sqrt(Dde[0][0])),12, 'f', 8).arg(rad2mas(Zde[1]),12, 'f', 8).arg(rad2mas(sqrt(Dde[1][1])),12, 'f', 8);
    qDebug() << QString("uweDe: %1\n").arg(rad2mas(sqrt(uweDe)),12, 'f', 8);


//    slsm(3, pointNum, Zc, Ac, rc, Wc);
//    iLSM(3, pointNum, 0.6, &exclind[0], Z, A, r, uwe, &Dx[0][0], -1, rn, W);
//    qDebug() << "Zc: " << Zc[0] << "\t" << Zc[1] << "\t" << Zc[2] << "\n";
//    qDebug() << QString("Zc: %1\t%2\t%3\n").arg(Zc[0],12, 'f', 8).arg(Zc[1],12, 'f', 8).arg(Zc[2],12, 'f', 8);

/*Plot
    QImage *baseImg;
    QVector<QRgb> colormap;
    QPen pointPen;
    QPen pointRotPen;
    QPen linePen;

    QBrush baseBrush(Qt::white, Qt::SolidPattern);
    QBrush pointBrush(Qt::darkGreen, Qt::SolidPattern);

   pointPen.setColor(Qt::darkGreen);
   pointPen.setWidth(2);
   pointRotPen.setColor(Qt::darkBlue);
   pointRotPen.setWidth(1);
   linePen.setColor(Qt::red);
   linePen.setWidth(1);

   QPoint pPos, pPosRot;
   int maxIW = sett->value("plot/maxIW").toInt();
   int maxIH = sett->value("plot/maxIH").toInt();

   QString resFileName = QString("res.bmp");

   baseImg = new QImage(maxIW, maxIH, QImage::Format_RGB444);
   QPainter painter(baseImg);

    if(!painter.isActive())
    {
            qDebug() << "\npainter is\'t active\n";
            return 1;
    }

    painter.fillRect(0, 0, maxIW, maxIH, baseBrush);
//    painter.setPen(pointPen);
    for(i=0; i<pointNum; i++)
    {
        /*
        pPos.setX(((objList[i][0]-raMin)/fabs(raMax-raMin))*maxIW);
        pPos.setY(((objList[i][1]-deMin)/fabs(deMax-deMin))*maxIH);
        pPosRot.setX(((objListRot[i][0]-raMin)/fabs(raMax-raMin))*maxIW);
        pPosRot.setY(((objListRot[i][1]-deMin)/fabs(deMax-deMin))*maxIH);
        /
        pPos.setX(((objList[i][0])/fabs(rad_to_mas(2.0*PI)))*maxIW);
        pPos.setY(((objList[i][1]-rad_to_mas(-PI/2.0))/fabs(rad_to_mas(PI)))*maxIH);
        pPosRot.setX(((objListRot[i][0])/fabs(rad_to_mas(2.0*PI)))*maxIW);
        pPosRot.setY(((objListRot[i][1]-rad_to_mas(-PI/2.0))/fabs(rad_to_mas(PI)))*maxIH);

        painter.setPen(linePen);
        painter.drawLine(pPos, pPosRot);
        painter.setPen(pointPen);

        painter.drawPoint(pPos);
        painter.setPen(pointRotPen);
        painter.drawPoint(pPosRot);
    }

    painter.end();
    baseImg->save(resFileName, "BMP");

*/

    qDebug() << "\nend\n";
    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;
    oFile.close();
    zFile.close();

    qInstallMsgHandler(0);
    return 0;

 //   return a.exec();
}

int randomSphereHpix(QVector <double> &ra, QVector <double> &de, int nsMax, double raMin, double raMax, double deMin, double deMax, int csys)
{
    double h, phi, x, y, z, rat, dect, lam, beta;
    long ipix, ipixMax;
    ipixMax = nsMax*nsMax*12;

    double s1 = sin(deMin);
    double s2 = sin(deMax);

    for(ipix=0; ipix<ipixMax; ipix++)
    {
        pix2ang_ring( nsMax, ipix, &dect, &rat);
        dect = PI/2.0 - dect;

        dect = asin(0.5*sin(dect)*(s2-s1) + 0.5*(s2+s1));

        if((rat<=raMax)&&(rat>=raMin)&&(dect>=deMin)&&(dect<=deMax))
        {
            if(csys==1)
            {
                lam = atan2(cos(dect)*sin(rat)*cos(EKV)-sin(dect)*sin(EKV), cos(dect)*cos(rat));
//                beta = atan2((cos(mas_to_rad(dec))*sin(mas_to_rad(ra))*sin(epsi)+sin(mas_to_rad(dec))*cos(epsi))*cos(lam), cos(mas_to_rad(dec))*cos(mas_to_rad(ra)));
//                beta = atan2((cos(mas_to_rad(dec))*sin(mas_to_rad(ra))*sin(epsi)+sin(mas_to_rad(dec))*cos(epsi))*sin(lam), (cos(mas_to_rad(dec))*sin(mas_to_rad(ra))*cos(epsi)-sin(mas_to_rad(dec))*sin(epsi)));
                beta = asin(cos(dect)*sin(rat)*sin(EKV)+sin(dect)*cos(EKV));

                if(beta>PI/2.0) {lam += PI; beta = PI/2.0 - beta;}
                if(beta<-PI/2.0) {lam += PI; beta = PI/2.0 + beta;}

                if((lam>2.0*PI)) lam -=2.0*PI;
                if((lam<0.0)) lam +=2.0*PI;
                rat = lam;
                dect = beta;

            }

            ra << rat;
            de << dect;
        }
    }
}

int randomSphere(double *ra, double *de, int num, double raMin, double raMax, double deMin, double deMax, int csys) //[rad]
{
    int k = 0;
    double h, phi, x, y, z, rat, dect, lam, beta;
    long ipix, nsMax, ipixMax, i;





    srand(time(NULL));

    do
    {


            h = 2.0*(rand())/(1.0*RAND_MAX)-1.0;
            phi = 2.0*PI*(rand()/(1.0*RAND_MAX));
            x= cos(phi) * sqrt(1-h*h); //вращаем вокруг по оси z на угол phi
            y= -sin(phi) * sqrt(1-h*h); //вращаем вокруг по оси z на угол phi
            z= h;
            rat = phi;//atan2(y, x);
            dect = atan2(z, sqrt(y*y+x*x));






        if((rat<=raMax)&&(rat>=raMin)&&(dect>=deMin)&&(dect<=deMax))
        {
            if(csys==1)
            {
                lam = atan2(cos(dect)*sin(rat)*cos(EKV)-sin(dect)*sin(EKV), cos(dect)*cos(rat));
//                beta = atan2((cos(mas_to_rad(dec))*sin(mas_to_rad(ra))*sin(epsi)+sin(mas_to_rad(dec))*cos(epsi))*cos(lam), cos(mas_to_rad(dec))*cos(mas_to_rad(ra)));
//                beta = atan2((cos(mas_to_rad(dec))*sin(mas_to_rad(ra))*sin(epsi)+sin(mas_to_rad(dec))*cos(epsi))*sin(lam), (cos(mas_to_rad(dec))*sin(mas_to_rad(ra))*cos(epsi)-sin(mas_to_rad(dec))*sin(epsi)));
                beta = asin(cos(dect)*sin(rat)*sin(EKV)+sin(dect)*cos(EKV));

                if(beta>PI/2.0) {lam += PI; beta = PI/2.0 - beta;}
                if(beta<-PI/2.0) {lam += PI; beta = PI/2.0 + beta;}

                if((lam>2.0*PI)) lam -=2.0*PI;
                if((lam<0.0)) lam +=2.0*PI;
                rat = lam;
                dect = beta;

            }

            ra[k] = rat;
            de[k] = dect;

            k++;
        }



    }while(k<num);

    return 0;
}




void get2Ddisp(double &z0, double &z1)
{
    double x, y, s, lns;
    //srand(time(NULL));
/////////////   dispersion  ////////////////
        do
        {
    //           srand(time(NULL));
            x = 2.0*rand()/(1.0*RAND_MAX) - 1.0;
            y = 2.0*rand()/(1.0*RAND_MAX) - 1.0;
            s = x*x + y*y;
        }while((s==0)||(s>1));
        lns = sqrt(-2.0*log(s)/s);
        z0 = x*lns;
        z1 = y*lns;

 //       zStm << QString("%1|%2|%3|%4|%5|%6\n").arg(x).arg(y).arg(s).arg(lns).arg(z0).arg(z1);

////////////////////////////////////////////
}
