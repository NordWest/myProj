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



    skyAreaLF sa;
    int yr, mth, day, hr, min;
    double sec, jDay, s;
    //sa = new SkyArea;

    sa.init(obsCatName, deleCatName, installDir);


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


    sa.grade(rList);

    rList.saveAs("res.lst");

    //delete sa;

    return 0;//a.exec();
}
