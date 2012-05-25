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

#include "./../libs/astro.h"
#include "./../libs/comfunc.h"
#include "./../libs/mb.h"

#define PI 3.141592653589

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

int randomSphere(double *ra, double *de, int num, double raMin, double raMax, double deMin, double deMaxm, int csys = 0);

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

    double *Eps = new double[3];
    Eps[0] = sett->value("rotation/ex", 0).toDouble();
    Eps[1] = sett->value("rotation/ey", 0).toDouble();
    Eps[2] = sett->value("rotation/ez", 0).toDouble();

//    double epsi = grad2rad(sett->value("general/eps").toDouble());
    int csys = sett->value("general/coord_sys", 0).toInt();

    double disp = sett->value("noice/disp", 0).toDouble();


    QList <double*> objList;
    QList <double*> objListRot;
    double *obj, *objR;
    double *A = new double[3*pointNum];
    double *r = new double[pointNum];
    double *W = new double[pointNum];
    double *Z = new double[3];

    double Dx[3][3];
    int exclind[pointNum];
    double uwe;
    int rn;

    Z[0] = Z[1] = Z[2] = 0.0;

    double *Ad = new double[2*pointNum];
    double *rd = new double[pointNum];
    double *Wd = new double[pointNum];
    double *Zd = new double[2];

    Zd[0] = Zd[1] = 0.0;
/*
    double *Ac = new double[3*pointNum];
    double *rc = new double[pointNum];
    double *Wc = new double[pointNum];
    double *Zc = new double[3];

    Zc[0] = Zc[1] = Zc[2] = 0.0;
*/
    double x, y, z, z0, z1, lns, s;
    double *ra, *dec;
    double lam, beta;
    double h, phi;
    int pN = 0;

    ra = new double[pointNum];
    dec = new double[pointNum];

//srand(time(NULL));
 //   qDebug() << "PI=" << PI << "\n";

/*
do//for(i=0; i<pointNum; i++)
{
//       obj = new double[2];
//        srand(time(NULL));

/*        x = 2.0*(rand())/(1.0*RAND_MAX)-1.0;
    y = 2.0*(rand())/(1.0*RAND_MAX)-1.0;/
    h = 2.0*(rand())/(1.0*RAND_MAX)-1.0;
    phi = 2.0*PI*(rand()/(1.0*RAND_MAX));
    x= cos(phi) * sqrt(1-h*h); //вращаем вокруг по оси z на угол phi
    y= -sin(phi) * sqrt(1-h*h); //вращаем вокруг по оси z на угол phi
    z= h;
    ra = phi;//atan2(y, x);
    dec = atan2(z, sqrt(y*y+x*x));

    ra = rad_to_mas(ra);
    dec = rad_to_mas(dec);
//       qDebug() << QString("%1\t%2\n").arg(ra).arg(dec);
    if((ra<=raMax)&&(ra>=raMin)&&(dec>=deMin)&&(dec<=deMax))
//        if(1)
        {
            if(csys==1)
            {
                lam = atan2((cos(mas_to_rad(dec))*sin(mas_to_rad(ra))*cos(epsi)-sin(mas_to_rad(dec))*sin(epsi)), cos(mas_to_rad(dec))*cos(mas_to_rad(ra)));
//                beta = atan2((cos(mas_to_rad(dec))*sin(mas_to_rad(ra))*sin(epsi)+sin(mas_to_rad(dec))*cos(epsi))*cos(lam), cos(mas_to_rad(dec))*cos(mas_to_rad(ra)));
//                beta = atan2((cos(mas_to_rad(dec))*sin(mas_to_rad(ra))*sin(epsi)+sin(mas_to_rad(dec))*cos(epsi))*sin(lam), (cos(mas_to_rad(dec))*sin(mas_to_rad(ra))*cos(epsi)-sin(mas_to_rad(dec))*sin(epsi)));
                beta = asin(cos(mas_to_rad(dec))*sin(mas_to_rad(ra))*sin(epsi)+sin(mas_to_rad(dec))*cos(epsi));

                if(beta>PI/2.0) {lam += PI; beta = PI/2.0 - beta;}
                if(beta<-PI/2.0) {lam += PI; beta = PI/2.0 + beta;}

                if((lam>2.0*PI)) lam -=2.0*PI;
                if((lam<0.0)) lam +=2.0*PI;
                ra = rad_to_mas(lam);
                dec = rad_to_mas(beta);
            }



            obj = new double[2];
            obj[0] = ra;
            obj[1] = dec;

            objList << obj;
            objR = new double[2];
            A[i*3] = (sin(mas_to_rad(obj[1]))*cos(mas_to_rad(obj[0])));
            A[i*3+1] = (sin(mas_to_rad(obj[1]))*sin(mas_to_rad(obj[0])));
            A[i*3+2] = -cos(mas_to_rad(obj[1]));

            Ad[i*3] = -sin(mas_to_rad(obj[0]));
            Ad[i*3+1] =     cos(mas_to_rad(obj[0]));
   //         Ad[i*3+2] = 0.0;
   /*         Ac[i*3] = A[i*3] + Ad[i*2];
            Ac[i*3+1] = A[i*3+1] + Ad[i*2+1];
            Ac[i*3+2] = A[i*3+2];
/
            W[i] = 1.0;
            Wd[i] = 1.0;
  //          Wc[i] = 1.0;


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
//            qDebug() << QString("z0= %1\tz1= %2\n").arg(z0).arg(z1);

//            z1 = z0 = 0;

            objR[1] = obj[1]+Ad[i*3]*Eps[0]+Ad[i*3+1]*Eps[1] + disp*z1;
            rd[i] = objR[1] - obj[1];
            r[i] = A[i*3]*Eps[0]+A[i*3+1]*Eps[1]+A[i*3+2]*Eps[2] + disp*z0;
            objR[0] = r[i]/cos(mas_to_rad(obj[1])) + obj[0];
    //        objR[0] = obj[0]+A[i]*Eps[0]+A[i+1]*Eps[1]+A[i+2]*Eps[2];
     //       objR[0] = obj[0]+rad_to_mas(-sin(mas_to_rad(obj[1]))*cos(mas_to_rad(obj[0]))*mas_to_rad(Eps[0]) - sin(mas_to_rad(obj[1]))*sin(mas_to_rad(obj[0]))*mas_to_rad(Eps[1]) + cos(mas_to_rad(obj[1]))*mas_to_rad(Eps[2]))/cos(mas_to_rad(obj[1]));
    //        r[i] = objR[0] - obj[0];
    //        rc[i] = r[i] + rd[i];
    //        qDebug() << "r[i]= " << r[i] << "\n";
            objListRot << objR;
            oData << QString("%1 %2 %3 %4 %5 %6\n").arg(obj[0], 12, 'f', 9).arg(obj[1], 12, 'f', 9).arg(objR[0], 12, 'f', 9).arg(objR[1], 12, 'f', 9).arg(obj[0]-objR[0], 12, 'f', 9).arg(obj[1]-objR[1], 12, 'f', 9);
            i++;
      }

    }while(i<pointNum);*/

//////////////////////////////////////////////////////////////////
    randomSphere(ra, dec, pointNum, grad2rad(raMin), grad2rad(raMax), grad2rad(deMin), grad2rad(deMax), csys);

    double orT0, orT1;

    for(i=0; i<pointNum; i++)
    {

        obj = new double[2];
        obj[0] = rad_to_mas(ra[i]);
        obj[1] = rad_to_mas(dec[i]);

        objList << obj;

        objR = new double[2];
        A[i*3] = (sin(dec[i])*cos(ra[i]));
        A[i*3+1] = (sin(dec[i])*sin(ra[i]));
        A[i*3+2] = -cos(dec[i]);

        Ad[i*3] = -sin(ra[i]);
        Ad[i*3+1] =     cos(ra[i]);
    //         Ad[i*3+2] = 0.0;
    /*         Ac[i*3] = A[i*3] + Ad[i*2];
        Ac[i*3+1] = A[i*3+1] + Ad[i*2+1];
        Ac[i*3+2] = A[i*3+2];
    */
        W[i] = 1.0;
        Wd[i] = 1.0;
    //          Wc[i] = 1.0;

/////////////   distersion  ////////////////
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

////////////////////////////////////////////
    //            qDebug() << QString("z0= %1\tz1= %2\n").arg(z0).arg(z1);

    //            z1 = z0 = 0;

        orT1 = dec[i]+Ad[i*3]*Eps[0]+Ad[i*3+1]*Eps[1] + disp*z1;
        rd[i] = orT1 - dec[i];
        r[i] = A[i*3]*Eps[0]+A[i*3+1]*Eps[1]+A[i*3+2]*Eps[2] + disp*z0;
        orT0 = r[i]/cos(dec[i]) + ra[i];

        objR[0] = rad2mas(orT0);
        objR[1] = rad2mas(orT1);
    //        objR[0] = obj[0]+A[i]*Eps[0]+A[i+1]*Eps[1]+A[i+2]*Eps[2];
    //       objR[0] = obj[0]+rad_to_mas(-sin(mas_to_rad(obj[1]))*cos(mas_to_rad(obj[0]))*mas_to_rad(Eps[0]) - sin(mas_to_rad(obj[1]))*sin(mas_to_rad(obj[0]))*mas_to_rad(Eps[1]) + cos(mas_to_rad(obj[1]))*mas_to_rad(Eps[2]))/cos(mas_to_rad(obj[1]));
    //        r[i] = objR[0] - obj[0];
    //        rc[i] = r[i] + rd[i];
    //        qDebug() << "r[i]= " << r[i] << "\n";
        objListRot << objR;
        oData << QString("%1 %2 %3 %4 %5 %6\n").arg(obj[0], 12, 'f', 9).arg(obj[1], 12, 'f', 9).arg(objR[0], 12, 'f', 9).arg(objR[1], 12, 'f', 9).arg(obj[0]-objR[0], 12, 'f', 9).arg(obj[1]-objR[1], 12, 'f', 9);
    }


////////////////////////////////////////////////////////////////////



    slsm(3, pointNum, Z, A, r, W);
//    iLSM(3, pointNum, 0.6, &exclind[0], Z, A, r, uwe, &Dx[0][0], -1, rn, W);
    qDebug() << QString("Z: %1\t%2\t%3\n").arg(Z[0],12, 'f', 8).arg(Z[1],12, 'f', 8).arg(Z[2],12, 'f', 8);
    slsm(2, pointNum, Zd, Ad, rd, Wd);
  //  qDebug() << "Zd: " << Zd[0] << "\t" << Zd[1] << "\n";
    qDebug() << QString("Zd: %1\t%2\n").arg(Zd[0],12, 'f', 8).arg(Zd[1],12, 'f', 8);


//    slsm(3, pointNum, Zc, Ac, rc, Wc);
//    iLSM(3, pointNum, 0.6, &exclind[0], Z, A, r, uwe, &Dx[0][0], -1, rn, W);
//    qDebug() << "Zc: " << Zc[0] << "\t" << Zc[1] << "\t" << Zc[2] << "\n";
//    qDebug() << QString("Zc: %1\t%2\t%3\n").arg(Zc[0],12, 'f', 8).arg(Zc[1],12, 'f', 8).arg(Zc[2],12, 'f', 8);

//Plot
    QImage *baseImg;
    QVector<QRgb> colormap;
    QPen pointPen;
    QPen pointRotPen;
    QPen linePen;

    QBrush baseBrush(Qt::white, Qt::SolidPattern);
    QBrush pointBrush(Qt::darkGreen, Qt::SolidPattern);

   pointPen.setColor(Qt::darkGreen);
   pointPen.setWidth(1);
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
        */
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
//

    qDebug() << "\nend\n";
    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;
    oFile.close();

    qInstallMsgHandler(0);
    return 0;

 //   return a.exec();
}

int randomSphere(double *ra, double *de, int num, double raMin, double raMax, double deMin, double deMax, int csys) //[rad]
{
    int k = 0;
    double h, phi, x, y, z, rat, dect, lam, beta;

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
