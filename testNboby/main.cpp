#include <QtCore/QCoreApplication>
//#include <QApplication>

#include "./../libs/dele.h"
//#include "./../libs/orbcat.h"
//#include "./../libs/observ.h"
//#include "./../libs/dele.h"
//#include "./../libs/orbit.h"
//#include "./../libs/skyarea.h"
#include "./../libs/astro.h"
#include "./../libs/comfunc.h"
#include "./rada.h"
#include "./../libs/redStat.h"
#include "./../libs/mpcs.h"
#include "./../libs/mpccat.h"

#include "./../libs/calc_epm.h"

#include <QDebug>
#include <QSettings>
#include <QDataStream>
#include <QDomDocument>

/*
#include <cuda.h>
#include <cutil.h>
#include <cutil_inline_drvapi.h>

// utilities and system includes
#include <shrUtils.h>
#include <shrQATest.h>
*/

#define OMPLIB

#include "./../libs/moody/capsule/capsuleBase/mopfile/MopFile.h"
#include "./../libs/moody/moody.h"
#include "./../libs/moody/capsule/capsuleBase/particle/Particle.h"
#include "./../libs/moody/capsule/Capsule.h"
#include "./../libs/moody/capsule/capsuleBase/CapsuleBase.h"

#include "./../libs/myDomMoody.h"

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

int nofzbody;
dele *nbody;
ever_params *eparam;
int SK, CENTER;
int centr_num;
int useEPM;
//double *mass;
QList <ParticleStruct*> pList;
QList <ParticleStruct*> iList;
QList <ParticleStruct*> jList;

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

void saveResults(double t0, double *X, double *V, double *X0, double *V0, int pos, QString name, QTextStream &resStm, QTextStream &dxStm, QTextStream &deStm)
{
    double* r = new double[3];
    double* v = new double[3];
    double Ri, Vi;

    Ri = sqrt(X[pos+0]*X[pos+0] + X[pos+1]*X[pos+1] + X[pos+2]*X[pos+2]);
    Vi = sqrt(V[pos+0]*V[pos+0] + V[pos+1]*V[pos+1] + V[pos+2]*V[pos+2])*AUKM/86400.0;
    resStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|1\n").arg(t0, 13, 'f', 4).arg(X[pos], 22, 'e', 15).arg(X[pos+1], 22, 'e', 15).arg(X[pos+2], 22, 'e', 15).arg(Ri, 22, 'e', 15).arg(V[pos], 22, 'e', 15).arg(V[pos+1], 22, 'e', 15).arg(V[pos+2], 22, 'e', 15).arg(name);

    resStm.flush();

    if(X0==NULL||V0==NULL) return;


    r[0] = X[pos]-X0[pos];
    r[1] = X[pos+1]-X0[pos+1];
    r[2] = X[pos+2]-X0[pos+2];

    Ri = sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);

    v[0] = V[pos]-V0[pos];
    v[1] = V[pos+1]-V0[pos+1];
    v[2] = V[pos+2]-V0[pos+2];

    dxStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9\n").arg(t0, 15, 'f', 7).arg(r[0], 18, 'g', 9).arg(r[1], 18, 'g', 9).arg(r[2], 18, 'g', 9).arg(Ri, 18, 'g', 9).arg(v[0], 18, 'g', 9).arg(v[1], 18, 'g', 9).arg(v[2], 18, 'g', 9).arg(name);

    dxStm.flush();

    Ri = sqrt(X0[pos+0]*X0[pos+0] + X0[pos+1]*X0[pos+1] + X0[pos+2]*r[pos+2]);

    deStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|1\n").arg(t0, 12, 'f', 4).arg(X0[pos], 18, 'g', 9).arg(X0[pos+1], 18, 'g', 9).arg(X0[pos+2], 18, 'g', 9).arg(Ri, 18, 'g', 9).arg(V0[pos], 18, 'g', 9).arg(V0[pos+1], 18, 'g', 9).arg(V0[pos+2], 18, 'g', 9).arg(name);

    deStm.flush();
}

void saveResultsM(double t0, double *X, double *V, double *X0, double *V0, int pos, QString name, QTextStream &resStm, QTextStream &dxStm)
{
    double* r = new double[3];
    double* v = new double[3];
    double Ri, Vi;

    Ri = sqrt(X[pos+0]*X[pos+0] + X[pos+1]*X[pos+1] + X[pos+2]*X[pos+2]);
    Vi = sqrt(V[pos+0]*V[pos+0] + V[pos+1]*V[pos+1] + V[pos+2]*V[pos+2])*AUKM/86400.0;
    resStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|1\n").arg(t0, 13, 'f', 4).arg(X[pos], 18, 'g', 9).arg(X[pos+1], 18, 'g', 9).arg(X[pos+2], 18, 'g', 9).arg(Ri, 18, 'g', 9).arg(V[pos], 18, 'g', 9).arg(V[pos+1], 18, 'g', 9).arg(V[pos+2], 18, 'f', 9).arg(name);



    r[0] = X[pos]-X0[pos];
    r[1] = X[pos+1]-X0[pos+1];
    r[2] = X[pos+2]-X0[pos+2];

    Ri = sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);

    v[0] = V[pos]-V0[pos];
    v[1] = V[pos+1]-V0[pos+1];
    v[2] = V[pos+2]-V0[pos+2];

    dxStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9\n").arg(t0, 12, 'f', 4).arg(r[0], 18, 'g', 9).arg(r[1], 18, 'g', 9).arg(r[2], 18, 'g', 9).arg(Ri, 18, 'g', 9).arg(v[0], 18, 'g', 9).arg(v[1], 18, 'g', 9).arg(v[2], 18, 'g', 9).arg(name);
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

    int i, N, teloi, nt, j, teloj, p;
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
    QString confFile = sett->value("general/confFile", "testMajor.xml").toString();
    QString mpcCatFile = sett->value("general/mpcCatFile", "mocorb.txt").toString();
    //int useConfMasses = sett->value("general/useConfMasses", 0).toInt();
    int useMoody = sett->value("general/useMoody", 0).toInt();
    t0 = sett->value("general/time0", 0).toDouble();
    dt = sett->value("general/dt", 1).toDouble();
    nstep = sett->value("general/nstep", 1).toDouble();
    int useMiriade = sett->value("general/useMiriade", 0).toInt();
    int obrat = !sett->value("general/obrat", 1).toInt();

    QString mopFileName = sett->value("moody/mopFile", "Reference_Project.mop").toString();

    CENTER = sett->value("general/center", 0).toInt();
    SK = sett->value("general/sk", 0).toInt();
    useEPM = sett->value("general/useEPM", 0).toInt();

    miriadeProcData.name = sett->value("processes/miriade_prog", "./miriadeEph").toString();
    miriadeProcData.folder = sett->value("processes/miriade_prog_folder", "./").toString();
    miriadeProcData.waitTime = sett->value("processes/miriade_wait_time", -1).toInt();
    if(miriadeProcData.waitTime>0) miriadeProcData.waitTime *= 1000;

    QString epmDir = sett->value("general/epmDir", "./").toString();

    eparam = new ever_params;

    QSettings *esett = new QSettings("./paramsEv.ini", QSettings::IniFormat);
    eparam->NOR = esett->value("general/NOR", 17).toInt();
    eparam->NCLASS = esett->value("general/NCLASS", -2).toInt();
    eparam->NI = esett->value("general/NI", 2).toInt();     //Number of Iteration
    eparam->NV = esett->value("general/NV", 6).toInt();     //Number of dependent Variable
    eparam->LL = esett->value("general/LL", 9).toInt();
    eparam->XL = esett->value("general/XL", 1.0e-9).toDouble();
    //eparam->t0 = esett->value("t0", 0.0).toDouble();
    //eparam->te = esett->value("te", 3.0e+3).toDouble();
    //eparam->stp = esett->value("stp", 100.0).toDouble();
    //eparam->shag = esett->value("shag", 10.0).toDouble();
    eparam->col = esett->value("general/col", 0.0015).toDouble();
    eparam->vout = esett->value("general/vout", 1000.0).toDouble();
    strncpy(&eparam->jkeys[0], esett->value("general/jkeys", "1111111111").toString().toAscii().data(), 10);




    double *emb = new double[3];
    double *embv = new double[3];


    mpccat mCat;
    int initMpc = mCat.init(mpcCatFile.toAscii().data());

    if(readCFG(confFile, pList))
    {
        qDebug() << QString("readCFG error\n");
        return 1;
    }


    int status;

    if(useEPM)
    {
        status = !InitTxt(epmDir.toAscii().data());
        centr_num = 11+!CENTER;
    }
    else
    {
        status = nbody->init(jplFile.toAscii().data());
        //status = nbody->init_jpl_bin(jplFile.toAscii().data());
    }

    if(status) return 1;

    //nofzbody = pList.size();
    //int iNum, jNum;
    //nofzbody=0;
    //nofjbody=0;
    for(i=0; i<pList.size(); i++)
    {
        if(pList.at(i)->identity==Advisor::collapsorFixed) continue;
        if(pList.at(i)->identity==Advisor::ordinary) iList << pList.at(i);
        if(pList.at(i)->identity==Advisor::planetesimal) jList << pList.at(i);
    }

    nofzbody=iList.size();

    //mass = new double[nofzbody];
    //if(CENTER) nofzbody-=1;





    N = (nofzbody)*3;

    X = new double[N];
    V = new double[N];
    X0 = new double[N];
    V0 = new double[N];
    r = new double[3];
    v = new double[3];

    QFile dxmFile;
    QTextStream dxmStm;
    QFile resmFile;
    QTextStream resmStm;
    double ra, de, ra0, de0;
    double ksi, zet, teta, T, t;


    QFile resFile("res.txt");
    resFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
    QTextStream resStm(&resFile);

    QFile dxFile("dxdy.txt");
    dxFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
    QTextStream dxStm(&dxFile);

    QFile deFile("de.txt");
    deFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
    QTextStream deStm(&deFile);





    solSys = new Everhardt(N, eparam->NCLASS, eparam->NOR, eparam->NI, eparam->LL, eparam->XL);


    char *astr = new char[256];
    QFile mpcFile("./mpc.txt");
    mpcFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream mpcStm(&mpcFile);

    QFile mpcFileM("./mpc_moody.txt");
    mpcFileM.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream mpcStmM(&mpcFileM);



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

            dxmFile.setFileName("dxdy_moody.txt");
            dxmFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
            dxmStm.setDevice(&dxmFile);

            resmFile.setFileName("res_moody.txt");
            resmFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
            resmStm.setDevice(&resmFile);
        //}
    }


   // p = 0;
    for(i=0; i<nofzbody; i++)
    {
        //mass[i] = pList[i]->mass;
//        if(pList.at(i)->interactionPermission==Advisor::interactNONE) continue;

        name = QString(iList[i]->name.data());

        if(useEPM) plaNum = epm_planet_num(name);
        else plaNum = planet_num(name.toAscii().data());
        //if(plaNum==10) continue;
        p = i*3;

        X[p] = iList[i]->x;
        X[p+1] = iList[i]->y;
        X[p+2] = iList[i]->z;
        V[p] = iList[i]->xd;
        V[p+1] = iList[i]->yd;
        V[p+2] = iList[i]->zd;


/*
        if(useMoody)
        {
            if(getMopName(mState, mItem, name)!=-1)
            {
                //mItem = mState->getMopItem(i);
                Xm[i*3] = mItem.x/AUKM/1000;
                Xm[i*3+1] = mItem.y/AUKM/1000;
                Xm[i*3+2] = mItem.z/AUKM/1000;
                Vm[i*3] = mItem.xd*SECINDAY/1000/AUKM;
                Vm[i*3+1] = mItem.yd*SECINDAY/1000/AUKM;
                Vm[i*3+2] = mItem.zd*SECINDAY/1000/AUKM;

                //saveResultsM(t0, Xm, Vm, X0, V0, i*3, name, resmStm, dxmStm);
            }
        }
*/
        if(plaNum!=-1)
        {
            if(useEPM)
            {
                status = calc_EPM(plaNum, centr_num, (int)t0, t0 - (int)t0, &X0[p], &V0[p]);
                 if(!status)
                 {
                     qDebug() << QString("error EPM\n");
                     return 1;
                 }
            }
            else
            {
                //nbody->detR(&X0[p+0], &X0[p+1], &X0[p+2], t0, plaNum, 0, CENTER, SK);
                //nbody->detR(&V0[p+0], &V0[p+1], &V0[p+2], t0, plaNum, 1, CENTER, SK);
                nbody->detState(&X0[p+0], &X0[p+1], &X0[p+2], &V0[p+0], &V0[p+1], &V0[p+2], t0, plaNum, CENTER, SK);
            }

            saveResults(t0, X, V, X0, V0, p, name, resStm, dxStm, deStm);

            //if(useMoody) saveResultsM(t0, Xm, Vm, X0, V0, i*3, name, resmStm, dxmStm);


        }
        else
        {
            if(initMpc) break;
            mCat.GetRecName(name.simplified().toAscii().data());

            TDB2UTC(t0, &jdUTC);

            if(useEPM)
            {
                status = calc_EPM(EARTH, centr_num, (int)t0, t0 - (int)t0, XE0, VE0);
                 if(!status)
                 {
                     qDebug() << QString("error EPM\n");
                     return 1;
                 }
            }
            else nbody->detState(&XE0[0], &XE0[1], &XE0[2], &VE0[0], &VE0[1], &VE0[2], t0, GEOCENTR_NUM, CENTER, SK);

            qDebug() << QString("%1: %2\t%3\t%4\t%5\t%6\t%7\n").arg(TF, 13, 'f', 4).arg(XE0[0], 22, 'e', 15).arg(XE0[1], 22, 'e', 15).arg(XE0[2], 22, 'e', 15).arg(VE0[0], 22, 'e', 15).arg(VE0[1], 22, 'e', 15).arg(VE0[2], 22, 'e', 15);

            if(useMiriade)
            {
                outerArguments << QString("-name=\"%1\"").arg(name.simplified());
                sJD = QString("%1").arg(jdUTC, 11, 'f',7);
                outerArguments << QString("-ep=%1").arg(sJD);

                outerProcess.setWorkingDirectory(miriadeProcData.folder);
                outerProcess.setProcessChannelMode(QProcess::MergedChannels);
                outerProcess.setReadChannel(QProcess::StandardOutput);

                outerProcess.start(miriadeProcData.name, outerArguments);

                if(!outerProcess.waitForFinished(miriadeProcData.waitTime))
                {
                    qDebug() << "\nmiriadeProc finish error\n";
                    break;
                }

                QTextStream objStream(outerProcess.readAllStandardOutput());

                while (!objStream.atEnd())
                {
                    objDataStr = objStream.readLine();
                    qDebug() << QString("objDataStr: %1").arg(objDataStr);
                    if(objDataStr.size()<1) continue;
                    if(objDataStr.at(0)=='#') continue;

                    resSL =  objDataStr.split(" ", QString::SkipEmptyParts);
                    if(resSL.size()<10) continue;
                    X0[p+0] = resSL.at(1).toDouble();
                    X0[p+1] = resSL.at(2).toDouble();
                    X0[p+2] = resSL.at(3).toDouble();
                    V0[p+0] = resSL.at(8).toDouble();
                    V0[p+1] = resSL.at(9).toDouble();
                    V0[p+2] = resSL.at(10).toDouble();

                    X0[p+0] += XE0[0];
                    X0[p+1] += XE0[1];
                    X0[p+2] += XE0[2];
                    V0[p+0] += VE0[0];
                    V0[p+1] += VE0[1];
                    V0[p+2] += VE0[2];


                }
                saveResults(t0, X, V, X0, V0, p, name, resStm, dxStm, deStm);
            }
            else saveResults(t0, X, V, NULL, NULL, p, name, resStm, dxStm, deStm);

            detRDnumGC(&ra, &de, X[p], X[p+1], X[p+2], XE0[0], XE0[1], XE0[2], 0, 0, 0);
            //detRDnumGC(&ra0, &de0, X0[i], X0[i+1], X0[i+2], X[9], X[10], X[11], 0, 0, 0);


            //ra = ra - ra0;
            //de = de - de0;
            mrec.r = ra;
            mrec.d = de;
            mrec.eJD = jdUTC;
            mrec.num = 1;
            mCat.record->getNumStr(mrec.head->Snum);
            //strcpy(mrec.head->Snum, mCat.record->number);
            mrec.tail->set_numOfObs("500");
            mrec.toString(astr);

            mpcStm << astr << "\n";


/*
            if(useMoody)
            {
                detRDnumGC(&ra, &de, Xm[p], Xm[p+1], Xm[p+2], XE0[0], XE0[1], XE0[2], 0, 0, 0);
                //detRDnumGC(&ra0, &de0, X0[i], X0[i+1], X0[i+2], X[9], X[10], X[11], 0, 0, 0);

                //TDB2UTC(t0, &jdUTC);
                //ra = ra - ra0;
                //de = de - de0;
                mrec.r = ra;
                mrec.d = de;
                mrec.eJD = jdUTC;
                mrec.num = 1;
                mCat.record->getNumStr(mrec.head->Snum);
                //strcpy(mrec.head->Snum, mCat.record->number);
                mrec.tail->set_numOfObs("500");
                mrec.toString(astr);

                mpcStmM << astr << "\n";
            }*/
        }



/*        else
        {
            if(useMiriade)
            {
                resMiri = getMiriadeObject(&mpcObj, jd2mjd(t0), name, miriadeProcData, objType);
                if(!resMiri)
                {

                }
            }
        }
*/

/*


*/


/*
        Ri = sqrt(X[i*3+0]*X[i*3+0] + X[i*3+1]*X[i*3+1] + X[i*3+2]*X[i*3+2]);
        Vi = sqrt(V[i*3+0]*V[i*3+0] + V[i*3+1]*V[i*3+1] + V[i*3+2]*V[i*3+2])*AUKM/86400.0;
        resStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|1\n").arg(t0, 13, 'f', 4).arg(X[i*3], 13, 'f', 9).arg(X[i*3+1], 13, 'f', 9).arg(X[i*3+2], 13, 'f', 9).arg(Ri, 13, 'f', 9).arg(V[i*3], 13, 'f', 9).arg(V[i*3+1], 13, 'f', 9).arg(V[i*3+2], 13, 'f', 9).arg(pList[i]->name.data());



        r[0] = X[i*3]-X0[i*3];
        r[1] = X[i*3+1]-X0[i*3+1];
        r[2] = X[i*3+2]-X0[i*3+2];

        Ri = sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);

        v[0] = V[i*3]-V0[i*3];
        v[1] = V[i*3+1]-V0[i*3+1];
        v[2] = V[i*3+2]-V0[i*3+2];

        dxStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9\n").arg(t0, 12, 'f', 4).arg(r[0], 13, 'f', 9).arg(r[1], 13, 'f', 9).arg(r[2], 13, 'f', 9).arg(Ri, 13, 'f', 9).arg(v[0], 13, 'f', 9).arg(v[1], 13, 'f', 9).arg(v[2], 13, 'f', 9).arg(pList[i]->name.data());

        Ri = sqrt(X0[i*3+0]*X0[i*3+0] + X0[i*3+1]*X0[i*3+1] + X0[i*3+2]*r[i*3+2]);

        deStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|1\n").arg(t0, 12, 'f', 4).arg(X0[i*3], 13, 'f', 9).arg(X0[i*3+1], 13, 'f', 9).arg(X0[i*3+2], 13, 'f', 9).arg(Ri, 13, 'f', 9).arg(V0[i*3], 13, 'f', 9).arg(V0[i*3+1], 13, 'f', 9).arg(V0[i*3+2], 13, 'f', 9).arg(pList[i]->name.data());
        */
    }

    //if(useMoody) mState = mFile->readState();
/*
    CM_int(CM, X, V);
    qDebug() << QString("CM: %1\t%2\t%3\n").arg(CM[0]).arg(CM[1]).arg(CM[2]);
    S_int(S, X, V);
    qDebug() << QString("S: %1\t%2\t%3\n").arg(S[0]).arg(S[1]).arg(S[2]);
    LF_int(&LF, X, V);
    qDebug() << QString("LF: %1\n").arg(LF);

    CM_int(CM0, X0, V0);
    qDebug() << QString("CM0: %1\t%2\t%3\n").arg(CM0[0]).arg(CM0[1]).arg(CM0[2]);
    S_int(S0, X0, V0);
    qDebug() << QString("S0: %1\t%2\t%3\n").arg(S0[0]).arg(S0[1]).arg(S0[2]);
    LF_int(&LF0, X0, V0);
    qDebug() << QString("LF0: %1\n").arg(LF0);
*/


//    for(ti=t0; ti<t1; ti+=dt)
    double *P = new double[9];
    double *ssb, *ssbv, mui, mui1, muis;
    double xt, yt, zt;
    ssb= new double[3];
    ssbv= new double[3];
    TF = t0;
    int jday;
    double pday;
/*
    for(teloi=0; teloi<nofzbody; teloi++)
    {
        i=teloi*3;
        V[i] = -V[i];
        V[i+1] = -V[i+1];
        V[i+2] = -V[i+2];
    }
*/
    do
    {
        for(nt=0; nt<nstep; nt++)
        {


            qDebug() << QString("\njd: %1\ntime: %2\n").arg(TF, 12, 'f', 4).arg(getStrFromDATEOBS(getDATEOBSfromMJD(jd2mjd(TF)), ":", 0, 3));

            TI = TF;
            TF += dt;


            jday = int(TF);
            pday = TF - jday;
            qDebug() << QString("jday: %1\tpday: %2\n").arg(jday).arg(pday);

            solSys->rada27(X, V, 0, fabs(dt));

            /*
            ssb[0] = 0;
            ssb[1] = 0;
            ssb[2] = 0;

            CM_int(CM, X, V);
            qDebug() << QString("CM: %1\t%2\t%3\n").arg(CM[0]).arg(CM[1]).arg(CM[2]);
            S_int(S, X, V);
            qDebug() << QString("S: %1\t%2\t%3\n").arg(S[0]).arg(S[1]).arg(S[2]);
            LF_int(&LF, X, V);
            qDebug() << QString("LF: %1\n").arg(LF);
            */
/*
            if(useEPM)
            {
                status = calc_EPM(3, 13, (int)TF, TF - (int)TF, emb, embv);
                 if(!status)
                 {
                     qDebug() << QString("error EPM\n");
                     return 1;
                 }
            }
    /*
            nbody->detR(&ssb[0], &ssb[1], &ssb[2], TF, SS_BARY, 0, CENTER, SK);
            nbody->detR(&ssbv[0], &ssbv[1], &ssbv[2], TF, SS_BARY, 1, CENTER, SK);
            qDebug() << QString("ssb: %1\t%2\t%3\n").arg(ssb[0]).arg(ssb[1]).arg(ssb[2]);
    */
 /*           muis = 0;

            for(teloi=0, i=0; teloi<nofzbody; teloi++, i+=3)
            {
                mui1=0.0;

                for(teloj=0, j=0; teloj<nofzbody; teloj++, j+=3)
                {
                    if(teloi!=teloj) mui1+=pow(pList[teloj]->mass, -1.0)/sqrt(pow(X[j+0] - X[i+0], 2) + pow(X[j+1] - X[i+1], 2) + pow(X[j+2] - X[i+2], 2));
                }
                mui1 *= 1.0/(2.0*CAU*CAU);
                mui1 = 1.0 + (V[i]*V[i]+V[i+1]*V[i+1]+V[i+2]*V[i+2])/(2.0*CAU*CAU) - mui1;

                mui = pow(pList[teloi]->mass, -1.0)*mui1;

                ssb[0] += mui*X[i];
                ssb[1] += mui*X[i+1];
                ssb[2] += mui*X[i+2];
                muis += mui;
            }

            ssb[0] /= muis;
            ssb[1] /= muis;
            ssb[2] /= muis;
*/
            //i=0;
            for(teloi=0; teloi<iList.size(); teloi++)
            {
                //if(pList.at(teloi)->interactionPermission==Advisor::interactNONE) continue;
                name = QString(iList[teloi]->name.data());
                if(useEPM) plaNum = epm_planet_num(name);
                else plaNum = planet_num(name.toAscii().data());
    /*
                X[i]+=ssb[0];
                X[i+1]+=ssb[1];
                X[i+2]+=ssb[2];
    */
                i = teloi*3;
                if(useMoody)
                {
                    if(getMopName(mState, mItem, name)!=-1)
                    {
                        //mItem = mState->getMopItem(teloi);

                        Xm[i] = mItem.x/AUKM/1000;
                        Xm[i+1] = mItem.y/AUKM/1000;
                        Xm[i+2] = mItem.z/AUKM/1000;
                        Vm[i] = mItem.xd*SECINDAY/1000/AUKM;
                        Vm[i+1] = mItem.yd*SECINDAY/1000/AUKM;
                        Vm[i+2] = mItem.zd*SECINDAY/1000/AUKM;

                        //saveResultsM(TF-t0, Xm, Vm, X, V, i, name, resmStm, dxmStm);


                    }
                }


                if(plaNum!=-1)
                {

                    if(useEPM)
                    {
                        status = calc_EPM(plaNum, centr_num, jday, pday, &X0[i], &V0[i]);
                         if(!status)
                         {
                             qDebug() << QString("error EPM\n");
                             return 1;
                         }
                         /*if(plaNum==3)
                         {
                             for(int k=0; k<3; k++)
                             {
                                 X0[i+0] -= emb[0];
                                 X0[i+1] -= emb[1];
                                 X0[i+2] -= emb[2];
                                 V0[i+0] -= embv[0];
                                 V0[i+1] -= embv[1];
                                 V0[i+2] -= embv[2];
                             }
                         }*/
                    }
                    else
                    {
                        //nbody->detR(&X0[i+0], &X0[i+1], &X0[i+2], TF, plaNum, 0, CENTER, SK);
                        //nbody->detR(&V0[i+0], &V0[i+1], &V0[i+2], TF, plaNum, 1, CENTER, SK);
                        nbody->detState(&X0[i+0], &X0[i+1], &X0[i+2], &V0[i+0], &V0[i+1], &V0[i+2], TF, plaNum, CENTER, SK);
                    }
/*

                    X[i]-=ssb[0];
                    X[i+1]-=ssb[1];
                    X[i+2]-=ssb[2];
                    V[i]-=ssbv[0];
                    V[i+1]-=ssbv[1];
                    V[i+2]-=ssbv[2];
*/
                    /*
                    if(plaNum==EARTH_NUM)
                    {
                        T = (TF - 2451545)/36525.0;
                        ksi = mas2rad((2306.2181*T + 0.30188*T*T + 0.017998*T*T*T)*1000);
                        teta = mas2rad((2004.3109*T - 0.42665*T*T - 0.041833*T*T*T)*1000);
                        zet = mas2rad((2306.2181*T + 1.09468*T*T + 0.018203*T*T*T)*1000);
/*
                        xt = (-sin(ksi)*sin(zet) + cos(ksi)*cos(zet)*cos(teta))*X[i] + (-cos(ksi)*sin(zet) - sin(ksi)*cos(zet)*cos(teta))*X[i+1] + (-cos(zet)*sin(teta))*X[i+2];
                        yt = (sin(ksi)*cos(zet) + cos(ksi)*sin(zet)*cos(teta))*X[i] + (cos(ksi)*cos(zet) - sin(ksi)*sin(zet)*cos(teta))*X[i+1] + (-sin(zet)*sin(teta))*X[i+2];
                        zt = (cos(ksi)*sin(teta))*X[i] + (-sin(ksi)*sin(teta))*X[i+1] + (cos(teta))*X[i+2];
  /
                        P[0] = (-sin(ksi)*sin(zet) + cos(ksi)*cos(zet)*cos(teta));
                        P[1] = (-cos(ksi)*sin(zet) - sin(ksi)*cos(zet)*cos(teta));
                        P[2] = (-cos(zet)*sin(teta));
                        P[3] = (sin(ksi)*cos(zet) + cos(ksi)*sin(zet)*cos(teta));
                        P[4] = (cos(ksi)*cos(zet) - sin(ksi)*sin(zet)*cos(teta));
                        P[5] = (-sin(zet)*sin(teta));
                        P[6] = (cos(ksi)*sin(teta));
                        P[7] = (-sin(ksi)*sin(teta));
                        P[8] = (cos(teta));

                        xt = P[0]*X[i] + P[1]*X[i+1] + P[2]*X[i+2];
                        yt = P[3]*X[i] + P[4]*X[i+1] + P[5]*X[i+2];
                        zt = P[6]*X[i] + P[7]*X[i+1] + P[8]*X[i+2];
 /*
                        xt = P[0]*X[i] + P[3]*X[i+1] + P[6]*X[i+2];
                        yt = P[1]*X[i] + P[4]*X[i+1] + P[7]*X[i+2];
                        zt = P[2]*X[i] + P[5]*X[i+1] + P[8]*X[i+2];
/
                        qDebug() << QString("prec: %1").arg(sqrt(pow(xt-X[i], 2.0)+pow(yt-X[i+1], 2.0)+pow(zt-X[i+2], 2.0)));
                        X[i] = xt;
                        X[i+1] = yt;
                        X[i+2] = zt;
                    }
*/

                    //if(pList.at(teloi)->interactionPermission!=Advisor::interactNONE)
                    //{
                        saveResults(TF, X, V, X0, V0, i, name, resStm, dxStm, deStm);
                    //}
                    /*else
                    {

                        X[i]=X0[i];
                        X[i+1]=X0[i+1];
                        X[i+2]=X0[i+2];

                        V[i]=V0[i];
                        V[i+1]=V0[i+1];
                        V[i+2]=V0[i+2];

                    }
                    */
                }
                else// if(pList.at(teloi)->interactionPermission!=Advisor::interactNONE)
                {


                    if(initMpc) break;
                    if(mCat.GetRecName(name.simplified().toAscii().data())) break;

                    TDB2UTC(TF, &jdUTC);

                    if(useEPM)
                    {
                        status = calc_EPM(EARTH, centr_num, (int)TF, TF - (int)TF, XE0, VE0);
                         if(!status)
                         {
                             qDebug() << QString("error EPM\n");
                             return 1;
                         }
                    }
                    else nbody->detState(&XE0[0], &XE0[1], &XE0[2], &VE0[0], &VE0[1], &VE0[2], TF, GEOCENTR_NUM, CENTER, SK);
                    //nbody->detState(&XE0[0], &XE0[1], &XE0[2], &VE0[0], &VE0[1], &VE0[2], TF, GEOCENTR_NUM, CENTER, SK);
                    qDebug() << QString("%1: %2\t%3\t%4\t%5\t%6\t%7\n").arg(TF, 13, 'f', 4).arg(XE0[0], 22, 'e', 15).arg(XE0[1], 22, 'e', 15).arg(XE0[2], 22, 'e', 15).arg(VE0[0], 22, 'e', 15).arg(VE0[1], 22, 'e', 15).arg(VE0[2], 22, 'e', 15);

                    if(useMiriade)
                    {
                        outerArguments << QString("-name=\"%1\"").arg(name.simplified());
                        sJD = QString("%1").arg(jdUTC, 11, 'f',7);
                        outerArguments << QString("-ep=%1").arg(sJD);

                        outerProcess.setWorkingDirectory(miriadeProcData.folder);
                        outerProcess.setProcessChannelMode(QProcess::MergedChannels);
                        outerProcess.setReadChannel(QProcess::StandardOutput);

                        outerProcess.start(miriadeProcData.name, outerArguments);

                        if(!outerProcess.waitForFinished(miriadeProcData.waitTime))
                        {
                            qDebug() << "\nmiriadeProc finish error\n";
                            break;
                        }

                        QTextStream objStream(outerProcess.readAllStandardOutput());

                        while (!objStream.atEnd())
                        {
                            objDataStr = objStream.readLine();
                            qDebug() << QString("objDataStr: %1").arg(objDataStr);
                            if(objDataStr.size()<1) continue;
                            if(objDataStr.at(0)=='#') continue;

                            resSL =  objDataStr.split(" ", QString::SkipEmptyParts);
                            if(resSL.size()<10) continue;
                            X0[i+0] = resSL.at(1).toDouble();
                            X0[i+1] = resSL.at(2).toDouble();
                            X0[i+2] = resSL.at(3).toDouble();
                            V0[i+0] = resSL.at(8).toDouble();
                            V0[i+1] = resSL.at(9).toDouble();
                            V0[i+2] = resSL.at(10).toDouble();

                            X0[i+0] += XE0[0];
                            X0[i+1] += XE0[1];
                            X0[i+2] += XE0[2];
                            V0[i+0] += VE0[0];
                            V0[i+1] += VE0[1];
                            V0[i+2] += VE0[2];


                        }
                        saveResults(TF, X, V, X0, V0, i, name, resStm, dxStm, deStm);
                    }
                    else saveResults(TF, X, V, NULL, NULL, i, name, resStm, dxStm, deStm);

                    detRDnumGC(&ra, &de, X[i], X[i+1], X[i+2], XE0[0], XE0[1], XE0[2], 0, 0, 0);
                    //detRDnumGC(&ra0, &de0, X0[i], X0[i+1], X0[i+2], X[9], X[10], X[11], 0, 0, 0);


                    //ra = ra - ra0;
                    //de = de - de0;
                    mrec.r = ra;
                    mrec.d = de;
                    mrec.eJD = jdUTC;
                    mrec.num = 1;
                    mCat.record->getNumStr(mrec.head->Snum);
                    //strcpy(, mCat.record->getNumStr(>number);
                    mrec.tail->set_numOfObs("500");
                    mrec.toString(astr);

                    mpcStm << astr << "\n";

                    if(useMoody)
                    {
                        detRDnumGC(&ra, &de, Xm[i], Xm[i+1], Xm[i+2], Xm[6], Xm[7], Xm[8], 0, 0, 0);
                        //detRDnumGC(&ra0, &de0, X0[i], X0[i+1], X0[i+2], X[9], X[10], X[11], 0, 0, 0);

                        //TDB2UTC(t0, &jdUTC);
                        //ra = ra - ra0;
                        //de = de - de0;
                        mrec.r = ra;
                        mrec.d = de;
                        mrec.eJD = jdUTC;
                        mrec.num = 1;
                        mCat.record->getNumStr(mrec.head->Snum);
                        //strcpy(mrec.head->Snum, mCat.record->number);
                        mrec.tail->set_numOfObs("500");
                        mrec.toString(astr);

                        mpcStmM << astr << "\n";
                    }

                    saveResults(TF, X, V, NULL, NULL, i, name, resStm, dxStm, deStm);
                    //qDebug() << QString("OC %1: %2\t%3\n").arg(name).arg(rad2mas(ra)).arg(rad2mas(de));
                }

                if(useMoody) saveResultsM(TF, Xm, Vm, X0, V0, i, name, resmStm, dxmStm);



                //i+=3;

    /*
                Ri = sqrt(X[i+0]*X[i+0] + X[i+1]*X[i+1] + X[i+2]*X[i+2]);
                Vi = sqrt(V[i+0]*V[i+0] + V[i+1]*V[i+1] + V[i+2]*V[i+2])*AUKM/86400.0;
                resStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|1\n").arg(TF, 13, 'f', 4).arg(X[i], 13, 'f', 9).arg(X[i+1], 13, 'f', 9).arg(X[i+2], 13, 'f', 9).arg(Ri, 13, 'f', 9).arg(V[i], 13, 'f', 9).arg(V[i+1], 13, 'f', 9).arg(V[i+2], 13, 'f', 9).arg(pList[teloi]->name.data());



                r[0] = X[i]-X0[i];
                r[1] = X[i+1]-X0[i+1];
                r[2] = X[i+2]-X0[i+2];

                Ri = sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);

                v[0] = V[i]-V0[i];
                v[1] = V[i+1]-V0[i+1];
                v[2] = V[i+2]-V0[i+2];

                dxStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9\n").arg(TF, 12, 'f', 4).arg(r[0], 13, 'f', 9).arg(r[1], 13, 'f', 9).arg(r[2], 13, 'f', 9).arg(Ri, 13, 'f', 9).arg(v[0], 13, 'f', 9).arg(v[1], 13, 'f', 9).arg(v[2], 13, 'f', 9).arg(pList[teloi]->name.data());

                Ri = sqrt(X0[i+0]*X0[i+0] + X0[i+1]*X0[i+1] + X0[i+2]*r[i+2]);

                deStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|1\n").arg(TF, 12, 'f', 4).arg(X0[i], 13, 'f', 9).arg(X0[i+1], 13, 'f', 9).arg(X0[i+2], 13, 'f', 9).arg(Ri, 13, 'f', 9).arg(V0[i], 13, 'f', 9).arg(V0[i+1], 13, 'f', 9).arg(V0[i+2], 13, 'f', 9).arg(pList[teloi]->name.data());
                */
            }
/*
            for(i=0; i<pList.size(); i++)
            {
                //mass[i] = pList[i]->mass;
                if(pList.at(i)->interactionPermission==Advisor::interactNONE) continue;

                name = QString(pList[i]->name.data());

                if(useEPM) plaNum = epm_planet_num(name);
                else plaNum = planet_num(name.toAscii().data());
                //if(plaNum==10) continue;
/*
                X[p] = pList[i]->x;
                X[p+1] = pList[i]->y;
                X[p+2] = pList[i]->z;
                V[p] = pList[i]->xd;
                V[p+1] = pList[i]->yd;
                V[p+2] = pList[i]->zd;
/

                p = i*3;


                if(plaNum!=-1)
                {
                    if(useEPM)
                    {
                        status = calc_EPM(plaNum, centr_num, (int)TF, TF - (int)TF, &X[p], &V[p]);
                         if(!status)
                         {
                             qDebug() << QString("error EPM\n");
                             return 1;
                         }
                    }
                    else
                    {
                        nbody->detR(&X[p+0], &X[p+1], &X[p+2], TF, plaNum, 0, CENTER, SK);
                        nbody->detR(&V[p+0], &V[p+1], &V[p+2], TF, plaNum, 1, CENTER, SK);
                    }

                    //saveResults(t0-t0, X, V, X0, V0, p, name, resStm, dxStm, deStm);
                }

                //p+=3;
            }

*/

            CM_int(CM0, X0, V0);
            qDebug() << QString("CM0: %1\t%2\t%3\n").arg(CM0[0]).arg(CM0[1]).arg(CM0[2]);
            S_int(S0, X0, V0);
            qDebug() << QString("S0: %1\t%2\t%3\n").arg(S0[0]).arg(S0[1]).arg(S0[2]);
            LF_int(&LF0, X0, V0);
            qDebug() << QString("LF0: %1\n").arg(LF0);

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



    resFile.close();

    dxFile.close();
    deFile.close();

    mpcFile.close();
    mpcFileM.close();

    if(useMoody)
    {
        resmFile.close();
        dxmFile.close();
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
