#include <QtCore>

#include "./../libs/mpcfile.h"
#include "./../astro/astro.h"

int main(int argc, char *argv[])//mpcs.ini.txt mpcs.cat.txt mpcs.res.txt
{

    QCoreApplication a(argc, argv);
    //if(argc<3) exit(1);

    setlocale(LC_NUMERIC, "C");

    int sMod = 0;
    mpcFile mpc0, mpc1;//, mpc0res, mpc1res;
    QString mpcFileName0, mpcFileName1, mpcFileName0_res, mpcFileName1_res, mpcFileName0_uniq, mpcFileName1_uniq;
    QFile mpcFile0, mpcFile1, mpcFile0_res, mpcFile1_res, mpcFile0_uniq, mpcFile1_uniq;
    QTextStream mpcStm0, mpcStm1, mpcStm0_res, mpcStm1_res, mpcStm0_uniq, mpcStm1_uniq;

    if(argc<3) sMod = 1;

    mpcFileName0 = QString(argv[1]);

    mpcFileName0_res = QString("%1/%2_res.txt").arg(QFileInfo(argv[1]).absolutePath()).arg(QFileInfo(argv[1]).baseName()).toAscii().data();
    //mpc0res.init(QString("%1_res.txt").arg(QFileInfo(argv[1]).completeBaseName()).toAscii().data());
    mpcFileName0_uniq = QString("%1/%2_uniq.txt").arg(QFileInfo(argv[1]).absolutePath()).arg(QFileInfo(argv[1]).baseName()).toAscii().data();
    if(argc==3)
    {
        mpcFileName1 = QString(argv[2]);
        mpcFileName1_res = QString("%1/%2_res.txt").arg(QFileInfo(argv[2]).absolutePath()).arg(QFileInfo(argv[2]).baseName()).toAscii().data();
        mpcFileName1_uniq = QString("%1/%2_uniq.txt").arg(QFileInfo(argv[2]).absolutePath()).arg(QFileInfo(argv[2]).baseName()).toAscii().data();
    }


    int i, j, sz0, sz1;
    int posMin;
    int uk;
    int obsNum0, obsNum1, obsNum;
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

    if(mpc0.init(mpcFileName0))return 1;
/*mpcFile0.setFileName(mpcFileName0);
    if(!mpcFile0.open(QFile::ReadOnly)) return 1;
    mpcStm0.setDevice(&mpcFile0);*/
    //tStr = mpcStm0.readLine();
    //sz0 = mpcFile0.size()/tStr.size();
    //mpcStm0.seek(0);

    mpcFile0_res.setFileName(mpcFileName0_res);
    if(!mpcFile0_res.open(QFile::WriteOnly | QFile::Truncate)) return 1;
    mpcStm0_res.setDevice(&mpcFile0_res);

    mpcFile0_uniq.setFileName(mpcFileName0_uniq);
    if(!mpcFile0_uniq.open(QFile::WriteOnly | QFile::Truncate)) return 1;
    mpcStm0_uniq.setDevice(&mpcFile0_uniq);

    if(argc==3)
    {
        //mpcFile1.setFileName(mpcFileName1);
        if(!mpc1.init(mpcFileName1))//mpcFile1.open(QFile::ReadOnly))
        {
            //mpcStm1.setDevice(&mpcFile1);
            //tStr = mpcStm1.readLine();
            //sz1 = mpcFile1.size()/tStr.size();
            //mpcStm1.seek(0);

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

    sz0 = mpc0.size();
    sz1 = mpc1.size();

    logStm << QString("sz0: %1\tsz1: %2\n").arg(sz0).arg(sz1);
    qDebug() << QString("sz0: %1\tsz1: %2\n").arg(sz0).arg(sz1);


    //DATEOBS dobs;

    mpcRec *rec0, *rec1;

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
    QString tStr0;
    int iNum=0;
	int rNum, uqNum;
	rNum = uqNum = 0;
/*
    while(!mpcStm0.atEnd())
    {
        tStr0 = mpcStm0.readLine();
        //qDebug() << QString("tStr0: %1\n").arg(tStr0);
        rec0.fromStr(tStr0);

        rec0.getObsCode(obsCode0);
        mjd0 = rec0.mjd();
        obsNum0 = rec0.getObsNum();
        rec0.getMpNumber(objNum0);

//        iNum++;
//        qDebug() << QString("%1/%2\t%3\t%4\t%5\n").arg(iNum).arg(sz0).arg(uqNum).arg(rNum0).arg(rNum1);

        uk=1;

        //if(((QString().compare(obsCode0, obsCode)==0)||(QString().compare("-1", obsCode)==0))&&((QString().compare(objNum0, objNum)==0)||(QString().compare("-1", objNum)==0)))
        if(((obsNum0==obsNum)||(obsNum==-1))&&((QString().compare(objNum0, objNum)==0)||(QString().compare("-1", objNum)==0)))
        {



            posMin = -1;
            dTmin = dtime +100;

            mpcStm1.seek(0);

            while(!mpcStm1.atEnd())
            {
                rec1.fromStr(mpcStm1.readLine());

                rec1.getObsCode(obsCode1);
                obsNum1 = rec1.getObsNum();
                rec1.getMpNumber(objNum1);
                mjd1 = rec1.mjd();
                dT = fabs(mjd0-mjd1);

                //qDebug() << QString("%1:%2\n").arg(obsNum0).arg(obsNum1);

                //if(((QString().compare(obsCode1, obsCode)==0)||(QString().compare("-1", obsCode)==0))&&((QString().compare(objNum0, objNum)==0)||(QString().compare("-1", objNum)==0)))
                if((obsNum1==obsNum0)&&(QString().compare(objNum1, objNum0)==0))
                {
                    //qDebug() << QString("dT: %1\n").arg(dT);
                    if(dT<dTmin)
                    {
                        //qDebug() << QString("dT: %1\n").arg(dT);
                        //posMin=1;
                        dTmin = dT;
                        rec1min.fromRec(rec1);
                    }
                }
            }
            if((dTmin<dtime)||(sz1==0))
            {
                //qDebug() << QString("dTmin: %1\n").arg(dTmin);
                logStm << QString("%1 | %2 | %3\n").arg(objNum0).arg(rec0.mjd(), 13, 'f', 5).arg(dTmin*1440);
                //rec1 = mpc1.at(posMin);
                mpcStm0_res << rec0.toStr() << "\n";
                if(sz1!=0) mpcStm1_res << rec1min.toStr() << "\n";

		rNum++;
                uk=0;
            }
        }
        if(uk)
        {
            mpcStm0_uniq << rec0.toStr() << "\n";
		uqNum++;
        }

	iNum++;
        qDebug() << QString("%1:\t%2/%3                              \n").arg(iNum).arg(uqNum).arg(rNum);


    }
    */


    for(i=0; i<sz0; i++)
    {
        uk=1;
        //if(mpc0.(i)) qDebug() << "\nERROR reading mpc0\n";

        rec0 = mpc0.at(i);
        //rec0.fromRec(*mpc0.recList.at(i));

        //qDebug() << QString("i: %1").arg(i);
        //qDebug() << QString("eJD|%1\n").arg(rec0->eJD);
        //qDebug() << QString("num|%1\n").arg(rec0->num);
        rec0->getObsCode(obsCode0);
        mjd0 = rec0->mjd();
        obsNum0 = rec0->getObsNum();
        rec0->getMpNumber(objNum0);

        //qDebug() << QString("%1 | %2 | %3\n").arg(obsCode0).arg(objNum0).arg(mjd0, 13, 'f', 5);
        //if(((QString().compare(obsCode0, obsCode)==0)||(QString().compare("-1", obsCode)==0))&&((QString().compare(objNum0, objNum)==0)||(QString().compare("-1", objNum)==0)))
        if(((obsNum0==obsNum)||(obsNum==-1))&&((QString().compare(objNum0, objNum)==0)||(QString().compare("-1", objNum)==0)))
        {


        //rec0->eJD
            posMin = -1;
            dTmin = dtime +100;
            sz1 = mpc1.size();
            //logStm << QString("%1/%2:\t%3/%4\t %5                              \n").arg(i).arg(sz0).arg(uqNum).arg(rNum).arg(sz1);
            for(j=0;j<sz1;j++)
            {

                //mpc1.getmpc(j);
                rec1 = mpc1.at(j);//.record;
                //rec1.fromRec(*mpc1.recList.at(j));

                //qDebug() << QString("eJD|%1:\t%2\t%3\n").arg(rec0->eJD).arg(rec1->eJD).arg(fabs(rec0->eJD-rec1->eJD));
                //qDebug() << QString("num|%1:\t%2\n").arg(rec0->num).arg(rec1->num);

                rec1->getObsCode(obsCode1);
                obsNum1 = rec1->getObsNum();
                rec1->getMpNumber(objNum1);
                mjd1 = rec1->mjd();

                dT = fabs(mjd0-mjd1);
                //if((QString().compare(objNum1, objNum0)==0)&&(QString().compare(obsCode1, obsCode0)==0))
                if((obsNum1==obsNum0)&&(QString().compare(objNum1, objNum0)==0)&&(dT<dTmin))
                {
                    posMin=j;
                    dTmin = dT;


                    /*recr = new mpc;
                    recr->set_mpc(rec0);
                    mpc0res.addmpc(recr);
                    recr = new mpc;
                    recr->set_mpc(rec1);
                    mpc1res.addmpc(recr);
                    qDebug() << QString("dr: %1\n").arg(fabs(rec0->eJD-rec1->eJD));/
                    //break;
                    //getDATEOBSfromMJD(&dobs, jd2mjd(recr->eJD));*/
                }


            }
            /*if((posMin!=-1)&&(dTmin<dtime))
            {
                logStm << QString("%1 | %2 | %3\n").arg(objNum0).arg(rec0->mjd(), 13, 'f', 5).arg(dTmin*1440);
                rec1 = mpc1.at(posMin);
                mpc0res.addRec(*rec0);
                mpc1res.addRec(*rec1);
            }*/


            if(((posMin!=-1)&&(dTmin<dtime))||(sz1==0))
            {
                //qDebug() << QString("dTmin: %1\n").arg(dTmin);
                //logStm << QString("%1 | %2 | %3\n").arg(objNum0).arg(rec0->mjd(), 13, 'f', 5).arg(dTmin*1440);

                mpcStm0_res << rec0->toStr() << "\n";

                if(sz1!=0)
                {
                    rec1 = mpc1.at(posMin);
                    //rec1.fromRec(*mpc1.recList.at(posMin));


                    mpcStm1_res << rec1->toStr() << "\n";


                    //qDebug() << QString("removeAt(%1)\n").arg(posMin);
                    mpc1.removeAt(posMin);
                    uk=0;
                }

                rNum++;

            }
            if(uk)
            {

                mpcStm0_uniq << rec0->toStr() << "\n";


            uqNum++;
            }


        }

        //iNum++;
        qDebug() << QString("%1/%2:\t%3/%4\t %5                              \n").arg(i).arg(sz0).arg(uqNum).arg(rNum).arg(sz1);


    }
/*
    mpc0res.save();
    mpc1res.save();
*/
    mpc1.saveAs(mpcFileName1_uniq);
//
//    mpcFile0.close();
//    mpcFile1.close();
    mpcFile0_res.close();
    mpcFile1_res.close();
    mpcFile0_uniq.close();

    logFile.close();

    return 0;//a.exec();
}
