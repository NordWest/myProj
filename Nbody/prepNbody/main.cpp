#include <QtCore/QCoreApplication>

#include "./../libs/dele.h"
//#include "./../libs/orbcat.h"
//#include "./../libs/observ.h"
//#include "./../libs/dele.h"
#include "./../libs/orbit.h"
//#include "./../libs/skyarea.h"
//#include "./../libs/astro.h"
#include <astro.h>
#include "./../libs/comfunc.h"
//#include "./../libs/rada.h"
#include "./../libs/redStat.h"
#include "./../libs/mpcs.h"
#include "./../libs/ephem_util.h"
#include "./../libs/calc_epm.h"
#include "./../libs/mpccat.h"
#include <SpiceUsr.h>

#include <QDebug>
#include <QSettings>
#include <QDataStream>
#include <QDomDocument>

#define OMPLIB

#include <iostream>
#include <iomanip>

#include "./../libs/moody/moody.h"
#include "./../libs/moody/capsule/capsuleBase/particle/Particle.h"
#include "./../libs/moody/capsule/Capsule.h"
#include "./../libs/moody/capsule/capsuleBase/CapsuleBase.h"
#include "./../libs/myDomMoody.h"


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
//ever_params *eparam;

//#define CENTER CENTER_BARY
//#define SK SK_EKVATOR

void writeState(QList <ParticleStruct*> pList);
void makeParamIn(double time0, double time1, double dtime, double timestep);

//int readCFG(QString fileName, QList<ParticleStruct *> &pList);
//int saveCFG(QString fileName, QList<ParticleStruct *> &pList);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    int SK, CENTER;
    procData miriadeProcData;
    int i, sz;
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
    //QList <ParticleStruct*> pmList;


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

    int jday = (int)time0;
    double pday = time0 - jday;

    time1 = time0+dtime*nstep;

    makeParamIn(time0, time1, dtime, timestep);


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

    int initMpc = mCat.init(mpcCatFile.toAscii().data());


    if(smlType==1&&initMpc)
    {
        qDebug() << QString("err: mCat not found: %1\n").arg(mpcCatFile.toAscii().data());
        return 1;
    }

    if(bigType==2)
    {
        furnsh_c ( leapName.toAscii().data() );     //load LSK kernel
        furnsh_c ( bspName.toAscii().data()  );     //load SPK/BSP kernel with planets ephemerides
    }



    if(readParticles(confFile, pList))
    {
        qDebug() << QString("readCFG error\n");
        return 1;
    }

    sz = pList.size();
    qDebug() << QString("pList: %1\n").arg(sz);
    double iniMassSun = pList.at(0)->mass;
    switch(massType)
    {
    case 0:         //Msol
        break;
    case 1:         //Msol^-1
        for(i=0;i<sz;i++) pList.at(i)->mass = 1.0/pList.at(i)->mass;
        break;
    case 2:         //GM or kg
        for(i=0;i<sz;i++) pList.at(i)->mass = pList.at(i)->mass/iniMassSun;
        break;
    }
    //Capsule *experiment = new Capsule;


    X = new double[3];
    V = new double[3];

    X0 = new double[3];
    V0 = new double[3];

    QFile bigFile("mercury6/big.in");
    bigFile.open(QFile::Truncate | QFile::WriteOnly);
    QTextStream bigStm(&bigFile);

    QFile smlFile("mercury6/small.in");
    smlFile.open(QFile::Truncate | QFile::WriteOnly);
    QTextStream smlStm(&smlFile);

    /*double mass[10];/* = {
                1.0, 5983000.0, 408522.0, 328900.1, 3098700.0, 1047.3908, 3499.2, 22930.0, 19260.0, 1812000.0, 0
    };/



    double gmass[10];

    double gms = nbody->headParam("GMS");
    gmass[9] = gms;
    mass[9] = 1.0;
    gmass[2] = nbody->headParam(QString("GMB").arg(i+1).toAscii().data());
    mass[2] = gms/gmass[2];
    for(i=0; i<9; i++)
    {
        if(i==2) continue;
        gmass[i] = nbody->headParam(QString("GM%1").arg(i+1).toAscii().data());
        mass[i] = gms/gmass[i];
    }

    for(i=0; i<10; i++)
    {
        if(si) mass[i] = SUN_MASS_KG/mass[i];
        qDebug() << QString("mass %1: %2 - %3\n").arg(i).arg(mass[i]).arg(gmass[i]);
    }
*/


    double coefX, coefXD;
    coefX = coefXD = 1.0;
    int isObj;
    /*if(si)
    {
        coefX = AUKM*1000;
        coefXD = 1000*AUKM/SECINDAY;
    }
*/

    jdTDT = TDB2TDT(time0);
    TDB2UTC(time0, &jdUTC);
    sJD = QString("%1").arg(jdUTC, 11, 'f',7);
    UTC2TDB(jdUTC, &jdTDB);

    qDebug() << QString("jd: %1 - %2 - %3 - %4\n").arg(time0, 11, 'f',7).arg(jdUTC, 11, 'f',7).arg(jdTDB, 11, 'f',7).arg(jdTDT, 11, 'f',7);


    bigStm << ")O+_06 Big-body initial data  (WARNING: Do not delete this line!!)\n";
    bigStm << ") Lines beginning with `)' are ignored.\n";
    bigStm << ")---------------------------------------------------------------------\n";
    bigStm << " style (Cartesian, Asteroidal, Cometary) = Cartesian\n";
    bigStm << QString(" epoch (in days) = %1\n").arg(time0, 15, 'f', 8);
    bigStm << ")---------------------------------------------------------------------\n";

    smlStm << ")O+_06 Big-body initial data  (WARNING: Do not delete this line!!)\n";
    smlStm << ") Lines beginning with `)' are ignored.\n";
    smlStm << ")---------------------------------------------------------------------\n";
    smlStm << " style (Cartesian, Asteroidal, Cometary) = Cartesian\n";
    //smlStm << QString(" epoch (in days) = %1\n").arg(time0, 15, 'f', 8);
    smlStm << ")---------------------------------------------------------------------\n";

    for(i=0; i<sz; i++)
    {
        name = QString(pList[i]->name.data());

        if(useEPM) plaNum = epm_planet_num(name);
        else plaNum = planet_num(name.toAscii().data());

        if(plaNum!=-1)
        {
            switch(bigType)
            {
                case 0:


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
                break;

                case 1:
                {

                    outerArguments.clear();


                    //outerArguments << QString("-name=earth");
                    outerArguments << QString("-name=%1").arg(name.simplified());
                    outerArguments << QString("-type=planet");
                    outerArguments << QString("-observer=@sun");
                    outerArguments << QString("-ep=%1").arg(time0, 15, 'f',7);
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
                    break;
                }
                case 2:
                {
                    if(plaNum!=SUN_NUM)
                    {
                        sName = QString("%1 BARYCENTER").arg(name.simplified().toAscii().data());
                        qDebug() << QString("name: %1\n").arg(sName);
                        sJD = QString("%1 JD").arg(time0, 15, 'f',7);
                        str2et_c(sJD.toAscii().data(), &et);
                        if(CENTER) spkezr_c (  sName.toAscii().data(), et, ref, "NONE", "sun", state, &lt );
                        else spkezr_c (  sName.toAscii().data(), et, ref, "NONE", "ssb", state, &lt );
                        X[0] = state[0]/AUKM;
                        X[1] = state[1]/AUKM;
                        X[2] = state[2]/AUKM;
                        V[0] = state[3]/AUKM;
                        V[1] = state[4]/AUKM;
                        V[2] = state[5]/AUKM;
                    }
                }
                    break;

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
            //if(si) pList[i]->mass = pList[0]->mass/pList[i]->mass;

            if(plaNum!=SUN_NUM)
            {
                bigStm << QString(" %1   m=%2 r=20.D0\n").arg(name, 10).arg(pList[i]->mass);
                bigStm << QString("%1 %2 %3\n%4 %5 %6\n0.0 0.0 0.0\n").arg(X[0], 26, 'e', 20).arg(X[1], 26, 'e', 20).arg(X[2], 26, 'e', 20).arg(V[0], 26, 'e', 20).arg(V[1], 26, 'e', 20).arg(V[2], 26, 'e', 20);
            }
            else
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
            /*
            if(useEPM)
            {
                status = calc_EPM(EARTH, centr_num, jday, pday, X0, V0);
                 if(!status)
                 {
                     qDebug() << QString("error EPM\n");
                     return 1;
                 }
            }
            else
            {

                //nbody->detR(&X[0], &X[1], &X[2], time0, plaNum, 0, CENTER, SK);
                //nbody->detR(&V[0], &V[1], &V[2], time0, plaNum, 1, CENTER, SK);
                nbody->detState(&X0[0], &X0[1], &X0[2], &V0[0], &V0[1], &V0[2], time0, GEOCENTR_NUM, CENTER, SK);
            }*/






                isObj = 0;
            switch(smlType)
            {
            case 1:
                {
                outerArguments.clear();


                outerArguments << QString("-name=earth");
                outerArguments << QString("-type=planet");
                outerArguments << QString("-observer=@sun");
                outerArguments << QString("-ep=%1").arg(sJD);
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
                    X0[0] = resSL.at(1).toDouble();
                    X0[1] = resSL.at(2).toDouble();
                    X0[2] = resSL.at(3).toDouble();
                    V0[0] = resSL.at(5).toDouble();
                    V0[1] = resSL.at(6).toDouble();
                    V0[2] = resSL.at(7).toDouble();
                    break;
                }


                qDebug() << QString("X0: %1\t%2\t%3\nV0: %4\t%5\t%6\n").arg(X0[0]).arg(X0[1]).arg(X0[2]).arg(V0[0]).arg(V0[1]).arg(V0[2]);


//////////////////////////
                    outerArguments.clear();

                    outerArguments << QString("-name=%1").arg(name.simplified());

                    outerArguments << QString("-ep=%1").arg(sJD);
                    //outerArguments << QString("-ep=%1").arg(time0, 15, 'f',7);
                    outerArguments << "-type=aster";
                    //outerArguments << QString("-observer=@sun");
                    outerArguments << QString("-observer=500");
                    outerArguments << "-tcoor=2";
                    outerArguments << "-rplane=1";

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
                        if(resSL.size()<8) continue;
                        isObj = 1;
                        X[0] = resSL.at(1).toDouble();
                        X[1] = resSL.at(2).toDouble();
                        X[2] = resSL.at(3).toDouble();
/*
                        V[0] = resSL.at(5).toDouble();
                        V[1] = resSL.at(6).toDouble();
                        V[2] = resSL.at(7).toDouble();
*/
                        V[0] = resSL.at(8).toDouble();
                        V[1] = resSL.at(9).toDouble();
                        V[2] = resSL.at(10).toDouble();

                        X[0] += X0[0];
                        X[1] += X0[1];
                        X[2] += X0[2];
                        V[0] += V0[0];
                        V[1] += V0[1];
                        V[2] += V0[2];






                    }
                    break;
                }
            case 0:
                {



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

                    isObj = 1;
                    break;
                }
            }






            if(!isObj)
            {
                pList.removeAt(i);
                i--;
            }
            else
            {
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

                smlStm << QString("%1\n").arg(name);//.arg(1.0/pList[i]->mass);
                smlStm << QString("%1 %2 %3\n%4 %5 %6\n0.0 0.0 0.0\n").arg(X[0], 26, 'e', 20).arg(X[1], 26, 'e', 20).arg(X[2], 26, 'e', 20).arg(V[0], 26, 'e', 20).arg(V[1], 26, 'e', 20).arg(V[2], 26, 'e', 20);
                qDebug() << QString("%1 %2 %3\n%4 %5 %6\n0.0 0.0 0.0\n").arg(X[0], 26, 'e', 20).arg(X[1], 26, 'e', 20).arg(X[2], 26, 'e', 20).arg(V[0], 26, 'e', 20).arg(V[1], 26, 'e', 20).arg(V[2], 26, 'e', 20);
            }
        }


    }

    bigFile.close();
    smlFile.close();

    saveParticles("./test.xml", pList);

    if(useMoody)
    {

        coefX = AUKM*1000;
        coefXD = 1000*AUKM/SECINDAY;

        cfgStruct cfgPar;

        cfgPar.gConst = 6.67428E-11;
        cfgPar.stepSize = timestep*86400;
        cfgPar.steps = dtime/timestep;
        cfgPar.states = nstep;

        saveCFG(cfg_file, cfgPar);

        for(i=0;i<pList.size();i++)
        {
            pList.at(i)->x *= coefX;
            pList.at(i)->y *= coefX;
            pList.at(i)->z *= coefX;

            pList.at(i)->xd *= coefXD;
            pList.at(i)->yd *= coefXD;
            pList.at(i)->zd *= coefXD;

            pList[i]->mass = SUN_MASS_KG*pList[i]->mass;

        }

        saveParticles(part_file, pList);

        QDir().remove(mop_file);
    }

    return 0;//a.exec();
}

void makeParamIn(double time0, double time1, double dtime, double timestep)
{
    QFile cfgFile("mercury6/param.in");
    cfgFile.open(QFile::Truncate | QFile::WriteOnly);
    QTextStream cfgStm(&cfgFile);

    cfgStm << ")O+_06 Integration parameters  (WARNING: Do not delete this line!!)\n";
    cfgStm << ") Lines beginning with `)' are ignored.\n";
    cfgStm << ")---------------------------------------------------------------------\n";
    cfgStm << ") Important integration parameters:\n";
    cfgStm << ")---------------------------------------------------------------------\n";
    cfgStm << " algorithm (MVS, BS, BS2, RADAU, HYBRID etc) = bs\n";
    cfgStm << QString(" start time (days) = %1\n").arg(time0, 15, 'f', 8);
    cfgStm << QString(" stop time (days) = %1\n").arg(time1, 15, 'f', 8);
    cfgStm << QString(" output interval (days) = %1\n").arg(dtime, 15, 'f', 8);
    cfgStm << QString(" timestep (days) = %1\n").arg(timestep, 15, 'f', 8);
    cfgStm << " accuracy parameter=1.d-12\n";
    cfgStm << ")---------------------------------------------------------------------\n";
    cfgStm << ") Integration options:\n";
    cfgStm << ")---------------------------------------------------------------------\n";
    cfgStm << " stop integration after a close encounter = no\n";
    cfgStm << " allow collisions to occur = no\n";
    cfgStm << " include collisional fragmentation = no\n";
    cfgStm << " express time in days or years = years\n";
    cfgStm << " express time relative to integration start time = no\n";
    cfgStm << " output precision = medium\n";
    cfgStm << " < not used at present >\n";
    cfgStm << " include relativity in integration= no\n";
    cfgStm << " include user-defined force = no\n";
    cfgStm << ")---------------------------------------------------------------------\n";
    cfgStm << ") These parameters do not need to be adjusted often:\n";
    cfgStm << ")---------------------------------------------------------------------\n";
    cfgStm << " ejection distance (AU)= 100\n";
    cfgStm << " radius of central body (AU) = 0.005\n";
    cfgStm << " central mass (solar) = 1.0\n";
    cfgStm << " central J2 = 0\n";
    cfgStm << " central J4 = 0\n";
    cfgStm << " central J6 = 0\n";
    cfgStm << " < not used at present >\n";
    cfgStm << " < not used at present >\n";
    cfgStm << " Hybrid integrator changeover (Hill radii) = 3.\n";
    cfgStm << " number of timesteps between data dumps = 500\n";
    cfgStm << " number of timesteps between periodic effects = 100\n";

    cfgFile.close();
}
