#include <QtCore>

#include "./../libs/mpcfile.h"
#include "./../astro/astro.h"

int main(int argc, char *argv[])//mpcs.ini.txt mpcs.cat.txt mpcs.res.txt
{

    QCoreApplication a(argc, argv);
    //if(argc<3) exit(1);

    setlocale(LC_NUMERIC, "C");

    int sMod = 0;
    mpcFile mpc0, mpc1, mpc0res, mpc1res;
    QString mpcFileName0, mpcFileName1, mpcFileName0_res, mpcFileName1_res, mpcFileName0_uniq, mpcFileName1_uniq;
    QFile mpcFile0, mpcFile1, mpcFile0_res, mpcFile1_res, mpcFile0_uniq, mpcFile1_uniq;
    QTextStream mpcStm0, mpcStm1, mpcStm0_res, mpcStm1_res, mpcStm0_uniq, mpcStm1_uniq;

    if(argc<3) sMod = 1;

    mpcFileName0 = QString(argv[1]);

    mpcFileName0_res = QString("%1_res.txt").arg(QFileInfo(argv[1]).completeBaseName()).toAscii().data();
    //mpc0res.init(QString("%1_res.txt").arg(QFileInfo(argv[1]).completeBaseName()).toAscii().data());
    mpcFileName0_uniq = QString("%1_uniq.txt").arg(QFileInfo(argv[1]).completeBaseName()).toAscii().data();
    if(argc==3)
    {
        mpcFileName1 = QString(argv[2]);
        mpcFileName1_res = QString("%1_res.txt").arg(QFileInfo(argv[2]).completeBaseName()).toAscii().data();
        mpcFileName1_uniq = QString("%1_uniq.txt").arg(QFileInfo(argv[2]).completeBaseName()).toAscii().data();
    }


    int i, j, sz0, sz1;
    int obsNum, posMin;
    double mjd0, mjd1, dT, dTmin;
    QString objNum0, objNum1, tStr;

    QFile logFile("log.txt");
    logFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream logStm(&logFile);

    QSettings *sett = new QSettings("./findSimMpcs.ini", QSettings::IniFormat);

    bool ook;
    QString obsCode = sett->value("general/obsCode", "-1").toString();
    QString objNum = sett->value("general/objNum", "-1").toString();
    QString obsCode0, obsCode1;

    bool isOk;
    QString upStr;


    unpackString(&upStr, obsCode);
    obsNum = upStr.toInt(&isOk);

    double dtime = sett->value("general/dtime", 0.0).toDouble();
    dtime /= 1440.0;



logStm << QString("Begin\n");
qDebug() << QString("Begin\n");

//    sz0 = mpc0.size();
//    sz1 = mpc1.size();
    mpcFile0.setFileName(mpcFileName0);
    if(!mpcFile0.open(QFile::ReadOnly)) return 1;
    mpcStm0.setDevice(&mpcFile0);
    tStr = mpcStm0.readLine();
    sz0 = mpcFile0.size()/tStr.size();
    mpcStm0.seek(0);

    mpcFile0_res.setFileName(mpcFileName0_res);
    if(!mpcFile0_res.open(QFile::WriteOnly | QFile::Truncate)) return 1;
    mpcStm0_res.setDevice(&mpcFile0_res);

    mpcFile0_uniq.setFileName(mpcFileName0_uniq);
    if(!mpcFile0_uniq.open(QFile::WriteOnly | QFile::Truncate)) return 1;
    mpcStm0_uniq.setDevice(&mpcFile0_uniq);

    if(argc==3)
    {
        mpcFile1.setFileName(mpcFileName1);
        if(mpcFile1.open(QFile::ReadOnly))
        {
            mpcStm1.setDevice(&mpcFile1);
            tStr = mpcStm1.readLine();
            sz1 = mpcFile1.size()/tStr.size();
            mpcStm1.seek(0);

            mpcFile1_res.setFileName(mpcFileName1_res);
            if(!mpcFile1_res.open(QFile::WriteOnly | QFile::Truncate)) return 1;
            mpcStm1_res.setDevice(&mpcFile1_res);
/*
            mpcFile1_uniq.setFileName(mpcFileName1_uniq);
            if(!mpcFile1_uniq.open(QFile::WriteOnly | QFile::Truncate)) return 1;
            mpcStm1_uniq.setDevice(&mpcFile1_uniq);
            */
        }
        else sz1=0;
    }

    logStm << QString("sz0: %1\tsz1: %2\n").arg(sz0).arg(sz1);
    qDebug() << QString("sz0: %1\tsz1: %2\n").arg(sz0).arg(sz1);


    //DATEOBS dobs;

    mpcRec rec0, rec1, rec1min;

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
    int uk;
    while(!mpcStm0.atEnd())
    {
        //tStr0 = mpcStm0.readLine();
        rec0.fromStr(mpcStm0.readLine());

        rec0.getObsCode(obsCode0);
        rec0.getMpNumber(objNum0);
        mjd0 = rec0.mjd();

        uk=1;

        if(((QString().compare(obsCode0, obsCode)==0)||(QString().compare("-1", obsCode)==0))&&((QString().compare(objNum0, objNum)==0)||(QString().compare("-1", objNum)==0)))
        {



            posMin = -1;
            dTmin = dtime +100;

            mpcStm1.seek(0);

            while(!mpcStm1.atEnd())
            {
                rec1.fromStr(mpcStm1.readLine());

                rec1.getObsCode(obsCode1);
                rec1.getMpNumber(objNum1);
                mjd1 = rec1.mjd();
                dT = fabs(mjd0-mjd1);

                if(((QString().compare(obsCode1, obsCode)==0)||(QString().compare("-1", obsCode)==0))&&((QString().compare(objNum0, objNum)==0)||(QString().compare("-1", objNum)==0)))
                {
                    if(dT<dTmin)
                    {
                        posMin=j;
                        dTmin = dT;
                        rec1min.fromRec(rec1);
                    }
                }
            }
            if(((posMin!=-1)&&(dTmin<dtime))||(sz1==0))
            {
                logStm << QString("%1 | %2 | %3\n").arg(objNum0).arg(rec0.mjd(), 13, 'f', 5).arg(dTmin*1440);
                //rec1 = mpc1.at(posMin);
                mpcStm0_res << rec0.toStr() << "\n";
                if(sz1!=0) mpcStm1_res << rec1min.toStr() << "\n";

                uk=0;
            }
        }
        if(uk)
        {
            mpcStm0_uniq << rec0.toStr() << "\n";
        }
    }
   /*
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
        if(((QString().compare(obsCode0, obsCode)==0)||(QString().compare("-1", obsCode)==0))&&((QString().compare(objNum0, objNum)==0)||(QString().compare("-1", objNum)==0)))
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
                    qDebug() << QString("dr: %1\n").arg(fabs(rec0->eJD-rec1->eJD));/
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
*/
//
    mpcFile0.close();
    mpcFile1.close();
    mpcFile0_res.close();
    mpcFile1_res.close();

    logFile.close();

    return 0;//a.exec();
}
