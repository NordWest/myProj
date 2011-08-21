#include <QtCore>

#include "./../libs/mpcs.h"
#include "./../libs/astro.h"

int main(int argc, char *argv[])//mpcs.ini.txt mpcs.cat.txt mpcs.res.txt
{
    QCoreApplication a(argc, argv);

    mpcs mpc0, mpc1, mpcres;

    mpc0.init(argv[1]);
    mpc1.init(argv[2]);
    mpcres.init(argv[3]);

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
    int useDtime = sett->value("general/useDtime", 0).toInt();
    double dtime = sett->value("general/dtime", 0.0).toDouble();


    sz0 = mpc0.nstr;
    sz1 = mpc1.nstr;


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
        mpc0.getmpc(i);
        rec0 = mpc0.record;

        //rec0->eJD
        for(j=0;j<sz1;j++)
        {
            mpc1.getmpc(j);
            rec1 = mpc1.record;

            if((fabs(rec0->eJD-rec1->eJD)<dtime)&&(rec0->num==rec1->num)&&(rec0->numOfObservatory==rec1->numOfObservatory))
            //if(fabs(rec0->eJD-rec1->eJD)<dtime)
            {
                recr = new mpc;
                recr->set_mpc(rec1);
                mpcres.addmpc(recr);
                //getDATEOBSfromMJD(&dobs, jd2mjd(recr->eJD));
            }
        }

    }
//

    return 0;//a.exec();
}
