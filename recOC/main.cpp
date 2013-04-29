#include <QtCore>

#include "./../libs/dele.h"
#include "./../libs/astro.h"
#include "./../libs/comfunc.h"
#include "./../libs/calc_epm.h"
#include "./../libs/mpccat.h"
#include "./../libs/mpcs.h"

#include "./../libs/cspice/SpiceUsr.h"
#include "./../libs/observ.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    QString tStr, name, sJD;
    QStringList dataSL, resSL, outerArguments;
    double time, jdUTC, jdTDB;
    double X[3], V[3];
    double Q[3], QV[3];
    double X1[3], V1[3];
    double dX[3], dV[3];
    double X0[3], V0[3];
    double XE0[3], VE0[3];
    double XEB0[3], VEB0[3];
    double XS0[3], VS0[3];
    double dist, dvel;
    double ra, de;
    int isObj, i;
    int SK, CENTER;
    QProcess outerProcess;
    QString objDataStr, strT, sName;
    mpc mrec;
    double dT, m, n;
    double dRa, dDec, normR, norm_sA;
    double T, mPrec, nPrec;
    double *s, *sA, *R, *P, *sV;
    double *v0, *v1, *v2;
    /*v0 = new myvector(3);
    v1 = new myvector(3);
    v2 = new myvector(3);*/
    s = new double[3];
    sA = new double[3];
    sV = new double[3];
    R = new double[3];
    P = new double[3];
    v0 = new double[3];
    v1 = new double[3];
    v2 = new double[3];
    double *Qb;
    Qb = new double[3];

    observ obsPos;

    double muc2 = 9.8704e-9;
    double ct0, ct1, tau, et;
    double normE, normQ, normP, range;

    SpiceDouble             state [6];
    SpiceDouble             lt;
    SpiceChar             * corr;
    SpiceChar             * ref;
    ref = new SpiceChar[256];
    corr = new SpiceChar[256];

    procData miriadeProcData;

    QSettings *sett = new QSettings("./pnb.ini", QSettings::IniFormat);

    QString jplFile = sett->value("general/jplFile", "./../../data/cats/binp1940_2020.405").toString();
    QString epmDir = sett->value("general/epmDir", "./").toString();
    QString obsFile = sett->value("general/obsFile", "./../../data/cats/Obs.txt").toString();
    QString obsCode = sett->value("general/obsCode", "500").toString();
    QString mpcCatFile = sett->value("general/mpcCatFile", "mocorb.txt").toString();
    int useEPM = sett->value("general/useEPM", 0).toInt();
    QString colSep = sett->value("general/colSep", "|").toString();

    int bigType = sett->value("general/bigType", 0).toInt();
    int smlType = sett->value("general/smlType", 0).toInt();

    int detMPC = sett->value("general/detMPC", 0).toInt();

    SK = sett->value("general/sk", 0).toInt();
    CENTER = sett->value("general/center", 0).toInt();

    miriadeProcData.name = sett->value("miriadeProcData/name", "./mpeph.exe").toString();
    miriadeProcData.folder = sett->value("miriadeProcData/folder", "./").toString();
    miriadeProcData.waitTime = sett->value("miriadeProcData/waitTime", -1).toInt();
    if(miriadeProcData.waitTime>0) miriadeProcData.waitTime *= 1000;

    //SPICE
    QString bspName = sett->value("SPICE/bspName", "./de421.bsp").toString();
    QString leapName = sett->value("SPICE/leapName", "./naif0010.tls").toString();
    sprintf(ref,"%s", sett->value("SPICE/ref", "J2000").toString().toAscii().data());
    sprintf(corr,"%s", sett->value("general/corr", "LT").toString().toAscii().data());

    dele *nbody;
    nbody = new dele();
    int centr_num;
    int status;
    int plaNum;
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

    obsPos.init(obsFile, bspName, leapName);
    obsPos.set_spice_parpam("Earth", "500", "sun", "J2000");

    mpccat mCat;
    int initMpc = mCat.init(mpcCatFile.toAscii().data());

    QString inFileName(argv[1]);
    QFileInfo fi(inFileName);
    QString dxFileName = QString("%1_dr.txt").arg(fi.absoluteFilePath());
    QString mpcFileName = QString("%1_mpc.txt").arg(fi.absoluteFilePath());
    QString spkFileName = QString("%1_spk.txt").arg(fi.absoluteFilePath());

    //QString inFileName("small.log");
    QFile inFile(inFileName);
    inFile.open(QFile::ReadOnly);
    QTextStream inStm(&inFile);

    QString bigFileName("big.log");
    QFile bigFile(bigFileName);
    bigFile.open(QFile::ReadOnly);
    QTextStream bigStm(&bigFile);

    //QString dxFileName("small_dr.txt");
    QFile dxFile(dxFileName);
    dxFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream dxStm(&dxFile);

    QFile spkFile;
    QTextStream spkStm;

    char *astr = new char[256];
    QFile mpcFile(mpcFileName);
    mpcFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream mpcStm(&mpcFile);

    QDir spkDir("./spks");
    QStringList filters, filesE;
    filters << "*.*";

    filesE = spkDir.entryList(filters);
    for(i=0; i<filesE.size(); i++)
    {
        spkDir.remove(filesE.at(i));
    }

    while(!inStm.atEnd())
    {
        tStr = inStm.readLine();
        dataSL = tStr.split(colSep, QString::SkipEmptyParts);
        if(dataSL.size()<8) continue;

        name = dataSL.at(0).simplified();
        time = dataSL.at(1).toDouble();

        X[0] = dataSL.at(2).toDouble();
        X[1] = dataSL.at(3).toDouble();
        X[2] = dataSL.at(4).toDouble();
        V[0] = dataSL.at(5).toDouble();
        V[1] = dataSL.at(6).toDouble();
        V[2] = dataSL.at(7).toDouble();

        qDebug() << QString("X: %1\t%2\t%3\nV: %4\t%5\t%6\n").arg(X[0]).arg(X[1]).arg(X[2]).arg(V[0]).arg(V[1]).arg(V[2]);


        TDB2UTC(time, &jdUTC);
        sJD = QString("%1").arg(jdUTC, 15, 'f',7);
        jdTDB = TDB2TDT(time);

        if(useEPM) plaNum = epm_planet_num(name);
        else plaNum = planet_num(name.toAscii().data());
        if(plaNum!=-1)
        {
            switch(bigType)
            {

            case 0:
            {

                if(useEPM)
                {
                    status = calc_EPM(plaNum, centr_num, (int)time, time-(int)time, X0, V0);
                     if(!status)
                     {
                         qDebug() << QString("error EPM\n");
                         return 1;
                     }
                }
                else nbody->detState(&X0[0], &X0[1], &X0[2], &V0[0], &V0[1], &V0[2], time, plaNum, CENTER, SK);
            }
                break;
            case 2:
            {
                if(plaNum!=SUN_NUM)
                {
                    sName = QString("%1 BARYCENTER").arg(name.simplified().toAscii().data());
                    qDebug() << QString("name: %1\n").arg(sName);
                    sJD = QString("%1 JD").arg(time, 15, 'f',7);
                    str2et_c(sJD.toAscii().data(), &et);
                    if(CENTER) spkezr_c (  sName.toAscii().data(), et, ref, "NONE", "sun", state, &lt );
                    else spkezr_c (  sName.toAscii().data(), et, ref, "NONE", "ssb", state, &lt );
                    X0[0] = state[0]/AUKM;
                    X0[1] = state[1]/AUKM;
                    X0[2] = state[2]/AUKM;
                    V0[0] = state[3]/AUKM;
                    V0[1] = state[4]/AUKM;
                    V0[2] = state[5]/AUKM;
                }
            }
            break;
            }
        }
        else
        {
            switch(bigType)
            {
            case 0:
            {
                if(useEPM)
                {
                    status = calc_EPM(3, centr_num, (int)time, time-(int)time, XE0, VE0);
                     if(!status)
                     {
                         qDebug() << QString("error EPM\n");
                         return 1;
                     }
                }
                else nbody->detState(&XE0[0], &XE0[1], &XE0[2], &VE0[0], &VE0[1], &VE0[2], time, GEOCENTR_NUM, CENTER_SUN, SK);
                if(useEPM)
                {
                    status = calc_EPM(3, epm_planet_num("Sun"), (int)time, time-(int)time, XEB0, VEB0);
                     if(!status)
                     {
                         qDebug() << QString("error EPM\n");
                         return 1;
                     }
                }
                else nbody->detState(&XEB0[0], &XEB0[1], &XEB0[2], &VEB0[0], &VEB0[1], &VEB0[2], time, GEOCENTR_NUM, CENTER_BARY, SK);
            }
                break;
            case 1:

                while(!bigStm.atEnd())
                {
                    tStr = bigStm.readLine();
                    dataSL = tStr.split(colSep, QString::SkipEmptyParts);
                    if(dataSL.size()<8) continue;

                    //name = dataSL.at(0).simplified();
                    if(QString().compare(dataSL.at(0).simplified(), "Earth")!=0) continue;
                    //time = dataSL.at(1).toDouble();

                    XE0[0] = dataSL.at(2).toDouble();
                    XE0[1] = dataSL.at(3).toDouble();
                    XE0[2] = dataSL.at(4).toDouble();
                    VE0[0] = dataSL.at(5).toDouble();
                    VE0[1] = dataSL.at(6).toDouble();
                    VE0[2] = dataSL.at(7).toDouble();

                    break;
                }
                break;
            case 2:
            {
                obsPos.setTDB(time);
                obsPos.det_observ();
                XE0[0] = obsPos.pos[0];
                XE0[1] = obsPos.pos[1];
                XE0[2] = obsPos.pos[2];
                VE0[0] = obsPos.vel[0];
                VE0[1] = obsPos.vel[1];
                VE0[2] = obsPos.vel[2];
            }
                break;
            }

            qDebug() << QString("XE0: %1\t%2\t%3\nVE0: %4\t%5\t%6\n").arg(XE0[0]).arg(XE0[1]).arg(XE0[2]).arg(VE0[0]).arg(VE0[1]).arg(VE0[2]);
/*



            outerArguments.clear();

            outerArguments << QString("-name=earth");
            outerArguments << QString("-type=planet");
            outerArguments << QString("-teph=1");
            outerArguments << QString("-observer=@sun");
            outerArguments << QString("-ep=%1").arg(sJD);
            //outerArguments << QString("-ep=%1").arg(time, 15, 'f',7);
            outerArguments << "-tcoor=2";
            outerArguments << "-rplane=1";

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
                qDebug() << QString("objDataStr: %1").arg(objDataStr);
                if(objDataStr.size()<1) continue;
                if(objDataStr.at(0)=='#') continue;

                resSL =  objDataStr.split(" ", QString::SkipEmptyParts);
                if(resSL.size()<8) continue;
                isObj = 1;
                XE0[0] = resSL.at(1).toDouble();
                XE0[1] = resSL.at(2).toDouble();
                XE0[2] = resSL.at(3).toDouble();
                VE0[0] = resSL.at(5).toDouble();
                VE0[1] = resSL.at(6).toDouble();
                VE0[2] = resSL.at(7).toDouble();
                break;
            }
*/
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
                else nbody->detState(&XS0[0], &XS0[1], &XS0[2], &VS0[0], &VS0[1], &VS0[2], time, SUN_NUM, CENTER_BARY, SK);
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

            qDebug() << QString("XS0: %1\t%2\t%3\t%4\t%5\t%6").arg(XS0[0]).arg(XS0[1]).arg(XS0[2]).arg(VS0[0]).arg(VS0[1]).arg(VS0[2]);

//Q
            X1[0] = X[0] + XS0[0];
            X1[1] = X[1] + XS0[1];
            X1[2] = X[2] + XS0[2];

            V1[0] = V[0] + VS0[0];
            V1[1] = V[1] + VS0[1];
            V1[2] = V[2] + VS0[2];

            getStrTfromMJD(&strT, jd2mjd(time));

            spkFileName = QString("%1/spks/%2_spk.txt").arg(fi.absolutePath()).arg(name);
            spkFile.setFileName(spkFileName);
            spkFile.open(QFile::WriteOnly | QFile::Append);
            spkStm.setDevice(&spkFile);
            qDebug() << QString("name: %1\n").arg(spkFileName);


            spkStm << QString("%1, %2, %3, %4, %5, %6, %7\n").arg(time, 15, 'f', 8).arg(X1[0], 18, 'g', 12).arg(X1[1], 18, 'g', 12).arg(X1[2], 18, 'g', 12).arg(V1[0]/SECINDAY, 18, 'g', 12).arg(V1[1]/SECINDAY, 18, 'g', 12).arg(V1[2]/SECINDAY, 18, 'g', 12);

            spkFile.close();
/*
            T = (time - 2451545)/36525.0;
            mPrec = 4612.4362*T;
            nPrec = 2004.3109*T;

            qDebug() << QString("T: %3\nm: %1\tn:%2\n").arg(mPrec).arg(nPrec).arg(T);
*/


            qDebug() << QString("XE0: %1\t%2\t%3\nVE0: %4\t%5\t%6\n").arg(XE0[0]).arg(XE0[1]).arg(XE0[2]).arg(VE0[0]).arg(VE0[1]).arg(VE0[2]);
//P
            /*
            R[0] = X[0] - XE0[0];
            R[1] = X[1] - XE0[1];
            R[2] = X[2] - XE0[2];

            normR = norm(R);

            normE = norm(XE0);
            normP = normR;
            normQ = norm(X);

            ct1 = 0.0;


            do
            {

                ct0 = ct1;
                tau = ct1/CAU;

                Qb[0] = X1[0] - V1[0]*tau;
                Qb[1] = X1[1] - V1[1]*tau;
                Qb[2] = X1[2] - V1[2]*tau;

                P[0] = Qb[0] - XEB0[0];
                P[1] = Qb[1] - XEB0[1];
                P[2] = Qb[2] - XEB0[2];

                normP = norm(P);

                Q[0] = Qb[0] - XS0[0];
                Q[1] = Qb[1] - XS0[1];
                Q[2] = Qb[2] - XS0[2];

                normQ = norm(Q);

                qDebug() << QString("normQ: %1\n").arg(normQ);
                qDebug() << QString("normP: %1\n").arg(normP);
                qDebug() << QString("normE: %1\n").arg(normE);

                ct1 = normP+2.0*muc2*log((normE+normQ+normP)/(normE+normQ-normP));
                //qDebug() << QString("ct1: %1\n").arg(ct1, 10);

                qDebug() << QString("dct1: %1\n").arg(fabs(ct1-ct0)/fabs(ct1), 10);
                qDebug() << QString("tau: %1\n").arg(tau*86400.0);//qDebug() << QString("normQ: %1\n").arg(normQ);

            }while((fabs(ct1-ct0)/fabs(ct1))>1e-10);

            tau = ct1/CAU;
*/


            /*

            s[0] = R[0]/normR;
            s[1] = R[1]/normR;
            s[2] = R[2]/normR;

            sV[0] = V[0] - VE0[0];
            sV[1] = V[1] - VE0[1];
            sV[2] = V[2] - VE0[2];

            Vmul3(v0, s, sV);
            Vmul3(v1, s, v0);

            sA[0] = s[0] + v1[0]*(1.0/CAU);
            sA[1] = s[1] + v1[1]*(1.0/CAU);
            sA[2] = s[2] + v1[2]*(1.0/CAU);

            norm_sA = norm(sA);
/*
            sA[0] = sA[0]/norm_sA;
            sA[1] = sA[1]/norm_sA;
            sA[2] = sA[2]/norm_sA;
*/

            if(detMPC)
            {
                outerArguments.clear();

                outerArguments << QString("-name=%1").arg(name.simplified().toLower());

                outerArguments << QString("-ep=%1").arg(sJD);
                //outerArguments << QString("-ep=%1").arg(time, 15, 'f',7);

        /*        if(useEPM) plaNum = epm_planet_num(name);
                else plaNum = planet_num(name.toAscii().data());
                if(plaNum!=-1) outerArguments << "-type=planet";
                else */

                outerArguments << "-type=aster";

                outerArguments << QString("-observer=%1").arg(obsCode);
                //outerArguments << QString("-observer=@sun");
                outerArguments << "-tcoor=2";
                outerArguments << "-rplane=1";

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

                QTextStream objStream(outerProcess.readAllStandardOutput());

                while (!objStream.atEnd())
                {
                    objDataStr = objStream.readLine();
                    //qDebug() << QString("objDataStr: %1").arg(objDataStr);
                    if(objDataStr.size()<1) continue;
                    if(objDataStr.at(0)=='#') continue;

                    resSL =  objDataStr.split(" ", QString::SkipEmptyParts);
                    if(resSL.size()<8) continue;
                    isObj = 1;
                    X0[0] = resSL.at(1).toDouble();
                    X0[1] = resSL.at(2).toDouble();
                    X0[2] = resSL.at(3).toDouble();
    /*
                    V0[0] = resSL.at(5).toDouble();
                    V0[1] = resSL.at(6).toDouble();
                    V0[2] = resSL.at(7).toDouble();
    */
                    V0[0] = resSL.at(8).toDouble();
                    V0[1] = resSL.at(9).toDouble();
                    V0[2] = resSL.at(10).toDouble();

                    X0[0] += XE0[0];
                    X0[1] += XE0[1];
                    X0[2] += XE0[2];
                    V0[0] += VE0[0];
                    V0[1] += VE0[1];
                    V0[2] += VE0[2];
                    //


                    qDebug() << QString("X0: %1\t%2\t%3\nV0: %4\t%5\t%6\n").arg(X0[0]).arg(X0[1]).arg(X0[2]).arg(V0[0]).arg(V0[1]).arg(V0[2]);

                    if(initMpc) break;
                    if(mCat.GetRecName(name.simplified().toAscii().data())) break;
    /*
                    dT = (time-2451545.0)/36525.0;
                    m = mas_to_grad((4612.4362*dT)*1000);
                    n = mas_to_grad((2004.3109*dT)*1000);
    */
                    //detRDnumGC(&ra, &de, Q[0], Q[1], Q[2], XE0[0], XE0[1], XE0[2], 0, 0, 0);
                    //detRDnumGC(&ra, &de, P[0], P[1], P[2], 0, 0, 0, 0, 0, 0);
                    //rdsys(&ra, &de, P[0], P[1], P[2]);

                    //dDec = (1.0/CAU)*(VE0[2]/cos(de) - VE0[0]*sin(de)*cos(ra) - VE0[1]*sin(de)*sin(ra) - VE0[2]*(pow(sin(de), 2.0)/cos(de)));
                    //dRa = (1.0/CAU)*(-VE0[0]*sin(ra) + VE0[1]*cos(ra))/cos(de);

                    state[0] = X[0];
                    state[1] = X[1];
                    state[2] = X[2];
                    state[3] = V[0];
                    state[4] = V[1];
                    state[5] = V[2];
                    obsPos.det_vect_radec(state, &ra, &de, &range);

                    mrec.r = ra;// + dRa;
                    mrec.d = de;// + dDec;

                    mrec.eJD = jdTDB;
                    //mrec.eJD = time;
                    mrec.num = 1;
                    mCat.record->getNumStr(mrec.head->Snum);
                    //strcpy(, mCat.record->getNumStr(>number);
                    mrec.tail->set_numOfObs(obsCode.toAscii().data());
                    mrec.toString(astr);

                    mpcStm << astr << "\n";

                    break;
                }
            }
        }

        dX[0] = X[0] - X0[0];
        dX[1] = X[1] - X0[1];
        dX[2] = X[2] - X0[2];

        dist = sqrt(dX[0]*dX[0] + dX[1]*dX[1] + dX[2]*dX[2]);

        dV[0] = V[0] - V0[0];
        dV[1] = V[1] - V0[1];
        dV[2] = V[2] - V0[2];

        dvel = sqrt(dV[0]*dV[0] + dV[1]*dV[1] + dV[2]*dV[2]);

        dxStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10\n").arg(name, -10).arg(time, 13, 'f', 4).arg(dX[0], 18, 'g', 9).arg(dX[1], 18, 'g', 9).arg(dX[2], 18, 'g', 9).arg(dist, 18, 'g', 9).arg(dV[0], 18, 'g', 9).arg(dV[1], 18, 'g', 9).arg(dV[2], 18, 'g', 9).arg(dvel, 18, 'g', 9);
        qDebug() << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10\n").arg(name, -10).arg(time, 13, 'f', 4).arg(dX[0], 18, 'g', 9).arg(dX[1], 18, 'g', 9).arg(dX[2], 18, 'g', 9).arg(dist, 18, 'g', 9).arg(dV[0], 18, 'g', 9).arg(dV[1], 18, 'g', 9).arg(dV[2], 18, 'g', 9).arg(dvel, 18, 'g', 9);

    }


    inFile.close();
    bigFile.close();
    dxFile.close();
    mpcFile.close();
    spkFile.close();

    return 0;//a.exec();
}
