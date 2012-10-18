#include <QtCore/QCoreApplication>

#include "./../libs/dele.h"
//#include "./../libs/orbcat.h"
//#include "./../libs/observ.h"
//#include "./../libs/dele.h"
//#include "./../libs/orbit.h"
//#include "./../libs/skyarea.h"
#include "./../libs/astro.h"
#include "./../libs/comfunc.h"
//#include "./../libs/rada.h"
#include "./../libs/redStat.h"
#include "./../libs/mpcs.h"
#include "./../libs/ephem_util.h"
#include "./../libs/calc_epm.h"

#include <QDebug>
#include <QSettings>
#include <QDataStream>
#include <QDomDocument>

#define OMPLIB

#include <iostream>
#include <iomanip>

#include "./../libs/moody/moody.h"
#include "./../libs/moody/capsule/capsuleBase/particle/Particle.h"
#include "./../libs/moody/capsule/Capsule.h"
#include "./../libs/moody/capsule/capsuleBase/CapsuleBase.h"
#include "./../libs/myDomMoody.h"


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

int epm_planet_num(QString name)
{
    if(QString().compare(name, "MERCURY", Qt::CaseInsensitive)==0) return 1;
    if(QString().compare(name, "VENUS", Qt::CaseInsensitive)==0) return 2;
    if(QString().compare(name, "EARTH", Qt::CaseInsensitive)==0) return 3;
    if(QString().compare(name, "GEOCENTR", Qt::CaseInsensitive)==0) return 3;
    if(QString().compare(name, "MARS", Qt::CaseInsensitive)==0) return 4;
    if(QString().compare(name, "JUPITER", Qt::CaseInsensitive)==0) return 5;
    if(QString().compare(name, "SATURN", Qt::CaseInsensitive)==0) return 6;
    if(QString().compare(name, "URANUS", Qt::CaseInsensitive)==0) return 7;
    if(QString().compare(name, "NEPTUNE", Qt::CaseInsensitive)==0) return 8;
    if(QString().compare(name, "PLUTO", Qt::CaseInsensitive)==0) return 9;
    if(QString().compare(name, "MOON", Qt::CaseInsensitive)==0) return 10;
    if(QString().compare(name, "SUN", Qt::CaseInsensitive)==0) return 11;
    if(QString().compare(name, "SOL", Qt::CaseInsensitive)==0) return 11;
    if(QString().compare(name, "SSB", Qt::CaseInsensitive)==0) return 12;
    if(QString().compare(name, "SOLAR-SYSTEM BARYCENTER", Qt::CaseInsensitive)==0) return 12;
    if(QString().compare(name, "EMB", Qt::CaseInsensitive)==0) return 13;
    if(QString().compare(name, "EARTH-MOON BARYCENTER", Qt::CaseInsensitive)==0) return 13;
    return -1;
}

int nofzbody;
dele *nbody;
//ever_params *eparam;

//#define CENTER CENTER_BARY
//#define SK SK_EKVATOR

//int readCFG(QString fileName, QList<ParticleStruct *> &pList);
//int saveCFG(QString fileName, QList<ParticleStruct *> &pList);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    int SK, CENTER;

    QSettings *sett = new QSettings("./pnb.ini", QSettings::IniFormat);

    QString jplFile = sett->value("general/jplFile", "./../../data/cats/binp1940_2020.405").toString();
    QString epmDir = sett->value("general/epmDir", "./").toString();
    QString obsFile = sett->value("general/obsFile", "./../../data/cats/Obs.txt").toString();
    QString obsCode = sett->value("general/obsCode", "500").toString();
    QString confFile = sett->value("general/confFile", "testMajor.xml").toString();
    double time0 = sett->value("general/time0", 2455201.0).toDouble();
    int si = sett->value("general/si", 0).toInt();
    int useEPM = sett->value("general/useEPM", 0).toInt();

    SK = sett->value("general/sk", 0).toInt();
    CENTER = sett->value("general/center", 0).toInt();

    dele *nbody;
    nbody = new dele();
    int centr_num;
    int status;
    if(useEPM)
    {
        status = !InitTxt(epmDir.toAscii().data());
        centr_num = 11+!CENTER;
    }
    else
    {
        status = nbody->init(jplFile.toAscii().data());
    }

    if(status)
    {
        qDebug() << QString("init ephemeride error\n");
        return 1;
    }

    QList <ParticleStruct*> pList;

    if(readCFG(confFile, pList))
    {
        qDebug() << QString("readCFG error\n");
        return 1;
    }


    int i, sz;
    sz = pList.size();
    qDebug() << QString("pList: %1\n").arg(sz);

    //Capsule *experiment = new Capsule;


    QString name;
    int plaNum;

    double *X, *V;
    QVector <double*> xVect, vVect;

    QFile xyFile("xyData.txt");
    xyFile.open(QFile::Truncate | QFile::WriteOnly);
    QTextStream xyStm(&xyFile);

    double dist, vel;
    /*double mass[10];/* = {
                1.0, 5983000.0, 408522.0, 328900.1, 3098700.0, 1047.3908, 3499.2, 22930.0, 19260.0, 1812000.0, 0
    };/



    double gmass[10];

    double gms = nbody->headParam("GMS");
    gmass[9] = gms;
    mass[9] = 1.0;
    gmass[2] = nbody->headParam(QString("GMB").arg(i+1).toAscii().data());
    mass[2] = gms/gmass[2];
    for(i=0; i<9; i++)
    {
        if(i==2) continue;
        gmass[i] = nbody->headParam(QString("GM%1").arg(i+1).toAscii().data());
        mass[i] = gms/gmass[i];
    }

    for(i=0; i<10; i++)
    {
        if(si) mass[i] = SUN_MASS_KG/mass[i];
        qDebug() << QString("mass %1: %2 - %3\n").arg(i).arg(mass[i]).arg(gmass[i]);
    }
*/
    double coefX, coefXD;
    coefX = coefXD = 1.0;
    if(si)
    {
        coefX = AUKM*1000;
        coefXD = 1000*AUKM/SECINDAY;
    }


    for(i=0; i<sz; i++)
    {


        name = QString(pList[i]->name.data());
//        if(QString().compare(name, "Sol")==0) plaNum = 10;
        //else
        if(useEPM) plaNum = epm_planet_num(name);
        else plaNum = planet_num(name.toAscii().data());

        if(plaNum!=-1)
        {
            X = new double[3];
            V = new double[3];

            if(useEPM)
            {
                status = calc_EPM(plaNum, centr_num, (int)time0, time0 - (int)time0, X, V);
                 if(!status)
                 {
                     qDebug() << QString("error EPM\n");
                     return 1;
                 }
            }
            else
            {

                nbody->detR(&X[0], &X[1], &X[2], time0, plaNum, 0, CENTER, SK);
                nbody->detR(&V[0], &V[1], &V[2], time0, plaNum, 1, CENTER, SK);
            }
            xVect << X;
            vVect << V;
            dist = sqrt(X[0]*X[0] + X[1]*X[1] + X[2]*X[2]);
            vel = sqrt(V[0]*V[0] + V[1]*V[1] + V[2]*V[2]);
            pList[i]->x = coefX*X[0];
            pList[i]->y = coefX*X[1];
            pList[i]->z = coefX*X[2];
            pList[i]->xd = coefXD*V[0];
            pList[i]->yd = coefXD*V[1];
            pList[i]->zd = coefXD*V[2];
            if(si) pList[i]->mass = SUN_MASS_KG/pList[i]->mass;

//            if(plaNum==10) pList[i]->mass = mass[9];
//            else pList[i]->mass = mass[plaNum];

//            pList[i]->mass = mass[plaNum];

        /*

            switch(plaNum)
            {
            case 0:
                pList[i]->mass = gms/nbody->headParam("GM0");
                break;
            }

*/
            xyStm << QString("%1|%2|%3|%4|%5|%6|%7|%8|%9\n").arg(name).arg(X[0], 26, 'e', 20).arg(X[1], 26, 'e', 20).arg(X[2], 26, 'e', 20).arg(dist, 26, 'e', 20).arg(V[0], 26, 'e', 20).arg(V[1], 26, 'e', 20).arg(V[2], 26, 'e', 20).arg(vel, 26, 'e', 20);
        }



        //Particle *p = new Particle;

        //p->fillFromExistingParticleStruct(*pList[i]);

        //experiment->addToPopulationSet(*p);
    }

    xyFile.close();

    saveCFG("test.xml", pList);
    //experiment->exportParticlesToXMLFile("test.xml");
    return 0;//a.exec();
}
/*
int readCFG(QString fileName, QList <ParticleStruct*> &pList)
{
    QString errorStr;
    int errorLine;
    int errorColumn;
    QString objName;
    QDomDocument domDocument;
    ParticleStruct *p;

    QFile cfgFile(fileName);
    if(!cfgFile.open(QIODevice::ReadOnly | QFile::Text))
    {
        qDebug() << QString("can't open cfg file\n");
        return 1;
    }

    if(!domDocument.setContent(&cfgFile, true, &errorStr, &errorLine,
                                     &errorColumn)) {
        qDebug() << QString("error setContent\n");
        return 1;
    }

    QDomElement root = domDocument.documentElement();
         if (root.hasAttribute("version")
                    && root.attribute("version") != "1.0") {
             qDebug() << "wrong version\n";
             return false;
         }

     std::string srep;
     std::string irep;

     QDomElement child = root.firstChildElement("particle");
          while (!child.isNull()) {
              p = new ParticleStruct;
              p->name = child.firstChildElement("name").text().toAscii().data();
              p->mass = child.firstChildElement("mass").text().toDouble();
              p->radius = child.firstChildElement("radius").text().toDouble();
              p->visualRepresentation = child.firstChildElement("visualSize").text().toInt();
              //p->interactionPermission = child.firstChildElement("interactionPermission").text().toAscii().data();
              irep = child.firstChildElement("interactionPermission").text().toAscii().data();
              if (irep == "interactALL") {
                  p->interactionPermission = Advisor::interactALL;
              }
              if (irep == "interactNONE") {
                  p->interactionPermission = Advisor::interactNONE;
              }
              if (irep == "interactEnvironmentOnly") {
                  p->interactionPermission = Advisor::interactEnvironmentOnly;
              }
              if (irep == "interactDifferentOnly") {
                  p->interactionPermission = Advisor::interactDifferentOnly;
              }
              //setting that determines what class of particle this is, which effects its interactions.
              srep = child.firstChildElement("identity").text().toAscii().data();
              if (srep == "collapsor") {
                  p->identity = Advisor::collapsor;
              }
              if (srep == "collapsorFixed") {
                  p->identity = Advisor::collapsorFixed;
              }
              if (srep == "nonInteractive") {
                  p->identity = Advisor::nonInteractive;
              }
              if (srep == "ordinary") {
                  p->identity = Advisor::ordinary;
              }
              if (srep == "chromosome") {
                  p->identity = Advisor::chromosome;
              }
              if (srep == "planetesimal") {
                  p->identity = Advisor::planetesimal;
              }
              qDebug() << QString("%1\n").arg(p->name.data());
              pList << p;
              child = child.nextSiblingElement("particle");
          }

    cfgFile.close();
    return 0;
}

int saveCFG(QString fileName, QList <ParticleStruct*> &pList)
{
    QString errorStr;
    int errorLine;
    int errorColumn;
    QString objName;
    QDomDocument domDocument;
    ParticleStruct *p;

    QFile cfgFile(fileName);
    cfgFile.open(QIODevice::ReadWrite | QFile::Truncate);
    QTextStream cfgStm(&cfgFile);

/*
    if(!domDocument.setContent(&cfgFile)) {
        qDebug() << "error setContent\n";
        return 1;
    }

/
    domDocument.createElement("root");

    cfgStm << "<?xml version=\"1.0\"?><root xsi:noNamespaceSchemaLocation = \"EnvironmentSet.xsd\" xmlns:xsi = \"http://www.w3.org/2001/XMLSchema-instance\">" << "\n";


     std::string srep;
     std::string irep;

     QDomElement child, vector, partDom;// = root.firstChildElement("particle");
    QDomText childText;
     int i, sz;
     sz = pList.size();
     for(i=0; i<sz; i++)
     {
  //       cfgStm << "<particles>\n";


   //      cfgStm << "<\particles>\n";

         //partDom.setTagName("particle");
         partDom = domDocument.createElement("particle");
         //partDom

         p = pList[i];

         child = domDocument.createElement("interactionPermission");
         switch(p->interactionPermission)
         {
             case Advisor::interactALL:
                 irep = "interactALL";
                 break;
             case Advisor::interactNONE:
                 irep = "interactNONE";
                 break;
             case Advisor::interactEnvironmentOnly:
                 irep = "interactEnvironmentOnly";
                 break;
             case Advisor::interactDifferentOnly:
                 irep = "interactDifferentOnly";
                 break;

         }
         childText = domDocument.createTextNode(QString(irep.data()));
         child.appendChild(childText);
         partDom.appendChild(child);


         switch(p->identity)
         {
             case Advisor::collapsor:
                 srep = "collapsor";
                 break;
             case Advisor::collapsorFixed:
                 srep = "collapsorFixed";
                 break;
             case Advisor::nonInteractive:
                 srep = "nonInteractive";
                 break;
             case Advisor::ordinary:
                 srep = "ordinary";
                 break;
             case Advisor::chromosome:
                 srep = "chromosome";
                 break;
             case Advisor::planetesimal:
                 srep = "planetesimal";
                 break;
         }
         child = domDocument.createElement("identity");
         childText = domDocument.createTextNode(QString(srep.data()));
         child.appendChild(childText);
         partDom.appendChild(child);

         child = domDocument.createElement("name");
         childText = domDocument.createTextNode(QString(p->name.data()));
         child.appendChild(childText);
         partDom.appendChild(child);

         child = domDocument.createElement("mass");
         childText = domDocument.createTextNode(QString("%1").arg(p->mass));
         child.appendChild(childText);
         partDom.appendChild(child);

         child = domDocument.createElement("radius");
         childText = domDocument.createTextNode(QString("%1").arg(p->radius));
         child.appendChild(childText);
         partDom.appendChild(child);


         child = domDocument.createElement("visualSize");
         childText = domDocument.createTextNode(QString("%1").arg(p->visualRepresentation));
         child.appendChild(childText);
         partDom.appendChild(child);

         vector = domDocument.createElement("vector");

         child = domDocument.createElement("X");
         childText = domDocument.createTextNode(QString("%1").arg(p->x));
         child.appendChild(childText);
         vector.appendChild(child);

         child = domDocument.createElement("Y");
         childText = domDocument.createTextNode(QString("%1").arg(p->y));
         child.appendChild(childText);
         vector.appendChild(child);

         child = domDocument.createElement("Z");
         childText = domDocument.createTextNode(QString("%1").arg(p->z));
         child.appendChild(childText);
         vector.appendChild(child);

         child = domDocument.createElement("XD");
         childText = domDocument.createTextNode(QString("%1").arg(p->xd));
         child.appendChild(childText);
         vector.appendChild(child);

         child = domDocument.createElement("YD");
         childText = domDocument.createTextNode(QString("%1").arg(p->yd));
         child.appendChild(childText);
         vector.appendChild(child);

         child = domDocument.createElement("ZD");
         childText = domDocument.createTextNode(QString("%1").arg(p->zd));
         child.appendChild(childText);
         vector.appendChild(child);

         partDom.appendChild(vector);


         domDocument.appendChild(partDom);

         }

//

     QString xml = domDocument.toString();
     //qDebug() << "xml:" << xml << "\n";

     cfgStm << xml << "\n";
     cfgStm << "</root>" << "\n";

    cfgFile.close();
}
*/
