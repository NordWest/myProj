#include <QCoreApplication>

#include <dele.h>
#include "./../../libs/orbit.h"
#include <astro.h>
#include "./../../libs/comfunc.h"
#include "./../../libs/redStat.h"
#include "./../../libs/mpcs.h"
//#include "./../../libs/ephem_util.h"
#include "./../../libs/calc_epm.h"
#include "./../../libs/mpccat.h"
#include "./../../libs/mpcfile.h"
#include "./../../libs/time_a.h"
#include "./../../libs/observ.h"
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
/*
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

    cout << msgType[type] << msg << endl;
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
*/

int nofzbody;
dele *nbody;
double *mass;
ever_params *eparam;
int *interact_permitions;

mpccat mCat;
int useEPM;
int centr_num;
int status;
int bigType, smlType;
SpiceChar             * corr;
SpiceChar             * ref;
SpiceDouble             state [6];
SpiceDouble             state_obs [6];
SpiceDouble             sgT0, sgT1, lt;
QString bspName, leapName;

observ *opos;


struct tState
{
    time_a T;
    double X[3], V[3];
};

struct bodyStates
{
    QString name;
    double mass;
    void addState(double tTDB, double *X, double *V);
    void sortTime();
    QList <tState*> states;
};

void bodyStates::addState(double tTDB, double *X, double *V)
{
    tState* st = new tState;

    st->T.setTDB(tTDB);
    st->X[0] = X[0];
    st->X[1] = X[1];
    st->X[2] = X[2];

    st->V[0] = V[0];
    st->V[1] = V[1];
    st->V[2] = V[2];

    states << st;
}

void bodyStates::sortTime()
{
    int sz = states.size();
    int pMin;
    double nMin;

    for(int i=0; i<sz-1; i++)
    {
        nMin = states[i]->T.TDB();
        pMin = i;
        for(int j=i+1; j<sz; j++)
        {
                if(states[j]->T.TDB()<nMin){pMin = j; nMin = states[j]->T.TDB();}
        }

        states.swap(i, pMin);
    }
}


struct bodyStateList
{
    QList <bodyStates*> bsList;
    int addParticle(double tTDB, ParticleStruct* par);
    int size();

};

int bodyStateList::addParticle(double tTDB, ParticleStruct* par)
{
    QString bodyName = par->name.data();

    int np = 1;
    tState *stT;
    stT = new tState;
    stT->T.setTDB(tTDB);
    stT->X[0] = par->x;
    stT->X[1] = par->y;
    stT->X[2] = par->z;
    stT->V[0] = par->xd;
    stT->V[1] = par->yd;
    stT->V[2] = par->zd;

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

void sortEQdtime(eqFile *eq_file, double T0)
{
    int i, j, sz;
    int pMin;
    double vMin, val;
    sz = eq_file->ocList.size();
    QString tstr;

    //qDebug() << QString("T0= %1\n").arg(T0);

    for(i=0; i<sz-1; i++)
    {
        pMin=i;
        vMin = fabs(eq_file->ocList.at(i)->MJday-T0);
        for(j=i+1; j<sz; j++)
        {
            val = fabs(eq_file->ocList.at(j)->MJday-T0);
            if(val<vMin)
            {
                vMin = val;
                pMin = j;
            }
        }
        if(pMin!=i) eq_file->ocList.swap(i, pMin);
    }
}

void sortMPCdtime(mpcFile *mpc_file, double T0)
{
    int i, j, sz;
    int pMin;
    double vMin, val;
    sz = mpc_file->size();
    QString tstr;

    //qDebug() << QString("T0= %1\n").arg(T0);

    for(i=0; i<sz-1; i++)
    {
        pMin=i;
        vMin = fabs(mpc_file->at(i)->mjd()-T0);
        for(j=i+1; j<sz; j++)
        {
            val = fabs(mpc_file->at(j)->mjd()-T0);
            if(val<vMin)
            {
                vMin = val;
                pMin = j;
            }
        }
        if(pMin!=i) mpc_file->recList.swap(i, pMin);
    }
}



int doNbody(double *X0, double *V0, double tf, double dt, double nstep, bodyStateList &bpList, double tBeg, double tEnd);
int makeSPK(int center, int sk, bodyStateList &bsList, QString spkFileName);
int spk2eq(QList <eqObjRec*> eqo_list, QString spkFleName, QString resFileName, QString obsCode);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");
/*
    QFile* logFile = new QFile("mpc2eq.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog0 = new QDataStream(logFile);
    QTextStream logstr(logFile);
*/
    if(argc<2)
    {
        qDebug() << "\nError: mpc file needed\n";
        exit(1);
    }

    int sk, center;
    procData miriadeProcData;
    int i, j, sz;
    //dele *nbody;
    //int centr_num;
    //int status;

    QString objDataStr;
    QString name, sName;
    int plaNum;

    double *X, *V;
    double *X0, *V0;
    QVector <double*> xVect, vVect;
    double Xsun[3], Vsun[3];
    double XE0[3], VE0[3];
    double XEB0[3], VEB0[3];
    double XS0[3], VS0[3];

    QStringList outerArguments, resSL;
    QProcess outerProcess;
    QString sJD;

    double dist, vel, jdUTC, jdTDB, jdTDT, et;
    double time0, time1, dtime, timestep;
    int nstep;

    mpcObjList m_objList;
    double tBeg, tEnd;
    tBeg = 9999999999;
    tEnd = 0;
    QString tStr, objName;
    char *tname = new char[256];

    //eqObjList eqo_list;
    //eqObjRec* eqoTemp;
    //eqFile* eqTemp;
    ocRec *ocTemp;
    int nObj;
    QList <orbit*> orbList;
    orbit* tOrb;
    int objNum;
    double T0, DT;
    double TF;
    int mNum;

    mpcrec mRec;
    OrbCat ocat;
    orbit orbRec;

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
    useEPM = sett->value("general/useEPM", 0).toInt();

    QString orbCatFile = sett->value("general/orbCatFile", "./res.ocat").toString();
    int useOrbCat = sett->value("general/useOrbCat", 0).toInt();

    sk = sett->value("general/sk", 0).toInt();
    center = sett->value("general/center", 0).toInt();

    bigType = sett->value("general/bigType", 0).toInt();
    smlType = sett->value("general/smlType", 0).toInt();

    time0 = sett->value("time/time0", 2455201.0).toDouble();
    dtime = sett->value("time/dtime", 0).toDouble();
    nstep = sett->value("time/nstep", 0).toInt();
    //timestep = sett->value("time/timestep", 0).toDouble();

    miriadeProcData.name = sett->value("miriadeProcData/name", "./mpeph.exe").toString();
    miriadeProcData.folder = sett->value("miriadeProcData/folder", "./").toString();
    miriadeProcData.waitTime = sett->value("miriadeProcData/waitTime", -1).toInt();
    if(miriadeProcData.waitTime>0) miriadeProcData.waitTime *= 1000;

//mpc2eq
    int isCountCols = sett->value("mpc2eq/isCountCols", 1).toInt();
    QString colsNums = sett->value("mpc2eq/colsNums", "4,5,6").toString();

    /*moody
    cfg_file = sett->value("moody/cfg_file").toString();
    part_file = sett->value("moody/part_file").toString();
    mop_file = sett->value("moody/mop_file").toString();*/
    cfg_file = QString("%1/cfg/cfg.xml").arg(moodyDir);
    part_file = QString("%1/particles/particles.xml").arg(moodyDir);
    mop_file = QString("%1/result/moodyProject.mop").arg(moodyDir);

    //CALCEPH
    QString ephFile = sett->value("CALCEPH/ephFile", "./../../data/cats/binp1940_2020.405").toString();


    //SPICE
    bspName = sett->value("SPICE/bspName", "./de421.bsp").toString();
    leapName = sett->value("SPICE/leapName", "./naif0010.tls").toString();
    sprintf(ref,"%s", sett->value("SPICE/ref", "J2000").toString().toAscii().data());
    sprintf(corr,"%s", sett->value("general/corr", "LT").toString().toAscii().data());

    furnsh_c ( leapName.toAscii().data() );     //load LSK kernel
    furnsh_c ( bspName.toAscii().data()  );     //load SPK/BSP kernel with planets ephemerides

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

///////////
    observ opos5;

    opos = new observ;
    if(opos->init(obsFile.toAscii().data(), jplFile.toAscii().data()))
    {
        qDebug() << "\nError opos init\n\n";
        exit(1);
    }
    opos->set_obs_parpam(GEOCENTR_NUM, center, sk, obsCode.toAscii().data());

    opos5.init(obsFile.toAscii().data(), jplFile.toAscii().data());
    opos5.set_obs_parpam(GEOCENTR_NUM, center, sk, "500");

    int jday;// = (int)time0;
    double pday;// = time0 - jday;

    time1 = time0+dtime*nstep;

    if(useOrbCat)ocat.init(orbCatFile.toAscii().data());
    if(mCat.init(mpcCatFile.toAscii().data()))
    {
        qDebug() << "\nError MPCCAT init\n\n";
        return 1;
    }

//Files names

    QString iniMpcFile, resFileName, spkFileName, xmlFileName;

    iniMpcFile = QString(argv[1]);
    resFileName = QString("%1.eq").arg(iniMpcFile.section(".", 0, -2));
    spkFileName = QString("%1.spk").arg(iniMpcFile.section(".", 0, -2));
    xmlFileName = QString("%1.xml").arg(iniMpcFile.section(".", 0, -2));

//Init MPC file
    mpcFile mpc_file;
    mpcRec* mpc_rec;
    mpcObjRec* moRec;
    if(mpc_file.init(iniMpcFile))
    {
        qDebug() << QString("error: Can't open file %1\n").arg(iniMpcFile);
        exit(1);
    }
    mNum = mpc_file.size();

    tBeg = UTC2TDB(mjd2jd(mpc_file.recList.first()->mjd()));
    tEnd = UTC2TDB(mjd2jd(mpc_file.recList.last()->mjd()));
    //tBeg = UTC2TDB(tBeg);
    //tEnd = UTC2TDB(tEnd);
    qDebug() << QString("Time span: %1 - %2\n").arg(tBeg, 12, 'f', 6).arg(tEnd, 12, 'f', 6);

//
    OrbCat orb_cat;
    OrbCat orb_cat0;

//////////////prepNB


    nbody = new dele();

    switch(bigType)
    {
        case 0:
        {
            if(useEPM)
            {
                status = !InitTxt(epmDir.toAscii().data());
                centr_num = 11+!center;
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

            centr_num = 11+!center;
        }
        break;
        case 2:
        {
            furnsh_c ( leapName.toAscii().data() );     //load LSK kernel
            furnsh_c ( bspName.toAscii().data()  );     //load SPK/BSP kernel with planets ephemerides
        }
        break;
    }
/*
    if(useEPM)
    {
        status = !InitTxt(epmDir.toAscii().data());
        centr_num = 11+!center;
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
*/
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

//append environment
    double orbJD;
    ParticleStruct *nPar;
    //sortMPCdtime(mpc_file);
    mNum = mpc_file.size();
    orbJD = 0;

    eqFile eq_file;
    QList <eqFile*> eqList;
    //eqFile* eqTemp;
    ocRec *oc_rec;

    for(i=0;i<mNum;i++)
    {
        mpc_rec = mpc_file.at(i);
        mpc_rec->getMpUPackNumber(tStr);

        tStr.replace(" ", "0");
        sprintf(tname, "%s", tStr.toAscii().data());

//qDebug() << QString("MpNumber: %1\n").arg(tname);

        if(mCat.GetRecNum(tname))
        {
            mpc_rec->getProvDest(tStr);
            qDebug() << QString("ProvDest: %1\n").arg(tStr.toAscii().data());
            if(mCat.GetProvDest(tStr.toAscii().data()))
            {
                qDebug() << QString("not found: %1\n").arg(mpc_rec->toStr());
                continue;
            }
        }

objName = QString(mCat.record->name).simplified();

        if(useOrbCat)
        {
            if(ocat.GetRecName(objName.toAscii().data())==-1)
            {
               qDebug() << QString("ocat\'t find object |%1|\n").arg(objName);
               orbJD += mCat.record->getEpoch();
            }
            else orbJD += ocat.record->eJD;
        }
        else
        {


        //qDebug() << QString("%1:\nepoch: %2\nMA: %3\nw: %4\nNode: %5\ninc: %6\necc: %7\na: %8\n").arg(mCat.record->name).arg(mCat.record->getEpoch(), 15, 'f',7).arg(mCat.record->meanA, 11, 'f',6).arg(mCat.record->w, 11, 'f',6).arg(mCat.record->Node, 11, 'f',6).arg(mCat.record->inc, 11, 'f',6).arg(mCat.record->ecc, 11, 'f',6).arg(mCat.record->a, 11, 'f',6);
            orbJD += mCat.record->getEpoch();
        }


        oc_rec = new ocRec;
        oc_rec->name = objName;
        oc_rec->MJday = mpc_rec->mjd();
        oc_rec->ra = mpc_rec->ra();
        oc_rec->de = mpc_rec->dec();
        oc_rec->mag = mpc_rec->magn();
        mpc_rec->getObsCode(oc_rec->obsCode);
        eq_file.ocList << oc_rec;

        isObj = 1;

        for(j=0; j<envSize; j++)
        {
            if(QString().compare(objName, pList[j]->name.data())==0)
            {
                isObj=0;
                break;
            }
        }
        if(isObj)
        {
            nPar = new ParticleStruct;
            nPar->name = objName.toAscii().data();
            nPar->identity = Advisor::planetesimal;
            nPar->interactionPermission = Advisor::interactALL;
            nPar->mass = -1.0;

            pList << nPar;
            envSize++;
        }
    }

//time
    jdTDT = orbJD/(1.0*mNum);
    jdTDB = TDT2TDB(jdTDT);
    jdUTC = TDT2UTC(jdTDT);

    qDebug() << QString("orb jdTDB: %1\n").arg(jdTDB, 15, 'f', 8);

    sJD = QString("%1").arg(jdUTC, 11, 'f',7);

    jday = (int)jdTDB;
    pday = jdTDB - jday;

//sun
    if(useEPM) plaNum = epm_planet_num("Sun");
    else plaNum = planet_num("Sun");
    if(center)
    {
        //nbody->detState(&Xsun[0], &Xsun[1], &Xsun[2], &Vsun[0], &Vsun[1], &Vsun[2], jdTDB, SUN_NUM, 0, sk);
        switch(bigType)
        {
            case 0:
            {
                if(useEPM)
                {
                    status = calc_EPM(plaNum, 12, (int)jdTDB, (int)jdTDB-jdTDB, Xsun, Vsun);
                     if(!status)
                     {
                         qDebug() << QString("error EPM\n");
                         return 1;
                     }
                }
                else
                {
                    nbody->detState(&Xsun[0], &Xsun[1], &Xsun[2], &Vsun[0], &Vsun[1], &Vsun[2], jdTDB, plaNum, 0, sk);
                }
            }
            break;

            case 1:
            {
                plaNum = epm_planet_num("Sun");

                //calceph_scompute((int)time0, time0-(int)time0, 16, 0, state);
                //qDebug() << QString("TT-TDB = %1\n").arg(state[0]);
                //timei = time0 + state[0];
                calceph_scompute((int)jdTDB, jdTDB-(int)jdTDB, plaNum, 12, state);
                Xsun[0] = state[0];
                Xsun[1] = state[1];
                Xsun[2] = state[2];
                Vsun[0] = state[3];
                Vsun[1] = state[4];
                Vsun[2] = state[5];

                break;
            }
            case 2:
            {
                    sJD = QString("%1 JD").arg(jdTDB, 15, 'f',7);
                    str2et_c(sJD.toAscii().data(), &et);
                    spkezr_c (  "Sun", et, ref, "NONE", "ssb", state, &lt );

                    Xsun[0] = state[0]/AUKM;
                    Xsun[1] = state[1]/AUKM;
                    Xsun[2] = state[2]/AUKM;
                    Vsun[0] = state[3]/AUKM*SECINDAY;
                    Vsun[1] = state[4]/AUKM*SECINDAY;
                    Vsun[2] = state[5]/AUKM*SECINDAY;


                break;
            }
        }
    }

    envSize = pList.size();
    qDebug() << QString("full envSize: %1\n").arg(envSize);
    nofzbody = envSize-1;
    mass = new double[nofzbody];
    X0 = new double[nofzbody*3];
    V0 = new double[nofzbody*3];

//    srand (time(NULL));
//    double decVol = 0.000001;//*(rand()%1)-0.0000005;


    int p=0;
    for(i=0; i<envSize; i++)
    {
        if(pList.at(i)->identity==Advisor::collapsorFixed) continue;
        name = QString(pList[i]->name.data());

        plaNum = planet_num(name.toAscii().data());

        if(plaNum!=-1)
        {
            switch(bigType)
            {
                case 0:
                {
                    if(useEPM)
                    {
                        plaNum = epm_planet_num(name);
                        status = calc_EPM(plaNum, centr_num, jday, pday, X, V);
                         if(!status)
                         {
                             qDebug() << QString("error EPM\n");
                             return 1;
                         }
                    }
                    else
                    {
                        //plaNum = planet_num(name.toAscii().data());
                        nbody->detState(&X[0], &X[1], &X[2], &V[0], &V[1], &V[2], jdTDB, plaNum, center, sk);
                    }
                }
                break;

                case 1:
                {
                plaNum = epm_planet_num(name);
                //calceph_scompute((int)time0, time0-(int)time0, 16, 0, state);
                //qDebug() << QString("TT-TDB = %1\n").arg(state[0]);
                //timei = time0 + state[0];
                calceph_scompute((int)jdTDB, jdTDB-(int)jdTDB, epm_planet_num(name), centr_num, state);
                X[0] = state[0];
                X[1] = state[1];
                X[2] = state[2];
                V[0] = state[3];
                V[1] = state[4];
                V[2] = state[5];



                    break;
                }
                case 2:
                {
                //plaNum = planet_num(name.toAscii().data());
                    if(plaNum!=SUN_NUM)
                    {
                        if(QString().compare(name.simplified(), "EMB", Qt::CaseInsensitive)!=0) sName = QString("%1 BARYCENTER").arg(name.simplified().toAscii().data());
                        else sName = name;
                        qDebug() << QString("name: %1\n").arg(sName);
                        sJD = QString("%1 JD").arg(jdTDB, 15, 'f',7);
                        str2et_c(sJD.toAscii().data(), &et);
                        if(center) spkezr_c (  sName.toAscii().data(), et, ref, "NONE", "sun", state, &lt );
                        else spkezr_c (  sName.toAscii().data(), et, ref, "NONE", "ssb", state, &lt );
                        X[0] = state[0]/AUKM;
                        X[1] = state[1]/AUKM;
                        X[2] = state[2]/AUKM;
                        V[0] = state[3]/AUKM*SECINDAY;
                        V[1] = state[4]/AUKM*SECINDAY;
                        V[2] = state[5]/AUKM*SECINDAY;
                    }
                }
                    break;
            case 3:
            {

                outerArguments.clear();


                //outerArguments << QString("-name=earth");
                outerArguments << QString("-name=%1").arg(name.simplified());
                outerArguments << QString("-type=planet");
                outerArguments << QString("-observer=@sun");
                outerArguments << QString("-ep=%1").arg(jdTDB, 15, 'f',7);
                //outerArguments << QString("-ep=%1").arg(sJD);
                //outerArguments << QString("-ep=%1").arg(time0, 15, 'f',7);

                qDebug() << outerArguments.join(" ") << "\n";

                outerProcess.setWorkingDirectory(miriadeProcData.folder);
                outerProcess.setProcessChannelMode(QProcess::MergedChannels);
                outerProcess.setReadChannel(QProcess::StandardOutput);

                outerProcess.start(miriadeProcData.name, outerArguments);

                if(!outerProcess.waitForFinished(miriadeProcData.waitTime))
                {
                    qDebug() << "\nmiriadeProc finish error\n";
                    break;
                }

                QTextStream ethStream(outerProcess.readAllStandardOutput());


                isObj = 0;


                while (!ethStream.atEnd())
                {
                    objDataStr = ethStream.readLine();
                    //qDebug() << QString("objDataStr: %1").arg(objDataStr);
                    if(objDataStr.size()<1) continue;
                    if(objDataStr.at(0)=='#') continue;

                    resSL =  objDataStr.split(" ", QString::SkipEmptyParts);
                    if(resSL.size()<8) continue;
                    //isObj = 1;
                    X[0] = resSL.at(1).toDouble();
                    X[1] = resSL.at(2).toDouble();
                    X[2] = resSL.at(3).toDouble();
                    V[0] = resSL.at(5).toDouble();
                    V[1] = resSL.at(6).toDouble();
                    V[2] = resSL.at(7).toDouble();
                    break;
                }


            }
            break;
            }
            /*
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
                nbody->detState(&X[0], &X[1], &X[2], &V[0], &V[1], &V[2], jdTDB, plaNum, center, sk);
            }*/
        }
        else
        {
            isObj = 0;

            if(mCat.GetRecName(name.simplified().toAscii().data()))
            {
               qDebug() << QString("cat\'t find object %1\n").arg(name.simplified().toAscii().data());
               break;
            }

            if(useOrbCat)
            {
                if(ocat.GetRecName(name.simplified().toAscii().data())==-1)
                {
                   //qDebug() << QString("cat\'t find object %1\n").arg(name.simplified().toAscii().data());
                   orbRec.get(&mCat);
                }
                else orbRec.get(ocat.record);
            }
            else
            {


            //qDebug() << QString("%1:\nepoch: %2\nMA: %3\nw: %4\nNode: %5\ninc: %6\necc: %7\na: %8\n").arg(mCat.record->name).arg(mCat.record->getEpoch(), 15, 'f',7).arg(mCat.record->meanA, 11, 'f',6).arg(mCat.record->w, 11, 'f',6).arg(mCat.record->Node, 11, 'f',6).arg(mCat.record->inc, 11, 'f',6).arg(mCat.record->ecc, 11, 'f',6).arg(mCat.record->a, 11, 'f',6);
                orbRec.get(&mCat);
            }

//decrease orbits

            if(!useOrbCat)
            {

/*
                qDebug() << QString("decVol: %1\n").arg(decVol);

                qDebug() << QString("%1:\nepoch: %2\nMA: %3\nw: %4\nNode: %5\ninc: %6\necc: %7\na: %8\n").arg(mCat.record->name).arg(mCat.record->getEpoch(), 15, 'f',7).arg(orbRec.elem->M0, 11, 'f',6).arg(orbRec.elem->w, 11, 'f',6).arg(orbRec.elem->Node, 11, 'f',6).arg(orbRec.elem->inc, 11, 'f',6).arg(orbRec.elem->ecc, 11, 'f',6).arg(orbRec.elem->q, 11, 'f',6);
*/
                //orbRec.elem->ecc = orbRec.elem->ecc + decVol;
                //orbRec.elem->inc = orbRec.elem->inc + decVol;
                //orbRec.elem->M0 = orbRec.elem->M0 + decVol;
                //orbRec.elem->Node = orbRec.elem->Node + decVol;
                //orbRec.elem->q = orbRec.elem->q + decVol;*/
                //orbRec.elem->w = orbRec.elem->w + decVol;

//                qDebug() << QString("%1:\nepoch: %2\nMA: %3\nw: %4\nNode: %5\ninc: %6\necc: %7\na: %8\n").arg(mCat.record->name).arg(mCat.record->getEpoch(), 15, 'f',7).arg(orbRec.elem->M0, 11, 'f',6).arg(orbRec.elem->w, 11, 'f',6).arg(orbRec.elem->Node, 11, 'f',6).arg(orbRec.elem->inc, 11, 'f',6).arg(orbRec.elem->ecc, 11, 'f',6).arg(orbRec.elem->q, 11, 'f',6);
            }

/////////////////

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

            mCat.record->copyTo(orb_cat.record);
            orb_cat.AddRec();


            pList[i]->theta = mCat.record->H;
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


            X0[p*3+0] = pList[i]->x;
            X0[p*3+1] = pList[i]->y;
            X0[p*3+2] = pList[i]->z;

            V0[p*3+0] = pList[i]->xd;
            V0[p*3+1] = pList[i]->yd;
            V0[p*3+2] = pList[i]->zd;

            mass[p] = pList[i]->mass;
            p++;
        //}
    }


    saveParticles(xmlFileName, pList);
    orb_cat.SaveAs(QString("%1_ocat.txt").arg(iniMpcFile.section(".", 0, -2)).toAscii().data());

/////////////////////////testNB

    QTime timeElapsed;// = QTime.currentTime();
    timeElapsed.start();

    int nt, N;
    double ti, tf, dT, ra, dec;
    double vmul;
    Everhardt *solSys;
    QString tstr, objNumStr;
    char* astr = new char[256];

    N = (nofzbody)*3;

    delete [] X;
    delete [] V;
    X = new double[N];
    V = new double[N];

    solSys = new Everhardt(N, eparam->NCLASS, eparam->NOR, eparam->NI, eparam->LL, eparam->XL);

    double dTmin;

    sortEQdtime(&eq_file, jd2mjd(jdUTC));

    QFile ocFile, mpcFile1;
    QTextStream ocStm, mpcStm;

    qDebug() << QString("resFileName: %1\n").arg(resFileName);
    ocFile.setFileName(resFileName);
    ocFile.open(QFile::WriteOnly | QFile::Truncate);
    ocStm.setDevice(&ocFile);

    QString mpcFileName = QString("%1_mpc.txt").arg(resFileName.section(".", 0, -2));
    mpcFile1.setFileName(mpcFileName);
    mpcFile1.open(QFile::WriteOnly | QFile::Truncate);
    mpcStm.setDevice(&mpcFile1);

    vmul = 1;
    for(j=0;j<nofzbody;j++)
    {
        X[j*3] = X0[j*3];
        X[j*3+1] = X0[j*3+1];
        X[j*3+2] = X0[j*3+2];

        V[j*3] = V0[j*3];
        V[j*3+1] = V0[j*3+1];
        V[j*3+2] = V0[j*3+2];
    }

    T0 = jdTDB;

mpc mrec;
double cosD;
double sDist, eDist, magC;
QFile impFile(QString("%1_imp.txt").arg(resFileName.section(".", 0, -2)));
impFile.open(QFile::Truncate | QFile::WriteOnly);
QTextStream impStm(&impFile);

int dOrb = 1;
int k;
double tdo;
double state0[6];

    for(i=0; i<mNum; i++)
    {
        oc_rec = eq_file.ocList.at(i);
        tBeg = T0;
        tEnd = UTC2TDB(mjd2jd(oc_rec->MJday));
        tf = tBeg;
        dT = tEnd - tf;

        //qDebug() << QString("i: %1/%2\n").arg(i).arg(mNum);
        //qDebug() << QString("tBeg= %1\ttEnd= %2\ndT= %3").arg(tBeg, 15, 'f', 7).arg(tEnd, 15, 'f', 7).arg(dT);

        //qDebug() << QString("dT= %1\tvmul= %2\n").arg(dT).arg(vmul);

        if(dT*vmul<0)
        {
            vmul = -vmul;

            for(j=0;j<nofzbody;j++)
            {
                V[j*3] = -V[j*3];
                V[j*3+1] = -V[j*3+1];
                V[j*3+2] = -V[j*3+2];
            }
        }
/*
        while((fabs(dT)/dtime)>1)
        {
            ti = tf;
            tf = tf+vmul*dtime;
            //qDebug() << QString("ti= %1\ttf= %2\n").arg(ti, 15, 'f', 8).arg(tf, 15, 'f', 8);
            solSys->rada27(X, V, 0, fabs(dtime));
            dT = tEnd - tf;
        }
*/
        //orb



//det Local orbits

        if(dOrb&&(fabs(dT)>10))
        {
            tdo = floor(tEnd) - vmul;
            solSys->rada27(X, V, 0, fabs(tf-tdo));

            qDebug() << "###############det Local orbits###############\n\n";
            for(k=0;k<orb_cat.nstr;k++)
            {
                orb_cat.GetRec(k);
                for(j=0;j<nofzbody;j++)
                {
                    if(QString().compare(QString(orb_cat.record->name).simplified(), pList.at(j+1)->name.data())==0)
                    {

                        state[0] = X[j*3];
                        state[1] = X[j*3+1];
                        state[2] = X[j*3+2];

                        state[3] = vmul*V[j*3];
                        state[4] = vmul*V[j*3+1];
                        state[5] = vmul*V[j*3+2];

                        findOrb(orbRec.elem, &state[0], &state[3], tdo);
                        orbRec.set(&orb_cat0);
                        orb_cat0.record->set_number(orb_cat.record->number);
                        orb_cat0.record->set_name(QString(orb_cat.record->name).simplified().toAscii().data());
                        orb_cat0.record->set_author("Berezhnoy");
                        orb_cat0.record->set_makeD(QDate().currentDate().toString("yyMMdd").toAscii().data());
                        orb_cat0.AddRec();
                        //orbRec.detRecEkv(&state0[0],&state0[1],&state0[2],tdo);
                        //orbRec.detRecEkvVel(&state0[3],&state0[4],&state0[5],tdo);
                        //qDebug() << QString("state: %1\t%2\t%3\t%4\t%5\t%6\n").arg(state[0]).arg(state[1]).arg(state[2]).arg(state[3]).arg(state[4]).arg(state[5]);
                        //qDebug() << QString("state0: %1\t%2\t%3\t%4\t%5\t%6\n").arg(state0[0]).arg(state0[1]).arg(state0[2]).arg(state0[3]).arg(state0[4]).arg(state0[5]);
                        //qDebug() << QString("dstate: %1\t%2\t%3\t%4\t%5\t%6\n").arg(state[0]-state[0]).arg(state[1]-state[1]).arg(state[2]-state[2]).arg(state[3]-state[3]).arg(state[4]-state[4]).arg(state[5]-state[5]);
                    }
                }

            }
            orb_cat0.SaveAs(QString("%1_ocat0.txt").arg(iniMpcFile.section(".", 0, -2)).toAscii().data());

            dOrb = 0;
            tf = tdo;
        }


        if(fabs(tf-tEnd)>1e-7)
        {
            ti = tf;
            tf = tEnd;
            //qDebug() << QString("ti= %1\ttf= %2\n").arg(ti, 15, 'f', 8).arg(tf, 15, 'f', 8);
            solSys->rada27(X, V, 0, fabs(tf-ti));
        }



        T0 = tEnd;

        for(j=0;j<nofzbody;j++)
        {
            if(QString().compare(oc_rec->name, pList.at(j+1)->name.data())==0)
            {
                switch(bigType)
                {
                    case 0:
                    {
                        if(useEPM)
                        {
                            status = calc_EPM(3, centr_num, (int)tEnd, tEnd-(int)tEnd, XE0, VE0);
                             if(!status)
                             {
                                 qDebug() << QString("error EPM\n");
                                 return 1;
                             }
                        }
                        else nbody->detState(&XE0[0], &XE0[1], &XE0[2], &VE0[0], &VE0[1], &VE0[2], tEnd, GEOCENTR_NUM, CENTER_SUN, sk);

                        if(useEPM)
                        {
                            status = calc_EPM(3, epm_planet_num("Sun"), (int)tEnd, tEnd-(int)tEnd, XEB0, VEB0);
                             if(!status)
                             {
                                 qDebug() << QString("error EPM\n");
                                 return 1;
                             }
                        }
                        else nbody->detState(&XEB0[0], &XEB0[1], &XEB0[2], &VEB0[0], &VEB0[1], &VEB0[2], tEnd, GEOCENTR_NUM, CENTER_BARY, sk);

                        if(useEPM)
                        {
                            status = calc_EPM(SUN_NUM, centr_num, (int)tEnd, tEnd-(int)tEnd, XS0, VS0);
                             if(!status)
                             {
                                 qDebug() << QString("error EPM\n");
                                 return 1;
                             }
                        }
                        else nbody->detState(&XS0[0], &XS0[1], &XS0[2], &VS0[0], &VS0[1], &VS0[2], tEnd, SUN_NUM, CENTER_BARY, sk);

                    }
                        break;
                    case 1:
                {
                        //calceph_scompute((int)timei, timei-(int)timei, 16, 0, state);
                        //qDebug() << QString("TT-TDB = %1\n").arg(state[0]);
                        //timei = time0 + state[0];
                    calceph_scompute((int)tEnd, tEnd-(int)tEnd, epm_planet_num("Earth"), epm_planet_num("Sun"), state);
                        XE0[0] = state[0];
                        XE0[1] = state[1];
                        XE0[2] = state[2];
                        VE0[0] = state[3];
                        VE0[1] = state[4];
                        VE0[2] = state[5];
                        calceph_scompute((int)tEnd, tEnd-(int)tEnd, epm_planet_num("Earth"), epm_planet_num("ssb"), state);
                        XEB0[0] = state[0];
                        XEB0[1] = state[1];
                        XEB0[2] = state[2];
                        VEB0[0] = state[3];
                        VEB0[1] = state[4];
                        VEB0[2] = state[5];
                        calceph_scompute((int)tEnd, tEnd-(int)tEnd, epm_planet_num("Sun"), epm_planet_num("ssb"), state);
                        XS0[0] = state[0];
                        XS0[1] = state[1];
                        XS0[2] = state[2];
                        VS0[0] = state[3];
                        VS0[1] = state[4];
                        VS0[2] = state[5];
                }
                        break;
                    case 2:
                    {
                        sJD = QString("%1 JD").arg(tEnd, 15, 'f',7);
                        str2et_c(sJD.toAscii().data(), &et);

                        qDebug() << QString("timei: %1\tet: %2\n").arg(sJD).arg(et);

                        spkezr_c (  "Earth", et, ref, "NONE", "sun", state, &lt );
                        XE0[0] = state[0]/AUKM;
                        XE0[1] = state[1]/AUKM;
                        XE0[2] = state[2]/AUKM;
                        VE0[0] = state[3]/AUKM*SECINDAY;
                        VE0[1] = state[4]/AUKM*SECINDAY;
                        VE0[2] = state[5]/AUKM*SECINDAY;

                        spkezr_c (  "Earth", et, ref, "NONE", "ssb", state, &lt );
                        XEB0[0] = state[0]/AUKM;
                        XEB0[1] = state[1]/AUKM;
                        XEB0[2] = state[2]/AUKM;
                        VEB0[0] = state[3]/AUKM*SECINDAY;
                        VEB0[1] = state[4]/AUKM*SECINDAY;
                        VEB0[2] = state[5]/AUKM*SECINDAY;

                        spkezr_c (  "sun", et, ref, "NONE", "ssb", state, &lt );
                        XS0[0] = state[0]/AUKM;
                        XS0[1] = state[1]/AUKM;
                        XS0[2] = state[2]/AUKM;
                        VS0[0] = state[3]/AUKM;
                        VS0[1] = state[4]/AUKM;
                        VS0[2] = state[5]/AUKM;
                    }
                        break;
                }




                state[0] = X[j*3]+XS0[0];
                state[1] = X[j*3+1]+XS0[1];
                state[2] = X[j*3+2]+XS0[2];

                state[3] = vmul*V[j*3]+VS0[0];
                state[4] = vmul*V[j*3+1]+VS0[1];
                state[5] = vmul*V[j*3+2]+VS0[2];

                opos->obs->getobsynumO(oc_rec->obsCode.toAscii().data());
                opos->det_observ_tdb(tEnd);

                state_obs[0] = XEB0[0]+opos->obs->x;
                state_obs[1] = XEB0[1]+opos->obs->y;
                state_obs[2] = XEB0[2]+opos->obs->z;
                state_obs[3] = VEB0[0]+opos->obs->vx;
                state_obs[4] = VEB0[1]+opos->obs->vy;
                state_obs[5] = VEB0[2]+opos->obs->vz;
                det_vect_radec(state, state_obs,&ra, &dec);

                //opos->det_vect_radec(state, &ra, &dec);

                cosD = cos(dec);
//                mrec.r = ra;// + dRa;
//                mrec.d = dec;// + dDec;

                ra = rad2grad(ra);
                dec = rad2grad(dec);

                //qDebug() << QString("%1-%2: %3:%4\t%5\t%6\t%7\t%8\t%9\n").arg(bodyName).arg(bodyNum).arg(timei, 15, 'f',7).arg(state[0], 21, 'e',15).arg(state[1], 21, 'e',15).arg(state[2], 21, 'e',15).arg(state[3], 21, 'e',15).arg(state[4], 21, 'e',15).arg(state[5], 21, 'e',15);

                oc_rec->ocRaCosDe = grad2mas(oc_rec->ra - ra)*cosD;
                oc_rec->ocDe = grad2mas(oc_rec->de - dec);

                sDist = norm3(&X[j*3]);
                eDist = dist3(state_obs, state);//sqrt((obs_pos->ox - x)*(obs_pos->ox - x) + (obs_pos->oy - y)*(obs_pos->oy - y) + (obs_pos->oz - z)*(obs_pos->oz - z));

                magC = det_m(pList[j+1]->theta, sDist, eDist, 5.8, detPhase(state_obs[0], state_obs[1], state_obs[2], state[0], state[1], state[2]));

                oc_rec->ocMag = oc_rec->mag - magC;

//                eqo_list.addEQ(oc_rec);

                impStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12|%13\n").arg(oc_rec->name, 16).arg(tEnd, 15, 'f', 7).arg(oc_rec->ra, 15, 'f', 11).arg(oc_rec->de, 15, 'f', 10).arg(ra, 15, 'f', 11).arg(dec, 15, 'f', 10).arg(state[0], 17, 'e', 12).arg(state[1], 17, 'e', 12).arg(state[2], 17, 'e', 12).arg(state[3], 17, 'e', 12).arg(state[4], 17, 'e', 12).arg(state[5], 17, 'e', 12).arg(oc_rec->obsCode);

                oc_rec->rec2sBase(&tstr);

                qDebug() << tstr << "\n";
/*                ocStm << tstr << "\n";


/*
                mCat.GetRecName(oc_rec->name.toAscii().data());
                //mCat.record->getNumStr(astr);
                //objNumStr = QString(astr);


                mrec.eJD = mjd2jd(oc_rec->MJday);
                mrec.num = 1;
                mCat.record->getNumStr(mrec.head->Snum);
                mrec.tail->set_numOfObs(obsCode.toAscii().data());
                mrec.toString(astr);

                //oc_rec->rec2MPC(&tstr, obsCode, objNumStr);

                mpcStm << astr << "\n";*/
            }
        }

    }

impFile.close();

    //QList <eqFile*> eqList;
    eqFile* eqTemp;
    eqSplitObjects(&eq_file, eqList);

    int szi, szj;
    szi = eqList.size();
    qDebug() << QString("eqList size: %1\n").arg(szi);
    for(i=0; i<szi; i++)
    {
        eqTemp = eqList.at(i);
        sortOClist(eqTemp->ocList);
        szj = eqTemp->ocList.size();
        for(j=0;j<szj;j++)
        {
            oc_rec = eqList.at(i)->ocList.at(j);
            tstr = oc_rec->rec2sBase1();

            qDebug() << tstr << "\n";
            ocStm << tstr << "\n";
        }
        if(isCountCols) eqTemp->countCols(colsNums);
        szj = eqTemp->colList.size();
        for(j=0; j<szj; j++)
        {
            eqTemp->colList.at(j)->rec2s(&tstr);
            qDebug() << tstr << "\n";
            ocStm << tstr << "\n";
        }
    }

    ocFile.close();
    mpcFile1.close();

    qDebug() << QString("Time elapsed: %1 sec\n").arg(timeElapsed.elapsed()/1000.0);

    //logFile->close();
    return 0;//a.exec();
}

int prepNbody()
{

}

int doNbody(double *X0, double *V0, double tf, double dt, double nstep, bodyStateList &bsList, double tBeg, double tEnd)
{
    QTextStream cout(stdout);
    qDebug() << QString("\n=== doNbody ===\n\n");
    Everhardt *solSys;

    int i, nt, jday, N;
    double ti, pday;
    double *X, *V;
    double r[3], v[3];
    double vmul;

    vmul = pow(-1, dt<0);

    N = (nofzbody)*3;

    X = new double[N];
    V = new double[N];

    QFile mRecFile("./nbrec.txt");
    mRecFile.open(QFile::Truncate | QFile::WriteOnly);
    QTextStream mRecStm(&mRecFile);

    solSys = new Everhardt(N, eparam->NCLASS, eparam->NOR, eparam->NI, eparam->LL, eparam->XL);

    for(i=0; i<N; i++)
    {
        X[i] = X0[i];
        V[i] = vmul*V0[i];
    }

    qDebug() << QString("nstep= %1\n").arg(nstep);

    char *str = new char[256];
    int p=0;

    for(nt=0; nt<nstep; nt++)
    {
        /*
        for(i=0; i<nofzbody; i++)
        {
            v[0] = vmul*V[i*3];
            v[1] = vmul*V[i*3+1];
            v[2] = vmul*V[i*3+2];
        mRecStm << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(bsList.bsList.at(i)->name, 16).arg(tf, 15, 'f',7).arg(X[i*3], 21, 'e',15).arg(X[i*3+1], 21, 'e',15).arg(X[i*3+2], 21, 'e',15).arg(v[0], 21, 'e',15).arg(v[1], 21, 'e',15).arg(v[2], 21, 'e',15);
        }*/
        if(nt/nstep>0.1*p)
        {
            dat2YMD_str(mjd2jd(tf), str);
            qDebug() << QString("\rstep= %1\%\tti= %2").arg(nt/nstep*100).arg(str);
            p++;
        }

        ti = tf;
        tf += dt;

        jday = int(tf);
        pday = tf - jday;

        solSys->rada27(X, V, 0, fabs(dt));

        if(tf<=tEnd&&tf>=tBeg)
        {
            for(i=0; i<nofzbody; i++)
            {
                v[0] = vmul*V[i*3];
                v[1] = vmul*V[i*3+1];
                v[2] = vmul*V[i*3+2];

                mRecStm << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(bsList.bsList.at(i)->name, 16).arg(tf, 15, 'f',7).arg(X[i*3], 21, 'e',15).arg(X[i*3+1], 21, 'e',15).arg(X[i*3+2], 21, 'e',15).arg(v[0], 21, 'e',15).arg(v[1], 21, 'e',15).arg(v[2], 21, 'e',15);

                bsList.bsList.at(i)->addState(mjd2jd(tf), &X[i*3], &v[0]);
            }
        }

    }

    mRecFile.close();
}

int makeSPK(int center, int sk, bodyStateList &bsList, QString spkFileName)
{
    qDebug() << "\n\n====makeSPK=====\n\n";
    int i, p, bodyNum;
    double timei;
    QString bodyName, sJD;
    SpiceInt       handle;
    SpiceDouble *segmentState;
    SpiceDouble *segmentEphs;
    double et;

    double X[3], V[3];
    double XS0[3], VS0[3];

    int ns;
    int szObj = bsList.size();

    QDir().remove(spkFileName);
    spkopn_c(spkFileName.toAscii().data(), "mpc", 0, &handle);

    QString recFileName = QString("%1.rec").arg(spkFileName.section(".", 0, -2));

    QFile mRecFile(recFileName);
    mRecFile.open(QFile::Truncate | QFile::WriteOnly);
    QTextStream mRecStm(&mRecFile);

    for(p=0; p<szObj; p++)
    {
    //    mopFile.resetFile();

        bsList.bsList.at(p)->sortTime();
        bodyName = bsList.bsList.at(p)->name;
        bodyNum = body_num(bodyName);
        ns = bsList.bsList.at(p)->states.size();
        if(bodyNum!=-1)continue;
        if(mCat.GetRecName(bodyName.toAscii().data())) continue;
        bodyNum = 2000000 + mCat.record->getNum();
        qDebug() << QString("%1: %2\n").arg(bodyName).arg(bodyNum);
        qDebug() << QString("%1: %2\n").arg(bsList.bsList.at(p)->states.at(0)->T.TDB(), 15, 'f', 8).arg(bsList.bsList.at(p)->states.at(ns-1)->T.TDB(), 15, 'f', 8);

        sJD = QString("%1 JD TDB").arg(bsList.bsList.at(p)->states.first()->T.TDB(), 15, 'f',7);
        str2et_c(sJD.toAscii().data(), &sgT0);
        qDebug() << QString("sJD0: %1\tet0: %2\n").arg(sJD).arg(sgT0, 15, 'f',7);

        sJD = QString("%1 JD TDB").arg(bsList.bsList.at(p)->states.last()->T.TDB(), 15, 'f',7);
        str2et_c(sJD.toAscii().data(), &sgT1);
        qDebug() << QString("sJD1: %1\tet1: %2\n").arg(sJD).arg(sgT1, 15, 'f',7);

        segmentState = new SpiceDouble[ns*6];
        segmentEphs = new SpiceDouble[ns];
        for(i=0; i<ns; i++)
        {
            timei = bsList.bsList.at(p)->states.at(i)->T.TDB();//time0+dtime*i;

            if(center) //helio to barycenter
            {
                switch(bigType)
                {
                case 0:
                {
                    if(useEPM)
                    {
                        status = calc_EPM(SUN_NUM, centr_num, (int)timei, timei-(int)timei, XS0, VS0);
                         if(!status)
                         {
                             qDebug() << QString("error EPM\n");
                             return 1;
                         }
                    }
                    else nbody->detState(&XS0[0], &XS0[1], &XS0[2], &VS0[0], &VS0[1], &VS0[2], timei, SUN_NUM, CENTER_BARY, sk);
                    XS0[0] *= AUKM;
                    XS0[1] *= AUKM;
                    XS0[2] *= AUKM;
                    VS0[0] *= AUKM/SECINDAY;
                    VS0[1] *= AUKM/SECINDAY;
                    VS0[2] *= AUKM/SECINDAY;
                }
                    break;
                case 2:
                {
                    sJD = QString("%1 JD").arg(timei, 15, 'f',7);
                    str2et_c(sJD.toAscii().data(), &et);
                    spkezr_c (  "sun", et, ref, "NONE", "ssb", state, &lt );
                    XS0[0] = state[0];
                    XS0[1] = state[1];
                    XS0[2] = state[2];
                    VS0[0] = state[3];
                    VS0[1] = state[4];
                    VS0[2] = state[5];
                }
                    break;
                }


            }

            sJD = QString("%1 JD TDB").arg(timei, 15, 'f',7);
            str2et_c(sJD.toAscii().data(), &et);
            segmentEphs[i] = et;

            X[0] = bsList.bsList.at(p)->states.at(i)->X[0]*AUKM + XS0[0];
            X[1] = bsList.bsList.at(p)->states.at(i)->X[1]*AUKM + XS0[1];
            X[2] = bsList.bsList.at(p)->states.at(i)->X[2]*AUKM + XS0[2];

            V[0] = bsList.bsList.at(p)->states.at(i)->V[0]*AUKM/SECINDAY + VS0[0];
            V[1] = bsList.bsList.at(p)->states.at(i)->V[1]*AUKM/SECINDAY + VS0[1];
            V[2] = bsList.bsList.at(p)->states.at(i)->V[2]*AUKM/SECINDAY + VS0[2];


            //mRecStm << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(bodyName).arg(timei, 15, 'f',7).arg(X[0]/AUKM, 21, 'e',15).arg(X[1]/AUKM, 21, 'e',15).arg(X[2]/AUKM, 21, 'e',15).arg(V[0]/AUKM*SECINDAY, 21, 'e',15).arg(V[1]/AUKM*SECINDAY, 21, 'e',15).arg(V[2]/AUKM*SECINDAY, 21, 'e',15);

            segmentState[i*6+0] = X[0];
            segmentState[i*6+1] = X[1];
            segmentState[i*6+2] = X[2];
            segmentState[i*6+3] = V[0];
            segmentState[i*6+4] = V[1];
            segmentState[i*6+5] = V[2];

        }

        spkw09_c(handle, bodyNum, 0, "J2000", sgT0, sgT1, "SPK_STATES_09", 9, ns, segmentState, segmentEphs);

        delete [] segmentState;
        delete [] segmentEphs;
    }

    spkcls_c(handle);
    mRecFile.close();
    return 0;
}

int spk2eq(QList <eqObjRec*> eqo_list, QString spkFleName, QString resFileName, QString obsCode)
{
    qDebug() << QString("\n=== spk2mpc ===\n");
    double range, ra, dec, timei;
    int bodyNum, i, p;
    double et;

    mpc mrec;
    double jdUTC;
    QFile ocFile, mpcFile;
    QTextStream ocStm, mpcStm;
    QString bodyName;
    char *astr = new char[256];
    QString objName, sJD, objNum;
    double state[6], lt;

    furnsh_c ( leapName.toAscii().data() );     //load LSK kernel
    furnsh_c ( bspName.toAscii().data()  );     //load SPK/BSP kernel with planets ephemerides
    furnsh_c ( spkFleName.toAscii().data()  );

    qDebug() << QString("resFileName: %1\n").arg(resFileName);
    ocFile.setFileName(resFileName);
    ocFile.open(QFile::WriteOnly | QFile::Truncate);
    ocStm.setDevice(&ocFile);

    QString mpcFileName = QString("%1_mpc.txt").arg(resFileName.section(".", 0, -2));
    mpcFile.setFileName(mpcFileName);
    mpcFile.open(QFile::WriteOnly | QFile::Truncate);
    mpcStm.setDevice(&mpcFile);

    int szObj = eqo_list.size();

    ocRec* oc_rec;

    QString tstr;

    for(p=0; p<szObj; p++)
    {
        bodyName = eqo_list[p]->objName;
        bodyNum = body_num(bodyName);

        if(bodyNum!=-1)continue;
        if(mCat.GetRecName(bodyName.toAscii().data())) continue;
        bodyNum = 2000000 + mCat.record->getNum();
        qDebug() << QString("%1: %2\n").arg(bodyName).arg(bodyNum);
        objName = QString("%1").arg(bodyNum);

        int nstep = eqo_list.at(p)->size();

        for(i=0; i<nstep; i++)
        {
            oc_rec = eqo_list.at(p)->eq_list.at(i);

            timei = mjd2jd(UTC2TDB(oc_rec->MJday));

            sJD = QString("%1 JD TDB").arg(timei, 15, 'f',7);
            str2et_c(sJD.toAscii().data(), &et);

            spkezr_c (  objName.toAscii().data(), et, "J2000", "NONE", "SSB", state, &lt );
            state[0] = state[0]/AUKM;
            state[1] = state[1]/AUKM;
            state[2] = state[2]/AUKM;
            state[3] = state[3]/AUKM*SECINDAY;
            state[4] = state[4]/AUKM*SECINDAY;
            state[5] = state[5]/AUKM*SECINDAY;

            opos->det_observ_tdb(timei);
            opos->det_vect_radec_ssb(state, &ra, &dec);

            ra = rad2grad(ra);
            dec = rad2grad(dec);

            //qDebug() << QString("%1-%2: %3:%4\t%5\t%6\t%7\t%8\t%9\n").arg(bodyName).arg(bodyNum).arg(timei, 15, 'f',7).arg(state[0], 21, 'e',15).arg(state[1], 21, 'e',15).arg(state[2], 21, 'e',15).arg(state[3], 21, 'e',15).arg(state[4], 21, 'e',15).arg(state[5], 21, 'e',15);

            oc_rec->ocRaCosDe = (oc_rec->ra - ra)*cos(dec);
            oc_rec->ocDe = (oc_rec->de - dec);

            oc_rec->rec2sBase(&tstr);

            qDebug() << tstr << "\n";
            ocStm << tstr << "\n";

            mCat.GetRecName(oc_rec->name.toAscii().data());
            mCat.record->getNumStr(astr);
            objNum = QString(astr);

            oc_rec->rec2MPC(&tstr, objNum);

            mpcStm << tstr << "\n";
        }


    }

    ocFile.close();
    mpcFile.close();
    return 0;
}
