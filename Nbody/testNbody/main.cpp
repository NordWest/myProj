#include <QtCore/QCoreApplication>
//#include <QApplication>

//#define SERIAL
#define OMPLIB

//#include "./../../libs/dele.h"
#include <dele.h>
//#include "./../../libs/orbcat.h"
//#include "./../../libs/observ.h"
//#include "./../../libs/dele.h"
//#include "./../../libs/orbit.h"
//#include "./../../libs/skyarea.h"
#include <astro.h>
#include "./../../libs/comfunc.h"
#include <rada.h>
#include "./../../libs/redStat.h"
#include "./../../libs/mpcs.h"
#include "./../../libs/mpccat.h"

#include "./../../libs/calc_epm.h"

#include <QDebug>
#include <QSettings>
#include <QDataStream>
#include <QDomDocument>

#include <calceph.h>
#include <SpiceUsr.h>

/*
#include <cuda.h>
#include <cutil.h>
#include <cutil_inline_drvapi.h>

// utilities and system includes
#include <shrUtils.h>
#include <shrQATest.h>
*/

#include "./../../libs/moody/capsule/capsuleBase/mopfile/MopFile.h"
#include "./../../libs/moody/moody.h"
#include "./../../libs/moody/capsule/capsuleBase/particle/Particle.h"
#include "./../../libs/moody/capsule/Capsule.h"
#include "./../../libs/moody/capsule/capsuleBase/CapsuleBase.h"

#include "./../../libs/myDomMoody.h"

//int readCFG(QString fileName, QList<ParticleStruct *> &pList);
//int saveCFG(QString fileName, QList<ParticleStruct *> &pList);

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


dele *nbody;
int nofzbody;
double *mass;
//double *massF;
//int *fbodynum;
//int nfbody;

QList <nbobjStruct*> pls;

ever_params *eparam;
int SK, CENTER;
int centr_num;
int useEPM;

//double *CM, *S, LF;
QTextStream resStmBig, resStmSmall;


QList <ParticleStruct*> pList;
//QList <ParticleStruct*> iList;
//QList <ParticleStruct*> jList;

void makeLog(double* X, double *V, double TF, double dt);

void CM_int(double *CM, double X[], double V[])
{
    int i;
    CM[0] = CM[1] = CM[2] = 0.0;
    for(i=0; i<nofzbody; i++)
    {
        CM[0] += pow(pList[i]->mass, -1.0)*V[i*3];
        CM[1] += pow(pList[i]->mass, -1.0)*V[i*3+1];
        CM[2] += pow(pList[i]->mass, -1.0)*V[i*3+2];
    }
}

void S_int(double *S, double X[], double V[])
{
    int i;
    S[0] = S[1] = S[2] = 0.0;
    for(i=0; i<nofzbody; i++)
    {
        S[0] += pow(pList[i]->mass, -1.0)*(X[i*3+1]*V[i*3+2]+V[i*3+1]*X[i*3+2]);
        S[1] += pow(pList[i]->mass, -1.0)*(X[i*3+2]*V[i*3]+V[i*3+2]*X[i*3]);
        S[2] += pow(pList[i]->mass, -1.0)*(X[i*3]*V[i*3+1]+V[i*3]*X[i*3+1]);
    }


}

void LF_int(double *LF, double X[], double V[])
{
    int i;
    *LF = 0.0;
    for(i=0; i<nofzbody; i++)
    {
        *LF += pow(pList[i]->mass, -1.0)*(V[i*3]*V[i*3]+V[i*3+1]*V[i*3+1]+V[i*3+2]*V[i*3+2]);
        //SM[1] += pow(mass[i], -1.0)*(X[i*3+2]*V[i*3]+V[i*3+2]*X[i*3]);
        //SM[2] += pow(mass[i], -1.0)*(X[i*3]*V[i*3+1]+V[i*3]*X[i*3+1]);
    }
    *LF*=0.5;
}

//#define CENTER CENTER_BARY
//#define SK SK_EKVATOR

//void saveResults(double t0, double *X, double *V, double *X0, double *V0, int pos, QString name, QTextStream &resStm, QTextStream &dxStm, QTextStream &deStm)
void saveResults(double t0, double *X, double *V, int pos, QString name, QTextStream &resStm, int dtneg)
{
//    double* r = new double[3];
//    double* v = new double[3];
    double Ri, Vi;

    double vmul = pow(-1, dtneg);

    Ri = sqrt(X[pos+0]*X[pos+0] + X[pos+1]*X[pos+1] + X[pos+2]*X[pos+2]);
    Vi = sqrt(V[pos+0]*V[pos+0] + V[pos+1]*V[pos+1] + V[pos+2]*V[pos+2])*AUKM/86400.0;
    resStm << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(name, -16).arg(t0, 13, 'f', 4).arg(X[pos], 22, 'e', 15).arg(X[pos+1], 22, 'e', 15).arg(X[pos+2], 22, 'e', 15).arg(V[pos]*vmul, 22, 'e', 15).arg(V[pos+1]*vmul, 22, 'e', 15).arg(V[pos+2]*vmul, 22, 'e', 15);

    resStm.flush();

}

//void saveResultsM(double t0, double *X, double *V, double *X0, double *V0, int pos, QString name, QTextStream &resStm, QTextStream &dxStm)
void saveResultsM(double t0, double *X, double *V, int pos, QString name, QTextStream &resStm)
{
    double* r = new double[3];
    double* v = new double[3];
    double Ri, Vi;

    Ri = sqrt(X[pos+0]*X[pos+0] + X[pos+1]*X[pos+1] + X[pos+2]*X[pos+2]);
    Vi = sqrt(V[pos+0]*V[pos+0] + V[pos+1]*V[pos+1] + V[pos+2]*V[pos+2])*AUKM/86400.0;
    resStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|1\n").arg(t0, 13, 'f', 4).arg(X[pos], 18, 'g', 9).arg(X[pos+1], 18, 'g', 9).arg(X[pos+2], 18, 'g', 9).arg(Ri, 18, 'g', 9).arg(V[pos], 18, 'g', 9).arg(V[pos+1], 18, 'g', 9).arg(V[pos+2], 18, 'f', 9).arg(name);


}

int getMiriadeObject(mpephRec *mpcObj, double mJD, QString objStr, procData miriadeProcData);
int getMopName(MopState *mState, MopItem &mItem, QString name)
{
    int i, sz;
    sz = mState->getItemCount();
    for(i=0;i<sz;i++)
    {
        mItem = mState->getMopItem(i);
        if(QString().compare(QString(mItem.name), name)==0) return i;
    }

    return -1;

}




int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);

    QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog0 = new QDataStream(logFile);
    QTextStream logstr(logFile);

    setlocale(LC_NUMERIC, "C");

    int i, N, teloi, nt, j, teloj, p, status;
    double *X, *V, TI, TF, *X0, *V0, *r, *v;
    double jdUTC;
    Particle *tPar;
    MopFile* mFile;
    MopState *mState;
    MopItem mItem;
    double *Xm, *Vm;
    //int SK, CENTER;
    double *CM = new double[3];
    double *S = new double[3];
    double LF;

    double *CM0 = new double[3];
    double *S0 = new double[3];
    double LF0;

    QString name;
    int plaNum, resMiri;
    mpephRec mpcObj;
    QString objDataStr, sJD;
    QStringList outerArguments, resSL;
    QProcess outerProcess;
    procData miriadeProcData;

    QList <ParticleStruct*> iniList;

    SpiceDouble             state [6];
    SpiceDouble             lt;
    SpiceChar             * corr;
    SpiceChar             * ref;
    ref = new SpiceChar[256];
    corr = new SpiceChar[256];


    Everhardt *solSys;

    //QList <ParticleStruct*> pList;
    //dele *nbody;
    nbody = new dele();

    double t0, nstep, ti, dt;
    mpc mrec;
    double XE0[3], VE0[3];
/*
    t0 = 2455201.0;
    t1 = t0+300;
    dt = 20.0;
*/
    QSettings *sett = new QSettings("./nb.ini", QSettings::IniFormat);

    QString jplFile = sett->value("general/jplFile", "./../../data/cats/binp1940_2020.405").toString();
    QString obsFile = sett->value("general/obsFile", "./../../data/cats/Obs.txt").toString();
    QString obsCode = sett->value("general/obsCode", "500").toString();
    QString confFile = "./test.xml";//sett->value("general/confFile", "testMajor.xml").toString();
    QString mpcCatFile = sett->value("general/mpcCatFile", "mocorb.txt").toString();
    //int useConfMasses = sett->value("general/useConfMasses", 0).toInt();
    int useMoody = 0;//sett->value("general/useMoody", 0).toInt();

    int useMiriade = sett->value("general/useMiriade", 0).toInt();
    int obrat = !sett->value("general/obrat", 1).toInt();

    int bigType = sett->value("general/bigType", 0).toInt();

    //time
    t0 = sett->value("time/time0", 0).toDouble();
    dt = sett->value("time/timestep", 1).toDouble();
    nstep = sett->value("time/nstep", 1).toDouble();
    int printstep = sett->value("time/printstep", 0).toInt();

    QString mopFileName = sett->value("moody/mopFile", "Reference_Project.mop").toString();

    CENTER = sett->value("general/center", 0).toInt();
    SK = sett->value("general/sk", 0).toInt();
    useEPM = sett->value("general/useEPM", 0).toInt();

    miriadeProcData.name = sett->value("processes/miriade_prog", "./miriadeEph").toString();
    miriadeProcData.folder = sett->value("processes/miriade_prog_folder", "./").toString();
    miriadeProcData.waitTime = sett->value("processes/miriade_wait_time", -1).toInt();
    if(miriadeProcData.waitTime>0) miriadeProcData.waitTime *= 1000;

    QString epmDir = sett->value("general/epmDir", "./").toString();

    //CALCEPH
    QString ephFile = sett->value("CALCEPH/ephFile", "./../../data/cats/binp1940_2020.405").toString();

    //SPICE
    QString bspName = sett->value("SPICE/bspName", "./de421.bsp").toString();
    QString leapName = sett->value("SPICE/leapName", "./naif0010.tls").toString();
    sprintf(ref,"%s", sett->value("SPICE/ref", "J2000").toString().toAscii().data());
    sprintf(corr,"%s", sett->value("general/corr", "LT").toString().toAscii().data());

    eparam = new ever_params;

    QSettings *esett = new QSettings("./paramsEv.ini", QSettings::IniFormat);
    eparam->NOR = esett->value("general/NOR", 17).toInt();
    eparam->NCLASS = esett->value("general/NCLASS", -2).toInt();
    eparam->NI = esett->value("general/NI", 2).toInt();     //Number of Iteration
    eparam->NV = esett->value("general/NV", 6).toInt();     //Number of dependent Variable
    eparam->LL = esett->value("general/LL", 9).toInt();
    eparam->XL = esett->value("general/XL", 1.0e-9).toDouble();
    eparam->col = esett->value("general/col", 0.0015).toDouble();
    eparam->vout = esett->value("general/vout", 1000.0).toDouble();
    strncpy(&eparam->jkeys[0], esett->value("general/jkeys", "1111111111").toString().toAscii().data(), 10);
    eparam->ppn = esett->value("general/ppn", 0).toInt();



    double *emb = new double[3];
    double *embv = new double[3];


    mpccat mCat;
    int initMpc = mCat.init(mpcCatFile.toAscii().data());

    switch(bigType)
    {
        case 0:
        {
            if(useEPM)
            {
                status = !InitTxt(epmDir.toAscii().data());
                centr_num = 11+!CENTER;
            }
            else
            {
                status = nbody->init(jplFile.toAscii().data());
            }

            if(status)
            {
                qDebug() << QString("init ephemeride error\n");
                return 1;
            }
        }

        break;

        case 1:
        {
            status = !calceph_sopen(ephFile.toAscii().data());
            if(status)
            {
                qDebug() << "\n\nError open capceph\n\n";
                exit(1);
            }

            centr_num = 11+!CENTER;
        }
        break;
        case 2:
        {
            furnsh_c ( leapName.toAscii().data() );     //load LSK kernel
            furnsh_c ( bspName.toAscii().data()  );     //load SPK/BSP kernel with planets ephemerides
        }
        break;
    }

    if(readParticles(confFile, iniList))
    {
        qDebug() << QString("readCFG error\n");
        return 1;
    }

    nbobjStruct* nb_rec;

    for(i=0; i<iniList.size(); i++)
    {
        if(iniList.at(i)->identity==Advisor::collapsorFixed)
        {
            if((QString().compare(QString(iniList.at(i)->name.data()), "Sun", Qt::CaseInsensitive))||(QString().compare(QString(iniList.at(i)->name.data()), "Sol", Qt::CaseInsensitive)))continue;
            nb_rec = new nbobjStruct;
            nb_rec->mass = iniList.at(i)->mass;
            nb_rec->planet_num = planet_num((char*)iniList.at(i)->name.data());
            pls << nb_rec;
            //continue;
        }
        else
        {
            pList << iniList.at(i);
            //if(pList.at(i)->identity==Advisor::ordinary) mass[p++] = iniList.at(i)->mass;
            //else

        }
        //if(pList.at(i)->identity==Advisor::ordinary) pList << iniList.at(i);
        //if(pList.at(i)->identity==Advisor::planetesimal) jList << pList.at(i);
    }

    nofzbody=pList.size();
    mass = new double[nofzbody];

    for(i=0; i<nofzbody; i++)
    {
        if(pList.at(i)->identity==Advisor::ordinary) mass[i] = pList.at(i)->mass;
        else mass[i] = -1.0;
    }



    N = (nofzbody)*3;

    X = new double[N];
    V = new double[N];
    X0 = new double[N];
    V0 = new double[N];
    r = new double[3];
    v = new double[3];

    QFile dxmFile;
    QTextStream dxmStm;
    QFile resmFileBig, resmFileSmall;
    QTextStream resmStmBig, resmStmSmall;
    double ra, de, ra0, de0;
    double ksi, zet, teta, T, t;

//res_big
    QFile resFileBig("res_big.txt");
    resFileBig.open(QIODevice::Truncate | QIODevice::WriteOnly);
    resStmBig.setDevice(&resFileBig);
/*
    QFile dxFileBig("dxdy_big.txt");
    dxFileBig.open(QIODevice::Truncate | QIODevice::WriteOnly);
    QTextStream dxStmBig(&dxFileBig);

    QFile deFileBig("de_big.txt");
    deFileBig.open(QIODevice::Truncate | QIODevice::WriteOnly);
    QTextStream deStmBig(&deFileBig);*/

//res_small
    QFile resFileSmall("res_small.txt");
    resFileSmall.open(QIODevice::Truncate | QIODevice::WriteOnly);
    resStmSmall.setDevice(&resFileSmall);
/*
    QFile dxFileSmall("dxdy_small.txt");
    dxFileSmall.open(QIODevice::Truncate | QIODevice::WriteOnly);
    QTextStream dxStmSmall(&dxFileSmall);

    QFile deFileSmall("de_small.txt");
    deFileSmall.open(QIODevice::Truncate | QIODevice::WriteOnly);
    QTextStream deStmSmall(&deFileSmall);
*/

    solSys = new Everhardt(N, eparam->NCLASS, eparam->NOR, eparam->NI, eparam->LL, eparam->XL);

/*
    char *astr = new char[256];
    QFile mpcFile("./mpc.txt");
    mpcFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream mpcStm(&mpcFile);

    QFile mpcFileM("./mpc_moody.txt");
    mpcFileM.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream mpcStmM(&mpcFileM);
*/


    if(useMoody)
    {


        mFile = new MopFile;
        mFile->setFilename(mopFileName.toAscii().data());
        //mFile->openMopfileReader();
        mState = mFile->readCyclingState();
        /*if(mState->getItemCount()!=nofzbody)
        {
            useMoody = 0;
            qDebug() << QString("Cant open MOP File\n");
        }
        else
        {*/
            Xm = new double[N];
            Vm = new double[N];
/*
            dxmFile.setFileName("dxdy_moody.txt");
            dxmFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
            dxmStm.setDevice(&dxmFile);
*/

            resmFileBig.setFileName("res_moody_big.txt");
            resmFileBig.open(QIODevice::Truncate | QIODevice::WriteOnly);
            resmStmBig.setDevice(&resmFileBig);

            resmFileSmall.setFileName("res_moody_small.txt");
            resmFileSmall.open(QIODevice::Truncate | QIODevice::WriteOnly);
            resmStmSmall.setDevice(&resmFileSmall);

        //}
    }


    double vmul;
    vmul = pow(-1, dt<0);

    p = 0;
    for(i=0; i<nofzbody; i++)
    {
        name = QString(pList[i]->name.data());

        if(useEPM) plaNum = epm_planet_num(name);
        else plaNum = planet_num(name.toAscii().data());
        p = i*3;

        X[p] = pList[i]->x;
        X[p+1] = pList[i]->y;
        X[p+2] = pList[i]->z;
        V[p] = pList[i]->xd*vmul;
        V[p+1] = pList[i]->yd*vmul;
        V[p+2] = pList[i]->zd*vmul;

        if(plaNum!=-1)
        {
            saveResults(t0, X, V, p, name, resStmBig, dt<0);
        }
        else
        {
            saveResults(t0, X, V, p, name, resStmSmall, dt<0);
        }
    }


/*
    CM_int(CM, X, V);
    qDebug() << QString("CM: %1\t%2\t%3\n").arg(CM[0]).arg(CM[1]).arg(CM[2]);
    S_int(S, X, V);
    qDebug() << QString("S: %1\t%2\t%3\n").arg(S[0]).arg(S[1]).arg(S[2]);
    LF_int(&LF, X, V);
    qDebug() << QString("LF: %1\n").arg(LF);
*/
    CM_int(CM0, X, V);
    qDebug() << QString("CM0: %1\t%2\t%3\n").arg(CM0[0]).arg(CM0[1]).arg(CM0[2]);
    S_int(S0, X, V);
    qDebug() << QString("S0: %1\t%2\t%3\n").arg(S0[0]).arg(S0[1]).arg(S0[2]);
    LF_int(&LF0, X, V);
    qDebug() << QString("LF0: %1\n").arg(LF0);

//    for(ti=t0; ti<t1; ti+=dt)
    double *P = new double[9];
    double *ssb, *ssbv, mui, mui1, muis;
    double xt, yt, zt, et;
    ssb= new double[3];
    ssbv= new double[3];
    TF = t0;
    int jday;
    double pday;
    //double state[6];
    QString sName;

    QTime timeElapsed;// = QTime.currentTime();
    timeElapsed.start();

    do
    {
        p=0;
        for(nt=0; nt<nstep; nt++)
        {
            TI = TF;
            TF += dt;

            jday = int(TF);
            pday = TF - jday;

            solSys->rada27(X, V, 0, fabs(dt));

            //i=0;
            if(p++==printstep)
            {
                qDebug() << QString("\njd: %1\ntime: %2\n").arg(TF, 12, 'f', 4).arg(getStrFromDATEOBS(getDATEOBSfromMJD(jd2mjd(TF)), ":", 0, 3));
                //qDebug() << QString("jday: %1\tpday: %2\n").arg(jday).arg(pday);
/*
                CM_int(CM, X, V);
                qDebug() << QString("CM: %1\t%2\t%3\n").arg(CM[0]).arg(CM[1]).arg(CM[2]);
                S_int(S, X, V);
                qDebug() << QString("S: %1\t%2\t%3\n").arg(S[0]).arg(S[1]).arg(S[2]);
                LF_int(&LF, X, V);
                qDebug() << QString("LF: %1\n").arg(LF);
*/
//                makeLog(X, V, TF, dt);


                for(teloi=0; teloi<pList.size(); teloi++)
                {
                    //if(pList.at(teloi)->interactionPermission==Advisor::interactNONE) continue;
                    name = QString(pList[teloi]->name.data());
                    if(useEPM) plaNum = epm_planet_num(name);
                    else plaNum = planet_num(name.toAscii().data());
                    i = teloi*3;
                    if(useMoody)
                    {
                        if(getMopName(mState, mItem, name)!=-1)
                        {
                            Xm[i] = mItem.x/AUKM/1000;
                            Xm[i+1] = mItem.y/AUKM/1000;
                            Xm[i+2] = mItem.z/AUKM/1000;
                            Vm[i] = mItem.xd*SECINDAY/1000/AUKM;
                            Vm[i+1] = mItem.yd*SECINDAY/1000/AUKM;
                            Vm[i+2] = mItem.zd*SECINDAY/1000/AUKM;
                          }
                    }


                    if(plaNum!=-1)
                    {
                        saveResults(TF, X, V, i, name, resStmBig, dt<0);
                        if(useMoody) saveResults(TF, Xm, Vm, i, name, resmStmBig, dt<0);
/*
                        switch(bigType)
                        {
                            case 0:
                            {
                                if(useEPM)
                                {
                                    status = calc_EPM(plaNum, centr_num, (int)TF, TF-(int)TF, &X[i], &V[i]);
                                     if(!status)
                                     {
                                         qDebug() << QString("error EPM\n");
                                         return 1;
                                     }
                                }
                                else
                                {
                                    nbody->detState(&X[i], &X[i+1], &X[i+2], &V[i], &V[i+1], &V[i+2], TF, plaNum, CENTER, SK);
                                }
                            }
                            break;

                            case 1:
                            {

                            //calceph_scompute((int)time0, time0-(int)time0, 16, 0, state);
                            //qDebug() << QString("TT-TDB = %1\n").arg(state[0]);
                            //timei = time0 + state[0];
                            calceph_scompute((int)TF, TF-(int)TF, epm_planet_num(name), centr_num, state);
                            X[i] = state[0];
                            X[i+1] = state[1];
                            X[i+2] = state[2];
                            V[i] = state[3];
                            V[i+1] = state[4];
                            V[i+2] = state[5];



                                break;
                            }
                            case 2:
                            {
                                if(plaNum!=SUN_NUM)
                                {
                                    sName = QString("%1 BARYCENTER").arg(name.simplified().toAscii().data());
                                    qDebug() << QString("name: %1\n").arg(sName);
                                    sJD = QString("%1 JD").arg(TF, 15, 'f',7);
                                    str2et_c(sJD.toAscii().data(), &et);
                                    if(CENTER) spkezr_c (  sName.toAscii().data(), et, ref, "NONE", "sun", state, &lt );
                                    else spkezr_c (  sName.toAscii().data(), et, ref, "NONE", "ssb", state, &lt );
                                    X[i] = state[0]/AUKM;
                                    X[i+1] = state[1]/AUKM;
                                    X[i+2] = state[2]/AUKM;
                                    V[i] = state[3]/AUKM*SECINDAY;
                                    V[i+1] = state[4]/AUKM*SECINDAY;
                                    V[i+2] = state[5]/AUKM*SECINDAY;
                                }
                            }
                                break;
                        }
                        V[i] *= vmul;
                        V[i+1] *= vmul;
                        V[i+2] *= vmul;
*/
                    }
                    else// if(pList.at(teloi)->interactionPermission!=Advisor::interactNONE)
                    {

                        saveResults(TF, X, V, i, name, resStmSmall, dt<0);
                        if(useMoody) saveResults(TF, Xm, Vm, i, name, resmStmSmall, dt<0);

                    }

                }
                p=0;
            }

            if(useMoody) mState = mFile->readState();


            //qDebug() << QString("SSB: %1\t%2\t%3\n").arg(ssb[0]).arg(ssb[1]).arg(ssb[2]);
        }
        if(obrat) break;
        dt = -dt;
        for(teloi=0; teloi<nofzbody; teloi++)
        {
            i=teloi*3;
            V[i] = -V[i];
            V[i+1] = -V[i+1];
            V[i+2] = -V[i+2];
        }
        obrat = 1;

    }while(1);

    if(p)
    {
        makeLog(X, V, TF, dt);
//        p=0;
    }

    qDebug() << QString("Time elapsed: %1 sec\n").arg(timeElapsed.elapsed()/1000.0);

    CM_int(CM, X, V);
    qDebug() << QString("CM: %1\t%2\t%3\n").arg(CM[0]).arg(CM[1]).arg(CM[2]);
    qDebug() << QString("CM0: %1\t%2\t%3\n").arg(CM0[0]).arg(CM0[1]).arg(CM0[2]);
    qDebug() << QString("dCM: %1\t%2\t%3\n").arg(CM[0]-CM0[0]).arg(CM[1]-CM0[1]).arg(CM[2]-CM0[2]);
    S_int(S, X, V);
    qDebug() << QString("S: %1\t%2\t%3\n").arg(S[0]).arg(S[1]).arg(S[2]);
    qDebug() << QString("S0: %1\t%2\t%3\n").arg(S0[0]).arg(S0[1]).arg(S0[2]);
    qDebug() << QString("dS: %1\t%2\t%3\n").arg(S[0]-S0[0]).arg(S[1]-S0[1]).arg(S[2]-S0[2]);
    LF_int(&LF, X, V);
    qDebug() << QString("LF: %1 - %2: %3\n").arg(LF).arg(LF0).arg(fabs(LF-LF0)/LF0);


    resFileBig.close();
    resFileSmall.close();

/*    dxFile.close();
    deFile.close();

    mpcFile.close();
    mpcFileM.close();
*/
    if(useMoody)
    {
        resmFileBig.close();
        resmFileSmall.close();
  //      dxmFile.close();
    }

}

void makeLog(double* X, double *V, double TF, double dt)
{
    int plaNum;
    QString name;
    int teloi, i;


        for(teloi=0; teloi<pList.size(); teloi++)
        {
            //if(pList.at(teloi)->interactionPermission==Advisor::interactNONE) continue;
            name = QString(pList[teloi]->name.data());
            if(useEPM) plaNum = epm_planet_num(name);
            else plaNum = planet_num(name.toAscii().data());
            i = teloi*3;
            /*if(useMoody)
            {
                if(getMopName(mState, mItem, name)!=-1)
                {
                    Xm[i] = mItem.x/AUKM/1000;
                    Xm[i+1] = mItem.y/AUKM/1000;
                    Xm[i+2] = mItem.z/AUKM/1000;
                    Vm[i] = mItem.xd*SECINDAY/1000/AUKM;
                    Vm[i+1] = mItem.yd*SECINDAY/1000/AUKM;
                    Vm[i+2] = mItem.zd*SECINDAY/1000/AUKM;
                  }
            }*/


            if(plaNum!=-1)
            {
                saveResults(TF, X, V, i, name, resStmBig, dt<0);
                //if(useMoody) saveResults(TF, Xm, Vm, i, name, resmStmBig, dt<0);

            }
            else// if(pList.at(teloi)->interactionPermission!=Advisor::interactNONE)
            {

                saveResults(TF, X, V, i, name, resStmSmall, dt<0);
                //if(useMoody) saveResults(TF, Xm, Vm, i, name, resmStmSmall, dt<0);

            }

        }
}

int getMiriadeObject(mpephRec *mpcObj, double mJD, QString objStr, procData miriadeProcData)
{
    qDebug() << "\ngetMiriadeObject\n";
    QStringList outerArguments;

    outerArguments << QString("-name=\"%1\"").arg(objStr.simplified());


    QString sJD = QString("%1").arg(mjd2jd(mJD), 11, 'f',7);
    outerArguments << QString("-ep=%1").arg(sJD);

    QProcess outerProcess;

    outerProcess.setWorkingDirectory(miriadeProcData.folder);
    outerProcess.setProcessChannelMode(QProcess::MergedChannels);
    outerProcess.setReadChannel(QProcess::StandardOutput);

//    if(FD_LOG_LEVEL) qDebug() << "outerArguments: " << miriadeProcData.name << " " << outerArguments.join("|") << "\n";

    outerProcess.start(miriadeProcData.name, outerArguments);

    if(!outerProcess.waitForFinished(miriadeProcData.waitTime))
    {
//        if(FD_LOG_LEVEL) qDebug() << "\nmpephProc finish error\n";
    }
    QTextStream objStream(outerProcess.readAllStandardOutput());
    QString objDataStr;

    while (!objStream.atEnd())
    //for(i=0; i<orList.size(); i++)
    {
        objDataStr = objStream.readLine();
        //objDataStr = orList.at(i);
        if(objDataStr.at(0)=='#') continue;
//        if(FD_LOG_LEVEL) qDebug() << QString("objDataStr: %1").arg(objDataStr);

//        if(FD_LOG_LEVEL) qDebug() << "mpcObj: " << mpcObj << "\n";
        if(mpcObj->fromMiriStr(objDataStr))
        {
//            if(FD_LOG_LEVEL) qDebug() << "\nfromString error\n";
            continue;
        }
        else return 0;
    }

    return 1;
}
