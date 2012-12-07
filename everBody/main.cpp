#include <QCoreApplication>
#include <QtCore>

static QDataStream* clog0 = 0;
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
    if(clog0 && clog0->device())
        *clog0 << type << msg;
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


#include "./rada.h"
#include "./../libs/astro.h"
#include "./../libs/dele.h"
//#include "./../libs/skyarea.h"
#include "./../libs/orbcat.h"
#include "./../libs/orbit.h"
#include "./../libs/observ.h"
#include "./../libs/comfunc.h"
#include <math.h>

//#include <QDebug>
//#include <QSettings>
//#include <QDataStream>

//struct ever_params;

ever_params *eparam;

/*
int ever(ifstream *file_date, ifstream *file_params, ofstream *file_res);
int preever(ifstream *file_kepler, ofstream *file_date);
int pe_graph(ifstream *file_inter, ofstream *file_res, int var);
*/
int ever(ever_params *epar, QString file_ilist, QString file_icat, QString file_ires, double t, QString obsCode);
int ever1(ever_params *epar, QString file_ilist, QString file_icat, QString file_ires, double t);

int fOrb(double x0, double x1, double x2, double v0, double v1, double v2, double t0, double mas, double *a, double *ecc, double *M0, double *Om, double *i, double *w);

int Sint(double X[], double V[]);
int LFint(double X[], double V[]);

double _EK(double M, double ecc);
double _Sign(double z, double eps);

dele *nbody;
int nofzbody;
double a, CC, omega, Ltilde, A;
double col, vout;

observ *opos;
//QString jplFile;


int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);

    //if(curDir.exists(logFileName)) return 1;
    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog0 = new QDataStream(logFile);

    //jplFile = "./../../data/cats/binp1940_2020.405";

    nbody = new dele();
    //int iniHeadRes = nbody->init_header("./../../data/cats/header.405");
    //int iniHeadRes = nbody->init_header("./../../data/cats/header1980_2000.405");
    //int iniJplRes = nbody->init_jpl("./../../data/cats/ascp2000.405");

    //int iniJplRes = nbody->init("./../../../data/cats/bin1940_2020.win.405");

    QSettings *sett = new QSettings("./nb.ini", QSettings::IniFormat);

    QString jplFile = sett->value("general/jplFile", "./../../data/cats/binp1940_2020.405").toString();
    QString obsFile = sett->value("general/obsFile", "./../../data/cats/Obs.txt").toString();
    QString obsCode = sett->value("general/obsCode", "500").toString();

    int iniJplRes = nbody->init(jplFile.toAscii().data());
    qDebug() << QString("iniJplRes: %1").arg(iniJplRes);
    if(iniJplRes) return 1;

    opos = new observ();
    int oires = opos->init(obsFile.toAscii().data(), jplFile.toAscii().data());
    if(oires)
    {
        qDebug() << QString("observ init error:%1").arg(oires);
        return 1;
    }
    opos->set_obs_parpam(GEOCENTR_NUM, CENTER_SUN, SK_EKVATOR, obsCode.toAscii().data());



    eparam = new ever_params;

    QSettings *esett = new QSettings("./paramsEv.ini", QSettings::IniFormat);
    eparam->NOR = esett->value("general/NOR", 15).toInt();
    eparam->NCLASS = esett->value("general/NCLASS", -2).toInt();
    eparam->NI = esett->value("general/NI", 2).toInt();
    eparam->NV = esett->value("general/NV", 6).toInt();
    eparam->LL = esett->value("general/LL", 6).toInt();
    eparam->XL = esett->value("general/XL", 0.0).toDouble();
    //eparam->t0 = esett->value("t0", 0.0).toDouble();
    //eparam->te = esett->value("te", 3.0e+3).toDouble();
    //eparam->stp = esett->value("stp", 100.0).toDouble();
    //eparam->shag = esett->value("shag", 10.0).toDouble();
    eparam->col = esett->value("general/col", 0.0015).toDouble();
    eparam->vout = esett->value("general/vout", 1000.0).toDouble();

    ever(eparam, "./ini.lst", "./ini.cat", "./res.lst", 2456000.5, obsCode);

//    return a.exec();

    qInstallMsgHandler(0);
    logFile->close();
    return 0;
}

int ever(ever_params *epar, QString file_ilist, QString file_icat, QString file_ires, double t, QString obsCode)
{
    int i, j, k, num0, num1;
    double x, y, z;
    double vx, vy, vz;
    //IList *iList;
    OrbCat *iCat;
    //RList *rList;
    orbit *iOrb;
    int N;
    int nb;
    double *X, *V, *r;
    double t0, t1, dt;
    mpc mpcRec;
    double TI, TF;
    double tc, tUTC, jdTT, jdTDB;
    double ra, dec, magn;
    double Sdist, Edist;
    char *chStr = new char[256];

    QString tStr;

    iOrb = new orbit();

    qDebug() << "\never\n";

/*    RLRecord *rlRec;
    iList = new IList();
    iList->init(file_ilist.toAscii().data());
    rList = new RList();
    rList->init(file_ires.toAscii().data());*/
/*
    QFile iniFile(file_ilist);
    iniFile.open(QFile::ReadOnly);
    QTextStream iniStm(&iniFile);

    while(!iniStm.atEnd())
    {

    }
  */
    QFile mpcFile("./mpcRes.txt");
    mpcFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream mpcStm(&mpcFile);

    QFile recObj, recState, mpcObj;
    QTextStream objStm, stateStm, mpcObjStm;



    iCat = new OrbCat();
    iCat->is_buff = 1;
    if(iCat->init(file_icat.toAscii().data()))
    {
        qDebug() << QString("file %1 not open\n").arg(file_icat);
        return 1;
    }

    nb = iCat->nstr;
    N = nb*3;
    X = new double[N];
    V = new double[N];
    nofzbody = nb;

    iCat->GetRec(0);
    t0 = iCat->record->eJD;
/*
    for(i=0; i<nb;i++)
    {
        if(iCat->GetRec(i)) continue;
        iOrb->get(iCat->record);
        iOrb->detRecEkv(&X[i*3], &X[i*3+1], &X[i*3+2], t0);
        iOrb->detRecEkvVel(&V[i*3], &V[i*3+1], &V[i*3+2], t0);
    }
*/

    //r = new double[3];

    //iCat->GetRec(0);

    qDebug() << QString("first epoch: %1\t%2\n").arg(t0, 10, 'f', 2).arg(getStrFromDATEOBS(getDATEOBSfromMJD(jd2mjd(t0)), ":", 0, 3));
    TDB2UTC(t0, &tUTC);
    UTC2TDB(tUTC, &jdTDB);
    TDB2TT(t0, &jdTT);

    qDebug() << QString("t-UTC: %1\nt-TT: %2\nUTC-TT: %3\n").arg((t0-tUTC)*86400).arg((t0-jdTT)*86400).arg((tUTC-jdTT)*86400);



    QDir dataDir("./data");//.remove("*.txt");
    QStringList fList;
    QStringList filterList;
    filterList << "*.txt" << "*.mpc";
    fList = dataDir.entryList(filterList);
    //qDebug() << fList.join("\n") << "\n";
    for(i=0; i<fList.size(); i++) dataDir.remove(fList.at(i));


    recState.setFileName("./data/recState.txt");
    recState.open(QFile::WriteOnly | QFile::Truncate);
    stateStm.setDevice(&recState);

    opos->det_observ(tUTC);

    stateStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9\n").arg(tUTC, 14, 'f', 5).arg(opos->ox, 15, 'f', 11).arg(opos->oy, 15, 'f', 11).arg(opos->oz, 15, 'f', 11).arg(sqrt(opos->ox*opos->ox + opos->oy*opos->oy + opos->oz*opos->oz), 15, 'f', 11).arg(opos->ovx, 15, 'f', 11).arg(opos->ovy, 15, 'f', 11).arg(opos->ovz, 15, 'f', 11).arg(sqrt(opos->ovx*opos->ovx + opos->ovy*opos->ovy + opos->ovz*opos->ovz), 15, 'f', 11);

    for(i=0; i<nb;i++)
    {
        qDebug() << QString("inum %1\n").arg(i);



        if(iCat->GetRec(i))
        {
//				AfxMessageBox("c1");
            qDebug() << QString("%1 is absent\n").arg(i);
            continue;
        }

        recObj.setFileName(QString("./data/%1.txt").arg(QString(iCat->record->name).simplified()));
        recObj.open(QFile::WriteOnly | QFile::Append);
        objStm.setDevice(&recObj);

        mpcObj.setFileName(QString("./data/%1.mpc").arg(QString(iCat->record->name).simplified()));
        mpcObj.open(QFile::WriteOnly | QFile::Append);
        mpcObjStm.setDevice(&mpcObj);

        //rlRec = new RLRecord;
        iOrb->get(iCat);
        //iOrb->detRecEkv(&x, &y, &z, iCat->record->eJD);
        iOrb->detRecEkv(&x, &y, &z, jdTT);
        iOrb->detRecEkvVel(&vx, &vy, &vz, jdTT);

        X[i*3] = x;
        X[i*3+1] = y;
        X[i*3+2] = z;
        V[i*3] = vx;
        V[i*3+1] = vy;
        V[i*3+2] = vz;

        objStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9\n").arg(jdTT, 14, 'f', 5).arg(x, 15, 'f', 11).arg(y, 15, 'f', 11).arg(z, 15, 'f', 11).arg(sqrt(x*x + y*y + z*z), 15, 'f', 11).arg(vx, 15, 'f', 11).arg(vy, 15, 'f', 11).arg(vz, 15, 'f', 11).arg(sqrt(vx*vx + vy*vy + vz*vz), 15, 'f', 11);


        detRDnumGC(&ra, &dec, x, y, z, opos->ox, opos->oy, opos->oz, opos->obs->dcx, opos->obs->dcy, opos->obs->dcz);

        Sdist = sqrt(x*x + y*y + z*z);
        Edist = sqrt((opos->ox - x)*(opos->ox - x) + (opos->oy - y)*(opos->oy - y) + (opos->oz - z)*(opos->oz - z));

        magn = det_m(iCat->record->H, Sdist, Edist, 5.8, detPhase(opos->ox, opos->oy, opos->oz, x, y, z));

        mpcRec.head->set_Snum(iCat->record->number);
        mpcRec.eJD = tUTC;
        mpcRec.r = ra;
        mpcRec.d = dec;
        mpcRec.tail->set_magn(magn);
        mpcRec.tail->set_numOfObs(obsCode.toAscii().data());

        mpcRec.toString(chStr);

        mpcStm << QString("%1\n").arg(QString(chStr));
        mpcObjStm << QString("%1\n").arg(QString(chStr));
        qDebug() << QString("%1\n").arg(QString(chStr));

        recObj.close();
        mpcObj.close();
    }



/*
    Everhardt *sun;
    sun = new Everhardt(nb, epar->NCLASS, epar->NOR, epar->NI, epar->LL, epar->XL);


    t1 = t0+20;
    dt = 2;

    for(k=0; k<10; k++)
    {

        TI = t0+k*dt;
        TF = t0+(k+1)*dt;

        TDB2UTC(TF, &tUTC);
        UTC2TDB(tUTC, &jdTDB);
        TDB2TT(TF, &jdTT);


        opos->det_observ(tUTC);

        sun->rada27(X, V, TI, TF);


        stateStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9\n").arg(tUTC, 14, 'f', 5).arg(opos->ox, 15, 'f', 11).arg(opos->oy, 15, 'f', 11).arg(opos->oz, 15, 'f', 11).arg(sqrt(opos->ox*opos->ox + opos->oy*opos->oy + opos->oz*opos->oz), 15, 'f', 11).arg(opos->ovx, 15, 'f', 11).arg(opos->ovy, 15, 'f', 11).arg(opos->ovz, 15, 'f', 11).arg(sqrt(opos->ovx*opos->ovx + opos->ovy*opos->ovy + opos->ovz*opos->ovz), 15, 'f', 11);

        for(i=0; i<nb;i++)
        {
            qDebug() << QString("inum %1\n").arg(i);



            if(iCat->GetRec(i))
            {
    //				AfxMessageBox("c1");
                qDebug() << QString("%1 is absent\n").arg(i);
                continue;
            }

            recObj.setFileName(QString("./data/%1.txt").arg(QString(iCat->record->name).simplified()));
            recObj.open(QFile::WriteOnly | QFile::Append);
            objStm.setDevice(&recObj);

            mpcObj.setFileName(QString("./data/%1.mpc").arg(QString(iCat->record->name).simplified()));
            mpcObj.open(QFile::WriteOnly | QFile::Append);
            mpcObjStm.setDevice(&mpcObj);


            x = X[i*3];
            y = X[i*3+1];
            z = X[i*3+2];
            vx = V[i*3];
            vy = V[i*3+1];
            vz = V[i*3+2];

            objStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9\n").arg(jdTT, 14, 'f', 5).arg(x, 15, 'f', 11).arg(y, 15, 'f', 11).arg(z, 15, 'f', 11).arg(sqrt(x*x + y*y + z*z), 15, 'f', 11).arg(vx, 15, 'f', 11).arg(vy, 15, 'f', 11).arg(vz, 15, 'f', 11).arg(sqrt(vx*vx + vy*vy + vz*vz), 15, 'f', 11);


            detRDnumGC(&ra, &dec, x, y, z, opos->ox, opos->oy, opos->oz, opos->obs->dcx, opos->obs->dcy, opos->obs->dcz);

            Sdist = sqrt(x*x + y*y + z*z);
            Edist = sqrt((opos->ox - x)*(opos->ox - x) + (opos->oy - y)*(opos->oy - y) + (opos->oz - z)*(opos->oz - z));

            magn = det_m(iCat->record->H, Sdist, Edist, 5.8, detPhase(opos->ox, opos->oy, opos->oz, x, y, z));

            mpcRec.head->set_Snum(iCat->record->number);
            mpcRec.eJD = tUTC;
            mpcRec.r = ra;
            mpcRec.d = dec;
            mpcRec.tail->set_magn(magn);
            mpcRec.tail->set_numOfObs(obsCode.toAscii().data());

            mpcRec.toString(chStr);

            mpcStm << QString("%1\n").arg(QString(chStr));
            mpcObjStm << QString("%1\n").arg(QString(chStr));
            qDebug() << QString("%1\n").arg(QString(chStr));

            recObj.close();
            mpcObj.close();
        }
    }
/*

    for(i=0; i<nb;i++)
    {
        rlRec = new RLRecord;
        iList->GetRec(i);
        if(iCat->GetRecName(iList->record->name)==-1)
        {
//				AfxMessageBox("c1");
            qDebug() << QString("%1 is absent\n").arg(i);
            continue;
        }
        rlRec->name = iCat->record->name;
        x = X[i*3];
        y = X[i*3+1];
        z = X[i*3+2];
        detRDnumGC(&rlRec->ra, &rlRec->dec, x, y, z, opos->ox, opos->oy, opos->oz, opos->obs->dcx, opos->obs->dcy, opos->obs->dcz);

        Sdist = sqrt(x*x + y*y + z*z);
        Edist = sqrt((opos->ox - x)*(opos->ox - x) + (opos->oy - y)*(opos->oy - y) + (opos->oz - z)*(opos->oz - z));

        rlRec->magn = det_m(iCat->record->H, Sdist, Edist, 5.8, detPhase(opos->ox, opos->oy, opos->oz, x, y, z));

        rlRec->taskName = "";
        rlRec->exp = 1.0;
        rlRec->num = iCat->record->number;
        //sprintf(rlRec->num, "%s", QString("%1").arg(iCat->record->number).toAscii().data());
        rList->AddRec(rlRec);
        qDebug() << QString("%1: %2\t%3\n").arg(rlRec->name).arg(mas_to_hms(grad_to_mas(rlRec->ra), " ", 3)).arg(mas_to_damas(grad_to_mas(rlRec->dec), " ", 3));
    }
    rList->save();
*/
    mpcFile.close();
    recState.close();

    return 0;
}


double _EK(double M, double ecc)
{
        double E, Ek, n;

        E = M;
        do
        {
                Ek = E;
                E = M + ecc*sin(E);
        }
        while(fabs(E - Ek) > 1e-5);

        return(E);
}

double _Sign(double z, double eps)
{
//	cout << "e " << e << endl;
        if(fabs(z)>eps) return(z/sqrt(z*z));
        return 0;
}
/*
int ever1(ever_params *epar, QString file_ilist, QString file_icat, QString file_ires, double t)
{
    int i, j, k, num0, num1;
    double x, y, z;
    IList *iList;
    OrbCat *iCat;
    RList *rList;
    RLRecord *rlRec;
    orbit *iOrb;
    int N;
    int nb;
    double *X, *V, *r;

    qDebug() << "\never\n";

    QFile resFile("./res.txt");
    resFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
    QTextStream resStm(&resFile);

    iList = new IList();
    iList->init(file_ilist.toAscii().data());
    rList = new RList();
    rList->init(file_ires.toAscii().data());
    iCat = new OrbCat();
    iCat->is_buff = 1;
    iCat->init(file_icat.toAscii().data());
    iOrb = new orbit();

    observ *opos = new observ();
    int oires = opos->init("./../../data/cats/Obs.txt", jplFile.toAscii().data());
    //int oires = opos->init("./../../../data/cats/Obs.txt", "./../../../data/cats/bin1940_2020.win.405");
    //int oires = opos->init("./../../data/cats/Obs.txt", "./../../data/cats/header.405", "./../../data/cats/ascp2000.405");
    if(oires)
    {
        qDebug() << QString("observ init error:%1").arg(oires);
        return 1;
    }
    opos->set_obs_parpam(GEOCENTR_NUM, CENTER_BARY, SK_EKVATOR, "084");

    nb = 1;//iList->nstr;
    N = nb*3;
    epar->NV = N;
    X = new double[N];
    V = new double[N];
    r = new double[3];
    iCat->GetRec(0);
    double t0 = iCat->record->eJD;
    qDebug() << QString("first epoch: %1\t%2\n").arg(t0, 10, 'f', 2).arg(getStrFromDATEOBS(getDATEOBSfromMJD(jd2mjd(t0)), ":", 0, 3));
    opos->det_observ(t0);
    for(i=0; i<nb;i++)
    {
        qDebug() << QString("inum %1\n").arg(i);
        if(iList->GetRec(i))
        {
            qDebug() << QString("iList[%1] error\n").arg(i);
            continue;
        }
        if(iCat->GetRecName(iList->record->name)==-1)
        {
//				AfxMessageBox("c1");
            qDebug() << QString("%1 is absent\n").arg(i);
            continue;
        }

        iOrb->get(iCat);
        //iOrb->detRecEkv(&x, &y, &z, iCat->record->eJD);
        iOrb->detRecEcl(&X[i*3], &X[i*3+1], &X[i*3+2], t0);
        iOrb->detRecEclVel(&V[i*3], &V[i*3+1], &V[i*3+2], t0);

        r[0] = X[i*3];
        r[1] = X[i*3+1];
        r[2] = X[i*3+2];

        rlRec = new RLRecord;

        resStm << QString("%1|%2|%3|%4|%5|%6\n").arg(X[i*3]).arg(X[i*3+1]).arg(X[i*3+2]).arg(V[i*3]).arg(V[i*3+1]).arg(V[i*3+2]);
        //RotX(r, -EKV);
        //detRDnumGC(&rList->record->RA, &rList->record->DEC, r[0], r[1], r[2], opos->ox, opos->oy, opos->oz, opos->obs->dcx, opos->obs->dcy, opos->obs->dcz);
        detRDnumGC(&rlRec->ra, &rlRec->dec, r[0], r[1], r[2], opos->ox, opos->oy, opos->oz, opos->obs->dcx, opos->obs->dcy, opos->obs->dcz);
        qDebug() << QString("RA %1\tDEC %2\n").arg(rlRec->ra).arg(rlRec->dec);
        qDebug() << QString("%1: %2\t%3\t%4\n").arg(iList->record->name).arg(getStrFromDATEOBS(getDATEOBSfromMJD(jd2mjd(t0)), ":", 0, 3)).arg(mas_to_hms(rad_to_mas(rlRec->ra), " ", 3)).arg(mas_to_damas(rad_to_mas(rlRec->dec), " ", 3));

        rList->AddRec(rlRec);

        qDebug() << QString("%1: %2 \t %3 \t %4\n").arg(i).arg(X[i*3]).arg(X[i*3+1]).arg(X[i*3+2]);
        qDebug() << QString("  : %1 \t %2 \t %3\t %4\n").arg(V[i*3]).arg(V[i*3+1]).arg(V[i*3+2]).arg(sqrt(V[i*3]*V[i*3] + V[i*3+1]*V[i*3+1] + V[i*3+2]*V[i*3+2])*AUKM/86400.0);
    }

    nofzbody = 1;

    Everhardt *sun;
    sun = new Everhardt(epar->NV, epar->NCLASS, epar->NOR, epar->NI, epar->LL, epar->XL);

    double Sdist, Edist;
    double TI, TF;
    double tc, tstep;
    //double t1;
    tstep = (t-t0)/10.0;

    qDebug() << QString("t= %1\tt0= %2\ttstep= %3\n").arg(t, 14, 'f', 2).arg(t0, 14, 'f', 2).arg(tstep);
    qDebug() << QString("t= %1\tt0= %2\ttstep= %3\n").arg(getStrFromDATEOBS(getDATEOBSfromMJD(jd2mjd(t)), ":", 0, 3)).arg(getStrFromDATEOBS(getDATEOBSfromMJD(jd2mjd(t0)), ":", 0, 3)).arg(tstep);

    for(tc = t0; tc<t; tc+=tstep)
    {
        qDebug() << QString("tc= %1date: %2\n").arg(tc).arg(getStrFromDATEOBS(getDATEOBSfromMJD(jd2mjd(tc)), ":", 0, 3));

        TI = tc;
        TF = tc+tstep;


        opos->det_observ(TF);

        sun->rada27(X, V, TI, TF);





        for(i=0; i<nb;i++)
        {

            resStm << QString("%1|%2|%3|%4|%5|%6\n").arg(X[i*3]).arg(X[i*3+1]).arg(X[i*3+2]).arg(V[i*3]).arg(V[i*3+1]).arg(V[i*3+2]);
            /*iList->GetRec(i);
            if(iCat->GetRecName(iList->record->name)==-1)
            {
    //				AfxMessageBox("c1");
                qDebug() << QString("%1 is absent\n").arg(i);
                continue;
            }
            rList->record->set_name(iCat->record->name);
            x = X[i*3];
            y = X[i*3+1];
            z = X[i*3+2];
            detRDnumGC(&rList->record->RA, &rList->record->DEC, x, y, z, opos->ox, opos->oy, opos->oz, opos->obs->dcx, opos->obs->dcy, opos->obs->dcz);

            Sdist = sqrt(x*x + y*y + z*z);
            Edist = sqrt((opos->ox - x)*(opos->ox - x) + (opos->oy - y)*(opos->oy - y) + (opos->oz - z)*(opos->oz - z));

            rList->record->magn = det_m(iCat->record->H, Sdist, Edist, 5.8, detPhase(opos->ox, opos->oy, opos->oz, x, y, z));

            rList->record->noftask = 0;
            rList->record->exp = 1.0;
            sprintf(rList->record->num, "%s", QString("%1").arg(iCat->record->number).toAscii().data());
            rList->AddRec(rList->record);
            qDebug() << QString("%1: %2\t%3\n").arg(rList->record->get_name()).arg(mas_to_hms(grad_to_mas(rList->record->RA), " ", 3)).arg(mas_to_damas(grad_to_mas(rList->record->DEC), " ", 3));/
        }
        //rList->Save();
    }

    resFile.close();

    return 0;
}
*/
