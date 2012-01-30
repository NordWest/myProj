#include <QtCore>

#include "./../libs/mpcfile.h"
#include "./../astro/astro.h"

int main(int argc, char *argv[])//mpcs.ini.txt mpcs.cat.txt mpcs.res.txt
{

    QCoreApplication a(argc, argv);

    setlocale(LC_NUMERIC, "C");

    int sMod = 0;
    mpcFile mpc0, mpc1, mpc0res, mpc1res;

    if(argc<3) sMod - 1;

    mpc0.init(argv[1]);
    if(argc>=3) mpc1.init(argv[2]);
    mpc0res.init(QString("%1_res.txt").arg(QFileInfo(argv[1]).completeBaseName()).toAscii().data());
    if(argc>=3) mpc1res.init(QString("%1_res.txt").arg(QFileInfo(argv[2]).completeBaseName()).toAscii().data());

    int i, j, sz0, sz1;
    int obsNum, posMin;
    double mjd0, mjd1, dT, dTmin;
    QString objNum0, objNum1;

    QFile logFile("log.txt");
    logFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream logStm(&logFile);

    QSettings *sett = new QSettings("./findSimMpcs.ini", QSettings::IniFormat);

    bool ook;
    QString obsName = sett->value("general/obsName", "-1").toString();
    QString objNum = sett->value("general/objNum", "-1").toString();
    QString obsCode0, obsCode1;
    /*obsNum = obsName.toInt(&ook);
    if((obsNum!=-1)||(!ook))
    {
        unpackString(&obsNumName, obsName);
        obsNum = obsNumName.toInt();
        qDebug() << QString("obsName= %1\tobsNumName= %2\tobsNum= %3\n").arg(obsName).arg(obsNumName).arg(obsNum);
    }*/
    //if(namenum) obsNum = obsName
    //int useDtime = sett->value("general/useDtime", 0).toInt();
    double dtime = sett->value("general/dtime", 0.0).toDouble();
    dtime /= 1440.0;


    sz0 = mpc0.size();
    sz1 = mpc1.size();

    logStm << QString("sz0: %1\tsz1: %2\n").arg(sz0).arg(sz1);


    //DATEOBS dobs;

    mpcRec *rec0, *rec1, *recr;

/*
    for(j=0;j<sz1;j++)
    {
        mpc1.getmpc(j);
        rec1 = mpc1.record;

        //if((fabs(rec0->eJD-rec1->eJD)<dtime)&&(rec0->num==rec1->num)&&(rec0->numOfObservatory==rec1->numOfObservatory))
        if((rec1->num==2)&&(rec1->numOfObservatory==84))
        {
            recr = new mpc;
            recr->set_mpc(rec1);
            mpcres.addmpc(recr);
            getDATEOBSfromMJD(&dobs, jd2mjd(recr->eJD));
        }
    }

*/
    for(i=0; i<sz0; i++)
    {
        //if(mpc0.(i)) qDebug() << "\nERROR reading mpc0\n";
        rec0 = mpc0.at(i);

        //qDebug() << QString("i: %1").arg(i);
        //qDebug() << QString("eJD|%1\n").arg(rec0->eJD);
        //qDebug() << QString("num|%1\n").arg(rec0->num);
        rec0->getObsCode(obsCode0);
        rec0->getMpNumber(objNum0);
        mjd0 = rec0->mjd();
        //qDebug() << QString("%1 | %2 | %3\n").arg(obsCode0).arg(objNum0).arg(mjd0, 13, 'f', 5);
        if(((QString().compare(obsCode0, obsName)==0)||(QString().compare("-1", obsName)==0))&&((QString().compare(objNum0, objNum)==0)||(QString().compare("-1", objNum)==0)))
        {
            if(sz1==0)
            {
                //qDebug() << QString("addRec\n");
                mpc0res.addRec(*rec0);
            }

        //rec0->eJD
            posMin = -1;
            dTmin = dtime +100;
            for(j=0;j<sz1;j++)
            {
                //mpc1.getmpc(j);
                rec1 = mpc1.at(j);//.record;

                //qDebug() << QString("eJD|%1:\t%2\t%3\n").arg(rec0->eJD).arg(rec1->eJD).arg(fabs(rec0->eJD-rec1->eJD));
                //qDebug() << QString("num|%1:\t%2\n").arg(rec0->num).arg(rec1->num);

                rec1->getObsCode(obsCode1);
                rec1->getMpNumber(objNum1);
                mjd1 = rec1->mjd();

                dT = fabs(mjd0-mjd1);
                if((QString().compare(objNum1, objNum0)==0)&&(QString().compare(obsCode1, obsCode0)==0))
                //if(fabs(rec0->eJD-rec1->eJD)<dtime)
                {
                    if(dT<dTmin)
                    {
                        posMin=j;
                        dTmin = dT;
                    }

                    /*recr = new mpc;
                    recr->set_mpc(rec0);
                    mpc0res.addmpc(recr);
                    recr = new mpc;
                    recr->set_mpc(rec1);
                    mpc1res.addmpc(recr);
                    qDebug() << QString("dr: %1\n").arg(fabs(rec0->eJD-rec1->eJD));*/
                    //break;
                    //getDATEOBSfromMJD(&dobs, jd2mjd(recr->eJD));
                }
            }
            if((posMin!=-1)&&(dTmin<dtime))
            {
                logStm << QString("%1 | %2 | %3\n").arg(objNum0).arg(rec0->mjd(), 13, 'f', 5).arg(dTmin*1440);
                rec1 = mpc1.at(posMin);
                mpc0res.addRec(*rec0);
                mpc1res.addRec(*rec1);
            }
        }
    }

    mpc0res.save();
    mpc1res.save();

//
    logFile.close();

    return 0;//a.exec();
}
