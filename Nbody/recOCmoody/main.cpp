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

struct spkRecord;
int getMopName(MopState *mState, MopItem &mItem, QString name);
int body_num(QString pname);
int prepSPK(int center, int sk);
int spk2mpc();


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
    if(QString().compare(pname, "Mercury")==0) return 1;
    if(QString().compare(pname, "Venus")==0) return 2;
    if(QString().compare(pname, "Earth")==0) return 3;
    if(QString().compare(pname, "Mars")==0) return 4;
    if(QString().compare(pname, "Jupiter")==0) return 5;
    if(QString().compare(pname, "Saturn")==0) return 6;
    if(QString().compare(pname, "Uranus")==0) return 7;
    if(QString().compare(pname, "Neptune")==0) return 8;
    if(QString().compare(pname, "Pluto")==0) return 9;
    if((QString().compare(pname, "Sun")==0)||(QString().compare(pname, "Sol")==0)) return 10;


    return -1;
}

double timei, time0, time1, dtime;
int nstep;
QString sJD;

SpiceInt       handle;
SpiceInt       bodyNum;
SpiceDouble sgT0, sgT1, et;
SpiceBoolean found;
SpiceDouble             state [6];
SpiceDouble             lt;
SpiceChar             * corr;
SpiceChar             * ref;

int centr_num, status;

QString jplFile, epmDir, obsFile, obsCode, mpcCatFile, colSep;
int useEPM, bigType, smlType;
QString part_file, mop_file;
QString bspName, leapName;


dele *nbody;
observ obsPos;
QList <ParticleStruct*> pList;
mpccat mCat;
int initMpc, szObj;

int main(int argc, char *argv[]) //recOCmoody
{
    QCoreApplication a(argc, argv);

    //Variables


    int i, j, sz, p;
    int sk, center;
    MopState *mopSt;
    MopItem mopIt;

    double tmin, pmin;


    spkRecord *spkRec;
    QList <spkRecord *> spkList;


    QSettings *sett;

    /////////////////

    ref = new SpiceChar[256];
    corr = new SpiceChar[256];

    sett = new QSettings("./nb.ini", QSettings::IniFormat);

    jplFile = sett->value("general/jplFile", "./../../data/cats/binp1940_2020.405").toString();
    epmDir = sett->value("general/epmDir", "./").toString();
    obsFile = sett->value("general/obsFile", "./../../data/cats/Obs.txt").toString();
    obsCode = sett->value("general/obsCode", "500").toString();
    mpcCatFile = sett->value("general/mpcCatFile", "mocorb.txt").toString();
    useEPM = sett->value("general/useEPM", 0).toInt();
    colSep = sett->value("general/colSep", "|").toString();

    bigType = sett->value("general/bigType", 0).toInt();
    smlType = sett->value("general/smlType", 0).toInt();

    //int detMPC = sett->value("general/detMPC", 0).toInt();

    sk = sett->value("general/sk", 0).toInt();
    center = sett->value("general/center", 0).toInt();

    //moody
    part_file = sett->value("moody/part_file").toString();
    mop_file = sett->value("moody/mop_file").toString();

    //SPICE
    bspName = sett->value("SPICE/bspName", "./de421.bsp").toString();
    leapName = sett->value("SPICE/leapName", "./naif0010.tls").toString();
    sprintf(ref,"%s", sett->value("SPICE/ref", "J2000").toString().toAscii().data());
    sprintf(corr,"%s", sett->value("general/corr", "LT").toString().toAscii().data());

    furnsh_c ( leapName.toAscii().data() );     //load LSK kernel
    furnsh_c ( bspName.toAscii().data()  );     //load SPK/BSP kernel with planets ephemerides

    time0 = sett->value("time/time0", 0).toDouble();
    dtime = sett->value("time/dtime", 0).toDouble();
    nstep = sett->value("time/nstep", 0).toInt();

    time1 = time0+dtime*(nstep-1);

    sJD = QString("%1 JD TDB").arg(time0, 15, 'f',7);
    str2et_c(sJD.toAscii().data(), &sgT0);
    qDebug() << QString("sJD0: %1\tet0: %2\n").arg(sJD).arg(sgT0, 15, 'f',7);

    sJD = QString("%1 JD TDB").arg(time1, 15, 'f',7);
    str2et_c(sJD.toAscii().data(), &sgT1);
    qDebug() << QString("sJD1: %1\tet1: %2\n").arg(sJD).arg(sgT1, 15, 'f',7);



    nbody = new dele();

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



    obsPos.init(obsFile.toAscii().data(), jplFile.toAscii().data());
    obsPos.set_obs_parpam(GEOCENTR_NUM, center, sk, obsCode.toAscii().data());

    initMpc = mCat.init(mpcCatFile.toAscii().data());

    //QString inFileName(argv[1]);

    qDebug() << QString("part_file: %1\n").arg(part_file);
    if(readParticles(part_file, pList))
    {
        qDebug() << QString("readCFG error\n");
        return 1;
    }



    szObj = pList.size();
    qDebug() << QString("szObj: %1\n").arg(szObj);
///
    if(prepSPK(center, sk))
    {
        qDebug() << QString("error: prepSPK\n");
        return 1;
    }

    if(spk2mpc())
    {
        qDebug() << QString("error: spk2mpc");
        return 1;
    }


    return 0;//a.exec();
}


int spk2mpc()
{
    double range, ra, dec, timei;
    int bodyNum, i, p;

    mpc mrec;
    double jdUTC;
    QFile mpcFile;
    QTextStream mpcStm;
    char *astr = new char[256];
    QString objName, sJD;
    double state[6], lt;

    furnsh_c ( leapName.toAscii().data() );     //load LSK kernel
    furnsh_c ( bspName.toAscii().data()  );     //load SPK/BSP kernel with planets ephemerides
    furnsh_c ( "./smp.spk"  );

    mpcFile.setFileName("./mpc.txt");
    mpcFile.open(QFile::WriteOnly | QFile::Truncate);
    mpcStm.setDevice(&mpcFile);

    int szObj = pList.size();


    for(p=0; p<szObj; p++)
    {

        bodyNum = body_num(pList[p]->name.data());

        if(bodyNum!=-1)continue;
        //if(bodyNum==-1)
        //{
            if(mCat.GetRecName((char*)pList[p]->name.data())) continue;
            bodyNum = 2000000 + mCat.record->getNum();
        //}
        //bods2c_c(pList[p]->name.data(), &bodyNum, &found);
        qDebug() << QString("%1: %2\n").arg(pList[p]->name.data()).arg(bodyNum);
        objName = QString("%1").arg(bodyNum);

        for(i=1; i<nstep-1; i++)
        {
//            mopSt = mopFile.readCyclingState();
//            qDebug() << "mopSt:" << mopSt << "\n";
//            sz = mopSt->getItemCount();
            timei = time0+dtime*(i);

            sJD = QString("%1 JD TDB").arg(timei, 15, 'f',7);
            str2et_c(sJD.toAscii().data(), &et);
            //qDebug() << QString("sJD: %1\tet: %2\n").arg(sJD).arg(et, 15, 'f',7);

            spkezr_c (  objName.toAscii().data(), et, "J2000", "LT", "Earth", state, &lt );
            recrad_c(state, &range, &ra, &dec);

            qDebug() << QString("%1-%2: %3:%4\t%5\t%6\t%7\t%8\t%9\n").arg(pList[p]->name.data()).arg(bodyNum).arg(timei, 15, 'f',7).arg(state[0]/AUKM, 21, 'e',15).arg(state[1]/AUKM, 21, 'e',15).arg(state[2]/AUKM, 21, 'e',15).arg(state[3], 21, 'e',15).arg(state[4], 21, 'e',15).arg(state[5], 21, 'e',15);

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
    return 0;
}

int clearSPKdir(QString dName)
{
    QDir spkDir(dName);
    QStringList filters, filesE;
    filters << "*.*";

    filesE = spkDir.entryList(filters);
    for(int i=0; i<filesE.size(); i++)
    {
        spkDir.remove(filesE.at(i));
    }
    return 0;
}


int prepSPK(int center, int sk)
{
    int i, p;
    double coefX, coefXD;

    SpiceInt       handle;
    SpiceDouble *segmentState;
    SpiceDouble *segmentEphs;

    double X[3], V[3];
    double XS0[3], VS0[3];

    MopState *mopSt;
    MopItem mopIt;
/*
    QFile mopFile(mop_file.toAscii().data());
    mopFile.open(QFile::ReadOnly);
    QTextStream mopStm(&mopFile);

    QStringList stateList;
    QString mopStr, name;

    QString spkFileName;
    QFile spkFile;
    QTextStream spkStm;

    clearSPKdir("./moodyProject/result/spk");

    mopStr = mopStm.readAll();
    stateList = mopStr.split("\$");

    for(p=0; p<szObj; p++)
    {
        name = pList[p]->name.data();
        spkFileName = QString("./moodyProject/result/spk/%1_spk.txt").arg(name);
        spkFile.setFileName(spkFileName);
        spkFile.open(QFile::WriteOnly | QFile::Append);
        spkStm.setDevice(&spkFile);
        qDebug() << QString("name: %1\n").arg(spkFileName);

        for(i=0; i<nstep; i++)
        {
            timei = time0+dtime*(i+1);
        }
    }
  */
    MopFile mopFile;
    mopFile.setFilename(mop_file.toAscii().data());
    mopFile.resetFile();
    //mopFile.openMopfileReader();

    segmentState = new SpiceDouble[nstep*6];
    segmentEphs = new SpiceDouble[nstep];

    QDir().remove("./smp.spk");
    spkopn_c("./smp.spk", "SMP", 0, &handle);

    QFile mRecFile("./moodyProject/result/rec.txt");
    mRecFile.open(QFile::Truncate | QFile::WriteOnly);
    QTextStream mRecStm(&mRecFile);

    coefX = 1000;
    coefXD = 1000;

    for(p=0; p<szObj; p++)
    {
        mopFile.resetFile();
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
            //            qDebug() << "mopSt:" << mopSt << "\n";
//            sz = mopSt->getItemCount();
            timei = time0+dtime*i;

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
                }
                    break;
                case 2:
                {
                    sJD = QString("%1 JD").arg(timei, 15, 'f',7);
                    str2et_c(sJD.toAscii().data(), &et);
                    spkezr_c (  "sun", et, ref, "NONE", "ssb", state, &lt );
                    XS0[0] = state[0]/AUKM;
                    XS0[1] = state[1]/AUKM;
                    XS0[2] = state[2]/AUKM;
                    VS0[0] = state[3]/AUKM*SECINDAY;
                    VS0[1] = state[4]/AUKM*SECINDAY;
                    VS0[2] = state[5]/AUKM*SECINDAY;
                }
                    break;
                }

                XS0[0] *= AUKM*1000.0;
                XS0[1] *= AUKM*1000.0;
                XS0[2] *= AUKM*1000.0;
                VS0[0] *= AUKM*1000.0/SECINDAY;
                VS0[1] *= AUKM*1000.0/SECINDAY;
                VS0[2] *= AUKM*1000.0/SECINDAY;
            }

            //qDebug() << QString("Sun: %1\t%2\t%3\t%4\t%5\t%6").arg(XS0[0], 21, 'e',15).arg(XS0[1], 21, 'e',15).arg(XS0[2], 21, 'e',15).arg(VS0[0], 21, 'e',15).arg(VS0[1], 21, 'e',15).arg(VS0[2], 21, 'e',15);

            sJD = QString("%1 JD TDB").arg(timei, 15, 'f',7);
            str2et_c(sJD.toAscii().data(), &et);
            segmentEphs[i] = et;

            if(i==0)
            {
                X[0] = pList[p]->x + XS0[0];
                X[1] = pList[p]->y + XS0[1];
                X[2] = pList[p]->z + XS0[2];

                V[0] = pList[p]->xd + VS0[0];
                V[1] = pList[p]->yd + VS0[1];
                V[2] = pList[p]->zd + VS0[2];
            }
            else
            {


                mopSt = mopFile.readState();

                if(getMopName(mopSt, mopIt, QString(pList[p]->name.data()))==-1)continue;

                X[0] = mopIt.x + XS0[0];
                X[1] = mopIt.y + XS0[1];
                X[2] = mopIt.z + XS0[2];

                V[0] = mopIt.xd + VS0[0];
                V[1] = mopIt.yd + VS0[1];
                V[2] = mopIt.zd + VS0[2];
            }


            mRecStm << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(pList[p]->name.data()).arg(timei, 15, 'f',7).arg(X[0]/1000.0/AUKM, 21, 'e',15).arg(X[1]/1000.0/AUKM, 21, 'e',15).arg(X[2]/1000.0/AUKM, 21, 'e',15).arg(V[0]/1000.0/AUKM*SECINDAY, 21, 'e',15).arg(V[1]/1000.0/AUKM*SECINDAY, 21, 'e',15).arg(V[2]/1000.0/AUKM*SECINDAY, 21, 'e',15);


                segmentState[i*6+0] = X[0]/coefX;
                segmentState[i*6+1] = X[1]/coefX;
                segmentState[i*6+2] = X[2]/coefX;
                segmentState[i*6+3] = V[0]/coefXD;
                segmentState[i*6+4] = V[1]/coefXD;
                segmentState[i*6+5] = V[2]/coefXD;

//qDebug() << QString("%1-%2: %3:%4\t%5\t%6\t%7\t%8\t%9\n").arg(pList[p]->name.data()).arg(bodyNum).arg(timei, 15, 'f',7).arg(segmentState[i*6+0], 21, 'e',15).arg(segmentState[i*6+1], 21, 'e',15).arg(segmentState[i*6+2], 21, 'e',15).arg(segmentState[i*6+3], 21, 'e',15).arg(segmentState[i*6+4], 21, 'e',15).arg(segmentState[i*6+5], 21, 'e',15);

                /*if(*found==false)
                {
                    if(mCat.GetRecName(mopIt.name)) continue;
                    bodyNum = 2000000 + mCat.record->getNum();
                }*/

                //qDebug() << QString("%1-%2: %3:%4\t%5\t%6\t%7\t%8\t%9\n").arg(mopIt.name).arg(bodyNum).arg(time).arg(X[0]).arg(X[1]).arg(X[2]).arg(V[0]).arg(V[1]).arg(V[2]);


        }

        spkw09_c(handle, bodyNum, 0, "J2000", sgT0, sgT1, "SPK_STATES_09", 9, nstep, segmentState, segmentEphs);

        //delete [] segmentState;
        //delete [] segmentEphs;
    }

    spkcls_c(handle);
    mRecFile.close();
    return 0;
}
