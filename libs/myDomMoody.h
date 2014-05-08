#ifndef MYDOMMOODY_H
#define MYDOMMOODY_H

#include "moody/capsule/capsuleBase/particle/Particle.h"

#include <QDomDocument>

struct cfgStruct
{
    double gConst;
    int states, steps, stepSize;
};

int readParticles(QString fileName, QList <ParticleStruct*> &pList, int *massType=NULL);

int readParticles(QString fileName, QList <ParticleStruct*> &pList, int *massType)
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

     QDomElement child = root.firstChildElement("massType");
     if(massType!=NULL)*massType = child.text().toInt();
     child = root.firstChildElement("particle");
     QDomElement vector;
     QDomElement rgb;
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

              vector = child.firstChildElement("vector");

              p->x = vector.firstChildElement("X").text().toDouble();
              p->y = vector.firstChildElement("Y").text().toDouble();
              p->z = vector.firstChildElement("Z").text().toDouble();
              p->xd = vector.firstChildElement("XD").text().toDouble();
              p->yd = vector.firstChildElement("YD").text().toDouble();
              p->zd = vector.firstChildElement("ZD").text().toDouble();

              rgb = child.firstChildElement("rgb");
              p->red = rgb.firstChildElement("red").text().toInt();
              p->green = rgb.firstChildElement("green").text().toInt();
              p->blue = rgb.firstChildElement("blue").text().toInt();

              //qDebug() << QString("%1\n").arg(p->name.data());
              pList << p;
              child = child.nextSiblingElement("particle");
          }

    cfgFile.close();
    return 0;
}



int saveParticles(QString fileName, QList <ParticleStruct*> &pList)
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

*/
    domDocument.createElement("root");

    cfgStm << "<?xml version=\"1.0\" encoding=\"UTF-8\"?><root xsi:noNamespaceSchemaLocation = \"EnvironmentSet.xsd\" xmlns:xsi = \"http://www.w3.org/2001/XMLSchema-instance\">" << "\n";


     std::string srep;
     std::string irep;

     QDomElement child, vector, rgb, partDom;// = root.firstChildElement("particle");
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
         childText = domDocument.createTextNode(QString("%1").arg(p->mass, 20, 'e', 12));
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
         childText = domDocument.createTextNode(QString("%1").arg(p->x, 26, 'e', 20));
         child.appendChild(childText);
         vector.appendChild(child);

         child = domDocument.createElement("Y");
         childText = domDocument.createTextNode(QString("%1").arg(p->y, 26, 'e', 20));
         child.appendChild(childText);
         vector.appendChild(child);

         child = domDocument.createElement("Z");
         childText = domDocument.createTextNode(QString("%1").arg(p->z, 26, 'e', 20));
         child.appendChild(childText);
         vector.appendChild(child);

         child = domDocument.createElement("XD");
         childText = domDocument.createTextNode(QString("%1").arg(p->xd, 26, 'e', 20));
         child.appendChild(childText);
         vector.appendChild(child);

         child = domDocument.createElement("YD");
         childText = domDocument.createTextNode(QString("%1").arg(p->yd, 26, 'e', 20));
         child.appendChild(childText);
         vector.appendChild(child);

         child = domDocument.createElement("ZD");
         childText = domDocument.createTextNode(QString("%1").arg(p->zd, 26, 'e', 20));
         child.appendChild(childText);
         vector.appendChild(child);

         partDom.appendChild(vector);


         rgb = domDocument.createElement("rgb");

         child = domDocument.createElement("red");
         childText = domDocument.createTextNode(QString("%1").arg(p->red));
         child.appendChild(childText);
         rgb.appendChild(child);

         child = domDocument.createElement("green");
         childText = domDocument.createTextNode(QString("%1").arg(p->green));
         child.appendChild(childText);
         rgb.appendChild(child);

         child = domDocument.createElement("blue");
         childText = domDocument.createTextNode(QString("%1").arg(p->blue));
         child.appendChild(childText);
         rgb.appendChild(child);

         partDom.appendChild(rgb);


         domDocument.appendChild(partDom);

         }

//

     QString xml = domDocument.toString();
     //qDebug() << "xml:" << xml << "\n";

     cfgStm << xml << "\n";
     cfgStm << "</root>" << "\n";

    cfgFile.close();
}

int saveCFG(QString fileName, cfgStruct& cfgPar)
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

*/
    domDocument.createElement("root");

    cfgStm << "<?xml version=\"1.0\" encoding=\"UTF-8\"?><root xsi:noNamespaceSchemaLocation = \"EnvironmentSet.xsd\" xmlns:xsi = \"http://www.w3.org/2001/XMLSchema-instance\">" << "\n";


     std::string srep;
     std::string irep;

     QDomElement child, vector, rgb, partDom;// = root.firstChildElement("particle");
    QDomText childText;

    //partDom = domDocument.createElement("G");
    child = domDocument.createElement("G");
    childText = domDocument.createTextNode(QString("%1").arg(cfgPar.gConst, 20, 'e', 12));
    child.appendChild(childText);
    domDocument.appendChild(child);

    child = domDocument.createElement("states");
    childText = domDocument.createTextNode(QString("%1").arg(cfgPar.states));
    child.appendChild(childText);
    domDocument.appendChild(child);

    child = domDocument.createElement("steps");
    childText = domDocument.createTextNode(QString("%1").arg(cfgPar.steps));
    child.appendChild(childText);
    domDocument.appendChild(child);

    child = domDocument.createElement("stepSize");
    childText = domDocument.createTextNode(QString("%1").arg(cfgPar.stepSize));
    child.appendChild(childText);
    domDocument.appendChild(child);




     QString xml = domDocument.toString();
     //qDebug() << "xml:" << xml << "\n";

     cfgStm << xml << "\n";
     cfgStm << "<version>1.0</version>" << "\n";
     cfgStm << "<MopName>moodyProject</MopName>" << "\n";
     cfgStm << "</root>" << "\n";

    cfgFile.close();
}


#endif // MYDOMMOODY_H
