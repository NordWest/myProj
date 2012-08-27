#include <QtCore/QCoreApplication>
#include <QtCore>
#include <QtGui>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDebug>
#include <QSettings>

#include "./../libs/dele.h"
//#include "./../libs/mpcs.h"
#include "./../libs/astro.h"
#include "./../libs/redStat.h"
#include "./../libs/mb.h"

dele* place;
QTextStream oData;
double ras, decs;

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

double pL(double I, double mu)  //I - фаза [rad], mu - apparent angular radius [rad].
{
    return((3.0*PI/16.0)*mu*((sin(I)*(1.0 + cos(I)))/(sin(I) + (PI-I)*cos(I))));
}
double pLS(double I, double mu)
{
    return((2.0/3.0/PI)*mu*((tan(I/2.0)*(sin(I) + (PI - I)*cos(I)))/(1.0 - sin(I/2.0)*tan(I/2.0)*log(1.0/tan(I/4.0)))));
}

void detGQ(double *G, double *Q, double ra, double dec, double tTime)
{
    double ox, oy, oz;
    double raS, decS;
    double P1, P2, P3;
    double g, q;

    if(place->detR(&ox, &oy, &oz, tTime, GEOCENTR_NUM, 0, CENTER_SUN, SK_EKVATOR))
    {
        qDebug() << "detR error\n";
    }
 //   oData << QString("%1\t%2\t%3\t%4\t%5\t").arg(tTime, 15, 'f', 6).arg(-ox, 15, 'f', 12).arg(-oy, 15, 'f', 12).arg(-oz, 15, 'f', 12).arg(sqrt(ox*ox+oy*oy+oz*oz), 15, 'f', 12);
    rdsys(&raS, &decS, -ox, -oy, -oz);
/*    raS = atan2(-oy, -ox);
    decS = atan2(-oz, sqrt(ox*ox + oy*oy));
    if(decS>PI/2.0) {raS += PI; decS = PI/2.0 - decS;}
    if(decS<-PI/2.0) {raS += PI; decS = PI/2.0 + decS;}

    if((raS>2.0*PI)) raS -=2.0*PI;
    if((raS<0.0)) raS +=2.0*PI;
*/
 //   oData << QString("%1\t%2\t").arg(mas_to_hms(rad_to_mas(raS), " ", 3)).arg(mas_to_damas(rad_to_mas(decS), " ", 3));
    ras = rad_to_mas(raS);
    decs = rad_to_mas(decS);

    P1 = sin(decS)*sin(dec) + cos(decS)*cos(dec)*cos(ra-raS);
    P2 = cos(decS)*sin(ra-raS);
    P3 = -sin(decS)*cos(dec) + cos(decS)*sin(dec)*cos(ra-raS);

    *Q = atan2(P2, P3);
//    *G = atan2(P3, P1*cos(*Q));
    *G = atan2(sqrt(P3*P3 + P2*P2), P1);

    double P1L, P2L, P3L;

    if(*Q<0.0) *Q += 2.0*PI;
    if(*Q>2.0*PI) *Q -= 2.0*PI;
/*    if(*G<0.0) *G += 2.0*PI;
    if(*G<0.0) *G += 2.0*PI;
*/
//    oData << QString("%1\t%2\t").arg(mas_to_damas(rad_to_mas(*G), ":", 3)).arg(mas_to_damas(rad_to_mas(*Q), ":", 3));

    P1L = cos(*G);
    P2L = sin(*G)*sin(*Q);
    P3L = sin(*G)*cos(*Q);

//   qDebug() << QString("\nProof: %1 ?= %2 ; %3 ?= %4 ; %5 ?= %6\n").arg(P1L).arg(P1).arg(P2L).arg(P2).arg(P3L).arg(P3);
}


int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);

    if(argc!=2) return 1;

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);
    QString fIni, fRes, tStr;
    QByteArray *tmp;
    const char *fname;
    char *dele_header;// = new char[255];
    char *dele_name;// = new char[255];



    QSettings *settings = new QSettings("phaseRed.ini", QSettings::IniFormat);
    double koef = settings->value("general/koef").toDouble();
    double diam = settings->value("general/diam").toDouble();
    double radAs = diam/2.0/AUKM;
    int model = settings->value("general/model").toInt();

    tStr = settings->value("general/dele_header").toString();
    tmp = new QByteArray(tStr.toAscii());
    dele_header = tmp->data();
//    qDebug() << "dele_header= " << dele_header << "\n";
    tStr = settings->value("general/dele_name").toString();
    tmp = new QByteArray(tStr.toAscii());
    dele_name = tmp->data();
//    qDebug() << "dele_name= " << dele_name << "\n";

    fIni = QString(argv[1]);
    fRes = QString("%1_pha_eq.txt").arg(fIni.left(fIni.lastIndexOf("_eq.txt")));

    QFile oFile(QString("%1.ph").arg(fIni.left(fIni.lastIndexOf("_eq.txt"))));
    oFile.open(QFile::WriteOnly | QFile::Truncate | QIODevice::Text);
    oData.setDevice(&oFile);

    eqFile *ocFile = new eqFile;
    tmp = new QByteArray(fIni.toAscii());//fIni.toAscii();
    fname = tmp->data();
    qDebug() << "fIni= " << fname << "\n";
    ocFile->init(fname);

    eqFile *phFile = new eqFile;
    tmp = new QByteArray(fRes.toAscii());
    fname = tmp->data();
    phFile->init(fname);
    phFile->clear();

    double mu;// = diam/AUKM;

    double pRa, pDec, Qu, Ge, I, tTime;
    int i, sz;
    double r0, r1;


    double cosDec, P, ra, dec, raOC, decOC;

    ocRec *ocl;

    qDebug() << "dele_header= " << dele_header << "\n";
    qDebug() << "dele_name= " << dele_name << "\n";

    place = new dele;
    if(place->init(dele_name))
    {
        qDebug() << "\ndele_header init error\n\n";
        exit(1);
    }
   // place->init_jpl(dele_name);

    sz = ocFile->ocList.size();
    qDebug() << "sz=" << sz << "\n";

    double *Zx = new double[1];
    double *Cx = new double[sz];
    double *Lx = new double[sz];
    double mDivX;
    int *excX = new int[sz];
    double uweX;
    double *dX = new double[1];
    double fl = -1;
    int rn;
    double *We = new double[sz];
    double koef1;

 //   do
  //  {

        Zx[0] = 0.0;

        for(i=0; i<sz; i++)
        {
    //        ocFile->ocList[i]->de;
            I = grad2rad(ocFile->ocList[i]->phase);
    //        Q = grad2rad(ocFile->ocList[i]->);
            mu = radAs/ocFile->ocList[i]->topDist;
            switch(model)
            {
                case 0:
                P = pL(I, mu);
                break;
                case 1:
                P = pLS(I, mu);
                break;
            }

            tTime = ocFile->ocList[i]->MJday + 2400000.5;
    //        place->detR(&ox, &oy, &oz, tTime, GEOCENTR_NUM, 0, CENTER_SUN, SK_ECLIPTIC);
    //        oData << QString("%1\t%2\t%3\t%4\t%5\n").arg(tTime, 15, 'f', 6).arg(ox, 15, 'f', 12).arg(oy, 15, 'f', 12).arg(oz, 15, 'f', 12).arg(sqrt(ox*ox+oy*oy+oz*oz), 15, 'f', 12);

            detGQ(&Ge, &Qu, mas_to_rad(ocFile->ocList[i]->ra), mas_to_rad(ocFile->ocList[i]->de), tTime);
            qDebug() << QString("TT=%1\tG=%2\tQ=%3\n").arg(tTime, 14, 'f', 6).arg(Ge).arg(Qu);

            pRa = koef*P*sin(Qu);
            pDec = koef*P*cos(Qu);

            Cx[i] = fabs(rad_to_mas(P));
//           Cx[i*2+1] = 1.0;
            Lx[i] = sqrt(ocFile->ocList[i]->ocRaCosDe*ocFile->ocList[i]->ocRaCosDe + ocFile->ocList[i]->ocDe*ocFile->ocList[i]->ocDe);
            We[i] = 1.0;
    //        oData << QString("%1\t%2\t%3\t%4\t%5\n").arg(tTime, 12, 'f', 4).arg(ocFile->ocList[i]->phase, 8, 'f', 3).arg(rad_to_mas(pRa), 8, 'f', 3).arg(rad_to_mas(pDec), 8, 'f', 3).arg(rad_to_mas(sqrt(pDec*pDec + pRa*pRa)), 8, 'f', 3);
    //        oData << QString("%1\t%2\t").arg(ocFile->ocList[i]->ra, 15, 'f', 3).arg(ocFile->ocList[i]->de, 15, 'f', 3);
    //        r0 = sqrt(pow(ocFile->ocList[i]->ra-ras, 2.0) + pow(ocFile->ocList[i]->de-decs,2.0));
    //        oData << QString("%1\t%2\t%3\n").arg(rad_to_mas(pRa)).arg(rad_to_mas(pDec)).arg(rad_to_mas(sqrt(pDec*pDec+pRa*pRa)));
            dec = mas_to_rad(ocFile->ocList[i]->de) + pDec;
            cosDec = cos(dec);
            decOC = mas_to_rad(ocFile->ocList[i]->ocDe) + pDec;
            ra = ocFile->ocList[i]->ra + rad_to_mas(pRa)/cosDec;
            raOC = ocFile->ocList[i]->ocRaCosDe + rad_to_mas(pRa);
            dec = rad_to_mas(dec);
            decOC = rad_to_mas(decOC);

//            Lx[i] = sqrt(raOC*raOC + decOC*decOC);


     //       r1 = sqrt(pow(ra-ras, 2.0) + pow(dec-decs,2.0));
      //      oData << QString("%1\n").arg(r0-r1, 15, 'f', 3);

            ocl = new ocRec;
            ocl->copy(*ocFile->ocList[i]);
            /*
            ocl->MJday = ocFile->ocList[i]->MJday;
            ocl->ra = ra;
            ocl->de = dec;
            ocl->mag0 = ocFile->ocList[i]->mag0;
            ocl->ocRaCosDe = raOC;
            ocl->ocDe = decOC;
            ocl->ocMag = ocFile->ocList[i]->ocMag;
            //ocl-> = ocl->ra;
            //ocl->dec_oc = ocl->de;
            ocl->topDist = ocFile->ocList[i]->topDist;
            ocl->muRa = ocFile->ocList[i]->muRa;
            ocl->muDe = ocFile->ocList[i]->muDe;
            ocl->Vr = ocFile->ocList[i]->Vr;
            ocl->phase = ocFile->ocList[i]->phase;
            ocl->elong = ocFile->ocList[i]->elong;
            ocl->name = QString(ocFile->ocList[i]->name);
            ocl->catNum = ocFile->ocList[i]->catNum;
            ocl->expTime = ocFile->ocList[i]->expTime;*/
            phFile->ocList << ocl;

            oData << QString("%1\t%2\t%3\t%4\t%5\n").arg(tTime, 12, 'f', 4).arg(ocl->phase, 8, 'f', 3).arg(ocl->ocRaCosDe, 8, 'f', 3).arg(ocl->ocDe, 8, 'f', 3).arg(sqrt(ocl->ocDe*ocl->ocDe + ocl->ocRaCosDe*ocl->ocRaCosDe*cosDec*cosDec), 8, 'f', 3);

        }

        phFile->save();


        iLSM(1, sz, 500, excX, Zx, Cx, Lx, uweX, dX, fl, rn, We);
        koef1 = Zx[0];
    qDebug() << QString("\nkoef= %1; uwe= %2\n").arg(Zx[0]).arg(sqrt(dX[0]));
 //   qDebug() << QString("\nkoef1= %1; uwe1= %2\n").arg(Zx[1]).arg(sqrt(dX[3]));
 //   }while
 //   phFile->save(fabs()>0.1);
/*

    double raS, decS;
    double ox, oy, oz;
    double dTime;
    for(tTime=2451538.50; tTime<2458864.50; tTime+=60.0)
    {
//        UTC2TDT(tTime, &dTime);
        place->detR(&ox, &oy, &oz, tTime, GEOCENTR_NUM, 0, CENTER_SUN, SK_EKVATOR);
//        place->detR(&ox, &oy, &oz, tTime, MERCURY_NUM, 0, CENTER_BARY, SK_EKVATOR);
        oData << QString("%1\t%2\t%3\t%4\t%5\n").arg(tTime, 15, 'f', 6).arg(ox, 15, 'f', 12).arg(oy, 15, 'f', 12).arg(oz, 15, 'f', 12).arg(sqrt(ox*ox+oy*oy+oz*oz), 15, 'f', 12);
        raS = atan2(oy, ox);
        decS = atan2(oz, sqrt(ox*ox + oy*oy));
        if(decS>PI/2.0) {raS += PI; decS = PI/2.0 - decS;}
        if(decS<-PI/2.0) {raS += PI; decS = PI/2.0 + decS;}

        if((raS>2.0*PI)) raS -=2.0*PI;
        if((raS<0.0)) raS +=2.0*PI;

//        oData << QString("%1\t%2\n").arg(mas_to_hms(rad_to_mas(raS), ":", 3)).arg(mas_to_damas(rad_to_mas(decS), ":", 3));


    }


*/
    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;
//    oFile.close();

    qInstallMsgHandler(0);

    return 0;
//  return a.exec();
}
