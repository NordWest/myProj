#include <QtCore>

#include "./../libs/dele.h"
#include "./../libs/astro.h"
#include "./../libs/comfunc.h"
#include "./../libs/calc_epm.h"
#include "./../libs/mpccat.h"
#include "./../libs/mpcs.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    QString tStr, name, sJD;
    QStringList dataSL, resSL, outerArguments;
    double time, jdUTC;
    double X[3], V[3];
    double dX[3], dV[3];
    double X0[3], V0[3];
    double XE0[3], VE0[3];
    double dist, dvel;
    double ra, de;
    int isObj, i;
    int SK, CENTER;
    QProcess outerProcess;
    QString objDataStr;
    mpc mrec;
    double dT, m, n;
    double dRa, dDec, normR;
    double *s, *sA, *R, *sV;
    myvector v0(3), v1(3), v2(3);
    /*v0 = new myvector(3);
    v1 = new myvector(3);
    v2 = new myvector(3);*/
    s = new double[3];
    sA = new double[3];
    sV = new double[3];
    R = new double[3];

    procData miriadeProcData;

    QSettings *sett = new QSettings("./pnb.ini", QSettings::IniFormat);

    QString jplFile = sett->value("general/jplFile", "./../../data/cats/binp1940_2020.405").toString();
    QString epmDir = sett->value("general/epmDir", "./").toString();
    QString obsFile = sett->value("general/obsFile", "./../../data/cats/Obs.txt").toString();
    QString obsCode = sett->value("general/obsCode", "500").toString();
    QString mpcCatFile = sett->value("general/mpcCatFile", "mocorb.txt").toString();
    int useEPM = sett->value("general/useEPM", 0).toInt();
    QString colSep = sett->value("general/colSep", "|").toString();

    SK = sett->value("general/sk", 0).toInt();
    CENTER = sett->value("general/center", 0).toInt();

    miriadeProcData.name = sett->value("miriadeProcData/name", "./mpeph.exe").toString();
    miriadeProcData.folder = sett->value("miriadeProcData/folder", "./").toString();
    miriadeProcData.waitTime = sett->value("miriadeProcData/waitTime", -1).toInt();
    if(miriadeProcData.waitTime>0) miriadeProcData.waitTime *= 1000;

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

    mpccat mCat;
    int initMpc = mCat.init(mpcCatFile.toAscii().data());

    QString inFileName(argv[1]);
    QFileInfo fi(inFileName);
    QString dxFileName = QString("%1_dr.txt").arg(fi.absoluteFilePath());
    QString mpcFileName = QString("%1_mpc.txt").arg(fi.absoluteFilePath());

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

    char *astr = new char[256];
    QFile mpcFile(mpcFileName);
    mpcFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream mpcStm(&mpcFile);

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

        if(useEPM) plaNum = epm_planet_num(name);
        else plaNum = planet_num(name.toAscii().data());
        if(plaNum!=-1)
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
        else
        {

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
                qDebug() << QString("XE0: %1\t%2\t%3\nVE0: %4\t%5\t%6\n").arg(XE0[0]).arg(XE0[1]).arg(XE0[2]).arg(VE0[0]).arg(VE0[1]).arg(VE0[2]);
                break;
            }
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
/*
            if(useEPM)
            {
                status = calc_EPM(GEOCENTR_NUM, centr_num, (int)time, time-(int)time, XE0, VE0);
                 if(!status)
                 {
                     qDebug() << QString("error EPM\n");
                     return 1;
                 }
            }
            else nbody->detState(&XE0[0], &XE0[1], &XE0[2], &VE0[0], &VE0[1], &VE0[2], time, GEOCENTR_NUM, CENTER, SK);
*/

            qDebug() << QString("XE0: %1\t%2\t%3\nVE0: %4\t%5\t%6\n").arg(XE0[0]).arg(XE0[1]).arg(XE0[2]).arg(VE0[0]).arg(VE0[1]).arg(VE0[2]);


            R[0] = X[0] - XE0[0];
            R[0] = X[1] - XE0[1];
            R[0] = X[2] - XE0[2];
            normR = norm(R);
            s[0] = R[0]/normR;
            s[1] = R[1]/normR;
            s[2] = R[2]/normR;

            sV[0] = V[0] - VE0[0];
            sV[1] = V[1] - VE0[1];
            sV[2] = V[2] - VE0[2];



            v0.set(0, sV[0]);
            v0.set(1, sV[1]);
            v0.set(2, sV[2]);

            v1.set(0, s[0]);
            v1.set(1, s[1]);
            v1.set(2, s[2]);

            v2 = Vmul(&v1, &Vmul(&v1, &v0));
            //v2 = v2*(1.0/CAU);
            qDebug() << QString("v2: %1 %2 %3").arg(v2.get(0)).arg(v2.get(1)).arg(v2.get(2));

            sA[0] = v2.get(0)*(1.0/CAU) + s[0];
            sA[1] = v2.get(1)*(1.0/CAU) + s[1];
            sA[2] = v2.get(2)*(1.0/CAU) + s[2];



            outerArguments.clear();

            outerArguments << QString("-name=%1").arg(name.simplified().toLower());

            outerArguments << QString("-ep=%1").arg(sJD);
            //outerArguments << QString("-ep=%1").arg(time, 15, 'f',7);

    /*        if(useEPM) plaNum = epm_planet_num(name);
            else plaNum = planet_num(name.toAscii().data());
            if(plaNum!=-1) outerArguments << "-type=planet";
            else */
            outerArguments << "-type=aster";


            outerArguments << QString("-observer=500");
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
                //detRDnumGC(&ra, &de, X[0], X[1], X[2], XE0[0], XE0[1], XE0[2], 0, 0, 0);
                rdsys(&ra, &de, sA[0], sA[1], sA[2]);

                dDec = (1.0/CAU)*(VE0[2]/cos(de) - VE0[0]*sin(de)*cos(ra) - VE0[1]*sin(de)*sin(ra) - VE0[2]*(pow(sin(de), 2.0)/cos(de)));
                dRa = (1.0/CAU)*(-VE0[0]*sin(ra) + VE0[1]*cos(ra))/cos(de);

                mrec.r = ra;// + dRa;
                mrec.d = de;// + dDec;

                mrec.eJD = jdUTC;
                mrec.num = 1;
                mCat.record->getNumStr(mrec.head->Snum);
                //strcpy(, mCat.record->getNumStr(>number);
                mrec.tail->set_numOfObs("500");
                mrec.toString(astr);

                mpcStm << astr << "\n";

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

    return 0;//a.exec();
}
