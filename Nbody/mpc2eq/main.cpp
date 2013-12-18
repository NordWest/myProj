#include <QCoreApplication>

#include "./../../libs/dele.h"
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

    int mNum = mpc_file.size();//.nstr();
    double tBeg, tEnd;
    tBeg = 9999999999;
    tEnd = 0;
    QString tStr, objName;
    char *tname = new char[256];

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
            if(mCat.GetProvDest(tStr.toAscii().data())) exit(1);
        }
        objName = QString(mCat.record->name);

        mCat.GetRecName(objName.toAscii().data());

        //qDebug() << QString("mCat record: %1").arg(mCat.str);

        m_objList.addMpcRec(mpc_rec, objName);
        //if(mpc_rec->mjd()<tBeg)
    }

    m_objList.sortByTime();
    QList <eqObjRec*> eqo_list;
    eqObjRec* eqoTemp;
    eqFile* eqTemp;
    ocRec *ocTemp;
    int nObj;

    for(i=0; i<m_objList.size(); i++)
    {
        moRec = m_objList.obj_list.at(i);
        qDebug() << QString("%1: %2").arg(moRec->objName).arg(moRec->size());

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

            qDebug() << moRec->mpc_list.at(j)->toStr();
           ocTemp = new ocRec;
           ocTemp->catName = "MPCCAT";
           ocTemp->name = eqoTemp->objName;
           ocTemp->ra = moRec->mpc_list.at(j)->ra();
           ocTemp->de = moRec->mpc_list.at(j)->dec();
           ocTemp->mag0 = moRec->mpc_list.at(j)->magn();
           ocTemp->MJday = moRec->mpc_list.at(j)->mjd();

           eqoTemp->eq_list->ocList << ocTemp;
        }
        for(j=0; j<eqoTemp->eq_list->ocList.size(); j++)
        {
            eqoTemp->eq_list->ocList.at(j)->rec2s_short(&tStr);
            qDebug() << tStr;
        }

        qDebug() << "\n";
    }


///////////////


    
    return 0;//a.exec();
}
