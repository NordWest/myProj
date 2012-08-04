#include <QtCore>

#include "./../libs/redStat.h"
#include "./../libs/mpccat.h"
#include "./../libs/comfunc.h"
//#include "./../libs/observatory.h"
//#include "./../libs/vectGrid3D.h"
//#include "./../libs/fitsdata.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    eqFile iFile;
    iFile.init(argv[1]);

    mpccat mpcc;
    mpcc.init("d:\\data\\cats\\MPCORB.DAT");

    int i, szi;

    szi = iFile.ocList.size();
    qDebug() << QString("rec nums: %1").arg(szi);

    QFile resFile("./res.txt");
    resFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream resStm(&resFile);

    int fRes;

    for(i=0; i<szi; i++)
    {
        qDebug() << QString("%1: |%2|\n").arg(i).arg((char*)iFile.ocList.at(i)->name.simplified().leftJustified(18, ' ').toAscii().data());
        fRes = mpcc.GetRecName((char*)iFile.ocList.at(i)->name.simplified().leftJustified(18, ' ').toAscii().data());
        qDebug() << QString("res: %1\tnum: %2\n").arg(fRes).arg(mpcc.record->number);
        resStm << QString("%1 ").arg(mpcc.record->getProvnum()) + getStrFromDATEOBS(getDATEOBSfromMJD(iFile.ocList.at(i)->MJday), " ", 0, 6) + " " + deg_to_hms(iFile.ocList.at(i)->ra, " ", 3) + " "+ deg_to_damas(iFile.ocList.at(i)->de, " ", 2) + " " + QString("%1 %2\n").arg(iFile.ocList.at(i)->ocRa, 8, 'f', 1, QLatin1Char(' ')).arg(iFile.ocList.at(i)->ocDe, 8, 'f', 1, QLatin1Char(' '));
 //       recStr->insert(0, QString("%1 %2 ").arg(name, 16).arg(mjd2jd(MJday), 13, 'f', 5) + getStrFromDATEOBS(getDATEOBSfromMJD(MJday), " ", 0, 6) + " " + deg_to_hms(ra, " ", 3) + " "+ deg_to_damas(de, " ", 2) + " " + QString("%1 %2").arg(ocRa, 8, 'f', 1, QLatin1Char(' ')).arg(ocDe, 8, 'f', 1, QLatin1Char(' ')));//+QString(" %1").arg(catName, 8));
    }


    resFile.close();

    return 0;
}
