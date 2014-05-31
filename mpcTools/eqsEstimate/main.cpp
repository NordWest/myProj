#include <QCoreApplication>

#include "./../../libs/redStat.h"
#include "./../../libs/mpccat.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    QString eqFileName(argv[1]);
    eqFile eq_ini;
    eq_ini.init(eqFileName.toAscii().data());

    QString resFolder = "./eqStat";
    QDir resDir(resFolder);

    int sz, i, j, szj, k, szk, is_mcat;
    QString tStr, tStr1;
    char *tstr = new char[256];

    eqObsList obs_list;
    mpccat mCat;

//Settings
    QSettings *sett = new QSettings("./nb.ini", QSettings::IniFormat);
    QString mpcCatFile = sett->value("general/mpcCatFile", "mocorb.txt").toString();
    int obsMin = sett->value("eqsEstimate/obsMin", 2).toInt();
    double ocMax = sett->value("eqsEstimate/ocMax", 100000).toDouble();

/////////////////////////

    if(mCat.init(mpcCatFile.toAscii().data()))
    {
        qDebug() << "\nError MPCCAT init\n\n";
        return 1;
    }

    sz =eq_ini.size();
    qDebug() << QString("eq num: %1\n").arg(sz);
    for(i=0;i<sz;i++)
    {
        obs_list.addEQ(eq_ini.at(i));
    }

    sz = obs_list.eqoList.size();
    qDebug() << QString("Observatories num: %1\n").arg(sz);

    eqObsRec* obs_rec;
    eqObjRec* obj_rec;
    ocRec *oc_rec;
    QList <colRec*> colList;
    QFile wFile(QString("%1.we").arg(eqFileName.section(".", 0, -2)));
    wFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream wStm(&wFile);

    QFile resFile(QString("%1.res").arg(eqFileName.section(".", 0, -2)));
    resFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream resStm(&resFile);

    QFile mpcFile(QString("%1.mpc").arg(eqFileName.section(".", 0, -2)));
    mpcFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream mpcStm(&mpcFile);

    int objnum, onumRa, onumDe;
    double rmsRa, rmsDe;
    QFile obsFile(QString("%1.obs").arg(eqFileName.section(".", 0, -2)));
    obsFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream obsStm(&obsFile);

    for(i=0;i<sz;i++)
    {
        obs_rec = obs_list.eqoList.at(i);
        //qDebug() << QString("obsCode: %1\n").arg(obs_rec->obsCode);
        szj = obs_rec->objList.eqrList.size();
        //qDebug() << QString("objNum: %1\n").arg(szj);
        objnum=0; onumRa=0; onumDe=0;
        rmsRa = rmsDe = 0.0;
        for(j=0;j<szj;j++)
        {
            obj_rec = obs_rec->objList.eqrList.at(j);

            is_mcat = !mCat.GetRecName(obj_rec->objName.toAscii().data());

            do3sigma(obj_rec->eq_list, 0.0, 3.0);

            if(obj_rec->eq_list.size()<obsMin) continue;
            if(countCols(obj_rec->eq_list, colList, "4,5"))continue;
            if((colList.at(0)->rmsOne>ocMax)||(colList.at(1)->rmsOne>ocMax)) continue;
            wStm << QString("%1@%2#%3|%4|%5|%6#%7|%8|%9|%10\n").arg(obj_rec->objName, 16).arg(obs_rec->obsCode, 3).arg(colList.at(0)->num, 5).arg(colList.at(0)->mean, 10, 'f', 2).arg(colList.at(0)->rmsMean, 10, 'f', 2).arg(colList.at(0)->rmsOne, 10, 'f', 2).arg(colList.at(1)->num, 5).arg(colList.at(1)->mean, 10, 'f', 2).arg(colList.at(1)->rmsMean, 10, 'f', 2).arg(colList.at(1)->rmsOne, 10, 'f', 2);
            szk = obj_rec->eq_list.size();
            for(k=0;k<szk;k++)
            {
                oc_rec = obj_rec->eq_list.at(k);
                oc_rec->rec2s(&tStr);
                resStm << tStr << "\n";

                if(is_mcat)
                {
                    mCat.record->getNumStr(tstr);
                    oc_rec->rec2MPC(&tStr, tstr);
                    mpcStm << tStr << "\n";
                }
            }

            colList.at(0)->rec2s(&tStr);
            resStm << tStr << "\n";
            colList.at(1)->rec2s(&tStr);
            resStm << tStr << "\n";
            resStm << "\n";

            objnum++;
            onumRa += colList.at(0)->num;
            onumDe += colList.at(1)->num;
            rmsRa += colList.at(0)->rmsOne;
            rmsDe += colList.at(1)->rmsOne;
        }

        if(objnum<10) continue;
        rmsRa /= objnum;
        rmsDe /= objnum;

        obsStm << QString("%1@%2#%3|%4#%5|%6\n").arg(objnum, 5).arg(obs_rec->obsCode, 3).arg(onumRa, 5).arg(rmsRa, 10, 'f', 2).arg(onumDe, 5).arg(rmsDe, 10, 'f', 2);
    }

    wFile.close();
    resFile.close();
    mpcFile.close();
    obsFile.close();
    return 0;//a.exec();
}
