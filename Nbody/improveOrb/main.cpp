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

QTextStream ostm(stdout);

void E2elem(double *E, orbElem *dElem, orbElem *elem);
void E2elem_psi(double *E, orbElem *dElem, orbElem *elem);
void E2elem_pqr(double *E, orbElem *dElem, orbElem *elem);
void detE00(inproveObject *impObj, observ &opos, orbElem *elem, orbElem *elemI);
void detE0(inproveObject *impObj, observ &opos, orbElem *elem, orbElem *elemI);
void detE1(inproveObject *impObj, observ &opos, orbElem *elem, orbElem *elemI);
void detE2(inproveObject *impObj, observ &opos, orbElem *elem, orbElem &dElem);
void detE3(inproveObject *impObj, observ &opos, orbElem *elem, orbElem &dElem);

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

    //QTextStream ostm(stdout);


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
    /*
    double *ai, *bi, *C;
    double *La, *Lb, *Lc;
    double *Ea, *Eb, *Ec;
    double *Wa, *Wb, *Wc;
    double uweA, uweB, uweC;
    double *Da, *Db, *Dc;
    double P[3], Q[3], R[3], N[3], Recl[3];
    double stateE[3], po, a1;
    double H, K, r, ss, aval, nn, dT;
    double psi[3];
    double tgw0, tgw1;
    double k1, k2, k3, k4, k5;
    double kEx[6], kEy[6], kEz[6];
    double PQ[3];
    elem_ekv = new orbElem;

    Ea = new double[6];
    Eb = new double[6];
    Ec = new double[6];
    Da = new double[6*6];
    Db = new double[6*6];
    Dc = new double[6*6];
    //Wc = new double[sz*2];
*/
    for(p=0; p<oNum; p++)
    {
        impObj = impObjList.at(p);
        elem = impObj->iniOrb.elem;
        elemI = impObj->impOrb.elem;



        ostm << QString("%1: %2\n\n").arg(p).arg(impObj->name);

        ostm << QString("\t\t: eJD\t\t\tM0\t\tq\t\tec\t\tinc\t\tw\t\tNode\t\n");
        ostm << QString("==============================================================================\n");
        ostm << QString("elem_ini\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(elem->eJD, 15, 'f', 8).arg(elem->M0, 10, 'f', 7).arg(elem->q, 10, 'f', 7).arg(elem->ecc, 10, 'f', 7).arg(elem->inc, 10, 'f', 7).arg(elem->w, 10, 'f', 7).arg(elem->Node, 10, 'f', 7);


        //detE00(impObj, opos, elem, elemI);
        //if(elem->ecc>0.1) detE0(impObj, opos, elem, elemI);
        //else detE1(impObj, opos, elem, elemI);
        if(elem->ecc<0.1) detE2(impObj, opos, elem, dElem);
        else detE3(impObj, opos, elem, dElem);

        elemI->ecc = elem->ecc + dElem.ecc;
        elemI->eJD = elem->eJD + dElem.eJD;
        elemI->inc = elem->inc + dElem.inc;
        elemI->M0 = elem->M0 + dElem.M0;
        elemI->Node = elem->Node + dElem.Node;
        elemI->q = elem->q + dElem.q;
        elemI->w = elem->w + dElem.w;

         //ostm << QString("elem_imp\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(elemI->eJD, 15, 'f', 8).arg(elemI->M0, 10, 'f', 7).arg(elemI->q, 10, 'f', 7).arg(elemI->ecc, 10, 'f', 7).arg(elemI->inc, 10, 'f', 7).arg(elemI->w, 10, 'f', 7).arg(elemI->Node, 10, 'f', 7);

        impObj->impOrb.set(&ocatI);

        ocatI.record->set_author("Berezhnoy");
        ocatI.record->set_name(impObj->name.toAscii().data());
        ocatI.record->set_number(p+1);
        ocatI.record->set_makeD(QDate().currentDate().toString("yyMMdd").toAscii().data());

        ocatI.AddRec();



    }

    ocatI.Save();


    return 0;//a.exec();
}

void E2elem_pqr(double *E, orbElem *dElem, orbElem *elem)
{
    double dp, dq, dr, a1;
    dElem->ecc = E[4];
    dp = E[0];
    dq = E[1];
    dr = E[2];
//3.3.10
    dElem->inc = dp*cos(elem->w) - dq*sin(elem->w);
    dElem->Node = (dp*sin(elem->w) + dq*cos(elem->w))/sin(elem->inc);
    dElem->w = dr - cos(elem->inc)*dElem->Node;

    dElem->M0 = E[5] - dr;
    a1 = E[3]/(elem->q/(1.0-elem->ecc));
    qDebug() << QString("da1: %1 = %2\n").arg(a1).arg(elem->q/(1.0-elem->ecc));
    dElem->q = a1*(1.0-elem->ecc);
}

void E2elem_psi(double *E, orbElem *dElem, orbElem *elem)
{
    double dp, dq, dr, a1;
    double psi[3];
    dElem->ecc = E[4];
//3.3.07
    psi[0] = E[0];
    psi[1] = E[1]*cos(EKV) + E[2]*sin(EKV);
    psi[2] = -E[1]*sin(EKV) + E[2]*cos(EKV);
//3.3.06
    dElem->w = (psi[0]*sin(elem->Node) - psi[1]*cos(elem->Node))/sin(elem->inc);
    dElem->inc = psi[0]*cos(elem->Node) + psi[1]*sin(elem->Node);
    dElem->Node = psi[2] - dElem->w*cos(elem->inc);


    dElem->M0 = E[5] - dr;
    a1 = E[3]/(elem->q/(1.0-elem->ecc));
    qDebug() << QString("da1: %1 = %2\n").arg(a1).arg(elem->q/(1.0-elem->ecc));
    dElem->q = a1*(1.0-elem->ecc);
}

void E2elem(double *E, orbElem *dElem, orbElem *elem)
{
    double a1;
    dElem->ecc = E[4];
    dElem->inc = E[2];
    dElem->Node = E[0];
    dElem->w = E[1];

    dElem->M0 = E[5] - E[1];
    a1 = E[3]/(elem->q/(1.0-elem->ecc));
    qDebug() << QString("da1: %1 = %2\n").arg(a1).arg(elem->q/(1.0-elem->ecc));
    dElem->q = a1*(1.0-elem->ecc);
}

void detE00(inproveObject *impObj, observ &opos, orbElem *elem, orbElem *elemI)
{
    ostm << QString("detE00: mid hi ecc\n");
    orbElem *elem_ekv, dElem;
    int i, j, sz;
    double *ai, *bi, *C;
    double *La, *Lb, *Lc;
    double *Ea, *Eb, *Ec;
    double *Wa, *Wb, *Wc;
    double uweA, uweB, uweC;
    double *Da, *Db, *Dc;
    double P[3], Q[3], R[3], N[3], Recl[3];
    double stateE[3], po, a1;
    double H, K, r, ss, aval, nn, dT;
    double psi[3];
    double tgw0, tgw1;
    double k1, k2, k3, k4, k5;
    double kEx[6], kEy[6], kEz[6];
    double PQ[3];
    improveState* impSt;

    elem_ekv = new orbElem;

    Ea = new double[6];
    Eb = new double[6];
    Ec = new double[6];
    Da = new double[6*6];
    Db = new double[6*6];
    Dc = new double[6*6];

    sz = impObj->impList.size();
    ai = new double[6*sz];
    bi = new double[6*sz];
    C = new double[6*sz*2];
    La = new double[sz];
    Lb = new double[sz];
    Lc = new double[2*sz];


    aval = elem->q/(1.0-elem->ecc);
    nn = ka*pow(aval, -1.5);

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
        //K = (ss/(aval*aval*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
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
        //r
        R[0] = sin(elem->inc)*sin(elem->Node);
        R[1] = -sin(elem->inc)*cos(elem_ekv->Node)*cos(EKV) - cos(elem->inc)*sin(EKV);
        R[2] = -sin(elem->inc)*cos(elem_ekv->Node)*sin(EKV) + cos(elem_ekv->inc)*cos(EKV);

        N[0] = cos(elem->Node);
        N[1] = sin(elem->Node)*cos(EKV);
        N[2] = sin(elem->Node)*sin(EKV);
        //ostm << QString("P: %1\t%2\t%3\n").arg(P[0]).arg(P[1]).arg(P[2]);
        //ostm << QString("Q: %1\t%2\t%3\n").arg(Q[0]).arg(Q[1]).arg(Q[2]);

        //Vmul3(&PQ[0], &P[0], &Q[0]);
        //ostm << QString("Smul: %1\tVmul: %2:%3:%4\n").arg(Smul3(&P[0], &Q[0])).arg(PQ[0]).arg(PQ[1]).arg(PQ[2]);

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
/
        R[0] = sin(elem_ekv->inc)*sin(elem_ekv->Node);
        R[1] = -sin(elem_ekv->inc)*cos(elem_ekv->Node);
        R[2] = cos(elem_ekv->inc);
        //proove??
        tgw0 = tan(elem_ekv->w);
        tgw1 = atan2((P[1]/cos(EKV) - P[0]*tan(elem_ekv->Node)), (Q[1]/cos(EKV) - Q[0]*tan(elem_ekv->Node)));
        //ostm << QString("%1 ?= %2\n").arg(elem_ekv->w).arg(tgw1);
        */
        //
        k1 = -sin(impSt->raC);///po;
        k2 = cos(impSt->raC);///po;
        k3 = -sin(impSt->decC)*cos(impSt->raC);///po;
        k4 = -sin(impSt->decC)*sin(impSt->raC);///po;
        k5 = cos(impSt->decC);///po;
/*
//  Node,inc,w
        if(elem->ecc<=0.25)
        {
            kEx[0] = -impSt->state[2]*sin(EKV) - impSt->state[1]*cos(EKV);
            kEx[1] = -impSt->state[0]/nn + R[1]*impSt->state[2] - R[2]*impSt->state[1];
            kEx[2] = N[1]*impSt->state[2] - N[2]*impSt->state[1];

            kEx[3] = impSt->state[0] - 1.5*dT*impSt->state[3];
            kEx[4] = H*impSt->state[0]+K*impSt->state[3];
            kEx[5] = impSt->state[3]/nn;


            kEy[0] = impSt->state[0]*cos(EKV);
            kEy[1] = -impSt->state[1]/nn + R[2]*impSt->state[0] - R[0]*impSt->state[2];
            kEy[2] = N[2]*impSt->state[0] - N[0]*impSt->state[2];

            kEy[3] = impSt->state[1] - 1.5*dT*impSt->state[4];
            kEy[4] = H*impSt->state[1]+K*impSt->state[4];
            kEy[5] = impSt->state[4]/nn;


            kEz[0] = impSt->state[0]*sin(EKV);
            kEz[1] = -impSt->state[2]/nn + R[0]*impSt->state[1] - R[1]*impSt->state[0];
            kEz[2] = N[0]*impSt->state[1] - N[1]*impSt->state[0];

            kEz[3] = impSt->state[2] - 1.5*dT*impSt->state[5];
            kEz[4] = H*impSt->state[2]+K*impSt->state[5];
            kEz[5] = impSt->state[5]/nn;
        }
        if(elem->ecc>0.25&&elem->ecc<0.85)
        {
            kEx[0] = -impSt->state[2]*sin(EKV) - impSt->state[1]*cos(EKV);
            kEx[1] = R[1]*impSt->state[2] - R[2]*impSt->state[1];
            kEx[2] = N[1]*impSt->state[2] - N[2]*impSt->state[1];

            kEx[3] = impSt->state[0] - 1.5*dT*impSt->state[3];
            kEx[4] = H*impSt->state[0]+K*impSt->state[3];
            kEx[5] = impSt->state[3]/nn;


            kEy[0] = impSt->state[0]*cos(EKV);
            kEy[1] = R[2]*impSt->state[0] - R[0]*impSt->state[2];
            kEy[2] = N[2]*impSt->state[0] - N[0]*impSt->state[2];

            kEy[3] = impSt->state[1] - 1.5*dT*impSt->state[4];
            kEy[4] = H*impSt->state[1]+K*impSt->state[4];
            kEy[5] = impSt->state[4]/nn;


            kEz[0] = impSt->state[0]*sin(EKV);
            kEz[1] = R[0]*impSt->state[1] - R[1]*impSt->state[0];
            kEz[2] = N[0]*impSt->state[1] - N[1]*impSt->state[0];

            kEz[3] = impSt->state[2] - 1.5*dT*impSt->state[5];
            kEz[4] = H*impSt->state[2]+K*impSt->state[5];
            kEz[5] = impSt->state[5]/nn;
        }

//  lowecc
        kEx[0] = 0.0;
        kEx[1] = impSt->state[2];
        kEx[2] = -impSt->state[3]/nn - impSt->state[1];
        kEx[3] = impSt->state[0] - 1.5*dT*impSt->state[3];
        kEx[4] = H*impSt->state[0]+K*impSt->state[3];
        kEx[5] = impSt->state[3]/nn;

        kEy[0] = -impSt->state[2];
        kEy[1] = 0.0;
        kEy[2] = -impSt->state[4]/nn+impSt->state[0];
        kEy[3] = impSt->state[1] - 1.5*dT*impSt->state[4];
        kEy[4] = H*impSt->state[1]+K*impSt->state[4];
        kEy[5] = impSt->state[4]/nn;

        kEz[0] = impSt->state[1];
        kEz[1] = -impSt->state[0];
        kEz[2] = -impSt->state[5]/nn;
        kEz[3] = impSt->state[2] - 1.5*dT*impSt->state[5];
        kEz[4] = H*impSt->state[2]+K*impSt->state[5];
        kEz[5] = impSt->state[5]/nn;
*/
//med hi ecc
//[0-5]:tab42
        kEx[0] = 0.0;
        kEx[1] = impSt->state[2];
        kEx[2] = -impSt->state[1];
        kEx[3] = impSt->state[0] - 1.5*dT*impSt->state[3];
        kEx[4] = H*impSt->state[0]+K*impSt->state[3];
        kEx[5] = impSt->state[3]/nn;

        kEy[0] = -impSt->state[2];
        kEy[1] = 0.0;
        kEy[2] = impSt->state[0];
        kEy[3] = impSt->state[1] - 1.5*dT*impSt->state[4];
        kEy[4] = H*impSt->state[1]+K*impSt->state[4];
        kEy[5] = impSt->state[4]/nn;

        kEz[0] = impSt->state[1];
        kEz[1] = - impSt->state[0];
        kEz[2] = 0.0;
        kEz[3] = impSt->state[2] - 1.5*dT*impSt->state[5];
        kEz[4] = H*impSt->state[2]+K*impSt->state[5];
        kEz[5] = impSt->state[5]/nn;

        for(j=0;j<6;j++)
        {
            ai[i*6+j] = k1*kEx[j] + k2*kEy[j];
            bi[i*6+j] = k3*kEx[j] + k4*kEy[j] + k5*kEz[j];
            C[i*6+j] = ai[i*6+j];
            C[6*sz+i*6+j] = bi[i*6+j];
            //ostm << QString("ai: %1\nbi: %2\n").arg(ai[i*6+j]).arg(bi[i*6+j]);
        }

        La[i] = po*(impSt->raO - impSt->raC)*cos(impSt->decC);
        Lb[i] = po*(impSt->decO - impSt->decC);
        Lc[i] = La[i];
        Lc[sz+i] = Lb[i];

        //ostm << QString("La: %1\nLb: %2\n").arg(La[i]).arg(Lb[i]);

    }
/*
//ai
    lsm(6, sz, Ea, ai, La, uweA, Da);

    ostm << QString("Ea: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Ea[0]).arg(Ea[1]).arg(Ea[2]).arg(Ea[3]).arg(Ea[4]).arg(Ea[5]);
    elemI = impObj->impOrb.elem;

    E2elem(Ea, &dElem, elem);

    ostm << QString("dElemA\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

//bi
    lsm(6, sz, Eb, bi, Lb, uweB, Db);

    ostm << QString("Eb: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Eb[0]).arg(Eb[1]).arg(Eb[2]).arg(Eb[3]).arg(Eb[4]).arg(Eb[5]);
    elemI = impObj->impOrb.elem;

    E2elem(Eb, &dElem, elem);

    ostm << QString("dElemB\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);
*/
//C
    for(int l=0; l<6; l++) Ec[l]=0;
    //for(int l=0; l<sz*2; l++) Wc[l]=1.0;
    lsm(6, sz*2, Ec, C, Lc, uweC, Dc);
    //slsm(6, sz*2, Ec, C, Lc);

    //ostm << QString("uweC: %1\n").arg(uweC);

    //ostm << QString("Ec: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Ec[0]).arg(Ec[1]).arg(Ec[2]).arg(Ec[3]).arg(Ec[4]).arg(Ec[5]);


    double ecSum = 0.0;
    for(int l=0; l<6; l++) ecSum += fabs(Ec[l]);
   ostm << QString("uweC: %1\tecSum:%2\n").arg(uweC).arg(ecSum);

    E2elem_psi(Ec, &dElem, elem);

    ostm << QString("dElemC\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

    elemI->ecc = elem->ecc - dElem.ecc;
    elemI->eJD = elem->eJD - dElem.eJD;
    elemI->inc = elem->inc - dElem.inc;
    elemI->M0 = elem->M0 - dElem.M0;
    elemI->Node = elem->Node - dElem.Node;
    elemI->q = elem->q - dElem.q;
    elemI->w = elem->w - dElem.w;
}

void detE0(inproveObject *impObj, observ &opos, orbElem *elem, orbElem *elemI)
{
    ostm << QString("detE0: arb ecc\n");
    orbElem *elem_ekv, dElem;
    int i, j, sz;
    double *ai, *bi, *C;
    double *La, *Lb, *Lc;
    double *Ea, *Eb, *Ec;
    double *Wa, *Wb, *Wc;
    double uweA, uweB, uweC;
    double *Da, *Db, *Dc;
    double P[3], Q[3], R[3], N[3], Recl[3];
    double stateE[3], po, a1;
    double H, K, r, ss, aval, nn, dT;
    double psi[3];
    double tgw0, tgw1;
    double k1, k2, k3, k4, k5;
    double kEx[6], kEy[6], kEz[6];
    double PQ[3];
    improveState* impSt;

    elem_ekv = new orbElem;

    Ea = new double[6];
    Eb = new double[6];
    Ec = new double[6];
    Da = new double[6*6];
    Db = new double[6*6];
    Dc = new double[6*6];

    sz = impObj->impList.size();
    ai = new double[6*sz];
    bi = new double[6*sz];
    C = new double[6*sz*2];
    La = new double[sz];
    Lb = new double[sz];
    Lc = new double[2*sz];


    aval = elem->q/(1.0-elem->ecc);
    nn = ka*pow(aval, -1.5);

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
        //K = (ss/(aval*aval*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
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
        //r
        R[0] = sin(elem->inc)*sin(elem->Node);
        R[1] = -sin(elem->inc)*cos(elem_ekv->Node)*cos(EKV) - cos(elem->inc)*sin(EKV);
        R[2] = -sin(elem->inc)*cos(elem_ekv->Node)*sin(EKV) + cos(elem_ekv->inc)*cos(EKV);

        N[0] = cos(elem->Node);
        N[1] = sin(elem->Node)*cos(EKV);
        N[2] = sin(elem->Node)*sin(EKV);
        //ostm << QString("P: %1\t%2\t%3\n").arg(P[0]).arg(P[1]).arg(P[2]);
        //ostm << QString("Q: %1\t%2\t%3\n").arg(Q[0]).arg(Q[1]).arg(Q[2]);

        //Vmul3(&PQ[0], &P[0], &Q[0]);
        //ostm << QString("Smul: %1\tVmul: %2:%3:%4\n").arg(Smul3(&P[0], &Q[0])).arg(PQ[0]).arg(PQ[1]).arg(PQ[2]);

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
/
        R[0] = sin(elem_ekv->inc)*sin(elem_ekv->Node);
        R[1] = -sin(elem_ekv->inc)*cos(elem_ekv->Node);
        R[2] = cos(elem_ekv->inc);
        //proove??
        tgw0 = tan(elem_ekv->w);
        tgw1 = atan2((P[1]/cos(EKV) - P[0]*tan(elem_ekv->Node)), (Q[1]/cos(EKV) - Q[0]*tan(elem_ekv->Node)));
        //ostm << QString("%1 ?= %2\n").arg(elem_ekv->w).arg(tgw1);
        */
        //
        k1 = -sin(impSt->raC);///po;
        k2 = cos(impSt->raC);///po;
        k3 = -sin(impSt->decC)*cos(impSt->raC);///po;
        k4 = -sin(impSt->decC)*sin(impSt->raC);///po;
        k5 = cos(impSt->decC);///po;
/*
//  Node,inc,w
        if(elem->ecc<=0.25)
        {
            kEx[0] = -impSt->state[2]*sin(EKV) - impSt->state[1]*cos(EKV);
            kEx[1] = -impSt->state[0]/nn + R[1]*impSt->state[2] - R[2]*impSt->state[1];
            kEx[2] = N[1]*impSt->state[2] - N[2]*impSt->state[1];

            kEx[3] = impSt->state[0] - 1.5*dT*impSt->state[3];
            kEx[4] = H*impSt->state[0]+K*impSt->state[3];
            kEx[5] = impSt->state[3]/nn;


            kEy[0] = impSt->state[0]*cos(EKV);
            kEy[1] = -impSt->state[1]/nn + R[2]*impSt->state[0] - R[0]*impSt->state[2];
            kEy[2] = N[2]*impSt->state[0] - N[0]*impSt->state[2];

            kEy[3] = impSt->state[1] - 1.5*dT*impSt->state[4];
            kEy[4] = H*impSt->state[1]+K*impSt->state[4];
            kEy[5] = impSt->state[4]/nn;


            kEz[0] = impSt->state[0]*sin(EKV);
            kEz[1] = -impSt->state[2]/nn + R[0]*impSt->state[1] - R[1]*impSt->state[0];
            kEz[2] = N[0]*impSt->state[1] - N[1]*impSt->state[0];

            kEz[3] = impSt->state[2] - 1.5*dT*impSt->state[5];
            kEz[4] = H*impSt->state[2]+K*impSt->state[5];
            kEz[5] = impSt->state[5]/nn;
        }
        if(elem->ecc>0.25&&elem->ecc<0.85)
        {
            kEx[0] = -impSt->state[2]*sin(EKV) - impSt->state[1]*cos(EKV);
            kEx[1] = R[1]*impSt->state[2] - R[2]*impSt->state[1];
            kEx[2] = N[1]*impSt->state[2] - N[2]*impSt->state[1];

            kEx[3] = impSt->state[0] - 1.5*dT*impSt->state[3];
            kEx[4] = H*impSt->state[0]+K*impSt->state[3];
            kEx[5] = impSt->state[3]/nn;


            kEy[0] = impSt->state[0]*cos(EKV);
            kEy[1] = R[2]*impSt->state[0] - R[0]*impSt->state[2];
            kEy[2] = N[2]*impSt->state[0] - N[0]*impSt->state[2];

            kEy[3] = impSt->state[1] - 1.5*dT*impSt->state[4];
            kEy[4] = H*impSt->state[1]+K*impSt->state[4];
            kEy[5] = impSt->state[4]/nn;


            kEz[0] = impSt->state[0]*sin(EKV);
            kEz[1] = R[0]*impSt->state[1] - R[1]*impSt->state[0];
            kEz[2] = N[0]*impSt->state[1] - N[1]*impSt->state[0];

            kEz[3] = impSt->state[2] - 1.5*dT*impSt->state[5];
            kEz[4] = H*impSt->state[2]+K*impSt->state[5];
            kEz[5] = impSt->state[5]/nn;
        }

//  lowecc
        kEx[0] = 0.0;
        kEx[1] = impSt->state[2];
        kEx[2] = -impSt->state[3]/nn - impSt->state[1];
        kEx[3] = impSt->state[0] - 1.5*dT*impSt->state[3];
        kEx[4] = H*impSt->state[0]+K*impSt->state[3];
        kEx[5] = impSt->state[3]/nn;

        kEy[0] = -impSt->state[2];
        kEy[1] = 0.0;
        kEy[2] = -impSt->state[4]/nn+impSt->state[0];
        kEy[3] = impSt->state[1] - 1.5*dT*impSt->state[4];
        kEy[4] = H*impSt->state[1]+K*impSt->state[4];
        kEy[5] = impSt->state[4]/nn;

        kEz[0] = impSt->state[1];
        kEz[1] = -impSt->state[0];
        kEz[2] = -impSt->state[5]/nn;
        kEz[3] = impSt->state[2] - 1.5*dT*impSt->state[5];
        kEz[4] = H*impSt->state[2]+K*impSt->state[5];
        kEz[5] = impSt->state[5]/nn;
*/
//arb ecc

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

        for(j=0;j<6;j++)
        {
            ai[i*6+j] = k1*kEx[j] + k2*kEy[j];
            bi[i*6+j] = k3*kEx[j] + k4*kEy[j] + k5*kEz[j];
            C[i*6+j] = ai[i*6+j];
            C[6*sz+i*6+j] = bi[i*6+j];
            //ostm << QString("ai: %1\nbi: %2\n").arg(ai[i*6+j]).arg(bi[i*6+j]);
        }

        La[i] = po*(impSt->raO - impSt->raC)*cos(impSt->decC);
        Lb[i] = po*(impSt->decO - impSt->decC);
        Lc[i] = La[i];
        Lc[sz+i] = Lb[i];

        //ostm << QString("La: %1\nLb: %2\n").arg(La[i]).arg(Lb[i]);

    }
/*
//ai
    lsm(6, sz, Ea, ai, La, uweA, Da);

    ostm << QString("Ea: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Ea[0]).arg(Ea[1]).arg(Ea[2]).arg(Ea[3]).arg(Ea[4]).arg(Ea[5]);
    elemI = impObj->impOrb.elem;

    E2elem_pqr(Ea, &dElem, elem);

    ostm << QString("dElemA\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

//bi
    lsm(6, sz, Eb, bi, Lb, uweB, Db);

    ostm << QString("Eb: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Eb[0]).arg(Eb[1]).arg(Eb[2]).arg(Eb[3]).arg(Eb[4]).arg(Eb[5]);
    elemI = impObj->impOrb.elem;

    E2elem_pqr(Eb, &dElem, elem);

    ostm << QString("dElemB\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);
*/
//C
    for(int l=0; l<6; l++) Ec[l]=0;
    //for(int l=0; l<sz*2; l++) Wc[l]=1.0;
    lsm(6, sz*2, Ec, C, Lc, uweC, Dc);
    //slsm(6, sz*2, Ec, C, Lc);

    //ostm << QString("uweC: %1\n").arg(uweC);

    //ostm << QString("Ec: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Ec[0]).arg(Ec[1]).arg(Ec[2]).arg(Ec[3]).arg(Ec[4]).arg(Ec[5]);


    double ecSum = 0.0;
    for(int l=0; l<6; l++) ecSum += fabs(Ec[l]);
   ostm << QString("uweC: %1\tecSum:%2\n").arg(uweC).arg(ecSum);

    E2elem_pqr(Ec, &dElem, elem);

    ostm << QString("dElemC\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

    elemI->ecc = elem->ecc - dElem.ecc;
    elemI->eJD = elem->eJD - dElem.eJD;
    elemI->inc = elem->inc - dElem.inc;
    elemI->M0 = elem->M0 - dElem.M0;
    elemI->Node = elem->Node - dElem.Node;
    elemI->q = elem->q - dElem.q;
    elemI->w = elem->w - dElem.w;
}

void detE1(inproveObject *impObj, observ &opos, orbElem *elem, orbElem *elemI)
{
    ostm << QString("detE1: low ecc\n");
    orbElem *elem_ekv, dElem;
    int i, j, sz;
    double *ai, *bi, *C;
    double *La, *Lb, *Lc;
    double *Ea, *Eb, *Ec;
    double *Wa, *Wb, *Wc;
    double uweA, uweB, uweC;
    double *Da, *Db, *Dc;
    double P[3], Q[3], R[3], N[3], Recl[3];
    double stateE[3], po, a1;
    double H, K, r, ss, aval, nn, dT;
    double psi[3];
    double tgw0, tgw1;
    double k1, k2, k3, k4, k5;
    double kEx[6], kEy[6], kEz[6];
    double PQ[3];
    improveState* impSt;

    elem_ekv = new orbElem;

    Ea = new double[6];
    Eb = new double[6];
    Ec = new double[6];
    Da = new double[6*6];
    Db = new double[6*6];
    Dc = new double[6*6];

    sz = impObj->impList.size();
    ai = new double[6*sz];
    bi = new double[6*sz];
    C = new double[6*sz*2];
    La = new double[sz];
    Lb = new double[sz];
    Lc = new double[2*sz];


    aval = elem->q/(1.0-elem->ecc);
    nn = ka*pow(aval, -1.5);

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
        //K = (ss/(aval*aval*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
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
        //r
        R[0] = sin(elem->inc)*sin(elem->Node);
        R[1] = -sin(elem->inc)*cos(elem_ekv->Node)*cos(EKV) - cos(elem->inc)*sin(EKV);
        R[2] = -sin(elem->inc)*cos(elem_ekv->Node)*sin(EKV) + cos(elem_ekv->inc)*cos(EKV);

        N[0] = cos(elem->Node);
        N[1] = sin(elem->Node)*cos(EKV);
        N[2] = sin(elem->Node)*sin(EKV);

        //
        k1 = -sin(impSt->raC);///po;
        k2 = cos(impSt->raC);///po;
        k3 = -sin(impSt->decC)*cos(impSt->raC);///po;
        k4 = -sin(impSt->decC)*sin(impSt->raC);///po;
        k5 = cos(impSt->decC);///po;

//  lowecc
        kEx[0] = 0.0;
        kEx[1] = impSt->state[2];
        kEx[2] = -impSt->state[3]/nn - impSt->state[1];
        kEx[3] = impSt->state[0] - 1.5*dT*impSt->state[3];
        kEx[4] = H*impSt->state[0]+K*impSt->state[3];
        kEx[5] = impSt->state[3]/nn;

        kEy[0] = -impSt->state[2];
        kEy[1] = 0.0;
        kEy[2] = -impSt->state[4]/nn+impSt->state[0];
        kEy[3] = impSt->state[1] - 1.5*dT*impSt->state[4];
        kEy[4] = H*impSt->state[1]+K*impSt->state[4];
        kEy[5] = impSt->state[4]/nn;

        kEz[0] = impSt->state[1];
        kEz[1] = -impSt->state[0];
        kEz[2] = -impSt->state[5]/nn;
        kEz[3] = impSt->state[2] - 1.5*dT*impSt->state[5];
        kEz[4] = H*impSt->state[2]+K*impSt->state[5];
        kEz[5] = impSt->state[5]/nn;

        for(j=0;j<6;j++)
        {
            ai[i*6+j] = k1*kEx[j] + k2*kEy[j];
            bi[i*6+j] = k3*kEx[j] + k4*kEy[j] + k5*kEz[j];
            C[i*6+j] = ai[i*6+j];
            C[6*sz+i*6+j] = bi[i*6+j];
            //ostm << QString("ai: %1\nbi: %2\n").arg(ai[i*6+j]).arg(bi[i*6+j]);
        }

        La[i] = po*(impSt->raO - impSt->raC)*cos(impSt->decC);
        Lb[i] = po*(impSt->decO - impSt->decC);
        Lc[i] = La[i];
        Lc[sz+i] = Lb[i];

        //ostm << QString("La: %1\nLb: %2\n").arg(La[i]).arg(Lb[i]);

    }
/*
//ai
    lsm(6, sz, Ea, ai, La, uweA, Da);

    //ostm << QString("Ea: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Ea[0]).arg(Ea[1]).arg(Ea[2]).arg(Ea[3]).arg(Ea[4]).arg(Ea[5]);
    elemI = impObj->impOrb.elem;

    E2elem_psi(Ea, &dElem, elem);

    ostm << QString("dElemA\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

//bi
    lsm(6, sz, Eb, bi, Lb, uweB, Db);

    //ostm << QString("Eb: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Eb[0]).arg(Eb[1]).arg(Eb[2]).arg(Eb[3]).arg(Eb[4]).arg(Eb[5]);
    elemI = impObj->impOrb.elem;

    E2elem_psi(Eb, &dElem, elem);

    ostm << QString("dElemB\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);
*/
//C
    for(int l=0; l<6; l++) Ec[l]=0;
    //for(int l=0; l<sz*2; l++) Wc[l]=1.0;
    lsm(6, sz*2, Ec, C, Lc, uweC, Dc);
    //slsm(6, sz*2, Ec, C, Lc);



    ostm << QString("Ec: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Ec[0]).arg(Ec[1]).arg(Ec[2]).arg(Ec[3]).arg(Ec[4]).arg(Ec[5]);

    double ecSum = 0.0;
    for(int l=0; l<6; l++) ecSum += fabs(Ec[l]);
   ostm << QString("uweC: %1\tecSum:%2\n").arg(uweC).arg(ecSum);
    //for(int l=0; l<6; l++) Ec[l]=0.0;

    E2elem_psi(Ec, &dElem, elem);

    ostm << QString("dElemC\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

    elemI->ecc = elem->ecc - dElem.ecc;
    elemI->eJD = elem->eJD - dElem.eJD;
    elemI->inc = elem->inc - dElem.inc;
    elemI->M0 = elem->M0 - dElem.M0;
    elemI->Node = elem->Node - dElem.Node;
    elemI->q = elem->q - dElem.q;
    elemI->w = elem->w - dElem.w;
}

void detE2(inproveObject *impObj, observ &opos, orbElem *elem, orbElem &dElem)
{
    ostm << QString("detE0: elem tab48 low ecc\n");
    orbElem *elem_ekv;
    int i, j, sz;
    double *ai, *bi, *C;
    double *La, *Lb, *Lc;
    double *Ea, *Eb, *Ec;
    double *Wa, *Wb, *Wc;
    double uweA, uweB, uweC;
    double *Da, *Db, *Dc;
    double P[3], Q[3], R[3], N[3], Recl[3];
    double stateE[3], po, a1;
    double H, K, r, ss, aval, nn, dT;
    double psi[3];
    double tgw0, tgw1;
    double k1, k2, k3, k4, k5;
    double kEx[6], kEy[6], kEz[6];
    double PQ[3];
    improveState* impSt;

    elem_ekv = new orbElem;

    Ea = new double[6];
    Eb = new double[6];
    Ec = new double[6];
    Da = new double[6*6];
    Db = new double[6*6];
    Dc = new double[6*6];

    sz = impObj->impList.size();
    ai = new double[6*sz];
    bi = new double[6*sz];
    C = new double[6*sz*2];
    La = new double[sz];
    Lb = new double[sz];
    Lc = new double[2*sz];


    aval = elem->q/(1.0-elem->ecc);
    nn = ka*pow(aval, -1.5);

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
        //K = (ss/(aval*aval*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
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
        //r
        R[0] = sin(elem->inc)*sin(elem->Node);
        R[1] = -sin(elem->inc)*cos(elem_ekv->Node)*cos(EKV) - cos(elem->inc)*sin(EKV);
        R[2] = -sin(elem->inc)*cos(elem_ekv->Node)*sin(EKV) + cos(elem_ekv->inc)*cos(EKV);

        N[0] = cos(elem->Node);
        N[1] = sin(elem->Node)*cos(EKV);
        N[2] = cos(elem->Node)*sin(EKV);
        //ostm << QString("P: %1\t%2\t%3\n").arg(P[0]).arg(P[1]).arg(P[2]);
        //ostm << QString("Q: %1\t%2\t%3\n").arg(Q[0]).arg(Q[1]).arg(Q[2]);

        //Vmul3(&PQ[0], &P[0], &Q[0]);
        //ostm << QString("Smul: %1\tVmul: %2:%3:%4\n").arg(Smul3(&P[0], &Q[0])).arg(PQ[0]).arg(PQ[1]).arg(PQ[2]);

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
/
        R[0] = sin(elem_ekv->inc)*sin(elem_ekv->Node);
        R[1] = -sin(elem_ekv->inc)*cos(elem_ekv->Node);
        R[2] = cos(elem_ekv->inc);
        //proove??
        tgw0 = tan(elem_ekv->w);
        tgw1 = atan2((P[1]/cos(EKV) - P[0]*tan(elem_ekv->Node)), (Q[1]/cos(EKV) - Q[0]*tan(elem_ekv->Node)));
        //ostm << QString("%1 ?= %2\n").arg(elem_ekv->w).arg(tgw1);
        */
        //
        k1 = -sin(impSt->raC);///po;
        k2 = cos(impSt->raC);///po;
        k3 = -sin(impSt->decC)*cos(impSt->raC);///po;
        k4 = -sin(impSt->decC)*sin(impSt->raC);///po;
        k5 = cos(impSt->decC);///po;

//  Node,inc,w - low ecc
//[0,2]:tab47
//[1]:tab48
//[3-5]:tab44
            kEx[0] = -impSt->state[2]*sin(EKV) - impSt->state[1]*cos(EKV);
            kEx[1] = -impSt->state[3]/nn + R[1]*impSt->state[2] - R[2]*impSt->state[1];
            kEx[2] = N[1]*impSt->state[2] - N[2]*impSt->state[1];

            kEx[3] = impSt->state[0] - 1.5*dT*impSt->state[3];
            kEx[4] = H*impSt->state[0]+K*impSt->state[3];
            kEx[5] = impSt->state[3]/nn;


            kEy[0] = impSt->state[0]*cos(EKV);
            kEy[1] = -impSt->state[4]/nn + R[2]*impSt->state[0] - R[0]*impSt->state[2];
            kEy[2] = N[2]*impSt->state[0] - N[0]*impSt->state[2];

            kEy[3] = impSt->state[1] - 1.5*dT*impSt->state[4];
            kEy[4] = H*impSt->state[1]+K*impSt->state[4];
            kEy[5] = impSt->state[4]/nn;


            kEz[0] = impSt->state[0]*sin(EKV);
            kEz[1] = -impSt->state[5]/nn + R[0]*impSt->state[1] - R[1]*impSt->state[0];
            kEz[2] = N[0]*impSt->state[1] - N[1]*impSt->state[0];

            kEz[3] = impSt->state[2] - 1.5*dT*impSt->state[5];
            kEz[4] = H*impSt->state[2]+K*impSt->state[5];
            kEz[5] = impSt->state[5]/nn;


        for(j=0;j<6;j++)
        {
            ai[i*6+j] = k1*kEx[j] + k2*kEy[j];
            bi[i*6+j] = k3*kEx[j] + k4*kEy[j] + k5*kEz[j];
            C[i*6+j] = ai[i*6+j];
            C[6*sz+i*6+j] = bi[i*6+j];
            //ostm << QString("ai: %1\nbi: %2\n").arg(ai[i*6+j]).arg(bi[i*6+j]);
        }

        La[i] = po*(impSt->raO - impSt->raC)*cos(impSt->decC);
        Lb[i] = po*(impSt->decO - impSt->decC);
        Lc[i] = La[i];
        Lc[sz+i] = Lb[i];

        //ostm << QString("La: %1\nLb: %2\n").arg(La[i]).arg(Lb[i]);

    }
//C
    for(int l=0; l<6; l++) Ec[l]=0;
    //for(int l=0; l<sz*2; l++) Wc[l]=1.0;
    lsm(6, sz*2, Ec, C, Lc, uweC, Dc);
    //slsm(6, sz*2, Ec, C, Lc);

    //ostm << QString("uweC: %1\n").arg(uweC);

    //ostm << QString("Ec: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Ec[0]).arg(Ec[1]).arg(Ec[2]).arg(Ec[3]).arg(Ec[4]).arg(Ec[5]);
    //ostm << QString("dEc: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Dc[0]).arg(Ec[6*1+1]).arg(Ec[6*2+2]).arg(Ec[6*3+3]).arg(Ec[6*4+4]).arg(Ec[6*5+5]);

    double ecSum = 0.0;
    for(int l=0; l<6; l++) ecSum += fabs(Ec[l]);
   ostm << QString("uweC: %1\tecSum:%2\n").arg(uweC).arg(ecSum);
    //for(int l=0; l<6; l++) Ec[l]=0.0;

    E2elem(Ec, &dElem, elem);

    ostm << QString("dElemC\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);
/*
    elemI->ecc = elem->ecc - dElem.ecc;
    elemI->eJD = elem->eJD - dElem.eJD;
    elemI->inc = elem->inc - dElem.inc;
    elemI->M0 = elem->M0 - dElem.M0;
    elemI->Node = elem->Node - dElem.Node;
    elemI->q = elem->q - dElem.q;
    elemI->w = elem->w - dElem.w;
    */
}

void detE3(inproveObject *impObj, observ &opos, orbElem *elem, orbElem &dElem)
{
    ostm << QString("detE0: elem tab47 arb ecc\n");
    orbElem *elem_ekv;
    int i, j, sz;
    double *ai, *bi, *C;
    double *La, *Lb, *Lc;
    double *Ea, *Eb, *Ec;
    double *Wa, *Wb, *Wc;
    double uweA, uweB, uweC;
    double *Da, *Db, *Dc;
    double P[3], Q[3], R[3], N[3], Recl[3];
    double stateE[3], po, a1;
    double H, K, r, ss, aval, nn, dT;
    double psi[3];
    double tgw0, tgw1;
    double k1, k2, k3, k4, k5;
    double kEx[6], kEy[6], kEz[6];
    double PQ[3];
    improveState* impSt;

    elem_ekv = new orbElem;

    Ea = new double[6];
    Eb = new double[6];
    Ec = new double[6];
    Da = new double[6*6];
    Db = new double[6*6];
    Dc = new double[6*6];

    sz = impObj->impList.size();
    ai = new double[6*sz];
    bi = new double[6*sz];
    C = new double[6*sz*2];
    La = new double[sz];
    Lb = new double[sz];
    Lc = new double[2*sz];


    aval = elem->q/(1.0-elem->ecc);
    nn = ka*pow(aval, -1.5);

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
        //K = (ss/(aval*aval*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
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
        //r
        R[0] = sin(elem->inc)*sin(elem->Node);
        R[1] = -sin(elem->inc)*cos(elem_ekv->Node)*cos(EKV) - cos(elem->inc)*sin(EKV);
        R[2] = -sin(elem->inc)*cos(elem_ekv->Node)*sin(EKV) + cos(elem_ekv->inc)*cos(EKV);

        N[0] = cos(elem->Node);
        N[1] = sin(elem->Node)*cos(EKV);
        N[2] = sin(elem->Node)*sin(EKV);
        //
        k1 = -sin(impSt->raC);///po;
        k2 = cos(impSt->raC);///po;
        k3 = -sin(impSt->decC)*cos(impSt->raC);///po;
        k4 = -sin(impSt->decC)*sin(impSt->raC);///po;
        k5 = cos(impSt->decC);///po;
//[0-2]:tab47
//[3-5]:tab44
            kEx[0] = -impSt->state[2]*sin(EKV) - impSt->state[1]*cos(EKV);
            kEx[1] = R[1]*impSt->state[2] - R[2]*impSt->state[1];
            kEx[2] = N[1]*impSt->state[2] - N[2]*impSt->state[1];

            kEx[3] = impSt->state[0] - 1.5*dT*impSt->state[3];
            kEx[4] = H*impSt->state[0]+K*impSt->state[3];
            kEx[5] = impSt->state[3]/nn;


            kEy[0] = impSt->state[0]*cos(EKV);
            kEy[1] = R[2]*impSt->state[0] - R[0]*impSt->state[2];
            kEy[2] = N[2]*impSt->state[0] - N[0]*impSt->state[2];

            kEy[3] = impSt->state[1] - 1.5*dT*impSt->state[4];
            kEy[4] = H*impSt->state[1]+K*impSt->state[4];
            kEy[5] = impSt->state[4]/nn;


            kEz[0] = impSt->state[0]*sin(EKV);
            kEz[1] = R[0]*impSt->state[1] - R[1]*impSt->state[0];
            kEz[2] = N[0]*impSt->state[1] - N[1]*impSt->state[0];

            kEz[3] = impSt->state[2] - 1.5*dT*impSt->state[5];
            kEz[4] = H*impSt->state[2]+K*impSt->state[5];
            kEz[5] = impSt->state[5]/nn;


        for(j=0;j<6;j++)
        {
            ai[i*6+j] = k1*kEx[j] + k2*kEy[j];
            bi[i*6+j] = k3*kEx[j] + k4*kEy[j] + k5*kEz[j];
            C[i*6+j] = ai[i*6+j];
            C[6*sz+i*6+j] = bi[i*6+j];
            //ostm << QString("ai: %1\nbi: %2\n").arg(ai[i*6+j]).arg(bi[i*6+j]);
        }

        La[i] = po*(impSt->raO - impSt->raC)*cos(impSt->decC);
        Lb[i] = po*(impSt->decO - impSt->decC);
        Lc[i] = La[i];
        Lc[sz+i] = Lb[i];

        //ostm << QString("La: %1\nLb: %2\n").arg(La[i]).arg(Lb[i]);

    }
//C
    for(int l=0; l<6; l++) Ec[l]=0;
    //for(int l=0; l<sz*2; l++) Wc[l]=1.0;
    lsm(6, sz*2, Ec, C, Lc, uweC, Dc);
    //slsm(6, sz*2, Ec, C, Lc);

    //ostm << QString("uweC: %1\n").arg(uweC);

    //ostm << QString("Ec: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Ec[0]).arg(Ec[1]).arg(Ec[2]).arg(Ec[3]).arg(Ec[4]).arg(Ec[5]);
    //ostm << QString("dEc: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Dc[0]).arg(Ec[6*1+1]).arg(Ec[6*2+2]).arg(Ec[6*3+3]).arg(Ec[6*4+4]).arg(Ec[6*5+5]);

    double ecSum = 0.0;
    for(int l=0; l<6; l++) ecSum += fabs(Ec[l]);
   ostm << QString("uweC: %1\tecSum:%2\n").arg(uweC).arg(ecSum);

    E2elem(Ec, &dElem, elem);

    ostm << QString("dElemC\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);
/*
    elemI->ecc = elem->ecc - dElem.ecc;
    elemI->eJD = elem->eJD - dElem.eJD;
    elemI->inc = elem->inc - dElem.inc;
    elemI->M0 = elem->M0 - dElem.M0;
    elemI->Node = elem->Node - dElem.Node;
    elemI->q = elem->q - dElem.q;
    elemI->w = elem->w - dElem.w;*/
}
