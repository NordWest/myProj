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

int randomSphere(double *ra, double *de, int num, double raMin, double raMax, double deMin, double deMax, int csys = 0);
int randomSphereHpix(QVector <double> &ra, QVector <double> &de, int nsMax, double raMin, double raMax, double deMin, double deMax, int csys);

int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);

    int i;
    QFile oFile("res.dat");
    oFile.open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream oData(&oFile);

    double raMin, raMax, deMin, deMax;
    QSettings *sett = new QSettings("sph.ini", QSettings::IniFormat);

    raMin = sett->value("general/raMin", 0).toDouble()*15.0;   //degree
    raMax = sett->value("general/raMax", 24).toDouble()*15.0;   //
    deMin = sett->value("general/deMin", -90).toDouble();        //
    deMax = sett->value("general/deMax", 90).toDouble();        //
    int pointNum = sett->value("general/pointNum", 0).toInt();
    int nsMax = sett->value("general/nsMax", 32).toInt();

    int rtype = sett->value("general/rtype", 0).toInt();
    //0 - randomSphere
    //1 - randomSphereHpix
    //2 - real points - do mod
    //3 - real points - not to mod, just summ

    QString inpFileName = sett->value("general/inpFileName", "inp.txt").toString();

    QString colSep = sett->value("general/colSep", "|").toString();
    int cx = sett->value("general/cx", 0).toInt();
    int cy = sett->value("general/cy", 1).toInt();
    int cxc = sett->value("general/cxcalc", 2).toInt();
    int cyc = sett->value("general/cycalc", 3).toInt();
    int cn = sett->value("general/cn", 4).toInt();

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

    int isUBmod = sett->value("ubMod/isUBmod", 0).toInt();        //
    double kPar = sett->value("ubMod/kPar", 300).toDouble();        //
    double bPar = sett->value("ubMod/bPar", 300).toDouble();
    double disp1;


//    double epsi = grad2rad(sett->value("general/eps").toDouble());
    int csys = sett->value("general/coord_sys", 0).toInt();

    double disp = mas_to_rad(sett->value("noice/disp", 0).toDouble());


    QList <double*> objList;
    QList <double*> objListRot;
    double *obj, *objR;

/*
    double *Ac = new double[3*pointNum];
    double *rc = new double[pointNum];
    double *Wc = new double[pointNum];
    double *Zc = new double[3];

    Zc[0] = Zc[1] = Zc[2] = 0.0;
*/
    double x, y, z, z0, z1, lns, s;
    double *ra, *dec;
    double *raC, *decC;
    double lam, beta;
    double h, phi;
    int pN = 0;

    QVector <double> raVect;
    QVector <double> deVect;
    QVector <double> raVectC;
    QVector <double> deVectC;
    QVector <int> numVect;

    QFile iFile;
    QTextStream iStm;
    QString tStr;


//////////////////////////////////////////////////////////////////
    switch(rtype)
    {
        case 0:
        {
            ra = new double[pointNum];
            dec = new double[pointNum];
            randomSphere(ra, dec, pointNum, grad2rad(raMin), grad2rad(raMax), grad2rad(deMin), grad2rad(deMax), csys);
        }
        break;
        case 1:
        {
            randomSphereHpix(raVect, deVect, nsMax, grad2rad(raMin), grad2rad(raMax), grad2rad(deMin), grad2rad(deMax), csys);
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
            iFile.setFileName(inpFileName);
            iFile.open(QFile::ReadOnly);
            iStm.setDevice(&iFile);

            while(!iStm.atEnd())
            {
                tStr = iStm.readLine();
                raVect << tStr.section(colSep, cx, cx).toDouble();
                deVect << tStr.section(colSep, cy, cy).toDouble();
                //numVect << tStr.section(colSep, cn, cn).toDouble();
            }
            pointNum = raVect.size();
            ra = new double[pointNum];
            dec = new double[pointNum];

            for(i=0; i<pointNum; i++)
            {
                ra[i] = raVect[i];
                dec[i] = deVect[i];
            }
        }
        break;
        case 3:
        {
            iFile.setFileName(inpFileName);
            iFile.open(QFile::ReadOnly);
            iStm.setDevice(&iFile);

            while(!iStm.atEnd())
            {
                tStr = iStm.readLine();
                raVect << tStr.section(colSep, cx, cx).toDouble();
                deVect << tStr.section(colSep, cy, cy).toDouble();
                raVectC << tStr.section(colSep, cxc, cxc).toDouble();
                deVectC << tStr.section(colSep, cyc, cyc).toDouble();
                //numVect << tStr.section(colSep, cn, cn).toDouble();
            }
            pointNum = raVect.size();
            ra = new double[pointNum];
            dec = new double[pointNum];
            raC = new double[pointNum];
            decC = new double[pointNum];

            for(i=0; i<pointNum; i++)
            {
                ra[i] = raVect[i];
                dec[i] = deVect[i];
                raC[i] = raVectC[i];
                decC[i] = deVectC[i];
            }
        }
        break;
    }



    double orT0, orT1;

    double *A = new double[3*pointNum*2];
    double *r = new double[pointNum*2];
    double *W = new double[pointNum*2];
    double *Z = new double[3];

    double *Ara = new double[3*pointNum];
    double *rRa = new double[pointNum];
    double *Wra = new double[pointNum];
    double *Zra = new double[3];

    double *Ade = new double[2*pointNum];
    double *rDe = new double[pointNum];
    double *Wde = new double[pointNum];
    double *Zde = new double[2];

    double Dx[3][3];
    double Dra[3][3];
    double Dde[2][2];
    int exclind[pointNum];
    double uwe, uweRa, uweDe;
    int rn;

    Z[0] = Z[1] = Z[2] = 0.0;
/*
    double *Ad = new double[2*pointNum];
    double *rd = new double[pointNum];
    double *Wd = new double[pointNum];
    double *Zd = new double[2];

    Zd[0] = Zd[1] = 0.0;
*/
    QFile zFile("z.dat");
    zFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream zStm(&zFile);

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
    //         Ad[i*3+2] = 0.0;
    /*         Ac[i*3] = A[i*3] + Ad[i*2];
        Ac[i*3+1] = A[i*3+1] + Ad[i*2+1];
        Ac[i*3+2] = A[i*3+2];
    */
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
    //            qDebug() << QString("z0= %1\tz1= %2\n").arg(z0).arg(z1);

    //            z1 = z0 = 0;


        //objR[0] = A[i*3]*Eps[0]+A[i*3+1]*Eps[1]+A[i*3+2]*Eps[2] + disp*z0;
        //objR[1] = A[pointNum+i*3]*Eps[0]+A[pointNum+i*3+1]*Eps[1]+A[pointNum+i*3+2]*Eps[2] + disp*z1;

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
            //if(oPos.set_obs_parpam(GEOCENTR_NUM, CENTER, SK, obsCode.toAscii().data())) qDebug() << QString("warn obsParam, %1\n").arg(obsCode);
            //oPos.det_observ(mjd2jd(mjd));
            //detAhnumGC(&Az, &hVal, oPos.obs->stime, oPos.obs->record->Cos, oPos.obs->record->Sin, rat, dect);
            //zet = PI/2.0 - hVal;
            disp1 = kPar*(rand()/(1.0*RAND_MAX)) + bPar;
            //if(hVal<0.0) qDebug() << QString("obsCode:%1\thVal: %2\tzet: %3\n").arg(obsCode).arg(rad2grad(hVal)).arg(rad2grad(zet));

    //////////////////////

            //objR[0] += disp1*z0;
            objR[1] += mas2rad(disp1*z1);
        }

///////////////////////////////////


        //orT0 = r[i]/cos(dec[i]) + ra[i];

        rRa[i] = r[i] = objR[0];
        rDe[i] = r[pointNum+i] = objR[1];

        objR[0] = r[i]/cos(dec[i])+ra[i];
        objR[1] = r[pointNum+i]+dec[i];

    //        objR[0] = obj[0]+A[i]*Eps[0]+A[i+1]*Eps[1]+A[i+2]*Eps[2];
    //       objR[0] = obj[0]+rad_to_mas(-sin(mas_to_rad(obj[1]))*cos(mas_to_rad(obj[0]))*mas_to_rad(Eps[0]) - sin(mas_to_rad(obj[1]))*sin(mas_to_rad(obj[0]))*mas_to_rad(Eps[1]) + cos(mas_to_rad(obj[1]))*mas_to_rad(Eps[2]))/cos(mas_to_rad(obj[1]));
    //        r[i] = objR[0] - obj[0];
    //        rc[i] = r[i] + rd[i];
    //        qDebug() << "r[i]= " << r[i] << "\n";

        objListRot << objR;
        oData << QString("%1|%2|%3|%4|%5|%6|%7\n").arg(obj[0], 12, 'e', 9).arg(obj[1], 12, 'e', 9).arg(objR[0], 12, 'e', 9).arg(objR[1], 12, 'e', 9).arg((objR[0]-obj[0])*cos(dec[i]), 12, 'e', 9).arg(objR[1]-obj[1], 12, 'e', 9).arg(numVect[i]);
        //oData << QString("%1 %2 %3 %4 %5 %6\n").arg(rad2grad(obj[0]), 12, 'e', 9).arg(rad2grad(obj[1]), 12, 'e', 9).arg(rad2grad(objR[0]), 12, 'e', 9).arg(rad2grad(objR[1]), 12, 'e', 9).arg(rad2grad((objR[0]-obj[0])*cos(dec[i])), 12, 'e', 9).arg(rad2grad(objR[1]-obj[1]), 12, 'e', 9);
        //oData << QString("%1 %2 %3 %4 %5 %6\n").arg(obj[0], 12, 'e', 9).arg(rad2grad(obj[1]), 12, 'e', 9).arg(rad2grad(objR[0]), 12, 'e', 9).arg(rad2grad(objR[1]), 12, 'e', 9).arg(rad2grad((objR[0]-obj[0])*cos(dec[i])), 12, 'e', 9).arg(rad2grad(objR[1]-obj[1]), 12, 'e', 9);
    }


////////////////////////////////////////////////////////////////////
    qDebug() << QString("Eps: %1\t%2\t%3\n").arg(rad2mas(Eps[0]),12, 'f', 8).arg(rad2mas(Eps[1]),12, 'f', 8).arg(rad2mas(Eps[2]),12, 'f', 8);

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
