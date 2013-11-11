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
#include "./../../libs/myDomMoody.h"

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

int body_num(QString pname)
{
    if(QString().compare(pname, "Mercury")==0) return 199;
    if(QString().compare(pname, "Venus")==0) return 299;
    if(QString().compare(pname, "Earth")==0) return 399;
    if(QString().compare(pname, "Mars")==0) return 499;
    if(QString().compare(pname, "Jupiter")==0) return 599;
    if(QString().compare(pname, "Saturn")==0) return 699;
    if(QString().compare(pname, "Uran")==0) return 799;
    if(QString().compare(pname, "Neptune")==0) return 899;
    if(QString().compare(pname, "Pluto")==0) return 999;
    if(QString().compare(pname, "Sun")==0) return 10;


    return -1;
}


int main(int argc, char *argv[]) //recOCmoody res.mop
{
    QCoreApplication a(argc, argv);

    int i, j, sz, p;
    MopState *mopSt;
    MopItem mopIt;

    double tmin, pmin;
    SpiceInt       handle;
    SpiceInt       bodyNum;
    SpiceDouble sgT0, sgT1;
    SpiceDouble *segmentState;
    SpiceDouble *segmentEphs;
    SpiceBoolean found;
    double range, ra, dec;


    spkRecord *spkRec;
    QList <spkRecord *> spkList;
    observ obsPos;

    SpiceDouble             state [6];
    SpiceDouble             lt;
    SpiceChar             * corr;
    SpiceChar             * ref;
    ref = new SpiceChar[256];
    corr = new SpiceChar[256];


    double timei, time0, time1, dt, et;
    int nstep;
    double X[3], V[3];
    double XS0[3], VS0[3];

    QList <ParticleStruct*> pList;

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

    //moody
    QString part_file = sett->value("moody/part_file").toString();
    QString mop_file = sett->value("moody/mop_file").toString();

    //SPICE
    QString bspName = sett->value("SPICE/bspName", "./de421.bsp").toString();
    QString leapName = sett->value("SPICE/leapName", "./naif0010.tls").toString();
    sprintf(ref,"%s", sett->value("SPICE/ref", "J2000").toString().toAscii().data());
    sprintf(corr,"%s", sett->value("general/corr", "LT").toString().toAscii().data());

    time0 = sett->value("general/time0", 0).toDouble();
    dt = sett->value("general/dt", 0).toDouble();
    nstep = sett->value("general/nstep", 0).toInt();

    time1 = time0+dt*(nstep-1);

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

    //if(bigType==2)
    //{
        furnsh_c ( leapName.toAscii().data() );     //load LSK kernel
        furnsh_c ( bspName.toAscii().data()  );     //load SPK/BSP kernel with planets ephemerides
//        furnsh_c ( "./codes_300ast.tf"  );
    //}

    obsPos.init(obsFile.toAscii().data(), jplFile.toAscii().data());
    obsPos.set_obs_parpam(GEOCENTR_NUM, center, sk, obsCode.toAscii().data());

    mpccat mCat;
    int initMpc = mCat.init(mpcCatFile.toAscii().data());

    //QString inFileName(argv[1]);

    qDebug() << QString("part_file: %1\n").arg(part_file);
    if(readCFG(part_file, pList))
    {
        qDebug() << QString("readCFG error\n");
        return 1;
    }

    MopFile mopFile;
    mopFile.setFilename(mop_file.toAscii().data());
    mopFile.resetFile();
    //mopFile.openMopfileReader();



    segmentState = new SpiceDouble[nstep*6];
    segmentEphs = new SpiceDouble[nstep];


    sJD = QString("%1 JD TDB").arg(time0, 15, 'f',7);
    str2et_c(sJD.toAscii().data(), &sgT0);
    sJD = QString("%1 JD TDB").arg(time1, 15, 'f',7);
    str2et_c(sJD.toAscii().data(), &sgT1);

    QDir().remove("./smp.spk");
    spkopn_c("./smp.spk", "SMP", 0, &handle);

    int szObj = pList.size();
    qDebug() << QString("szObj: %1\n").arg(szObj);

    for(p=0; p<szObj; p++)
    {
        bodyNum = body_num(pList[p]->name.data());

        if(bodyNum==-1)
        {
            if(mCat.GetRecName((char*)pList[p]->name.data())) continue;
            bodyNum = 2000000 + mCat.record->getNum();
        }
        //bods2c_c(pList[p]->name.data(), &bodyNum, &found);
        qDebug() << QString("%1: %2\n").arg(pList[p]->name.data()).arg(bodyNum);

        for(i=0; i<nstep; i++)
        {
            mopSt = mopFile.readCyclingState();
//            qDebug() << "mopSt:" << mopSt << "\n";
//            sz = mopSt->getItemCount();
            timei = time0+dt*i;
/*
            if(center) //helio to barycenter
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
*/
            if(getMopName(mopSt, mopIt, QString(pList[p]->name.data()))==-1)continue;

                X[0] = mopIt.x;// + XS0[0];
                X[1] = mopIt.y;// + XS0[1];
                X[2] = mopIt.z;// + XS0[2];

                V[0] = mopIt.xd;// + VS0[0];
                V[1] = mopIt.yd;// + VS0[1];
                V[2] = mopIt.zd;// + VS0[2];


                sJD = QString("%1 JD TDB").arg(timei, 15, 'f',7);
                str2et_c(sJD.toAscii().data(), &et);
                segmentEphs[i] = et;

                segmentState[i*6+0] = mopIt.x*AUKM;
                segmentState[i*6+1] = mopIt.y*AUKM;
                segmentState[i*6+2] = mopIt.z*AUKM;
                segmentState[i*6+3] = mopIt.xd*AUKM;///SECINDAY;
                segmentState[i*6+4] = mopIt.yd*AUKM;//SECINDAY;
                segmentState[i*6+5] = mopIt.zd*AUKM;//SECINDAY;



                /*if(*found==false)
                {
                    if(mCat.GetRecName(mopIt.name)) continue;
                    bodyNum = 2000000 + mCat.record->getNum();
                }*/

                //qDebug() << QString("%1-%2: %3:%4\t%5\t%6\t%7\t%8\t%9\n").arg(mopIt.name).arg(bodyNum).arg(time).arg(X[0]).arg(X[1]).arg(X[2]).arg(V[0]).arg(V[1]).arg(V[2]);


        }

        spkw13_c(handle, bodyNum, 0, "J2000", sgT0, sgT1, "SPK_STATES_13", 7, nstep, segmentState, segmentEphs);

        //delete [] segmentState;
        //delete [] segmentEphs;
    }

    spkcls_c(handle);

    mpc mrec;
    double jdUTC;
    QFile mpcFile;
    QTextStream mpcStm;
    char *astr = new char[256];
    QString objName;
    //double state[6], lt;

    furnsh_c ( leapName.toAscii().data() );     //load LSK kernel
    furnsh_c ( bspName.toAscii().data()  );     //load SPK/BSP kernel with planets ephemerides
    furnsh_c ( "./smp.spk"  );

    mpcFile.setFileName("./mpc.txt");
    mpcFile.open(QFile::WriteOnly | QFile::Truncate);
    mpcStm.setDevice(&mpcFile);

    for(p=0; p<szObj; p++)
    {
        objName = QString(pList[p]->name.data());
        bodyNum = body_num(pList[p]->name.data());

        if(bodyNum!=-1)continue;
        //if(bodyNum==-1)
        //{
            if(mCat.GetRecName((char*)pList[p]->name.data())) continue;
            bodyNum = 2000000 + mCat.record->getNum();
        //}
        //bods2c_c(pList[p]->name.data(), &bodyNum, &found);
        qDebug() << QString("%1: %2\n").arg(pList[p]->name.data()).arg(bodyNum);

        for(i=0; i<nstep; i++)
        {
//            mopSt = mopFile.readCyclingState();
//            qDebug() << "mopSt:" << mopSt << "\n";
//            sz = mopSt->getItemCount();
            timei = time0+dt*i;

            sJD = QString("%1 JD TDB").arg(timei, 15, 'f',7);
            qDebug() << QString("sJD: %1\n").arg(sJD);
            str2et_c(sJD.toAscii().data(), &et);

            spkezr_c (  objName.toAscii().data(), et, "J2000", "LT", "Earth", state, &lt );
            recrad_c(state, &range, &ra, &dec);

            mrec.r = ra;// + dRa;
            mrec.d = dec;// + dDec;

            //tstr = QString(utctim);
            //jdUTC = mjd2jd(getMJDfromStrT(tstr));
            TDB2UTC(timei, &jdUTC);
            mrec.eJD = jdUTC;//obsPos.ctime.UTC();
            //mrec.num = 1;

            //qDebug() << QString("ut: %1\teJD: %2\n").arg(ut, 15, 'f', 7).arg(mrec.eJD, 15, 'f', 7);

            if(!initMpc)mCat.record->getNumStr(mrec.head->Snum);
            else mrec.head->set_Snum(1);

            //strcpy(, mCat.record->getNumStr(>number);
            mrec.tail->set_numOfObs("500");
            mrec.toString(astr);

            mpcStm << astr << "\n";

        }


    }

    mpcFile.close();
    return 0;//a.exec();
}

