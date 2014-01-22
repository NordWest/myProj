#include <QCoreApplication>

#include <dele.h>
#include "./../../libs/orbit.h"
#include <astro.h>
#include "./../../libs/comfunc.h"
#include "./../../libs/redStat.h"
#include "./../../libs/mpcs.h"
#include "./../../libs/ephem_util.h"
#include "./../../libs/calc_epm.h"
#include "./../../libs/mpccat.h"
#include "./../../libs/mpcfile.h"
#include <SpiceUsr.h>
#include <rada.h>

#define OMPLIB

#include <iostream>
#include <iomanip>

#include "./../../libs/moody/moody.h"
#include "./../../libs/moody/capsule/capsuleBase/particle/Particle.h"
#include "./../../libs/moody/capsule/Capsule.h"
#include "./../../libs/moody/capsule/capsuleBase/CapsuleBase.h"
#include "./../../libs/myDomMoody.h"

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
double *mass;
ever_params *eparam;
int *interact_permitions;

struct eqObjRec
{
    QString objName;
    eqFile* eq_list;
    void sortByTime()
    {
        eq_list->sortOClist();
    }
    int size()
    {
        return eq_list->ocList.size();
    }

};


struct tState
{
    double t;

};

struct bodyStates
{
    QString name;
    double mass;
    void addState(double *X, double *V);
    QList <stateData*> states;
};

void bodyStates::addState(double *X, double *V)
{
    stateData* st = new stateData;

    st->Position[0] = X[0];
    st->Position[1] = X[1];
    st->Position[2] = X[2];

    st->Velocity[0] = V[0];
    st->Velocity[1] = V[1];
    st->Velocity[2] = V[2];
}

struct bodyStateList
{
    QList <bodyStates*> bsList;
    int addParticle(bodyStates* par);
    int size();

};

int bodyStateList::addParticle(ParticleStruct* par)
{
    QString bodyName = par->name.data();

    int np = 1;
    stateData *stT;
    stT = new stateData;
    stT->Position[0] = par->x;
    stT->Position[1] = par->y;
    stT->Position[2] = par->z;
    stT->Velocity[0] = par->xd;
    stT->Velocity[1] = par->yd;
    stT->Velocity[2] = par->zd;

    for(int i=0; i<bsList.size(); i++)
    {
        if(QString().compare(bodyName, bsList.at(i)->name)==0)
        {
            np = 0;
            bsList.at(i)->states << stT;
            break;
        }
    }

    bodyStates* nbd;

    if(np)
    {
        nbd = new bodyStates;
        nbd->name = bodyName;
        nbd->mass = par->mass;
        nbd->states << stT;
        bsList << nbd;
    }
}

int bodyStateList::size()
{
    return(bsList.size());
}

int doNbody(double tf, double dt, double nstep, bodyStateList &bpList);


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    if(argc<2) exit(1);

    int SK, CENTER;
    procData miriadeProcData;
    int i, j, sz;
    dele *nbody;
    int centr_num;
    int status;

    QString objDataStr;
    QString name, sName;
    int plaNum;

    double *X, *V;
    double *X0, *V0;
    QVector <double*> xVect, vVect;
    double Xsun[3], Vsun[3];

    QStringList outerArguments, resSL;
    QProcess outerProcess;
    QString sJD;

    double dist, vel, jdUTC, jdTDB, jdTDT, et;
    double time0, time1, dtime, timestep;
    int nstep;
    mpccat mCat;
    mpcrec mRec;
    orbit orbRec;

    SpiceDouble             state [6];
    SpiceDouble             lt;
    SpiceChar             * corr;
    SpiceChar             * ref;
    ref = new SpiceChar[256];
    corr = new SpiceChar[256];

    QString cfg_file, part_file, mop_file;

    bodyStateList bsList;

    QList <ParticleStruct*> pList;
//Settings
    QSettings *sett = new QSettings("./nb.ini", QSettings::IniFormat);

    QString jplFile = sett->value("general/jplFile", "./../../data/cats/binp1940_2020.405").toString();
    QString epmDir = sett->value("general/epmDir", "./").toString();
    QString obsFile = sett->value("general/obsFile", "./../../data/cats/Obs.txt").toString();
    QString obsCode = sett->value("general/obsCode", "500").toString();
    QString mpcCatFile = sett->value("general/mpcCatFile", "mocorb.txt").toString();
    QString confFile = sett->value("general/confFile", "particles.xml").toString();
    QString moodyDir = QDir(sett->value("general/moodyDir", "./testProject").toString()).absolutePath();

    int useMoody = sett->value("general/useMoody", 0).toInt();
    int massType = sett->value("general/massType", 0).toInt();
    int useEPM = sett->value("general/useEPM", 0).toInt();

    SK = sett->value("general/sk", 0).toInt();
    CENTER = sett->value("general/center", 0).toInt();

    int bigType = sett->value("general/bigType", 0).toInt();
    int smlType = sett->value("general/smlType", 0).toInt();

    time0 = sett->value("time/time0", 2455201.0).toDouble();
    dtime = sett->value("time/dtime", 0).toDouble();
    nstep = sett->value("time/nstep", 0).toInt();
    timestep = sett->value("time/timestep", 0).toDouble();

    miriadeProcData.name = sett->value("miriadeProcData/name", "./mpeph.exe").toString();
    miriadeProcData.folder = sett->value("miriadeProcData/folder", "./").toString();
    miriadeProcData.waitTime = sett->value("miriadeProcData/waitTime", -1).toInt();
    if(miriadeProcData.waitTime>0) miriadeProcData.waitTime *= 1000;

    /*moody
    cfg_file = sett->value("moody/cfg_file").toString();
    part_file = sett->value("moody/part_file").toString();
    mop_file = sett->value("moody/mop_file").toString();*/
    cfg_file = QString("%1/cfg/cfg.xml").arg(moodyDir);
    part_file = QString("%1/particles/particles.xml").arg(moodyDir);
    mop_file = QString("%1/result/moodyProject.mop").arg(moodyDir);

    //SPICE
    QString bspName = sett->value("SPICE/bspName", "./de421.bsp").toString();
    QString leapName = sett->value("SPICE/leapName", "./naif0010.tls").toString();
    sprintf(ref,"%s", sett->value("SPICE/ref", "J2000").toString().toAscii().data());
    sprintf(corr,"%s", sett->value("general/corr", "LT").toString().toAscii().data());
///////////

    int jday = (int)time0;
    double pday = time0 - jday;

    time1 = time0+dtime*nstep;

    if(mCat.init(mpcCatFile.toAscii().data())) return 1;

//Init MPC file

    QString iniMpcFile = QString(argv[1]);

    mpcFile mpc_file;
    mpcFile mpcfTemp;
    mpcRec* mpc_rec;
    mpcObjRec* moRec;
    mpc_file.init(iniMpcFile);

    mpcObjList m_objList;

    //int mNum = mpc_file.size();//.nstr();
    double tBeg, tEnd;
    tBeg = 9999999999;
    tEnd = 0;
    QString tStr, objName;
    char *tname = new char[256];

    QList <eqObjRec*> eqo_list;
    eqObjRec* eqoTemp;
    eqFile* eqTemp;
    ocRec *ocTemp;
    int nObj;
    QList <orbit*> orbList;
    orbit* tOrb;
    int objNum;
    double T0, T1, DT;
    double TI, TF;

    for(i=0;i<mNum;i++)
    {
        mpc_rec = mpc_file.at(i);
        mpc_rec->getMpNumber(tStr);

        tStr.replace(" ", "0");


        sprintf(tname, "%s", tStr.toAscii().data());

        //qDebug() << QString("tStr: %1\n").arg(tname);

        if(mCat.GetProvDest(tname))
        {
            mpc_rec->getProvDest(tStr);
            if(mCat.GetProvDest(tStr.toAscii().data()))
            {
                qDebug() << QString("not found: %1\n").arg(mpc_rec->toStr());
                continue;
            }
        }
        objName = QString(mCat.record->name).simplified();



        //mCat.GetRecName(objName.toAscii().data());

        //qDebug() << QString("mCat record: %1").arg(mCat.str);

        m_objList.addMpcRec(mpc_rec, objName);
        //if(mpc_rec->mjd()<tBeg)
    }

    m_objList.sortByTime();

    for(i=0; i<m_objList.size(); i++)
    {
        moRec = m_objList.obj_list.at(i);
        qDebug() << QString("%1: %2").arg(moRec->objName).arg(moRec->size());

        if(!mCat.GetRecName(moRec->objName.toAscii().data()))
        {
            tOrb = new orbit;
            tOrb->get(&mCat);
            orbList << tOrb;
        }

        nObj=1;
        for(j=0;j<eqo_list.size(); j++)
        {
            if(QString().compare(moRec->objName, eqo_list.at(j)->objName)==0)
            {
                nObj=0;
                eqoTemp = eqo_list.at(j);
                break;
            }
        }
        if(nObj)
        {
            eqoTemp = new eqObjRec;
            eqoTemp->objName = moRec->objName;
            eqoTemp->eq_list = new eqFile;
            eqo_list << eqoTemp;
        }

        for(j=0; j<moRec->size(); j++)
        {

            //qDebug() << moRec->mpc_list.at(j)->toStr();
           ocTemp = new ocRec;
           ocTemp->catName = "MPCCAT";
           ocTemp->name = eqoTemp->objName;
           ocTemp->ra = moRec->mpc_list.at(j)->ra();
           ocTemp->de = moRec->mpc_list.at(j)->dec();
           ocTemp->mag0 = moRec->mpc_list.at(j)->magn();
           ocTemp->MJday = moRec->mpc_list.at(j)->mjd();

           eqoTemp->eq_list->ocList << ocTemp;
        }

        /*
        for(j=0; j<eqoTemp->eq_list->ocList.size(); j++)
        {
            eqoTemp->eq_list->ocList.at(j)->rec2s_short(&tStr);
            qDebug() << tStr;
        }*/

        if(moRec->mpc_list.at(0)->mjd()<tBeg) tBeg = moRec->mpc_list.at(0)->mjd();
        if(moRec->mpc_list.at(moRec->mpc_list.size()-1)->mjd()>tEnd) tEnd = moRec->mpc_list.at(moRec->mpc_list.size()-1)->mjd();


    }

    tBeg = UTC2TDB(tBeg);
    tEnd = UTC2TDB(tEnd);
    qDebug() << QString("Time span: %1 - %2\n").arg(tBeg, 12, 'f', 6).arg(tEnd, 12, 'f', 6);

    tBeg = floor(tBeg-1);
    tEnd = floor(tEnd+1);
    qDebug() << QString("Time span: %1 - %2\n").arg(tBeg, 12, 'f', 6).arg(tEnd, 12, 'f', 6);



    objNum = orbList.size();

    qDebug() << QString("objNum: %1\n").arg(objNum);
    double orbJD = 0.0;
    for(i=0; i<objNum; i++)
    {
        orbJD += orbList.at(i)->elem->eJD;
        //qDebug() << QString("orb eJD: %1\n").arg(jd2mjd(orbList.at(i)->elem->eJD));
    }

    orbJD /= 1.0*objNum;
    orbJD = jd2mjd(orbJD);
    qDebug() << QString("orb eJD mean: %1\n").arg(orbJD);

    int intCase;    //0-normal; 1-back in time; 2-between

///////////////
    if((orbJD<=tBeg)&&(orbJD<tEnd)) intCase = 0;
    if((orbJD>tBeg)&&(orbJD>=tEnd)) intCase = 1;
    if((orbJD>tBeg)&&(orbJD<tEnd)) intCase = 2;

    qDebug() << QString("intCase: %1\n").arg(intCase);



    T0 = orbJD;


//////////////prepNB


    nbody = new dele();

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

    if(readParticles(confFile, pList))
    {
        qDebug() << QString("readCFG error\n");
        return 1;
    }

    int envSize = pList.size();
    qDebug() << QString("envSize: %1\n").arg(envSize);

    double iniMassSun = pList.at(0)->mass;
    switch(massType)
    {
    case 0:         //Msol
        break;
    case 1:         //Msol^-1
        for(i=0;i<envSize;i++) pList.at(i)->mass = 1.0/pList.at(i)->mass;
        break;
    case 2:         //GM or kg
        for(i=0;i<envSize;i++) pList.at(i)->mass = pList.at(i)->mass/iniMassSun;
        break;
    }

    X = new double[3];
    V = new double[3];

    X0 = new double[3];
    V0 = new double[3];

    double coefX, coefXD;
    coefX = coefXD = 1.0;
    int isObj;

    jdTDT = TDB2TDT(T0);
    jdUTC = TDB2UTC(T0);
    sJD = QString("%1").arg(jdUTC, 11, 'f',7);
    jdTDB = UTC2TDB(jdUTC);
    qDebug() << QString("test jd: %1 - %2 - %3 - %4\n").arg(time0, 11, 'f',7).arg(jdUTC, 11, 'f',7).arg(jdTDB, 11, 'f',7).arg(jdTDT, 11, 'f',7);

//    int mNum = 0;

    for(i=0; i<envSize; i++)
    {
        name = QString(pList[i]->name.data());

        if(useEPM) plaNum = epm_planet_num(name);
        else plaNum = planet_num(name.toAscii().data());

        if(plaNum!=-1)
        {
            if(useEPM)
            {
                status = calc_EPM(plaNum, centr_num, jday, pday, X, V);
                 if(!status)
                 {
                     qDebug() << QString("error EPM\n");
                     return 1;
                 }
            }
            else
            {
                nbody->detState(&X[0], &X[1], &X[2], &V[0], &V[1], &V[2], time0, plaNum, CENTER, SK);
            }

            if(plaNum==SUN_NUM)
            {
                Xsun[0] = X[0];
                Xsun[1] = X[1];
                Xsun[2] = X[2];
                Vsun[0] = V[0];
                Vsun[1] = V[1];
                Vsun[2] = V[2];
            }

        }
        else
        {
            isObj = 0;

            if(mCat.GetRecName(name.simplified().toAscii().data()))
            {
               qDebug() << QString("cat\'t find object %1\n").arg(name.simplified().toAscii().data());
               break;
            }
            qDebug() << QString("%1:\nepoch: %2\nMA: %3\nw: %4\nNode: %5\ninc: %6\necc: %7\na: %8\n").arg(mCat.record->name).arg(mCat.record->getEpoch(), 15, 'f',7).arg(mCat.record->meanA, 11, 'f',6).arg(mCat.record->w, 11, 'f',6).arg(mCat.record->Node, 11, 'f',6).arg(mCat.record->inc, 11, 'f',6).arg(mCat.record->ecc, 11, 'f',6).arg(mCat.record->a, 11, 'f',6);
            orbRec.get(&mCat);

            orbRec.detRecEkv(&X[0], &X[1], &X[2], jdTDT);
            orbRec.detRecEkvVel(&V[0], &V[1], &V[2], jdTDT);

            if(!center)
            {
                X[0] += Xsun[0];
                X[1] += Xsun[1];
                X[2] += Xsun[2];
                V[0] += Vsun[0];
                V[1] += Vsun[1];
                V[2] += Vsun[2];
            }


        }

        xVect << X;
        vVect << V;
        dist = sqrt(X[0]*X[0] + X[1]*X[1] + X[2]*X[2]);
        vel = sqrt(V[0]*V[0] + V[1]*V[1] + V[2]*V[2]);
        pList[i]->x = coefX*X[0];
        pList[i]->y = coefX*X[1];
        pList[i]->z = coefX*X[2];
        pList[i]->xd = coefXD*V[0];
        pList[i]->yd = coefXD*V[1];
        pList[i]->zd = coefXD*V[2];

        if(pList.at(i)->identity!=Advisor::collapsorFixed)
        {
            bsList.addParticle(pList[i]);
            //mass[mNum++] = pList[i]->mass;
        }
    }


    ParticleStruct *nPar;
    for(i=0; i<objNum; i++)
    {
        moRec = m_objList.obj_list.at(i);
        isObj=1;

        for(j=envSize-1; j>=0; j--)
        {
            if(QString().compare(moRec->objName, QString(pList.at(j)->name.data()), Qt::CaseInsensitive)==0)
            {
                isObj=0;
                break;
            }
        }
        if(isObj)
        {
            orbRec.detRecEkv(&X[0], &X[1], &X[2], jdTDT);
            orbRec.detRecEkvVel(&V[0], &V[1], &V[2], jdTDT);

            if(!center)
            {
                X[0] += Xsun[0];
                X[1] += Xsun[1];
                X[2] += Xsun[2];
                V[0] += Vsun[0];
                V[1] += Vsun[1];
                V[2] += Vsun[2];
            }

            nPar = new ParticleStruct;
            nPar->name = moRec->objName.toAscii().data();
            nPar->x = X[0];
            nPar->y = X[1];
            nPar->z = X[2];
            nPar->xd = V[0];
            nPar->yd = V[1];
            nPar->zd = V[2];
            nPar->identity = Advisor::planetesimal;
            nPar->interactionPermission = Advisor::interactALL;
            nPar->mass = -1.0;

            pList << nPar;
            bsList.addParticle(nPar);
            //mass[mNum++] = -1.0;
        }
    }

    saveParticles("./test.xml", pList);

/////////////////////////testNB

    QTime timeElapsed;// = QTime.currentTime();
    timeElapsed.start();
    //double TI, TF, DT;
    nofzbody = bsList.size();
    mass = new double[nofzbody];
//    double *X0, *Y0;
    X0 = new double[nofzbody*3];
    V0 = new double[nofzbody*3];

    for(i=0; i<nofzbody; i++)
    {
        X0[i*3+0] = bsList.bsList.at(i)->states.at(0)->Position[0];
        X0[i*3+1] = bsList.bsList.at(i)->states.at(0)->Position[1];
        X0[i*3+2] = bsList.bsList.at(i)->states.at(0)->Position[2];

        V0[i*3+0] = bsList.bsList.at(i)->states.at(0)->Velocity[0];
        V0[i*3+1] = bsList.bsList.at(i)->states.at(0)->Velocity[1];
        V0[i*3+2] = bsList.bsList.at(i)->states.at(0)->Velocity[2];

        mass[i] = bsList.bsList.at(i)->mass;
    }

    switch(intCase)
    {
    case 0:
        TF = tEnd;
        DT = dtime;
        break;
    case 1:
        TF = tBeg;
        DT = -dtime;
        break;
    case 2:
        break;
    }








    
    return 0;//a.exec();
}

int prepNbody()
{

}

int doNbody(double *X0, double *V0, double tf, double dt, double nstep, bodyStateList &bsList)
{
    Everhardt *solSys;

    int i, teloi, nt, jday;
    double ti, pday;
    double *X, *V;

    N = (nofzbody)*3;

    X = new double[N];
    V = new double[N];
//    X0 = new double[N];
//    V0 = new double[N];
//    r = new double[3];
//    v = new double[3];


    solSys = new Everhardt(N, eparam->NCLASS, eparam->NOR, eparam->NI, eparam->LL, eparam->XL);

    for(i=0; i<N; i++)
    {
        X[i] = X0[i];
        V[i] = V0[i];
    }

    for(nt=0; nt<nstep; nt++)
    {
        qDebug() << QString("\njd: %1\ntime: %2\n").arg(tf, 12, 'f', 4).arg(getStrFromDATEOBS(getDATEOBSfromMJD(jd2mjd(tf)), ":", 0, 3));

        ti = tf;
        tf += dt;

        jday = int(tf);
        pday = tf - jday;
        qDebug() << QString("jday: %1\tpday: %2\n").arg(jday).arg(pday);

        solSys->rada27(X, V, 0, fabs(dt));

        for(i=0; i<nofzbody; i++)
        {
            bsList.bsList.at(i)->addState(&X[i*3], &V[i*3]);
        }

    }
}
