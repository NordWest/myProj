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
    double X[3], V[3], t0;

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
        ocat.record->getOrbElem(orb.elem);
        ostm << QString("%1: %2\t%3\t%4\t%5\t%6\t%7\t%8\n").arg(ocat.record->name).arg(ocat.record->eJD, 15, 'f', 8).arg(ocat.record->M0, 10, 'f', 7).arg(ocat.record->q, 10, 'f', 7).arg(ocat.record->ec, 10, 'f', 7).arg(ocat.record->inc, 10, 'f', 7).arg(ocat.record->w, 10, 'f', 7).arg(ocat.record->Node, 10, 'f', 7);
        t0 = orb.elem->eJD;
        orb.detRecEkv(&X[0], &X[1], &X[2], t0);
        orb.detRecEkvVel(&V[0], &V[1], &V[2], t0);

        findOrbEkv(&elem, X, V, t0);

        ostm << QString("elem\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(elem.eJD, 15, 'f', 8).arg(elem.M0, 10, 'f', 7).arg(elem.q, 10, 'f', 7).arg(elem.ec, 10, 'f', 7).arg(elem.inc, 10, 'f', 7).arg(elem.w, 10, 'f', 7).arg(elem.Node, 10, 'f', 7);


    }
    
    return 0;//a.exec();
}
