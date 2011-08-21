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

//#include "./../libs/mpcs.h"
#include "./../libs/astro.h"
#include "./../libs/redStat.h"



struct solar_hp_rec
{
    int id;
    double raDeg, deDeg, mJD, delay, theta, e_lambda;
    int flag;
};

struct eph_rec
{
    double mJD, ra, dec, dist, vMag, phase, elong, muRAcosDE, muDE, Dist_dot; //[rad]
};
//RA[HH:MM:SS.SSSSS]|DEC[signDD:MM:SS.SSSS]|top dist[a.u.]|Vmag|phase[deg]|elongation[deg]|RAmotion[mas/min]|DECmotion[mas/min]|Radial_Velocity[km/s]|MPCnumber

/*
Byte-by-byte Description of file: solar_ha.dat
--------------------------------------------------------------------------------
   Bytes Format Units   Label       Explanations
--------------------------------------------------------------------------------
   1-  3  I3    ---     ID          Object number                         (SHA1)
   5- 15  F11.7 deg     RAdeg       Reference point RA  (ICRS system)     (SHA2)
  17- 27  F11.7 deg     DEdeg       Reference point Dec (ICRS system)     (SHA3)
  29- 41  F13.7 d       Ep-2440000 *Measurement epoch                     (SHA4)
  43- 47  F5.2  s       delay      *Light delay time                      (SHA5)
  49- 55  F7.3  deg     theta      *Position angle, theta                 (SHA6)
  57- 62  F6.2  mas   e_lambda      Estimated standard error of abscissa  (SHA7)
      64  I1    ---     flag       *[1,2]FAST or NDAC flag                (SHA8)
--------------------------------------------------------------------------------
Note on Ep-2440000: the measurement epoch is specified in JD with respect to
     JD(TT)2440000.0 and is corrected to the geocentre.
Note on delay: this gives the applied light time delay in the geocentric
     direction of the observed object between the satellite and the Earth.
Note on theta: The position angle of the slit coordinate direction w is
     reckoned positive from North through East.
Note on flag: If the transit corresponds to an NDAC record the flag is 1.
    For a FAST record the flag is 2.
*/

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



int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);

    if(argc!=2) return 1;

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);

    QSettings *settings = new QSettings("hipPhase.ini", QSettings::IniFormat);

    QString ast_eph_prog = settings->value("programs/ast_eph_prog").toString();//soft to find asteroids
    QString ast_eph_prog_folder = settings->value("programs/ast_eph_prog_folder").toString();//working folder of soft to find asteroids
    int flagTarg = settings->value("general/flag").toInt();

    QString fIni = QString(argv[1]);
//    QString fIni = QString("./18.sph");

    QFile iFile(fIni);
    if(!iFile.open(QFile::ReadOnly | QIODevice::Text)) qDebug() << QString("file %1 is not open\n").arg(argv[1]);
    QTextStream iData(&iFile);

    QString fSe;
    if(flagTarg==1) fSe = QString("FAST");
    if(flagTarg==2) fSe = QString("NDAC");
/*
    QFile eFile(argv[2]);
    eFile.open(QFile::ReadOnly | QIODevice::Text);
    QTextStream eData(&eFile);
/
    QFile oFile("res0.dat");
    oFile.open(QFile::WriteOnly | QFile::Truncate | QIODevice::Text);
    QTextStream oData(&oFile);

    QFile tFile("times.dat");
    tFile.open(QFile::WriteOnly | QFile::Truncate | QIODevice::Text);
    QTextStream tData(&tFile);
*/
    QString sLine;
    QStringList sList;
    QList <solar_hp_rec*> rList;
    QList <eph_rec*> eList;
    solar_hp_rec *sRec;
    eph_rec *eRec;

    int sz, i;
    const char *fname;



    QProcess outerProcess;
    QStringList outerArguments;
    QString ObjNum, sT, sMJD, msgstr;
    int is_ok;
    double dT, dRa, dDec;


    if(iData.status()) qDebug() << "status " << iData.status() << "\n";
    else qDebug() << "\nstatus GOOD\n";
    int flag;
    while(!iData.atEnd())
    {
        sLine = iData.readLine();
        sList = sLine.split("|");
        flag = sList.at(7).toInt();
//        qDebug() << "flag= " << flag << "\n";
        if(flagTarg&&(flagTarg!=flag))
        {
            qDebug() << QString("flagTarg=%1\tflag=%2\n").arg(flagTarg).arg(flag);
            continue;
        }
        sRec = new solar_hp_rec;

        sRec->id = sList.at(0).toInt();
        sRec->raDeg = sList.at(1).toDouble();
        sRec->deDeg = sList.at(2).toDouble();
        sRec->mJD = sList.at(3).toDouble()+39999.5;
        sRec->delay = sList.at(4).toDouble();
        sRec->theta = sList.at(5).toDouble();
        sRec->e_lambda = sList.at(6).toDouble();
        sRec->flag = flag;

        rList << sRec;

//        tData << QString("%1\n").arg(sRec->mJD+2440000.0, 19, 'f', 11);
    }
    iFile.close();
    qDebug() << "tot= " << rList.size() << "\n";

    outerProcess.setWorkingDirectory(ast_eph_prog_folder);
    outerProcess.setProcessChannelMode(QProcess::MergedChannels);
    outerProcess.setReadChannel(QProcess::StandardOutput);

    sz = rList.size();
    for(i=0; i<sz; i++)
    {
//        mjd = mjdfromS2Cfits(date_obs_str)+exp_time/86400;
        ObjNum = QString("%1").arg(rList[i]->id);
        sMJD = QString( "%1" ).arg(rList[i]->mJD,17,'f',10,QLatin1Char( ' ' ));

        outerArguments.clear();
        outerArguments<< "-num" << ObjNum << sMJD;
//        qDebug() << ast_eph_prog + " " + outerArguments.join("|") << endl;
        outerProcess.start(ast_eph_prog, outerArguments);

        is_ok = outerProcess.waitForFinished(-1);
        QTextStream objStream(outerProcess.readAllStandardOutput());

//        objStream.readLine();
        if(is_ok) msgstr = objStream.readLine();
        sT = msgstr.section('|',0,0);
        if(sT.size()<2)
        {
                qDebug() << " ephemerides error" << endl;
                continue;
        }
        sT = msgstr.section('|',1,1);
        if(sT.size()<2)
        {
                qDebug() << " ephemerides error" << endl;
                continue;
        }

        eRec = new eph_rec;
        eRec->mJD = rList[i]->mJD;

        qDebug() << msgstr << "\n";
        sT = msgstr.section('|',0,0);
        eRec->ra = hms_to_mas(sT, ":");
        sT = msgstr.section('|',1,1);
        eRec->dec = damas_to_mas(sT, ":");
        sT = msgstr.section('|',2,2);
        eRec->dist = sT.toDouble();
        sT = msgstr.section('|',3,3);
        eRec->vMag = sT.toDouble();
        sT = msgstr.section('|',4,4);
        eRec->phase = sT.toDouble();
        sT = msgstr.section('|',5,5);
        eRec->elong = sT.toDouble();
        sT = msgstr.section('|',6,6);
        eRec->muRAcosDE = sT.toDouble();
        sT = msgstr.section('|',7,7);
        eRec->muDE = sT.toDouble();
        sT = msgstr.section('|',8,8);
        eRec->Dist_dot = sT.toDouble();

//RA[HH:MM:SS.SSSSS]|DEC[signDD:MM:SS.SSSS]|top dist[a.u.]|Vmag|phase[deg]|elongation[deg]|RAmotion*CosDEC[mas/min]|DECmotion[mas/min]|Radial_Velocity[km/s]|MPCnumber
        eList << eRec;
    }

//    mpcFile->save();


/*
    while(!eData.atEnd())
    {
        sLine = eData.readLine();
        if(sLine[0]=='#') continue;

 /*       sList = sLine.split(" ", QString::SkipEmptyParts);
/*
        if(sList.size()!=19)
        {
            qDebug() << "WARN sListSize= " << sList.size() << "\n";
            break;
        }
/
        eRec = new eph_rec;

//        dat2JD_time(&eRec->eJD, sLine.mid(8, 4).toInt(), sLine.mid(4, 2).toInt(), sLine.mid(0, 3).toInt(), sLine.mid(13, 2).toInt(), sLine.mid(16, 2).toInt(), sLine.mid(19, 5).toDouble());
        eRec->eJD = sLine.mid(2, 22).toDouble();
        hms2rad(&eRec->ra, 1, sLine.mid(27, 2).toInt(), sLine.mid(30, 2).toInt(), sLine.mid(33, 8).toDouble());
        gms2rad(&eRec->dec, sLine.mid(42, 1)=="+", sLine.mid(43, 2).toInt(), sLine.mid(46, 2).toInt(), sLine.mid(49, 7).toDouble());

        eRec->dist = sLine.mid(57, 14).toDouble();
        eRec->vMag = sLine.mid(72, 7).toDouble();
        eRec->phase = sLine.mid(80, 7).toDouble();
        eRec->elong = sLine.mid(88, 8).toDouble();
        eRec->muRAcosDE = sLine.mid(97, 10).toDouble();
        eRec->muDE = sLine.mid(108, 10).toDouble();
        eRec->Dist_dot = sLine.mid(119, 9).toDouble();
        /*
        dat2JD_time(&eRec->eJD, sList.at(2).toInt(), sList.at(1).toInt(), sList.at(0).toInt(), sList.at(3).toInt(), sList.at(4).toInt(), sList.at(5).toDouble());
        hms2rad(&eRec->ra, 1, sList.at(6).toInt(), sList.at(7).toInt(), sList.at(8).toDouble());
        gms2rad(&eRec->dec, sign(sList.at(9).toInt()), sList.at(9).toInt(), sList.at(10).toInt(), sList.at(11).toDouble());
        eRec->dist = sList.at(12).toDouble();
        eRec->vMag = sList.at(13).toDouble();
        eRec->phase = sList.at(14).toDouble();
        eRec->elong = sList.at(15).toDouble();
        eRec->muRAcosDE = sList.at(16).toDouble();
        eRec->muDE = sList.at(17).toDouble();
        eRec->Dist_dot = sList.at(18).toDouble();
/
  //      tData << QString("%1\n").arg(eRec->eJD - 2440000.0, 19, 'f', 11);
        eList << eRec;
    }
*/
    sz = rList.size();
    qDebug() << sz << " ?= " << eList.size() << "\n";
    if(sz!=eList.size()) return 1;

    QString fO = QString("%1_%2.ph").arg(fIni.left(fIni.lastIndexOf(".sph"))).arg(fSe);
    qDebug() << "\noFile: " << fO << "\n";

    qDebug() << "\noIni: " << fIni << "\n";

    QFile oFile(fO);
    oFile.open(QFile::WriteOnly | QFile::Truncate | QIODevice::Text);
    QTextStream oData(&oFile);

    QString fRes = QString("%1_%2_eq.txt").arg(fIni.left(fIni.lastIndexOf(".sph"))).arg(fSe);
    QByteArray *tmp = new QByteArray(fRes.toAscii());
    fname = tmp->data();

    qDebug() << "\nfname: " << fname << "\n";
    eqFile *ocFile = new eqFile;
    ocFile->init(fname);
    ocFile->clear();
    ocRec *ocl;

    for(i=0; i<sz; i++)
    {
        dT = rList[i]->mJD - eList[i]->mJD;
        dRa = (rad_to_mas(grad2rad(rList[i]->raDeg)) - eList[i]->ra)*cos(grad2rad(rList[i]->deDeg));
        dDec = rad_to_mas(grad2rad(rList[i]->deDeg)) - eList[i]->dec;
//        oData << QString("%1\t%2\t%3\t%4\t%5\n").arg(rList[i]->mJD, 19, 'f', 11).arg(dT).arg(eList[i]->phase, 6, 'f', 2).arg(dRa).arg(dDec);
        oData << QString("%1|%2|%3|%4|%5|%6|%7\n").arg(eList[i]->mJD, 10, 'f', 3).arg(eList[i]->ra, 15, 'f', 3).arg(rad_to_mas(grad2rad(rList[i]->raDeg)), 15, 'f', 3).arg(eList[i]->dec, 15, 'f', 3).arg(rad_to_mas(grad2rad(rList[i]->deDeg)), 15, 'f', 3).arg(dRa).arg(dDec);
//        oData << QString("%1|%2|%3|%4|%5\n\n").arg(getStrFromDATEOBS(getDATEOBSfromMJD(rList[i]->mJD), " ", 0, 5)).arg(mas_to_hms(rad_to_mas(grad2rad(rList[i]->raDeg)), " ", 4)).arg(mas_to_damas(rad_to_mas(grad2rad(rList[i]->deDeg)), " ", 4)).arg(dRa).arg(dDec);

//MJday		    |ra		         |de		   |mag0    |ocRa    |ocDe  |ocMag|ra_oc	       |de_oc		  |topDist	       |muRa	   |muDe     |Vr      |phase   |elong       |name   |catNum|expTime
//2008 05 01.93212|14 26 14.5889|+00 05 12.342|13.279|  -113.6|    28.7|    -1.6|14 26 15.2930|+00 04 53.780| 1.906076352|  -467.40|   219.00|  2.21|   5.420| 164.300|00890|0	   | 180.000
//int id;
 //   double raDeg, deDeg, mJD, delay, theta, e_lambda;
 //   int flag;
 //double mJD, ra, dec, dist, vMag, phase, elong, muRAcosDE, muDE, Dist_dot; //[rad]
        ocl = new ocRec;
        ocl->MJday = rList[i]->mJD;
        ocl->ra = rad_to_mas(grad2rad(rList[i]->raDeg));
        ocl->de = rad_to_mas(grad2rad(rList[i]->deDeg));
        ocl->mag0 = eList[i]->vMag;
        ocl->ocRa = dRa;
        ocl->ocDe = dDec;
        ocl->ocMag = 0.0;
        ocl->ra_oc = ocl->ra;
        ocl->dec_oc = ocl->de;
        ocl->topDist = eList[i]->dist;
        ocl->muRa = eList[i]->muRAcosDE;
        ocl->muDe = eList[i]->muDE;
        ocl->Vr = eList[i]->Dist_dot;
        ocl->phase = eList[i]->phase;
        ocl->elong = eList[i]->elong;
        ocl->name = QString("%1").arg(rList[i]->id, 5, 10, QLatin1Char('0'));
        ocl->catNum = 0;
        ocl->expTime = 0.0;
        ocFile->ocList << ocl;
    }
    ocFile->save();

    delete clog;
    clog = 0;
    delete logFile;
    logFile = 0;
//    oFile.close();

    qInstallMsgHandler(0);

    return 0;
//    return a.exec();
}
