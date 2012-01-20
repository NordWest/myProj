#include <QtCore>

#include "./../libs/mpcs.h"
#include "./../astro/astro.h"

int main(int argc, char *argv[])//mpcs.ini.txt mpcs.cat.txt mpcs.res.txt
{

    QCoreApplication a(argc, argv);

    setlocale(LC_NUMERIC, "C");

    mpcs mpc0, mpc1, mpc0res, mpc1res;

    mpc0.init(argv[1]);
    mpc1.init(argv[2]);
    mpc0res.init(QString("%1_res.txt").arg(QFileInfo(argv[1]).completeBaseName()).toAscii().data());
    mpc1res.init(QString("%1_res.txt").arg(QFileInfo(argv[2]).completeBaseName()).toAscii().data());

    int i, j, sz0, sz1;
    int obsNum;

    QSettings *sett = new QSettings("./findSimMpcs.ini", QSettings::IniFormat);

    bool ook;
    QString obsName = sett->value("general/obsName", "-1").toString();
    QString obsNumName;
    obsNum = obsName.toInt(&ook);
    if((obsNum!=-1)||(!ook))
    {
        unpackString(&obsNumName, obsName);
        obsNum = obsNumName.toInt();
        qDebug() << QString("obsName= %1\tobsNumName= %2\tobsNum= %3\n").arg(obsName).arg(obsNumName).arg(obsNum);
    }
    //if(namenum) obsNum = obsName
    //int useDtime = sett->value("general/useDtime", 0).toInt();
    double dtime = sett->value("general/dtime", 0.0).toDouble();
    dtime /= 1440.0;


    sz0 = mpc0.nstr;
    sz1 = mpc1.nstr;

    qDebug() << QString("sz0: %1\tsz1: %2\n").arg(sz0).arg(sz1);


    //DATEOBS dobs;

    mpc *rec0, *rec1, *recr;

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
        if(mpc0.getmpc(i)) qDebug() << "\nERROR reading mpc0\n";
        rec0 = mpc0.record;

        //qDebug() << QString("i: %1").arg(i);
        //qDebug() << QString("eJD|%1\n").arg(rec0->eJD);
        //qDebug() << QString("num|%1\n").arg(rec0->num);

        //rec0->eJD
        for(j=0;j<sz1;j++)
        {
            mpc1.getmpc(j);
            rec1 = mpc1.record;

            //qDebug() << QString("eJD|%1:\t%2\t%3\n").arg(rec0->eJD).arg(rec1->eJD).arg(fabs(rec0->eJD-rec1->eJD));
            //qDebug() << QString("num|%1:\t%2\n").arg(rec0->num).arg(rec1->num);

            if((fabs(rec0->eJD-rec1->eJD)<dtime)&&(rec0->num==rec1->num)&&(rec0->numOfObservatory==rec1->numOfObservatory))
            //if(fabs(rec0->eJD-rec1->eJD)<dtime)
            {
                recr = new mpc;
                recr->set_mpc(rec0);
                mpc0res.addmpc(recr);
                recr = new mpc;
                recr->set_mpc(rec1);
                mpc1res.addmpc(recr);
                qDebug() << QString("dr: %1\n").arg(fabs(rec0->eJD-rec1->eJD));
                break;
                //getDATEOBSfromMJD(&dobs, jd2mjd(recr->eJD));
            }
        }

    }
//

    return 0;//a.exec();
}
