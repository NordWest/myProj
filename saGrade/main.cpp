#include <QtCore/QCoreApplication>
#include "./../libs/observ.h"
#include "./../libs/orbit.h"
#include "./../libs/skyarealf.h"
#include "./../libs/astro.h"
#include "./../libs/comfunc.h"
#include "./../libs/redStat.h"
#include "./../libs/mpcfile.h"

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);

    QSettings sett("./saGrade.ini", QSettings::IniFormat);

    QString mpcCatName = sett.value("general/mpcCatName", "mpcorb.dat").toString();
    QString deleCatName = sett.value("general/deleCatName", "dele.cat").toString();
    QString obsCatName = sett.value("general/obsCatName", "Obs.txt").toString();
    QString installDir = sett.value("general/installDir", "./").toString();
    QString obsCode = sett.value("general/obsCode", "084").toString();

    procData miriadeProcData;
    miriadeProcData.name = sett.value("miriadeProcData/name", "./mpeph.exe").toString();
    miriadeProcData.folder = sett.value("miriadeProcData/folder", "./").toString();
    miriadeProcData.waitTime = sett.value("miriadeProcData/waitTime", -1).toInt();
    if(miriadeProcData.waitTime>0) miriadeProcData.waitTime *= 1000;


    skyAreaLF sa;
    int yr, mth, day, hr, min;
    double sec, jDay, s;

    //sa = new SkyArea;

    sa.init(obsCatName, deleCatName, installDir);
    sa.obs_pos->set_obs_parpam(GEOCENTR_NUM, CENTER_SUN, SK_EKVATOR, obsCode.toAscii().data());

    QDateTime dtCurr;
    dtCurr = QDateTime().currentDateTime();

    QString utcStr = dtCurr.toString("yyyy MM dd hh mm ss.zzz");
    qDebug() << QString("cTimeStr: %1\n").arg(utcStr);
    yr = utcStr.section(" ", 0, 0).toInt();
    mth = utcStr.section(" ", 1, 1).toInt();
    day = utcStr.section(" ", 2, 2).toInt();
    hr = utcStr.section(" ", 3, 3).toInt();
    min = utcStr.section(" ", 4, 4).toInt();
    sec = utcStr.section(" ", 5, 5).toDouble();
    qDebug() << QString("%1|%2|%3|%4|%5|%6\n").arg(yr).arg(mth).arg(day).arg(hr).arg(min).arg(sec);
    dat2JD_time(&jDay, yr, mth, day, hr, min, sec);

    qDebug() << QString("jDay: %1\n").arg(jDay, 12, 'f', 5);

    //UTC2s_alt(jDay, sa.obs_pos->obs->record->Long, &s);
    UTC2s(jDay, 0, &s);

    qDebug() << QString("s: %1\n").arg(s);

    sa.initVisualProp(jDay);
    sa.set_opt(0, 360, -10, 90, 4, 18);

    //deg_to_damas(&strT0, le de, QString spl_symb, int ndig);

    qDebug() << QString("s: %1\ntr: %2\nts: %3\n").arg(s).arg(rad2grad(sa.params.timeSunRise)).arg(rad2grad(sa.params.timeSunSet));

    sa.params.ignore_task_rules = 1;

    //sa.UpdateTaskCatalog(3);

    resList rList;
    resRecord *rRec;
    tlRecord *taskRec = new tlRecord;
    catRecord *cRec = new catRecord;
    resRecord *rRecD = new resRecord;

    QStringList outerArguments;
    QProcess outerProcess;
    QString name, sJD, objDataStr;
    QStringList resSL;
    mpephRec mpephR;

    sa.grade(rList);

    rList.saveAs("res.lst");

    int i, j, sz, isObj;

    qDebug() << QString("proof mpc\n\n");

    sJD = QString("%1").arg(sa.obs_pos->ctime.UTC(), 15, 'f',7);
    sz = rList.size();
    for(i=0; i<sz; i++)
    {
        rRec = rList.at(i);
        for(j=0; j<rRec->tasks.size(); j++)
        {
            if(sa.getTaskCat(rRec->tasks.at(j), taskRec, cRec))continue;
            if(cRec->catType==MPC_CAT_TYPE)
            {
                name = rRec->name;
                outerArguments.clear();

                outerArguments << QString("-name=%1").arg(name.simplified().toLower());


                outerArguments << QString("-ep=%1").arg(sJD);
                //outerArguments << QString("-ep=%1").arg(time, 15, 'f',7);

        /*        if(useEPM) plaNum = epm_planet_num(name);
                else plaNum = planet_num(name.toAscii().data());
                if(plaNum!=-1) outerArguments << "-type=planet";
                else */

                outerArguments << "-type=aster";

                //outerArguments << QString("-observer=%1").arg(obsCode);
                //outerArguments << QString("-observer=@sun");
                //outerArguments << "-tcoor=2";
                //outerArguments << "-rplane=1";

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

                isObj=0;
                while (!objStream.atEnd())
                {
                    objDataStr = objStream.readLine();


                    if(objDataStr.size()<1) continue;
                    if(objDataStr.at(0)=='#') continue;
                    isObj = !mpephR.fromMiriStr(objDataStr);
                    qDebug() << QString("objDataStr: %1").arg(objDataStr);
                    break;

                }

                if(!isObj) break;

                rRec->toString(objDataStr);
                qDebug() << QString("resRec: %1").arg(objDataStr);

                rRecD->ra = rRec->ra - mpephR.ra;
                rRecD->dec = rRec->dec - mpephR.de;
                rRecD->mu_ra = rRec->mu_ra - mpephR.muRaCosDe;
                rRecD->mu_dec = rRec->mu_dec - mpephR.muDe;

                rRecD->toString(objDataStr);
                qDebug() << QString("resRecD: %1").arg(objDataStr);

            }
        }
    }

    //delete sa;

    return 0;//a.exec();
}
