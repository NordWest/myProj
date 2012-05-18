#include <QtCore/QCoreApplication>
//#include <QApplication>

#include "./../libs/dele.h"
//#include "./../libs/orbcat.h"
//#include "./../libs/observ.h"
//#include "./../libs/dele.h"
//#include "./../libs/orbit.h"
#include "./../libs/skyarea.h"
#include "./../libs/astro.h"
#include "./../libs/comfunc.h"
#include "./../libs/rada.h"
#include "./../libs/redStat.h"
#include "./../libs/mpcs.h"

#include <QDebug>
#include <QSettings>
#include <QDataStream>


static QDataStream* clog0 = 0;
void customMessageHandler(QtMsgType type, const char* msg)
{
    static const char* msgType[] =
    {
        "Debug    : ",
        "Warning  : ",
        "Critical : ",
        "Fatal    : "
    };

    static QTextStream cout(stdout);
    static QTextStream cerr(stderr);

    cerr << msgType[type] << msg << endl;
    if(clog0 && clog0->device())
        *clog0 << type << msg;
    if(type == QtFatalMsg)
    {
        cerr << "aborting..." << endl;

#if defined(Q_CC_MSVC) && defined(QT_DEBUG) && defined(_CRT_ERROR) && defined(_DEBUG)
        int reportMode = _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
        _CrtSetReportMode(_CRT_ERROR, reportMode);
        int ret = _CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, QT_VERSION_STR, msg);
        if(ret == 0 && reportMode & _CRTDBG_MODE_WNDW)
            return;
        else if(ret == 1)
            _CrtDbgBreak();
#endif

#if defined(Q_OS_UNIX) && defined(QT_DEBUG)
        abort();
#else
        exit(1);
#endif
    }
}

int nofzbody;
dele *nbody;
ever_params *eparam;

int Sint(double X[], double V[]);
int LFint(double X[], double V[]);

int main(int argc, char *argv[])
{
    qInstallMsgHandler(customMessageHandler);
    QCoreApplication a(argc, argv);


    QFile* logFile = new QFile("messages.log");
        if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
            clog0 = new QDataStream(logFile);
        QTextStream logstr(logFile);

        //setlocale(LC_ALL, 0);
        //QLocale::setDefault(QLocale::German);
        setlocale(LC_NUMERIC, "C");


        int i, j, k, N, komp, teloi, teloj;
        //double k2 = 6.672590000e-8;
//	N = (mass->n-1)*3;
        //nofzbody = 10;

        //int Ni = nofzbody*3;
        //int Nj = 10*3;
        double Ri, Rj, *D, res0, res1, *Y, *VY, Vi;
        double TS;
        double ra, de;

        //D = new double[Nj];
        //Y = new double[Nj];
        //VY = new double[Nj];
        //QSettings sett("testNb.ini", QSettings::IniFormat);
        QString jplFile = "./../../data/cats/binp1940_2020.405";


        eqFile eqRes;
        ocRec *eqRec;


        TS = 2444208.50;

        nbody = new dele();

        //int iniHeadRes = nbody->init_header("./../../data/cats/header1940_2020.405");
        //int iniJplRes = nbody->init("./../../data/cats/binp1940.405");
        int iniJplRes = nbody->init(jplFile.toAscii().data());
        //int iniJplRes = nbody->init("./../../../data/cats/bin1940_2020.win.405");
        qDebug() << QString("iniJplRes: %1\n").arg(iniJplRes);
        if(iniJplRes) return 1;

        eparam = new ever_params;

        QSettings *esett = new QSettings("./paramsEv.ini", QSettings::IniFormat);
        eparam->NOR = esett->value("NOR", 17).toInt();
        eparam->NCLASS = esett->value("NCLASS", -2).toInt();
        eparam->NI = esett->value("NI", 2).toInt();
        eparam->NV = esett->value("NV", 6).toInt();
        eparam->LL = esett->value("LL", 6).toInt();
        eparam->XL = esett->value("XL", 0.0).toDouble();
        //eparam->t0 = esett->value("t0", 0.0).toDouble();
        //eparam->te = esett->value("te", 3.0e+3).toDouble();
        //eparam->stp = esett->value("stp", 100.0).toDouble();
        //eparam->shag = esett->value("shag", 10.0).toDouble();
        eparam->col = esett->value("col", 0.0015).toDouble();
        eparam->vout = esett->value("vout", 1000.0).toDouble();
        strncpy(&eparam->jkeys[0], esett->value("jkeys", "1111111111").toString().toAscii().data(), 10);

        //ever1(eparam, "./ini.lst", "./ini.cat", "./res.lst", 2456208.50);


        double mass[11] = {1.0, 5983000.0, 408522.0, 328900.1, 3098700.0, 1047.3908, 3499.2, 22930.0, 19260.0, 1812000.0, 0};

        int pla[11] = {
                SUN_NUM, MERCURY_NUM, VENUS_NUM, GEOCENTR_NUM, MARS_NUM, JUPITER_NUM, SATURN_NUM, URANUS_NUM, NEPTUNE_NUM, PLUTO_NUM, MOON_NUM
        };

        QFile resFile("res.txt");
        resFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
        QTextStream resStm(&resFile);
        resStm << QString("0.0|0.0|0.0|0.0|0.0|0.0|0.0|0.0|0\n");
        //resFile.close();

        double t0, t1, ti, dt;

        t0 = 2455201.0;
        t1 = t0+300;
        dt = (t1-t0)/20.0;


        qDebug() << QString("first epoch: %1\t%2\n").arg(t0, 10, 'f', 2).arg(getStrFromDATEOBS(getDATEOBSfromMJD(jd2mjd(t0)), ":", 0, 3));
        qDebug() << QString("end epoch: %1\t%2\n").arg(t1, 10, 'f', 2).arg(getStrFromDATEOBS(getDATEOBSfromMJD(jd2mjd(t1)), ":", 0, 3));
        qDebug() << QString("step: %1\t%2\n").arg(dt);


        double *X, *V;
        //double *X0, *V0;

        int nb = 10;//iList->nstr;
        int objNum = 5;
        eparam->NV = (nb+objNum)*3;
        N = eparam->NV;
        X = new double[N];
        V = new double[N];
        //X0 = new double[N];
        //V0 = new double[N];
        //r = new double[3];
        //nofzbody = 1;

        double TI, TF;

        nofzbody = nb+objNum;

        Everhardt *sun;
        sun = new Everhardt(N, eparam->NCLASS, eparam->NOR, eparam->NI, eparam->LL, eparam->XL);
/*
        nbody->detR(&X[0], &X[1], &X[2], t0, GEOCENTR_NUM, 0, CENTER_BARY, SK_ECLIPTIC);
        nbody->detR(&V[0], &V[1], &V[2], t0, GEOCENTR_NUM, 1, CENTER_BARY, SK_ECLIPTIC);
*/


/////////

        observ *opos = new observ();
        int oires = opos->init("./../../data/cats/Obs.txt", jplFile.toAscii().data());

        if(oires)
        {
            qDebug() << QString("observ init error:%1").arg(oires);
            return 1;
        }
        opos->set_obs_parpam(GEOCENTR_NUM, CENTER_SUN, SK_EKVATOR, "084");

        IList *iList;
        OrbCat *iCat;
        RList *rList;
        RLRecord *rlRec;
        orbit *iOrb;
        QString file_ilist, file_icat, file_ires;

        double *r = new double[3];

        file_ilist = QString("./ini.lst");
        file_icat = QString("./ini.cat");
        file_ires = QString("./res.lst");

        iList = new IList();
        iList->init(file_ilist.toAscii().data());
        rList = new RList();
        rList->init(file_ires.toAscii().data());
        iCat = new OrbCat();
        iCat->is_buff = 1;
        iCat->init(file_icat.toAscii().data());
        int icNum = iCat->nstr;
        iOrb = new orbit();



        opos->det_observ(t0);


        k=0;

        for(i=0; i<objNum&&i<icNum; i++)
        {
            iCat->GetRec(i);
            iOrb->get(iCat);
            //iOrb->detRecEkv(&x, &y, &z, iCat->record->eJD);
            iOrb->detRecEkv(&X[(nb+i)*3], &X[(nb+i)*3+1], &X[(nb+i)*3+2], t0);
            iOrb->detRecEkvVel(&V[(nb+i)*3], &V[(nb+i)*3+1], &V[(nb+i)*3+2], t0);

            r[0] = X[(nb+i)*3];
            r[1] = X[(nb+i)*3+1];
            r[2] = X[(nb+i)*3+2];

            rlRec = new RLRecord;

            Ri = sqrt(X[(nb+i)*3]*X[(nb+i)*3] + X[(nb+i)*3+1]*X[(nb+i)*3+1] + X[(nb+i)*3+2]*X[(nb+i)*3+2]);
            resStm << QString("%1|%2|%3|%4|%5|%6|%7|2\n").arg(X[(nb+i)*3]).arg(X[(nb+i)*3+1]).arg(X[(nb+i)*3+2]).arg(Ri).arg(V[(nb+i)*3]).arg(V[(nb+i)*3+1]).arg(V[(nb+i)*3+2]);
            //RotX(r, -EKV);
            detRDnumGC(&rlRec->ra, &rlRec->dec, r[0], r[1], r[2], opos->ox, opos->oy, opos->oz, opos->obs->dcx, opos->obs->dcy, opos->obs->dcz);
            qDebug() << QString("RA %1\tDEC %2\n").arg(rlRec->ra).arg(rlRec->dec);
            qDebug() << QString("%1: %2\t%3\t%4\n").arg(iList->record->name).arg(getStrFromDATEOBS(getDATEOBSfromMJD(jd2mjd(t0)), ":", 0, 3)).arg(mas_to_hms(rad_to_mas(rlRec->ra), " ", 3)).arg(mas_to_damas(rad_to_mas(rlRec->dec), " ", 3));

            qDebug() << QString("%1: %2 \t %3 \t %4\n").arg(i).arg(X[(nb+i)*3]).arg(X[(nb+i)*3+1]).arg(X[(nb+i)*3+2]);
            qDebug() << QString("  : %1 \t %2 \t %3\t %4\n").arg(V[(nb+i)*3]).arg(V[(nb+i)*3+1]).arg(V[(nb+i)*3+2]).arg(sqrt(V[(nb+i)*3]*V[(nb+i)*3] + V[(nb+i)*3+1]*V[(nb+i)*3+1] + V[(nb+i)*3+2]*V[(nb+i)*3+2])*AUKM/86400.0);

            rList->AddRec(rlRec);

            eqRec = new ocRec;
            eqRec->ra = rad2grad(rlRec->ra);
            eqRec->de = rad2grad(rlRec->dec);
            eqRec->MJday = jd2mjd(t0);
            eqRec->mag0 = 0;
            iCat->GetRec(k);
            eqRec->name = iCat->record->name;
            eqRes.ocList << eqRec;
            k++;
        }


        //t0 = iCat->record->eJD;








/////////

        for(teloi=0, i=0; teloi<nb; teloi++, i+=3)
        {
            nbody->detR(&X[i+0], &X[i+1], &X[i+2], t0, pla[teloi], 0, CENTER_BARY, SK_EKVATOR);
            nbody->detR(&V[i+0], &V[i+1], &V[i+2], t0, pla[teloi], 1, CENTER_BARY, SK_EKVATOR);


                Ri = sqrt(X[i+0]*X[i+0] + X[i+1]*X[i+1] + X[i+2]*X[i+2]);
                Vi = sqrt(V[i+0]*V[i+0] + V[i+1]*V[i+1] + V[i+2]*V[i+2])*AUKM/86400.0;
                resStm << QString("%1|%2|%3|%4|%5|%6|%7|1\n").arg(X[i]).arg(X[i+1]).arg(X[i+2]).arg(Ri).arg(V[i]).arg(V[i+1]).arg(V[i+2]);


            qDebug() << QString("teloi: %1\tplaNum= %2\tmass= %3\n").arg(teloi).arg(pla[teloi]).arg(mass[teloi]);
            qDebug() << QString("X: %1\t%2\t%3\t%4\n").arg(X[i+0]).arg(X[i+1]).arg(X[i+2]).arg(sqrt(X[i+0]*X[i+0] + X[i+1]*X[i+1] + X[i+2]*X[i+2]));//, t0, pla[teloi], 0, CENTER_SUN, SK_ECLIPTIC);
            qDebug() << QString("V: %1\t%2\t%3\t%4\n").arg(V[i+0]).arg(V[i+1]).arg(V[i+2]).arg(sqrt(V[i+0]*V[i+0] + V[i+1]*V[i+1] + V[i+2]*V[i+2])*AUKM/86400.0);
            //nbody->detR(&V[i+0], &V[i+1], &V[i+2], t0, pla[teloi], 1, CENTER_SUN, SK_ECLIPTIC);
/*
            X[i+0] *= AUSM;
            X[i+1] *= AUSM;
            X[i+2] *= AUSM;
            V[i+0] *= AUSM/86400.0;
            V[i+1] *= AUSM/86400.0;
            V[i+2] *= AUSM/86400.0;*/
        }

   /*     nbody->detR(&X0[0], &X0[1], &X0[2], t0, SUN_NUM, 0, CENTER_SUN, SK_EKVATOR);
        nbody->detR(&V0[0], &V0[1], &V0[2], t0, SUN_NUM, 1, CENTER_SUN, SK_EKVATOR);
        X[0] = X0[0];
        X[1] = X0[1];
        X[2] = X0[2];
        V[0] = V0[0];
        V[1] = V0[1];
        V[2] = V0[2];*/
/*
        t1 = (t1-t0)*86400.0;
        t0 = 0.0;
        dt *= 86400.0;


*/


        //t1 = t0+300;
        //dt = (t1-t0)/10.0;


        //resFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
        //resStm.setDevice(&resFile);


        for(ti=t0; ti<t1; ti+=dt)
        {


            //nbody->detR(&X0[0], &X0[1], &X0[2], ti+dt, GEOCENTR_NUM, 0, CENTER_SUN, SK_EKVATOR);
            //nbody->detR(&V0[0], &V0[1], &V0[2], ti+dt, GEOCENTR_NUM, 1, CENTER_SUN, SK_EKVATOR);
            qDebug() << QString("\njd: %1\ntime: %2\n").arg(ti, 12, 'f', 4).arg(getStrFromDATEOBS(getDATEOBSfromMJD(jd2mjd(ti)), ":", 0, 3));

            TI = ti;
            TF = ti+dt;

            sun->rada27(X, V, TI, TF);


            opos->det_observ(TF);


            //for(teloi=0, i=0; teloi<nb; teloi++, i+=3) resStm << QString("%1|%2|%3|%4|%5|%6\n").arg(X[i]/AUSM).arg(X[i+1]/AUSM).arg(X[i+2]/AUSM).arg(V[i]/AUSM*86400.0).arg(V[i+1]/AUSM*86400.0).arg(V[i+2]/AUSM*86400.0);

            for(teloi=0, i=0; teloi<=nb; teloi++, i+=3)
            {
                Ri = sqrt(X[i+0]*X[i+0] + X[i+1]*X[i+1] + X[i+2]*X[i+2]);
                Vi = sqrt(V[i+0]*V[i+0] + V[i+1]*V[i+1] + V[i+2]*V[i+2])*AUKM/86400.0;
                resStm << QString("%1|%2|%3|%4|%5|%6|%7|1\n").arg(X[i]).arg(X[i+1]).arg(X[i+2]).arg(Ri).arg(V[i]).arg(V[i+1]).arg(V[i+2]);
            }

            k=0;

            for(teloi=nb, i=nb*3; teloi<nb+objNum; teloi++, i+=3)
            {
                Ri = sqrt(X[i+0]*X[i+0] + X[i+1]*X[i+1] + X[i+2]*X[i+2]);
                Vi = sqrt(V[i+0]*V[i+0] + V[i+1]*V[i+1] + V[i+2]*V[i+2])*AUKM/86400.0;
                resStm << QString("%1|%2|%3|%4|%5|%6|%7|2\n").arg(X[i]).arg(X[i+1]).arg(X[i+2]).arg(Ri).arg(V[i]).arg(V[i+1]).arg(V[i+2]);
                qDebug() << QString("teloi: %1\tplaNum= %2\tmass= %3\n").arg(teloi).arg(pla[teloi]).arg(mass[teloi]);
                qDebug() << QString("X: %1\t%2\t%3\t%4\n").arg(X[i+0]).arg(X[i+1]).arg(X[i+2]).arg(Ri);//, t0, pla[teloi], 0, CENTER_SUN, SK_ECLIPTIC);
                qDebug() << QString("V: %1\t%2\t%3\t%4\n").arg(V[i+0]).arg(V[i+1]).arg(V[i+2]).arg(Vi);

                r[0] = X[i];
                r[1] = X[i+1];
                r[2] = X[i+2];

                eqRec = new ocRec;

                detRDnumGC(&ra, &de, r[0], r[1], r[2], opos->ox, opos->oy, opos->oz, opos->obs->dcx, opos->obs->dcy, opos->obs->dcz);

                eqRec->ra = rad2grad(ra);
                eqRec->de = rad2grad(de);

                eqRec->MJday = jd2mjd(TF);

                eqRec->mag0 = 0;
                iCat->GetRec(k);
                eqRec->name = iCat->record->name;
                eqRes.ocList << eqRec;
                k++;
            }
            //qDebug() << QString("%1|%2|%3|%4|%5|%6\n").arg(X[0]-X0[0]).arg(X[1]-X0[1]).arg(X[2]-X0[2]).arg(V[0]-V0[0]).arg(V[1]-V0[1]).arg(V[2]-V0[2]);
/*
            for(j=0, teloj=0; j<Nj; j+=3, teloj++)
            {
                    nbody->detR(&Y[j], &Y[j+1], &Y[j+2], ti, pla[teloj], 0, CENTER_BARY, SK_EKVATOR);
                    nbody->detR(&VY[j], &VY[j+1], &VY[j+2], ti, pla[teloj], 1, CENTER_BARY, SK_EKVATOR);
                    qDebug() << QString("plaNum= %1\tY:\t%2\t%3\t%4\t%5\n").arg(pla[teloj]).arg(Y[j]).arg(Y[j+1]).arg(Y[j+2]).arg(sqrt(Y[j]*Y[j] + Y[j+1]*Y[j+1] + Y[j+2]*Y[j+2]));
                    resStm << QString("%1|%2|%3|%4|%5|%6|%7\n").arg(pla[teloj]).arg(Y[j]).arg(Y[j+1]).arg(Y[j+2]).arg(VY[j]).arg(VY[j+1]).arg(VY[j+2]);
            }
            */


        }

        resFile.close();


        //mpcs mcsRes;
        //mpcrec mpcRec;
        QString tStr;

        QFile mpcFile("./mpc.txt");
        mpcFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
        QTextStream mpcStm(&mpcFile);


        for(i=0; i<eqRes.ocList.size();i++)
        {
            iCat->GetRecName(eqRes.ocList.at(i)->name.simplified().toAscii().data());
            eqRes.ocList.at(i)->rec2MPC(&tStr, "084", QString("%1").arg(iCat->record->number, 5, 10, QLatin1Char( '0' )));
            mpcStm << tStr << "\n";
        }

        mpcFile.close();

        eqRes.saveAs("eq.txt");



        rList->saveAs("res.lst");
    qInstallMsgHandler(0);
    logFile->close();
    return 0;

}
/*
int Sint(double X[], double V[])
{
        double c0, c1, c2, m;
        double Ex0, Ey0, Ez0, Ex1, Ey1, Ez1, Ex2, Ey2, Ez2, Ex3, Ey3, Ez3, Ex4, Ey4, Ez4;
        int i, N, teloi;

        N = (mass->n-1)*3;

        Ex0 = Ey0 = Ez0 = Ex1 = Ey1 = Ez1 = Ex2 = Ey2 = Ez2 = Ex3 = Ey3 = Ez3 = Ex4 = Ey4 = Ez4 = 0.0;
        m = 0.0;

        for(i=0, teloi=1; i<N; i+=3, teloi++)
        {
                Ex0 += mass->get(teloi)*(X[i+1]*V[i+2] - X[i+2]*V[i+1]);
                Ey0 += mass->get(teloi)*(X[i+2]*V[i+0] - X[i+0]*V[i+2]);
                Ez0 += mass->get(teloi)*(X[i+0]*V[i+1] - X[i+1]*V[i+0]);

                Ex1 += mass->get(teloi)*X[i+1];
                Ey1 += mass->get(teloi)*X[i+2];
                Ez1 += mass->get(teloi)*X[i];

                Ex2 += mass->get(teloi)*V[i+2];
                Ey2 += mass->get(teloi)*V[i];
                Ez2 += mass->get(teloi)*V[i+1];

                Ex3 += mass->get(teloi)*X[i+2];
                Ey3 += mass->get(teloi)*X[i];
                Ez3 += mass->get(teloi)*X[i+1];

                Ex4 += mass->get(teloi)*V[i+1];
                Ey4 += mass->get(teloi)*V[i+2];
                Ez4 += mass->get(teloi)*V[i];

                m += mass->get(teloi);
        }

        m += mass->get(0);

        c0 = Ex0 - (Ex1*Ex2 - Ex3*Ex4)/m;
        c1 = Ey0 - (Ey1*Ey2 - Ey3*Ey4)/m;
        c2 = Ez0 - (Ez1*Ez2 - Ez3*Ez4)/m;

        *si << c0 << " " << c1 << " " << c2 << " = " << sqrt(c0*c0 + c1*c1 + c2*c2) << endl;

        return 0;
}

int LFint(double X[], double V[])
{
        int i, j, N, teloi, teloj;
        double E0, E1, E2, E3, U, h, m;

        N = (mass->n-1)*3;

        m = U = E0 = E1 = E2 = E3 = 0.0;

        for(i=0, teloi=1; i<N; i+=3, teloi++)
        {
                E0 += mass->get(teloi)*(V[i]*V[i] + V[i+1]*V[i+1] + V[i+2]*V[i+2]);
                E1 += mass->get(teloi)*V[i];
                E1 += mass->get(teloi)*V[i+1];
                E1 += mass->get(teloi)*V[i+2];
                m += mass->get(teloi);
        }

        m += mass->get(0);

        for(i=0, teloi=1; i<N; i+=3, teloi++)
        {
                for(j=0, teloj=1; j<N; j+=3, teloj++)
                {
                        if(i!=j) U += (mass->get(teloi)*mass->get(teloj))/(sqrt((X[i] - X[j])*(X[i] - X[j]) + (X[i+1] - X[j+1])*(X[i+1] - X[j+1]) + (X[i+2] - X[j+2])*(X[i+2] - X[j+2])));
                }
                U += (mass->get(teloi)*mass->get(0))/(sqrt((X[i])*(X[i]) + (X[i+1])*(X[i+1]) + (X[i+2])*(X[i+2])));
        }
        U *= G/2.0;

        h = E0/2.0 - (E1*E1 + E2*E2 + E3*E3)/(2.0*m) - U;

        *lfi << h << endl;
}
*/

/*
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

 /*   QFile* logFile = new QFile("messages.log");
    if(logFile->open(QFile::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
        clog = new QDataStream(logFile);
    QTextStream logstr(logFile);/

    int i, j, k, num0, num1;
    double x, y, z, r, v, Vx, Vy, Vz, Vr, Vn;
    IList *iList;
    OrbCat *iCat;
    RList *rList;
    orbit *iOrb;
    int N;
    int nb;
    double *X, *V, *R;

    iList = new IList();
    iList->init("./ini.lst");
    rList = new RList();
    rList->init("./res.lst");
    iCat = new OrbCat();
    iCat->is_buff = 1;
    iCat->init("./ini.cat");
    iOrb = new orbit();

    observ *opos = new observ();
    int oires = opos->init("./../../../data/cats/Obs.txt", "./../../../data/cats/header1980_2000.405", "./../../../data/cats/binp1980_2000.405");
    if(oires)
    {
        qDebug() << QString("observ init error:%1").arg(oires);
        return 1;
    }
    opos->set_obs_parpam(SUN_NUM, CENTER_BARY, SK_ECLIPTIC, "084");

    nb = iList->nstr;
    N = nb*3;


    qDebug()<< QString("cat num: %1\n").arg(iCat->nstr);
    if(iCat->GetRec(0))qDebug() << QString("absent\n");
    double t0 = iCat->record->eJD;
    double t1, aelm, T, step, t;
    qDebug() << QString("first epoch: %1\t%2\n").arg(t0, 10, 'f', 2).arg(getStrFromDATEOBS(getDATEOBSfromMJD(jd2mjd(t0)), ":", 0, 3));

/*
     qDebug() << QString("inum %1\n").arg(i);
    if(iList->GetRec(0))
    {
        qDebug() << QString("iList[%1] error\n").arg(i);
        //continue;
    }
    qDebug() << QString("name: %1\n").arg(iList->record->name);
    if(iCat->GetRecName(iList->record->name)==-1)
    {
//				AfxMessageBox("c1");
        qDebug() << QString("%1 is absent\n").arg(0);
        //continue;
    }
/
    iOrb->get(iCat);
    aelm=iOrb->elem->q/(1.0 - iOrb->elem->ec);
    T = sqrt(pow(aelm, 3.0))*365.2425;
    qDebug() << QString("a= %1\tT= %2\n").arg(aelm).arg(T);
    step = T/100.0;
    t1 = t0+T;
    double x1, y1, z1;
    for(t=t0; t<t0+step; t+=step)
    {
        opos->det_observ(t);
        iOrb->detPolarOrb(&r, &v, t);
        iOrb->detPolarOrbVel(&Vr, &Vn, t);
        qDebug() << QString("%1: %2\t%3: %4\t%5\n").arg(t, 9, 'f', 2).arg(r, 10, 'f', 8).arg(rad2grad(v), 10, 'f', 8).arg(Vr, 16, 'f', 14).arg(Vn, 16, 'f', 14);
        iOrb->detRecEcl(&x, &y, &z, t);
        iOrb->detRecEclVel(&Vx, &Vy, &Vz, t);
        qDebug() << QString("%1: %2\t%3\t%4=%5: %6\t%7\t%8\n").arg(t, 9, 'f', 2).arg(x, 10, 'f', 8).arg(y, 10, 'f', 8).arg(z, 10, 'f', 8).arg(sqrt(x*x+y*y+z*z), 10, 'f', 8).arg(Vx, 16, 'f', 14).arg(Vy, 16, 'f', 14).arg(Vz, 16, 'f', 14);
        x1 = x - opos->ox;
        y1 = y - opos->oy;
        z1 = z - opos->oz;
        qDebug() << QString("sun:%1\t%2\t%3\n").arg(opos->ox, 10, 'f', 8).arg(opos->oy, 10, 'f', 8).arg(opos->oz, 10, 'f', 8);
        qDebug() << QString("r1: %2\t%3\t%4=%5\n").arg(x1, 10, 'f', 8).arg(y1, 10, 'f', 8).arg(z1, 10, 'f', 8).arg(sqrt(x1*x1+y1*y1+z1*z1), 10, 'f', 8);
    }

    int res;
    dele *inter = new dele();
    res = inter->init_header("./../../../data/cats/header.405");
    //res = inter->init_header("./../../../data/cats/header1980_2000.405");
    qDebug() << QString("res init_header: %1\n").arg(res);
    res = inter->init_jpl("./../../../data/cats/binp2000.405");
    //res = inter->init_jpl("./../../../data/cats/binp1980_2000.405");
    qDebug() << QString("res init_jpl: %1\n").arg(res);
    res = inter->detR(&x, &y, &z, 2451536.50, GEOCENTR_NUM, 0, CENTER_SUN, SK_ECLIPTIC);
    qDebug() << QString("res detR: %1\n").arg(res);

    qDebug() << QString("sun: %1\t%2\t%3\n").arg(x, 15, 'f', 12).arg(y, 15, 'f', 12).arg(z, 15, 'f', 12);

 /*
    for(i=0; i<nb;i++)
    {
        qDebug() << QString("inum %1\n").arg(i);
        if(iList->GetRec(i))
        {
            qDebug() << QString("iList[%1] error\n").arg(i);
            continue;
        }
        if(iCat->GetRecName(iList->record->name)==-1)
        {
//				AfxMessageBox("c1");
            qDebug() << QString("%1 is absent\n").arg(i);
            continue;
        }

        iOrb->get(iCat);
        //iOrb->detRecEkv(&x, &y, &z, iCat->record->eJD);
        iOrb->detRecEcl(&X[i*3], &X[i*3+1], &X[i*3+2], t0);
        iOrb->detRecEclVel(&V[i+3], &V[i*3+1], &V[i*3+2], t0);

        r[0] = X[i*3];
        r[1] = X[i*3+1];
        r[2] = X[i*3+2];
        //RotX(r, -EKV);
        detRDnumGC(&rList->record->RA, &rList->record->DEC, r[0], r[1], r[2], opos->ox, opos->oy, opos->oz, opos->obs->dcx, opos->obs->dcy, opos->obs->dcz);
        qDebug() << QString("RA %1\tDEC %2\n").arg(rList->record->RA).arg(rList->record->DEC);
        qDebug() << QString("%1: %2\t%3\t%4\n").arg(iList->record->name).arg(getStrFromDATEOBS(getDATEOBSfromMJD(jd2mjd(t0)), ":", 0, 3)).arg(mas_to_hms(rad_to_mas(rList->record->RA), " ", 3)).arg(mas_to_damas(rad_to_mas(rList->record->DEC), " ", 3));

        qDebug() << QString("%1: %2 \t %3 \t %4\n").arg(i).arg(X[i*3]).arg(X[i*3+1]).arg(X[i*3+2]);
        qDebug() << QString("  : %1 \t %2 \t %3\n").arg(V[i*3]).arg(V[i*3+1]).arg(V[i*3+2]);
    }
/


    //return a.exec();
    return 0;
}*/
