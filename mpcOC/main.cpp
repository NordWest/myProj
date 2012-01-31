#include <QtCore/QCoreApplication>
#include "./../libs/mpcfile.h"
#include "./../libs/comfunc.h"
#include "./../libs/fitsdata.h"
#include "./../libs/redStat.h"
#include "./../libs/orbcat.h"
#include <QString>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");
    mpcFile mpFile;
    eqFile eqResFile;
    objResRec *orTemp;
    ocRec *ocTemp;
    mpephRec mpeRec;
    QString fileName = QString(argv[1]);
    int i, sz, getRes;
    QString tStr;


    mpFile.init(fileName);
    qDebug() << QString("mpc size: %1\n").arg(mpFile.size());
/*
    for(i=0; i<mpFile.size(); i++)
    {
        mpFile.at(i)->getProvDest(tStr);
        qDebug() << QString("%1: %2\n").arg(i).arg(tStr);
    }

    }*/


    QFileInfo fi(fileName);
    QString oFileName;
    if(argc>2)
    {
        oFileName = QString(argv[2]);
    }
    else
    {
        oFileName = QString("%1/%2_eq.txt").arg(fi.absolutePath()).arg(fi.completeBaseName());
    }

    QString cfgFileName = "mpcOC.ini";

    procData mpephProcData;
    procData miriProcData;

    QSettings *sett = new QSettings(cfgFileName, QSettings::IniFormat);

//objects   ////
    //int lspmFind = sett->value("objects/lspmFind", 0).toInt();
    //int skybotFind = sett->value("objects/skybotFind", 0).toInt();
    //int tryMpeph = sett->value("general/tryMpeph", 0).toInt();
    QString mpcOrbCat = sett->value("general/mpcOrbCat", "mpcorb.dat").toString();
    int useMiriade = sett->value("general/useMiriade", 0).toInt();
    //int mpephType = sett->value("objects/mpephType", 0).toInt();
    //double magObj0 = sett->value("objects/mag0", 6.0).toDouble();
    //double magObj1 = sett->value("objects/mag1", 16.0).toDouble();
    //QString headObjName = sett->value("objects/headObjName", "OBJECT").toString();

    mpephProcData.name = sett->value("processes/mpeph_prog", "./mpeph").toString();
    mpephProcData.folder = sett->value("processes/mpeph_prog_folder", "./").toString();
    mpephProcData.waitTime = sett->value("processes/mpeph_wait_time", -1).toInt();
    if(mpephProcData.waitTime>0) mpephProcData.waitTime *= 1000;
    miriProcData.name = sett->value("processes/miriade_prog", "./miriadeEph").toString();
    miriProcData.folder = sett->value("processes/miriade_prog_folder", "./").toString();
    miriProcData.waitTime = sett->value("processes/miriade_wait_time", -1).toInt();
    if(miriProcData.waitTime>0) miriProcData.waitTime *= 1000;


    OrbCat mpCat;
    if(mpCat.init(mpcOrbCat.toAscii().data()))
    {
        qDebug() << QString("mpoCat init error!\n");
        return 1;
    }
    qDebug() << QString("mpCat size: %1\n").arg(mpCat.nstr);

    QString objName;
    bool isNum;
    int objNum;
    double mjd, ra, de, magn;
    char *upstr = new char[256];
    sz = mpFile.size();
    for(i=0; i<sz; i++)
    {
        mpFile.at(i)->getMpNumber(objName);
        //objName.simplified();
        objNum = objName.toInt(&isNum);

        if(!isNum||objNum<=0)
        {
            mpFile.at(i)->getProvDest(objName);
            upackDigStr(upstr, objName.toAscii().data());
            objName = QString(upstr);
        }
        else
        {
            if(mpCat.GetRec(objNum-1)!=-1) objName = QString(mpCat.record->name);
            else objName = QString("noname");
        }
        qDebug() << QString("%1: %2 - %3\n").arg(i).arg(objNum).arg(objName);
        mjd = mpFile.at(i)->mjd();
        ra = mpFile.at(i)->ra();
        de = mpFile.at(i)->dec();
        magn = mpFile.at(i)->magn();

        if(useMiriade) getRes = getMiriadeObject(&mpeRec, mjd, objName, miriProcData);
        else getRes = getMpephObject(&mpeRec, mjd, objName, 1, mpephProcData);

        if(!getRes)
        {
            ocTemp = new ocRec;
            ocTemp->name = objName;
            ocTemp->MJday = mjd;
            ocTemp->ra = ra;
            ocTemp->de = de;
            ocTemp->mag0 = magn;
            ocTemp->ocRaCosDe = grad_to_mas(ra*cos(grad2rad(de)) - mpeRec.ra*cos(grad2rad(mpeRec.de)));
            ocTemp->ocDe = grad_to_mas(de - mpeRec.de);
            ocTemp->elong = mpeRec.elong;
            ocTemp->ocMag = magn - mpeRec.Vmag;
            ocTemp->phase = mpeRec.phase;
            ocTemp->Vr = mpeRec.Vr;
            ocTemp->muRaCosDe = mpeRec.muRaCosDe;
            ocTemp->muDe = mpeRec.muDe;
            ocTemp->topDist = mpeRec.topDist;
            if(useMiriade) ocTemp->catName = QString("miriade");
            else ocTemp->catName = QString("mpeph");
            ocTemp->catMagName = QString("Vmag");
            ocTemp->rec2s(&tStr);
            qDebug() << tStr << "\n";
            eqResFile.ocList.append(ocTemp);
        }
        else qDebug() << "\ngetMpephObject\n";


        qDebug() << QString("%1: %2\n").arg(i).arg(tStr);
    }

    /*
    sz = orFile.ocList.size();

    for(i=0; i<sz; i++)
    {
        orTemp = orFile.ocList.at(i);
        if(useMiriade) getRes = getMiriadeObject(&mpeRec, orTemp->mJD, orTemp->name, miriProcData);
        else getRes = getMpephObject(&mpeRec, orTemp->mJD, orTemp->name, 1, mpephProcData);

        if(!getRes)
        {
            ocTemp = new ocRec;
            orTemp->toOcRec(ocTemp);
            ocTemp->ocRaCosDe = grad_to_mas(orTemp->ra*cos(grad2rad(orTemp->de)) - mpeRec.ra*cos(grad2rad(mpeRec.de)));
            ocTemp->ocDe = grad_to_mas(orTemp->de - mpeRec.de);
            ocTemp->elong = mpeRec.elong;
            ocTemp->ocMag = orTemp->mag - mpeRec.Vmag;
            ocTemp->phase = mpeRec.phase;
            ocTemp->Vr = mpeRec.Vr;
            ocTemp->muRaCosDe = mpeRec.muRaCosDe;
            ocTemp->muDe = mpeRec.muDe;
            ocTemp->topDist = mpeRec.topDist;
            if(useMiriade) ocTemp->catName = QString("miriade");
            else ocTemp->catName = QString("mpeph");
            ocTemp->rec2s(&tStr);
            qDebug() << tStr << "\n";
            eqResFile.ocList.append(ocTemp);
        }
        else qDebug() << "\ngetMpephObject\n";
    }
*/
    qDebug() << QString("save %1\n").arg(oFileName);
    eqResFile.saveAs(oFileName);

    return 0;//a.exec();
}
