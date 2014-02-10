#include <QCoreApplication>
#include "./../../libs/observ.h"
#include "./../../libs/orbcat.h"
#include "./../../libs/orbit.h"

#include <iostream>

struct improveState
{
    double jdTDB;
    double raO, decO;
    double raC, decC;
    double state[6];
};

struct inproveObject
{
    QString name;
    QList <improveState*> impList;
    orbit iniOrb;
    orbit impOrb;
};


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    orbit orb;
    orbElem *elem;
    eposrec erec;
    double X[3], V[3], t0;
    int sk, center;




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
    opos.init(obsFile.toAscii().data(), jplFile.toAscii().data());

    OrbCat ocat;
    ocat.init(argv[1]);

    int i, sz, nObj;
    sz = ocat.nstr;

    QTextStream ostm(stdout);


    QFile stFile(argv[2]);
    stFile.open(QFile::ReadOnly);
    QTextStream stStm(&stFile);

    QString tStr, objName;
    QStringList opers;

    QList <inproveObject*> impObjList;
    improveState* impSt;
    inproveObject* impObj;

    while(!stStm.atEnd())
    {
        opers = stStm.readLine().split("|");
        objName = opers.at(0).simplified();
        impSt = new improveState;
        impSt->jdTDB = opers[1].toDouble();
        impSt->raO = opers[2].toDouble();
        impSt->decO = opers[3].toDouble();
        impSt->raC = opers[4].toDouble();
        impSt->decC = opers[5].toDouble();
        impSt->state[0] = opers[6].toDouble();
        impSt->state[1] = opers[7].toDouble();
        impSt->state[2] = opers[8].toDouble();
        impSt->state[3] = opers[9].toDouble();
        impSt->state[4] = opers[10].toDouble();
        impSt->state[5] = opers[11].toDouble();

        nObj = 1;
        sz = impObjList.size();
        for(i=0; i<sz; i++)
        {
            if(QString().compare(objName, impObjList.at(i)->name)==0)
            {
                nObj=0;
                impObjList.at(i)->impList << impSt;
                break;
            }
        }
        if(nObj)
        {
            impObj = new inproveObject;
            impObj->name = objName;
            qDebug() << QString("nObj: %1\n").arg(objName);
            if(ocat.GetRecName(objName.toAscii().data())==-1) continue;
            impObj->iniOrb.get(&ocat);
            impObj->impList << impSt;
            impObjList << impObj;
        }
    }

    stFile.close();

    int p;
    int oNum = impObjList.size();
    qDebug() << QString("improve objects: %1\n").arg(oNum);

    double *Ei = new double[6];
    double *ai, *bi, *ci;
    double P[3], Q[3];
    double H, K, r, ss, aval;

    for(p=0; p<oNum; p++)
    {
        impObj = impObjList.at(p);
        elem = impObj->iniOrb.elem;

        qDebug() << QString("%1: %2\n\n").arg(p).arg(impObj->name);

        sz = impObj->impList.size();
        for(i=0; i<sz; i++)
        {
            impSt = impObj->impList.at(i);
            r = sqrt(impSt->state[0]*impSt->state[0] + impSt->state[1]*impSt->state[1] + impSt->state[2]*impSt->state[2]);
            ss = impSt->state[0]*impSt->state[3] + impSt->state[1]*impSt->state[4] + impSt->state[2]*impSt->state[5];
            aval = elem->q/(1.0-elem->ecc);
            H = (r-aval*(1.0 + elem->ecc*elem->ecc))/(elem->ecc*aval*(1.0 - elem->ecc*elem->ecc));
            K = (aval*ss/(ka*ka*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
            //qDebug() << QString("%1\t%2\t%3\n").arg(aval).arg(H).arg(K);
        }
    }












/*
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
  */
    return 0;//a.exec();
}
