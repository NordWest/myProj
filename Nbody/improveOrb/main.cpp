#include <QCoreApplication>
#include "./../../libs/observ.h"
#include "./../../libs/orbcat.h"
#include "./../../libs/orbit.h"
#include <mb.h>

#include <SpiceUsr.h>
#include <calceph.h>

#include <iostream>

struct improveState
{
    double jdTDB;
    double raO, decO;
    double raC, decC;
    double state[6];
    QString obsCode;
    double ocRaCosD()
    {
        return((raO-raC)*cos(decO));
    }
    double ocDec()
    {
        return(decO-decC);
    }

};

struct inproveObject
{
    QString name;
    QList <improveState*> impList;
    orbit iniOrb;
    orbit impOrb;
};

QTextStream ostm(stdout);
void detPQR(double *P, double *Q, double *R, orbElem *elem);
void detN(double *N, orbElem *elem);
void E2elem_arb(double *E, orbElem *dElem, orbElem *elem);
void E2elem_low(double *E, orbElem *dElem, orbElem *elem);
void E2elem_psi(double *E, orbElem *dElem, orbElem *elem);
void E2elem_pqr(double *E, orbElem *dElem, orbElem *elem);
void detE00(inproveObject *impObj, observ &opos, orbElem *elem, orbElem &dElem);
void detE0(inproveObject *impObj, observ &opos, orbElem *elem, orbElem &dElem);
void detE1(inproveObject *impObj, observ &opos, orbElem *elem, orbElem &dElem);
void detE2(inproveObject *impObj, observ &opos, orbElem *elem, orbElem &dElem);
void detE3(inproveObject *impObj, observ &opos, orbElem *elem, orbElem &dElem);
void detKE2(double *kEx, double *kEy, double *kEz, double *state, double H, double K, double nn, double dT, orbElem *elem);
void get2Ddisp(double &z0, double &z1);
void E2elem_low1(double *E, orbElem *dElem, orbElem *elem);
void detEB(inproveObject *impObj, observ &opos, orbElem *elem, orbElem &dElem);

int main(int argc, char *argv[])    //improveOrb ocat imp
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    orbit orb;
    orbElem *elem, *elem_ekv, *elemI, dElem, elemC, dElemC;
    eposrec erec;
    double X[3], V[3], t0;
    int sk, center;
    int i, sz, nObj, p, j;
    OrbCat ocat, ocatI;
    observ opos;
    double ocRaMin, ocRaMax, ocDeMin, ocDeMax, ocRa, ocDe;
    double histStepRa, histStepDe;
    double z0, z1, disp;

    SpiceDouble             state [6];
    SpiceDouble             lt;
    SpiceChar             * corr;
    SpiceChar             * ref;
    ref = new SpiceChar[256];
    corr = new SpiceChar[256];


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

        int histSz = sett->value("improveObj/histSz", 7).toInt();
        disp = sett->value("improveObj/disp", 0).toInt();

        sk = sett->value("general/sk", 0).toInt();
        center = sett->value("general/center", 0).toInt();

        double time0 = sett->value("time/time0", 2455201.0).toDouble();

        int bigType = sett->value("general/bigType", 0).toInt();

        //CALCEPH
        QString ephFile = sett->value("CALCEPH/ephFile", "./../../data/cats/binp1940_2020.405").toString();

        //SPICE
        QString bspName = sett->value("SPICE/bspName", "./de421.bsp").toString();
        QString leapName = sett->value("SPICE/leapName", "./naif0010.tls").toString();
        sprintf(ref,"%s", sett->value("SPICE/ref", "J2000").toString().toAscii().data());
        sprintf(corr,"%s", sett->value("SPICE/corr", "LT").toString().toAscii().data());

//////////////////////////////

    if(bigType==1)
    {
        opos.init(obsFile.toAscii().data(), ephFile.toAscii().data(), 1);
        opos.set_obs_parpam("Earth", 1, 0, obsCode.toAscii().data());
    }
    else
    {
        opos.init(obsFile.toAscii().data(), jplFile.toAscii().data());
        opos.set_obs_parpam(GEOCENTR_NUM, 1, 0, obsCode.toAscii().data());
    }

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

    double psi[3], dr[3], dv[3];
    psi[0] = 0.0;
    psi[1] = 1.0e-5;
    psi[2] = 1.0e-8;


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
        opos.det_observ_tdb(impSt->jdTDB);

        impSt->state[0] = opers[6].toDouble();// + opos.state[0];
        impSt->state[1] = opers[7].toDouble();// + opos.state[1];
        impSt->state[2] = opers[8].toDouble();// + opos.state[2];
        impSt->state[3] = opers[9].toDouble();// + opos.state[3];
        impSt->state[4] = opers[10].toDouble();// + opos.state[4];
        impSt->state[5] = opers[11].toDouble();// + opos.state[5];
/*
        dr[0] = psi[1]*impSt->state[2] - psi[2]*impSt->state[1];
        dr[1] = psi[2]*impSt->state[0] - psi[0]*impSt->state[2];
        dr[2] = psi[0]*impSt->state[1] - psi[1]*impSt->state[0];

        ostm << QString("state:\t%1\t%2\t%3\n").arg(impSt->state[0]).arg(impSt->state[1]).arg(impSt->state[2]);
        qDebug() << QString("state:\t%1\t%2\t%3\n").arg(dr[0]).arg(dr[1]).arg(dr[2]);
        impSt->state[0] += dr[0];
        impSt->state[1] += dr[1];
        impSt->state[2] += dr[2];
*/
        //dr[0] = (impSt->state[0] - 1.5*())
        /*
        impSt->state[0] = opers[6].toDouble();// + opos.state[0];
        impSt->state[1] = opers[7].toDouble();// + opos.state[1];
        impSt->state[2] = opers[8].toDouble();// + opos.state[2];
        impSt->state[3] = opers[9].toDouble();// + opos.state[3];
        impSt->state[4] = opers[10].toDouble();// + opos.state[4];
        impSt->state[5] = opers[11].toDouble();// + opos.state[5];
        */
        impSt->obsCode = opers.at(12).simplified();

        //ostm << QString("state: %1\t%2\t%3\t%4\t%5\t%6\n").arg(impSt->state[0]).arg(impSt->state[1]).arg(impSt->state[2]).arg(impSt->state[3]).arg(impSt->state[4]).arg(impSt->state[5]);

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


    int *histRa = new int[histSz];
    int *histDe = new int[histSz];
    for(i=0;i<histSz;i++)
    {
        histRa[i] = 0;
        histDe[i] = 0;
    }
    int pos;

    QFile histFile("hist.txt");
    histFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream histStm(&histFile);

    QFile dElemFile("delem.txt");
    dElemFile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream deStm(&dElemFile);
    double stateC[6], stateE[6], dstate[6];

    for(p=0; p<oNum; p++)
    {
        impObj = impObjList.at(p);
        sz = impObj->impList.size();
/*
        for(i=0;i<sz;i++)
        {
            impSt = impObj->impList.at(i);

            get2Ddisp(z0, z1);
            impSt->decO += mas2rad(z1*disp);
            impSt->raO = impSt->raO + mas2rad(z0*disp)/cos(impSt->decO);
        }
*/
        ocRaMin = ocDeMin = 1000000;
        ocRaMax = ocDeMax = -1000000;
        for(i=0;i<sz;i++)
        {
            impSt = impObj->impList.at(i);
            //ocRa = fabs(rad2mas(impSt->ocRaCosD()));
            //ocDe = fabs(rad2mas(impSt->ocDec()));
            ocRa = rad2mas(impSt->ocRaCosD());
            ocDe = rad2mas(impSt->ocDec());
            if(ocRa<ocRaMin) ocRaMin = ocRa;
            if(ocRa>ocRaMax) ocRaMax = ocRa;
            if(ocDe<ocDeMin) ocDeMin = ocDe;
            if(ocDe>ocDeMax) ocDeMax = ocDe;
        }
        ocRaMin = floor(ocRaMin);
        ocRaMax = floor(ocRaMax)+1;
        ocDeMin = floor(ocDeMin);
        ocDeMax = floor(ocDeMax)+1;

        ostm << QString("%1: oc_ra:%2:%3\toc_de: %4-%5\n").arg(impObj->name).arg(ocRaMin).arg(ocRaMax).arg(ocDeMin).arg(ocDeMax);
        histStepRa = (ocRaMax-ocRaMin)/histSz;
        histStepDe = (ocDeMax-ocDeMin)/histSz;
        for(i=0;i<sz;i++)
        {
            impSt = impObj->impList.at(i);
            //ocRa = fabs(rad2mas(impSt->ocRaCosD()));
            //ocDe = fabs(rad2mas(impSt->ocDec()));
            ocRa = rad2mas(impSt->ocRaCosD());
            ocDe = rad2mas(impSt->ocDec());
            pos = floor((ocRa-ocRaMin)/histStepRa);
            histRa[pos]++;
            pos = floor((ocDe-ocDeMin)/histStepDe);
            histDe[pos]++;
        }
        for(i=0;i<histSz;i++)
        {
            histStm << QString("%1|%2|%3|%4|%5|%6\n").arg(impObj->name, 16).arg(i).arg(i*histStepRa).arg(histRa[i]).arg(i*histStepDe).arg(histDe[i]);
        }
    }
    histFile.close();

    opos.set_obs_parpam(GEOCENTR_NUM, center, sk, obsCode.toAscii().data());

    double *R0, *V0;
    R0 = new double[3];
    V0 = new double[3];
    double ti;

    double *ai, *bi, *C;
    double *La, *Lb, *Lc;
    double *Ea, *Eb, *Ec;
    double *Wa, *Wb, *Wc;
    double uweA, uweB, uweC;
    double *Da, *Db, *Dc;
    //double *Ci, *Li, *Ei;
    double kECx[6], kECy[6], kECz[6];
    double kEx[6], kEy[6], kEz[6];
    double H, K, nn, dT, aval, pval, r, ss, po;
    double k1, k2, k3, k4, k5;

    Ea = new double[6];
    Eb = new double[6];
    Ec = new double[6];
    Da = new double[6*6];
    Db = new double[6*6];
    Dc = new double[6*6];


    for(p=0; p<oNum; p++)
    {
        impObj = impObjList.at(p);
        elem = impObj->iniOrb.elem;
        elemI = impObj->impOrb.elem;

        ostm << QString("%1: %2\n\n").arg(p).arg(impObj->name);
        ostm << QString("\t\t: eJD\t\t\tM0\t\tq\t\tec\t\tinc\t\tw\t\tNode\t\n");
        ostm << QString("==============================================================================\n");
        ostm << QString("elem_ini\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(elem->eJD, 15, 'f', 8).arg(rad2grad(elem->M0), 10, 'f', 7).arg(elem->q, 10, 'f', 7).arg(elem->ecc, 10, 'f', 7).arg(rad2grad(elem->inc), 10, 'f', 7).arg(rad2grad(elem->w), 10, 'f', 7).arg(rad2grad(elem->Node), 10, 'f', 7);

/*
        t0 = impObj->iniOrb.elem->eJD;
        i=0;

        R0[0] = impObj->impList.at(i)->state[0];
        R0[1] = impObj->impList.at(i)->state[1];
        R0[2] = impObj->impList.at(i)->state[2];
        V0[0] = impObj->impList.at(i)->state[3];
        V0[1] = impObj->impList.at(i)->state[4];
        V0[2] = impObj->impList.at(i)->state[5];
        findOrb(&elemC, R0, V0, t0);

        //ostm << QString("\t\t: eJD\t\t\tM0\t\tq\t\tec\t\tinc\t\tw\t\tNode\t\n");
        //ostm << QString("==============================================================================\n");
        ostm << QString("elem_C\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(elemC.eJD, 15, 'f', 8).arg(rad2grad(elemC.M0), 10, 'f', 7).arg(elemC.q, 10, 'f', 7).arg(elemC.ecc, 10, 'f', 7).arg(rad2grad(elemC.inc), 10, 'f', 7).arg(rad2grad(elemC.w), 10, 'f', 7).arg(rad2grad(elemC.Node), 10, 'f', 7);
/*
        dElemC.eJD = elem->eJD - elemC.eJD;

        dElemC.M0 = elem->M0 - elemC.M0;
        dElemC.q = elem->q - elemC.q;
        dElemC.ecc = elem->ecc - elemC.ecc;

        dElemC.Node = elem->Node - elemC.Node;
        dElemC.inc = elem->inc - elemC.inc;
        dElemC.w = elem->w - elemC.w;

        ostm << QString("delem_C\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(elemC.eJD-elem->eJD, 15, 'f', 8).arg(elemC.M0-elem->M0, 10, 'f', 7).arg(elemC.q-elem->q, 10, 'f', 7).arg(elemC.ecc-elem->ecc, 10, 'f', 7).arg(elemC.inc-elem->inc, 10, 'f', 7).arg(elemC.w-elem->w, 10, 'f', 7).arg(elemC.Node-elem->Node, 10, 'f', 7);


        sz = impObj->impList.size();

        aval = elem->a();
        nn = ka/pow(aval, 1.5);
        pval = aval*(1.0 - elem->ecc*elem->ecc);

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

        for(i=0;i<sz;i++)
        {
            impSt = impObj->impList.at(i);
            ti = TDB2TDT(impSt->jdTDB);

            detRecEkv(elem, &impSt->state[0], &impSt->state[1], &impSt->state[2], TDB2TDT(impSt->jdTDB));
            detRecEkvVel(elem, &impSt->state[3], &impSt->state[4], &impSt->state[5], ti);

            detRecEkv(&elemC, &stateC[0], &stateC[1], &stateC[2], TDB2TDT(impSt->jdTDB));
            detRecEkvVel(&elemC, &stateC[3], &stateC[4], &stateC[5], ti);

            for(j=0;j<6;j++) dstate[j] = impSt->state[j]-stateC[j];

            //ostm << QString("state: %1\t%2\t%3\t%4\t%5\t%6\n").arg(impSt->state[0], 10, 'e').arg(impSt->state[1], 10, 'e').arg(impSt->state[2], 10, 'e').arg(impSt->state[3], 10, 'e').arg(impSt->state[4], 10, 'e').arg(impSt->state[5], 10, 'e');
            //ostm << QString("stateC: %1\t%2\t%3\t%4\t%5\t%6\n").arg(stateC[0], 10, 'e').arg(stateC[1], 10, 'e').arg(stateC[2], 10, 'e').arg(stateC[3], 10, 'e').arg(stateC[4], 10, 'e').arg(stateC[5], 10, 'e');

            //ostm << QString("dst: %1\t%2\t%3\t%4\t%5\t%6\n").arg(dstate[0], 10, 'e').arg(dstate[1], 10, 'e').arg(dstate[2], 10, 'e').arg(dstate[3], 10, 'e').arg(dstate[4], 10, 'e').arg(dstate[5], 10, 'e');

            kECx[0] = (impSt->state[0]-stateC[0])/dElemC.Node;
            kECx[1] = (impSt->state[0]-stateC[0])/dElemC.w;
            kECx[2] = (impSt->state[0]-stateC[0])/dElemC.inc;
            kECx[3] = (impSt->state[0]-stateC[0])/dElemC.a()*elem->a();
            kECx[4] = (impSt->state[0]-stateC[0])/dElemC.ecc;
            kECx[5] = (impSt->state[0]-stateC[0])/dElemC.M0;

            kECy[0] = (impSt->state[1]-stateC[1])/dElemC.Node;
            kECy[1] = (impSt->state[1]-stateC[1])/dElemC.w;
            kECy[2] = (impSt->state[1]-stateC[1])/dElemC.inc;
            kECy[3] = (impSt->state[1]-stateC[1])/dElemC.a()*elem->a();
            kECy[4] = (impSt->state[1]-stateC[1])/dElemC.ecc;
            kECy[5] = (impSt->state[1]-stateC[1])/dElemC.M0;

            kECz[0] = (impSt->state[2]-stateC[2])/dElemC.Node;
            kECz[1] = (impSt->state[2]-stateC[2])/dElemC.w;
            kECz[2] = (impSt->state[2]-stateC[2])/dElemC.inc;
            kECz[3] = (impSt->state[2]-stateC[2])/dElemC.a()*elem->a();
            kECz[4] = (impSt->state[2]-stateC[2])/dElemC.ecc;
            kECz[5] = (impSt->state[2]-stateC[2])/dElemC.M0;


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

            //H = (r + pval - 2.0*aval)/(elem->ecc*pval);
            //K = ((r+pval)/pval)*(ss/(aval*aval*elem->ecc*nn))/nn;
            H = (r-aval*(1.0 + elem->ecc*elem->ecc))/(elem->ecc*aval*(1.0 - elem->ecc*elem->ecc));
            //K = (aval*ss/(ka*ka*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
            //H = (1.0 + r/pval - 2.0/(1.0-elem->ecc*elem->ecc))/(elem->ecc);
            //K = pow(ka, -2.0)*aval*ss*(1.0+r/pval)/elem->ecc;

            K = (ss/(aval*aval*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
            dT = ti - t0;
            detKE2(kEx, kEy, kEz, impSt->state, H, K, nn, dT, elem);

            k1 = -sin(impSt->raC)/po;
            k2 = cos(impSt->raC)/po;
            k3 = -sin(impSt->decC)*cos(impSt->raC)/po;
            k4 = -sin(impSt->decC)*sin(impSt->raC)/po;
            k5 = cos(impSt->decC)/po;


            for(j=0;j<6;j++)
            {

                ai[i*6+j] = k1*kECx[j] + k2*kECy[j];
                bi[i*6+j] = k3*kECx[j] + k4*kECy[j] + k5*kECz[j];
                C[i*6*2+j] = ai[i*6+j];
                C[i*6*2+6+j] = bi[i*6+j];
                //ostm << QString("kEx[%3]: %1 - %2\n").arg(kEx[j]).arg(kECx[j]).arg(j);
                //ostm << QString("kEy[%3]: %1 - %2\n").arg(kEy[j]).arg(kECy[j]).arg(j);
                //ostm << QString("kEz[%3]: %1 - %2\n").arg(kEz[j]).arg(kECz[j]).arg(j);
            }

            La[i] = k1*dstate[0] + k2*dstate[1];
            Lb[i] = k3*dstate[0] + k4*dstate[1] + k5*dstate[2];
            Lc[i*2] = La[i];
            Lc[i*2+1] = Lb[i];

/*
            dr[0] = (impSt->state[0] - 1.5*(ti-t0)*impSt->state[3])*(dElemC.a()/elem->a());
            dr[1] = (impSt->state[1] - 1.5*(ti-t0)*impSt->state[4])*(dElemC.a()/elem->a());
            dr[2] = (impSt->state[2] - 1.5*(ti-t0)*impSt->state[5])*(dElemC.a()/elem->a());
/
        }

        lsm(6, sz*2, Ec, C, Lc, uweC, Dc);
        ostm << QString("Ec: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Ec[0], 12, 'f', 7).arg(Ec[1], 12, 'f', 7).arg(Ec[2], 12, 'f', 7).arg(Ec[3], 12, 'f', 7).arg(Ec[4], 12, 'f', 7).arg(Ec[5], 12, 'f', 7);
        E2elem_low(Ec, &dElem, elem);
        ostm << QString("dElemC\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);
*/
        //detE00(impObj, opos, elem, elemI);
        //if(elem->ecc>0.1) detE0(impObj, opos, elem, elemI);
        //else detE1(impObj, opos, elem, elemI);
/*
        detEB(impObj, opos, elem, dElem);
        deStm << QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\n").arg(impObj->name, 16).arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

*/
        detE00(impObj, opos, elem, dElem);
        deStm << QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\n").arg(impObj->name, 16).arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);
/*

        detE0(impObj, opos, elem, dElem);
        deStm << QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\n").arg(impObj->name, 16).arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);


        detE1(impObj, opos, elem, dElem);
        deStm << QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\n").arg(impObj->name, 16).arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);



        detE2(impObj, opos, elem, dElem);
        deStm << QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\n").arg(impObj->name, 16).arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

        detE3(impObj, opos, elem, dElem);
        deStm << QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\n\n").arg(impObj->name, 16).arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

        //if(elem->ecc<0.1) continue;
            //detE00(impObj, opos, &elemC, dElem);
        //else detE3(impObj, opos, &elemC, dElem);
/*
        elemI->eJD = elemC.eJD + dElem.eJD;

        elemI->M0 = elemC.M0 + dElem.M0;
        elemI->q = elemC.q - dElem.q;
        elemI->ecc = elemC.ecc - dElem.ecc;

        elemI->Node = elemC.Node - dElem.Node;
        elemI->inc = elemC.inc - dElem.inc;
        elemI->w = elemC.w - dElem.w;

        //elemI->eJD = elem->eJD;
        elemI->M0 = elem->M0;
*/

        elemI->eJD = elem->eJD + dElem.eJD;

        elemI->M0 = elem->M0 + dElem.M0;
        elemI->q = elem->q + dElem.q;
        elemI->ecc = elem->ecc + dElem.ecc;

        elemI->Node = elem->Node + dElem.Node;
        elemI->inc = elem->inc + dElem.inc;
        elemI->w = elem->w + dElem.w;

        if(elemI->M0<0.0) elemI->M0 += 2.0*PI*(floor(fabs(elemI->M0)/(2.0*PI))+1);
        if(elemI->Node<0.0) elemI->Node += 2.0*PI*(floor(fabs(elemI->Node)/(2.0*PI))+1);
        if(elemI->w<0.0) elemI->w += 2.0*PI*(floor(fabs(elemI->w)/(2.0*PI))+1);

        if(elemI->M0>2.0*PI) elemI->M0 -= (2.0*PI)*floor(elemI->M0/(2.0*PI));
        if(elemI->Node>2.0*PI) elemI->Node -= 2.0*PI*floor(elemI->Node/(2.0*PI));
        if(elemI->w>2.0*PI) elemI->w -= 2.0*PI*floor(elemI->w/(2.0*PI));

        ostm << QString("elem_imp\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(elemI->eJD, 15, 'f', 8).arg(rad2grad(elemI->M0), 10, 'f', 7).arg(elemI->q, 10, 'f', 7).arg(elemI->ecc, 10, 'f', 7).arg(rad2grad(elemI->inc), 10, 'f', 7).arg(rad2grad(elemI->w), 10, 'f', 7).arg(rad2grad(elemI->Node), 10, 'f', 7);
        ostm << QString("elem_ini\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(elem->eJD, 15, 'f', 8).arg(rad2grad(elem->M0), 10, 'f', 7).arg(elem->q, 10, 'f', 7).arg(elem->ecc, 10, 'f', 7).arg(rad2grad(elem->inc), 10, 'f', 7).arg(rad2grad(elem->w), 10, 'f', 7).arg(rad2grad(elem->Node), 10, 'f', 7);
        //ostm << QString("elem_ini\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(elem->eJD, 15, 'f', 8).arg(elem->M0, 10, 'f', 7).arg(elem->q, 10, 'f', 7).arg(elem->ecc, 10, 'f', 7).arg(elem->inc, 10, 'f', 7).arg(elem->w, 10, 'f', 7).arg(elem->Node, 10, 'f', 7);

        //impObj->impOrb.set(&ocatI);
        elemI->set(ocatI.record);
        //elemC.set(ocatI.record);

        ocatI.record->set_author("Berezhnoy");
        ocatI.record->set_name(impObj->name.toAscii().data());
        ocatI.record->set_number(p+1);
        ocatI.record->set_makeD(QDate().currentDate().toString("yyMMdd").toAscii().data());

        ocatI.AddRec();

        //ostm << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(impObj->name, 16).arg(fabs(elem->eJD-elemI->eJD)/elem->eJD, 15, 'f', 8).arg((elem->M0-elemI->M0)/elem->M0, 10, 'f', 7).arg((elem->q-elemI->q)/elem->q, 10, 'f', 7).arg((elem->ecc-elemI->ecc)/elem->ecc, 10, 'f', 7).arg((elem->inc-elemI->inc)/elem->inc, 10, 'f', 7).arg((elem->w-elemI->w)/elem->w, 10, 'f', 7).arg((elem->Node-elemI->Node)/elem->Node, 10, 'f', 7);
        //deStm << QString("%1|%2|%3|%4|%5|%6|%7|%8\n").arg(impObj->name, 16).arg(fabs(elem->eJD-elemI->eJD)/elem->eJD, 15, 'f', 8).arg((elem->M0-elemI->M0)/elem->M0, 10, 'f', 7).arg((elem->q-elemI->q)/elem->q, 10, 'f', 7).arg((elem->ecc-elemI->ecc)/elem->ecc, 10, 'f', 7).arg((elem->inc-elemI->inc)/elem->inc, 10, 'f', 7).arg((elem->w-elemI->w)/elem->w, 10, 'f', 7).arg((elem->Node-elemI->Node)/elem->Node, 10, 'f', 7);


    }

    ocatI.Save();


    return 0;//a.exec();
}

void detEB(inproveObject *impObj, observ &opos, orbElem *elem, orbElem &dElem)
{
    ostm << QString("detE00: mid hi ecc\n");
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
    double H, K, r, ss, aval, nn, dT, pval;
    double psi[3];
    double tgw0, tgw1;
    double k1, k2, k3, k4, k5;
    double kEx[6], kEy[6], kEz[6];
    double kExS[6], kEyS[6], kEzS[6];
    double PQ[3];
    double psi0, psi1, psi2;
    int lowEcc;
    improveState* impSt;
    QStringList aiList, biList;

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


    lowEcc = elem->ecc<0.1;
    aval = elem->q/(1.0-elem->ecc);
    nn = ka*pow(aval, -1.5);
    pval = aval*(1.0 - elem->ecc*elem->ecc);

    ostm << QString("impObj sz: %1\n").arg(sz);
    double kx[6], ky[6], kz[6];

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

        H = (r + pval - 2.0*aval)/(2.0*elem->ecc*pval);
        K = ((r+pval)*ss)*(1.0/(2.0*pval*elem->ecc*nn*nn*aval*aval));
        //H = (r-aval*(1.0 + elem->ecc*elem->ecc))/(elem->ecc*aval*(1.0 - elem->ecc*elem->ecc));
        //K = (aval*ss/(ka*ka*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
        //K = (ss/(aval*aval*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
        dT = impSt->jdTDB - TDT2TDB(elem->eJD);
        //qDebug() << QString("%1\t%2\t%3\n").arg(aval).arg(H).arg(K);
        //P??

        detPQR(P, Q, R, elem);

        detN(N, elem);
        /*N[0] = cos(elem->Node);
        N[1] = sin(elem->Node)*cos(EKV);
        N[2] = sin(elem->Node)*sin(EKV);*/

//med hi ecc
//[0-5]:tab42

//pqs low ecc
        kEx[0] = impSt->state[3]/nn;
        //dp
        kEx[1] = P[1]*impSt->state[2] - P[2]*impSt->state[1];
        //dq
        kEx[2] = Q[1]*impSt->state[2] - Q[2]*impSt->state[1];
        //ds*ecc
        kEx[3] =  (R[1]*impSt->state[2] - R[2]*impSt->state[1] - impSt->state[3]/nn)/(2.0*elem->ecc);
        //da
        kEx[4] = (impSt->state[0] - 1.5*dT*impSt->state[3])/10.0;
        //decc
        kEx[5] = H*impSt->state[0]+K*impSt->state[3];
        //dM+ds

        kEy[0] = impSt->state[4]/nn;
        kEy[1] = P[2]*impSt->state[0] - P[0]*impSt->state[2];
        kEy[2] = Q[2]*impSt->state[0] - Q[0]*impSt->state[2];
        kEy[3] = (R[2]*impSt->state[0] - R[0]*impSt->state[2]-impSt->state[4]/nn)/(2.0*elem->ecc);
        kEy[4] = (impSt->state[1] - 1.5*dT*impSt->state[4])/10.0;
        kEy[5] = H*impSt->state[1]+K*impSt->state[4];

        kEz[0] = impSt->state[5]/nn;
        kEz[1] = P[0]*impSt->state[1] - P[1]*impSt->state[0];
        kEz[2] = Q[0]*impSt->state[1] - Q[1]*impSt->state[0];
        kEz[3] = (R[0]*impSt->state[1] - R[1]*impSt->state[0]-impSt->state[5]/nn)/(2.0*elem->ecc);
        kEz[4] = (impSt->state[2] - 1.5*dT*impSt->state[5])/10.0;
        kEz[5] = H*impSt->state[2]+K*impSt->state[5];

    ///////////////////////////////////////

        //
        k1 = -sin(impSt->raC)/po;
        k2 = cos(impSt->raC)/po;
        k3 = -sin(impSt->decC)*cos(impSt->raC)/po;
        k4 = -sin(impSt->decC)*sin(impSt->raC)/po;
        k5 = cos(impSt->decC)/po;

        //aiList.clear(); biList.clear();
        for(j=0;j<6;j++)
        {
            ai[i*6+j] = k1*kEx[j] + k2*kEy[j];
            bi[i*6+j] = k3*kEx[j] + k4*kEy[j] + k5*kEz[j];
            C[i*6+j] = ai[i*6+j];
            C[6*sz+i*6+j] = bi[i*6+j];
            //ostm << QString("ai: %1\nbi: %2\n").arg(ai[i*6+j]).arg(bi[i*6+j]);
            //aiList << QString("%1").arg(ai[i*6+j], 15, 'f', 10);
            //biList << QString("%1").arg(bi[i*6+j], 15, 'f', 10);
        }

        La[i] = (impSt->raO - impSt->raC)*cos(impSt->decC);
        Lb[i] = (impSt->decO - impSt->decC);
        Lc[i] = La[i];
        Lc[sz+i] = Lb[i];

        //ostm << QString("La: %1 = %2\n").arg(aiList.join(" + ")).arg(La[i]);
        //ostm << QString("Lb: %1 = %2\n").arg(biList.join(" + ")).arg(Lb[i]);
    }

//ai
    for(int l=0; l<6; l++) Ea[l]=0;
    lsm(6, sz, Ea, ai, La, uweA, Da);

    ostm << QString("uweA: %1\n").arg(uweA);
    ostm << QString("Ea:\t%1\t%2\t%3\t%4\t%5\t%6\n").arg(Ea[0], 12, 'f', 7).arg(Ea[1], 12, 'f', 7).arg(Ea[2], 12, 'f', 7).arg(Ea[3], 12, 'f', 7).arg(Ea[4], 12, 'f', 7).arg(Ea[5], 12, 'f', 7);
    //elemI = impObj->impOrb.elem;
/*
    double summ, summPow;
    summPow = 0.0;
    for(i=0;i<sz;i++)
    {
        summ = 0;
        aiList.clear();biList.clear();
        for(j=0; j<6; j++)
        {
            summ += Ea[j]*ai[i*6+j];
            aiList << QString("%1*%2").arg(ai[i*6+j], 15, 'f', 10).arg(Ea[j]);
            biList << QString("%1").arg(Ea[j]*ai[i*6+j], 15, 'f', 10);
        }
        summPow += summ*summ;
        ostm << QString("La:\t%1 = %2\n").arg(summ).arg(La[i]);
        ostm << QString("La: %1 = %2\n").arg(aiList.join(" + ")).arg(La[i]);
        ostm << QString("La: %1 = %2\n").arg(biList.join(" + ")).arg(La[i]);
    }
    ostm << QString("summPow:\t%1\n").arg(summPow);
/*
    E2elem_low1(Ea, &dElem, elem);

    //ostm << QString("dElemA\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

//bi
    //slsm(6, sz, Eb, bi, Lb);
    for(int l=0; l<6; l++) Eb[l]=0;
    lsm(6, sz, Eb, bi, Lb, uweB, Db);

    ostm << QString("uweB: %1\n").arg(uweB);
    ostm << QString("Eb:\t%1\t%2\t%3\t%4\t%5\t%6\n").arg(Eb[0], 12, 'f', 7).arg(Eb[1], 12, 'f', 7).arg(Eb[2], 12, 'f', 7).arg(Eb[3], 12, 'f', 7).arg(Eb[4], 12, 'f', 7).arg(Eb[5], 12, 'f', 7);
    //elemI = impObj->impOrb.elem;

    E2elem_low1(Eb, &dElem, elem);

    //ostm << QString("dElemB\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);
*/

//C
    for(int l=0; l<6; l++) Ec[l]=0;
    //for(int l=0; l<sz*2; l++) Wc[l]=1.0;
    lsm(6, sz*2, Ec, C, Lc, uweC, Dc);
    //slsm(6, sz*2, Ec, C, Lc);

    //ostm << QString("uweC: %1\n").arg(uweC);

    ostm << QString("Ec:\t%1\t%2\t%3\t%4\t%5\t%6\n").arg(Ec[0], 12, 'f', 7).arg(Ec[1], 12, 'f', 7).arg(Ec[2], 12, 'f', 7).arg(Ec[3], 12, 'f', 7).arg(Ec[4], 12, 'f', 7).arg(Ec[5], 12, 'f', 7);


    double ecSum = 0.0;
    for(int l=0; l<6; l++) ecSum += fabs(Ec[l]);
   ostm << QString("uweC: %1\tecSum:%2\n").arg(uweC).arg(ecSum);

    //E2elem_pqs_low(Ec, &dElem, elem);
   //E2elem_low1(Ec, &dElem, elem);
/*
   double summ, summPow;
   summPow = 0.0;
   for(i=0;i<sz*2;i++)
   {
       summ = 0;
       aiList.clear();biList.clear();
       for(j=0; j<6; j++)
       {
           summ += Ec[j]*C[i*6+j];
           aiList << QString("%1*%2").arg(C[i*6+j], 15, 'f', 10).arg(Ec[j]);
           biList << QString("%1").arg(Ec[j]*C[i*6+j], 15, 'f', 10);
       }
       summPow += summ*summ;
       ostm << QString("Lc:\t%1 = %2\n").arg(summ).arg(Lc[i]);
       ostm << QString("Lc: %1 = %2\n").arg(aiList.join(" + ")).arg(Lc[i]);
       ostm << QString("Lc: %1 = %2\n").arg(biList.join(" + ")).arg(Lc[i]);
   }
   ostm << QString("summPow:\t%1\n").arg(summPow);
*/
   double dp, dq, dr, da, a0;
   dp = Ec[1];
   dq = Ec[2];
   dr = Ec[3]/(2.0*elem->ecc);

   dElem.M0 = Ec[0] - dr;
   dElem.ecc = Ec[5]/2.0;
   a0 = elem->q/(1.0-elem->ecc*elem->ecc);
   da = Ec[4]*a0/10.0;
   dElem.q = da*(1.0-elem->ecc*elem->ecc);

   dElem.inc = dp*cos(elem->w) - dq*sin(elem->w);
   dElem.Node = (dp*sin(elem->w) + dq*cos(elem->w))/sin(elem->inc);
   dElem.w = dr - cos(elem->inc)*dElem.Node;

    ostm << QString("dElemC\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

}


void detE00(inproveObject *impObj, observ &opos, orbElem *elem, orbElem &dElem)
{
    ostm << QString("detE00: mid hi ecc\n");
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
    double H, K, r, ss, aval, nn, dT, pval;
    double psi[3];
    double tgw0, tgw1;
    double k1, k2, k3, k4, k5;
    double kEx[6], kEy[6], kEz[6];
    double kExS[6], kEyS[6], kEzS[6];
    double PQ[3];
    double psi0, psi1, psi2;
    int lowEcc;
    improveState* impSt;
    QStringList aiList, biList;

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


    double Eval;
    lowEcc = elem->ecc<0.1;
    aval = elem->q/(1.0-elem->ecc);
    nn = ka*pow(aval, -1.5);
    pval = aval*(1.0 - elem->ecc*elem->ecc);


    ostm << QString("impObj sz: %1\n").arg(sz);
    double kx[6], ky[6], kz[6];

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
        dT = impSt->jdTDB - TDT2TDB(elem->eJD);


        //eKepler(&Eval, nn*dT+elem->M0, elem->ecc);
        //ostm << QString("proove vel: %1 ?= %2\n").arg(ss/nn).arg(aval*aval*elem->ecc*sin(Eval));

        H = (r + pval - 2.0*aval)/(elem->ecc*pval);
        K = ((r+pval)/pval)*(ss/(aval*aval*elem->ecc*nn));
        //H = (r-aval*(1.0 + elem->ecc*elem->ecc))/(elem->ecc*aval*(1.0 - elem->ecc*elem->ecc));
        //K = (aval*ss/(ka*ka*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
        //K = (ss/(aval*aval*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));

        //qDebug() << QString("%1\t%2\t%3\n").arg(aval).arg(H).arg(K);
        //P??

        detPQR(P, Q, R, elem);

        detN(N, elem);
        /*N[0] = cos(elem->Node);
        N[1] = sin(elem->Node)*cos(EKV);
        N[2] = sin(elem->Node)*sin(EKV);

//med hi ecc
//[0-5]:tab42
/*
//pqs low ecc
        //dp
        kEx[0] = P[1]*impSt->state[2] - P[2]*impSt->state[1];
        //dq
        kEx[1] = Q[1]*impSt->state[2] - Q[2]*impSt->state[1];
        //ds*ecc
        kEx[2] =  ((R[1]*impSt->state[2] - R[2]*impSt->state[1]) - impSt->state[3]/nn)/elem->ecc;
        //da
        kEx[3] = impSt->state[0] - 1.5*dT*impSt->state[3];
        //decc
        kEx[4] = H*impSt->state[0]+K*impSt->state[3]/nn;
        //dM+ds
        kEx[5] = impSt->state[3]/nn;

        kEy[0] = P[2]*impSt->state[0] - P[0]*impSt->state[2];
        kEy[1] = Q[2]*impSt->state[0] - Q[0]*impSt->state[2];
        kEy[2] = ((R[2]*impSt->state[0] - R[0]*impSt->state[2])-impSt->state[4]/nn)/elem->ecc;
        kEy[3] = impSt->state[1] - 1.5*dT*impSt->state[4];
        kEy[4] = H*impSt->state[1]+K*impSt->state[4]/nn;
        kEy[5] = impSt->state[4]/nn;

        kEz[0] = P[0]*impSt->state[1] - P[1]*impSt->state[0];
        kEz[1] = Q[0]*impSt->state[1] - Q[1]*impSt->state[0];
        kEz[2] = ((R[0]*impSt->state[1] - R[1]*impSt->state[0])-impSt->state[5]/nn)/elem->ecc;
        kEz[3] = impSt->state[2] - 1.5*dT*impSt->state[5];
        kEz[4] = H*impSt->state[2]+K*impSt->state[5]/nn;
        kEz[5] = impSt->state[5]/nn;
*/
        //dNode
        kx[0] = -impSt->state[2]*sin(EKV) - impSt->state[1]*cos(EKV);
        //dw
        kx[1] = R[1]*impSt->state[2] - R[2]*impSt->state[1];
        //di
        kx[2] = N[1]*impSt->state[2] - N[2]*impSt->state[1];

        kx[3] = impSt->state[0] - 1.5*dT*impSt->state[3];
        kx[4] = H*impSt->state[0]+K*impSt->state[3]/nn;
        kx[5] = impSt->state[3]/nn;
        //kEx[5] = -state[3]/nn - state[1];

        ky[0] = impSt->state[0]*cos(EKV);
        ky[1] = R[2]*impSt->state[0] - R[0]*impSt->state[2];
        ky[2] = N[2]*impSt->state[0] - N[0]*impSt->state[2];

        ky[3] = impSt->state[1] - 1.5*dT*impSt->state[4];
        ky[4] = H*impSt->state[1]+K*impSt->state[4]/nn;
        ky[5] = impSt->state[4]/nn;
        //kEy[5] = -state[4]/nn + state[0];

        kz[0] = impSt->state[0]*sin(EKV);
        kz[1] = R[0]*impSt->state[1] - R[1]*impSt->state[0];
        kz[2] = N[0]*impSt->state[1] - N[1]*impSt->state[0];

        kz[3] = impSt->state[2] - 1.5*dT*impSt->state[5];
        kz[4] = H*impSt->state[2]+K*impSt->state[5]/nn;
        kz[5] = impSt->state[5]/nn;
        //kEz[5] = -state[5]/nn;
        //ostm << QString("dNode: %1\n").arg(kx[0]*impSt->state[0] + ky[0]*impSt->state[1] + kz[0]*impSt->state[2]);
        //ostm << QString("dw: %1\n").arg(kx[1]*impSt->state[0] + ky[1]*impSt->state[1] + kz[1]*impSt->state[2]);
        //ostm << QString("dinc: %1\n").arg(kx[2]*impSt->state[0] + ky[2]*impSt->state[1] + kz[2]*impSt->state[2]);

//////////////////////////////////////  substitution
        //kEx[0] = kx[0];
        //kEx[1] = kx[1];
        //e*dpi
        kEx[0] = (kx[0] - kx[5])/elem->ecc;
        //sini*dNode
        kEx[1] = (kx[1] - kx[0])/sin(elem->inc);
        kEx[2] = kx[2];
        kEx[3] = kx[3];
        kEx[4] = kx[4];
        //deta
        kEx[5] = kx[5];

        //kEy[0] = ky[0];
        //kEy[1] = ky[1];
        kEy[0] = (ky[0] - ky[5])/elem->ecc;
        kEy[1] = (ky[1] - ky[0])/sin(elem->inc);
        kEy[2] = ky[2];
        kEy[3] = ky[3];
        kEy[4] = ky[4];
        kEy[5] = ky[5];

        //kEz[0] = kz[0];
        //kEz[1] = kz[1];
        kEz[0] = (kz[0] - kz[5])/elem->ecc;
        kEz[1] = (kz[1] - kz[0])/sin(elem->inc);
        kEz[2] = kz[2];
        kEz[3] = kz[3];
        kEz[4] = kz[4];
        kEz[5] = kz[5];

    ///////////////////////////////////////



        //
        k1 = -sin(impSt->raC);///po;
        k2 = cos(impSt->raC);///po;
        k3 = -sin(impSt->decC)*cos(impSt->raC);//po;
        k4 = -sin(impSt->decC)*sin(impSt->raC);//po;
        k5 = cos(impSt->decC);//po;


        //aiList.clear(); biList.clear();
        for(j=0;j<6;j++)
        {
            ai[i*6+j] = k1*kEx[j] + k2*kEy[j];
            bi[i*6+j] = k3*kEx[j] + k4*kEy[j] + k5*kEz[j];
            C[i*6+j] = ai[i*6+j];
            C[6*sz+i*6+j] = bi[i*6+j];
            //ostm << QString("ai: %1\nbi: %2\n").arg(ai[i*6+j]).arg(bi[i*6+j]);
            //aiList << QString("%1").arg(ai[i*6+j], 15, 'f', 10);
            //biList << QString("%1").arg(bi[i*6+j], 15, 'f', 10);
        }

        La[i] = po*(impSt->raO - impSt->raC)*cos(impSt->decC);
        Lb[i] = po*(impSt->decO - impSt->decC);
        Lc[i] = La[i];
        Lc[sz+i] = Lb[i];

        //ostm << QString("La: %1 = %2\n").arg(aiList.join(" + ")).arg(La[i]);
        //ostm << QString("Lb: %1 = %2\n").arg(biList.join(" + ")).arg(Lb[i]);
    }
/*
//ai
    for(int l=0; l<6; l++) Ea[l]=0;
    lsm(6, sz, Ea, ai, La, uweA, Da);

    ostm << QString("uweA: %1\n").arg(uweA);
    ostm << QString("Ea:\t%1\t%2\t%3\t%4\t%5\t%6\n").arg(Ea[0], 12, 'f', 7).arg(Ea[1], 12, 'f', 7).arg(Ea[2], 12, 'f', 7).arg(Ea[3], 12, 'f', 7).arg(Ea[4], 12, 'f', 7).arg(Ea[5], 12, 'f', 7);
    //elemI = impObj->impOrb.elem;

    E2elem_low1(Ea, &dElem, elem);

    //ostm << QString("dElemA\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

//bi
    //slsm(6, sz, Eb, bi, Lb);
    for(int l=0; l<6; l++) Eb[l]=0;
    lsm(6, sz, Eb, bi, Lb, uweB, Db);

    ostm << QString("uweB: %1\n").arg(uweB);
    ostm << QString("Eb:\t%1\t%2\t%3\t%4\t%5\t%6\n").arg(Eb[0], 12, 'f', 7).arg(Eb[1], 12, 'f', 7).arg(Eb[2], 12, 'f', 7).arg(Eb[3], 12, 'f', 7).arg(Eb[4], 12, 'f', 7).arg(Eb[5], 12, 'f', 7);
    //elemI = impObj->impOrb.elem;

    E2elem_low1(Eb, &dElem, elem);

    //ostm << QString("dElemB\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);
*/

//C
    for(int l=0; l<6; l++) Ec[l]=0;
    //for(int l=0; l<sz*2; l++) Wc[l]=1.0;
    lsm(6, sz*2, Ec, C, Lc, uweC, Dc);
    //slsm(6, sz*2, Ec, C, Lc);

    //ostm << QString("uweC: %1\n").arg(uweC);

    ostm << QString("Ec:\t%1\t%2\t%3\t%4\t%5\t%6\n").arg(Ec[0], 12, 'f', 7).arg(Ec[1], 12, 'f', 7).arg(Ec[2], 12, 'f', 7).arg(Ec[3], 12, 'f', 7).arg(Ec[4], 12, 'f', 7).arg(Ec[5], 12, 'f', 7);


    double ecSum = 0.0;
    for(int l=0; l<6; l++) ecSum += fabs(Ec[l]);
   ostm << QString("uweC: %1\tecSum:%2\n").arg(uweC).arg(ecSum);

    //E2elem_pqs_low(Ec, &dElem, elem);
   E2elem_low1(Ec, &dElem, elem);

    ostm << QString("dElemC\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

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

void E2elem_pqs_low(double *E, orbElem *dElem, orbElem *elem)
{
    double dp, dq, ds, a1;
    dElem->ecc = E[4];
    dp = E[0];
    dq = E[1];
    ds = E[2]/elem->ecc;
//3.3.10
    dElem->inc = dp*cos(elem->w) - dq*sin(elem->w);
    dElem->Node = (dp*sin(elem->w) + dq*cos(elem->w))/sin(elem->inc);
    dElem->w = ds - cos(elem->inc)*dElem->Node;

    dElem->M0 = E[5] - ds;
    a1 = E[3]/(elem->q/(1.0-elem->ecc));
    qDebug() << QString("da1: %1 = %2\n").arg(a1).arg(elem->q/(1.0-elem->ecc));
    dElem->q = a1*(1.0-elem->ecc);
}

void E2elem_psi(double *E, orbElem *dElem, orbElem *elem)
{
    double dp, dq, dr, a0;
    double psi[3];
    dElem->ecc = E[4];

    psi[0] = E[0];
    psi[1] = E[1];
    psi[2] = E[2];
    /*
//3.3.07
    psi[0] = E[0];
    psi[1] = E[1]*cos(EKV) + E[2]*sin(EKV);
    psi[2] = -E[1]*sin(EKV) + E[2]*cos(EKV);
//3.3.06
    dElem->w = (psi[0]*sin(elem->Node) - psi[1]*cos(elem->Node))/sin(elem->inc);
    dElem->inc = psi[0]*cos(elem->Node) + psi[1]*sin(elem->Node);
    dElem->Node = psi[2] - dElem->w*cos(elem->inc);
*/
    dElem->inc = cos(elem->Node)*E[0] + sin(elem->Node)*(E[1]*cos(EKV) + E[2]*sin(EKV));
    dElem->w = (sin(elem->Node)*E[0] - cos(elem->Node)*(E[1]*cos(EKV) + E[2]*sin(EKV)))/sin(elem->inc);
    dElem->Node = -cos(elem->inc)*dElem->w - E[1]*sin(EKV)+E[2]*cos(EKV);

    dElem->M0 = E[5];
    a0 = elem->q/(1.0-elem->ecc);
    double da = E[3]*a0;
    double q1 = (a0+da)*(1.0-(elem->ecc+dElem->ecc));
    dElem->q = q1 - elem->q;
    //qDebug() << QString("da1: %1 = %2\n").arg(a1).arg(elem->q/(1.0-elem->ecc));
    //dElem->q = a1*(1.0-elem->ecc);
}

//E1
void E2elem_psi_low(double *E, orbElem *dElem, orbElem *elem)
{
    double dp, dq, dr, a1;
    double psi[3];
    dElem->ecc = E[4];

    psi[0] = E[0];
    psi[1] = E[1];
    psi[2] = E[2];
    /*
//3.3.07
    psi[0] = E[0];
    psi[1] = E[1]*cos(EKV) + E[2]*sin(EKV);
    psi[2] = -E[1]*sin(EKV) + E[2]*cos(EKV);
//3.3.06
    dElem->w = (psi[0]*sin(elem->Node) - psi[1]*cos(elem->Node))/sin(elem->inc);
    dElem->inc = psi[0]*cos(elem->Node) + psi[1]*sin(elem->Node);
    dElem->Node = psi[2] - dElem->w*cos(elem->inc);
*/
    dElem->inc = cos(elem->Node)*E[0] + sin(elem->Node)*(E[1]*cos(EKV) + E[2]*sin(EKV));
    dElem->w = (sin(elem->Node)*E[0] - cos(elem->Node)*(E[1]*cos(EKV) + E[2]*sin(EKV)))/sin(elem->inc);
    dElem->Node = -cos(elem->inc)*dElem->w - E[1]*sin(EKV)+E[2]*cos(EKV);

    dElem->M0 = E[5]-E[2];
    a1 = E[3]/(elem->q/(1.0-elem->ecc));
    qDebug() << QString("da1: %1 = %2\n").arg(a1).arg(elem->q/(1.0-elem->ecc));
    dElem->q = a1*(1.0-elem->ecc);
}



void E2elem_arb(double *E, orbElem *dElem, orbElem *elem)
{
    double a1, da1;
    dElem->ecc = E[4];
    dElem->inc = E[2];
    dElem->Node = E[0];
    dElem->w = E[1];

    dElem->M0 = E[5] - E[1];
    a1 = elem->q/(1.0-elem->ecc);
    da1 = E[3]*a1;
    //qDebug() << QString("da1: %1 = %2\n").arg(a1).arg(da1);
    dElem->q = da1*(1.0-elem->ecc);
}

void E2elem_low(double *E, orbElem *dElem, orbElem *elem)
{
    double a1, da1;
    dElem->ecc = E[4];
    dElem->inc = E[2];
    dElem->Node = E[1]/sin(elem->inc);
    dElem->w = E[0]/elem->ecc - dElem->Node;

    dElem->M0 = E[5] - dElem->w - dElem->Node;
    a1 = elem->q/(1.0-elem->ecc);
    da1 = E[3]*a1;
    qDebug() << QString("da1: %1 = %2\n").arg(a1).arg(da1);
    //dElem->q = da1*(1.0-elem->ecc);
    dElem->q = da1*(1.0-dElem->ecc);
}

void E2elem_low1(double *E, orbElem *dElem, orbElem *elem)
{
    dElem->ecc = E[4];
    dElem->inc = E[2];
    dElem->Node = E[1]/sin(elem->inc);
    dElem->w = E[0]/elem->ecc - dElem->Node;

    dElem->M0 = E[5] - dElem->w - dElem->Node;
    double a0 = elem->q/(1.0-elem->ecc);
    double da = E[3]*a0;
    double q1 = (a0+da)*(1.0-(elem->ecc+dElem->ecc));
    //dElem->q = q1 - elem->q;
    dElem->q = da*(1.0-dElem->ecc);
}



void detPQR(double *P, double *Q, double *R, orbElem *elem)
{
    //P??
    P[0] = cos(elem->w)*cos(elem->Node) - sin(elem->w)*sin(elem->Node)*cos(elem->inc);
    P[1] = (cos(elem->w)*sin(elem->Node) + sin(elem->w)*cos(elem->Node)*cos(elem->inc))*cos(EKV) - sin(elem->w)*sin(elem->inc)*sin(EKV);
    P[2] = (cos(elem->w)*sin(elem->Node) + sin(elem->w)*cos(elem->Node)*cos(elem->inc))*sin(EKV) + sin(elem->w)*sin(elem->inc)*cos(EKV);
    //Q??
    Q[0] = -sin(elem->w)*cos(elem->Node) - cos(elem->w)*sin(elem->Node)*cos(elem->inc);
    Q[1] = (-sin(elem->w)*sin(elem->Node) + cos(elem->w)*cos(elem->Node)*cos(elem->inc))*cos(EKV) - cos(elem->w)*sin(elem->inc)*sin(EKV);
    Q[2] = (-sin(elem->w)*sin(elem->Node) + cos(elem->w)*cos(elem->Node)*cos(elem->inc))*sin(EKV) + cos(elem->w)*sin(elem->inc)*cos(EKV);
    //r
    R[0] = sin(elem->inc)*sin(elem->Node);
    R[1] = -sin(elem->inc)*cos(elem->Node)*cos(EKV) - cos(elem->inc)*sin(EKV);
    R[2] = -sin(elem->inc)*cos(elem->Node)*sin(EKV) + cos(elem->inc)*cos(EKV);
}

void detN(double *N, orbElem *elem)
{
    N[0] = cos(elem->Node);
    N[1] = sin(elem->Node)*cos(EKV);
    N[2] = cos(elem->Node)*sin(EKV);
}

void detKE0(double *kEx, double *kEy, double *kEz, double *state, double H, double K, double nn, double dT, orbElem *elem)
{
//arb ecc
//tab44
    double P[3], Q[3], R[3];
    detPQR(P, Q, R, elem);
    kEx[0] = P[1]*state[2] - P[2]*state[1];
    kEx[1] = Q[1]*state[2] - Q[2]*state[1];
    kEx[2] = -state[3]/nn + R[1]*state[2] - R[2]*state[1];
    kEx[3] = state[0] - 1.5*dT*state[3];
    kEx[4] = H*state[0]+K*state[3];
    kEx[5] = state[3];

    kEy[0] = P[2]*state[0] - P[0]*state[2];
    kEy[1] = Q[2]*state[0] - Q[0]*state[2];
    kEy[2] = -state[4]/nn + R[2]*state[0] - R[0]*state[2];
    kEy[3] = state[1] - 1.5*dT*state[4];
    kEy[4] = H*state[1]+K*state[4];
    kEy[5] = state[4];

    kEz[0] = P[0]*state[1] - P[1]*state[0];
    kEz[1] = Q[0]*state[1] - Q[1]*state[0];
    kEz[2] = -state[5]/nn + R[0]*state[1] - R[1]*state[0];
    kEz[3] = state[2] - 1.5*dT*state[5];
    kEz[4] = H*state[2]+K*state[5];
    kEz[5] = state[5];
}

void detKE1(double *kEx, double *kEy, double *kEz, double *state, double H, double K, double nn, double dT)
{
 //tabl42
 //[2] - tabl43

    kEx[0] = 0.0;
    kEx[1] = state[2];
    kEx[2] = -state[3]/nn - state[1];
    kEx[3] = state[0] - 1.5*dT*state[3];
    kEx[4] = H*state[0]+K*state[3];
    kEx[5] = state[3]/nn;

    kEy[0] = -state[2];
    kEy[1] = 0.0;
    kEy[2] = -state[4]/nn+state[0];
    kEy[3] = state[1] - 1.5*dT*state[4];
    kEy[4] = H*state[1]+K*state[4];
    kEy[5] = state[4]/nn;

    kEz[0] = state[1];
    kEz[1] = -state[0];
    kEz[2] = -state[5]/nn;
    kEz[3] = state[2] - 1.5*dT*state[5];
    kEz[4] = H*state[2]+K*state[5];
    kEz[5] = state[5]/nn;
}

void detKE2(double *kEx, double *kEy, double *kEz, double *state, double H, double K, double nn, double dT, orbElem *elem)
{
    //  Node,inc,w - low ecc
    //[0,2]:tab47
    //[1]:tab48
    //[3-5]:tab44

    double N[3];
    double P[3], Q[3], R[3];
    detPQR(P, Q, R, elem);
    detN(N, elem);

        kEx[0] = -state[2]*sin(EKV) - state[1]*cos(EKV);
        kEx[1] = -state[3]/nn + R[1]*state[2] - R[2]*state[1];
        kEx[2] = N[1]*state[2] - N[2]*state[1];

        kEx[3] = state[0] - 1.5*dT*state[3];
        kEx[4] = H*state[0]+K*state[3];
        kEx[5] = state[3]/nn;
        //kEx[5] = -state[3]/nn - state[1];

        kEy[0] = state[0]*cos(EKV);
        kEy[1] = -state[4]/nn + R[2]*state[0] - R[0]*state[2];
        kEy[2] = N[2]*state[0] - N[0]*state[2];

        kEy[3] = state[1] - 1.5*dT*state[4];
        kEy[4] = H*state[1]+K*state[4];
        kEy[5] = state[4]/nn;
        //kEy[5] = -state[4]/nn + state[0];


        kEz[0] = state[0]*sin(EKV);
        kEz[1] = -state[5]/nn + R[0]*state[1] - R[1]*state[0];
        kEz[2] = N[0]*state[1] - N[1]*state[0];

        kEz[3] = state[2] - 1.5*dT*state[5];
        kEz[4] = H*state[2]+K*state[5];
        kEz[5] = state[5]/nn;
        //kEz[5] = -state[5]/nn;


    ///////////////////////////////////////
}

void detKE3(double *kEx, double *kEy, double *kEz, double *state, double H, double K, double nn, double dT, orbElem *elem)
{
    double N[3];
    double P[3], Q[3], R[3];
    detPQR(P, Q, R, elem);
    detN(N, elem);
    //[0-2]:tab47
    //[3-5]:tab44
        kEx[0] = -state[2]*sin(EKV) - state[1]*cos(EKV);
        kEx[1] = R[1]*state[2] - R[2]*state[1];
        kEx[2] = N[1]*state[2] - N[2]*state[1];

        kEx[3] = state[0] - 1.5*dT*state[3];
        kEx[4] = H*state[0]+K*state[3];
        kEx[5] = state[3]/nn;

        kEy[0] = state[0]*cos(EKV);
        kEy[1] = R[2]*state[0] - R[0]*state[2];
        kEy[2] = N[2]*state[0] - N[0]*state[2];

        kEy[3] = state[1] - 1.5*dT*state[4];
        kEy[4] = H*state[1]+K*state[4];
        kEy[5] = state[4]/nn;


        kEz[0] = state[0]*sin(EKV);
        kEz[1] = R[0]*state[1] - R[1]*state[0];
        kEz[2] = N[0]*state[1] - N[1]*state[0];

        kEz[3] = state[2] - 1.5*dT*state[5];
        kEz[4] = H*state[2]+K*state[5];
        kEz[5] = state[5]/nn;
}

void detKE2sa(double *kEx, double *kEy, double *kEz, double *state, double H, double K, double nn, double dT, orbElem *elem)
{
    //  Node,inc,w - low ecc
    //[0,2]:tab47
    //[1]:tab48
    //[3-5]:tab44
    double kx[6], ky[6], kz[6];
    double N[3];
    detN(N, elem);
    double P[3], Q[3], R[3];
    detPQR(P, Q, R, elem);

        kx[0] = -state[2]*sin(EKV) - state[1]*cos(EKV);
        kx[1] = -state[3]/nn + R[1]*state[2] - R[2]*state[1];
        kx[2] = N[1]*state[2] - N[2]*state[1];

        kx[3] = state[0] - 1.5*dT*state[3];
        kx[4] = H*state[0]+K*state[3];
        kx[5] = state[3]/nn;
        //kEx[5] = -state[3]/nn - state[1];

        ky[0] = state[0]*cos(EKV);
        ky[1] = -state[4]/nn + R[2]*state[0] - R[0]*state[2];
        ky[2] = N[2]*state[0] - N[0]*state[2];

        ky[3] = state[1] - 1.5*dT*state[4];
        ky[4] = H*state[1]+K*state[4];
        ky[5] = state[4]/nn;
        //kEy[5] = -state[4]/nn + state[0];

        kz[0] = state[0]*sin(EKV);
        kz[1] = -state[5]/nn + R[0]*state[1] - R[1]*state[0];
        kz[2] = N[0]*state[1] - N[1]*state[0];

        kz[3] = state[2] - 1.5*dT*state[5];
        kz[4] = H*state[2]+K*state[5];
        kz[5] = state[5]/nn;
        //kEz[5] = -state[5]/nn;
//////////////////////////////////////  substitution
        kEx[0] = (kx[1] - kx[0])/sin(elem->inc);
        kEx[1] = (kx[0] - kx[5])/elem->ecc;
        kEx[2] = kx[2];
        kEx[3] = kx[3];
        kEx[4] = kx[4];
        kEx[5] = kx[5];

        kEy[0] = (ky[1] - ky[0])/sin(elem->inc);
        kEy[1] = (ky[0] - ky[5])/elem->ecc;
        kEy[2] = ky[2];
        kEy[3] = ky[3];
        kEy[4] = ky[4];
        kEy[5] = ky[5];

        kEz[0] = (kz[1] - kz[0])/sin(elem->inc);
        kEz[1] = (kz[0] - kz[5])/elem->ecc;
        kEz[2] = kz[2];
        kEz[3] = kz[3];
        kEz[4] = kz[4];
        kEz[5] = kz[5];

    ///////////////////////////////////////
}

void detE(inproveObject *impObj, observ &opos, orbElem *elem, orbElem &dElem)
{
    ostm << QString("detE1: low ecc\n");
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
    double H, K, r, ss, aval, nn, dT, pval;
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
    pval = aval*(1.0 - elem->ecc*elem->ecc);
//ostm << QString("nn: %1\n").arg(nn);
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

        //H = (r + pval - 2.0*aval)/(elem->ecc*pval);
        //K = ((r+pval)/pval)*(ss/(aval*aval*elem->ecc*nn))/nn;
        H = (r-aval*(1.0 + elem->ecc*elem->ecc))/(elem->ecc*aval*(1.0 - elem->ecc*elem->ecc));
        K = (aval*ss/(ka*ka*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));

        //K = (ss/(aval*aval*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
        dT = impSt->jdTDB - TDT2TDB(elem->eJD);

//        detPQR(P, Q, R, elem);
        //ostm << QString("proove R: %1 ?= 1\n").arg(R[0]*R[0] + R[1]*R[1] + R[2]*R[2]);



        //
        k1 = -sin(impSt->raC);///po;
        k2 = cos(impSt->raC);///po;
        k3 = -sin(impSt->decC)*cos(impSt->raC);///po;
        k4 = -sin(impSt->decC)*sin(impSt->raC);///po;
        k5 = cos(impSt->decC);///po;

//  lowecc
        detKE0(kEx, kEy, kEz, impSt->state, H, K, nn, dT, elem);
        detKE1(kEx, kEy, kEz, impSt->state, H, K, nn, dT);
        detKE2(kEx, kEy, kEz, impSt->state, H, K, nn, dT, elem);
        detKE3(kEx, kEy, kEz, impSt->state, H, K, nn, dT, elem);

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

//ai
    lsm(6, sz, Ea, ai, La, uweA, Da);

    ostm << QString("Ea: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Ea[0]).arg(Ea[1]).arg(Ea[2]).arg(Ea[3]).arg(Ea[4]).arg(Ea[5]);
    //elemI = impObj->impOrb.elem;

    //E2elem_psi(Ea, &dElem, elem);

    //ostm << QString("dElemA\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

//bi
    lsm(6, sz, Eb, bi, Lb, uweB, Db);

    ostm << QString("Eb: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Eb[0]).arg(Eb[1]).arg(Eb[2]).arg(Eb[3]).arg(Eb[4]).arg(Eb[5]);
    //elemI = impObj->impOrb.elem;

    //E2elem_psi(Eb, &dElem, elem);

    //ostm << QString("dElemB\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

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

    E2elem_pqr(Ec, &dElem, elem);
    E2elem_psi(Ec, &dElem, elem);

    ostm << QString("dElemC\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

}














































void detE0(inproveObject *impObj, observ &opos, orbElem *elem, orbElem &dElem)
{
    ostm << QString("detE0: arb ecc\n");
    orbElem *elem_ekv;
    int i, j, sz;
    double *ai, *bi, *C;
    double *La, *Lb, *Lc;
    double *Ea, *Eb, *Ec;
    double *dEc;
    double *Wa, *Wb, *Wc;
    double uweA, uweB, uweC;
    double *Da, *Db, *Dc;
    double P[3], Q[3], R[3], N[3], Recl[3];
    double stateE[3], po, a1;
    double H, K, rval, ss, aval, nn, dT, pval, Eval;
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

    dEc = new double[6];

    sz = impObj->impList.size();
    ai = new double[6*sz];
    bi = new double[6*sz];
    C = new double[6*sz*2];
    La = new double[sz];
    Lb = new double[sz];
    Lc = new double[2*sz];


    aval = elem->q/(1.0-elem->ecc);
    nn = ka*pow(aval, -1.5);
    pval = aval*(1.0 - elem->ecc*elem->ecc);

    eKepler(&Eval, elem->M0, elem->ecc);


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

        rval = sqrt(impSt->state[0]*impSt->state[0] + impSt->state[1]*impSt->state[1] + impSt->state[2]*impSt->state[2]);
        ss = impSt->state[0]*impSt->state[3] + impSt->state[1]*impSt->state[4] + impSt->state[2]*impSt->state[5];
        po = sqrt(stateE[0]*stateE[0] + stateE[1]*stateE[1] + stateE[2]*stateE[2]);

        //ostm << QString("proove: %1\t?=\t%2\n").arg(ss/nn).arg(aval*aval*elem->ecc*sin(Eval));
        //H = (r + pval - 2.0*aval)/(elem->ecc*pval);
        //K = ((r+pval)/pval)*(ss/(aval*aval*elem->ecc*nn))/nn;
        H = (rval-aval*(1.0 + elem->ecc*elem->ecc))/(elem->ecc*aval*(1.0 - elem->ecc*elem->ecc));
        K = (aval*ss/(ka*ka*elem->ecc))*(1.0 + rval/(aval*(1.0 - elem->ecc*elem->ecc)));
        //K = (ss/(aval*aval*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
        dT = impSt->jdTDB - TDT2TDB(elem->eJD);
        //qDebug() << QString("%1\t%2\t%3\n").arg(aval).arg(H).arg(K);

        //P??
/*        P[0] = cos(elem->w)*cos(elem->Node) - sin(elem->w)*sin(elem->Node)*cos(elem->inc);
        P[1] = (cos(elem->w)*sin(elem->Node) + sin(elem->w)*cos(elem->Node)*cos(elem->inc))*cos(EKV) - sin(elem->w)*sin(elem->inc)*sin(EKV);
        P[2] = (cos(elem->w)*sin(elem->Node) + sin(elem->w)*cos(elem->Node)*cos(elem->inc))*sin(EKV) + sin(elem->w)*sin(elem->inc)*cos(EKV);
        //Q??
        Q[0] = -sin(elem->w)*cos(elem->Node) - cos(elem->w)*sin(elem->Node)*cos(elem->inc);
        Q[1] = (-sin(elem->w)*sin(elem->Node) + cos(elem->w)*cos(elem->Node)*cos(elem->inc))*cos(EKV) - cos(elem->w)*sin(elem->inc)*sin(EKV);
        Q[2] = (-sin(elem->w)*sin(elem->Node) + cos(elem->w)*cos(elem->Node)*cos(elem->inc))*sin(EKV) + cos(elem->w)*sin(elem->inc)*cos(EKV);
        //r
        R[0] = sin(elem->inc)*sin(elem->Node);
        R[1] = -sin(elem->inc)*cos(elem->Node)*cos(EKV) - cos(elem->inc)*sin(EKV);
        R[2] = -sin(elem->inc)*cos(elem->Node)*sin(EKV) + cos(elem->inc)*cos(EKV);
*/
        //detPQR(P, Q, R, elem);
        //ostm << QString("proove R: %1 ?= 1\n").arg(R[0]*R[0] + R[1]*R[1] + R[2]*R[2]);

        //detN(N, elem);
        detKE0(kEx, kEy, kEz, impSt->state, H, K, nn, dT, elem);
        /*
        N[0] = cos(elem->Node);
        N[1] = sin(elem->Node)*cos(EKV);
        N[2] = sin(elem->Node)*sin(EKV);
        //ostm << QString("P: %1\t%2\t%3\n").arg(P[0]).arg(P[1]).arg(P[2]);
        //ostm << QString("Q: %1\t%2\t%3\n").arg(Q[0]).arg(Q[1]).arg(Q[2]);




        /*
//arb ecc
//tab44
        kEx[0] = P[1]*impSt->state[2] - P[2]*impSt->state[1];
        kEx[1] = Q[1]*impSt->state[2] - Q[2]*impSt->state[1];
        kEx[2] = -impSt->state[3]/nn + R[1]*impSt->state[2] - R[2]*impSt->state[1];
        kEx[3] = impSt->state[0] - 1.5*dT*impSt->state[3];
        kEx[4] = H*impSt->state[0]+K*impSt->state[3];
        kEx[5] = impSt->state[3];

        kEy[0] = P[2]*impSt->state[0] - P[0]*impSt->state[2];
        kEy[1] = Q[2]*impSt->state[0] - Q[0]*impSt->state[2];
        kEy[2] = -impSt->state[4]/nn + R[2]*impSt->state[0] - R[0]*impSt->state[2];
        kEy[3] = impSt->state[1] - 1.5*dT*impSt->state[4];
        kEy[4] = H*impSt->state[1]+K*impSt->state[4];
        kEy[5] = impSt->state[4];

        kEz[0] = P[0]*impSt->state[1] - P[1]*impSt->state[0];
        kEz[1] = Q[0]*impSt->state[1] - Q[1]*impSt->state[0];
        kEz[2] = -impSt->state[5]/nn + R[0]*impSt->state[1] - R[1]*impSt->state[0];
        kEz[3] = impSt->state[2] - 1.5*dT*impSt->state[5];
        kEz[4] = H*impSt->state[2]+K*impSt->state[5];
        kEz[5] = impSt->state[5];
*/

        //
        k1 = -sin(impSt->raC);///po;
        k2 = cos(impSt->raC);///po;
        k3 = -sin(impSt->decC)*cos(impSt->raC);///po;
        k4 = -sin(impSt->decC)*sin(impSt->raC);///po;
        k5 = cos(impSt->decC);///po;

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

        //ostm << QString("dpar0: %1\t\t%2\n\n").arg(ai[i*6+4]).arg(La[i]/0.0000001);
        //ostm << QString("dpar1: %1\t\t%2\n\n").arg(bi[i*6+4]).arg(Lb[i]/0.0000001);

        //ostm << QString("La: %1\nLb: %2\n").arg(La[i]).arg(Lb[i]);

    }

//ai
    lsm(6, sz, Ea, ai, La, uweA, Da);

    //ostm << QString("Ea: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Ea[0], 12, 'f', 7).arg(Ea[1], 12, 'f', 7).arg(Ea[2], 12, 'f', 7).arg(Ea[3], 12, 'f', 7).arg(Ea[4], 12, 'f', 7).arg(Ea[5], 12, 'f', 7);
    //elemI = impObj->impOrb.elem;

    E2elem_pqr(Ea, &dElem, elem);

    ostm << QString("dElemA\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

//bi
    for(int l=0; l<6; l++) Eb[l]=0;
    lsm(6, sz, Eb, bi, Lb, uweB, Db);

    //ostm << QString("Eb: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Eb[0], 12, 'f', 7).arg(Eb[1], 12, 'f', 7).arg(Eb[2], 12, 'f', 7).arg(Eb[3], 12, 'f', 7).arg(Eb[4], 12, 'f', 7).arg(Eb[5], 12, 'f', 7);
    //elemI = impObj->impOrb.elem;

    E2elem_pqr(Eb, &dElem, elem);

    ostm << QString("dElemB\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

//C
    for(int l=0; l<6; l++) Ec[l]=0;
    //for(int l=0; l<sz*2; l++) Wc[l]=1.0;
    lsm(6, sz*2, Ec, C, Lc, uweC, Dc);
    //slsm(6, sz*2, Ec, C, Lc);

    //ostm << QString("uweC: %1\n").arg(uweC);

    //ostm << QString("Ec: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Ec[0], 12, 'f', 7).arg(Ec[1], 12, 'f', 7).arg(Ec[2], 12, 'f', 7).arg(Ec[3], 12, 'f', 7).arg(Ec[4], 12, 'f', 7).arg(Ec[5], 12, 'f', 7);
//qDebug() << QString("Z: %1 +- %2\t%3 +- %4\t%5 +- %6\n").arg(rad2mas(Z[0]),12, 'f', 8).arg(rad2mas(sqrt(Dx[0][0])),12, 'f', 8).arg(rad2mas(Z[1]),12, 'f', 8).arg(rad2mas(sqrt(Dx[1][1])),12, 'f', 8).arg(rad2mas(Z[2]),12, 'f', 8).arg(rad2mas(sqrt(Dx[2][2])),12, 'f', 8);

    double ecSum = 0.0;
    for(int l=0; l<6; l++) ecSum += fabs(Ec[l]);
   //ostm << QString("uweC: %1\tecSum:%2\n").arg(uweC).arg(ecSum);

    E2elem_pqr(Ec, &dElem, elem);

    ostm << QString("dElemC\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);
/*
    dElem->ecc = elem->ecc - dElem.ecc;
    dElem->eJD = elem->eJD - dElem.eJD;
    dElem->inc = elem->inc - dElem.inc;
    dElem->M0 = elem->M0 - dElem.M0;
    dElem->Node = elem->Node - dElem.Node;
    dElem->q = elem->q - dElem.q;
    dElem->w = elem->w - dElem.w;*/
}

void detE1(inproveObject *impObj, observ &opos, orbElem *elem, orbElem &dElem)
{
    ostm << QString("detE1: low ecc\n");
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
    double H, K, r, ss, aval, nn, dT, pval;
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
    pval = aval*(1.0 - elem->ecc*elem->ecc);
//ostm << QString("nn: %1\n").arg(nn);
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

        //H = (1.0 + r/pval - 2.0/(1.0-elem->ecc*elem->ecc))/(elem->ecc);
        //K = pow(ka, -2.0)*aval*ss*(1.0+r/pval)/elem->ecc;//((r+pval)/pval)*(ss/(aval*aval*elem->ecc*nn))/nn;
        H = (r-aval*(1.0 + elem->ecc*elem->ecc))/(elem->ecc*aval*(1.0 - elem->ecc*elem->ecc));
        K = (aval*ss/(ka*ka*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));

        //K = (ss/(aval*aval*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
        dT = impSt->jdTDB - TDT2TDB(elem->eJD);
        //qDebug() << QString("%1\t%2\t%3\n").arg(aval).arg(H).arg(K);
        //P??
/*
        //P??
        P[0] = cos(elem->w)*cos(elem->Node) - sin(elem->w)*sin(elem->Node)*cos(elem->inc);
        P[1] = (cos(elem->w)*sin(elem->Node) + sin(elem->w)*cos(elem->Node)*cos(elem->inc))*cos(EKV) - sin(elem->w)*sin(elem->inc)*sin(EKV);
        P[2] = (cos(elem->w)*sin(elem->Node) + sin(elem->w)*cos(elem->Node)*cos(elem->inc))*sin(EKV) + sin(elem->w)*sin(elem->inc)*cos(EKV);
        //Q??
        Q[0] = -sin(elem->w)*cos(elem->Node) - cos(elem->w)*sin(elem->Node)*cos(elem->inc);
        Q[1] = (-sin(elem->w)*sin(elem->Node) + cos(elem->w)*cos(elem->Node)*cos(elem->inc))*cos(EKV) - cos(elem->w)*sin(elem->inc)*sin(EKV);
        Q[2] = (-sin(elem->w)*sin(elem->Node) + cos(elem->w)*cos(elem->Node)*cos(elem->inc))*sin(EKV) + cos(elem->w)*sin(elem->inc)*cos(EKV);
        //r
        R[0] = sin(elem->inc)*sin(elem->Node);
        R[1] = -sin(elem->inc)*cos(elem->Node)*cos(EKV) - cos(elem->inc)*sin(EKV);
        R[2] = -sin(elem->inc)*cos(elem->Node)*sin(EKV) + cos(elem->inc)*cos(EKV);
*/
        detPQR(P, Q, R, elem);
        //ostm << QString("proove R: %1 ?= 1\n").arg(R[0]*R[0] + R[1]*R[1] + R[2]*R[2]);

        detN(N, elem);

        //
        k1 = -sin(impSt->raC);///po;
        k2 = cos(impSt->raC);//po;
        k3 = -sin(impSt->decC)*cos(impSt->raC);//po;
        k4 = -sin(impSt->decC)*sin(impSt->raC);//po;
        k5 = cos(impSt->decC);//po;

//  lowecc
        detKE1(kEx, kEy, kEz, impSt->state, H, K, nn, dT);
        /*
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
        for(j=0;j<6;j++)
        {
            ai[i*6+j] = k1*kEx[j] + k2*kEy[j];
            bi[i*6+j] = k3*kEx[j] + k4*kEy[j] + k5*kEz[j];
            C[i*6+j] = ai[i*6+j];
            C[6*sz+i*6+j] = bi[i*6+j];
            //ostm << QString("ai: %1\nbi: %2\n").arg(ai[i*6+j]).arg(bi[i*6+j]);
        }

        La[i] = po*(impSt->raO - impSt->raC)*cos(impSt->decO);
        Lb[i] = po*(impSt->decO - impSt->decC);
        Lc[i] = La[i];
        Lc[sz+i] = Lb[i];

        //ostm << QString("La: %1\nLb: %2\n").arg(La[i]).arg(Lb[i]);

    }

//ai
    lsm(6, sz, Ea, ai, La, uweA, Da);

    ostm << QString("Ea: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Ea[0]).arg(Ea[1]).arg(Ea[2]).arg(Ea[3]).arg(Ea[4]).arg(Ea[5]);
    //elemI = impObj->impOrb.elem;

    //E2elem_psi(Ea, &dElem, elem);

    //ostm << QString("dElemA\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

//bi
    lsm(6, sz, Eb, bi, Lb, uweB, Db);

    ostm << QString("Eb: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Eb[0]).arg(Eb[1]).arg(Eb[2]).arg(Eb[3]).arg(Eb[4]).arg(Eb[5]);
    //elemI = impObj->impOrb.elem;

    //E2elem_psi(Eb, &dElem, elem);

    //ostm << QString("dElemB\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

//C
    for(int l=0; l<6; l++) Ec[l]=0;
    //for(int l=0; l<sz*2; l++) Wc[l]=1.0;
    lsm(6, sz*2, Ec, C, Lc, uweC, Dc);
    //slsm(6, sz*2, Ec, C, Lc);

    ostm << QString("Ec:\t%1\t%2\t%3\t%4\t%5\t%6\n").arg(Ec[0], 10, 'f', 7).arg(Ec[1], 10, 'f', 7).arg(Ec[2], 10, 'f', 7).arg(Ec[3], 10, 'f', 7).arg(Ec[4], 10, 'f', 7).arg(Ec[5], 10, 'f', 7);
    double dEc[6];
    for(j=0;j<6;j++)
    {
        dEc[j] = sqrt(Dc[j+j*6]);
    }
    ostm << QString("dEc:\t%1\t%2\t%3\t%4\t%5\t%6\n").arg(dEc[0], 10, 'f', 7).arg(dEc[1], 10, 'f', 7).arg(dEc[2], 10, 'f', 7).arg(dEc[3], 10, 'f', 7).arg(dEc[4], 10, 'f', 7).arg(dEc[5], 10, 'f', 7);


    double ecSum = 0.0;
    for(int l=0; l<6; l++) ecSum += fabs(Ec[l]);
   ostm << QString("uweC: %1\tecSum:%2\n").arg(uweC).arg(ecSum);
    //for(int l=0; l<6; l++) Ec[l]=0.0;

    E2elem_psi_low(Ec, &dElem, elem);

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

void detE2(inproveObject *impObj, observ &opos, orbElem *elem, orbElem &dElem)
{
    ostm << QString("detE2: elem tab48 low ecc\n");
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
    double H, K, r, ss, aval, nn, dT, pval;
    double psi[3];
    double tgw0, tgw1;
    double k1, k2, k3, k4, k5;
    double kEx[6], kEy[6], kEz[6];
    double kEx1[6], kEy1[6], kEz1[6];
    double PQ[3];
    improveState* impSt;
    QStringList aiList, biList;

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

    double Eval;
    aval = elem->q/(1.0-elem->ecc);
    //nn = grad2rad(kaGRAD*pow(aval, -1.5));
    nn = ka/pow(aval, 1.5);
    pval = aval*(1.0 - elem->ecc*elem->ecc);
//ostm << QString("pval: %1\n").arg(pval);
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

        //H = (r + pval - 2.0*aval)/(elem->ecc*pval);
        //K = ((r+pval)/pval)*(ss/(aval*aval*elem->ecc*nn))/nn;
        H = (r-aval*(1.0 + elem->ecc*elem->ecc))/(elem->ecc*aval*(1.0 - elem->ecc*elem->ecc));
        K = (aval*ss/(ka*ka*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
        //H = (1.0 + r/pval - 2.0/(1.0-elem->ecc*elem->ecc))/(elem->ecc);
        //K = pow(ka, -2.0)*aval*ss*(1.0+r/pval)/elem->ecc;

        //K = (ss/(aval*aval*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
        dT = impSt->jdTDB - TDT2TDB(elem->eJD);
        //ostm << QString("%1\t%2\t%3\n").arg(aval).arg(H).arg(K);
        //ostm << QString("dT\t%1\n").arg(dT);

        //eKepler(&Eval, nn*dT+elem->M0, elem->ecc);
        //ostm << QString("proove vel: %1 ?= %2\n").arg(ss/nn).arg(aval*aval*elem->ecc*sin(Eval));

/*        //proove??
        tgw0 = tan(elem_ekv->w);
        tgw1 = atan2((P[1]/cos(EKV) - P[0]*tan(elem_ekv->Node)), (Q[1]/cos(EKV) - Q[0]*tan(elem_ekv->Node)));
        //ostm << QString("%1 ?= %2\n").arg(elem_ekv->w).arg(tgw1);
        */
        //
        k1 = -sin(impSt->raC);//po;
        k2 = cos(impSt->raC);//po;
        k3 = -sin(impSt->decC)*cos(impSt->raC);//po;
        k4 = -sin(impSt->decC)*sin(impSt->raC);//po;
        k5 = cos(impSt->decC);//po;

        //detKE2(kEx, kEy, kEz, impSt->state, H, K, nn, dT, elem);

         detPQR(P, Q, R, elem);
         detN(N, elem);
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
            //kEx[5] = -impSt->state[3]/nn - impSt->state[1];


            kEy[0] = impSt->state[0]*cos(EKV);
            kEy[1] = -impSt->state[4]/nn + R[2]*impSt->state[0] - R[0]*impSt->state[2];
            kEy[2] = N[2]*impSt->state[0] - N[0]*impSt->state[2];

            kEy[3] = impSt->state[1] - 1.5*dT*impSt->state[4];
            kEy[4] = H*impSt->state[1]+K*impSt->state[4];
            kEy[5] = impSt->state[4]/nn;
            //kEy[5] = -impSt->state[4]/nn + impSt->state[0];


            kEz[0] = impSt->state[0]*sin(EKV);
            kEz[1] = -impSt->state[5]/nn + R[0]*impSt->state[1] - R[1]*impSt->state[0];
            kEz[2] = N[0]*impSt->state[1] - N[1]*impSt->state[0];

            kEz[3] = impSt->state[2] - 1.5*dT*impSt->state[5];
            kEz[4] = H*impSt->state[2]+K*impSt->state[5];
            kEz[5] = impSt->state[5]/nn;

            //ostm << QString("dNode: %1\n").arg(kEx[0]*impSt->state[0] + kEy[0]*impSt->state[1] + kEz[0]*impSt->state[2]);
            //ostm << QString("dw: %1\n").arg(kEx[1]*impSt->state[0] + kEy[1]*impSt->state[1] + kEz[1]*impSt->state[2]);
            //ostm << QString("dinc: %1\n").arg(kEx[2]*impSt->state[0] + kEy[2]*impSt->state[1] + kEz[2]*impSt->state[2]);
            //kEz[5] = -impSt->state[5]/nn;
            /*
//////////////////////////////////////  substitution
            kEx1[0] = (kEx[1] - kEx[0])/sin(elem->inc);
            kEx1[1] = (kEx[0] - kEx[5])/elem->ecc;
            kEx1[2] = kEx[2];
            kEx1[3] = kEx[3];
            kEx1[4] = kEx[4];
            kEx1[5] = kEx[5];

            kEy1[0] = (kEy[1] - kEy[0])/sin(elem->inc);
            kEy1[1] = (kEy[0] - kEy[5])/elem->ecc;
            kEy1[2] = kEy[2];
            kEy1[3] = kEy[3];
            kEy1[4] = kEy[4];
            kEy1[5] = kEy[5];

            kEz1[0] = (kEz[1] - kEz[0])/sin(elem->inc);
            kEz1[1] = (kEz[0] - kEz[5])/elem->ecc;
            kEz1[2] = kEz[2];
            kEz1[3] = kEz[3];
            kEz1[4] = kEz[4];
            kEz1[5] = kEz[5];

///////////////////////////////////////
*/
        //aiList.clear(); biList.clear();
        for(j=0;j<6;j++)
        {
            ai[i*6+j] = k1*kEx[j] + k2*kEy[j];
            bi[i*6+j] = k3*kEx[j] + k4*kEy[j] + k5*kEz[j];
            C[i*6*2+j] = ai[i*6+j];
            C[i*6*2+6+j] = bi[i*6+j];
            //ostm << QString("ai: %1\tbi: %2\n").arg(ai[i*6+j]).arg(bi[i*6+j]);
            //aiList << QString("%1").arg(ai[i*6+j], 15, 'f', 10);
            //biList << QString("%1").arg(bi[i*6+j], 15, 'f', 10);
        }

        La[i] = po*(impSt->raO - impSt->raC)*cos(impSt->decO);
        Lb[i] = po*(impSt->decO - impSt->decC);
        Lc[i*2] = La[i];
        Lc[i*2+1] = Lb[i];

        //ostm << QString("La: %1\nLb: %2\n").arg(La[i]).arg(Lb[i]);
        //ostm << QString("La: %1 = %2\n").arg(aiList.join(" + ")).arg(La[i]);
        //ostm << QString("Lb: %1 = %2\n").arg(biList.join(" + ")).arg(Lb[i]);
    }
/*
//ai
    lsm(6, sz, Ea, ai, La, uweA, Da);

    //ostm << QString("uweA: %1\n").arg(uweA);
    //ostm << QString("Ea: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Ea[0], 12, 'f', 7).arg(Ea[1], 12, 'f', 7).arg(Ea[2], 12, 'f', 7).arg(Ea[3], 12, 'f', 7).arg(Ea[4], 12, 'f', 7).arg(Ea[5], 12, 'f', 7);
    //elemI = impObj->impOrb.elem;

    E2elem_low(Ea, &dElem, elem);

    //ostm << QString("dElemA\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

//bi
    //slsm(6, sz, Eb, bi, Lb);
    lsm(6, sz, Eb, bi, Lb, uweB, Db);

    //ostm << QString("uweB: %1\n").arg(uweB);
    //ostm << QString("Eb: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Eb[0], 12, 'f', 7).arg(Eb[1], 12, 'f', 7).arg(Eb[2], 12, 'f', 7).arg(Eb[3], 12, 'f', 7).arg(Eb[4], 12, 'f', 7).arg(Eb[5], 12, 'f', 7);
    //elemI = impObj->impOrb.elem;

    E2elem_low(Eb, &dElem, elem);

    //ostm << QString("dElemB\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);
*/
//C
    for(int l=0; l<6; l++) Ec[l]=0;
    //for(int l=0; l<sz*2; l++) Wc[l]=1.0;
    lsm(6, sz*2, Ec, C, Lc, uweC, Dc);
    //for(int l=0; l<6; l++) Ec[l]/=1000;
    //slsm(6, sz*2, Ec, C, Lc);

    //ostm << QString("uweC: %1\n").arg(uweC);
    /*double Ai;
    for(i=0; i<sz*2; i++)
    {
        Ai = 0.0;
        for(j=0;j<6;j++)
        {
            Ai += C[i*6+j]*Ec[j];

        }
        ostm << QString("Lai: %1 - %2 = %3\n").arg(Lc[i]).arg(Ai).arg(fabs(Lc[i]-Ai));
//        La[i] = po*(impSt->raO - impSt->raC)*cos(impSt->decC);
    }
*/
    ostm << QString("Ec: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Ec[0], 12, 'f', 7).arg(Ec[1], 12, 'f', 7).arg(Ec[2], 12, 'f', 7).arg(Ec[3], 12, 'f', 7).arg(Ec[4], 12, 'f', 7).arg(Ec[5], 12, 'f', 7);

    double dEc[6];
    for(j=0;j<6;j++)
    {
        dEc[j] = Dc[j+j*6];
    }
    ostm << QString("dEc: %1\t%2\t%3\t%4\t%5\t%6\n").arg(dEc[0]).arg(dEc[1]).arg(dEc[2]).arg(dEc[3]).arg(dEc[4]).arg(dEc[5]);

    double ecSum = 0.0;
    for(int l=0; l<6; l++) ecSum += fabs(Ec[l]);
   ostm << QString("uweC: %1\tecSum:%2\n").arg(uweC).arg(ecSum);
    //for(int l=0; l<6; l++) Ec[l]=0.0;

    E2elem_low(Ec, &dElem, elem);

    ostm << QString("dElemC\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

    //ostm << QString("dElemC\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(rad2grad(dElem.M0), 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(rad2grad(dElem.inc), 10, 'f', 7).arg(rad2grad(dElem.w), 10, 'f', 7).arg(rad2grad(dElem.Node), 10, 'f', 7);
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
    ostm << QString("detE3: elem tab47 arb ecc\n");
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
    double H, K, r, ss, aval, nn, dT, pval;
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
    pval = aval*(1.0 - elem->ecc*elem->ecc);

    QStringList aiList, biList;

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

        //ostm << QString("stateE: %1\t%2\t%3\t%4\t%5\t%6\n").arg(stateE[0]).arg(stateE[1]).arg(stateE[2]).arg(stateE[3]).arg(stateE[4]).arg(stateE[5]);

        r = sqrt(impSt->state[0]*impSt->state[0] + impSt->state[1]*impSt->state[1] + impSt->state[2]*impSt->state[2]);
        ss = impSt->state[0]*impSt->state[3] + impSt->state[1]*impSt->state[4] + impSt->state[2]*impSt->state[5];
        po = sqrt(stateE[0]*stateE[0] + stateE[1]*stateE[1] + stateE[2]*stateE[2]);

        //ostm << QString("po: %1\n").arg(po);
        //H = (r + pval - 2.0*aval)/(elem->ecc*pval);
        //K = ((r+pval)/pval)*(ss/(aval*aval*elem->ecc*nn))/nn;
        H = (r-aval*(1.0 + elem->ecc*elem->ecc))/(elem->ecc*aval*(1.0 - elem->ecc*elem->ecc));
        K = (aval*ss/(ka*ka*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
        //K = (ss/(aval*aval*elem->ecc))*(1.0 + r/(aval*(1.0 - elem->ecc*elem->ecc)));
        dT = impSt->jdTDB - TDT2TDB(elem->eJD);
        //qDebug() << QString("%1\t%2\t%3\n").arg(aval).arg(H).arg(K);
        //P??
        detPQR(P, Q, R, elem);
/*        //P??
        P[0] = cos(elem->w)*cos(elem->Node) - sin(elem->w)*sin(elem->Node)*cos(elem->inc);
        P[1] = (cos(elem->w)*sin(elem->Node) + sin(elem->w)*cos(elem->Node)*cos(elem->inc))*cos(EKV) - sin(elem->w)*sin(elem->inc)*sin(EKV);
        P[2] = (cos(elem->w)*sin(elem->Node) + sin(elem->w)*cos(elem->Node)*cos(elem->inc))*sin(EKV) + sin(elem->w)*sin(elem->inc)*cos(EKV);
        //Q??
        Q[0] = -sin(elem->w)*cos(elem->Node) - cos(elem->w)*sin(elem->Node)*cos(elem->inc);
        Q[1] = (-sin(elem->w)*sin(elem->Node) + cos(elem->w)*cos(elem->Node)*cos(elem->inc))*cos(EKV) - cos(elem->w)*sin(elem->inc)*sin(EKV);
        Q[2] = (-sin(elem->w)*sin(elem->Node) + cos(elem->w)*cos(elem->Node)*cos(elem->inc))*sin(EKV) + cos(elem->w)*sin(elem->inc)*cos(EKV);
        //r
        R[0] = sin(elem->inc)*sin(elem->Node);
        R[1] = -sin(elem->inc)*cos(elem->Node)*cos(EKV) - cos(elem->inc)*sin(EKV);
        R[2] = -sin(elem->inc)*cos(elem->Node)*sin(EKV) + cos(elem->inc)*cos(EKV);
*/
        //ostm << QString("proove R: %1 ?= 1\n").arg(R[0]*R[0] + R[1]*R[1] + R[2]*R[2]);
        detN(N, elem);
/*
        N[0] = cos(elem->Node);
        N[1] = sin(elem->Node)*cos(EKV);
        N[2] = sin(elem->Node)*sin(EKV);*/
        //
        k1 = -sin(impSt->raC);///po;
        k2 = cos(impSt->raC);///po;
        k3 = -sin(impSt->decC)*cos(impSt->raC);///po;
        k4 = -sin(impSt->decC)*sin(impSt->raC);///po;
        k5 = cos(impSt->decC);//po;

        detKE3(kEx, kEy, kEz, impSt->state, H, K, nn, dT, elem);
        /*
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
*/

            //aiList.clear(); biList.clear();
        for(j=0;j<6;j++)
        {
            ai[i*6+j] = k1*kEx[j] + k2*kEy[j];
            bi[i*6+j] = k3*kEx[j] + k4*kEy[j] + k5*kEz[j];
            C[i*6+j] = ai[i*6+j];
            C[6*sz+i*6+j] = bi[i*6+j];
            //aiList << QString("%1").arg(ai[i*6+j], 15, 'f', 10);
            //biList << QString("%1").arg(bi[i*6+j], 15, 'f', 10);
            //ostm << QString("ai: %1\nbi: %2\n").arg(bi[i*6+j]);
        }

        La[i] = po*(impSt->raO - impSt->raC)*cos(impSt->decC);
        Lb[i] = po*(impSt->decO - impSt->decC);
        Lc[i] = La[i];
        Lc[sz+i] = Lb[i];

        //ostm << QString("La: %1 = %2\n").arg(aiList.join(" + ")).arg(La[i]);
        //ostm << QString("Lb: %1 = %2\n").arg(biList.join(" + ")).arg(Lb[i]);

        //ostm << QString("dpar0: %1\t\t%2\n\n").arg(ai[i*6+2]).arg(La[i]/0.000001);
        //ostm << QString("dpar1: %1\t\t%2\n\n").arg(bi[i*6+2]).arg(Lb[i]/0.000001);

        //ostm << QString("La: %1\nLb: %2\n").arg(La[i]).arg(Lb[i]);

    }
//C
    for(int l=0; l<6; l++) Ec[l]=0;
    //for(int l=0; l<sz*2; l++) Wc[l]=1.0;
    lsm(6, sz*2, Ec, C, Lc, uweC, Dc);
    //for(int l=0; l<6; l++) Ec[l]=mas2rad(Ec[l]);
    //slsm(6, sz*2, Ec, C, Lc);

    //ostm << QString("uweC: %1\n").arg(uweC);

    //ostm << QString("Ec: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Ec[0]).arg(Ec[1]).arg(Ec[2]).arg(Ec[3]).arg(Ec[4]).arg(Ec[5]);
    //ostm << QString("dEc: %1\t%2\t%3\t%4\t%5\t%6\n").arg(Dc[0]).arg(Dc[6*1+1]).arg(Dc[6*2+2]).arg(Dc[6*3+3]).arg(Dc[6*4+4]).arg(Dc[6*5+5]);

    double ecSum = 0.0;
    for(int l=0; l<6; l++) ecSum += fabs(Ec[l]);
   ostm << QString("uweC: %1\tecSum:%2\n").arg(uweC).arg(ecSum);

    E2elem_arb(Ec, &dElem, elem);

    ostm << QString("dElemC\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(dElem.M0, 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(dElem.inc, 10, 'f', 7).arg(dElem.w, 10, 'f', 7).arg(dElem.Node, 10, 'f', 7);

    //ostm << QString("dElemC\t\t: %1\t%2\t%3\t%4\t%5\t%6\t%7\n\n").arg(dElem.eJD, 15, 'f', 8).arg(rad2grad(dElem.M0), 10, 'f', 7).arg(dElem.q, 10, 'f', 7).arg(dElem.ecc, 10, 'f', 7).arg(rad2grad(dElem.inc), 10, 'f', 7).arg(rad2grad(dElem.w), 10, 'f', 7).arg(rad2grad(dElem.Node), 10, 'f', 7);
/*
    elemI->ecc = elem->ecc - dElem.ecc;
    elemI->eJD = elem->eJD - dElem.eJD;
    elemI->inc = elem->inc - dElem.inc;
    elemI->M0 = elem->M0 - dElem.M0;
    elemI->Node = elem->Node - dElem.Node;
    elemI->q = elem->q - dElem.q;
    elemI->w = elem->w - dElem.w;*/
}

void get2Ddisp(double &z0, double &z1)
{
    double x, y, s, lns;
    //srand(time(NULL));
/////////////   dispersion  ////////////////
        do
        {
    //           srand(time(NULL));
            x = 2.0*rand()/(1.0*RAND_MAX) - 1.0;
            y = 2.0*rand()/(1.0*RAND_MAX) - 1.0;
            s = x*x + y*y;
        }while((s==0)||(s>1));
        lns = sqrt(-2.0*log(s)/s);
        z0 = x*lns;
        z1 = y*lns;

 //       zStm << QString("%1|%2|%3|%4|%5|%6\n").arg(x).arg(y).arg(s).arg(lns).arg(z0).arg(z1);

////////////////////////////////////////////
}
