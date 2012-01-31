#include <QtCore/QCoreApplication>
#include "./../libs/redStat.h"
//#include "./../libs/mpcfile.h"
#include "./../libs/astro.h"
#include "./../libs/mpccat.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC, "C");

    eqFile eqFile;
    eqFile.init(argv[1]);
    int i, szi;

    ocRec *ocRecord;

    mpccat mpoCat;
    QString objName, objNum, tStr;

    //mpcs mpFile;
    QString resFileName = QString(argv[2]);

    QString cfgFileName = "eq2mpc.ini";
    QSettings *sett = new QSettings(cfgFileName, QSettings::IniFormat);
    QString mpcOrbCat = sett->value("general/mpcOrbCat", "mpcorb.dat").toString();
    QString obsCode = sett->value("general/obsCode", "084").toString();

    if(mpoCat.init(mpcOrbCat.toAscii().data())) qDebug() << QString("mpoCat init error!\n");

    QFile resFile(resFileName);
    if(!resFile.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        qDebug() << QString("File %1 not open. stop\n").arg(resFileName);
        return 1;
    }
    QTextStream resStm(&resFile);

    char *tstr = new char[256];

    szi = eqFile.ocList.size();

    for(i=0;i<szi;i++)
    {
        ocRecord = eqFile.ocList.at(i);

        objName = ocRecord->name.simplified();
        //catName = objStat.objList.at(i)->catName.simplified();
        //catMagName = objStat.objList.at(i)->catMagName.simplified();

        mpoCat.GetRecName(objName.toAscii().data());
        mpoCat.record->getNumStr(tstr);
        objNum = QString(tstr);


        ocRecord->rec2MPC(&tStr, obsCode, objNum);

        resStm << tStr << "\n";

    }


    resFile.close();


    return 0;//a.exec();
}
