#include <QtCore/QCoreApplication>
#include "./../libs/observ.h"
#include "./../libs/orbit.h"
#include "./../libs/skyarea.h"
#include "./../libs/astro.h"
#include "./../libs/comfunc.h"
#include "./../libs/redStat.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    observ obsPos;

    procData miriadeProcData;
    miriadeProcData.name = "./miriadeEph";
    miriadeProcData.folder = "./";
    miriadeProcData.waitTime = -1;

    obsPos.init("./Obs.txt", "./../../data/cats/bin1940_2020.lin.405");

    obsPos.set_obs_parpam(GEOCENTR_NUM, CENTER_BARY, SK_EKVATOR, "084");


    IList iList;
    OrbCat iCat;
    orbit orb_elem;
    double Sdist, Edist, x, y, z, magn, ra, de, utc;
    QString tRa, tDe;
    double dRa, dDe;

    iList.init("ini.lst");
    iCat.init("ini.cat");

    int szi, i;
    szi = iList.nstr;

    utc = 2454500.5;

    obsPos.det_observ(utc);

    QProcess outerProcess;
    QStringList outerArguments;
    QString objDataStr;
    mpephRec mpcObj;

    for(i=0; i<szi; i++)
    {
        iList.GetRec(i);

        iCat.GetRecName(iList.record->name);

        orb_elem.get(&iCat);
        orb_elem.detRecEkv(&x, &y, &z, obsPos.otime);
        //res_list->record->set_name(iCat->record->name);
        //sprintf(this->res_list->record->num, "%8d", this->orb_catalog->record->number);

        detRDnumGC(&ra, &de, x, y, z, obsPos.ox, obsPos.oy, obsPos.oz, obsPos.obs->dcx, obsPos.obs->dcy, obsPos.obs->dcz);

        Sdist = sqrt(x*x + y*y + z*z);
        Edist = sqrt((obsPos.ox - x)*(obsPos.ox - x) + (obsPos.oy - y)*(obsPos.oy - y) + (obsPos.oz - z)*(obsPos.oz - z));

        magn = det_m(iCat.record->H, Sdist, Edist, 5.8, detPhase(obsPos.ox, obsPos.oy, obsPos.oz, x, y, z));

        deg_to_hms(&tRa, rad2grad(ra), " ", 4);
        deg_to_damas(&tDe, rad2grad(de), " ", 3);
        qDebug() << QString("name: %1\tra: %2\tde: %3\n").arg(iList.record->name).arg(tRa).arg(tDe);

        outerArguments.clear();
        outerArguments << QString("-name=\"%1\"").arg(QString(iList.record->name).simplified());
        QString sJD = QString("%1").arg(utc, 11, 'f',7);
        outerArguments << QString("-ep=%1").arg(sJD);

        outerProcess.setWorkingDirectory(miriadeProcData.folder);
        outerProcess.setProcessChannelMode(QProcess::MergedChannels);
        outerProcess.setReadChannel(QProcess::StandardOutput);

        //qDebug() << "outerArguments: " << miriadeProcData.name << " " << outerArguments.join("|") << "\n";

        outerProcess.start(miriadeProcData.name, outerArguments);

        if(!outerProcess.waitForFinished(miriadeProcData.waitTime))
        {
            qDebug() << "\nmpephProc finish error\n";
        }
        QTextStream objStream(outerProcess.readAllStandardOutput());

        while (!objStream.atEnd())
        //for(i=0; i<orList.size(); i++)
        {
            objDataStr = objStream.readLine();
            //objDataStr = orList.at(i);
            if(objDataStr.at(0)=='#') continue;
            //qDebug() << QString("objDataStr: %1").arg(objDataStr);


            if(mpcObj.fromMiriStr(objDataStr))
            {
                qDebug() << "\nfromString error\n";
                continue;
            }
            else break;
        }

        deg_to_hms(&tRa, mpcObj.ra, " ", 4);
        deg_to_damas(&tDe, mpcObj.de, " ", 3);
        qDebug() << QString("miriade: %1\t%2\n").arg(tRa).arg(tDe);

        dRa = fabs(mpcObj.ra - rad2grad(ra));
        dDe = fabs(mpcObj.de -rad2grad(de) );
        qDebug() << QString("dRa: %1\tdDe: %2\n").arg(grad_to_mas(dRa)/1000).arg(grad_to_mas(dDe)/1000);
    }


    return 0;//a.exec();
}
