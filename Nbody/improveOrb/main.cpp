#include <QCoreApplication>
#include "./../../libs/observ.h"
#include "./../../libs/orbcat.h"
#include "./../../libs/orbit.h"

#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    orbit orb;
    orbElem elem;
    eposrec erec;
    double X[3], V[3], t0;




//
//Settings
        QSettings *sett = new QSettings("./nb.ini", QSettings::IniFormat);

        QString jplFile = sett->value("general/jplFile", "./../../data/cats/binp1940_2020.405").toString();
        QString epmDir = sett->value("general/epmDir", "./").toString();
        QString obsFile = sett->value("general/obsFile", "./../../data/cats/Obs.txt").toString();
        QString obsCode = sett->value("general/obsCode", "500").toString();
        QString mpcCatFile = sett->value("general/mpcCatFile", "mocorb.txt").toString();
        QString confFile = sett->value("general/confFile", "particles.xml").toString();
        QString moodyDir = QDir(sett->value("general/moodyDir", "./testProject").toString()).absolutePath();


        sk = sett->value("general/sk", 0).toInt();
        center = sett->value("general/center", 0).toInt();

//////////////////////////////

    observ opos;
    opos.init(obsFile, jplFile);

    OrbCat ocat;
    ocat.init(argv[1]);

    int i, sz;
    sz = ocat.nstr;

    QTextStream ostm(stdout);


    ostm << QString("\t\t: eJD\t\t\tM0\t\tq\t\tec\t\tinc\t\tw\t\tNode\t\n");
    ostm << QString("==============================================================================\n");

    for(i=0;i<sz;i++)
    {
        ocat.GetRec(i);
        orb.get(&ocat);
        ostm << QString("%1: %2\t%3\t%4\t%5\t%6\t%7\t%8\n").arg(ocat.record->name).arg(ocat.record->eJD, 15, 'f', 8).arg(ocat.record->M0, 10, 'f', 7).arg(ocat.record->q, 10, 'f', 7).arg(ocat.record->ecc, 10, 'f', 7).arg(ocat.record->inc, 10, 'f', 7).arg(ocat.record->w, 10, 'f', 7).arg(ocat.record->Node, 10, 'f', 7);

        t0 = orb.elem->eJD;
        orb.detRecEkv(&X[0], &X[1], &X[2], t0);
        orb.detRecEkvVel(&V[0], &V[1], &V[2], t0);

        findOrb(&elem, X, V, t0);

        elem.set(&erec);

        ostm << QString("elem\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(erec.eJD, 15, 'f', 8).arg(erec.M0, 10, 'f', 7).arg(erec.q, 10, 'f', 7).arg(erec.ecc, 10, 'f', 7).arg(erec.inc, 10, 'f', 7).arg(erec.w, 10, 'f', 7).arg(erec.Node, 10, 'f', 7);

        ostm << QString("elem\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(erec.eJD-ocat.record->eJD, 15, 'f', 8).arg(erec.M0-ocat.record->M0, 10, 'f', 7).arg(erec.q-ocat.record->q, 10, 'f', 7).arg(erec.ecc-ocat.record->ecc, 10, 'f', 7).arg(erec.inc-ocat.record->inc, 10, 'f', 7).arg(erec.w-ocat.record->w, 10, 'f', 7).arg(erec.Node-ocat.record->Node, 10, 'f', 7);


    }
    
    return 0;//a.exec();
}
