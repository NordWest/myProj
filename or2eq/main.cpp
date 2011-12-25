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
        oFileName = QString("%1_eq.txt").arg(fi.completeBaseName());
    }

    QString cfgFileName = "or2eq.ini";

    procData mpephProcData;

    QSettings *sett = new QSettings(cfgFileName, QSettings::IniFormat);

    mpephProcData.name = sett->value("processes/mpeph_prog", "./mpeph").toString();
    mpephProcData.folder = sett->value("processes/mpeph_prog_folder", "./").toString();
    mpephProcData.waitTime = sett->value("processes/mpeph_wait_time", -1).toInt();
    if(mpephProcData.waitTime>0) mpephProcData.waitTime *= 1000;

    int i, sz;
    QString tStr;
    sz = orFile.ocList.size();

    for(i=0; i<sz; i++)
    {
        orTemp = orFile.ocList.at(i);

        if(!getMpephObject(&mpeRec, orTemp->mJD, orTemp->name, 1, mpephProcData))
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
            ocTemp->rec2s(&tStr);
            qDebug() << tStr << "\n";
            eqResFile.ocList.append(ocTemp);
        }
        else qDebug() << "\ngetMpephObject\n";
    }

    qDebug() << QString("save %1\n").arg(oFileName);
    eqResFile.saveAs(oFileName);

    return 0;//a.exec();
}
