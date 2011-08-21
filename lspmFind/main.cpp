#include <QtCore>
#include <QCoreApplication>
#include "./../libs/sscatFB.h"
#include "./../libs/astro.h"
#include "./../libs/comfunc.h"

int main(int argc, char *argv[])    //getLSPM.exe ra de {rad|square|} LSPMcat muMin
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    if(argc<5) return 1;

    QTextStream stream(stdout);

    QString tStr, areaType, areaSize;
    QStringList tList;
    double ra_oc, de_oc, distMax, dist;

    tStr = QString(argv[1]);
    ra_oc = hms_to_deg(tStr, ":");
    tStr = QString(argv[2]);
    de_oc = damas_to_deg(tStr, ":");
    tStr = QString(argv[3]);
    areaType = tStr.section("=", 0, 0);
    areaSize = tStr.section("=", 1, 1);
    int selType;
    //qDebug() <<QString("%1=%2").arg(areaType).arg(areaSize);
    if(areaType=="r")
    {
        selType=0;
        distMax=areaSize.toDouble()/60.0;//grad
    }



    sscatFB *starCat = new sscatFB;
    if(starCat->init(argv[4]))
    {
        stream << "err: catalog not found\n";
        return 2;
    }

    double muMin;
    muMin = 0;
    if(argc==6) muMin = atof(argv[5]);

    int i=0;
    char *str_out = new char[1024];
    double raC, deC, cosD;
    cosD = cos(grad2rad(ra_oc));
    while(!starCat->GetRec(i))
    {
        //qDebug() << QString("pos= %1\n").arg(i);

        raC = starCat->record->RAdeg;
        deC = starCat->record->DEdeg;

        dist = sqrt(pow((raC-ra_oc)*cosD, 2.0) + pow(deC-de_oc, 2.0));
        //qDebug() << QString("pos= %1\tdist= %2\n").arg(i).arg(dist-distMax);
        if(dist<distMax&&((argc!=6)||(muMin<starCat->record->pm)))
        {
            starCat->rec2s(str_out);
            stream << str_out;
        }
        i++;
    }
/*
    if(starCat->GetName(argv[1])==-1)
    {
        stream << "err: star not found\n";
        return 3;
    }

    char *str_out = new char[1024];
    starCat->rec2s(str_out);

    stream << str_out << "\n";


*/
    delete [] str_out;
    return 0;
   // return a.exec();
}
