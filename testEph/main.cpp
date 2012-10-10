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


    dele *nbody;


    nbody = new dele();
    double t0, nstep, ti, dt;
    int centerNum, skNum;
    int i, j, plaNum;

    QSettings *sett = new QSettings("./nb.ini", QSettings::IniFormat);

    QString jplFile = sett->value("general/jplFile", "./../../data/cats/binp1940_2020.405").toString();
    t0 = sett->value("general/time0", 0).toDouble();
    dt = sett->value("general/dt", 1).toDouble();
    nstep = sett->value("general/nstep", 1).toDouble();
    QString confFile = sett->value("general/confFile", "testMajor.xml").toString();

    centerNum = sett->value("general/center", 0).toInt();
    skNum = sett->value("general/sk", 0).toInt();

    QString resDirName("./resEph");
    QDir resDir(resDirName);

    QFile resFile;
    QTextStream resStm;
    QString resFileName;

    QString name;

//    plaNum = 11;

    int iniJplRes = nbody->init(jplFile.toAscii().data());
    if(iniJplRes) return 1;

    double X[3], V[3];
    double x, y, z, vx, vy, vz;

    if(readCFG(confFile, pList))
    {
        qDebug() << QString("readCFG error\n");
        return 1;
    }

    nofzbody = pList.size();


    for(i=0; i<nofzbody; i++)
    {
        name = QString(pList[i]->name.data());
        if(QString().compare(name, "Sol")==0) plaNum = 10;
        else plaNum = planet_num(name.toAscii().data());

        resFileName = QString("%1/%2.txt").arg(resDir.absolutePath()).arg(name);
        qDebug() << QString("%1\n").arg(resFileName);
        resFile.setFileName(resFileName);
        resFile.open(QFile::WriteOnly | QFile::Truncate);
        resStm.setDevice(&resFile);

        for(j=0; j<nstep; j++)
        {
            ti = t0+dt*j;

            nbody->detR(&x, &y, &z, ti, plaNum, 0, centerNum, skNum);
            nbody->detR(&vx, &vy, &vz, ti, plaNum, 1, centerNum, skNum);

            resStm << QString("%1|%2|%3|%4|%5|%6|%7\n").arg(ti, 12, 'f', 4).arg(x, 20, 'e', 12).arg(y, 20, 'e', 12).arg(z, 20, 'e', 12).arg(vx, 20, 'e', 12).arg(vy, 20, 'e', 12).arg(vz, 20, 'e', 12);

        }

        resFile.close();

    }


    
    return 0;//a.exec();
}
