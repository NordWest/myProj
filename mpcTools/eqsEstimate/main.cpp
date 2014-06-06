#include <QCoreApplication>

#include "./../../libs/redStat.h"
#include "./../../libs/mpccat.h"

struct objObsStatRec
{
    int objNum, onumRa, onumDe;
    double meanRa, rmsMeanRa, rmsOneRa;
    double meanDe, rmsMeanDe, rmsOneDe;
    objObsStatRec()
    {
        objNum = 0;
        onumRa = onumDe = 0;
        meanRa = rmsMeanRa = rmsOneRa = 0.0;
        meanDe = rmsMeanDe = rmsOneDe = 0.0;
    };
};

struct obsStatRec
{
    QString obsCode;
    objObsStatRec objStat;

    void addStat(objObsStatRec &nStat)
    {
        objStat.objNum++;
        objStat.onumRa += nStat.onumRa;
        objStat.meanRa += nStat.meanRa;
        objStat.rmsMeanRa += nStat.rmsMeanRa;
        objStat.rmsOneRa += nStat.rmsOneRa;
        objStat.onumDe += nStat.onumDe;
        objStat.meanDe += nStat.meanDe;
        objStat.rmsMeanDe += nStat.rmsMeanDe;
        objStat.rmsOneDe += nStat.rmsOneDe;
    }
};

struct obsStatList
{
    QList <obsStatRec*> osList;
    int addStat(QString oCode, objObsStatRec &oStat)
    {
        int i, sz;
        int nObs = -1;
        sz = osList.size();
        obsStatRec* osRec;
        for(i=0;i<sz;i++)
        {
            if(QString().compare(oCode,osList.at(i)->obsCode)==0)
            {
                nObs = i;
                osRec = osList.at(i);
                break;
            }
        }
        if(nObs==-1)
        {
            osRec = new obsStatRec;
            osRec->obsCode = oCode;
            osList << osRec;
        }
        osRec->addStat(oStat);
    };
    void doStat(int oMin)
    {
        int i, sz;
        sz = osList.size();
        for(i=sz-1;i>=0;i--)
        {
            if(osList.at(i)->objStat.objNum<oMin)
            {
                osList.removeAt(i);
                continue;
            }
            osList.at(i)->objStat.meanRa /= osList.at(i)->objStat.objNum;
            osList.at(i)->objStat.rmsMeanRa /= osList.at(i)->objStat.objNum;
            osList.at(i)->objStat.rmsOneRa /= osList.at(i)->objStat.objNum;
            osList.at(i)->objStat.meanDe /= osList.at(i)->objStat.objNum;
            osList.at(i)->objStat.rmsMeanDe /= osList.at(i)->objStat.objNum;
            osList.at(i)->objStat.rmsOneDe /= osList.at(i)->objStat.objNum;
        }
    };
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");


/*    eqFile eq_ini;
    eq_ini.init(eqFileName.toAscii().data());
*/
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
    int objMin = sett->value("eqsEstimate/objMin", 5).toInt();

/////////////////////////

    if(mCat.init(mpcCatFile.toAscii().data()))
    {
        qDebug() << "\nError MPCCAT init\n\n";
        return 1;
    }

    /*sz =eq_ini.size();
    //qDebug() << QString("eq num: %1\n").arg(sz);
    for(i=0;i<sz;i++)
    {
        obs_list.addEQ(eq_ini.at(i));
    }
*/

//    sz = obs_list.eqoList.size();
//    qDebug() << QString("Observatories num: %1\n").arg(sz);

    eqObsRec* obs_rec;
    eqObjRec* obj_rec;
    //obsStat* oStat;
    ocRec *oc_rec, *oc_recT;
    QList <ocRec*> ocList;
    QList <colRec*> colList;
    //QList <obsStat*> statList;

    objObsStatRec objStat;
    obsStatList obsStat;

    QString eqFileName(argv[1]);
    QFile iniFile(eqFileName);
    iniFile.open(QFile::ReadOnly);
    QTextStream iniStm(&iniFile);

    QFile wFile(QString("%1.we").arg(eqFileName.section(".", 0, -2)));
    wFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream wStm(&wFile);

    QFile resFile(QString("%1.res").arg(eqFileName.section(".", 0, -2)));
    resFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream resStm(&resFile);

    QFile ocFile(QString("%1_oc.txt").arg(eqFileName.section(".", 0, -2)));
    ocFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream ocStm(&ocFile);

    QFile mpcFile(QString("%1.mpc").arg(eqFileName.section(".", 0, -2)));
    mpcFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream mpcStm(&mpcFile);

    int objnum, onumRa, onumDe;
    double rmsRa, rmsDe;
    QFile obsFile(QString("%1.obs").arg(eqFileName.section(".", 0, -2)));
    obsFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream obsStm(&obsFile);

    QString obsCodeT, objNameT;
    int nObs, nObj;
    int objNumIni, obsNumIni, ocNumIni;
    int objNumRes, obsNumRes, ocNumRes;
    ocNumIni = objNumIni = obsNumIni = 0;
    ocNumRes = objNumRes = obsNumRes = 0;

    obsCodeT="";
    objNameT="";
    while(!iniStm.atEnd())
    {
        tStr = iniStm.readLine();
        if(tStr.contains("%")||tStr.contains("#")) continue;
        oc_rec = new ocRec;
        if(oc_rec->s2rec(tStr)) continue;

        ocNumIni++;
        //qDebug() << QString("%1@%2\n").arg(oc_rec->name).arg(oc_rec->obsCode);

        nObs = (QString().compare(oc_rec->obsCode, obsCodeT)!=0);
        nObj = (QString().compare(oc_rec->name, objNameT)!=0);
        if(nObs||nObj)
        {


            do3sigma(ocList, 0.0, 3.0);

            objNumIni++;


            if((ocList.size()>=obsMin)&&(!countCols(ocList, colList, "4,5"))&&(colList.at(0)->rmsOne<=ocMax)&&(colList.at(1)->rmsOne<ocMax))
            {
                is_mcat = !mCat.GetRecName(objNameT.toAscii().data());
                //if(countCols(ocList, colList, "4,5"))continue;
                //if((colList.at(0)->rmsOne>ocMax)||(colList.at(1)->rmsOne>ocMax)) continue;
                wStm << QString("%1@%2#%3|%4|%5|%6#%7|%8|%9|%10\n").arg(objNameT, 16).arg(obsCodeT, 3).arg(colList.at(0)->num, 5).arg(colList.at(0)->mean, 10, 'f', 2).arg(colList.at(0)->rmsMean, 10, 'f', 2).arg(colList.at(0)->rmsOne, 10, 'f', 2).arg(colList.at(1)->num, 5).arg(colList.at(1)->mean, 10, 'f', 2).arg(colList.at(1)->rmsMean, 10, 'f', 2).arg(colList.at(1)->rmsOne, 10, 'f', 2);
                szk = ocList.size();
                for(k=0;k<szk;k++)
                {
                    oc_recT = ocList.at(k);
                    oc_recT->rec2s(&tStr);
                    resStm << tStr << "\n";
                    ocNumRes++;
                    if(is_mcat)
                    {
                        mCat.record->getNumStr(tstr);
                        oc_recT->rec2MPC(&tStr, tstr);
                        mpcStm << tStr << "\n";
                    }
                    ocStm << QString("%1|%2|%3|%4|%5|%6|%7\n").arg(oc_recT->name, 16).arg(mjd2jd(oc_recT->MJday), 15, 'f', 7).arg(oc_recT->ra, 15, 'f', 11).arg(oc_recT->de, 15, 'f', 10).arg(oc_recT->ra-mas2grad(oc_recT->ocRaCosDe)/cos(grad2rad(oc_recT->de)), 15, 'f', 11).arg(oc_recT->de-mas2grad(oc_recT->ocDe), 15, 'f', 10).arg(oc_recT->obsCode);
                    //ocStm << QString("%1|%2|%3")
                }


                colList.at(0)->rec2s(&tStr);
                resStm << tStr << "\n";
                colList.at(1)->rec2s(&tStr);
                resStm << tStr << "\n";
                //resStm << "\n";

                //objStat.objNum++;
                objStat.onumRa = colList.at(0)->num;
                objStat.onumDe = colList.at(1)->num;
                objStat.meanRa = colList.at(0)->mean;
                objStat.rmsMeanRa = colList.at(0)->rmsMean;
                objStat.rmsOneRa = colList.at(0)->rmsOne;
                objStat.meanDe = colList.at(1)->mean;
                objStat.rmsMeanDe = colList.at(1)->rmsMean;
                objStat.rmsOneDe = colList.at(1)->rmsOne;
                obsStat.addStat(oc_recT->obsCode, objStat);

            }
/*

            if(nObs)
            {
                if(oStat!=NULL&&) statList << oStat;
                oStat = new obsStat;
            }
*/
            ocList.clear();
//ocList.clear();
            objNameT = oc_rec->name;
            obsCodeT = oc_rec->obsCode;
            //sz = ocList.size();
            //for(i=sz-1;i>=0;i--) ocList.removeAt(i);

        }

        ocList << oc_rec;
/*        if(objnum<10) continue;
        rmsRa /= objnum;
        rmsDe /= objnum;
*/

    }
    obsNumIni = obsStat.osList.size();
    obsStat.doStat(0);
    sz = obsStat.osList.size();
    obsNumRes = sz;
    for(i=0;i<sz;i++)
    {
        obsStm << QString("%1@%2#%3|%4|%5|%6#%7|%8|%9|%10\n").arg(obsStat.osList.at(i)->objStat.objNum, 5).arg(obsStat.osList.at(i)->obsCode, 3).arg(obsStat.osList.at(i)->objStat.onumRa, 5).arg(obsStat.osList.at(i)->objStat.meanRa, 10, 'f', 2).arg(obsStat.osList.at(i)->objStat.rmsMeanRa, 10, 'f', 2).arg(obsStat.osList.at(i)->objStat.rmsOneRa, 10, 'f', 2).arg(obsStat.osList.at(i)->objStat.onumDe, 5).arg(obsStat.osList.at(i)->objStat.meanDe, 10, 'f', 2).arg(obsStat.osList.at(i)->objStat.rmsMeanDe, 10, 'f', 2).arg(obsStat.osList.at(i)->objStat.rmsOneDe, 10, 'f', 2);
        objNumRes+=obsStat.osList.at(i)->objStat.objNum;
    }

    qDebug() << QString("ocNumIni= %1\tobjNumIni= %2\tobsNumIni = %3\n\n").arg(ocNumIni).arg(objNumIni).arg(obsNumIni);
    qDebug() << QString("ocNumRes= %1\tobjNumRes= %2\tobsNumRes = %3\n\n").arg(ocNumRes).arg(objNumRes).arg(obsNumRes);

    wFile.close();
    resFile.close();
    mpcFile.close();
    obsFile.close();
    ocFile.close();
    return 0;//a.exec();
}
