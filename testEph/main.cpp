#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <QDataStream>
#include <QDomDocument>


#include "./../libs/dele.h"
//#include "./../libs/orbcat.h"
//#include "./../libs/observ.h"
//#include "./../libs/dele.h"
//#include "./../libs/orbit.h"
//#include "./../libs/skyarea.h"
#include "./../libs/astro.h"
#include "./../libs/comfunc.h"
#include "./../libs/rada.h"
#include "./../libs/redStat.h"
#include "./../libs/mpcs.h"

#define OMPLIB

#include "./../libs/moody/capsule/capsuleBase/mopfile/MopFile.h"
#include "./../libs/moody/moody.h"
#include "./../libs/moody/capsule/capsuleBase/particle/Particle.h"
#include "./../libs/moody/capsule/Capsule.h"
#include "./../libs/moody/capsule/capsuleBase/CapsuleBase.h"
#include "./../libs/myDomMoody.h"

#include "./../libs/calc_epm.h"

#include "./../libs/cspice/SpiceUsr.h"


int nofzbody;
ever_params *eparam;
QList <ParticleStruct*> pList;

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
/*
int epm_planet_num(QString name)
{
    if(QString().compare(name, "MERCURY", Qt::CaseInsensitive)==0) return 1;
    if(QString().compare(name, "VENUS", Qt::CaseInsensitive)==0) return 2;
    if(QString().compare(name, "EARTH", Qt::CaseInsensitive)==0) return 3;
    if(QString().compare(name, "GEOCENTR", Qt::CaseInsensitive)==0) return 3;
    if(QString().compare(name, "MARS", Qt::CaseInsensitive)==0) return 4;
    if(QString().compare(name, "JUPITER", Qt::CaseInsensitive)==0) return 5;
    if(QString().compare(name, "SATURN", Qt::CaseInsensitive)==0) return 6;
    if(QString().compare(name, "URANUS", Qt::CaseInsensitive)==0) return 7;
    if(QString().compare(name, "NEPTUNE", Qt::CaseInsensitive)==0) return 8;
    if(QString().compare(name, "PLUTO", Qt::CaseInsensitive)==0) return 9;
    if(QString().compare(name, "MOON", Qt::CaseInsensitive)==0) return 10;
    if(QString().compare(name, "SUN", Qt::CaseInsensitive)==0) return 11;
    if(QString().compare(name, "SOL", Qt::CaseInsensitive)==0) return 11;
    if(QString().compare(name, "SSB", Qt::CaseInsensitive)==0) return 12;
    if(QString().compare(name, "SOLAR-SYSTEM BARYCENTER", Qt::CaseInsensitive)==0) return 12;
    if(QString().compare(name, "EMB", Qt::CaseInsensitive)==0) return 13;
    if(QString().compare(name, "EARTH-MOON BARYCENTER", Qt::CaseInsensitive)==0) return 13;
    return -1;
}
*/
void saveResults(double t0, double *X, double *V, double *X0, double *V0, int pos, QString name, QTextStream &resStm, QTextStream &dxStm, QTextStream &deStm)
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

    Ri = sqrt(X0[pos+0]*X0[pos+0] + X0[pos+1]*X0[pos+1] + X0[pos+2]*r[pos+2]);

    deStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|1\n").arg(t0, 12, 'f', 4).arg(X0[pos], 18, 'g', 9).arg(X0[pos+1], 18, 'g', 9).arg(X0[pos+2], 18, 'g', 9).arg(Ri, 18, 'g', 9).arg(V0[pos], 18, 'g', 9).arg(V0[pos+1], 18, 'g', 9).arg(V0[pos+2], 18, 'g', 9).arg(name);
}

int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);

    setlocale(LC_NUMERIC, "C");

    QFile* logFile = new QFile("testeph.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog0 = new QDataStream(logFile);
    QTextStream logstr(logFile);


    procData miriadeProcData;
    QProcess outerProcess;
    QString objDataStr;
    QStringList resSL, outerArguments;

    dele *nbody;


    nbody = new dele();
    double t0, nstep, ti, dt, et, tii;
    //int centerNum, skNum;
    int centr_num, plaNum;
    int CENTER, SK;
    int i, j, plaNumEPM, plaNumDE, k;

    SpiceDouble             state [6];
    SpiceDouble             lt;
    SpiceChar             * corr;
    SpiceChar             * ref;
    ref = new SpiceChar[256];
    corr = new SpiceChar[256];


    QSettings *sett = new QSettings("./nb.ini", QSettings::IniFormat);

    QString jplFile = sett->value("general/jplFile", "./../../data/cats/binp1940_2020.405").toString();
    t0 = sett->value("general/time0", 0).toDouble();
    dt = sett->value("general/dt", 1).toDouble();
    nstep = sett->value("general/nstep", 1).toDouble();
    QString confFile = sett->value("general/confFile", "testMajor.xml").toString();
    QString epmDir = sett->value("general/epmDir", "./").toString();
    int useEPM = sett->value("general/useEPM", 0).toInt();

    SK = sett->value("general/sk", 0).toInt();
    CENTER = sett->value("general/center", 0).toInt();

    int bigType = sett->value("general/bigType", 0).toInt();
    int smlType = sett->value("general/smlType", 0).toInt();

    miriadeProcData.name = sett->value("miriadeProcData/name", "./mpeph.exe").toString();
    miriadeProcData.folder = sett->value("miriadeProcData/folder", "./").toString();
    miriadeProcData.waitTime = sett->value("miriadeProcData/waitTime", -1).toInt();
    if(miriadeProcData.waitTime>0) miriadeProcData.waitTime *= 1000;

    //SPICE
    QString bspName = sett->value("SPICE/bspName", "./de421.bsp").toString();
    QString leapName = sett->value("SPICE/leapName", "./naif0010.tls").toString();
    sprintf(ref,"%s", sett->value("SPICE/ref", "J2000").toString().toAscii().data());
    sprintf(corr,"%s", sett->value("general/corr", "LT").toString().toAscii().data());


    double x, y, z, vx, vy, vz;
    double dx, dy, dz, dvx, dvy, dvz;
    double dist, dvel;
    double *X, *V;
    double *X0, *V0;
    double *dX, *dV;

    X = new double[3];
    V = new double[3];

    X0 = new double[3];
    V0 = new double[3];

    dX = new double[3];
    dV = new double[3];

    QString resDirName("./resEph");
    QDir resDir(resDirName);


    QFile resFile;
    QTextStream resStm;
    QString resFileName, sJD;

    QString name, sName;
    int status;
    int jday;
    double pday, t, jdUTC, jdTDB;

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


    if(bigType==2)
    {
        furnsh_c ( leapName.toAscii().data() );     //load LSK kernel
        furnsh_c ( bspName.toAscii().data()  );     //load SPK/BSP kernel with planets ephemerides
    }
    //qDebug() << QString("sk: %1\ncenter de: %2\ncenter EPM: %3\n").arg(skNum).arg(centerNum).arg(centr_num);



//    plaNum = 11;

    int iniJplRes = nbody->init(jplFile.toAscii().data());
    if(iniJplRes) return 1;

//    double X[3], V[3];



    if(readCFG(confFile, pList))
    {
        qDebug() << QString("readCFG error\n");
        return 1;
    }

    nofzbody = pList.size();

    resFileName = QString("./rel.txt");
    //qDebug() << QString("res file name: %1\n").arg(resFileName);
    resFile.setFileName(resFileName);
    resFile.open(QFile::WriteOnly | QFile::Truncate);
    resStm.setDevice(&resFile);

    for(k=0; k<nstep; k++)
    {
        ti = t0 + dt*(k-nstep/2);
        tii=t0;
        //tii = ti+0.000766;
        //tii = t0+0.000777500;
        //TDB2UTC(ti, &jdUTC);
        //UTC2TDB(ti, &jdTDB);
        //tii = TDB2TDT(ti);

        for(i=0; i<nofzbody; i++)
        {
            name = QString(pList[i]->name.data());
            plaNumEPM = epm_planet_num(name);
            plaNumDE = planet_num(name.toAscii().data());

            if(useEPM) plaNum = epm_planet_num(name);
            else plaNum = planet_num(name.toAscii().data());

            if(((plaNum==SUN_NUM||plaNumEPM==11)&&CENTER)||plaNum==-1) continue;

/*
                resFileName = QString("%1/rel_%2.txt").arg(resDir.absolutePath()).arg(name);
                qDebug() << QString("res file name: %1\n").arg(resFileName);
                resFile.setFileName(resFileName);
                resFile.open(QFile::WriteOnly | QFile::Truncate);
                resStm.setDevice(&resFile);
    /*
                resFileNameDE = QString("%1/de_%2.txt").arg(resDir.absolutePath()).arg(name);
                qDebug() << QString("de file name: %1\n").arg(resFileNameDE);
                resFileDE.setFileName(resFileNameDE);
                resFileDE.open(QFile::WriteOnly | QFile::Truncate);
                resStmDE.setDevice(&resFileDE);

                resFileNameEPM = QString("%1/epm_%2.txt").arg(resDir.absolutePath()).arg(name);
                qDebug() << QString("epm file name: %1\n").arg(resFileNameEPM);
                resFileEPM.setFileName(resFileNameEPM);
                resFileEPM.open(QFile::WriteOnly | QFile::Truncate);
                resStmEPM.setDevice(&resFileEPM);
    */
                /*
                for(j=0; j<nstep; j++)
                {
                    ti = t0+dt*j;
                    jday = int(ti);
                    pday = ti - jday;// + 60.0/86400.0;

                    qDebug() << QString("ti: %1\tjday: %2\tpday: %3\n").arg(ti, 12, 'f').arg(jday).arg(pday);

                    //nbody->detR(&x, &y, &z, ti, plaNum, 0, centerNum, skNum);
                    //nbody->detR(&vx, &vy, &vz, ti, plaNum, 1, centerNum, skNum);
                    //nbody->detState(&x, &y, &z, &vx, &vy, &vz, ti, plaNumDE, centerNum, skNum);


*/
            //UTC2TDB(t0, &jdTDB);

                    if(useEPM)
                    {
                        status = calc_EPM(plaNum, centr_num, (int)tii, tii-(int)tii, X, V);
                         if(!status)
                         {
                             qDebug() << QString("error EPM\n");
                             return 1;
                         }
                    }
                    else nbody->detState(&X[0], &X[1], &X[2], &V[0], &V[1], &V[2], tii, plaNum, CENTER, SK);

                    dist = sqrt(X[0]*X[0] + X[1]*X[1] + X[2]*X[2]);
                    dvel = sqrt(V[0]*V[0] + V[1]*V[1] + V[2]*V[2]);
                    qDebug() << QString("DE: %1|%2|%3|%4|%5|%6|%7|%8|%9|%10\n").arg(name, -10).arg(tii, 15, 'f', 6).arg(X[0], 18, 'g', 9).arg(X[1], 18, 'g', 9).arg(X[2], 18, 'g', 9).arg(dist, 18, 'g', 9).arg(V[0], 18, 'g', 9).arg(V[1], 18, 'g', 9).arg(V[2], 18, 'g', 9).arg(dvel, 18, 'g', 9);



                    //sJD = QString("%1").arg(t0, 15, 'f',7);

                    switch(bigType)
                    {

                    case 1:
                    {
                        sJD = QString("%1").arg(jdUTC, 15, 'f',7);
                            outerArguments.clear();

                            outerArguments << QString("-name=%1").arg(name.simplified());
                            outerArguments << QString("-type=planet");
                            outerArguments << QString("-observer=@sun");
                            //outerArguments << QString("-ep=%1").arg(t0, 15, 'f',7);
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

                            while (!ethStream.atEnd())
                            {
                                objDataStr = ethStream.readLine();
                                //
                                if(objDataStr.size()<1) continue;
                                if(objDataStr.at(0)=='#') continue;

                                qDebug() << QString("objDataStr: %1").arg(objDataStr);

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
                    }
                        break;
                    case 2:
                    {
                        //if(plaNum!=SUN_NUM&&plaNumEPM!=11)
                        //{
                        sJD = QString("%1 JD").arg(ti, 15, 'f',7);
                        str2et_c(sJD.toAscii().data(), &et);
                            sName = QString("%1 BARYCENTER").arg(name.simplified().toAscii().data());
                            qDebug() << QString("name: %1\n").arg(sName);
                            if(CENTER) spkezr_c (  sName.toAscii().data(), et, ref, "NONE", "sun", state, &lt );
                            else spkezr_c (  sName.toAscii().data(), et, ref, "NONE", "ssb", state, &lt );
                            X0[0] = state[0]/AUKM;
                            X0[1] = state[1]/AUKM;
                            X0[2] = state[2]/AUKM;
                            V0[0] = state[3]/AUKM;
                            V0[1] = state[4]/AUKM;
                            V0[2] = state[5]/AUKM;
                        //}
                    }
                        break;

                    }

                    dist = sqrt(X0[0]*X0[0] + X0[1]*X0[1] + X0[2]*X0[2]);
                    dvel = sqrt(V0[0]*V0[0] + V0[1]*V0[1] + V0[2]*V0[2]);

                    qDebug() << QString("EPH: %1|%2|%3|%4|%5|%6|%7|%8|%9|%10\n").arg(name, -10).arg(ti, 15, 'f', 6).arg(X0[0], 18, 'g', 9).arg(X0[1], 18, 'g', 9).arg(X0[2], 18, 'g', 9).arg(dist, 18, 'g', 9).arg(V0[0], 18, 'g', 9).arg(V0[1], 18, 'g', 9).arg(V0[2], 18, 'g', 9).arg(dvel, 18, 'g', 9);


                    dX[0] = X[0] - X0[0];
                    dX[1] = X[1] - X0[1];
                    dX[2] = X[2] - X0[2];

                    dist = sqrt(dX[0]*dX[0] + dX[1]*dX[1] + dX[2]*dX[2]);

                    dV[0] = V[0] - V0[0];
                    dV[1] = V[1] - V0[1];
                    dV[2] = V[2] - V0[2];

                    dvel = sqrt(dV[0]*dV[0] + dV[1]*dV[1] + dV[2]*dV[2]);

                    resStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11\n").arg(name, -10).arg(ti-tii, 18, 'f', 10).arg(dX[0], 18, 'g', 9).arg(dX[1], 18, 'g', 9).arg(dX[2], 18, 'g', 9).arg(dist, 18, 'g', 9).arg(dV[0], 18, 'g', 9).arg(dV[1], 18, 'g', 9).arg(dV[2], 18, 'g', 9).arg(dvel, 18, 'g', 9).arg(plaNum);
                    qDebug() << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10\n").arg(name, -10).arg(ti, 13, 'f', 4).arg(dX[0], 18, 'g', 9).arg(dX[1], 18, 'g', 9).arg(dX[2], 18, 'g', 9).arg(dist, 18, 'g', 9).arg(dV[0], 18, 'g', 9).arg(dV[1], 18, 'g', 9).arg(dV[2], 18, 'g', 9).arg(dvel, 18, 'g', 9);


                //}

                //resFile.close();
            //resFileDE.close();
            //resFileEPM.close();



        }
    }


resFile.close();
    
    return 0;//a.exec();
}
