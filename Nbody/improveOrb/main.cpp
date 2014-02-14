#include <QCoreApplication>
#include "./../../libs/observ.h"
#include "./../../libs/orbcat.h"
#include "./../../libs/orbit.h"
#include <mb.h>

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

void E2elem(double *E, orbElem *dElem, orbElem *elem);


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    orbit orb;
    orbElem *elem, *elem_ekv, *elemI, dElem;
    eposrec erec;
    double X[3], V[3], t0;
    int sk, center;
    int i, sz, nObj, p, j;
    OrbCat ocat, ocatI;
    observ opos;




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

    opos.init(obsFile.toAscii().data(), jplFile.toAscii().data());
    opos.set_obs_parpam(GEOCENTR_NUM, center, sk, obsCode.toAscii().data());

    ocat.init(argv[1]);
    sz = ocat.nstr;

    QDir().remove("res.ocat");
    ocatI.init("res.ocat");

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
        impSt->raO = grad2rad(opers[2].toDouble());
        impSt->decO = grad2rad(opers[3].toDouble());
        impSt->raC = grad2rad(opers[4].toDouble());
        impSt->decC = grad2rad(opers[5].toDouble());
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
            //ostm << QString("nObj: %1\n").arg(objName);
            if(ocat.GetRecName(objName.toAscii().data())==-1) continue;
            impObj->iniOrb.get(&ocat);
            impObj->impList << impSt;
            impObjList << impObj;
        }
    }

    stFile.close();


    int oNum = impObjList.size();
    ostm << QString("improve objects: %1\n").arg(oNum);

    double *ai, *bi, *C;
    double *La, *Lb, *Lc;
    double *Ea, *Eb, *Ec;
    double *Wa, *Wb, *Wc;
    double uweA, uweB, uweC;
    double *Da, *Db, *Dc;
    double P[3], Q[3], R[3], Recl[3];
    double stateE[3], po, a1;
    double H, K, r, ss, aval, nn, dT;
    double psi[3];
    double tgw0, tgw1;
    double k1, k2, k3, k4, k5;
    double kEx[6], kEy[6], kEz[6];
    elem_ekv = new orbElem;

    Ea = new double[6];
    Eb = new double[6];
    Ec = new double[6];
    Da = new double[6*6];
    Db = new double[6*6];
    Dc = new double[6*6];
    //Wc = new double[sz*2];

    for(p=0; p<oNum; p++)
    {
        impObj = impObjList.at(p);
        elem = impObj->iniOrb.elem;

        aval = elem->q/(1.0-elem->ecc);
        nn = ka*pow(aval, -1.5);

        ostm << QString("%1: %2\n\n").arg(p).arg(impObj->name);

        ostm << QString("\t\t: eJD\t\t\tM0\t\tq\t\tec\t\tinc\t\tw\t\tNode\t\n");
        ostm << QString("==============================================================================\n");
        ostm << QString("elem_ini\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(elem->eJD, 15, 'f', 8).arg(elem->M0, 10, 'f', 7).arg(elem->q, 10, 'f', 7).arg(elem->ecc, 10, 'f', 7).arg(elem->inc, 10, 'f', 7).arg(elem->w, 10, 'f', 7).arg(elem->Node, 10, 'f', 7);

        sz = impObj->impList.size();
        ai = new double[6*sz];
        bi = new double[6*sz];
        C = new double[6*sz*2];
        La = new double[sz];
        Lb = new double[sz];
        Lc = new double[2*sz];
        for(i=0; i<sz; i++)
        {
            impSt = impObj->impList.at(i);
            opos.det_observ_tdb(impSt->jdTDB);
            stateE[0] = impSt->state[0] - opos.state[0];
            stateE[1] = impSt->state[1] - opos.state[1];
            stateE[2] = impSt->state[2] - opos.state[2];
            stateE[3] = impSt->state[3] - opos.state[3];
            stateE[4] = impSt->state[4] - opos.state[4];
            stateE[5] = impSt->state[5] - opos.state[5];

            r = sqrt(impSt->state[0]*impSt->state[0] + impSt->state[1]*impSt->state[1] + impSt->state[2]*impSt->state[2]);
            ss = impSt->state[0]*impSt->state[3] + impSt->state[1]*impSt->state[4] + impSt->state[2]*impSt->state[5];
            po = sqrt(stateE[0]*stateE[0] + stateE[1]*stateE[1] + stateE[2]*stateE[2]);
            H = (r-aval*(1.0 + elem->ecc*elem->ecc))/(elem->ecc*aval*(1.0 - elem->ecc*elem->ecc));
            K = (aval*ss/(ka*ka*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
            dT = impSt->jdTDB - TDT2TDB(elem->eJD);
            //qDebug() << QString("%1\t%2\t%3\n").arg(aval).arg(H).arg(K);
            //P??

            P[0] = cos(elem->w)*cos(elem->Node) - sin(elem->w)*sin(elem->Node)*cos(elem->inc);
            P[1] = (cos(elem->w)*sin(elem->Node) + sin(elem->w)*cos(elem->Node)*cos(elem->inc))*cos(EKV) - sin(elem->w)*sin(elem->inc)*sin(EKV);
            P[2] = (cos(elem->w)*sin(elem->Node) + sin(elem->w)*cos(elem->Node)*cos(elem->inc))*sin(EKV) + sin(elem->w)*sin(elem->inc)*cos(EKV);
            //Q??
            Q[0] = -sin(elem->w)*cos(elem->Node) - cos(elem->w)*sin(elem->Node)*cos(elem->inc);
            Q[1] = (-sin(elem->w)*sin(elem->Node) + cos(elem->Node)*cos(elem->inc))*cos(EKV) - cos(elem->w)*sin(elem->inc)*sin(EKV);
            Q[2] = (-sin(elem->w)*sin(elem->Node) + cos(elem->w)*cos(elem->Node)*cos(elem->inc))*sin(EKV) + cos(elem->w)*sin(elem->inc)*cos(EKV);

            //P
            /*
            Ecl2Ekv(&elem_ekv->inc, &elem_ekv->Node, &elem_ekv->w, elem->inc, elem->Node, elem->w);
            P[0] = cos(elem_ekv->w)*cos(elem_ekv->Node) - sin(elem_ekv->w)*sin(elem_ekv->Node)*cos(elem_ekv->inc);
            P[1] = cos(elem_ekv->w)*sin(elem_ekv->Node) + sin(elem_ekv->w)*cos(elem_ekv->Node)*cos(elem_ekv->inc);
            P[2] = sin(elem_ekv->w)*sin(elem_ekv->inc);
            //Q??
            Q[0] = -sin(elem_ekv->w)*cos(elem_ekv->Node) - cos(elem_ekv->w)*sin(elem_ekv->Node)*cos(elem_ekv->inc);
            Q[1] = -sin(elem_ekv->w)*sin(elem_ekv->Node) + cos(elem_ekv->Node)*cos(elem_ekv->inc);
            Q[2] = cos(elem_ekv->w)*sin(elem_ekv->inc);
            //R
*/
            R[0] = sin(elem_ekv->inc)*sin(elem_ekv->Node);
            R[1] = -sin(elem_ekv->inc)*cos(elem_ekv->Node);
            R[2] = cos(elem_ekv->inc);
            //proove??
            tgw0 = tan(elem_ekv->w);
            tgw1 = atan2((P[1]/cos(EKV) - P[0]*tan(elem_ekv->Node)), (Q[1]/cos(EKV) - Q[0]*tan(elem_ekv->Node)));
            //ostm << QString("%1 ?= %2\n").arg(elem_ekv->w).arg(tgw1);
            //
            k1 = -sin(impSt->raC)/po;
            k2 = cos(impSt->raC)/po;
            k3 = -sin(impSt->decC)*cos(impSt->raC)/po;
            k4 = -sin(impSt->decC)*sin(impSt->raC)/po;
            k5 = cos(impSt->decC)/po;

            kEx[0] = P[1]*impSt->state[2] - P[2]*impSt->state[1];
            kEx[1] = Q[1]*impSt->state[2] - Q[2]*impSt->state[1];
            kEx[2] = -impSt->state[3]/nn + R[1]*impSt->state[2] - R[2]*impSt->state[1];
            kEx[3] = impSt->state[0] - 1.5*dT*impSt->state[3];
            kEx[4] = H*impSt->state[0]+K*impSt->state[3];
            kEx[5] = impSt->state[3]/nn;

            kEy[0] = P[2]*impSt->state[0] - P[0]*impSt->state[2];
            kEy[1] = Q[2]*impSt->state[0] - Q[0]*impSt->state[2];
            kEy[2] = -impSt->state[4]/nn + R[2]*impSt->state[0] - R[0]*impSt->state[2];
            kEy[3] = impSt->state[1] - 1.5*dT*impSt->state[4];
            kEy[4] = H*impSt->state[1]+K*impSt->state[4];
            kEy[5] = impSt->state[4]/nn;

            kEz[0] = P[0]*impSt->state[1] - P[1]*impSt->state[0];
            kEz[1] = Q[0]*impSt->state[1] - Q[1]*impSt->state[0];
            kEz[2] = -impSt->state[5]/nn + R[0]*impSt->state[1] - R[1]*impSt->state[0];
            kEz[3] = impSt->state[2] - 1.5*dT*impSt->state[5];
            kEz[4] = H*impSt->state[2]+K*impSt->state[5];
            kEz[5] = impSt->state[5]/nn;
            //
            for(j=0;j<6;j++)
            {
                ai[i*6+j] = k1*kEx[j] + k2*kEy[j];
                bi[i*6+j] = k3*kEx[j] + k4*kEy[j] + k5*kEz[j];
                C[i*6+j] = ai[i*6+j];
                C[6*sz+i*6+j] = bi[i*6+j];
            }

            La[i] = (impSt->raO - impSt->raC)*cos(impSt->decC);
            Lb[i] = impSt->decO - impSt->decC;
            Lc[i] = La[i];
            Lc[sz+i] = Lb[i];


        }
/*
//ai
        lsm(6, sz, Ea, ai, La, uweA, Da);

        ostm << QString("Ea: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Ea[0]).arg(Ea[1]).arg(Ea[2]).arg(Ea[3]).arg(Ea[4]).arg(Ea[5]);
        elemI = impObj->impOrb.elem;

        E2elem(Ea, &dElem, elem);

        //ostm << QString("dElemA\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

//bi
        lsm(6, sz, Eb, bi, Lb, uweB, Db);

        ostm << QString("Eb: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Eb[0]).arg(Eb[1]).arg(Eb[2]).arg(Eb[3]).arg(Eb[4]).arg(Eb[5]);
        elemI = impObj->impOrb.elem;

        E2elem(Eb, &dElem, elem);

        //ostm << QString("dElemB\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);
*/
//C
        for(int l=0; l<6; l++) Ec[l]=0;
        //for(int l=0; l<sz*2; l++) Wc[l]=1.0;
        //slsm(6, sz*2, Ec, C, Lc, uweC, Dc);
        slsm(6, sz*2, Ec, C, Lc);

//        ostm << QString("uweC: %1\n").arg(uweC);

        //ostm << QString("Ec: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Ec[0]).arg(Ec[1]).arg(Ec[2]).arg(Ec[3]).arg(Ec[4]).arg(Ec[5]);
        elemI = impObj->impOrb.elem;

        E2elem(Ec, &dElem, elem);

        ostm << QString("dElemC\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

        elemI->ecc = elem->ecc + dElem.ecc;
        elemI->eJD = elem->eJD + dElem.eJD;
        elemI->inc = elem->inc + dElem.inc;
        elemI->M0 = elem->M0 + dElem.M0;
        elemI->Node = elem->Node + dElem.Node;
        elemI->q = elem->q + dElem.q;
        elemI->w = elem->w + dElem.w;


        impObj->impOrb.set(&ocatI);

        ocatI.record->set_author("Berezhnoy");
        ocatI.record->set_name(impObj->name.toAscii().data());
        ocatI.record->set_number(p+1);
        ocatI.record->set_makeD(QDate().currentDate().toString("yyMMdd").toAscii().data());

        ocatI.AddRec();



    }

    ocatI.Save();



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

void E2elem(double *E, orbElem *dElem, orbElem *elem)
{
    double psi[3], a1;
    dElem->ecc = E[4];
    psi[0] = E[0];
    psi[1] = E[1];
    psi[2] = E[2];
//3.3.10
    dElem->inc = psi[0]*cos(elem->w) - psi[1]*sin(elem->w);
    dElem->Node = (psi[0]*sin(elem->w) + psi[1]*cos(elem->w))/sin(elem->inc);
    dElem->w = psi[2] - cos(elem->inc)*dElem->Node;

    dElem->M0 = E[5] - psi[2];
    a1 = E[3]/(elem->q/(1.0-elem->ecc));
    qDebug() << QString("da1: %1 = %2\n").arg(a1).arg(elem->q/(1.0-elem->ecc));
    dElem->q = a1*(1.0-elem->ecc);
}
