#include <QtCore>

#include "./../libs/dele.h"
#include "./../libs/astro.h"
#include "./../libs/comfunc.h"
#include "./../libs/calc_epm.h"

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
    int isObj;
    int SK, CENTER;
    QProcess outerProcess;
    QString objDataStr;

    procData miriadeProcData;

    QSettings *sett = new QSettings("./pnb.ini", QSettings::IniFormat);

    QString jplFile = sett->value("general/jplFile", "./../../data/cats/binp1940_2020.405").toString();
    QString epmDir = sett->value("general/epmDir", "./").toString();
    QString obsFile = sett->value("general/obsFile", "./../../data/cats/Obs.txt").toString();
    QString obsCode = sett->value("general/obsCode", "500").toString();
    int useEPM = sett->value("general/useEPM", 0).toInt();

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

    QString inFileName(argv[1]);
    QFileInfo fi(inFileName);
    QString dxFileName = QString("%1_dr.txt").arg(fi.absoluteFilePath());

    //QString inFileName("small.log");
    QFile inFile(inFileName);
    inFile.open(QFile::ReadOnly);
    QTextStream inStm(&inFile);

    //QString dxFileName("small_dr.txt");
    QFile dxFile(dxFileName);
    dxFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream dxStm(&dxFile);

    while(!inStm.atEnd())
    {
        tStr = inStm.readLine();
        dataSL = tStr.split("|");
        if(dataSL.size()<10) continue;

        time = dataSL.at(0).toDouble();


        X[0] = dataSL.at(1).toDouble();
        X[1] = dataSL.at(2).toDouble();
        X[2] = dataSL.at(3).toDouble();
        V[0] = dataSL.at(5).toDouble();
        V[1] = dataSL.at(6).toDouble();
        V[2] = dataSL.at(7).toDouble();

        name = dataSL.at(8).simplified();

        qDebug() << QString("X: %1\t%2\t%3\nV: %4\t%5\t%6\n").arg(X[0]).arg(X[1]).arg(X[2]).arg(V[0]).arg(V[1]).arg(V[2]);

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


 /*       outerArguments.clear();


        outerArguments << QString("-name=earth");
        outerArguments << QString("-type=planet");
        outerArguments << QString("-observer=@sun");
        outerArguments << QString("-ep=%1").arg(time, 15, 'f',7);

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
            isObj = 1;
            XE0[0] = resSL.at(1).toDouble();
            XE0[1] = resSL.at(2).toDouble();
            XE0[2] = resSL.at(3).toDouble();
            VE0[0] = resSL.at(5).toDouble();
            VE0[1] = resSL.at(6).toDouble();
            VE0[2] = resSL.at(7).toDouble();
            break;
        }


        qDebug() << QString("XE0: %1\t%2\t%3\nV0: %4\t%5\t%6\n").arg(XE0[0]).arg(XE0[1]).arg(XE0[2]).arg(VE0[0]).arg(VE0[1]).arg(VE0[2]);
*/
        }
        else
        {

            outerArguments.clear();

            TDB2UTC(time, &jdUTC);
            outerArguments << QString("-name=\"%1\"").arg(name.simplified().toLower());
            sJD = QString("%1").arg(jdUTC, 15, 'f',7);
            outerArguments << QString("-ep=%1").arg(sJD);

    /*        if(useEPM) plaNum = epm_planet_num(name);
            else plaNum = planet_num(name.toAscii().data());
            if(plaNum!=-1) outerArguments << "-type=planet";
            else */
            outerArguments << "-type=aster";


            outerArguments << QString("-observer=@sun");
            //outerArguments << "-tcoor=2";
            //outerArguments << "-rplane=1";

            //qDebug() << outerArguments.join(" ") << "\n";

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
                X0[0] = resSL.at(1).toDouble();
                X0[1] = resSL.at(2).toDouble();
                X0[2] = resSL.at(3).toDouble();
                V0[0] = resSL.at(5).toDouble();
                V0[1] = resSL.at(6).toDouble();
                V0[2] = resSL.at(7).toDouble();
                qDebug() << QString("X0: %1\t%2\t%3\nV0: %4\t%5\t%6\n").arg(X0[0]).arg(X0[1]).arg(X0[2]).arg(V0[0]).arg(V0[1]).arg(V0[2]);




    /*
                X0[0] += XE0[0];
                X0[1] += XE0[1];
                X0[2] += XE0[2];
                V0[0] += VE0[0];
                V0[1] += VE0[1];
                V0[2] += VE0[2];*/
                break;
    /*
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
    */
                //smlStm << QString(" %1\n").arg(name).arg(1.0/pList[i]->mass);
                //smlStm << QString("%1 %2 %3\n%4 %5 %6\n0.0 0.0 0.0\n").arg(X[0], 26, 'e', 20).arg(X[1], 26, 'e', 20).arg(X[2], 26, 'e', 20).arg(V[0], 26, 'e', 20).arg(V[1], 26, 'e', 20).arg(V[2], 26, 'e', 20);


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

        dxStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10\n").arg(time).arg(name).arg(dX[0]).arg(dX[1]).arg(dX[2]).arg(dist).arg(dV[0]).arg(dV[1]).arg(dV[2]).arg(dvel);
        qDebug() << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10\n").arg(time).arg(name).arg(dX[0]).arg(dX[1]).arg(dX[2]).arg(dist).arg(dV[0]).arg(dV[1]).arg(dV[2]).arg(dvel);

    }


    inFile.close();
    dxFile.close();


    //nbody->detState(&X0[0], &X0[1], &X0[2], &V0[0], &V0[1], &V0[2], time0, GEOCENTR_NUM, CENTER, SK);
    
    return 0;//a.exec();
}
