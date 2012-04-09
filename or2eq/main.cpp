#include <QtCore/QCoreApplication>
#include "./../libs/fitsdata.h"
#include "./../libs/redStat.h"
#include <QString>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    objResidualFile orFile;
    eqFile eqResFile;
    objResRec *orTemp;
    ocRec *ocTemp;
    mpephRec mpeRec;
    QString fileName = QString(argv[1]);

    orFile.init(fileName);
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

    QFile resFile("resObj.txt");
    resFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream resStm(&resFile);

    QString cfgFileName = "or2eq.ini";

    procData mpephProcData;
    procData miriProcData;

    QSettings *sett = new QSettings(cfgFileName, QSettings::IniFormat);

//objects   ////
    //int lspmFind = sett->value("objects/lspmFind", 0).toInt();
    //int skybotFind = sett->value("objects/skybotFind", 0).toInt();
    //int tryMpeph = sett->value("general/tryMpeph", 0).toInt();
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

    int i, sz, getRes;
    QString tStr;
    objResultsRec resRec;
    sz = orFile.ocList.size();
    for(i=0; i<sz; i++)
    {
        orFile.ocList.at(i)->toResultsRec(&resRec);
        resRec.rec2s(&tStr);
        resStm << tStr << "\n";
    }
    resFile.close();
/*
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

    qDebug() << QString("save %1\n").arg(oFileName);
    eqResFile.saveAs(oFileName);
*/
    return 0;//a.exec();
}
