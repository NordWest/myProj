#include <QCoreApplication>

#define OMPLIB

#include <SpiceUsr.h>
#include <astro.h>
#include "./../../libs/comfunc.h"
#include "./../../libs/calc_epm.h"
#include "./../../libs/mpccat.h"
#include "./../../libs/mpcs.h"
#include "./../../libs/observ.h"
#include "./../../libs/moody/moody.h"

struct spkRecord
{
    double time, X[3], V[3];
    QString toString()
    {
        return(QString("%1, %2, %3, %4, %5, %6, %7").arg(time, 15, 'f', 8).arg(X[0], 18, 'g', 12).arg(X[1], 18, 'g', 12).arg(X[2], 18, 'g', 12).arg(V[0], 18, 'g', 12).arg(V[1], 18, 'g', 12).arg(V[2], 18, 'g', 12));
    };
    int fromString(QString inStr)
    {
        QStringList opers;
        opers = inStr.split(", ");
        if(opers.size()<7) return 1;
        time = opers[0].toDouble();
        X[0] = opers[1].toDouble();
        X[1] = opers[2].toDouble();
        X[2] = opers[3].toDouble();
        V[0] = opers[4].toDouble();
        V[1] = opers[5].toDouble();
        V[2] = opers[6].toDouble();
        return 0;
    };
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int i, j, sz;

    spkRecord *spkRec;
    QList <spkRecord *> spkList;
    observ obsPos;

    SpiceDouble             state [6];
    SpiceDouble             lt;
    SpiceChar             * corr;
    SpiceChar             * ref;
    ref = new SpiceChar[256];
    corr = new SpiceChar[256];


    double time, time0, dt, et;
    int nstep;
    double X[3], V[3];
    double XS0[3], VS0[3];

    QString sJD;

    QSettings *sett = new QSettings("./nb.ini", QSettings::IniFormat);

    QString jplFile = sett->value("general/jplFile", "./../../data/cats/binp1940_2020.405").toString();
    QString epmDir = sett->value("general/epmDir", "./").toString();
    QString obsFile = sett->value("general/obsFile", "./../../data/cats/Obs.txt").toString();
    QString obsCode = sett->value("general/obsCode", "500").toString();
    QString mpcCatFile = sett->value("general/mpcCatFile", "mocorb.txt").toString();
    int useEPM = sett->value("general/useEPM", 0).toInt();
    QString colSep = sett->value("general/colSep", "|").toString();

    int bigType = sett->value("general/bigType", 0).toInt();
    int smlType = sett->value("general/smlType", 0).toInt();

    //int detMPC = sett->value("general/detMPC", 0).toInt();

    int sk = sett->value("general/sk", 0).toInt();
    int center = sett->value("general/center", 0).toInt();

    //SPICE
    QString bspName = sett->value("SPICE/bspName", "./de421.bsp").toString();
    QString leapName = sett->value("SPICE/leapName", "./naif0010.tls").toString();
    sprintf(ref,"%s", sett->value("SPICE/ref", "J2000").toString().toAscii().data());
    sprintf(corr,"%s", sett->value("general/corr", "LT").toString().toAscii().data());

    time0 = sett->value("general/time0", 0).toDouble();
    dt = sett->value("general/dt", 0).toDouble();
    nstep = sett->value("general/nstep", 0).toInt();

    dele *nbody;
    nbody = new dele();
    int centr_num;
    int status;
    int plaNum;


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

    if(bigType==2)
    {
        furnsh_c ( leapName.toAscii().data() );     //load LSK kernel
        furnsh_c ( bspName.toAscii().data()  );     //load SPK/BSP kernel with planets ephemerides
    }

    obsPos.init(obsFile.toAscii().data(), jplFile.toAscii().data());
    obsPos.set_obs_parpam(GEOCENTR_NUM, center, sk, obsCode.toAscii().data());

    mpccat mCat;
    int initMpc = mCat.init(mpcCatFile.toAscii().data());

    QString inFileName(argv[1]);

    MopFile mopFile;
    mopFile.setFilename(inFileName.toAscii().data());
    //mopFile.openMopfileReader();

    MopState *mopSt;
    MopItem mopIt;

    for(i=0; i<nstep; i++)
    {
        mopSt = mopFile.readCyclingState();
        sz = mopSt->getItemCount();
        time = time0+dt;

        if(center) //gelio to barycenter
        {
            switch(bigType)
            {
            case 0:
            {
                if(useEPM)
                {
                    status = calc_EPM(SUN_NUM, centr_num, (int)time, time-(int)time, XS0, VS0);
                     if(!status)
                     {
                         qDebug() << QString("error EPM\n");
                         return 1;
                     }
                }
                else nbody->detState(&XS0[0], &XS0[1], &XS0[2], &VS0[0], &VS0[1], &VS0[2], time, SUN_NUM, CENTER_BARY, sk);
            }
                break;
            case 2:
            {
                sJD = QString("%1 JD").arg(time, 15, 'f',7);
                str2et_c(sJD.toAscii().data(), &et);
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
        }

        qDebug() << QString("Sun: %1\t%2\t%3\t%4\t%5\t%6").arg(XS0[0]).arg(XS0[1]).arg(XS0[2]).arg(VS0[0]).arg(VS0[1]).arg(VS0[2]);

        for(j=0; j<sz; j++)
        {
            mopIt = mopSt->getMopItem(j);
            //mopIt = mopSt->getContent();
            X[0] = mopIt.x;// + XS0[0];
            X[1] = mopIt.y;// + XS0[1];
            X[2] = mopIt.z;// + XS0[2];

            V[0] = mopIt.xd;// + VS0[0];
            V[1] = mopIt.yd;// + VS0[1];
            V[2] = mopIt.zd;// + VS0[2];

            qDebug() << QString("%1:%2: %3\t%4\t%5\t%6\t%7\t%8\n").arg(mopIt.name).arg(time).arg(XS0[0]).arg(X[1]).arg(X[2]).arg(V[0]).arg(V[1]).arg(V[2]);

        }
    }


    
    return 0;//a.exec();
}
